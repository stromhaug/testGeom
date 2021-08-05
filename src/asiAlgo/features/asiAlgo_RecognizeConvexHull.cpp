//-----------------------------------------------------------------------------
// Created on: 03 August 2021
//-----------------------------------------------------------------------------
// Copyright (c) 2021-present, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_RecognizeConvexHull.h>

// Analysis Situs includes
#include <asiAlgo_AAG.h>
#include <asiAlgo_BuildConvexHull.h>
#include <asiAlgo_BVHFacets.h>
#include <asiAlgo_FeatureAttrConvexHull.h>
#include <asiAlgo_ProjectPointOnMesh.h>
#include <asiAlgo_SampleFace.h>

// OpenCascade includes
#include <BRepAdaptor_Surface.hxx>
#include <TopExp_Explorer.hxx>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

namespace
{
  bool IsPlanar(const TopoDS_Face& face,
                const double       angTolRad)
  {
    if ( asiAlgo_Utils::IsPlanar(face) )
      return true;

    std::vector<gp_Dir> norms;

    // Get triangulation.
    TopLoc_Location loc;
    Handle(Poly_Triangulation) tris = BRep_Tool::Triangulation(face, loc);

    // Iterate over triangles.
    const Poly_Array1OfTriangle& triangles = tris->Triangles();
    const TColgp_Array1OfPnt&    nodes     = tris->Nodes();
    //
    for ( int elemId = triangles.Lower(); elemId <= triangles.Upper(); ++elemId )
    {
      const Poly_Triangle& tri = triangles(elemId);

      int n1, n2, n3;
      tri.Get(n1, n2, n3);

      gp_Pnt P0 = nodes(n1);
      P0.Transform(loc);
      //
      gp_Pnt P1 = nodes(n2);
      P1.Transform(loc);
      //
      gp_Pnt P2 = nodes(n3);
      P2.Transform(loc);

      /* Initialize normal */

      gp_Vec V1(P0, P1);
      //
      if ( V1.SquareMagnitude() < 1e-8 )
        continue; // Skip invalid facet.

      gp_Vec V2(P0, P2);
      //
      if ( V2.SquareMagnitude() < 1e-8 )
        continue; // Skip invalid facet.

      // Compute norm
      gp_Vec normVec = V1.Crossed(V2);
      //
      if ( normVec.SquareMagnitude() < 1e-8 )
        continue; // Skip invalid facet

      norms.push_back(normVec);
    }

    if ( norms.empty() )
      return false;

    if ( norms.size() == 1 )
      return true;

    const gp_Dir& refNorm = norms[0];

    for ( size_t i = 1; i < norms.size(); ++i )
    {
      const double ang = norms[i].Angle(refNorm);
      //
      if ( Abs(ang) > angTolRad )
        return false;
    }

    return true;
  }
}

//-----------------------------------------------------------------------------

asiAlgo_RecognizeConvexHull::asiAlgo_RecognizeConvexHull(const TopoDS_Shape&  shape,
                                                         ActAPI_ProgressEntry progress,
                                                         ActAPI_PlotterEntry  plotter)
//
: asiAlgo_Recognizer ( shape, nullptr, progress, plotter ),
  m_iGridPts         ( 20 ),
  m_fToler           ( 0.1 ), // mm
  m_bHaines          ( true )
{}

//-----------------------------------------------------------------------------

asiAlgo_RecognizeConvexHull::asiAlgo_RecognizeConvexHull(const Handle(asiAlgo_AAG)& aag,
                                                         ActAPI_ProgressEntry       progress,
                                                         ActAPI_PlotterEntry        plotter)
//
: asiAlgo_Recognizer ( aag, progress, plotter ),
  m_iGridPts         ( 20 ),
  m_fToler           ( 0.1 ), // mm
  m_bHaines          ( true )
{}

//-----------------------------------------------------------------------------

void asiAlgo_RecognizeConvexHull::SetGridResolution(const int numSlices)
{
  m_iGridPts = numSlices;
}

//-----------------------------------------------------------------------------

int asiAlgo_RecognizeConvexHull::GetGridResolution() const
{
  return m_iGridPts;
}

//-----------------------------------------------------------------------------

void asiAlgo_RecognizeConvexHull::SetTolerance(const double tol)
{
  m_fToler = tol;
}

//-----------------------------------------------------------------------------

double asiAlgo_RecognizeConvexHull::GetTolerance() const
{
  return m_fToler;
}

//-----------------------------------------------------------------------------

void asiAlgo_RecognizeConvexHull::SetUseHaines(const bool on)
{
  m_bHaines = on;
}

//-----------------------------------------------------------------------------

bool asiAlgo_RecognizeConvexHull::GetUseHaines() const
{
  return m_bHaines;
}

//-----------------------------------------------------------------------------

bool asiAlgo_RecognizeConvexHull::Perform()
{
  // Clean up the result.
  m_result.faces.Clear();
  m_result.ids.Clear();

  const bool toDraw = !m_plotter.Access().IsNull();

  /* =============
   *  Prepare AAG.
   * ============= */

  // Build AAG if not available.
  if ( m_aag.IsNull() )
  {
#if defined COUT_DEBUG
    TIMER_NEW
    TIMER_GO
#endif

    m_aag = new asiAlgo_AAG(m_master, false);

#if defined COUT_DEBUG
    TIMER_FINISH
    TIMER_COUT_RESULT_MSG("Construct AAG")
#endif
  }

  /* ===================
   *  Build convex hull.
   * =================== */

  const TopoDS_Shape& shape = m_aag->GetMasterShape();

#if defined COUT_DEBUG
  TIMER_NEW
  TIMER_GO
#endif

  Handle(Poly_Triangulation) hull;

  asiAlgo_BuildConvexHull buildHull(m_progress, m_plotter);
  //
  if ( !buildHull.Perform(shape, hull) )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Failed to build convex hull.");
    return false;
  }

