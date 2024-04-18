// Created on: 1993-10-11
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

#ifndef asiAlgo_HLRBRep_PolyHLRToShape_HeaderFile
#define asiAlgo_HLRBRep_PolyHLRToShape_HeaderFile

#include <asiAlgo_HlrAux.h>

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Boolean.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Integer.hxx>

class TopoDS_Shape;

namespace asiAlgo {
namespace hlr {

class PolyAlgo;

//! A framework for filtering the computation
//! results of an PreciseAlgo algorithm by extraction.
//! From the results calculated by the algorithm on
//! a shape, a filter returns the type of edge you
//! want to identify. You can choose any of the following types of output:
//! -   visible sharp edges
//! -   hidden sharp edges
//! -   visible smooth edges
//! -   hidden smooth edges
//! -   visible sewn edges
//! -   hidden sewn edges
//! -   visible outline edges
//! -   hidden outline edges.
//! -   visible isoparameters and
//! -   hidden isoparameters.
//! Sharp edges present a C0 continuity (non G1).
//! Smooth edges present a G1 continuity (non G2).
//! Sewn edges present a C2 continuity.
//! The result is composed of 2D edges in the
//! projection plane of the view which the
//! algorithm has worked with. These 2D edges
//! are not included in the data structure of the visualized shape.
//! In order to obtain a complete image, you must
//! combine the shapes given by each of the chosen filters.
//! The construction of the shape does not call a
//! new computation of the algorithm, but only
//! reads its internal results.
class PolyHlrToShape
{
public:

  DEFINE_STANDARD_ALLOC


  //! Constructs a framework for filtering the results
  //! of the PreciseAlgo algorithm, A.
  //! Use the extraction filters to obtain the results you want for A.
  asiAlgo_EXPORT PolyHlrToShape();

  asiAlgo_EXPORT void Update (const Handle(PolyAlgo)& A);

  asiAlgo_EXPORT void Show();

  asiAlgo_EXPORT void Hide();

  asiAlgo_EXPORT TopoDS_Shape VCompound();

  asiAlgo_EXPORT TopoDS_Shape VCompound (const TopoDS_Shape& S);

  //! Sets the extraction filter for visible smooth edges.
  asiAlgo_EXPORT TopoDS_Shape Rg1LineVCompound();

  asiAlgo_EXPORT TopoDS_Shape Rg1LineVCompound (const TopoDS_Shape& S);

  //! Sets the extraction filter for visible sewn edges.
  asiAlgo_EXPORT TopoDS_Shape RgNLineVCompound();

  asiAlgo_EXPORT TopoDS_Shape RgNLineVCompound (const TopoDS_Shape& S);

  asiAlgo_EXPORT TopoDS_Shape OutLineVCompound();

  //! Sets the extraction filter for visible outlines.
  asiAlgo_EXPORT TopoDS_Shape OutLineVCompound (const TopoDS_Shape& S);

  asiAlgo_EXPORT TopoDS_Shape HCompound();

  asiAlgo_EXPORT TopoDS_Shape HCompound (const TopoDS_Shape& S);

  asiAlgo_EXPORT TopoDS_Shape Rg1LineHCompound();

  //! Sets the extraction filter for hidden smooth edges.
  asiAlgo_EXPORT TopoDS_Shape Rg1LineHCompound (const TopoDS_Shape& S);

  asiAlgo_EXPORT TopoDS_Shape RgNLineHCompound();

  //! Sets the extraction filter for hidden sewn edges.
  asiAlgo_EXPORT TopoDS_Shape RgNLineHCompound (const TopoDS_Shape& S);

  asiAlgo_EXPORT TopoDS_Shape OutLineHCompound();

  //! Sets the extraction filter for hidden outlines.
  //! Hidden outlines occur, for instance, in tori. In
  //! this case, the inner outlines of the torus seen on its side are hidden.
  asiAlgo_EXPORT TopoDS_Shape OutLineHCompound (const TopoDS_Shape& S);

private:


  asiAlgo_EXPORT TopoDS_Shape InternalCompound (const Standard_Integer typ, const Standard_Boolean visible, const TopoDS_Shape& S);


