//-----------------------------------------------------------------------------
// Created on: 23 August 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
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
//    * Neither the name of Sergey Slyadnev nor the
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

// cmdMisc includes
#include <cmdMisc.h>

// asiAlgo includes
#include <asiAlgo_BaseCloud.h>
#include <asiAlgo_PlaneOnPoints.h>
#include <asiAlgo_Timer.h>
#include <asiAlgo_Utils.h>

// asiUI includes
#include <asiUI_CommonFacilities.h>
#include <asiUI_IV.h>

// asiTcl includes
#include <asiTcl_PluginMacro.h>

// asiEngine includes
#include <asiEngine_Model.h>
#include <asiEngine_Part.h>

// OCCT includes
#include <BOPAlgo_Splitter.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepOffsetAPI_MakePipeShell.hxx>
#include <BRepOffsetAPI_MakeThickSolid.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepTools.hxx>
#include <GCPnts_QuasiUniformAbscissa.hxx>
#include <GCPnts_TangentialDeflection.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <GeomAPI.hxx>
#include <gp_Circ.hxx>
#include <gp_Pln.hxx>
#include <IntTools_FClass2d.hxx>
#include <ShapeUpgrade_UnifySameDomain.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#ifdef USE_MOBIUS
  #include <mobius/bspl_FindSpan.h>
  #include <mobius/bspl_UnifyKnots.h>
  #include <mobius/cascade.h>
  #include <mobius/core_HeapAlloc.h>
  #include <mobius/geom_BSplineCurve.h>
  #include <mobius/geom_CoonsSurface.h>
  #include <mobius/geom_FairBCurve.h>
  #include <mobius/geom_FairBSurf.h>
  #include <mobius/geom_InterpolateMultiCurve.h>
  #include <mobius/geom_SkinSurface.h>
#endif

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #pragma message("===== warning: DRAW_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

double SignedDistance(const gp_Pln& pln, const gp_Pnt& P)
{
  const gp_Pnt& loc = pln.Position().Location();
  const gp_Dir& dir = pln.Position().Direction();
  double D = ( dir.X() * ( P.X() - loc.X() ) +
               dir.Y() * ( P.Y() - loc.Y() ) +
               dir.Z() * ( P.Z() - loc.Z() ) );
  return D;
}

//-----------------------------------------------------------------------------

int MISC_TestHexagonBops(const Handle(asiTcl_Interp)& interp,
                         int                          /*argc*/,
                         const char**                 /*argv*/)
{
  const double r = 0.02;
  const double h = 0.25;
  const double step = 0.08;

  /* =======================
   *  Build hexagonal prism
   * ======================= */

  TIMER_NEW
  TIMER_GO

  // Get hexagon poles in 2D
  gp_XY Poles2d[6];
  asiAlgo_Utils::HexagonPoles(gp::Origin2d().XY(),
                              1.0,
                              Poles2d[0],
                              Poles2d[1],
                              Poles2d[2],
                              Poles2d[3],
                              Poles2d[4],
                              Poles2d[5]);

  // Choose working plane
  Handle(Geom_Plane) workPlane = new Geom_Plane( gp_Pln( gp::Origin(), gp::DZ() ) );

  // Populate wire builder
  BRepBuilderAPI_MakePolygon mkPoly;
  for ( int k = 0; k < 6; ++k )
    mkPoly.Add( workPlane->Value( Poles2d[k].X(), Poles2d[k].Y() ) );
  //
  mkPoly.Add( workPlane->Value( Poles2d[0].X(), Poles2d[0].Y() ) );

  // Build wire
  mkPoly.Build();
  const TopoDS_Wire& wire = mkPoly.Wire();

  // Build face
  TopoDS_Face face = BRepBuilderAPI_MakeFace(wire);

  // Build prism
  TopoDS_Shape prism = BRepPrimAPI_MakePrism(face, gp_Vec( workPlane->Axis().Direction() )*h);

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Build hexagonal solid")

  interp->GetPlotter().REDRAW_SHAPE("contour", wire, Color_Red, 1.0, true);
  interp->GetPlotter().REDRAW_SHAPE("face", face);
  interp->GetPlotter().REDRAW_SHAPE("prism", prism);

  /* =================
   *  Build cylinders
   * ================= */

  TIMER_RESET
  TIMER_GO

  // Build bounding box for the hexa
  double xMin, yMin, zMin, xMax, yMax, zMax;
  asiAlgo_Utils::Bounds(prism, xMin, yMin, zMin, xMax, yMax, zMax);

  BRep_Builder BB;
  TopoDS_Compound cyls;
  BB.MakeCompound(cyls);

  // List for cutter
  TopTools_ListOfShape tools;

  // Prepare two-dimensional classifier
  IntTools_FClass2d classifier;
  classifier.Init( face, Precision::Confusion() );

  // Count number of primitives which pass the test
  int cylCount = 0;

  double xcurr = xMin;
  do
  {
    double ycurr = yMin;

    do
    {
      // Classify center point
      const TopAbs_State state = classifier.TestOnRestriction( gp_Pnt2d(xcurr, ycurr), 0.05 );
      //
      if ( state == TopAbs_IN )
      {
        gp_Pnt cylcenter(xcurr, ycurr, 0.0);
        TopoDS_Shape cyl = BRepPrimAPI_MakeCylinder( gp_Ax2(cylcenter, gp::DZ() ), r, h );
        //
        BB.Add(cyls, cyl);
        tools.Append(cyl);

        cylCount++;
      }

      ycurr += step;
    }
    while ( ycurr < yMax );

    xcurr += step;
  }
  while ( xcurr < xMax );

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Build cylinders")

  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Number of cylinders: %1" << cylCount);
  interp->GetPlotter().REDRAW_SHAPE("cyls", cyls, Color_Yellow);

  /* ================
   *  Cut primitives
   * ================ */

  TIMER_RESET
  TIMER_GO

  TopoDS_Shape result = asiAlgo_Utils::BooleanCut(prism, tools, false);

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Cut cylinders")

  interp->GetPlotter().REDRAW_SHAPE("result", result);

  return 0;
}

//-----------------------------------------------------------------------------

int MISC_TestHexagonBopsFaces(const Handle(asiTcl_Interp)& interp,
                              int                          /*argc*/,
                              const char**                 /*argv*/)
{
  const double r = 0.02;
  const double h = 0.25;
  const double step = 0.06;

  /* ===============
   *  Build hexagon
   * =============== */

  TIMER_NEW
  TIMER_GO

  // Get hexagon poles in 2D
  gp_XY Poles2d[6];
  asiAlgo_Utils::HexagonPoles(gp::Origin2d().XY(),
                              1.0,
                              Poles2d[0],
                              Poles2d[1],
                              Poles2d[2],
                              Poles2d[3],
                              Poles2d[4],
                              Poles2d[5]);

  // Choose working plane
  Handle(Geom_Plane) workPlane = new Geom_Plane( gp_Pln( gp::Origin(), gp::DZ() ) );

  // Populate wire builder
  BRepBuilderAPI_MakePolygon mkPoly;
  for ( int k = 0; k < 6; ++k )
    mkPoly.Add( workPlane->Value( Poles2d[k].X(), Poles2d[k].Y() ) );
  //
  mkPoly.Add( workPlane->Value( Poles2d[0].X(), Poles2d[0].Y() ) );

  // Build wire
  mkPoly.Build();
  const TopoDS_Wire& wire = mkPoly.Wire();

  // Build face
  TopoDS_Face face = BRepBuilderAPI_MakeFace(wire);

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Build hexagon")

  interp->GetPlotter().REDRAW_SHAPE("contour", wire, Color_Red, 1.0, true);
  interp->GetPlotter().REDRAW_SHAPE("face", face);

  /* ===============
   *  Build circles
   * =============== */

  TIMER_RESET
  TIMER_GO

  // Build bounding box for the hexa
  double xMin, yMin, zMin, xMax, yMax, zMax;
  asiAlgo_Utils::Bounds(face, xMin, yMin, zMin, xMax, yMax, zMax);

  BRep_Builder BB;
  TopoDS_Compound circles;
  BB.MakeCompound(circles);

  // List for cutter
  TopTools_ListOfShape tools;

  // Prepare two-dimensional classifier
  IntTools_FClass2d classifier;
  classifier.Init( face, Precision::Confusion() );

  // Count number of primitives which pass the test
  int circCount = 0;

  double xcurr = xMin;
  do
  {
    double ycurr = yMin;

    do
    {
      // Classify center point
      const TopAbs_State state = classifier.TestOnRestriction( gp_Pnt2d(xcurr, ycurr), 0.05 );
      //
      if ( state == TopAbs_IN )
      {
        gp_Pnt       circcenter(xcurr, ycurr, 0.0);
        gp_Circ      circ(gp_Ax2(circcenter, gp::DZ() ), r);
        TopoDS_Wire  circWire = BRepBuilderAPI_MakeWire( BRepBuilderAPI_MakeEdge(circ) );
        TopoDS_Shape circFace = BRepBuilderAPI_MakeFace(circWire);
        //
        BB.Add(circles, circFace);
        tools.Append(circFace);

        circCount++;
      }

      ycurr += step;
    }
    while ( ycurr < yMax );

    xcurr += step;
  }
  while ( xcurr < xMax );

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Build circles")

  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Number of circles: %1" << circCount);
  interp->GetPlotter().REDRAW_SHAPE("circles", circles, Color_Yellow);

  /* ================
   *  Cut primitives
   * ================ */

  TIMER_RESET
  TIMER_GO

  TopoDS_Shape cutres = asiAlgo_Utils::BooleanCut(face, tools, false);

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Cut circles")

  interp->GetPlotter().REDRAW_SHAPE("cutres", cutres);

  /* =========
   *  Extrude
   * ========= */

  TIMER_RESET
  TIMER_GO

  // Build prism
  TopoDS_Shape result = BRepPrimAPI_MakePrism(cutres, gp_Vec( workPlane->Axis().Direction() )*h);

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Extrude")

  interp->GetPlotter().REDRAW_SHAPE("result", result);

  return 0;
}

