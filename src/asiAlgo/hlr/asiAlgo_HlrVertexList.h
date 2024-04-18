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

#ifndef asiAlgo_HLRBRep_VertexList_HeaderFile
#define asiAlgo_HLRBRep_VertexList_HeaderFile

#include <asiAlgo_HlrEdgeInterferenceTool.h>

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <HLRAlgo_ListIteratorOfInterferenceList.hxx>
#include <Standard_Boolean.hxx>

#include <TopAbs_Orientation.hxx>

class HLRAlgo_Intersection;

namespace asiAlgo {
namespace hlr {

class VertexList
{
public:

  DEFINE_STANDARD_ALLOC


  asiAlgo_EXPORT VertexList(const EdgeInterferenceTool& T, const HLRAlgo_ListIteratorOfInterferenceList& I);

  //! Returns True when the curve is periodic.
  asiAlgo_EXPORT Standard_Boolean IsPeriodic() const;

  //! Returns True when there are more vertices.
  asiAlgo_EXPORT Standard_Boolean More() const;

  //! Proceeds to the next vertex.
  asiAlgo_EXPORT void Next();

  //! Returns the current vertex
  asiAlgo_EXPORT const HLRAlgo_Intersection& Current() const;

  //! Returns True if the current vertex is on the boundary of the edge.
  asiAlgo_EXPORT Standard_Boolean IsBoundary() const;

  //! Returns  True   if   the current    vertex  is  an
  //! interference.
  asiAlgo_EXPORT Standard_Boolean IsInterference() const;

  //! Returns the  orientation of the  current vertex if
  //! it is on the boundary of the edge.
  asiAlgo_EXPORT TopAbs_Orientation Orientation() const;

  //! Returns  the transition  of the  current vertex if
  //! it is an interference.
  asiAlgo_EXPORT TopAbs_Orientation Transition() const;

  //! Returns  the  transition  of  the  current  vertex
  //! relative to the boundary if it is an interference.
  asiAlgo_EXPORT TopAbs_Orientation BoundaryTransition() const;

private:

  HLRAlgo_ListIteratorOfInterferenceList myIterator;
  EdgeInterferenceTool myTool;
  Standard_Boolean fromEdge;
  Standard_Boolean fromInterf;

};

}
}

#endif // _HLRBRep_VertexList_HeaderFile
