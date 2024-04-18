// Created on: 1992-08-26
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

#ifndef asiAlgo_HLRBRep_Intersector_HeaderFile
#define asiAlgo_HLRBRep_Intersector_HeaderFile

#include <asiAlgo_HlrCInter.h>
#include <asiAlgo_HlrInterCSurf.h>

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Address.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>

class gp_Lin;
class IntCurveSurface_IntersectionPoint;
class IntRes2d_IntersectionSegment;
class IntCurveSurface_IntersectionSegment;

namespace asiAlgo {
namespace hlr {

//! The Intersector  computes 2D  intersections of the projections of 3D curves.
//! It can also computes the intersection of a 3D line and a surface.
class Intersector
{
public:

  DEFINE_STANDARD_ALLOC

  asiAlgo_EXPORT Intersector();

  //! Performs the auto intersection of an edge.
  //! The edge domain is cut at start with da1*(b-a) and at end with db1*(b-a).
  asiAlgo_EXPORT void Perform (const Standard_Address A1, const Standard_Real da1, const Standard_Real db1);

  //! Performs the intersection between the two edges.
  //! The edges domains are cut at start with da*(b-a) and at end with db*(b-a).
  asiAlgo_EXPORT void Perform (const Standard_Integer nA, const Standard_Address A1, const Standard_Real da1, const Standard_Real db1, const Standard_Integer nB, const Standard_Address A2, const Standard_Real da2, const Standard_Real db2, const Standard_Boolean NoBound);

  //! Create a single IntersectionPoint (U on A1) (V on A2)
  //! The point is middle on both curves.
  asiAlgo_EXPORT void SimulateOnePoint (const Standard_Address A1, const Standard_Real U, const Standard_Address A2, const Standard_Real V);

  asiAlgo_EXPORT void Load (Standard_Address& A);

  asiAlgo_EXPORT void Perform (const gp_Lin& L, const Standard_Real P);

  asiAlgo_EXPORT Standard_Boolean IsDone() const;

  asiAlgo_EXPORT Standard_Integer NbPoints() const;

  asiAlgo_EXPORT const IntRes2d_IntersectionPoint& Point (const Standard_Integer N) const;

  asiAlgo_EXPORT const IntCurveSurface_IntersectionPoint& CSPoint (const Standard_Integer N) const;

  asiAlgo_EXPORT Standard_Integer NbSegments() const;

  asiAlgo_EXPORT const IntRes2d_IntersectionSegment& Segment (const Standard_Integer N) const;

  asiAlgo_EXPORT const IntCurveSurface_IntersectionSegment& CSSegment (const Standard_Integer N) const;

  asiAlgo_EXPORT void Destroy();
~Intersector()
{
  Destroy();
}

private:

  IntRes2d_IntersectionPoint mySinglePoint;
  Standard_Integer myTypePerform;
  CInter myIntersector;
  InterCSurf myCSIntersector;
  Standard_Address mySurface;
  PolyhedronOfInterCSurf* myPolyhedron;

};

}
}

#endif // _HLRBRep_Intersector_HeaderFile
