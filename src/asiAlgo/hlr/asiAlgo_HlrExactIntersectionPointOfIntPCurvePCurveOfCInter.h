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

#ifndef asiAlgo_HLRBRep_ExactIntersectionPointOfTheIntPCurvePCurveOfCInter_HeaderFile
#define asiAlgo_HLRBRep_ExactIntersectionPointOfTheIntPCurvePCurveOfCInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <asiAlgo_HlrDistBetweenPCurvesOfIntPCurvePCurveOfCInter.h>
#include <math_Vector.hxx>
#include <Standard_Address.hxx>

namespace asiAlgo {
namespace hlr {

class CurveTool;
class ProjPCurOfCInter;
class IntPCurvePCurveOfCInter;
class Polygon2dOfIntPCurvePCurveOfCInter;
class DistBetweenPCurvesOfIntPCurvePCurveOfCInter;

class ExactIntersectionPointOfIntPCurvePCurveOfCInter
{
public:

  DEFINE_STANDARD_ALLOC


  asiAlgo_EXPORT ExactIntersectionPointOfIntPCurvePCurveOfCInter(const Standard_Address& C1, const Standard_Address& C2, const Standard_Real Tol);

  asiAlgo_EXPORT void Perform (const Polygon2dOfIntPCurvePCurveOfCInter& Poly1, const Polygon2dOfIntPCurvePCurveOfCInter& Poly2, Standard_Integer& NumSegOn1, Standard_Integer& NumSegOn2, Standard_Real& ParamOnSeg1, Standard_Real& ParamOnSeg2);

  asiAlgo_EXPORT void Perform (const Standard_Real Uo, const Standard_Real Vo, const Standard_Real UInf, const Standard_Real VInf, const Standard_Real USup, const Standard_Real VSup);

  asiAlgo_EXPORT Standard_Integer NbRoots() const;

  asiAlgo_EXPORT void Roots (Standard_Real& U, Standard_Real& V);

  asiAlgo_EXPORT Standard_Boolean AnErrorOccurred() const;

private:


  asiAlgo_EXPORT void MathPerform();


  Standard_Boolean done;
  Standard_Integer nbroots;
  Standard_Real myTol;
  DistBetweenPCurvesOfIntPCurvePCurveOfCInter FctDist;
  math_Vector ToleranceVector;
  math_Vector BInfVector;
  math_Vector BSupVector;
  math_Vector StartingPoint;
  math_Vector Root;
  Standard_Boolean anErrorOccurred;

};

}
}

#endif // _HLRBRep_ExactIntersectionPointOfTheIntPCurvePCurveOfCInter_HeaderFile