#if defined COUT_DEBUG
  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(m_progress, "Build convex hull")
#endif

  /* ===================================================
   *  Sample faces and project their points to the hull.
   * =================================================== */

  asiAlgo_Feature convexHullFaces;

  // Build BVH for the convex hull.
  Handle(asiAlgo_BVHFacets) hullBvh = new asiAlgo_BVHFacets(hull);
  //
  if ( toDraw )
    hullBvh->Dump(m_plotter);

  // Projection algorithm.
  asiAlgo_ProjectPointOnMesh pointOnMesh(hullBvh);

#if defined COUT_DEBUG
  TIMER_RESET
  TIMER_GO
#endif

  Handle(asiAlgo_BaseCloud<double>) projPts3d = new asiAlgo_BaseCloud<double>;

  int numPointsProcessed = 0;

  // Get the faces to iterate over.
  const TopTools_IndexedMapOfShape& allFaces = m_aag->GetMapOfFaces();
  //
  for ( int f = 1; f <= allFaces.Extent(); ++f )
  {
    const TopoDS_Face& face = TopoDS::Face( allFaces(f) );

    // Sample face in its UV domain.
    asiAlgo_SampleFace sampleFace(face);
    //
    sampleFace.SetUseHaines (m_bHaines);
    sampleFace.SetSquare    (true);
    //
    if ( !sampleFace.Perform(m_iGridPts) )
      continue;

    Handle(asiAlgo_BaseCloud<double>) pts3d = sampleFace.GetResult3d();

    // In the case of undersampling, we do not want to make the overlay grid
    // finer (it's bad for performance). We rather fall back to vertices on
    // a face and use them as the probe points.
    int numProbes = pts3d->GetNumberOfElements();
    //
    if ( !numProbes && ::IsPlanar(face, 5.*M_PI/180.) )
    {
      TopTools_IndexedMapOfShape verts;
      TopExp::MapShapes(face, TopAbs_VERTEX, verts);
      //
      for ( int vidx = 1; vidx <= verts.Extent(); ++vidx )
      {
        pts3d->AddElement( BRep_Tool::Pnt( TopoDS::Vertex( verts(vidx) ) ) );
        numProbes++;
      }
    }

    numPointsProcessed += numProbes;

    if ( toDraw )
    {
      TCollection_AsciiString ptsName("probes_"); ptsName += f;
      TCollection_AsciiString faceName("face_"); faceName += f;

      m_plotter.DRAW_SHAPE(face, Color_Blue, faceName);
      m_plotter.DRAW_POINTS(pts3d->GetCoordsArray(), Color_Red, ptsName);
    }

    // Project points.
    int numOk = 0;
    //
    for ( int p = 0; p < numProbes; ++p )
    {
      gp_Pnt P      = pts3d->GetElement(p);
      gp_Pnt P_proj = pointOnMesh.Perform(P);

      const double dist = P.Distance(P_proj);

      if ( dist < m_fToler )
      {
        numOk++;
        if ( toDraw )
          projPts3d->AddElement(P_proj);
      }
    }

    if ( numOk > numProbes / 2 )
      convexHullFaces.Add(f);
  }

#if defined COUT_DEBUG
  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Convex-hull face recognition")
#endif

  if ( toDraw )
  {
    m_plotter.REDRAW_POINTS("proj", projPts3d->GetCoordsArray(), Color_Violet);
    m_plotter.REDRAW_TRIANGULATION("hull", hull, Color_Default, 1.0);
  }

  m_progress.SendLogMessage(LogInfo(Normal) << "Total number of points processed: %1."
                                            << numPointsProcessed);

  /* ============================
   *  Settle down AAG attributes.
   * ============================ */

  m_result.ids = convexHullFaces;

  for ( asiAlgo_Feature::Iterator fit(m_result.ids); fit.More(); fit.Next() )
  {
    const int fid = fit.Key();

    Handle(asiAlgo_FeatureAttrConvexHull)
      chAttr = new asiAlgo_FeatureAttrConvexHull;
    //
    m_aag->SetNodeAttribute(fid, chAttr);
  }

  return true; // Success.
}

//-----------------------------------------------------------------------------

void asiAlgo_RecognizeConvexHull::sampleSpecialCases(const TopoDS_Face&                       face,
                                                     const Handle(asiAlgo_BaseCloud<double>)& pts)
{
  double r = 0., angMin = 0., angMax = 0., hMin = 0., hMax = 0.;
  gp_Ax1 ax;

  BRepAdaptor_Surface bas(face);

  TopTools_IndexedMapOfShape verts;
  TopExp::MapShapes(face, TopAbs_VERTEX, verts);

  /* Planar face */
  if ( ::IsPlanar(face, 5.*M_PI/180.) )
  {
    // Add vertices.
    for ( int vidx = 1; vidx <= verts.Extent(); ++vidx )
    {
      pts->AddElement( BRep_Tool::Pnt( TopoDS::Vertex( verts(vidx) ) ) );
    }
  }

  /* Cylindrical face */
  else if ( asiAlgo_Utils::IsCylindrical(face, r, ax, true, angMin, angMax, hMin, hMax) )
  {
    // Add vertices.
    for ( int vidx = 1; vidx <= verts.Extent(); ++vidx )
    {
      pts->AddElement( BRep_Tool::Pnt( TopoDS::Vertex( verts(vidx) ) ) );
    }

    // Add midpoint.
    gp_Pnt2d midPt( (angMin + angMax)*0.5, (hMin + hMax)*0.5 );
    pts->AddElement(midPt);
  }
}
