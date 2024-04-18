// Created on: 1992-10-14
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef asiAlgo_HLRBRep_CInter_HeaderFile
#define asiAlgo_HLRBRep_CInter_HeaderFile

#include <asiAlgo_HlrIntConicCurveOfCInter.h>
#include <asiAlgo_HlrIntPCurvePCurveOfCInter.h>

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Real.hxx>
#include <IntCurve_IntConicConic.hxx>
#include <IntRes2d_Intersection.hxx>
#include <Standard_Address.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Precision.hxx>

class Standard_ConstructionError;
class IntRes2d_Domain;

namespace asiAlgo {
namespace hlr {

class CurveTool;
class ProjPCurOfCInter;
class CurveLocatorOfProjPCurOfCInter;
class LocateExtPCOfProjPCurOfCInter;
class IntConicCurveOfCInter;
class IntersectorOfIntConicCurveOfCInter;
class IntConicCurveOfCInter;
class IntPCurvePCurveOfCInter;
class Polygon2dOfIntPCurvePCurveOfCInter;
class DistBetweenPCurvesOfIntPCurvePCurveOfCInter;
class ExactIntersectionPointOfIntPCurvePCurveOfCInter;

class CInter  : public IntRes2d_Intersection
{
public:

  DEFINE_STANDARD_ALLOC


  //! Empty constructor.
  asiAlgo_EXPORT CInter();

  //! Self Intersection of a curve
  asiAlgo_EXPORT CInter(const Standard_Address& C, const Standard_Real TolConf, const Standard_Real Tol);

  //! Self Intersection of a curve with a domain.
  asiAlgo_EXPORT CInter(const Standard_Address& C, const IntRes2d_Domain& D, const Standard_Real TolConf, const Standard_Real Tol);

  //! Intersection between 2 curves.
  asiAlgo_EXPORT CInter(const Standard_Address& C1, const Standard_Address& C2, const Standard_Real TolConf, const Standard_Real Tol);

  //! Intersection between 2 curves.
  asiAlgo_EXPORT CInter(const Standard_Address& C1, const IntRes2d_Domain& D1, const Standard_Address& C2, const Standard_Real TolConf, const Standard_Real Tol);

  //! Intersection between 2 curves.
  asiAlgo_EXPORT CInter(const Standard_Address& C1, const Standard_Address& C2, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol);

  //! Intersection between 2 curves.
  asiAlgo_EXPORT CInter(const Standard_Address& C1, const IntRes2d_Domain& D1, const Standard_Address& C2, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol);

  //! Intersection between 2 curves.
  asiAlgo_EXPORT void Perform (const Standard_Address& C1, const IntRes2d_Domain& D1, const Standard_Address& C2, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol);

  //! Intersection between 2 curves.
  asiAlgo_EXPORT void Perform (const Standard_Address& C1, const Standard_Address& C2, const Standard_Real TolConf, const Standard_Real Tol);

  //! Intersection between 2 curves.
  asiAlgo_EXPORT void Perform (const Standard_Address& C1, const IntRes2d_Domain& D1, const Standard_Real TolConf, const Standard_Real Tol);

  //! Intersection between 2 curves.
  asiAlgo_EXPORT void Perform (const Standard_Address& C1, const Standard_Real TolConf, const Standard_Real Tol);

  //! Intersection between 2 curves.
  asiAlgo_EXPORT void Perform (const Standard_Address& C1, const IntRes2d_Domain& D1, const Standard_Address& C2, const Standard_Real TolConf, const Standard_Real Tol);

  //! Intersection between 2 curves.
  asiAlgo_EXPORT void Perform (const Standard_Address& C1, const Standard_Address& C2, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol);

  //! Create a domain from a curve
  asiAlgo_EXPORT IntRes2d_Domain ComputeDomain (const Standard_Address& C1, const Standard_Real TolDomain) const;

  //! Set / get minimum number of points in polygon intersection.
  asiAlgo_EXPORT void SetMinNbSamples (const Standard_Integer theMinNbSamples);
  asiAlgo_EXPORT Standard_Integer GetMinNbSamples () const;

private:

  //! Intersection between 2 curves.
  asiAlgo_EXPORT void InternalPerform (const Standard_Address& C1, const IntRes2d_Domain& D1, const Standard_Address& C2, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol, const Standard_Boolean Composite);

  //! Part of InternalCompositePerform function
  asiAlgo_EXPORT void InternalCompositePerform_noRecurs (const Standard_Integer NbInterC1, const Standard_Address& C1, const Standard_Integer NumInterC1, const TColStd_Array1OfReal& Tab1, const IntRes2d_Domain& D1, const Standard_Integer NbInterC2, const Standard_Address& C2, const Standard_Integer NumInterC2, const TColStd_Array1OfReal& Tab2, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol);

  //! Intersection between 2 curves.
  asiAlgo_EXPORT void InternalCompositePerform (const Standard_Address& C1, const IntRes2d_Domain& D1, const Standard_Integer N1, const Standard_Integer NB1, const TColStd_Array1OfReal& Tab1, const Standard_Address& C2, const IntRes2d_Domain& D2, const Standard_Integer N2, const Standard_Integer NB2, const TColStd_Array1OfReal& Tab2, const Standard_Real TolConf, const Standard_Real Tol, const Standard_Boolean Composite);

