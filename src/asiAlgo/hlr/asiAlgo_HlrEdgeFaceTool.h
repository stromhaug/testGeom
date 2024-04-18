// Created on: 1993-10-18
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

#ifndef asiAlgo_HLRBRep_EdgeFaceTool_HeaderFile
#define asiAlgo_HLRBRep_EdgeFaceTool_HeaderFile

// asiAlgo includes
#include <asiAlgo.h>

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Address.hxx>
#include <Standard_Boolean.hxx>

class gp_Dir;

namespace asiAlgo {
namespace hlr {

//! The EdgeFaceTool computes the  UV coordinates at a
//! given parameter on a Curve and a Surface.  It also
//! compute the signed  curvature value in a direction
//! at a given u,v point on a surface.
class EdgeFaceTool
{
public:

  DEFINE_STANDARD_ALLOC


  asiAlgo_EXPORT static Standard_Real CurvatureValue (const Standard_Address F, const Standard_Real U, const Standard_Real V, const gp_Dir& Tg);

  //! return True if U and V are found.
  asiAlgo_EXPORT static Standard_Boolean UVPoint (const Standard_Real Par, const Standard_Address E, const Standard_Address F, Standard_Real& U, Standard_Real& V);

};

}
}

#endif // _HLRBRep_EdgeFaceTool_HeaderFile
