//-----------------------------------------------------------------------------
// Created on: 19 April 2023
//-----------------------------------------------------------------------------
// Copyright (c) 2023, Elizaveta Krylova
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
#include <asiAlgo_Join3dEdges.h>

// asiAlgo includes
#include <asiAlgo_CheckValidity.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepTools_ReShape.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <GCE2d_MakeLine.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAPI_Interpolate.hxx>
#include <GeomAPI.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <gp_Pln.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeConstruct_Curve.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeFix_Edge.hxx>
#include <ShapeFix_Face.hxx>
#include <ShapeFix_ShapeTolerance.hxx>
#include <ShapeFix_ShapeTolerance.hxx>
#include <ShapeFix_Wire.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

//-----------------------------------------------------------------------------

namespace
{
  template<class Pt>
  static inline void GetReversedParameters(const Pt& p11,
                                           const Pt& p12,
                                           const Pt& p21,
                                           const Pt& p22,
                                           bool& isRev1,
                                           bool& isRev2)
  {
    isRev1 = Standard_False;
    isRev2 = Standard_False;
    //gka protection against crossing seem on second face

    double d11   = p11.Distance(p21);
    double d21   = p12.Distance(p21);
    double d12   = p11.Distance(p22);
    double d22   = p22.Distance(p12);
    double Dmin1 = Min(d11,d21);
    double Dmin2 = Min(d12,d22);
    if(fabs(Dmin1 - Dmin2) <= Precision::Confusion() || Dmin2 > Dmin1) {
      isRev1 = (d11 < d21 ? Standard_True : Standard_False);
    }
    else if(Dmin2 < Dmin1) {
      isRev1 = (d12 < d22 ? Standard_True  : Standard_False);
      isRev2 = Standard_True;
    }
  }

  template<class Curve>
  static inline void SegmentCurve(Curve&       curve,
                                  const double first,
                                  const double last)
  {
    const double pconfusion = Precision::PConfusion();

    if (curve->FirstParameter() < first - pconfusion ||
        curve->LastParameter() > last + pconfusion)
    {
      if (curve->IsPeriodic())
      {
        curve->Segment(first, last);
      }
      else
      {
        curve->Segment(Max(curve->FirstParameter(), first),
                       Min(curve->LastParameter(), last));
      }
    }
  }

  template<class Curve>
  static inline Curve GetCurveCopy(const Curve&              curve,
                                   double&                   first,
                                   double&                   last,
                                   const TopAbs_Orientation& orient)
  {
    if ( orient == TopAbs_REVERSED ) {
      double cf = first;
      first = curve->ReversedParameter ( last );
      last = curve->ReversedParameter ( cf );
      return curve->Reversed();
    }
    return Curve::DownCast(curve->Copy());
  }

  // Joins the passed 3D curves.
  bool JoinCurves(const Handle(Geom_Curve)& ac3d1,
                  const Handle(Geom_Curve)& ac3d2,
                  const TopAbs_Orientation& Orient1,
                  const TopAbs_Orientation& Orient2,
                  double&                   first1,
                  double&                   last1,
                  double&                   first2,
                  double&                   last2,
                  Handle(Geom_Curve)&       c3dOut,
                  bool&                     isRev1,
                  bool&                     isRev2)

  {
    Handle(Geom_Curve) c3d1,c3d2;

    c3d1 = GetCurveCopy ( ac3d1, first1, last1, Orient1 );
    c3d2 = GetCurveCopy ( ac3d2, first2, last2, Orient2 );

    ShapeConstruct_Curve scc;
    Standard_Boolean after =  true;
    Handle(Geom_BSplineCurve) bsplc1 = scc.ConvertToBSpline(c3d1, first1, last1,Precision::Confusion());
    Handle(Geom_BSplineCurve) bsplc2 = scc.ConvertToBSpline(c3d2, first2, last2,Precision::Confusion());

    if(bsplc1.IsNull() || bsplc2.IsNull())
    {
      return false;
    }

    SegmentCurve(bsplc1,first1, last1);
    SegmentCurve(bsplc2,first2, last2);

    //regression on file 866026_M-f276-f311.brep bug OCC482
    gp_Pnt pp11 =  bsplc1->Pole(1);
    gp_Pnt pp12 =  bsplc1->Pole(bsplc1->NbPoles());

    gp_Pnt pp21 =  bsplc2->Pole(1);
    gp_Pnt pp22 =  bsplc2->Pole(bsplc2->NbPoles());

    GetReversedParameters(pp11,pp12,pp21,pp22,isRev1,isRev2);

    if(isRev1) {
      bsplc1->Reverse();
    }
    if(isRev2)
      bsplc2->Reverse();

    gp_Pnt pmid = 0.5 * ( bsplc1->Pole(bsplc1->NbPoles()).XYZ() + bsplc2->Pole(1).XYZ() );
    bsplc1->SetPole(bsplc1->NbPoles(), pmid);
    bsplc2->SetPole(1, pmid);
    GeomConvert_CompCurveToBSplineCurve connect3d(bsplc1);
    if(!connect3d.Add(bsplc2,Precision::Confusion(), after, false))
    {
      return false;
    }
    c3dOut = connect3d.BSplineCurve();
    return true;
  }
}

