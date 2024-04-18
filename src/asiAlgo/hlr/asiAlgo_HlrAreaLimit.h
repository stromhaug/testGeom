// Created on: 1997-04-17
// Created by: Christophe MARION
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef asiAlgo_HLRBRep_AreaLimit_HeaderFile
#define asiAlgo_HLRBRep_AreaLimit_HeaderFile

// asiAlgo includes
#include <asiAlgo.h>

#include <Standard_Type.hxx>

#include <HLRAlgo_Intersection.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_State.hxx>
#include <Standard_Transient.hxx>

namespace asiAlgo {
namespace hlr {

class AreaLimit;
DEFINE_STANDARD_HANDLE(AreaLimit, Standard_Transient)

//! The  private  nested class AreaLimit represents   a --
//! vertex on  the Edge with the  state on the left and --
//! the right.
class AreaLimit : public Standard_Transient
{
public:

  //! The previous and next field are set to NULL.
  asiAlgo_EXPORT AreaLimit(const HLRAlgo_Intersection& V, const Standard_Boolean Boundary, const Standard_Boolean Interference, const TopAbs_State StateBefore, const TopAbs_State StateAfter, const TopAbs_State EdgeBefore, const TopAbs_State EdgeAfter);

  asiAlgo_EXPORT void StateBefore (const TopAbs_State St);

  asiAlgo_EXPORT void StateAfter (const TopAbs_State St);

  asiAlgo_EXPORT void EdgeBefore (const TopAbs_State St);

  asiAlgo_EXPORT void EdgeAfter (const TopAbs_State St);

  asiAlgo_EXPORT void Previous (const Handle(AreaLimit)& P);

  asiAlgo_EXPORT void Next (const Handle(AreaLimit)& N);

  asiAlgo_EXPORT const HLRAlgo_Intersection& Vertex() const;

  asiAlgo_EXPORT Standard_Boolean IsBoundary() const;

  asiAlgo_EXPORT Standard_Boolean IsInterference() const;

  asiAlgo_EXPORT TopAbs_State StateBefore() const;

  asiAlgo_EXPORT TopAbs_State StateAfter() const;

  asiAlgo_EXPORT TopAbs_State EdgeBefore() const;

  asiAlgo_EXPORT TopAbs_State EdgeAfter() const;

  asiAlgo_EXPORT Handle(AreaLimit) Previous() const;

  asiAlgo_EXPORT Handle(AreaLimit) Next() const;

  asiAlgo_EXPORT void Clear();

  DEFINE_STANDARD_RTTIEXT(AreaLimit,Standard_Transient)

private:

  HLRAlgo_Intersection myVertex;
  Standard_Boolean myBoundary;
  Standard_Boolean myInterference;
  TopAbs_State myStateBefore;
  TopAbs_State myStateAfter;
  TopAbs_State myEdgeBefore;
  TopAbs_State myEdgeAfter;
  Handle(AreaLimit) myPrevious;
  Handle(AreaLimit) myNext;

};

}
}

#endif // _HLRBRep_AreaLimit_HeaderFile
