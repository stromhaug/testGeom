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

#ifndef asiAlgo_HLRBRep_ThePolygonToolOfInterCSurf_HeaderFile
#define asiAlgo_HLRBRep_ThePolygonToolOfInterCSurf_HeaderFile

// asiAlgo includes
#include <asiAlgo_HlrPolygonOfInterCSurf.h>

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>

class Standard_OutOfRange;
class gp_Pnt;
class Bnd_Box;

namespace asiAlgo {
namespace hlr {

class PolygonOfInterCSurf;

class PolygonToolOfInterCSurf
{
public:

  DEFINE_STANDARD_ALLOC


  //! Give the bounding box of the polygon.
  asiAlgo_EXPORT static const Bnd_Box& Bounding (const PolygonOfInterCSurf& thePolygon);

  asiAlgo_EXPORT static Standard_Real DeflectionOverEstimation (const PolygonOfInterCSurf& thePolygon);

  asiAlgo_EXPORT static Standard_Boolean Closed (const PolygonOfInterCSurf& thePolygon);

  asiAlgo_EXPORT static Standard_Integer NbSegments (const PolygonOfInterCSurf& thePolygon);

  //! Give the point of range Index in the Polygon.
  asiAlgo_EXPORT static const gp_Pnt& BeginOfSeg (const PolygonOfInterCSurf& thePolygon, const Standard_Integer Index);

  //! Give the point of range Index in the Polygon.
  asiAlgo_EXPORT static const gp_Pnt& EndOfSeg (const PolygonOfInterCSurf& thePolygon, const Standard_Integer Index);

  asiAlgo_EXPORT static void Dump (const PolygonOfInterCSurf& thePolygon);

};

//=================================================================
  inline const Bnd_Box& PolygonToolOfInterCSurf::Bounding
          (const PolygonOfInterCSurf& thePolygon)
  {
    return thePolygon.Bounding();
  }
//=================================================================
  inline Standard_Real PolygonToolOfInterCSurf::DeflectionOverEstimation
          (const PolygonOfInterCSurf& thePolygon)
  {
    return thePolygon.DeflectionOverEstimation();
  }
//=================================================================
  inline Standard_Boolean PolygonToolOfInterCSurf::Closed
          (const PolygonOfInterCSurf& thePolygon)
  {
    return thePolygon.Closed();
  }
//=================================================================
  inline Standard_Integer PolygonToolOfInterCSurf::NbSegments
          (const PolygonOfInterCSurf& thePolygon)
  {
    return thePolygon.NbSegments();
  }
//=================================================================
  inline const gp_Pnt& PolygonToolOfInterCSurf::BeginOfSeg
          (const PolygonOfInterCSurf& thePolygon,
           const Standard_Integer Index)
  {
    return thePolygon.BeginOfSeg(Index);
  }
//=================================================================
  inline const gp_Pnt& PolygonToolOfInterCSurf::EndOfSeg
          (const PolygonOfInterCSurf& thePolygon,
           const Standard_Integer Index)
  {
    return thePolygon.EndOfSeg(Index);
  }

}
}

#endif // _HLRBRep_ThePolygonToolOfInterCSurf_HeaderFile
