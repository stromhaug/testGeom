// Created on: 1993-05-04
// Created by: Modelistation
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

#ifndef asiAlgo_HLRBRep_ShapeToHLR_HeaderFile
#define asiAlgo_HLRBRep_ShapeToHLR_HeaderFile

// asiAlgo includes
#include <asiAlgo.h>

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepTopAdaptor_MapOfShapeTool.hxx>
#include <Standard_Integer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <Standard_Boolean.hxx>

class HLRTopoBRep_OutLiner;
class HLRAlgo_Projector;
class TopoDS_Face;

namespace asiAlgo {
namespace hlr {

class Data;

//! compute  the   OutLinedShape  of  a Shape with  an
//! OutLiner,    a  Projector  and   create  the  Data
//! Structure of a Shape.
class ShapeToHlr
{
public:

  DEFINE_STANDARD_ALLOC

  //! Creates  a DataStructure   containing the OutLiner
  //! <S> depending on the projector <P> and nbIso.
  asiAlgo_EXPORT static Handle(Data) Load (const Handle(HLRTopoBRep_OutLiner)& S, const HLRAlgo_Projector& P, BRepTopAdaptor_MapOfShapeTool& MST, const Standard_Integer nbIso = 0);

private:

  asiAlgo_EXPORT static void ExploreFace (const Handle(HLRTopoBRep_OutLiner)& S, const Handle(Data)& DS, const TopTools_IndexedMapOfShape& FM, const TopTools_IndexedMapOfShape& EM, Standard_Integer& i, const TopoDS_Face& F, const Standard_Boolean closed);

  asiAlgo_EXPORT static void ExploreShape (const Handle(HLRTopoBRep_OutLiner)& S, const Handle(Data)& DS, const TopTools_IndexedMapOfShape& FM, const TopTools_IndexedMapOfShape& EM);

};

}
}

#endif // _HLRBRep_ShapeToHLR_HeaderFile