  Handle(PolyAlgo) myAlgo;
  ListOfBPnt2d myBiPntVis;
  ListOfBPnt2d myBiPntHid;
  Standard_Boolean myHideMode;


};


//=======================================================================
//function : Show
//purpose  : 
//=======================================================================

inline void PolyHlrToShape::Show()
{ myHideMode = Standard_False; }

//=======================================================================
//function : Hide
//purpose  : 
//=======================================================================

inline void PolyHlrToShape::Hide()
{ myHideMode = Standard_True; }

//=======================================================================
//function : VCompound
//purpose  : 
//=======================================================================

inline TopoDS_Shape PolyHlrToShape::VCompound()
{ return InternalCompound(4,Standard_True,TopoDS_Shape()); }

//=======================================================================
//function : VCompound
//purpose  : 
//=======================================================================

inline TopoDS_Shape
PolyHlrToShape::VCompound(const TopoDS_Shape& S)
{ return InternalCompound(4,Standard_True,S); }

//=======================================================================
//function : Rg1LineVCompound
//purpose  : 
//=======================================================================

inline TopoDS_Shape PolyHlrToShape::Rg1LineVCompound()
{ return InternalCompound(2,Standard_True,TopoDS_Shape()); }

//=======================================================================
//function : Rg1LineVCompound
//purpose  : 
//=======================================================================

inline TopoDS_Shape
PolyHlrToShape::Rg1LineVCompound(const TopoDS_Shape& S)
{ return InternalCompound(2,Standard_True,S); }

//=======================================================================
//function : RgNLineVCompound
//purpose  : 
//=======================================================================

inline TopoDS_Shape PolyHlrToShape::RgNLineVCompound()
{ return InternalCompound(3,Standard_True,TopoDS_Shape()); }

//=======================================================================
//function : RgNLineVCompound
//purpose  : 
//=======================================================================

inline TopoDS_Shape
PolyHlrToShape::RgNLineVCompound(const TopoDS_Shape& S)
{ return InternalCompound(3,Standard_True,S); }

//=======================================================================
//function : OutLineVCompound
//purpose  : 
//=======================================================================

inline TopoDS_Shape PolyHlrToShape::OutLineVCompound()
{ return InternalCompound(1,Standard_True,TopoDS_Shape()); }

//=======================================================================
//function : OutLineVCompound
//purpose  : 
//=======================================================================

inline TopoDS_Shape
PolyHlrToShape::OutLineVCompound(const TopoDS_Shape& S)
{ return InternalCompound(1,Standard_True,S); }

//=======================================================================
//function : HCompound
//purpose  : 
//=======================================================================

inline TopoDS_Shape PolyHlrToShape::HCompound()
{ return InternalCompound(4,Standard_False,TopoDS_Shape()); }

//=======================================================================
//function : HCompound
//purpose  : 
//=======================================================================

inline TopoDS_Shape
PolyHlrToShape::HCompound(const TopoDS_Shape& S)
{ return InternalCompound(4,Standard_False,S); }

//=======================================================================
//function : Rg1LineHCompound
//purpose  : 
//=======================================================================

inline TopoDS_Shape PolyHlrToShape::Rg1LineHCompound()
{ return InternalCompound(2,Standard_False,TopoDS_Shape()); }

//=======================================================================
//function : Rg1LineHCompound
//purpose  : 
//=======================================================================

inline TopoDS_Shape 
PolyHlrToShape::Rg1LineHCompound(const TopoDS_Shape& S)
{ return InternalCompound(2,Standard_False,S); }

//=======================================================================
//function : RgNLineHCompound
//purpose  : 
//=======================================================================

inline TopoDS_Shape PolyHlrToShape::RgNLineHCompound()
{ return InternalCompound(3,Standard_False,TopoDS_Shape()); }

//=======================================================================
//function : RgNLineHCompound
//purpose  : 
//=======================================================================

inline TopoDS_Shape
PolyHlrToShape::RgNLineHCompound(const TopoDS_Shape& S)
{ return InternalCompound(3,Standard_False,S); }

//=======================================================================
//function : OutLineHCompound
//purpose  : 
//=======================================================================

inline TopoDS_Shape PolyHlrToShape::OutLineHCompound()
{ return InternalCompound(1,Standard_False,TopoDS_Shape()); }

//=======================================================================
//function : OutLineHCompound
//purpose  : 
//=======================================================================

inline TopoDS_Shape
PolyHlrToShape::OutLineHCompound(const TopoDS_Shape& S)
{ return InternalCompound(1,Standard_False,S); }

}
}

#endif // _HLRBRep_PolyHLRToShape_HeaderFile
