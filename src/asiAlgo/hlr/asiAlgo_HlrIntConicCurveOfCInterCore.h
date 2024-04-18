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

#ifndef asiAlgo_HLRBRep_TheIntConicCurveOfCInterCore_HeaderFile
#define asiAlgo_HLRBRep_TheIntConicCurveOfCInterCore_HeaderFile

// asiAlgo includes
#include <asiAlgo_HlrIntersectorOfIntConicCurveOfCInter.h>

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <IntRes2d_Intersection.hxx>
#include <IntRes2d_Domain.hxx>
#include <IntCurve_IConicTool.hxx>
#include <Standard_Address.hxx>
#include <Standard_Real.hxx>

class gp_Lin2d;
class gp_Circ2d;
class gp_Elips2d;
class gp_Parab2d;
class gp_Hypr2d;

namespace asiAlgo {
namespace hlr {

  class CurveTool;
  class ProjPCurOfCInter;
  class IntersectorOfIntConicCurveOfCInter;
  class ImpParToolOfIntersectorOfIntConicCurveOfCInter;

  class IntConicCurveOfCInterCore : public IntRes2d_Intersection {
  public:

    DEFINE_STANDARD_ALLOC


    //! Empty constructor.
    asiAlgo_EXPORT IntConicCurveOfCInterCore();

    //! Intersection between a line and a parametric curve.
    asiAlgo_EXPORT IntConicCurveOfCInterCore(const gp_Lin2d &L, const IntRes2d_Domain &D1,
                                                    const Standard_Address &PCurve, const IntRes2d_Domain &D2,
                                                    const Standard_Real TolConf, const Standard_Real Tol);

    //! Intersection between a line and a parametric curve.
    asiAlgo_EXPORT IntConicCurveOfCInterCore(const gp_Circ2d &C, const IntRes2d_Domain &D1,
                                                    const Standard_Address &PCurve, const IntRes2d_Domain &D2,
                                                    const Standard_Real TolConf, const Standard_Real Tol);

    //! Intersection between an ellipse and a parametric curve.
    asiAlgo_EXPORT IntConicCurveOfCInterCore(const gp_Elips2d &E, const IntRes2d_Domain &D1,
                                                    const Standard_Address &PCurve, const IntRes2d_Domain &D2,
                                                    const Standard_Real TolConf, const Standard_Real Tol);

    //! Intersection between a parabola and a parametric curve.
    asiAlgo_EXPORT IntConicCurveOfCInterCore(const gp_Parab2d &Prb, const IntRes2d_Domain &D1,
                                                    const Standard_Address &PCurve, const IntRes2d_Domain &D2,
                                                    const Standard_Real TolConf, const Standard_Real Tol);

    //! Intersection between the main branch of an hyperbola
    //! and a parametric curve.
    asiAlgo_EXPORT IntConicCurveOfCInterCore(const gp_Hypr2d &H, const IntRes2d_Domain &D1,
                                                    const Standard_Address &PCurve, const IntRes2d_Domain &D2,
                                                    const Standard_Real TolConf, const Standard_Real Tol);

    //! Intersection between a line and a parametric curve.
    asiAlgo_EXPORT void
    Perform(const gp_Lin2d &L, const IntRes2d_Domain &D1, const Standard_Address &PCurve, const IntRes2d_Domain &D2,
            const Standard_Real TolConf, const Standard_Real Tol);

    //! Intersection between a line and a parametric curve.
    asiAlgo_EXPORT void
    Perform(const gp_Circ2d &C, const IntRes2d_Domain &D1, const Standard_Address &PCurve, const IntRes2d_Domain &D2,
            const Standard_Real TolConf, const Standard_Real Tol);

    //! Intersection between an ellipse and a parametric curve.
    asiAlgo_EXPORT void
    Perform(const gp_Elips2d &E, const IntRes2d_Domain &D1, const Standard_Address &PCurve, const IntRes2d_Domain &D2,
            const Standard_Real TolConf, const Standard_Real Tol);

    //! Intersection between a parabola and a parametric curve.
    asiAlgo_EXPORT void
    Perform(const gp_Parab2d &Prb, const IntRes2d_Domain &D1, const Standard_Address &PCurve, const IntRes2d_Domain &D2,
            const Standard_Real TolConf, const Standard_Real Tol);

    //! Intersection between the main branch of an hyperbola
    //! and a parametric curve.
    asiAlgo_EXPORT void
    Perform(const gp_Hypr2d &H, const IntRes2d_Domain &D1, const Standard_Address &PCurve, const IntRes2d_Domain &D2,
            const Standard_Real TolConf, const Standard_Real Tol);

  private:

    //! Intersection between a conic fom gp
    //! and a parametric curve.
    asiAlgo_EXPORT void
    Perform(const IntCurve_IConicTool &ICurve, const IntRes2d_Domain &D1, const Standard_Address &PCurve,
            const IntRes2d_Domain &D2, const Standard_Real TolConf, const Standard_Real Tol);

  };

//--------------------------------------------------------------------------------
  inline IntConicCurveOfCInterCore::IntConicCurveOfCInterCore()
  { }

//--------------------------------------------------------------------------------
  inline IntConicCurveOfCInterCore::IntConicCurveOfCInterCore(const gp_Lin2d& L,
                                                               const IntRes2d_Domain& D1,
                                                               const Standard_Address& PCurve,
                                                               const IntRes2d_Domain& D2,
                                                               const Standard_Real TolConf,
                                                               const Standard_Real Tol) {

    Perform(IntCurve_IConicTool(L),D1,PCurve,D2,TolConf,Tol);
  }


  inline void IntConicCurveOfCInterCore::Perform (const gp_Lin2d& L,
                                                  const IntRes2d_Domain& D1,
                                                  const Standard_Address& PCurve,
                                                  const IntRes2d_Domain& D2,
                                                  const Standard_Real TolConf,
                                                  const Standard_Real Tol) {

    Perform(IntCurve_IConicTool(L),D1,PCurve,D2,TolConf,Tol);
  }
//--------------------------------------------------------------------------------
  inline void IntConicCurveOfCInterCore::Perform (const gp_Circ2d& C,
                                                  const IntRes2d_Domain& D1,
                                                  const Standard_Address& PCurve,
                                                  const IntRes2d_Domain& D2,
                                                  const Standard_Real TolConf,
                                                  const Standard_Real Tol) {
    if(!D1.IsClosed()) {
      IntRes2d_Domain D(D1);
      D.SetEquivalentParameters(D1.FirstParameter(),D1.FirstParameter()+M_PI+M_PI);
      Perform(IntCurve_IConicTool(C),D,PCurve,D2,TolConf,Tol);
      //-- throw Standard_ConstructionError("Domaine incorrect");
    }
    else {
      Perform(IntCurve_IConicTool(C),D1,PCurve,D2,TolConf,Tol);
    }
  }
//--------------------------------------------------------------------------------
  inline void IntConicCurveOfCInterCore::Perform (const gp_Elips2d& E,
                                                  const IntRes2d_Domain& D1,
                                                  const Standard_Address& PCurve,
                                                  const IntRes2d_Domain& D2,
                                                  const Standard_Real TolConf,
                                                  const Standard_Real Tol) {
    if(!D1.IsClosed()) {
      IntRes2d_Domain D(D1);
      D.SetEquivalentParameters(D1.FirstParameter(),D1.FirstParameter()+M_PI+M_PI);
      Perform(IntCurve_IConicTool(E),D,PCurve,D2,TolConf,Tol);
    }
    else {
      Perform(IntCurve_IConicTool(E),D1,PCurve,D2,TolConf,Tol);
    }
  }
//--------------------------------------------------------------------------------
  inline void IntConicCurveOfCInterCore::Perform (const gp_Parab2d& Prb,
                                                  const IntRes2d_Domain& D1,
                                                  const Standard_Address& PCurve,
                                                  const IntRes2d_Domain& D2,
                                                  const Standard_Real TolConf,
                                                  const Standard_Real Tol) {
    Perform(IntCurve_IConicTool(Prb),D1,PCurve,D2,TolConf,Tol);
  }
//--------------------------------------------------------------------------------
  inline void IntConicCurveOfCInterCore::Perform (const gp_Hypr2d& H,
                                                  const IntRes2d_Domain& D1,
                                                  const Standard_Address& PCurve,
                                                  const IntRes2d_Domain& D2,
                                                  const Standard_Real TolConf,
                                                  const Standard_Real Tol) {
    Perform(IntCurve_IConicTool(H),D1,PCurve,D2,TolConf,Tol);
  }
//--------------------------------------------------------------------------------
  inline void IntConicCurveOfCInterCore::Perform (const IntCurve_IConicTool& ICurve,
                                                  const IntRes2d_Domain& D1,
                                                  const Standard_Address& PCurve,
                                                  const IntRes2d_Domain& D2,
                                                  const Standard_Real TolConf,
                                                  const Standard_Real Tol) {
    IntersectorOfIntConicCurveOfCInter myintersection;
    myintersection.SetReversedParameters(ReversedParameters());
    myintersection.Perform(ICurve,D1,PCurve,D2,TolConf,Tol);
    this->SetValues(myintersection);
  }
//================================================================================

}
}

#endif