//-----------------------------------------------------------------------------

int MISC_PushPull(const Handle(asiTcl_Interp)& interp,
                  int                          /*argc*/,
                  const char**                 argv)
{
  TopoDS_Shape
    partShape = Handle(asiEngine_Model)::DownCast( interp->GetModel() )->GetPartNode()->GetShape();

  int faceId = atoi(argv[1]); // 1-based

  TopTools_IndexedMapOfShape M;
  TopExp::MapShapes(partShape, TopAbs_FACE, M);
  TopoDS_Face faceShape = TopoDS::Face( M.FindKey(faceId) );

  interp->GetPlotter().DRAW_SHAPE(faceShape, Color_Blue, 1.0, true, "faceShape");

  Handle(Geom_Surface) surf = BRep_Tool::Surface(faceShape);

  double uMin, uMax, vMin, vMax;
  BRepTools::UVBounds(faceShape, uMin, uMax, vMin, vMax);

  double uMid = (uMin + uMax)*0.5;
  double vMid = (vMin + vMax)*0.5;

  gp_Pnt P;
  gp_Vec dS_dU, dS_dV;
  surf->D1(uMid, vMid, P, dS_dU, dS_dV);

  gp_Dir norm = dS_dU^dS_dV;

  if ( faceShape.Orientation() == TopAbs_REVERSED )
    norm.Reverse();

  gp_Vec offset = norm.XYZ() * atof(argv[2]);

  TopoDS_Shape prism = BRepPrimAPI_MakePrism(faceShape, offset);
  interp->GetPlotter().REDRAW_SHAPE("prism", prism, Color_Blue);

  TopoDS_Shape fused = BRepAlgoAPI_Fuse(prism, partShape);

  ShapeUpgrade_UnifySameDomain Maximize(fused);
  Maximize.Build();
  fused = Maximize.Shape();

  interp->GetPlotter().REDRAW_SHAPE("fused", fused, Color_Green);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int MISC_TestExtend(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc != 4 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  /* ================
   *  Process inputs
   * ================ */

  TopoDS_Shape
    partShape = Handle(asiEngine_Model)::DownCast( interp->GetModel() )->GetPartNode()->GetShape();

  enum t_dim
  {
    dim_X, dim_Y, dim_Z
  };

  enum t_dir
  {
    dir_up, dir_down, dir_both
  };

  // Take dimension of interest
  t_dim dim;
  TCollection_AsciiString dimStr(argv[1]);
  //
  if ( dimStr == "X" )
    dim = dim_X;
  else if ( dimStr == "Y" )
    dim = dim_Y;
  else if ( dimStr == "Z" )
    dim = dim_Z;
  else
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Unexpected dimension.");
    return TCL_OK;
  }

  // Take direction of interest
  t_dir dir;
  TCollection_AsciiString dirStr(argv[2]);
  //
  if ( dirStr == "up" )
    dir = dir_up;
  else if ( dirStr == "down" )
    dir = dir_down;
  else if ( dirStr == "both" )
    dir = dir_both;
  else
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Unexpected direction.");
    return TCL_OK;
  }

  // Take offset value
  const double offset = atof(argv[3]);

  /* =================================
   *  Build AABB for the working part
   * ================================= */

  double xMin, yMin, zMin, xMax, yMax, zMax;
  asiAlgo_Utils::Bounds(partShape, xMin, yMin, zMin, xMax, yMax, zMax);
  //
  interp->GetProgress().SendLogMessage( LogInfo(Normal) << "Bounding box:\n"
                                                           "\t X min = %1\n"
                                                           "\t Y min = %2\n"
                                                           "\t Z min = %3\n"
                                                           "\t X max = %4\n"
                                                           "\t Y max = %5\n"
                                                           "\t Z max = %6"
                                                        << xMin << yMin << zMin
                                                        << xMax << yMax << zMax );

  // Create bounding box to draw it
  TopoDS_Shape bndbox = BRepPrimAPI_MakeBox( gp_Pnt(xMin, yMin, zMin), gp_Pnt(xMax, yMax, zMax) );
  //
  interp->GetPlotter().REDRAW_SHAPE("bounding box", bndbox, Color_Yellow, 1.0, true);

  /* ======================
   *  Build section planes
   * ====================== */

  const double xMid = (xMin + xMax)*0.5;
  const double yMid = (yMin + yMax)*0.5;
  const double zMid = (zMin + zMax)*0.5;
  //
  gp_Pnt P_mid(xMid, yMid, zMid);

  // Build infinite planes for slicing
  Handle(Geom_Plane) nX_plane = new Geom_Plane( P_mid, gp::DX() );
  Handle(Geom_Plane) nY_plane = new Geom_Plane( P_mid, gp::DY() );
  Handle(Geom_Plane) nZ_plane = new Geom_Plane( P_mid, gp::DZ() );

  double maxDelta = Max( xMax - xMid, Max(yMax - yMid, zMax - zMid) );
  maxDelta += maxDelta*0.1;

  // Draw
  interp->GetPlotter().REDRAW_SURFACE("nX_plane", nX_plane, maxDelta, maxDelta, Color_Red,   0.7);
  interp->GetPlotter().REDRAW_SURFACE("nY_plane", nY_plane, maxDelta, maxDelta, Color_Green, 0.7);
  interp->GetPlotter().REDRAW_SURFACE("nZ_plane", nZ_plane, maxDelta, maxDelta, Color_Blue,  0.7);

  // Choose the working section plane
  Handle(Geom_Plane) sectionPlane;
  switch ( dim )
  {
    case dim_X: sectionPlane = nX_plane; break;
    case dim_Y: sectionPlane = nY_plane; break;
    case dim_Z: sectionPlane = nZ_plane; break;
    default: break;
  }

  TopoDS_Face sectionFace = BRepBuilderAPI_MakeFace(sectionPlane->Pln(),
                                                   -maxDelta,
                                                    maxDelta,
                                                   -maxDelta,
                                                    maxDelta);

  /* =============
   *  Split solid
   * ============= */

  // Prepare objects and tools
  TopTools_ListOfShape objects;
  objects.Append(partShape);
  //
  TopTools_ListOfShape tools;
  tools.Append(sectionFace);

  BOPAlgo_Splitter mkSplit;
  mkSplit.SetArguments(objects);
  mkSplit.SetTools(tools);
  mkSplit.Perform();
  mkSplit.SetGlue(BOPAlgo_GlueFull);
  //
  if ( mkSplit.HasErrors() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "BOP splitter failed.");
    return TCL_OK;
  }

  // Get result
  const TopoDS_Shape& resVols = mkSplit.Shape();
  //
  //interp->GetPlotter().REDRAW_SHAPE("resVols", resVols);

  // Get output solids: there should be two
  std::vector<TopoDS_Solid> solids;
  //
  for ( TopExp_Explorer exp(resVols, TopAbs_SOLID); exp.More(); exp.Next() )
  {
    solids.push_back( TopoDS::Solid( exp.Current() ) );
  }
  //
  if ( solids.size() < 2 )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "BOP splitter is expected to return several solids while returned %1."
                                                        << int ( solids.size() ) );
    return TCL_OK;
  }

  // Get image of the section face. This image is necessary to prepare the
  // tool object.
  const TopTools_ListOfShape& modifiedSectionList = mkSplit.Modified(sectionFace);
  //
  TopoDS_Compound modifiedSection;
  BRep_Builder().MakeCompound(modifiedSection);
  //
  for ( TopTools_ListIteratorOfListOfShape lit(modifiedSectionList); lit.More(); lit.Next() )
  {
    BRep_Builder().Add( modifiedSection, lit.Value() );
  }
  //
  interp->GetPlotter().REDRAW_SHAPE("modifiedSection", modifiedSection);

  /* ================================================
   *  Prepare deep copy to resolve non-manifold face
   * ================================================ */

  // This code is bad. It should be reworked to copy non-manifold face only
  for ( size_t s = 0; s < solids.size(); ++s )
    solids[s] = TopoDS::Solid( BRepBuilderAPI_Copy(solids[s]) );

  // Redraw
  //for ( size_t s = 0; s < solids.size(); ++s )
  //{
  //  interp->GetPlotter().DRAW_SHAPE(solids[s], "spart");
  //}

  /* ======================================================
   *  Attribute solids as belonging to up/down half-spaces
   * ====================================================== */

  std::vector<TopoDS_Solid> upSolids, downSolids;

  for ( size_t s = 0; s < solids.size(); ++s )
  {
    // Get center of solid
    double xMin, yMin, zMin, xMax, yMax, zMax;
    asiAlgo_Utils::Bounds(solids[s], xMin, yMin, zMin, xMax, yMax, zMax);
    //
    gp_Pnt P_mid( 0.5*(xMin + xMax), 0.5*(yMin + yMax), 0.5*(zMin + zMax) );

    // Classify wrt half-space
    if ( SignedDistance(sectionPlane->Pln(), P_mid) > 0 )
      upSolids.push_back(solids[s]);
    else
      downSolids.push_back(solids[s]);
  }

  // Redraw
  //interp->GetPlotter().REDRAW_SHAPE("upSolid", upSolid, Color_Red, 0.5);
  //interp->GetPlotter().REDRAW_SHAPE("downSolid", downSolid, Color_Blue, 0.5);

  /* ======================================
   *  Move solids wrt the passed direction
   * ====================================== */

  if ( dir == dir_up )
  {
    // Prepare transformation
    gp_Trsf T; T.SetTranslation( gp_Vec( sectionPlane->Axis().Direction() )*offset );

    for ( size_t s = 0; s < upSolids.size(); ++s )
      upSolids[s] = TopoDS::Solid( BRepBuilderAPI_Transform(upSolids[s], T, true) );
  }
  else if ( dir == dir_down )
  {
    // Prepare transformation
    gp_Trsf T; T.SetTranslation( gp_Vec( sectionPlane->Axis().Direction() ).Reversed()*offset );

    for ( size_t s = 0; s < downSolids.size(); ++s )
      downSolids[s] = TopoDS::Solid( BRepBuilderAPI_Transform(downSolids[s], T, true) );
  }
  else if ( dir == dir_both )
  {
    // Prepare transformation
    gp_Trsf T_down; T_down.SetTranslation( gp_Vec( sectionPlane->Axis().Direction() ).Reversed()*offset*0.5 );
    gp_Trsf T_up; T_up.SetTranslation( gp_Vec( sectionPlane->Axis().Direction() )*offset*0.5 );

    for ( size_t s = 0; s < downSolids.size(); ++s )
      downSolids[s] = TopoDS::Solid( BRepBuilderAPI_Transform(downSolids[s], T_down, true) );

    for ( size_t s = 0; s < upSolids.size(); ++s )
      upSolids[s] = TopoDS::Solid( BRepBuilderAPI_Transform(upSolids[s], T_up, true) );
  }

  TopoDS_Compound upSolidsComp, downSolidsComp;
  BRep_Builder().MakeCompound(upSolidsComp);
  BRep_Builder().MakeCompound(downSolidsComp);
  //
  for ( size_t s = 0; s < upSolids.size(); ++s )
    BRep_Builder().Add(upSolidsComp, upSolids[s]);
  //
  for ( size_t s = 0; s < downSolids.size(); ++s )
    BRep_Builder().Add(downSolidsComp, downSolids[s]);

  // Redraw
  {
    interp->GetPlotter().REDRAW_SHAPE("upSolid_moved",   upSolidsComp,   Color_White, 1.0, true);
    interp->GetPlotter().REDRAW_SHAPE("downSolid_moved", downSolidsComp, Color_White, 1.0, true);
  }

  /* =====================
   *  Prepare tool object
   * ===================== */

  TopoDS_Shape toolObj;

  if ( dir == dir_up )
  {
    // Prepare extrusion vector
    gp_Vec vec = gp_Vec( sectionPlane->Axis().Direction() )*offset;
    //
    toolObj = BRepPrimAPI_MakePrism(modifiedSection, vec, true);
  }
  else if ( dir == dir_down )
  {
    // Prepare extrusion vector
    gp_Vec vec = gp_Vec( sectionPlane->Axis().Direction() ).Reversed()*offset;
    //
    toolObj = BRepPrimAPI_MakePrism(modifiedSection, vec, true);
  }
  else if ( dir == dir_both )
  {
    // Prepare extrusion vector
    gp_Vec vecUp   = gp_Vec( sectionPlane->Axis().Direction() )*offset*0.5;
    gp_Vec vecDown = gp_Vec( sectionPlane->Axis().Direction() ).Reversed()*offset*0.5;
    //
    TopoDS_Shape toolUp   = BRepPrimAPI_MakePrism(modifiedSection, vecUp, true);
    TopoDS_Shape toolDown = BRepPrimAPI_MakePrism(modifiedSection, vecDown, true);

    TopoDS_Compound toolComp;
    BRep_Builder().MakeCompound(toolComp);
    BRep_Builder().Add(toolComp, toolUp);
    BRep_Builder().Add(toolComp, toolDown);
    //
    toolObj = toolComp;
  }
  //
  interp->GetPlotter().REDRAW_SHAPE("toolObj", toolObj, Color_Magenta);

  /* ===================================
   *  Fuse tool object with other parts
   * =================================== */

  // Prepare objects to fuse
  TopTools_ListOfShape objects2Fuse;
  //
  for ( TopoDS_Iterator it(upSolidsComp); it.More(); it.Next() )
  {
    objects2Fuse.Append( it.Value() );
  }
  //
  for ( TopoDS_Iterator it(downSolidsComp); it.More(); it.Next() )
  {
    objects2Fuse.Append( it.Value() );
  }
  //
  if ( toolObj.ShapeType() == TopAbs_COMPOUND )
  {
    for ( TopoDS_Iterator it(toolObj); it.More(); it.Next() )
    {
      objects2Fuse.Append( it.Value() );
    }
  }
  else
    objects2Fuse.Append(toolObj);

  TopoDS_Shape result = asiAlgo_Utils::BooleanFuse(objects2Fuse);

  asiAlgo_Utils::MaximizeFaces(result);

  interp->GetPlotter().REDRAW_SHAPE("result", result);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int MISC_TestBuilder(const Handle(asiTcl_Interp)& interp,
                     int                          argc,
                     const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  BRep_Builder BB;

  // Construct any face
  TopoDS_Face face;
  BB.MakeFace(face);
  face.Orientation(TopAbs_REVERSED);

  // Construct any wire
  TopoDS_Wire wire;
  BB.MakeWire(wire);
  wire.Orientation(TopAbs_REVERSED);

  // Add wire to face
  BB.Add(face, wire);

  for ( TopoDS_Iterator it(face, false, false); it.More(); it.Next() )
  {
    // We don't care of internal/external here
    std::cout << "Orientation of sub-shape is "
              << (it.Value().Orientation() == TopAbs_FORWARD ? "forward" : "reversed");
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int MISC_TestComposite(const Handle(asiTcl_Interp)& interp,
                       int                          argc,
                       const char**                 /*argv*/)
{
  if ( argc != 1 )
    return TCL_ERROR;

  // Build rectangular face.
  TopoDS_Face R;
  {
    gp_Pnt P1(0, 0, 0),
           P2(1, 0, 0),
           P3(1, 1, 0),
           P4(0, 1, 0);

    TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(P1, P2);
    TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(P2, P3);
    TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(P3, P4);
    TopoDS_Edge E4 = BRepBuilderAPI_MakeEdge(P4, P1);
    TopoDS_Wire W = BRepBuilderAPI_MakeWire(E1, E2, E3, E4);

    R = BRepBuilderAPI_MakeFace(W);
  }

  // Build circular face.
  TopoDS_Face C1;
  {
    gp_Ax2 localAxes = gp::XOY();
    TopoDS_Edge E = BRepBuilderAPI_MakeEdge( gp_Circ(localAxes, 0.25), 0, 2*M_PI );
    TopoDS_Wire W = BRepBuilderAPI_MakeWire(E);

    C1 = BRepBuilderAPI_MakeFace(W);
  }

  gp_Trsf T1; T1.SetTranslation( gp_Vec(1, 0, 0) );
  TopoDS_Face C2 = TopoDS::Face( BRepBuilderAPI_Transform(C1, T1, true) );

  gp_Trsf T2; T2.SetTranslation( gp_Vec(0, 1, 0) );
  TopoDS_Face C3 = TopoDS::Face( BRepBuilderAPI_Transform(C2, T2, true) );

  gp_Trsf T3; T3.SetTranslation( gp_Vec(-1, 0, 0) );
  TopoDS_Face C4 = TopoDS::Face( BRepBuilderAPI_Transform(C3, T3, true) );

  gp_Trsf T4; T4.SetTranslation( gp_Vec(0.5, 0.5, 0) );
  TopoDS_Face C5 = TopoDS::Face( BRepBuilderAPI_Transform(C1, T4, true) );

  interp->GetPlotter().REDRAW_SHAPE("R", R, Color_Red, 1.0);
  interp->GetPlotter().REDRAW_SHAPE("C1", C1, Color_Blue, 1.0);
  interp->GetPlotter().REDRAW_SHAPE("C2", C2, Color_Blue, 1.0);
  interp->GetPlotter().REDRAW_SHAPE("C3", C3, Color_Blue, 1.0);
  interp->GetPlotter().REDRAW_SHAPE("C4", C4, Color_Blue, 1.0);
  interp->GetPlotter().REDRAW_SHAPE("C5", C5, Color_Blue, 1.0);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int MISC_TestOffset(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 /*argv*/)
{
  if ( argc != 1 )
    return TCL_ERROR;

  // Build rectangular face.
  TopoDS_Face R;
  TopoDS_Edge E1, E2, E3, E4;
  {
    gp_Pnt P1(0, 0, 0),
           P2(1, 0, 0),
           P3(1, 1, 0),
           P4(0, 1, 0);

    E1 = BRepBuilderAPI_MakeEdge(P1, P2);
    E2 = BRepBuilderAPI_MakeEdge(P2, P3);
    E3 = BRepBuilderAPI_MakeEdge(P3, P4);
    E4 = BRepBuilderAPI_MakeEdge(P4, P1);

    BRepBuilderAPI_MakeWire mkWire;
    mkWire.Add(E1); E1 = mkWire.Edge();
    mkWire.Add(E2); E2 = mkWire.Edge();
    mkWire.Add(E3); E3 = mkWire.Edge();
    mkWire.Add(E4); E4 = mkWire.Edge();

    R = BRepBuilderAPI_MakeFace( mkWire.Wire() );
  }

  // Make offset (use any offset value)
  BRepOffsetAPI_MakeThickSolid mkOffset;
  mkOffset.MakeThickSolidBySimple(R, 0.5);

  // Get result
  TopoDS_Shape result;
  if ( mkOffset.IsDone() )
  {
    result = mkOffset.Shape();
  }

  // Images of E1
  {
    // MODIFIED
    const TopTools_ListOfShape& MODIFIED = mkOffset.Modified(E1);
    //
    if ( !MODIFIED.IsEmpty() )
    {
      for ( TopTools_ListIteratorOfListOfShape lit(MODIFIED); lit.More(); lit.Next() )
        interp->GetPlotter().DRAW_SHAPE(lit.Value(), "E1 MODIFIED");
    }

    // GENERATED
    const TopTools_ListOfShape& GENERATED = mkOffset.Generated(E1);
    //
    if ( !GENERATED.IsEmpty() )
    {
      for ( TopTools_ListIteratorOfListOfShape lit(GENERATED); lit.More(); lit.Next() )
        interp->GetPlotter().DRAW_SHAPE(lit.Value(), "E1 GENERATED");
    }
  }

  // Images of R
  {
    // MODIFIED
    const TopTools_ListOfShape& MODIFIED = mkOffset.Modified(R);
    //
    if ( !MODIFIED.IsEmpty() )
    {
      for ( TopTools_ListIteratorOfListOfShape lit(MODIFIED); lit.More(); lit.Next() )
        interp->GetPlotter().DRAW_SHAPE(lit.Value(), "R MODIFIED");
    }

    // GENERATED
    const TopTools_ListOfShape& GENERATED = mkOffset.Generated(R);
    //
    if ( !GENERATED.IsEmpty() )
    {
      for ( TopTools_ListIteratorOfListOfShape lit(GENERATED); lit.More(); lit.Next() )
        interp->GetPlotter().DRAW_SHAPE(lit.Value(), "R GENERATED");
    }
  }

  interp->GetPlotter().REDRAW_SHAPE("result", result);
  return TCL_OK;
}

//-----------------------------------------------------------------------------

#include <Geom_BezierCurve.hxx>
#include <GeomConvert.hxx>
#include <GeomFill_Pipe.hxx>

int MISC_TestPipe1(const Handle(asiTcl_Interp)& interp,
                   int                          argc,
                   const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Construct path.
  TColgp_Array1OfPnt pathPoles(1, 3);
  pathPoles(1) = gp_Pnt(0,   0,   0);
  pathPoles(2) = gp_Pnt(100, 0,   0);
  pathPoles(3) = gp_Pnt(100, 100, 0);
  //
  Handle(Geom_BezierCurve) path = new Geom_BezierCurve(pathPoles);

  // Construct sections.
  Handle(Geom_Curve)
    c1 = GeomConvert::CurveToBSplineCurve(new Geom_TrimmedCurve(new Geom_Circle(gp_Ax2( gp::Origin(), gp::DX() ), 10.0), 0, 2*M_PI), Convert_Polynomial);
  //
  Handle(Geom_Curve)
    c2 = GeomConvert::CurveToBSplineCurve(new Geom_TrimmedCurve(new Geom_Circle(gp_Ax2( gp::Origin(), gp::DX() ), 20.0), 0, 2*M_PI), Convert_Polynomial);

  interp->GetPlotter().REDRAW_CURVE("c1", c1, Color_Red);
  interp->GetPlotter().REDRAW_CURVE("c2", c2, Color_Red);

  // Make pipe.
  GeomFill_Pipe Pipe(path, c1, c2);
  Pipe.Perform();

  // Get the result.
  const Handle(Geom_Surface)& result = Pipe.Surface();

  // Set the result as an output.
  interp->GetPlotter().REDRAW_SURFACE("result", result, Color_White);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int MISC_TestSweep1(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Construct path.
  BRepBuilderAPI_MakePolygon mkPath;
  mkPath.Add( gp_Pnt(0,   0,   0) );
  mkPath.Add( gp_Pnt(100, 0,   0) );
  mkPath.Add( gp_Pnt(100, 100, 0) );
  mkPath.Add( gp_Pnt(200, 50,  0) );
  //
  const TopoDS_Wire& pathWire = mkPath.Wire();
  //
  interp->GetPlotter().REDRAW_SHAPE("pathWire", pathWire, Color_Yellow, 1.0, true);

  // Initialize sweeping utility.
  BRepOffsetAPI_MakePipeShell mkPipeShell(pathWire);

  // Construct section.
  Handle(Geom_Curve)
    c1 = new Geom_Circle(gp_Ax2( gp::Origin(), gp::DX() ), 10.0);
  //
  TopoDS_Edge sectionEdge = BRepBuilderAPI_MakeEdge(c1);
  TopoDS_Wire sectionWire = BRepBuilderAPI_MakeWire(sectionEdge);
  //
  interp->GetPlotter().REDRAW_SHAPE("sectionWire", sectionWire, Color_Red, 1.0, true);

  // Add section to the sweeping tool.
  bool isT = false,
       isR = false;
  //
  mkPipeShell.Add(sectionWire, isT, isR);

  // Set evolution mode.
  mkPipeShell.SetMode(true);

  // Set transition strategy for C0 joints.
  mkPipeShell.SetTransitionMode(BRepBuilderAPI_RightCorner);

  // Build.
  mkPipeShell.Build();
  //
  if ( !mkPipeShell.IsDone() )
  {
    BRepBuilderAPI_PipeError Stat = mkPipeShell.GetStatus();
    if ( Stat == BRepBuilderAPI_PlaneNotIntersectGuide )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "One plane does not intersect the guide.");
    }
    if ( Stat == BRepBuilderAPI_ImpossibleContact )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "One section can not be in contact with the guide.");
    }
    return TCL_ERROR;
  }

  // Set the result.
  TopoDS_Shape result = mkPipeShell.Shape();
  //
  interp->GetPlotter().REDRAW_SHAPE("result", result);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int MISC_TestSweep2(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Construct section.
  Handle(Geom_Curve)
    c1 = new Geom_Circle(gp_Ax2( gp::Origin(), gp_Dir(1, 1, 0) ), 10.0);
  //
  TopoDS_Edge sectionEdge = BRepBuilderAPI_MakeEdge(c1);
  TopoDS_Wire sectionWire = BRepBuilderAPI_MakeWire(sectionEdge);
  //
  interp->GetPlotter().REDRAW_SHAPE("sectionWire", sectionWire, Color_Red, 1.0, true);

  BRepPrimAPI_MakePrism mkPrism( sectionWire, gp_Vec(100, 0, 0) );

  // Set the result.
  TopoDS_Shape result = mkPrism.Shape();
  //
  interp->GetPlotter().REDRAW_SHAPE("result", result);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

#include <asiAlgo_IneqOpt.h>

int MISC_TestIneq(const Handle(asiTcl_Interp)& interp,
                  int                          argc,
                  const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  /* ======================
   *  Set input parameters
   * ====================== */

  //---------------------------------------------------------------------------

  // Prepare inversion mode.
  Handle(asiAlgo_IneqOptParams) params = new asiAlgo_IneqOptParams(true);

  const int numVariables   = 3;
  const int numConstraints = 3;

  // Coefficients.
  const double bestPositionalTolerAbs = 0.99;
  const double bestAngularTolerAbs    = 0.99;
  //
  std::vector< t_ineqNCoord<double> >
    coeffs = { t_ineqNCoord<double>({1.0, -30.0, 0.0}),
               t_ineqNCoord<double>({1.0,  30.0, 0.0}),
               t_ineqNCoord<double>({0.0,   0.0, 1.0})
             };

  // Limits.
  std::vector<t_ineqRange>
    ANu = { t_ineqRange(-1.0, 1.0),
            t_ineqRange(-1.0, 1.0),
            t_ineqRange(-1.0, 1.0)
          };
  //
  std::vector<t_ineqRange>
    AKNu = { t_ineqRange(-bestPositionalTolerAbs, bestPositionalTolerAbs),
             t_ineqRange(-bestPositionalTolerAbs, bestPositionalTolerAbs),
             t_ineqRange(-bestAngularTolerAbs,    bestAngularTolerAbs)
            };

  //---------------------------------------------------------------------------

  //const int numVariables   = 6;
  //const int numConstraints = 6;

  //// Coefficients.
  //std::vector<t_ineqNCoord<double>>
  //  coeffs = { t_ineqNCoord<double>({1.0, -30.0, 0.0, 0.0, 0.0,  0.0}),
  //             t_ineqNCoord<double>({1.0,  30.0, 0.0, 0.0, 0.0,  0.0}),
  //             t_ineqNCoord<double>({0.0,   0.0, 1.0, 0.0, 0.0,  0.0}),
  //             t_ineqNCoord<double>({0.0,   0.0, 0.0, 1.0, 0.0,  0.0}),
  //             t_ineqNCoord<double>({0.0,   0.0, 0.0, 0.0, 1.0, -5.0}),
  //             t_ineqNCoord<double>({0.0,   0.0, 0.0, 0.0, 1.0,  5.0})
  //           };

  //// Limits.
  //std::vector<t_ineqRange>
  //  ANu = { t_ineqRange(-1.0,  1.0),
  //          t_ineqRange(-1.0,  1.0),
  //          t_ineqRange(-0.8,  0.8),
  //          t_ineqRange(-1.0,  1.0),
  //          t_ineqRange(-2.0,  2.0),
  //          t_ineqRange(-2.0,  2.0)
  //        };
  ////
  //std::vector<t_ineqRange>
  //  AKNu = { t_ineqRange(0.9,  1.0),
  //            t_ineqRange(0.5,  1.0),
  //            t_ineqRange(0.7,  0.8),
  //            t_ineqRange(0.9,  1.0),
  //            t_ineqRange(1.5,  2.0),
  //            t_ineqRange(1.5,  2.0)
  //          };

  //Handle(asiAlgo_IneqOptParams) params = new asiAlgo_IneqOptParams(false);
  ////
  //const int numVariables   = 3;
  //const int numConstraints = 4;

  //// Coefficients.
  //std::vector<t_ineqNCoord<double>>
  //  coeffs = { t_ineqNCoord<double>({-0.5,   0.0,  0.0}),
  //             t_ineqNCoord<double>({ 0.0,   3.0,  0.0}),
  //             t_ineqNCoord<double>({ 1.0,  -1.2,  2.0}),
  //             t_ineqNCoord<double>({ 0.5,  -0.2,  1.0}),
  //             t_ineqNCoord<double>({-0.15, -0.02, 3.0}) };

  //// Limits.
  //std::vector<t_ineqRange>
  //  ANu = { t_ineqRange(0.0,   40.0),
  //          t_ineqRange(0.0,   40.0),
  //          t_ineqRange(30.0,  120.0),
  //          t_ineqRange(10.0,  40.0),
  //          t_ineqRange(20.0,  80.0) };
  ////
  //std::vector<t_ineqRange>
  //  AKNu = { t_ineqRange(20.0, 40.0),
  //           t_ineqRange(30.0, 30.0),
  //           t_ineqRange(50.0, 120.0),
  //           t_ineqRange(20.0, 30.0),
  //           t_ineqRange(25.0, 70.0) };

  //---------------------------------------------------------------------------

  //Handle(asiAlgo_IneqOptParams) params = new asiAlgo_IneqOptParams(false);
  //const int numVariables   = 3;
  //const int numConstraints = 5;

  //// Coefficients.
  //std::vector<t_ineqNCoord<double>>
  //  coeffs = { t_ineqNCoord<double>({0.5, 0.0,  0.0}),
  //             t_ineqNCoord<double>({0.0, 3.0,  0.0}),
  //             t_ineqNCoord<double>({0.5, 0.5,  0.0}),
  //             t_ineqNCoord<double>({0.0, 0.25, 0.25}),
  //             t_ineqNCoord<double>({1.0, 1.2,  1.6}) };

  //// Limits.
  //std::vector<t_ineqRange>
  //  ANu = { t_ineqRange(0.0,   70.0),
  //          t_ineqRange(0.0,   80.0),
  //          t_ineqRange(0.0,   80.0),
  //          t_ineqRange(10.0,  100.0),
  //          t_ineqRange(10.0,  250.0) };
  ////
  //std::vector<t_ineqRange>
  //  AKNu = { t_ineqRange(20.0, 45.0),
  //           t_ineqRange(20.0, 30.0),
  //           t_ineqRange(40.0, 70.0),
  //           t_ineqRange(10.0, 50.0),
  //           t_ineqRange(50.0, 110.0) };

  //---------------------------------------------------------------------------

  //const int numVariables   = 3;
  //const int numConstraints = 9;

  //// Coefficients.
  //std::vector<t_ineqNCoord<double>>
  //  coeffs = { t_ineqNCoord<double>({1.0, 1.0, 1.0, 1.0, 1.0, 1.0}),
  //             t_ineqNCoord<double>({1.0, 1.0, 1.0, 0.0, 0.0, 0.0}),
  //             t_ineqNCoord<double>({0.0, 0.0, 0.0, 1.0, 1.0, 1.0}),
  //             t_ineqNCoord<double>({1.0, 0.0, 0.0, 0.0, 0.0, 0.0}),
  //             t_ineqNCoord<double>({0.0, 1.0, 0.0, 0.0, 0.0, 0.0}),
  //             t_ineqNCoord<double>({0.0, 0.0, 1.0, 0.0, 0.0, 0.0}),
  //             t_ineqNCoord<double>({0.0, 0.0, 0.0, 1.0, 0.0, 0.0}),
  //             t_ineqNCoord<double>({0.0, 0.0, 0.0, 0.0, 1.0, 0.0}),
  //             t_ineqNCoord<double>({0.0, 0.0, 0.0, 0.0, 0.0, 1.0})
  //           };

  //// Limits.
  //std::vector<t_ineqRange>
  //  ANu = { t_ineqRange(50.0, 80.0),
  //          t_ineqRange(10.0, 22.0),
  //          t_ineqRange(40.0, 60.0),
  //          t_ineqRange(9.0,  12.0),
  //          t_ineqRange(5.0,  11.0),
  //          t_ineqRange(3.0,  9.0),
  //          t_ineqRange(6.0,  6.0),
  //          t_ineqRange(10.0, 24.0),
  //          t_ineqRange(5.0,  19.0)
  //        };
  ////
  //std::vector<t_ineqRange>
  //  AKNu = { t_ineqRange(70.0, 80.0),
  //            t_ineqRange(10.0, 15.0),
  //            t_ineqRange(40.0, 60.0),
  //            t_ineqRange(9.0,  12.0),
  //            t_ineqRange(5.0,  11.0),
  //            t_ineqRange(3.0,  9.0),
  //            t_ineqRange(6.0,  6.0),
  //            t_ineqRange(10.0, 24.0),
  //            t_ineqRange(5.0,  19.0)
  //          };

  // Prepare parameters for the inequality optimizer.
  params->SetN (numVariables);
  params->SetM (numConstraints);
  //
  for ( int nu = 1; nu <= numConstraints; ++nu )
  {
    // Set the two-sided inequality constraint.
    params->SetAMinus (nu, ANu[nu-1].values.first);
    params->SetAPlus  (nu, ANu[nu-1].values.second);

    // Set 0-penalty ranges.
    params->SetInterval0(nu, AKNu[nu-1].values.first, AKNu[nu-1].values.second);

    // Set coefficients.
    params->SetCoeffs(nu, coeffs[nu-1]);
  }

  /* ===============
   *  Run optimizer
   * =============== */

  asiAlgo_IneqOpt Optimizer( params, interp->GetProgress(), interp->GetPlotter() );
  //
  if ( !Optimizer.Perform() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Optimizer failed.");
    return TCL_OK;
  }

  // Get solution.
  const t_ineqNCoord<int>&    SolR = Optimizer.GetSolR();
  const t_ineqNCoord<double>& SolX = Optimizer.GetSolX();

  // Draw solution.
  if ( SolX.Dim && numVariables == 2 )
    interp->GetPlotter().DRAW_POINT(gp_Pnt2d(SolX.V[0], SolX.V[1]), Color_Green, "SolX");
  else if ( SolX.Dim && numVariables == 3 )
    interp->GetPlotter().DRAW_POINT(gp_Pnt(SolX.V[0], SolX.V[1], SolX.V[2]), Color_Green, "SolX");

  TCollection_AsciiString SolRStr, SolXStr;
  //
  for ( size_t k = 0; k < SolR.Dim; ++k )
  {
    SolRStr += SolR.V[k];

    if ( k < SolR.Dim - 1 )
      SolRStr += " ";
  }
  //
  for ( size_t k = 0; k < SolX.Dim; ++k )
  {
    SolXStr += SolX.V[k];

    if ( k < SolX.Dim - 1 )
      SolXStr += " ";
  }

  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Optimizer converged with\n\tX = %1\n\tR = %2."
                                                       << SolXStr << SolRStr);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int MISC_TestEvalCurve(const Handle(asiTcl_Interp)& interp,
                       int                          argc,
                       const char**                 argv)
{
  if ( argc < 5 || argc > 6 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Find Curve Node by name.
  Handle(ActAPI_INode) node = interp->GetModel()->FindNodeByName(argv[1]);
  //
  if ( node.IsNull() || !node->IsKind( STANDARD_TYPE(asiData_IVCurveNode) ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Node '%1' is not a curve."
                                                        << argv[1]);
    return TCL_OK;
  }
  //
  Handle(asiData_IVCurveNode)
    curveNode = Handle(asiData_IVCurveNode)::DownCast(node);

  // Get curve.
  double f, l;
  Handle(Geom_Curve) occtCurve = curveNode->GetCurve(f, l);
  //
  if ( occtCurve.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "The curve in question is NULL.");
    return TCL_OK;
  }

  // Get parameter value.
  const double u = atof(argv[2]);

  // Get number of iterations.
  const int numIters = atoi(argv[3]);
  //
  if ( numIters <= 0 )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Number of iterations should be positive.");
    return TCL_OK;
  }

  // Get order of derivative to compute.
  const int order = atoi(argv[4]);

  // Check whether Mobius evaluation is requested.
  bool isMobius = interp->HasKeyword(argc, argv, "mobius");

  TIMER_NEW

  // Evaluate curve.
  if ( !isMobius )
  {
    TIMER_GO

    gp_Vec eval_DN;
    for ( int i = 0; i < numIters; ++i )
    {
      eval_DN = occtCurve->DN(u, order);
    }

    TIMER_FINISH
    TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Curve evaluation OCCT")

    interp->GetPlotter().REDRAW_VECTOR_AT("eval_DN", gp::Origin(), eval_DN, Color_Yellow);
  }
  else
  {
#ifndef USE_MOBIUS
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Mobius module is disabled.");
    return TCL_ERROR;
#else

    Handle(Geom_BSplineCurve)
      occtBCurve = Handle(Geom_BSplineCurve)::DownCast(occtCurve);
    //
    if ( occtBCurve.IsNull() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "The curve in question is not a B-curve.");
      return TCL_OK;
    }

    // Convert to Mobius curve.
    mobius::ptr<mobius::bcurve>
      mobCurve = mobius::cascade::GetMobiusBCurve(occtBCurve);

    TIMER_GO

    // Prepare matrix to hold evaluation results.
    mobius::core_HeapAlloc2D<double> Alloc;
    double** dN = Alloc.Allocate(mobCurve->GetDegree() + 1,
                                 mobCurve->GetDegree() + 1,
                                 true);

    // Evaluate.
    mobius::xyz eval_DN;
    for ( int i = 0; i < numIters; ++i )
    {
      mobCurve->Eval_Dk(dN, u, order, eval_DN);
    }

    TIMER_FINISH
    TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Curve evaluation Mobius")

    interp->GetPlotter().REDRAW_VECTOR_AT("eval_DN",
                                          gp::Origin(),
                                          gp_Vec( eval_DN.X(), eval_DN.Y(), eval_DN.Z() ),
                                          Color_Yellow);
#endif
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int MISC_TestEvalSurf(const Handle(asiTcl_Interp)& interp,
                      int                          argc,
                      const char**                 argv)
{
  if ( argc < 5 || argc > 6 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Find Surface Node by name.
  Handle(ActAPI_INode) node = interp->GetModel()->FindNodeByName(argv[1]);
  //
  if ( node.IsNull() || !node->IsKind( STANDARD_TYPE(asiData_IVSurfaceNode) ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Node '%1' is not a surface."
                                                        << argv[1]);
    return TCL_OK;
  }
  //
  Handle(asiData_IVSurfaceNode)
    surfNode = Handle(asiData_IVSurfaceNode)::DownCast(node);

  // Get surface.
  Handle(Geom_Surface) occtSurface = surfNode->GetSurface();
  //
  if ( occtSurface.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "The surface in question is NULL.");
    return TCL_OK;
  }

  // Get parameter pair.
  const double u = atof(argv[2]);
  const double v = atof(argv[3]);

  // Get number of iterations.
  const int numIters = atoi(argv[4]);
  //
  if ( numIters <= 0 )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Number of iterations should be positive.");
    return TCL_OK;
  }

  // Get order of derivative to compute.
  const int order = atoi(argv[5]);

  // Check whether Mobius evaluation is requested.
  bool isMobius = interp->HasKeyword(argc, argv, "mobius");

  TIMER_NEW

  // Evaluate surface.
  if ( !isMobius )
  {
    TIMER_GO

    gp_Pnt eval_P;
    for ( int i = 0; i < numIters; ++i )
    {
      occtSurface->D0(u, v, eval_P);
    }

    TIMER_FINISH
    TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Surface evaluation OCCT")

    interp->GetPlotter().REDRAW_POINT("eval_P", eval_P, Color_Yellow);
  }
  else
  {
#ifndef USE_MOBIUS
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Mobius module is disabled.");
    return TCL_ERROR;
#else

    Handle(Geom_BSplineSurface)
      occtBSurf = Handle(Geom_BSplineSurface)::DownCast(occtSurface);
    //
    if ( occtBSurf.IsNull() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "The surface in question is not a B-surface.");
      return TCL_OK;
    }

    // Convert to Mobius surface.
    mobius::ptr<mobius::bsurf>
      mobBSurf = mobius::cascade::GetMobiusBSurface(occtBSurf);

    TIMER_GO

    mobius::xyz eval_P;
    for ( int i = 0; i < numIters; ++i )
    {
      mobBSurf->Eval(u, v, eval_P);
    }

    TIMER_FINISH
    TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Surface evaluation Mobius")

    interp->GetPlotter().REDRAW_POINT("eval_P",
                                       gp_Pnt( eval_P.X(), eval_P.Y(), eval_P.Z() ),
                                       Color_Yellow);
#endif
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int MISC_Test(const Handle(asiTcl_Interp)& interp,
              int                          argc,
              const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  
  return TCL_OK;
}

#if defined USE_MOBIUS
//-----------------------------------------------------------------------------

int MISC_TestFair(const Handle(asiTcl_Interp)& interp,
                  int                          argc,
                  const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node to access the selected faces.
  Handle(asiData_PartNode)
    partNode = Handle(asiEngine_Model)::DownCast( interp->GetModel() )->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part Node is null or ill-defined.");
    return TCL_OK;
  }
  Handle(asiAlgo_AAG) aag   = partNode->GetAAG();
  TopoDS_Shape        shape = partNode->GetShape();

  // Get face.
  const int fid = atoi(argv[1]);
  //
  if ( !partNode->GetAAG()->HasFace(fid) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Face %1 does not exist in the working part."
                                                        << fid);
    return TCL_ERROR;
  }
  //
  const TopoDS_Face& face = aag->GetFace(fid);

  // Get host B-surface.
  Handle(Geom_BSplineSurface)
    occtBSurf = Handle(Geom_BSplineSurface)::DownCast( BRep_Tool::Surface(face) );
  //
  if ( occtBSurf.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Face %1 is not based on B-spline surface."
                                                        << fid);
    return TCL_ERROR;
  }

  // Convert to Mobius B-surface.
  mobius::ptr<mobius::bsurf>
    mobSurf = mobius::cascade::GetMobiusBSurface(occtBSurf);

  // Print bending energy.
  const double initEnergy = mobSurf->ComputeBendingEnergy();
  //
  interp->GetProgress().SendLogMessage( LogNotice(Normal) << "Initial bending energy: %1"
                                                          << initEnergy );

  // Utilities to find spans.
  std::vector<double> U = mobSurf->GetKnots_U();
  std::vector<double> V = mobSurf->GetKnots_V();
  const int           p = mobSurf->GetDegree_U();
  const int           q = mobSurf->GetDegree_V();
  //
  mobius::bspl_FindSpan findSpanU(U, p);
  mobius::bspl_FindSpan findSpanV(V, q);

  // Prepare fairing operator.
  mobius::geom_FairBSurf F(mobSurf, 1.0, NULL, NULL);

  // Calculate characteristics size for discretization of boundary.
  double uMin, uMax, vMin, vMax;
  BRepTools::UVBounds(face, uMin, uMax, vMin, vMax);
  //
  const double featSize = gp_Pnt2d(uMin, vMin).Distance( gp_Pnt2d(uMax, vMax) )*0.01;

  // Get edges of the face which are the constraints to satisfy.
  TopTools_IndexedMapOfShape faceEdges;
  TopExp::MapShapes(face, TopAbs_EDGE, faceEdges);

  // For each edge, take its parameteric curve and discretize it to obtain
  // series of (u,v) coordinates. These coordinates will be used to find
  // indices of the effective control points which should be constrained.
  std::vector<gp_XY> pts;
  for ( int eidx = 1; eidx <= faceEdges.Extent(); ++eidx )
  {
    const TopoDS_Edge& E = TopoDS::Edge( faceEdges(eidx) );

    // Discretize with a uniform curvilinear step.
    BRepAdaptor_Curve2d gac(E, face);
    GCPnts_UniformAbscissa Defl(gac, featSize);
    //
    if ( !Defl.IsDone() )
    {
      interp->GetPlotter().DRAW_SHAPE(E, Color_Red, 1.0, true, "cannotDiscr");
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Edge discretization failed.");
      return TCL_OK;
    }

    // Calculate discretization points.
    for ( int i = 1; i <= Defl.NbPoints(); ++i )
    {
      gp_XY P = gac.Value( Defl.Parameter(i) ).XY();
      //
      pts.push_back(P);

      //interp->GetPlotter().DRAW_POINT(P, Color_Red, "pts");

      // Get (u,v) coordinates.
      const double u = P.X();
      const double v = P.Y();

      // Find effective span.
      const int spanU = findSpanU(u);
      const int spanV = findSpanV(v);

      std::cout << "Found span in [u,v]: [" << spanU << "," << spanV << "]" << std::endl;

      gp_Pnt2d S1( U[spanU], V[spanV] );
      gp_Pnt2d S2( U[spanU + 1], V[spanV] );
      gp_Pnt2d S3( U[spanU + 1], V[spanV + 1] );
      gp_Pnt2d S4( U[spanU], V[spanV + 1] );

      /*interp->GetPlotter().DRAW_LINK(S1, S2, Color_Green, "S12");
      interp->GetPlotter().DRAW_LINK(S2, S3, Color_Green, "S23");
      interp->GetPlotter().DRAW_LINK(S3, S4, Color_Green, "S34");
      interp->GetPlotter().DRAW_LINK(S4, S1, Color_Green, "S41");*/

      const mobius::xyz& mobPole1 = mobSurf->GetPoles()[spanU][spanV];

      // Pin point.
      F.AddPinnedPole(spanU, spanV);

      interp->GetPlotter().DRAW_POINT(gp_Pnt( mobPole1.X(), mobPole1.Y(), mobPole1.Z() ), Color_Blue, "mobPole");

      for ( int k = 0; k < p; ++k )
      {
        //const mobius::xyz& PPP = mobSurf->Poles()[spanU - k][spanV];
        F.AddPinnedPole(spanU-k, spanV);
        //interp->GetPlotter().DRAW_POINT(gp_Pnt( PPP.X(), PPP.Y(), PPP.Z() ), Color_Red, "mobPoleU");
      }

      for ( int k = 0; k < q; ++k )
      {
        //const mobius::xyz& PPP = mobSurf->Poles()[spanU][spanV - k];
        F.AddPinnedPole(spanU, spanV-k);
        //interp->GetPlotter().DRAW_POINT(gp_Pnt( PPP.X(), PPP.Y(), PPP.Z() ), Color_Green, "mobPoleV");
      }
    }
  }

  TIMER_NEW
  TIMER_GO

  // Perform fairing.
  if ( !F.Perform() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Fairing failed.");
    return TCL_OK;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Fair surface")

  // Get fairing result.
  const mobius::ptr<mobius::bsurf>& mobFaired = F.GetResult();

  // Print bending energy.
  const double resEnergy = mobFaired->ComputeBendingEnergy();
  //
  interp->GetProgress().SendLogMessage( LogNotice(Normal) << "Resulting bending energy: %1"
                                                          << resEnergy );

  // Convert to OpenCascade surface.
  Handle(Geom_BSplineSurface)
    occtFaired = mobius::cascade::GetOpenCascadeBSurface(mobFaired);
  //
  interp->GetPlotter().DRAW_SURFACE(occtFaired, (resEnergy < initEnergy) ? Color_Green : Color_Red, "faired");

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int MISC_TestCoons(const Handle(asiTcl_Interp)& interp,
                   int                          argc,
                   const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  /* ===================
   *  Build rail curves
   * =================== */

  mobius::ptr<mobius::bcurve> c0, c1, b0, b1;
  mobius::xyz p00, p01, p10, p11;

  // Build `c` curves.
  {
    std::vector<mobius::xyz> c0_pts = {
      mobius::xyz(0., 0., 0.),
      mobius::xyz(1., 1., 1.),
      mobius::xyz(1., 2., 2.),
      mobius::xyz(2., 3., 4.)
    };

    std::vector<mobius::xyz> c1_pts = {
      mobius::xyz(5., 0., 0.),
      mobius::xyz(5., 1., 2.),
      mobius::xyz(5., 2., 2.),
      mobius::xyz(6., 3., 5.)
    };

    // Corner points.
    p00 = c0_pts[0];
    p01 = c1_pts[0];
    p10 = c0_pts[c0_pts.size() - 1];
    p11 = c1_pts[c1_pts.size() - 1];

    interp->GetPlotter().REDRAW_POINT("p00", mobius::cascade::GetOpenCascadePnt(p00), Color_Yellow);
    interp->GetPlotter().REDRAW_POINT("p01", mobius::cascade::GetOpenCascadePnt(p01), Color_Yellow);
    interp->GetPlotter().REDRAW_POINT("p10", mobius::cascade::GetOpenCascadePnt(p10), Color_Yellow);
    interp->GetPlotter().REDRAW_POINT("p11", mobius::cascade::GetOpenCascadePnt(p11), Color_Yellow);

    // Build sample curves.
    mobius::geom_InterpolateMultiCurve multiInterp(3,
                                                   mobius::ParamsSelection_Centripetal,
                                                   mobius::KnotsSelection_Average);
    //
    multiInterp.AddRow(c0_pts);
    multiInterp.AddRow(c1_pts);

    if ( multiInterp.Perform() )
    {
      // Get rail curves.
      c0 = multiInterp.GetResult(0);
      c1 = multiInterp.GetResult(1);
      //
      interp->GetPlotter().REDRAW_CURVE("c0", mobius::cascade::GetOpenCascadeBCurve(c0), Color_Default);
      interp->GetPlotter().REDRAW_CURVE("c1", mobius::cascade::GetOpenCascadeBCurve(c1), Color_Default);
    }
    else
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Multi-curve interpolation failed.");
      return TCL_ERROR;
    }
  }

  // Build `b` curves.
  {
    std::vector<mobius::xyz> b0_pts = {
      mobius::xyz(0.,   0.,  0.),
      mobius::xyz(2.1, -0.3, 0.),
      mobius::xyz(4.1,  0.1, 0.5),
      mobius::xyz(5.,   0.,  0.)
    };

    std::vector<mobius::xyz> b1_pts = {
      mobius::xyz(2.,  3.,  4.),
      mobius::xyz(3.,  3.2, 4.1),
      mobius::xyz(5.,  2.8, 4.5),
      mobius::xyz(6.,  3.,  5.)
    };

    // Build sample curves.
    mobius::geom_InterpolateMultiCurve multiInterp(3,
                                                   mobius::ParamsSelection_Centripetal,
                                                   mobius::KnotsSelection_Average);
    //
    multiInterp.AddRow(b0_pts);
    multiInterp.AddRow(b1_pts);

    if ( multiInterp.Perform() )
    {
      // Get rail curves.
      b0 = multiInterp.GetResult(0);
      b1 = multiInterp.GetResult(1);
      //
      interp->GetPlotter().REDRAW_CURVE("b0", mobius::cascade::GetOpenCascadeBCurve(b0), Color_Default);
      interp->GetPlotter().REDRAW_CURVE("b1", mobius::cascade::GetOpenCascadeBCurve(b1), Color_Default);
    }
    else
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Multi-curve interpolation failed.");
      return TCL_ERROR;
    }
  }

  /* ===================
   *  Build Coons patch
   * =================== */

  // Build Coons surface.
  mobius::ptr<mobius::geom_CoonsSurface>
    coons = new mobius::geom_CoonsSurface(c0, c1, b0, b1, p00, p01, p10, p11);

  // Sample patch.
  Handle(asiAlgo_BaseCloud<double>) pts = new asiAlgo_BaseCloud<double>;
  //
  const double uMin = coons->GetMinParameter_U();
  const double uMax = coons->GetMaxParameter_U();
  const double vMin = coons->GetMinParameter_V();
  const double vMax = coons->GetMaxParameter_V();
  //
  const double deltaU = (uMax - uMin)/100.;
  const double deltaV = (vMax - vMin)/100.;
  //
  double u = uMin;
  //
  bool stopU = false;
  do
  {
    if ( u > uMax )
      stopU = true;
    else
    {
      double v = vMin;
      //
      bool stopV = false;
      do
      {
        if ( v > vMax )
          stopV = true;
        else
        {
          mobius::xyz P;
          coons->Eval(u, v, P);

          pts->AddElement( P.X(), P.Y(), P.Z() );

          v += deltaV;
        }
      }
      while ( !stopV );

      u += deltaU;
    }
  }
  while ( !stopU );
  //
  interp->GetPlotter().DRAW_POINTS(pts->GetCoordsArray(), Color_White, "coons");

  // Use constrained filling for demo.
  std::vector<Handle(Geom_BSplineCurve)>
    curves = { mobius::cascade::GetOpenCascadeBCurve(c0),
               mobius::cascade::GetOpenCascadeBCurve(c1),
               mobius::cascade::GetOpenCascadeBCurve(b0),
               mobius::cascade::GetOpenCascadeBCurve(b1) };
  //
  Handle(Geom_BSplineSurface) surf;
  //
  if ( !asiAlgo_Utils::Fill4Contour(curves, surf) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Filling 4-sides failed.");
    return TCL_ERROR;
  }
  //
  interp->GetPlotter().DRAW_SURFACE(surf, Color_White, "coonssurf");

  /* =========================================================
   *  Use Boolean sum approach to build an equivalent surface
   * ========================================================= */

  // Build P1S.
  mobius::ptr<mobius::bsurf>  P1S;
  Handle(Geom_BSplineSurface) P1Socc;
  {
    // Prepare rail curves.
    std::vector< mobius::ptr<mobius::bcurve> > rails = {c0, c1};

    // Skin ruled surface through c0 and c1 rails.
    mobius::geom_SkinSurface skinner(rails, 1, false);
    //
    if ( !skinner.Perform() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot build P1S.");
      return TCL_ERROR;
    }
    //
    P1S = skinner.GetResult();

    // Convert to OCC form to run IncreaseDegree() method which does not
    // currently exist in Mobius.
    P1Socc = mobius::cascade::GetOpenCascadeBSurface(P1S);

    // Elevate degree.
    P1Socc->IncreaseDegree(3, 3);

    // Convert back to Mobius.
    P1S = mobius::cascade::GetMobiusBSurface(P1Socc);

    // Draw.
    interp->GetPlotter().REDRAW_SURFACE("P1S", mobius::cascade::GetOpenCascadeBSurface(P1S), Color_Red);
  }

  // Build P2S.
  mobius::ptr<mobius::bsurf>  P2S;
  Handle(Geom_BSplineSurface) P2Socc;
  {
    // Prepare rail curves.
    std::vector< mobius::ptr<mobius::bcurve> > rails = {b0, b1};

    // Skin ruled surface through b0 and b1 rails.
    mobius::geom_SkinSurface skinner(rails, 1, false);
    //
    if ( !skinner.Perform() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot build P2S.");
      return TCL_ERROR;
    }
    //
    P2S = skinner.GetResult();

    // Flip UV coordinates of P2S to be compliant with other surfaces.
    P2S->ExchangeUV();

    // Convert to OCC form to run IncreaseDegree() method which does not
    // currently exist in Mobius.
    P2Socc = mobius::cascade::GetOpenCascadeBSurface(P2S);

    // Elevate degree.
    P2Socc->IncreaseDegree(3, 3);

    // Convert back to Mobius.
    P2S = mobius::cascade::GetMobiusBSurface(P2Socc);

    // Draw.
    interp->GetPlotter().REDRAW_SURFACE("P2S", mobius::cascade::GetOpenCascadeBSurface(P2S), Color_Green);
  }

  // Build P12S.
  mobius::ptr<mobius::bsurf>  P12S;
  Handle(Geom_BSplineSurface) P12Socc;
  {
    std::vector< std::vector<mobius::xyz> >
      poles = { {p00, p01},
                {p10, p11} };

    // Define knot vector.
    std::vector<double> T = {0, 0, 1, 1};

    // Construct directly.
    P12S = new mobius::geom_BSplineSurface(poles, T, T, 1, 1);

    // Convert to OCC form to run IncreaseDegree() method which does not
    // currently exist in Mobius.
    P12Socc = mobius::cascade::GetOpenCascadeBSurface(P12S);

    // Elevate degree.
    P12Socc->IncreaseDegree(3, 3);

    // Convert back to Mobius.
    P12S = mobius::cascade::GetMobiusBSurface(P12Socc);

    // Draw.
    interp->GetPlotter().REDRAW_SURFACE("P12S", mobius::cascade::GetOpenCascadeBSurface(P12S), Color_Blue);
  }

  enum PS
  {
    PS_P1S = 0,
    PS_P2S,
    PS_P12S
  };

  // All U knots.
  std::vector< std::vector<double> >
    knotVectors_U = { P1S->GetKnots_U(),
                      P2S->GetKnots_U(),
                      P12S->GetKnots_U() };

  // All V knots.
  std::vector< std::vector<double> >
    knotVectors_V = { P1S->GetKnots_V(),
                      P2S->GetKnots_V(),
                      P12S->GetKnots_V() };

  // Compute addendum knots.
  mobius::bspl_UnifyKnots unifyKnots;
  //
  std::vector< std::vector<double> > U_addendums = unifyKnots(knotVectors_U);
  std::vector< std::vector<double> > V_addendums = unifyKnots(knotVectors_V);

  // Insert U knots to P1S.
  for ( size_t ii = 0; ii < U_addendums[PS_P1S].size(); ++ii )
    P1S->InsertKnot_U(U_addendums[PS_P1S][ii]);

  // Insert V knots to P1S.
  for ( size_t ii = 0; ii < V_addendums[PS_P1S].size(); ++ii )
    P1S->InsertKnot_V(V_addendums[PS_P1S][ii]);

  // Insert U knots to P2S.
  for ( size_t ii = 0; ii < U_addendums[PS_P2S].size(); ++ii )
    P2S->InsertKnot_U(U_addendums[PS_P2S][ii]);

  // Insert V knots to P2S.
  for ( size_t ii = 0; ii < V_addendums[PS_P2S].size(); ++ii )
    P2S->InsertKnot_V(V_addendums[PS_P2S][ii]);

  // Insert U knots to P12S.
  for ( size_t ii = 0; ii < U_addendums[PS_P12S].size(); ++ii )
    P12S->InsertKnot_U(U_addendums[PS_P12S][ii]);

  // Insert V knots to P12S.
  for ( size_t ii = 0; ii < V_addendums[PS_P12S].size(); ++ii )
    P12S->InsertKnot_V(V_addendums[PS_P12S][ii]);

  // Common knots.
  const std::vector<double>& Ucommon = P1S->GetKnots_U();
  const std::vector<double>& Vcommon = P1S->GetKnots_V();

  // Common degrees.
  const int pcommon = P1S->GetDegree_U();
  const int qcommon = P1S->GetDegree_V();

  // Draw.
  interp->GetPlotter().REDRAW_SURFACE("P1S",  mobius::cascade::GetOpenCascadeBSurface(P1S),  Color_Blue);
  interp->GetPlotter().REDRAW_SURFACE("P2S",  mobius::cascade::GetOpenCascadeBSurface(P2S),  Color_Blue);
  interp->GetPlotter().REDRAW_SURFACE("P12S", mobius::cascade::GetOpenCascadeBSurface(P12S), Color_Blue);

  // Now all patches are of the same degrees and defined on identical knot
  // vectors. It means that all patches are defined on the same basis. Therefore,
  // we can now produce a Boolean sum.

  const std::vector< std::vector<mobius::xyz> >& polesP1S  = P1S->GetPoles();
  const std::vector< std::vector<mobius::xyz> >& polesP2S  = P2S->GetPoles();
  const std::vector< std::vector<mobius::xyz> >& polesP12S = P12S->GetPoles();

  const int numPolesU = P1S->GetNumOfPoles_U();
  const int numPolesV = P1S->GetNumOfPoles_V();

  // Compute the resulting poles.
  std::vector< std::vector<mobius::xyz> > resPoles;
  //
  for ( int i = 0; i < numPolesU; ++i )
  {
    std::vector<mobius::xyz> col;
    for ( int j = 0; j < numPolesV; ++j )
    {
      mobius::xyz resPole = polesP1S[i][j] + polesP2S[i][j] - polesP12S[i][j];
      //
      col.push_back(resPole);
    }
    resPoles.push_back(col);
  }

  // Construct the resulting surface.
  mobius::ptr<mobius::bsurf>
    mobRes = new mobius::bsurf(resPoles, Ucommon, Vcommon, pcommon, qcommon);
  //
  interp->GetPlotter().REDRAW_SURFACE("res", mobius::cascade::GetOpenCascadeBSurface(mobRes), Color_Default);

  return TCL_OK;
}
#endif

