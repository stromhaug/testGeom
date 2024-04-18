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

// Own include
#include <asiAlgo_HlrCurveTool.h>

// asiAlgo includes
#include <asiAlgo_HlrCurve.h>

// OpenCascade includes
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColStd_Array1OfReal.hxx>

#define   IS_C2_COMPOSITE 0

using namespace asiAlgo::hlr;

//-----------------------------------------------------------------------------


 Standard_Real
CurveTool::FirstParameter (const Standard_Address C)
{ return(((Curve *)C)->FirstParameter()); }

//=======================================================================
//function : LastParameter
//purpose  :
//=======================================================================

 Standard_Real
CurveTool::LastParameter (const Standard_Address C)
{ return(((Curve *)C)->LastParameter()); }

//=======================================================================
//function : Continuity
//purpose  :
//=======================================================================

 GeomAbs_Shape
CurveTool::Continuity (const Standard_Address C)
{ return(((Curve *)C)->Continuity()); }

//=======================================================================
//function : NbIntervals
//purpose  :
//=======================================================================

 Standard_Integer
CurveTool::NbIntervals(const Standard_Address C)
{
#if IS_C2_COMPOSITE
  return(((Curve *)C)->NbIntervals(GeomAbs_C2));
#else
  return(((Curve *)C)->NbIntervals(GeomAbs_C1));
#endif
}

//=======================================================================
//function : Intervals
//purpose  :
//=======================================================================

 void
CurveTool::Intervals(const Standard_Address C,
			     TColStd_Array1OfReal& Tab)
{
#if IS_C2_COMPOSITE
  ((Curve *)C)->Intervals(Tab,GeomAbs_C2);
#else
  ((Curve *)C)->Intervals(Tab,GeomAbs_C1);
#endif
}

//=======================================================================
//function : GetInterval
//purpose  :
//=======================================================================

 void
CurveTool::GetInterval(const Standard_Address /*C*/,
			       const Standard_Integer i,
			       const TColStd_Array1OfReal& Tab,
			       Standard_Real& a,
			       Standard_Real& b)
{
  a = Tab.Value(i);
  b = Tab.Value(i+1);
}

//=======================================================================
//function : IsClosed
//purpose  :
//=======================================================================

 Standard_Boolean
CurveTool::IsClosed(const Standard_Address C)
{ return(((Curve *)C)->IsClosed()); }

//=======================================================================
//function : IsPeriodic
//purpose  :
//=======================================================================

 Standard_Boolean
CurveTool::IsPeriodic(const Standard_Address C)
{ return(((Curve *)C)->IsPeriodic()); }

//=======================================================================
//function : Period
//purpose  :
//=======================================================================

 Standard_Real
CurveTool::Period(const Standard_Address C)
{ return(((Curve *)C)->Period()); }

//=======================================================================
//function : Value
//purpose  :
//=======================================================================

 gp_Pnt2d
CurveTool::Value (const Standard_Address C,
			  const Standard_Real U)
{ return(((Curve *)C)->Value(U)); }

//=======================================================================
//function : D0
//purpose  :
//=======================================================================

 void
CurveTool::D0(const Standard_Address C,
		      const Standard_Real U,
		      gp_Pnt2d& P)
{ ((Curve *)C)->D0(U,P); }

//=======================================================================
//function : D1
//purpose  :
//=======================================================================

 void
CurveTool::D1 (const Standard_Address C,
		       const Standard_Real U,
		       gp_Pnt2d& P,
		       gp_Vec2d& T)
{ ((Curve *)C)->D1(U,P,T); }

//=======================================================================
//function : D2
//purpose  :
//=======================================================================

 void
CurveTool::D2 (const Standard_Address C,
		       const Standard_Real U,
		       gp_Pnt2d& P,
		       gp_Vec2d& T,
		       gp_Vec2d& N)
{ ((Curve *)C)->D2(U,P,T,N); }

//=======================================================================
//function : D3
//purpose  :
//=======================================================================

 void
CurveTool::D3 (const Standard_Address C,
		       const Standard_Real U,
		       gp_Pnt2d& P,
		       gp_Vec2d& V1,
		       gp_Vec2d& V2,
		       gp_Vec2d& V3)
{ ((Curve *)C)->D3(U,P,V1,V2,V3); }

//=======================================================================
//function : DN
//purpose  :
//=======================================================================

 gp_Vec2d
CurveTool::DN (const Standard_Address C,
		       const Standard_Real U,
		       const Standard_Integer N)
{ return(((Curve *)C)->DN(U,N)); }

//=======================================================================
//function : Resolution
//purpose  :
//=======================================================================

 Standard_Real
