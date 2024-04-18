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

#ifndef asiAlgo_HLRBRep_ThePolyhedronToolOfInterCSurf_HeaderFile
#define asiAlgo_HLRBRep_ThePolyhedronToolOfInterCSurf_HeaderFile

// asiAlgo includes
#include <asiAlgo_HlrPolyhedronOfInterCSurf.h>

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Bnd_HArray1OfBox.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>

class Standard_OutOfRange;
class Bnd_Box;
class gp_Pnt;

namespace asiAlgo {
namespace hlr {

class PolyhedronOfInterCSurf;

class PolyhedronToolOfInterCSurf
{
public:

  DEFINE_STANDARD_ALLOC

  //! Give the bounding box of the PolyhedronTool.
  asiAlgo_EXPORT static const Bnd_Box& Bounding (const PolyhedronOfInterCSurf& thePolyh);

  //! Give the array of boxes. The box <n> corresponding
  //! to the triangle <n>.
  asiAlgo_EXPORT static const Handle(Bnd_HArray1OfBox)& ComponentsBounding (const PolyhedronOfInterCSurf& thePolyh);

  //! Give the tolerance of the polygon.
  asiAlgo_EXPORT static Standard_Real DeflectionOverEstimation (const PolyhedronOfInterCSurf& thePolyh);

  //! Give the number of triangles in this polyhedral surface.
  asiAlgo_EXPORT static Standard_Integer NbTriangles (const PolyhedronOfInterCSurf& thePolyh);

  //! Give the indices  of  the 3 points of  the triangle of
  //! address Index in the PolyhedronTool.
  asiAlgo_EXPORT static void Triangle (const PolyhedronOfInterCSurf& thePolyh, const Standard_Integer Index, Standard_Integer& P1, Standard_Integer& P2, Standard_Integer& P3);

  //! Give the point of index i in the polyhedral surface.
  asiAlgo_EXPORT static const gp_Pnt& Point (const PolyhedronOfInterCSurf& thePolyh, const Standard_Integer Index);

  //! Give the address Tricon of the triangle connexe to
  //! the triangle of address Triang by the edge Pivot Pedge
  //! and the third point of this  connexe triangle.
  //! When we are on a free edge TriCon==0 but the function return
  //! the value of the triangle in the other side of Pivot on the free edge.
  //! Used to turn around a vertex.
  asiAlgo_EXPORT static Standard_Integer TriConnex (const PolyhedronOfInterCSurf& thePolyh, const Standard_Integer Triang, const Standard_Integer Pivot, const Standard_Integer Pedge, Standard_Integer& TriCon, Standard_Integer& OtherP);

  //! This method returns true if the edge based on points with
  //! indices Index1 and Index2 represents a boundary edge.
  //! It is necessary to take into account the boundary deflection for this edge.
  asiAlgo_EXPORT static Standard_Boolean IsOnBound (const PolyhedronOfInterCSurf& thePolyh, const Standard_Integer Index1, const Standard_Integer Index2);

  //! This method returns a border deflection of the polyhedron.
  asiAlgo_EXPORT static Standard_Real GetBorderDeflection (const PolyhedronOfInterCSurf& thePolyh);

  asiAlgo_EXPORT static void Dump (const PolyhedronOfInterCSurf& thePolyh);

};


  inline const Bnd_Box& PolyhedronToolOfInterCSurf::Bounding
          (const PolyhedronOfInterCSurf& thePolyh)
  {
    return thePolyh.Bounding();
  }

  inline const Handle(Bnd_HArray1OfBox)& PolyhedronToolOfInterCSurf::
  ComponentsBounding(const PolyhedronOfInterCSurf& thePolyh)
  {
    return thePolyh.ComponentsBounding();
  }

  inline Standard_Real PolyhedronToolOfInterCSurf::DeflectionOverEstimation
          (const PolyhedronOfInterCSurf& thePolyh)
  {
    return thePolyh.DeflectionOverEstimation();
  }

  inline Standard_Integer PolyhedronToolOfInterCSurf::NbTriangles
          (const PolyhedronOfInterCSurf& thePolyh)
  {
    return thePolyh.NbTriangles();
  }



  inline void PolyhedronToolOfInterCSurf::Triangle(const PolyhedronOfInterCSurf& thePolyh,
                                                       const Standard_Integer Index,
                                                       Standard_Integer& P1,
                                                       Standard_Integer& P2,
                                                       Standard_Integer& P3)
  {
    thePolyh.Triangle(Index, P1,P2,P3);
  }


  inline const gp_Pnt& PolyhedronToolOfInterCSurf::Point
          (const PolyhedronOfInterCSurf& thePolyh,
           const Standard_Integer Index)
  {
    return thePolyh.Point(Index);
  }

  inline Standard_Integer PolyhedronToolOfInterCSurf::TriConnex
          (const PolyhedronOfInterCSurf& thePolyh,
           const Standard_Integer Triang,
           const Standard_Integer Pivot,
           const Standard_Integer Pedge,
           Standard_Integer& TriCon,
           Standard_Integer& OtherP)
  {
    return thePolyh.TriConnex(Triang, Pivot, Pedge, TriCon, OtherP);
  }

//  Modified by Sergey KHROMOV - Fri Dec  7 13:47:07 2001 Begin
//=======================================================================
//function : IsOnBound
//purpose  : This method returns true if the edge based on points with
//           indices Index1 and Index2 represents a boundary edge.
//=======================================================================

  inline Standard_Boolean PolyhedronToolOfInterCSurf::IsOnBound
          (const PolyhedronOfInterCSurf& thePolyh,
           const Standard_Integer Index1,
           const Standard_Integer Index2)
  {
    return thePolyh.IsOnBound(Index1, Index2);
  }

//=======================================================================
//function : GetBorderDeflection
//purpose  : This method returns a border deflection of the polyhedron.
//=======================================================================

  inline Standard_Real PolyhedronToolOfInterCSurf::GetBorderDeflection
          (const PolyhedronOfInterCSurf& thePolyh)
  {
    return thePolyh.GetBorderDeflection();
  }

}
}

#endif // _HLRBRep_ThePolyhedronToolOfInterCSurf_HeaderFile
