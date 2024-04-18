// Created on: 1995-07-17
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <asiAlgo_HlrBCurveTool.h>

#include <BRepAdaptor_Curve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>

using namespace asiAlgo::hlr;

//=======================================================================
//function : FirstParameter
//purpose  :
//=======================================================================

 Standard_Real
BCurveTool::FirstParameter (const BRepAdaptor_Curve& C)
{ return(C.FirstParameter()); }

//=======================================================================
//function : LastParameter
//purpose  :
//=======================================================================

 Standard_Real
BCurveTool::LastParameter (const BRepAdaptor_Curve& C)
{ return(C.LastParameter()); }

//=======================================================================
//function : Continuity
//purpose  :
//=======================================================================

 GeomAbs_Shape
BCurveTool::Continuity (const BRepAdaptor_Curve& C)
{ return(C.Continuity()); }

//=======================================================================
//function : NbIntervals
//purpose  :
//=======================================================================

 Standard_Integer
BCurveTool::NbIntervals(const BRepAdaptor_Curve& C,
                                const GeomAbs_Shape Sh)
{ return(C.NbIntervals(Sh)); }

//=======================================================================
//function : Intervals
//purpose  :
//=======================================================================

 void
BCurveTool::Intervals(const BRepAdaptor_Curve& C,
                              TColStd_Array1OfReal& Tab,
                              const GeomAbs_Shape Sh)
{ C.Intervals(Tab,Sh); }

//=======================================================================
//function : IsClosed
//purpose  :
//=======================================================================

 Standard_Boolean
BCurveTool::IsClosed(const BRepAdaptor_Curve& C)
{ return(C.IsClosed()); }

//=======================================================================
//function : IsPeriodic
//purpose  :
//=======================================================================

 Standard_Boolean
BCurveTool::IsPeriodic(const BRepAdaptor_Curve& C)
{ return(C.IsPeriodic()); }

//=======================================================================
//function : Period
//purpose  :
//=======================================================================

 Standard_Real
BCurveTool::Period(const BRepAdaptor_Curve& C)
{ return(C.Period()); }

//=======================================================================
//function : Value
//purpose  :
//=======================================================================

 gp_Pnt
BCurveTool::Value (const BRepAdaptor_Curve& C,
			   const Standard_Real U)
{ return(C.Value(U)); }

//=======================================================================
//function : D0
//purpose  :
//=======================================================================

 void
BCurveTool::D0(const BRepAdaptor_Curve& C,
		       const Standard_Real U,
		       gp_Pnt& P)
{ C.D0(U,P); }

//=======================================================================
//function : D1
//purpose  :
//=======================================================================

 void
BCurveTool::D1 (const BRepAdaptor_Curve& C,
			const Standard_Real U,
			gp_Pnt& P,
			gp_Vec& T)
{ C.D1(U,P,T); }

 void
BCurveTool::D2 (const BRepAdaptor_Curve& C,
			const Standard_Real U,
			gp_Pnt& P,
			gp_Vec& T,
			gp_Vec& N)
{ C.D2(U,P,T,N); }

//=======================================================================
//function : D3
//purpose  :
//=======================================================================

 void
BCurveTool::D3 (const BRepAdaptor_Curve& C,
			const Standard_Real U,
			gp_Pnt& P,
			gp_Vec& V1,
			gp_Vec& V2,
			gp_Vec& V3)
{ C.D3(U,P,V1,V2,V3); }

//=======================================================================
//function : DN
//purpose  :
//=======================================================================

 gp_Vec
BCurveTool::DN (const BRepAdaptor_Curve& C,
			const Standard_Real U,
			const Standard_Integer N)
{ return(C.DN(U,N)); }

//=======================================================================
//function : Resolution
//purpose  :
//=======================================================================

 Standard_Real
BCurveTool::Resolution(const BRepAdaptor_Curve& C,
			       const Standard_Real R3d)
{ return(C.Resolution(R3d)); }

//=======================================================================
//function : GetType
//purpose  :
//=======================================================================

 GeomAbs_CurveType
 BCurveTool::GetType(const BRepAdaptor_Curve& C)
{ return(C.GetType()); }

//=======================================================================
//function : Line
//purpose  :
//=======================================================================

 gp_Lin
