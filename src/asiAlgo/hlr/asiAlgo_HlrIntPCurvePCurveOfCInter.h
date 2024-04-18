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

#ifndef asiAlgo_HLRBRep_TheIntPCurvePCurveOfCInter_HeaderFile
#define asiAlgo_HLRBRep_TheIntPCurvePCurveOfCInter_HeaderFile

// asiAlgo includes
#include <asiAlgo.h>

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <IntRes2d_Domain.hxx>
#include <IntRes2d_Intersection.hxx>
#include <Standard_Address.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>

class IntRes2d_Domain;

namespace asiAlgo {
namespace hlr {

class CurveTool;
class ProjPCurOfCInter;
class Polygon2dOfIntPCurvePCurveOfCInter;
class DistBetweenPCurvesOfIntPCurvePCurveOfCInter;
class ExactIntersectionPointOfIntPCurvePCurveOfCInter;

class IntPCurvePCurveOfCInter  : public IntRes2d_Intersection
{
public:

  DEFINE_STANDARD_ALLOC

  asiAlgo_EXPORT IntPCurvePCurveOfCInter();

  asiAlgo_EXPORT void Perform (const Standard_Address& Curve1, const IntRes2d_Domain& Domain1, const Standard_Address& Curve2, const IntRes2d_Domain& Domain2, const Standard_Real TolConf, const Standard_Real Tol);

  asiAlgo_EXPORT void Perform (const Standard_Address& Curve1, const IntRes2d_Domain& Domain1, const Standard_Real TolConf, const Standard_Real Tol);

  //! Set / get minimum number of points in polygon for intersection.
  asiAlgo_EXPORT void SetMinNbSamples (const Standard_Integer theMinNbSamples);
  asiAlgo_EXPORT Standard_Integer GetMinNbSamples () const;

protected:

  asiAlgo_EXPORT void Perform (const Standard_Address& Curve1, const IntRes2d_Domain& Domain1, const Standard_Address& Curve2, const IntRes2d_Domain& Domain2, const Standard_Real TolConf, const Standard_Real Tol, const Standard_Integer NbIter, const Standard_Real DeltaU, const Standard_Real DeltaV);

  asiAlgo_EXPORT void Perform (const Standard_Address& Curve1, const IntRes2d_Domain& Domain1, const Standard_Real TolConf, const Standard_Real Tol, const Standard_Integer NbIter, const Standard_Real DeltaU, const Standard_Real DeltaV);

private:

  //! Method to find intersection between two curves
  //! :  returns false for case when some points of polygon
  //! : were replaced on line and exact point of intersection was not found
  //! : for case when point of intersection was found
  //! : during prelimanary search for line (case of bad paramerization of Bspline for example).
  asiAlgo_EXPORT Standard_Boolean findIntersect (const Standard_Address& Curve1, const IntRes2d_Domain& Domain1, const Standard_Address& Curve2, const IntRes2d_Domain& Domain2, const Standard_Real TolConf, const Standard_Real Tol, const Standard_Integer NbIter, const Standard_Real DeltaU, const Standard_Real DeltaV, const Polygon2dOfIntPCurvePCurveOfCInter& thePoly1, const Polygon2dOfIntPCurvePCurveOfCInter& thePoly2, const Standard_Boolean isFullRepresentation);

  IntRes2d_Domain DomainOnCurve1;
  IntRes2d_Domain DomainOnCurve2;

  //! Minimal number of sample points
  Standard_Integer myMinPntNb;
};

}
}

#endif // _HLRBRep_TheIntPCurvePCurveOfCInter_HeaderFile
