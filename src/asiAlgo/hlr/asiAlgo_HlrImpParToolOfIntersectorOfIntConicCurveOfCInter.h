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

#ifndef asiAlgo_HLRBRep_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfCInter_HeaderFile
#define asiAlgo_HLRBRep_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfCInter_HeaderFile

// asiAlgo includes
#include <asiAlgo.h>

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Address.hxx>
#include <IntCurve_IConicTool.hxx>
#include <math_FunctionWithDerivative.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>

class IntCurve_IConicTool;

namespace asiAlgo {
namespace hlr {

class CurveTool;

class ImpParToolOfIntersectorOfIntConicCurveOfCInter  : public math_FunctionWithDerivative
{
public:

  DEFINE_STANDARD_ALLOC

  //! Constructor of the class.
  asiAlgo_EXPORT ImpParToolOfIntersectorOfIntConicCurveOfCInter(const IntCurve_IConicTool& IT, const Standard_Address& PC);

  //! Computes the value of the signed distance between
  //! the implicit curve and the point at parameter Param
  //! on the parametrised curve.
  asiAlgo_EXPORT Standard_Boolean Value (const Standard_Real Param, Standard_Real& F) Standard_OVERRIDE;

  //! Computes the derivative of the previous function at
  //! parameter Param.
  asiAlgo_EXPORT Standard_Boolean Derivative (const Standard_Real Param, Standard_Real& D) Standard_OVERRIDE;

  //! Computes the value and the derivative of the function.
  asiAlgo_EXPORT Standard_Boolean Values (const Standard_Real Param, Standard_Real& F, Standard_Real& D) Standard_OVERRIDE;

private:

  Standard_Address TheParCurve;
  IntCurve_IConicTool TheImpTool;

};

}
}

#endif // _HLRBRep_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfCInter_HeaderFile
