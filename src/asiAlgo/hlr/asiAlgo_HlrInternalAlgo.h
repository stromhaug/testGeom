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

#ifndef asiAlgo_InternalAlgo_HeaderFile
#define asiAlgo_InternalAlgo_HeaderFile

// asiAlgo includes
#include <asiAlgo_HlrAux.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OpenCascade includes
#include <HLRAlgo_Projector.hxx>
#include <BRepTopAdaptor_MapOfShapeTool.hxx>

class HLRTopoBRep_OutLiner;

namespace asiAlgo {
namespace hlr {

class Data;
class ShapeBounds;
class InternalAlgo;

//! \ingroup ASI_MODELING
//!
//! Base class for precise HLR algorithm.
class InternalAlgo : public ActAPI_IAlgorithm
{
  DEFINE_STANDARD_RTTI_INLINE(InternalAlgo, ActAPI_IAlgorithm)

public:

  //! Ctor with optional diagnostic tools.
  asiAlgo_EXPORT
    InternalAlgo(ActAPI_ProgressEntry progress = nullptr,
                 ActAPI_PlotterEntry  plotter  = nullptr);

public:

  //! set the projector.
  asiAlgo_EXPORT void Projector (const HLRAlgo_Projector& P);

  //! set the projector.
  asiAlgo_EXPORT HLRAlgo_Projector& Projector();

  //! update the DataStructure.
  asiAlgo_EXPORT void Update();

  //! add the shape <S>.
  asiAlgo_EXPORT void Load (const Handle(HLRTopoBRep_OutLiner)& S, const Handle(Standard_Transient)& SData, const Standard_Integer nbIso = 0);

  //! add the shape <S>.
  asiAlgo_EXPORT void Load (const Handle(HLRTopoBRep_OutLiner)& S, const Standard_Integer nbIso = 0);

  //! return the index of the Shape <S> and  return 0 if
  //! the Shape <S> is not found.
  asiAlgo_EXPORT Standard_Integer Index (const Handle(HLRTopoBRep_OutLiner)& S) const;

  //! remove the Shape of Index <I>.
  asiAlgo_EXPORT void Remove (const Standard_Integer I);

  //! Change the Shape Data of the Shape of index <I>.
  asiAlgo_EXPORT void ShapeData (const Standard_Integer I, const Handle(Standard_Transient)& SData);

  asiAlgo_EXPORT SeqOfShapeBounds& GetSeqOfShapeBounds();

  asiAlgo_EXPORT Standard_Integer NbShapes() const;

  asiAlgo_EXPORT ShapeBounds& GetShapeBounds (const Standard_Integer I);

  //! init the status of the selected edges depending of
  //! the back faces of a closed shell.
  asiAlgo_EXPORT void InitEdgeStatus();

  //! select all the DataStructure.
  asiAlgo_EXPORT void Select();

  //! select  only   the Shape of index <I>.
  asiAlgo_EXPORT void Select (const Standard_Integer I);

  //! select only the edges of the Shape <S>.
  asiAlgo_EXPORT void SelectEdge (const Standard_Integer I);

  //! select only the faces of the Shape <S>.
  asiAlgo_EXPORT void SelectFace (const Standard_Integer I);

  //! set to visible all the edges.
  asiAlgo_EXPORT void ShowAll();

  //! set to visible all the edges of the Shape <S>.
  asiAlgo_EXPORT void ShowAll (const Standard_Integer I);

  //! set to hide all the edges.
  asiAlgo_EXPORT void HideAll();

  //! set to  hide all the  edges of the  Shape <S>.
  asiAlgo_EXPORT void HideAll (const Standard_Integer I);

  //! own hiding  of all the shapes of the DataStructure
  //! without hiding by each other.
  asiAlgo_EXPORT void PartialHide();

  //! hide all the DataStructure.
  asiAlgo_EXPORT void Hide();

  //! hide the Shape <S> by itself.
  asiAlgo_EXPORT void Hide (const Standard_Integer I);

  //! hide the Shape <S1> by the shape <S2>.
  asiAlgo_EXPORT void Hide (const Standard_Integer I, const Standard_Integer J);

  asiAlgo_EXPORT void Debug (const Standard_Boolean deb);

  asiAlgo_EXPORT Standard_Boolean Debug() const;

  asiAlgo_EXPORT Handle(Data) DataStructure() const;

private:

  //! first if <SideFace> own hiding  of the side faces.
  //! After hiding  of    the  selected  parts  of   the
  //! DataStructure.
  asiAlgo_EXPORT void HideSelected (const Standard_Integer I, const Standard_Boolean SideFace);

  Handle(Data) myDS;
  HLRAlgo_Projector myProj;
  SeqOfShapeBounds myShapes;
  BRepTopAdaptor_MapOfShapeTool myMapOfShapeTool;
  Standard_Boolean myDebug;


};

}
}

#endif // _InternalAlgo_HeaderFile
