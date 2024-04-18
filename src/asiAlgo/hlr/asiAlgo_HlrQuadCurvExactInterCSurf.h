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

#ifndef asiAlgo_HLRBRep_TheQuadCurvExactInterCSurf_HeaderFile
#define asiAlgo_HLRBRep_TheQuadCurvExactInterCSurf_HeaderFile

// asiAlgo includes
#include <asiAlgo.h>

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <Standard_Address.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>

class gp_Lin;

namespace asiAlgo {
namespace hlr {

class SurfaceTool;
class LineTool;
class QuadCurvFuncOfQuadCurvExactInterCSurf;

class QuadCurvExactInterCSurf
{
public:

  DEFINE_STANDARD_ALLOC


  //! Provides the signed distance function : Q(w)
  //! and its first derivative dQ(w)/dw
  asiAlgo_EXPORT QuadCurvExactInterCSurf(const Standard_Address& S, const gp_Lin& C);

  asiAlgo_EXPORT Standard_Boolean IsDone() const;

  asiAlgo_EXPORT Standard_Integer NbRoots() const;

  asiAlgo_EXPORT Standard_Real Root (const Standard_Integer Index) const;

  asiAlgo_EXPORT Standard_Integer NbIntervals() const;

  //! U1 and U2 are the parameters of
  //! a segment on the curve.
  asiAlgo_EXPORT void Intervals (const Standard_Integer Index, Standard_Real& U1, Standard_Real& U2) const;

private:

  Standard_Integer nbpnts;
  TColStd_SequenceOfReal pnts;
  Standard_Integer nbintv;
  TColStd_SequenceOfReal intv;

};

}
}

#endif // _HLRBRep_TheQuadCurvExactInterCSurf_HeaderFile