  Standard_Real param1inf;
  Standard_Real param1sup;
  Standard_Real param2inf;
  Standard_Real param2sup;
  IntCurve_IntConicConic intconiconi;
  IntConicCurveOfCInter intconicurv;
  IntPCurvePCurveOfCInter intcurvcurv;

};

//----------------------------------------------------------------------
  inline CInter::CInter () :
          param1inf(-Precision::Infinite()),
          param1sup(Precision::Infinite()),
          param2inf(-Precision::Infinite()),
          param2sup(Precision::Infinite())
  {
  }
//----------------------------------------------------------------------
  inline CInter::CInter(const Standard_Address& C,
                                                              const Standard_Real TolConf,
                                                              const Standard_Real Tol) :
          param1inf(-Precision::Infinite()),
          param1sup(Precision::Infinite()),
          param2inf(-Precision::Infinite()),
          param2sup(Precision::Infinite())
  {
    Perform(C,TolConf,Tol);
  }
//----------------------------------------------------------------------
  inline CInter::CInter(const Standard_Address& C,
                                                              const IntRes2d_Domain& D,
                                                              const Standard_Real TolConf,
                                                              const Standard_Real Tol) :
          param1inf(-Precision::Infinite()),
          param1sup(Precision::Infinite()),
          param2inf(-Precision::Infinite()),
          param2sup(Precision::Infinite())
  {
    Perform(C,D,TolConf,Tol);
  }
//----------------------------------------------------------------------
  inline CInter::CInter(const Standard_Address& C1,
                                                              const Standard_Address& C2,
                                                              const Standard_Real TolConf,
                                                              const Standard_Real Tol) :
          param1inf(-Precision::Infinite()),
          param1sup(Precision::Infinite()),
          param2inf(-Precision::Infinite()),
          param2sup(Precision::Infinite())
  {
    Perform(C1,C2,TolConf,Tol);
  }
//----------------------------------------------------------------------
  inline CInter::CInter (const Standard_Address& C1,
                                                               const IntRes2d_Domain& D1,
                                                               const Standard_Address& C2,
                                                               const Standard_Real TolConf,
                                                               const Standard_Real Tol) :
          param1inf(-Precision::Infinite()),
          param1sup(Precision::Infinite()),
          param2inf(-Precision::Infinite()),
          param2sup(Precision::Infinite())
  {
    Perform(C1,D1,C2,TolConf,Tol);
  }
//----------------------------------------------------------------------
  inline CInter::CInter (const Standard_Address& C1,
                                                               const Standard_Address& C2,
                                                               const IntRes2d_Domain& D2,
                                                               const Standard_Real TolConf,
                                                               const Standard_Real Tol) :
          param1inf(-Precision::Infinite()),
          param1sup(Precision::Infinite()),
          param2inf(-Precision::Infinite()),
          param2sup(Precision::Infinite())
  {
    Perform(C1,C2,D2,TolConf,Tol);
  }
//----------------------------------------------------------------------
  inline CInter::CInter (const Standard_Address& C1,
                                                               const IntRes2d_Domain& D1,
                                                               const Standard_Address& C2,
                                                               const IntRes2d_Domain& D2,
                                                               const Standard_Real TolConf,
                                                               const Standard_Real Tol) :
          param1inf(-Precision::Infinite()),
          param1sup(Precision::Infinite()),
          param2inf(-Precision::Infinite()),
          param2sup(Precision::Infinite())
  {
    Perform(C1,D1,C2,D2,TolConf,Tol);
  }
//---------------------------------------------------------------------
  inline void CInter::Perform(const Standard_Address& C1,
                                                 const Standard_Address& C2,
                                                 const Standard_Real TolConf,
                                                 const Standard_Real Tol) {
    Standard_Real TolDomain = Tol;
    if(TolConf>TolDomain) { TolDomain = TolConf; }
    Perform(C1,ComputeDomain(C1,TolDomain),C2,ComputeDomain(C2,TolDomain),TolConf,Tol);
  }
//----------------------------------------------------------------------
  inline void CInter::Perform(const Standard_Address& C1,
                                                 const IntRes2d_Domain& D1,
                                                 const Standard_Address& C2,
                                                 const Standard_Real TolConf,
                                                 const Standard_Real Tol) {
    Standard_Real TolDomain = Tol;
    if(TolConf>TolDomain) { TolDomain = TolConf; }
    Perform(C1,D1,C2,ComputeDomain(C2,TolDomain),TolConf,Tol);
  }
//----------------------------------------------------------------------
  inline void CInter::Perform(const Standard_Address& C1,
                                                 const Standard_Address& C2,
                                                 const IntRes2d_Domain& D2,
                                                 const Standard_Real TolConf,
                                                 const Standard_Real Tol) {
    Standard_Real TolDomain = Tol;
    if(TolConf>TolDomain) { TolDomain = TolConf; }
    Perform(C1,ComputeDomain(C1,TolDomain),C2,D2,TolConf,Tol);
  }

}
}

#endif // _HLRBRep_CInter_HeaderFile
