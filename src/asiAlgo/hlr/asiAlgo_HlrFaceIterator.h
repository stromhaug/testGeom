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

#ifndef asiAlgo_HLRBRep_FaceIterator_HeaderFile
#define asiAlgo_HLRBRep_FaceIterator_HeaderFile

// asiAlgo includes
#include <asiAlgo.h>

#include <HLRAlgo_EdgesBlock.hxx>
#include <HLRAlgo_WiresBlock.hxx>

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_Orientation.hxx>

namespace asiAlgo {
namespace hlr {

class FaceData;

class FaceIterator
{
public:

  DEFINE_STANDARD_ALLOC


  asiAlgo_EXPORT FaceIterator();

  //! Begin an exploration of the edges of the face <fd>
  asiAlgo_EXPORT void InitEdge (FaceData& fd);

  asiAlgo_EXPORT Standard_Boolean MoreEdge() const;

  asiAlgo_EXPORT void NextEdge();

  //! Returns True if the current edge is the first of a
  //! wire.
  asiAlgo_EXPORT Standard_Boolean BeginningOfWire() const;

  //! Returns True if the current edge is the  last of a
  //! wire.
  asiAlgo_EXPORT Standard_Boolean EndOfWire() const;

  //! Skip the current wire in the exploration.
  asiAlgo_EXPORT void SkipWire();

  //! Returns the edges of the current wire.
  asiAlgo_EXPORT Handle(HLRAlgo_EdgesBlock) Wire() const;

  asiAlgo_EXPORT Standard_Integer Edge() const;

  asiAlgo_EXPORT TopAbs_Orientation Orientation() const;

  asiAlgo_EXPORT Standard_Boolean OutLine() const;

  asiAlgo_EXPORT Standard_Boolean Internal() const;

  asiAlgo_EXPORT Standard_Boolean Double() const;

  asiAlgo_EXPORT Standard_Boolean IsoLine() const;

private:

  Standard_Integer iWire;
  Standard_Integer nbWires;
  Standard_Integer iEdge;
  Standard_Integer nbEdges;
  Handle(HLRAlgo_WiresBlock) myWires;
  Handle(HLRAlgo_EdgesBlock) myEdges;

};


//=======================================================================
//function : MoreEdge
//purpose  : 
//=======================================================================

inline Standard_Boolean FaceIterator::MoreEdge () const
{ return iWire <= nbWires;}

//=======================================================================
//function : BeginningOfWire
//purpose  : 
//=======================================================================

inline Standard_Boolean FaceIterator::BeginningOfWire () const
{ return iEdge == 1;}

//=======================================================================
//function : EndOfWire
//purpose  : 
//=======================================================================

inline Standard_Boolean FaceIterator::EndOfWire () const
{ return iEdge == nbEdges;}

//=======================================================================
//function : SkipWire
//purpose  : 
//=======================================================================

inline void FaceIterator::SkipWire ()
{ 
  iEdge = nbEdges;
  NextEdge();
}

//=======================================================================
//function : Wire
//purpose  : 
//=======================================================================

inline Handle(HLRAlgo_EdgesBlock) FaceIterator::Wire() const
{ return myWires->Wire(iWire); }

//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================

inline Standard_Integer FaceIterator::Edge () const
{ return myEdges->Edge(iEdge); }

//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

inline TopAbs_Orientation FaceIterator::Orientation() const
{ return myEdges->Orientation(iEdge); }

//=======================================================================
//function : OutLine
//purpose  : 
//=======================================================================

inline Standard_Boolean FaceIterator::OutLine() const
{ return myEdges->OutLine(iEdge); }

//=======================================================================
//function : Internal
//purpose  : 
//=======================================================================

inline Standard_Boolean FaceIterator::Internal() const
{ return myEdges->Internal(iEdge); }

//=======================================================================
//function : Double
//purpose  : 
//=======================================================================

inline Standard_Boolean FaceIterator::Double() const
{ return myEdges->Double(iEdge); }

//=======================================================================
//function : IsoLine
//purpose  : 
//=======================================================================

inline Standard_Boolean FaceIterator::IsoLine() const
{ return myEdges->IsoLine(iEdge); }

}
}

#endif // _HLRBRep_FaceIterator_HeaderFile
