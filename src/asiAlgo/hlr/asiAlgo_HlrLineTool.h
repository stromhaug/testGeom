// Created on: 1993-08-18
// Created by: Christophe MARION
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef asiAlgo_HLRBRep_LineTool_HeaderFile
#define asiAlgo_HLRBRep_LineTool_HeaderFile

// asiAlgo includes
#include <asiAlgo.h>

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Boolean.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <GeomAbs_CurveType.hxx>
#include <gp_Lin.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Parab.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <ElCLib.hxx>

class Standard_OutOfRange;
class Standard_NoSuchObject;
class Standard_DomainError;
class gp_Lin;
class gp_Pnt;
class gp_Vec;
class Geom_BezierCurve;
class Geom_BSplineCurve;

namespace asiAlgo {
namespace hlr {

//! The  LineTool  class  provides  class  methods to
//! access the methodes of the Line.
class LineTool
{
public:

  DEFINE_STANDARD_ALLOC


  asiAlgo_EXPORT static Standard_Real FirstParameter (const gp_Lin& C);

  asiAlgo_EXPORT static Standard_Real LastParameter (const gp_Lin& C);

  asiAlgo_EXPORT static GeomAbs_Shape Continuity (const gp_Lin& C);

  //! If necessary,   breaks the line  in  intervals of
  //! continuity <S>.     And   returns  the  number  of
  //! intervals.
  asiAlgo_EXPORT static Standard_Integer NbIntervals (const gp_Lin& C, const GeomAbs_Shape S);

  //! Sets the current working interval.
  asiAlgo_EXPORT static void Intervals (const gp_Lin& C, TColStd_Array1OfReal& T, const GeomAbs_Shape Sh);

  //! Returns  the  first  parameter    of  the  current
  //! interval.
  asiAlgo_EXPORT static Standard_Real IntervalFirst (const gp_Lin& C);

  //! Returns  the  last  parameter    of  the  current
  //! interval.
  asiAlgo_EXPORT static Standard_Real IntervalLast (const gp_Lin& C);

  asiAlgo_EXPORT static GeomAbs_Shape IntervalContinuity (const gp_Lin& C);

  asiAlgo_EXPORT static Standard_Boolean IsClosed (const gp_Lin& C);

  asiAlgo_EXPORT static Standard_Boolean IsPeriodic (const gp_Lin& C);

  asiAlgo_EXPORT static Standard_Real Period (const gp_Lin& C);

  //! Computes the point of parameter U on the line.
  asiAlgo_EXPORT static gp_Pnt Value (const gp_Lin& C, const Standard_Real U);

  //! Computes the point of parameter U on the line.
  asiAlgo_EXPORT static void D0 (const gp_Lin& C, const Standard_Real U, gp_Pnt& P);

  //! Computes the point of parameter U on the line with its
  //! first derivative.
  //! Raised if the continuity of the current interval
  //! is not C1.
  asiAlgo_EXPORT static void D1 (const gp_Lin& C, const Standard_Real U, gp_Pnt& P, gp_Vec& V);

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  //! Raised if the continuity of the current interval
  //! is not C2.
  asiAlgo_EXPORT static void D2 (const gp_Lin& C, const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2);

  //! Returns the point P of parameter U, the first, the second
  //! and the third derivative.
  //! Raised if the continuity of the current interval
  //! is not C3.
  asiAlgo_EXPORT static void D3 (const gp_Lin& C, const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3);

  //! The returned vector gives the value of the derivative for the
  //! order of derivation N.
  //! Raised if the continuity of the current interval
  //! is not CN.
  //! Raised if N < 1.
  asiAlgo_EXPORT static gp_Vec DN (const gp_Lin& C, const Standard_Real U, const Standard_Integer N);

  //! Returns the parametric  resolution corresponding
  //! to the real space resolution <R3d>.
  asiAlgo_EXPORT static Standard_Real Resolution (const gp_Lin& C, const Standard_Real R3d);

  //! Returns  the  type of the   line  in the  current
  //! interval :   Line,   Circle,   Ellipse, Hyperbola,
  //! Parabola, BezierCurve, BSplineCurve, OtherCurve.
  asiAlgo_EXPORT static GeomAbs_CurveType GetType (const gp_Lin& C);

  asiAlgo_EXPORT static gp_Lin Line (const gp_Lin& C);

  asiAlgo_EXPORT static gp_Circ Circle (const gp_Lin& C);

  asiAlgo_EXPORT static gp_Elips Ellipse (const gp_Lin& C);

  asiAlgo_EXPORT static gp_Hypr Hyperbola (const gp_Lin& C);

  asiAlgo_EXPORT static gp_Parab Parabola (const gp_Lin& C);

  asiAlgo_EXPORT static Handle(Geom_BezierCurve) Bezier (const gp_Lin& C);

  asiAlgo_EXPORT static Handle(Geom_BSplineCurve) BSpline (const gp_Lin& C);

  asiAlgo_EXPORT static Standard_Integer Degree (const gp_Lin& C);

  asiAlgo_EXPORT static Standard_Integer NbPoles (const gp_Lin& C);

  asiAlgo_EXPORT static void Poles (const gp_Lin& C, TColgp_Array1OfPnt& TP);

  asiAlgo_EXPORT static Standard_Boolean IsRational (const gp_Lin& C);

  asiAlgo_EXPORT static void PolesAndWeights (const gp_Lin& C, TColgp_Array1OfPnt& TP, TColStd_Array1OfReal& TW);

  asiAlgo_EXPORT static Standard_Integer NbKnots (const gp_Lin& C);