//-----------------------------------------------------------------------------

asiAlgo_Join3dEdges::asiAlgo_Join3dEdges(const TopoDS_Shape&  masterCAD,
                                         ActAPI_ProgressEntry progress,
                                         ActAPI_PlotterEntry  plotter)
{
  m_master   = masterCAD;
  m_progress = progress;
  m_plotter  = plotter;
}

//-----------------------------------------------------------------------------

bool asiAlgo_Join3dEdges::Perform(const TopTools_IndexedMapOfShape& edges)
{
  if ( edges.Extent() != 2 )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Only a pair of edges is accepted.");
    return false;
  }

  // Join edges
  TopoDS_Edge E1, E2, newE;
  chooseOrder(edges, E1, E2);
  //
  // Get orientation of the edges on their host faces. We want to have
  // orientation irrelevant of face orientation, as we are going to
  // work in the parametric domain.
  TopAbs_Orientation eFirstOri = TopAbs_EXTERNAL, eSecondOri = TopAbs_EXTERNAL;
  //
  for (TopExp_Explorer exp(m_master, TopAbs_EDGE); exp.More(); exp.Next())
  {
    if (exp.Current().IsPartner(E1))
      eFirstOri = exp.Current().Orientation();
    else if (exp.Current().IsPartner(E2))
      eSecondOri = exp.Current().Orientation();
  }
  // Choose orientation of the bridge edge being built.
  bool isRev1, isRev2;
  if (eFirstOri == TopAbs_FORWARD)
    isRev1 = false;
  else
    isRev1 = true;
  if (eSecondOri == TopAbs_FORWARD)
    isRev2 = false;
  else
    isRev2 = true;
  //
  if (!joinEdges(E1, E2, eFirstOri, eSecondOri, newE, isRev1, isRev2))
  {
    return false;
  }

  // Change old face with the reconstructed one
  Handle(BRepTools_ReShape) ReShape = new BRepTools_ReShape;
  ReShape->Replace(E1, newE);
  ReShape->Remove(E2);
  m_result = ReShape->Apply(m_master);

  m_progress.SendLogMessage(LogInfo(Normal) << "Edges have been concatenated successfully.");

  return true; // Success
}

//-----------------------------------------------------------------------------

void asiAlgo_Join3dEdges::chooseOrder(const TopTools_IndexedMapOfShape& edges,
                                    TopoDS_Edge&                      eFirst,
                                    TopoDS_Edge&                      eSecond) const
{
  const TopoDS_Edge& eCandidate1 = TopoDS::Edge( edges(1) );
  const TopoDS_Edge& eCandidate2 = TopoDS::Edge( edges(2) );

  double f1, l1, f2, l2;
  Handle(Geom_Curve) cCandidate1 = BRep_Tool::Curve(eCandidate1, f1, l1);
  Handle(Geom_Curve) cCandidate2 = BRep_Tool::Curve(eCandidate2, f2, l2);

  const double dist1 = cCandidate1->Value(l1).Distance(cCandidate2->Value(f2));
  const double dist2 = cCandidate2->Value(l2).Distance(cCandidate1->Value(f1));

  if (dist1 < dist2)
  {
    eFirst  = eCandidate1;
    eSecond = eCandidate2;
  }
  else
  {
    eFirst  = eCandidate2;
    eSecond = eCandidate1;
  }
}

//-----------------------------------------------------------------------------

bool asiAlgo_Join3dEdges::joinEdges(const TopoDS_Edge&        eFirst,
                                    const TopoDS_Edge&        eSecond,
                                    const TopAbs_Orientation& eFirstOri,
                                    const TopAbs_Orientation& eSecondOri,
                                    TopoDS_Edge&              eResult,
                                    bool                      isRev1,
                                    bool                      isRev2) const
{
  ShapeAnalysis_Edge sae;

  Handle(Geom_Curve) c1, c2;
  double first1, last1, first2, last2;

  if (!sae.Curve3d(eFirst, c1, first1, last1, false))
    return false;

  if (!sae.Curve3d(eSecond, c2, first2, last2, false))
    return false;

  Handle(Geom_Curve) cRes;
  //
  if(!::JoinCurves(c1,
                   c2,
                   eFirstOri,
                   eSecondOri,
                   first1,
                   last1,
                   first2,
                   last2,
                   cRes,
                   isRev1,
                   isRev2))
  {
    return false;
  }

  // Build edge.
  BRepBuilderAPI_MakeEdge mkEdge(cRes);
  eResult = mkEdge.Edge();

  // Recover missing geometry
  ShapeFix_Edge SFE;
  SFE.FixAddCurve3d(eResult);

  return true;
}
