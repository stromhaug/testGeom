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

#ifndef asiAlgo_HLRBRep_ShapeBounds_HeaderFile
#define asiAlgo_HLRBRep_ShapeBounds_HeaderFile

// asiAlgo includes
#include <asiAlgo.h>

// OpenCascade includes
#include <HLRAlgo_EdgesBlock.hxx>

class HLRTopoBRep_OutLiner;

namespace asiAlgo {
namespace hlr {

//! Contains  a Shape and the  bounds of its vertices,
//! edges and faces in the DataStructure.
class ShapeBounds
{
public:

  DEFINE_STANDARD_ALLOC

  asiAlgo_EXPORT ShapeBounds();

  asiAlgo_EXPORT ShapeBounds(const Handle(HLRTopoBRep_OutLiner)& S, const Handle(Standard_Transient)& SData, const Standard_Integer nbIso, const Standard_Integer V1, const Standard_Integer V2, const Standard_Integer E1, const Standard_Integer E2, const Standard_Integer F1, const Standard_Integer F2);

  asiAlgo_EXPORT ShapeBounds(const Handle(HLRTopoBRep_OutLiner)& S, const Standard_Integer nbIso, const Standard_Integer V1, const Standard_Integer V2, const Standard_Integer E1, const Standard_Integer E2, const Standard_Integer F1, const Standard_Integer F2);

  asiAlgo_EXPORT void Translate (const Standard_Integer NV, const Standard_Integer NE, const Standard_Integer NF);

  asiAlgo_EXPORT void Shape (const Handle(HLRTopoBRep_OutLiner)& S);

  asiAlgo_EXPORT const Handle(HLRTopoBRep_OutLiner)& Shape() const;

  asiAlgo_EXPORT void ShapeData (const Handle(Standard_Transient)& SD);

  asiAlgo_EXPORT const Handle(Standard_Transient)& ShapeData() const;

  asiAlgo_EXPORT void NbOfIso (const Standard_Integer nbIso);

  asiAlgo_EXPORT Standard_Integer NbOfIso() const;

  asiAlgo_EXPORT void Sizes (Standard_Integer& NV, Standard_Integer& NE, Standard_Integer& NF) const;

  asiAlgo_EXPORT void Bounds (Standard_Integer& V1, Standard_Integer& V2, Standard_Integer& E1, Standard_Integer& E2, Standard_Integer& F1, Standard_Integer& F2) const;

  void UpdateMinMax (const HLRAlgo_EdgesBlock::MinMaxIndices& theTotMinMax)
  {
    myMinMax = theTotMinMax;
  }

  HLRAlgo_EdgesBlock::MinMaxIndices& MinMax()
  {
    return myMinMax;
  }

private:

  Handle(HLRTopoBRep_OutLiner) myShape;
  Handle(Standard_Transient) myShapeData;
  Standard_Integer myNbIso;
  Standard_Integer myVertStart;
  Standard_Integer myVertEnd;
  Standard_Integer myEdgeStart;
  Standard_Integer myEdgeEnd;
  Standard_Integer myFaceStart;
  Standard_Integer myFaceEnd;
  HLRAlgo_EdgesBlock::MinMaxIndices myMinMax;


};


//=======================================================================
//function : ShapeBounds
//purpose  :
//=======================================================================

inline ShapeBounds::ShapeBounds ()
: myNbIso(0),
  myVertStart(0),
  myVertEnd(0),
  myEdgeStart(0),
  myEdgeEnd(0),
  myFaceStart(0),
  myFaceEnd(0)
{
}

//=======================================================================
//function : Shape
//purpose  :
//=======================================================================

inline void 
ShapeBounds::Shape (const Handle(HLRTopoBRep_OutLiner)& S)
{ myShape = S; }

//=======================================================================
//function : Shape
//purpose  :
//=======================================================================

inline const Handle(HLRTopoBRep_OutLiner) &
ShapeBounds::Shape () const
{ return myShape; }

//=======================================================================
//function : ShapeData
//purpose  :
//=======================================================================

inline void ShapeBounds::
ShapeData (const Handle(Standard_Transient)& SD)
{ myShapeData = SD; }

//=======================================================================
//function : ShapeData
//purpose  :
//=======================================================================

inline const Handle(Standard_Transient) & ShapeBounds::ShapeData () const
{ return myShapeData; }

//=======================================================================
//function : NbOfIso
//purpose  :
//=======================================================================

inline void ShapeBounds::NbOfIso (const Standard_Integer nbIso)
{ myNbIso = nbIso; }

//=======================================================================
//function : NbOfIso
//purpose  :
//=======================================================================

inline Standard_Integer ShapeBounds::NbOfIso () const
{ return myNbIso; }


}
}

#endif // _HLRBRep_ShapeBounds_HeaderFile