  asiAlgo_EXPORT static void KnotsAndMultiplicities (const gp_Lin& C, TColStd_Array1OfReal& TK, TColStd_Array1OfInteger& TM);

  asiAlgo_EXPORT static Standard_Integer NbSamples (const gp_Lin& C, const Standard_Real U0, const Standard_Real U1);

  asiAlgo_EXPORT static void SamplePars (const gp_Lin& C, const Standard_Real U0, const Standard_Real U1, const Standard_Real Defl, const Standard_Integer NbMin, Handle(TColStd_HArray1OfReal)& Pars);

};


inline Standard_Real LineTool::FirstParameter(const gp_Lin&)
{ return RealFirst(); }

inline Standard_Real LineTool::LastParameter(const gp_Lin& ) 
{ return RealLast(); }

inline GeomAbs_Shape LineTool::Continuity( const gp_Lin& )
{ return GeomAbs_CN; }

inline Standard_Integer LineTool::NbIntervals
  (const gp_Lin& , const GeomAbs_Shape )
{ return 1; }

inline void LineTool::Intervals
  (const gp_Lin& , TColStd_Array1OfReal& , const GeomAbs_Shape)
{}

inline Standard_Real LineTool::IntervalFirst(const gp_Lin& )
{ return RealFirst(); }

inline Standard_Real LineTool::IntervalLast(const gp_Lin& )
{ return RealLast(); }

inline GeomAbs_Shape LineTool::IntervalContinuity(const gp_Lin& )
{ return GeomAbs_CN; }

inline Standard_Boolean LineTool::IsClosed( const gp_Lin& )
{ return Standard_False; }

inline Standard_Boolean LineTool::IsPeriodic( const gp_Lin& )
{ return Standard_False; }

inline Standard_Real LineTool::Period(const gp_Lin& )
{ return 0.; }

inline gp_Pnt LineTool::Value(const gp_Lin& C,const Standard_Real U)
{ return ElCLib::Value(U,C); }

inline void LineTool::D0
  (const gp_Lin& C,const Standard_Real U, gp_Pnt& P)
{ P = ElCLib::Value(U,C); }

inline void LineTool::D1
  (const gp_Lin& C,const Standard_Real U, 
   gp_Pnt& P, gp_Vec& V)
{ ElCLib::D1(U,C,P,V); }

inline void LineTool::D2
  (const gp_Lin& C, const Standard_Real U,
   gp_Pnt& P, gp_Vec& V1, gp_Vec& V2)
{ 
  ElCLib::D1(U,C,P,V1);
  V2 = gp_Vec(0,0,0);
}

inline void LineTool::D3
  (const gp_Lin& C, const Standard_Real U,
   gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3)
{
  ElCLib::D1(U,C,P,V1);
  V2 = V3 = gp_Vec(0,0,0);
}

inline gp_Vec LineTool::DN
  (const gp_Lin& C, const Standard_Real U, const Standard_Integer N)
{ return ElCLib::DN(U,C,N); }

inline Standard_Real LineTool::Resolution
  ( const gp_Lin& , const Standard_Real R3D)
{ return R3D; }

inline GeomAbs_CurveType LineTool::GetType(const gp_Lin& )
{ return GeomAbs_Line; }

inline gp_Lin LineTool::Line(const gp_Lin& C)  
{ return C; }

inline gp_Circ LineTool::Circle(const gp_Lin&) 
{ return gp_Circ(); }

inline gp_Elips LineTool::Ellipse(const gp_Lin&) 
{ return gp_Elips(); }

inline gp_Hypr LineTool::Hyperbola(const gp_Lin&)  
{ return gp_Hypr(); }

inline gp_Parab LineTool::Parabola(const gp_Lin&) 
{ return gp_Parab(); }

inline Handle(Geom_BezierCurve) LineTool::Bezier(const gp_Lin&) 
{
  Handle(Geom_BezierCurve) NullCurve;
  return NullCurve;
}

inline Handle(Geom_BSplineCurve) LineTool::BSpline(const gp_Lin&) 
{
  Handle(Geom_BSplineCurve) NullCurve;
  return NullCurve;
}

inline Standard_Integer LineTool::Degree(const gp_Lin& )
{ return 1; }

inline Standard_Integer LineTool::NbPoles(const gp_Lin&)
{ return 0; }

inline void LineTool::Poles(const gp_Lin&, TColgp_Array1OfPnt&)
{}

inline Standard_Boolean LineTool::IsRational(const gp_Lin&)
{ return Standard_False; }

inline void LineTool::PolesAndWeights
  (const gp_Lin&, TColgp_Array1OfPnt&, TColStd_Array1OfReal&)
{}

inline Standard_Integer LineTool::NbKnots(const gp_Lin&)
{ return 0; }

inline void LineTool::KnotsAndMultiplicities
  (const gp_Lin&, TColStd_Array1OfReal&, TColStd_Array1OfInteger&)
{}

inline Standard_Integer LineTool::NbSamples(const gp_Lin&,
						    const Standard_Real,
						    const Standard_Real ) 
{ return 3; }

// modified by NIZHNY-MKK  Tue Nov  1 18:49:28 2005
inline void LineTool::SamplePars (const gp_Lin&,
					  const Standard_Real U0,
					  const Standard_Real U1,
					  const Standard_Real,
					  const Standard_Integer,
					  Handle(TColStd_HArray1OfReal)& Pars) {

  Pars = new TColStd_HArray1OfReal(1, 3);
  Pars->SetValue(1, U0);
  Pars->SetValue(2, (U0 + U1) * 0.5);
  Pars->SetValue(3, U1);
}

}
}

#endif // _HLRBRep_LineTool_HeaderFile