//-----------------------------------------------------------------------------

void cmdMisc::Factory(const Handle(asiTcl_Interp)&      interp,
                      const Handle(Standard_Transient)& /*data*/)
{
  static const char* group = "cmdMisc";

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-hexagon-bops",
    //
    "test-hexagon-bops \n"
    "\t No arguments are expected. This command demonstrates solid Booleans \n"
    "\t of OpenCascade. It cuts many cylinders from a hexagonal prism.",
    //
    __FILE__, group, MISC_TestHexagonBops);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-hexagon-bops-faces",
    //
    "test-hexagon-bops-faces \n"
    "\t No arguments are expected. This command demonstrates solid Booleans \n"
    "\t of OpenCascade. It cuts many circles from a hexagon and then \n"
    "\t constructs a prism.",
    //
    __FILE__, group, MISC_TestHexagonBopsFaces);

  //-------------------------------------------------------------------------//
  interp->AddCommand("push-pull",
    //
    "push-pull faceId offset \n"
    "\t Push/pull operation.",
    //
    __FILE__, group, MISC_PushPull);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-extend",
    //
    "test-extend dim dir offset \n"
    "\t Shape extension operation. The following parameters are accepted: \n"
    "\t - dim: dimension to change (\"X\", \"Y\" or \"Z\"); \n"
    "\t - dir: in which direction to move (\"up\", \"down\" or \"both\"); \n"
    "\t - offset: offset to apply (can be positive or negative).",
    //
    __FILE__, group, MISC_TestExtend);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-builder",
    //
    "test-builder \n"
    "\t Reproducer for issue with BRep_Builder.",
    //
    __FILE__, group, MISC_TestBuilder);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-composite",
    //
    "test-composite \n"
    "\t Constructs sample non-manifold CAD model.",
    //
    __FILE__, group, MISC_TestComposite);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-offset",
    //
    "test-offset \n"
    "\t Constructs simple solid by offset.",
    //
    __FILE__, group, MISC_TestOffset);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-pipe1",
    //
    "test-pipe \n"
    "\t Problem reproducer for pipes.",
    //
    __FILE__, group, MISC_TestPipe1);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-sweep1",
    //
    "test-sweep1 \n"
    "\t Problem reproducer for sweeping.",
    //
    __FILE__, group, MISC_TestSweep1);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-sweep2",
    //
    "test-sweep2 \n"
    "\t Problem reproducer for sweeping.",
    //
    __FILE__, group, MISC_TestSweep2);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-ineq",
    //
    "test-ineq \n"
    "\t Test for inequality solver.",
    //
    __FILE__, group, MISC_TestIneq);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-eval-curve",
    //
    "test-eval-curve curveName u num order [-mobius]\n"
    "\t Evaluates curve <curveName> for the given parameter value <u>.\n"
    "\t If <-mobius> keyword is used, evaluation is performed using Mobius\n"
    "\t functions. The argument <num> specifies how many iterations to use.\n"
    "\t This function is used to test performance of curve evaluation.",
    //
    __FILE__, group, MISC_TestEvalCurve);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-eval-surf",
    //
    "test-eval-surf surfName u v num order [-mobius]\n"
    "\t Evaluates surface <surfName> for the given parameter pair <u, v>.\n"
    "\t If <-mobius> keyword is used, evaluation is performed using Mobius\n"
    "\t functions. The argument <num> specifies how many iterations to use.\n"
    "\t This function is used to test performance of surface evaluation.",
    //
    __FILE__, group, MISC_TestEvalSurf);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test",
    //
    "test \n"
    "\t Problem reproducer for anything.",
    //
    __FILE__, group, MISC_Test);

#if defined USE_MOBIUS
  //-------------------------------------------------------------------------//
  interp->AddCommand("test-fair",
    //
    "test-fair fid\n"
    "\t Test fairing function.",
    //
    __FILE__, group, MISC_TestFair);

  //-------------------------------------------------------------------------//
  interp->AddCommand("test-coons",
    //
    "test-coons\n"
    "\t Test Coons patch.",
    //
    __FILE__, group, MISC_TestCoons);
#endif
}

// Declare entry point
ASIPLUGIN(cmdMisc)