CurveTool::Resolution(const Standard_Address C,
			      const Standard_Real R3d)
{ return(((Curve *)C)->Resolution(R3d)); }

//=======================================================================
//function : GetType
//purpose  :
//=======================================================================

 GeomAbs_CurveType
CurveTool::GetType(const Standard_Address C)
{ return(((Curve *)C)->GetType()); }

//=======================================================================
//function : TheType
//purpose  :
//=======================================================================

 GeomAbs_CurveType
CurveTool::TheType(const Standard_Address C)
{ return(((Curve *)C)->GetType()); }

//=======================================================================
//function : Line
//purpose  :
//=======================================================================

 gp_Lin2d
CurveTool::Line (const Standard_Address C)
{ return(((Curve *)C)->Line()); }

//=======================================================================
//function : Circle
//purpose  :
//=======================================================================

 gp_Circ2d
CurveTool::Circle (const Standard_Address C)
{ return(((Curve *)C)->Circle()); }

//=======================================================================
//function : Ellipse
//purpose  :
//=======================================================================

 gp_Elips2d
CurveTool::Ellipse (const Standard_Address C)
{ return(((Curve *)C)->Ellipse()); }

//=======================================================================
//function : Parabola
//purpose  :
//=======================================================================

 gp_Parab2d
CurveTool::Parabola (const Standard_Address C)
{ return(((Curve *)C)->Parabola()); }

//=======================================================================
//function : Hyperbola
//purpose  :
//=======================================================================

 gp_Hypr2d
CurveTool::Hyperbola (const Standard_Address C)
{ return(((Curve *)C)->Hyperbola()); }

//=======================================================================
//function : Bezier
//purpose  :
//=======================================================================

 Handle(Geom2d_BezierCurve)
     CurveTool::Bezier (const Standard_Address /*C*/)
{
#ifdef OCCT_DEBUG
  std::cout<<"  CurveTool::Bezier : Not Implemented "<<std::endl;
#endif
  //-- return(((Curve *)C)->Bezier());
  return(0);
}

//=======================================================================
//function : BSpline
//purpose  :
//=======================================================================

 Handle(Geom2d_BSplineCurve)
     CurveTool::BSpline (const Standard_Address /*C*/)
{
#ifdef OCCT_DEBUG
  std::cout<<"  CurveTool::BSpline : Not Implemented "<<std::endl;
#endif
  //-- return(((Curve *)C)->BSpline());
  return(0);
}

//=======================================================================
//function : EpsX
//purpose  :
//=======================================================================

 Standard_Real
CurveTool::EpsX(const Standard_Address /*C*/)
{ return(1e-10); }


//=======================================================================
//function : Degree
//purpose  :
//=======================================================================

 Standard_Integer
     CurveTool::Degree (const Standard_Address C)
{
  return(((Curve *)C)->Degree());
}


//=======================================================================
//function : NbSamples
//purpose  :
//=======================================================================
Standard_Integer
CurveTool::NbSamples (const Standard_Address C)
{
  GeomAbs_CurveType typC = ((Curve *)C)->GetType();
  static Standard_Real nbsOther = 10.0;
  Standard_Real nbs = nbsOther;

  if(typC == GeomAbs_Line)
    nbs = 2;
  else if(typC == GeomAbs_BezierCurve)
    nbs = 3 + ((Curve *)C)->NbPoles();
  else if(typC == GeomAbs_BSplineCurve) {
    nbs = ((Curve *)C)->NbKnots();
    nbs*= ((Curve *)C)->Degree();
    if(nbs < 2.0) nbs=2;
  }
  if(nbs>50)
    nbs = 50;
  return((Standard_Integer)nbs);
}

//=======================================================================
//function : NbSamples
//purpose  :
//=======================================================================

Standard_Integer
CurveTool::NbSamples (const Standard_Address C,
                              const Standard_Real u1,
                              const Standard_Real u2)
{
  GeomAbs_CurveType typC = ((Curve *)C)->GetType();
  static Standard_Real nbsOther = 10.0;
  Standard_Real nbs = nbsOther;

  if(typC == GeomAbs_Line)
    nbs = 2;
  else if(typC == GeomAbs_BezierCurve)
    nbs = 3 + ((Curve *)C)->NbPoles();
  else if(typC == GeomAbs_BSplineCurve) {
    Handle(Geom_Curve) aCurve = ((Curve *)C)->AccessCurve().Curve().Curve();
    GeomAdaptor_Curve GAcurve(aCurve, u1, u2);
    nbs = GAcurve.NbIntervals(GeomAbs_CN) + 1;
    nbs*= ((Curve *)C)->Degree();
    if(nbs < 2.0) nbs=2;
  }
  if(nbs>50)
    nbs = 50;
  return((Standard_Integer)nbs);
}