BCurveTool::Line (const BRepAdaptor_Curve& C)
{ return(C.Line()); }

//=======================================================================
//function : Circle
//purpose  :
//=======================================================================

 gp_Circ
BCurveTool::Circle (const BRepAdaptor_Curve& C)
{ return(C.Circle()); }

//=======================================================================
//function : Ellipse
//purpose  :
//=======================================================================

 gp_Elips
BCurveTool::Ellipse (const BRepAdaptor_Curve& C)
{ return(C.Ellipse()); }

//=======================================================================
//function : Parabola
//purpose  :
//=======================================================================

 gp_Parab
BCurveTool::Parabola (const BRepAdaptor_Curve& C)
{ return(C.Parabola()); }

//=======================================================================
//function : Hyperbola
//purpose  :
//=======================================================================

 gp_Hypr
BCurveTool::Hyperbola (const BRepAdaptor_Curve& C)
{ return(C.Hyperbola()); }

//=======================================================================
//function : Degree
//purpose  :
//=======================================================================

 Standard_Integer
BCurveTool::Degree (const BRepAdaptor_Curve& C)
{ return(C.Degree()); }

//=======================================================================
//function : IsRational
//purpose  :
//=======================================================================

 Standard_Boolean
BCurveTool::IsRational (const BRepAdaptor_Curve& C)
{ return(C.IsRational()); }

//=======================================================================
//function : NbPoles
//purpose  :
//=======================================================================

 Standard_Integer
BCurveTool::NbPoles (const BRepAdaptor_Curve& C)
{ return(C.NbPoles()); }

//=======================================================================
//function : NbKnots
//purpose  :
//=======================================================================

 Standard_Integer
BCurveTool::NbKnots(const BRepAdaptor_Curve& C)
{ return(C.NbKnots()); }

//=======================================================================
//function : NbSamples
//purpose  :
//=======================================================================
Standard_Integer
BCurveTool::NbSamples (const BRepAdaptor_Curve& C,
			       const Standard_Real U0,
			       const Standard_Real U1)
{
  GeomAbs_CurveType typC = C.GetType();
  static Standard_Real nbsOther = 10.0;
  Standard_Real nbs = nbsOther;

  if(typC == GeomAbs_Line)
    nbs = 2;
  else if(typC == GeomAbs_BezierCurve)
    nbs = 3 + C.NbPoles();
  else if(typC == GeomAbs_BSplineCurve) {
    nbs = C.NbKnots();
    nbs*= C.Degree();
    nbs*= C.LastParameter()- C.FirstParameter();
    nbs/= U1-U0;
    if(nbs < 2.0) nbs=2;
  }
  if(nbs>50)
    nbs = 50;
  return((Standard_Integer)nbs);
}

//=======================================================================
//function : Poles
//purpose  :
//=======================================================================

void BCurveTool::Poles(const BRepAdaptor_Curve& C,
			       TColgp_Array1OfPnt& T)
{
  if(C.GetType() == GeomAbs_BezierCurve)
    C.Bezier()->Poles(T);
  else if(C.GetType() == GeomAbs_BSplineCurve)
    C.BSpline()->Poles(T);
}

//=======================================================================
//function : PolesAndWeights
//purpose  :
//=======================================================================

void BCurveTool::PolesAndWeights(const BRepAdaptor_Curve& C,
					 TColgp_Array1OfPnt& T,
					 TColStd_Array1OfReal& W)
{
  if(C.GetType() == GeomAbs_BezierCurve) {
    const Handle(Geom_BezierCurve) HB = C.Bezier();
    HB->Poles(T);
    HB->Weights(W);
  }
  else if(C.GetType() == GeomAbs_BSplineCurve) {
    const Handle(Geom_BSplineCurve) HB = C.BSpline();
    HB->Poles(T);
    HB->Weights(W);
  }
}

//=======================================================================
//function : Bezier
//purpose  :
//=======================================================================

Handle(Geom_BezierCurve)
     BCurveTool::Bezier (const BRepAdaptor_Curve& C)
{ return(C.Bezier()); }

//=======================================================================
//function : BSpline
//purpose  :
//=======================================================================

Handle(Geom_BSplineCurve)
     BCurveTool::BSpline (const BRepAdaptor_Curve& C)
{ return(C.BSpline()); }
