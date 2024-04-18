// Created on: 1993-01-11
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

#ifndef asiAlgo_HlrData_HeaderFile
#define asiAlgo_HlrData_HeaderFile

#include <asiAlgo_HlrAux.h>
#include <asiAlgo_HlrCLProps.h>
#include <asiAlgo_HlrSLProps.h>
#include <asiAlgo_HlrFaceIterator.h>
#include <asiAlgo_HlrIntersector.h>

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <Standard_Integer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Standard_ShortReal.hxx>
#include <HLRAlgo_Projector.hxx>
#include <Standard_Real.hxx>
#include <Standard_Address.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <Standard_Boolean.hxx>
#include <GeomAbs_CurveType.hxx>
#include <TopAbs_Orientation.hxx>
#include <HLRAlgo_Interference.hxx>
#include <Standard_Transient.hxx>
#include <BRepTopAdaptor_MapOfShapeTool.hxx>
#include <TopAbs_State.hxx>
#include <HLRAlgo_InterferenceList.hxx>

class BRepTopAdaptor_TopolTool;
class gp_Dir2d;
class IntRes2d_IntersectionPoint;
class TableauRejection;

namespace asiAlgo {
namespace hlr {

class EdgeData;
class FaceData;

class Data;
DEFINE_STANDARD_HANDLE(Data, Standard_Transient)

class Data : public Standard_Transient
{
public:

  //! Create an  empty data structure  of <NV> vertices,
  //! <NE> edges and <NF> faces.
  asiAlgo_EXPORT Data(const Standard_Integer NV, const Standard_Integer NE, const Standard_Integer NF);

  //! Write <DS>    in   me  with   a     translation of
  //! <dv>,<de>,<df>.
  asiAlgo_EXPORT void Write (const Handle(Data)& DS, const Standard_Integer dv, const Standard_Integer de, const Standard_Integer df);

  asiAlgo_EXPORT Array1OfEData& EDataArray();

  asiAlgo_EXPORT Array1OfFData& FDataArray();

  //! Set the  tolerance for the  rejections  during the
  //! exploration
  asiAlgo_EXPORT void Tolerance (const Standard_ShortReal tol);

  //! returns  the tolerance for the  rejections  during
  //! the exploration
  asiAlgo_EXPORT Standard_ShortReal Tolerance() const;

  //! end of building  of the Data and updating
  //! all the information linked to the projection.
  asiAlgo_EXPORT void Update (const HLRAlgo_Projector& P);

  asiAlgo_EXPORT HLRAlgo_Projector& Projector();

  asiAlgo_EXPORT Standard_Integer NbVertices() const;

  asiAlgo_EXPORT Standard_Integer NbEdges() const;

  asiAlgo_EXPORT Standard_Integer NbFaces() const;

  asiAlgo_EXPORT TopTools_IndexedMapOfShape& EdgeMap();

  asiAlgo_EXPORT TopTools_IndexedMapOfShape& FaceMap();

  //! to compare with only non rejected edges.
  asiAlgo_EXPORT void InitBoundSort (const HLRAlgo_EdgesBlock::MinMaxIndices& MinMaxTot, const Standard_Integer e1, const Standard_Integer e2);

  //! Begin an iteration only  on visible Edges
  //! crossing the face number <FI>.
  asiAlgo_EXPORT void InitEdge (const Standard_Integer FI, BRepTopAdaptor_MapOfShapeTool& MST);

  asiAlgo_EXPORT Standard_Boolean MoreEdge();

  asiAlgo_EXPORT void NextEdge (const Standard_Boolean skip = Standard_True);

  //! Returns the  current Edge
  asiAlgo_EXPORT Standard_Integer Edge() const;

  //! Returns true if   the  current edge to   be hidden
  //! belongs to the hiding face.
  asiAlgo_EXPORT Standard_Boolean HidingTheFace() const;

  //! Returns true if the current hiding face is not  an
  //! auto-intersected one.
  asiAlgo_EXPORT Standard_Boolean SimpleHidingFace() const;

  //! Intersect  the current  Edge  with the boundary of
  //! the hiding  face.   The interferences are given by
  //! the More, Next, and Value methods.
  asiAlgo_EXPORT void InitInterference();

  asiAlgo_EXPORT Standard_Boolean MoreInterference() const;

  asiAlgo_EXPORT void NextInterference();

  //! Returns  True if the  interference is rejected.
  asiAlgo_EXPORT Standard_Boolean RejectedInterference();

  //! Returns True if the rejected interference is above
  //! the face.
  asiAlgo_EXPORT Standard_Boolean AboveInterference();

  asiAlgo_EXPORT HLRAlgo_Interference& Interference();

  //! Returns the local description of the projection of
  //! the current LEdge  at parameter  <Param>.
  asiAlgo_EXPORT void LocalLEGeometry2D (const Standard_Real Param, gp_Dir2d& Tg, gp_Dir2d& Nm, Standard_Real& Cu);

  //! Returns the local description of the projection of
  //! the current FEdge  at parameter  <Param>.
  asiAlgo_EXPORT void LocalFEGeometry2D (const Standard_Integer FE, const Standard_Real Param, gp_Dir2d& Tg, gp_Dir2d& Nm, Standard_Real& Cu);

  //! Returns the local  3D   state of the  intersection
  //! between the current edge and the current face at the
  //! <p1> and <p2> parameters.
  asiAlgo_EXPORT void EdgeState (const Standard_Real p1, const Standard_Real p2, TopAbs_State& stbef, TopAbs_State& staf);

  //! Returns the  true if the  Edge <ED> belongs to the
  //! Hiding Face.
  asiAlgo_EXPORT Standard_Boolean EdgeOfTheHidingFace (const Standard_Integer E, const EdgeData& ED) const;

  //! Returns the number of  levels of hiding face above
  //! the   first  point  of   the    edge <ED>.     The
  //! InterferenceList is  given to  compute far away of
  //! the Interferences and then come back.
  asiAlgo_EXPORT Standard_Integer HidingStartLevel (const Standard_Integer E, const EdgeData& ED, const HLRAlgo_InterferenceList& IL);

  //! Returns   the  state   of  the   Edge  <ED>  after
  //! classification.
  asiAlgo_EXPORT TopAbs_State Compare (const Standard_Integer E, const EdgeData& ED);

  //! Simple classification of part of edge [p1,  p2].
  //! Returns OUT if at least 1 of Nbp points of edge is out; otherwise returns IN.
  //! It is used to check "suspicion" hidden part of edge.
  asiAlgo_EXPORT TopAbs_State SimplClassify (const Standard_Integer E, const EdgeData& ED, const Standard_Integer Nbp, const Standard_Real p1, const Standard_Real p2);

  //! Classification of an edge.
  asiAlgo_EXPORT TopAbs_State Classify (const Standard_Integer E, const EdgeData& ED, const Standard_Boolean LevelFlag, Standard_Integer& Level, const Standard_Real param);

  //! Returns true if the current face is bad.
  asiAlgo_EXPORT Standard_Boolean IsBadFace() const;

  asiAlgo_EXPORT void Destroy();
~Data()
{
  Destroy();
}

  DEFINE_STANDARD_RTTIEXT(Data,Standard_Transient)

private:

  //! Orient the   OutLines  ( left  must  be  inside in
  //! projection ). Returns True if the face of a closed
  //! shell has been inverted;
  asiAlgo_EXPORT Standard_Boolean OrientOutLine (const Standard_Integer I, FaceData& FD);

  //! Orient the Edges which  are not  Internal OutLine,
  //! not Double and not IsoLine.
  asiAlgo_EXPORT void OrientOthEdge (const Standard_Integer I, FaceData& FD);

  //! Returns  True  if the  intersection is  rejected.
  asiAlgo_EXPORT Standard_Boolean RejectedPoint (const IntRes2d_IntersectionPoint& PInter, const TopAbs_Orientation BoundOri, const Standard_Integer NumSeg);

  //! Returns True if there is a common vertex between myLE and myFE depending on <head1> and <head2>.
  asiAlgo_EXPORT Standard_Boolean SameVertex (const Standard_Boolean head1, const Standard_Boolean head2);

private:

  Standard_Integer myNbVertices;
  Standard_Integer myNbEdges;
  Standard_Integer myNbFaces;
  TopTools_IndexedMapOfShape myEMap;
  TopTools_IndexedMapOfShape myFMap;
  Array1OfEData myEData;
  Array1OfFData myFData;
  TColStd_Array1OfInteger myEdgeIndices;
  Standard_ShortReal myToler;
  HLRAlgo_Projector myProj;
  CLProps myLLProps;
  CLProps myFLProps;
  SLProps mySLProps;
  Standard_Real myBigSize;
  FaceIterator myFaceItr1;
  FaceIterator myFaceItr2;
  Standard_Integer iFace;
  FaceData* iFaceData;
  Standard_Address iFaceGeom;
  HLRAlgo_EdgesBlock::MinMaxIndices* iFaceMinMax;
  GeomAbs_SurfaceType iFaceType;
  Standard_Boolean iFaceBack;
  Standard_Boolean iFaceSimp;
  Standard_Boolean iFaceSmpl;
  Standard_Boolean iFaceTest;
  Standard_Integer myHideCount;
  Standard_Real myDeca[16];
  Standard_Real mySurD[16];
  Standard_Integer myCurSortEd;
  Standard_Integer myNbrSortEd;
  Standard_Integer myLE;
  Standard_Boolean myLEOutLine;
  Standard_Boolean myLEInternal;
  Standard_Boolean myLEDouble;
  Standard_Boolean myLEIsoLine;
  EdgeData* myLEData;
  const Curve* myLEGeom;
  HLRAlgo_EdgesBlock::MinMaxIndices* myLEMinMax;
  GeomAbs_CurveType myLEType;
  Standard_ShortReal myLETol;
  Standard_Integer myFE;
  TopAbs_Orientation myFEOri;
  Standard_Boolean myFEOutLine;
  Standard_Boolean myFEInternal;
  Standard_Boolean myFEDouble;
  EdgeData* myFEData;
  Curve* myFEGeom;
  GeomAbs_CurveType myFEType;
  Standard_ShortReal myFETol;
  Intersector myIntersector;
  Handle(BRepTopAdaptor_TopolTool) myClassifier;
  Standard_Boolean mySameVertex;
  Standard_Boolean myIntersected;
  Standard_Integer myNbPoints;
  Standard_Integer myNbSegments;
  Standard_Integer iInterf;
  HLRAlgo_Interference myIntf;
  Standard_Boolean myAboveIntf;
  TableauRejection* myReject;

};

//=======================================================================
//function : EDataArray
//purpose  : 
//=======================================================================

inline Array1OfEData & Data::EDataArray ()
{ return myEData; }

//=======================================================================
//function : FDataArray
//purpose  : 
//=======================================================================

inline Array1OfFData & Data::FDataArray ()
{ return myFData; }

//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

inline void Data::Tolerance (const Standard_ShortReal tol)
{ myToler = tol; }

//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

inline Standard_ShortReal Data::Tolerance () const
{ return myToler; }

//=======================================================================
//function : Projector
//purpose  : 
//=======================================================================

inline HLRAlgo_Projector & Data::Projector ()
{ return myProj; }

//=======================================================================
//function : NbVertices
//purpose  : 
//=======================================================================

inline Standard_Integer Data::NbVertices () const
{ return myNbVertices; }

//=======================================================================
//function : NbEdges
//purpose  : 
//=======================================================================

inline Standard_Integer Data::NbEdges () const
{ return myNbEdges; }

//=======================================================================
//function : NbFaces
//purpose  : 
//=======================================================================

inline Standard_Integer Data::NbFaces () const
{ return myNbFaces; }

//=======================================================================
//function : EdgeMap
//purpose  : 
//=======================================================================

inline TopTools_IndexedMapOfShape & Data::EdgeMap ()
{ return myEMap; }

//=======================================================================
//function : FaceMap
//purpose  : 
//=======================================================================

inline TopTools_IndexedMapOfShape & Data::FaceMap ()
{ return myFMap; }

//=======================================================================
//function : SimpleHidingFace
//purpose  : 
//=======================================================================
       
inline Standard_Boolean Data::SimpleHidingFace () const
{ return iFaceSimp; }

//=======================================================================
//function : HidingTheFace
//purpose  : 
//=======================================================================
       
inline Standard_Boolean Data::HidingTheFace () const
{ return iFaceTest; }

//=======================================================================
//function : MoreInterference
//purpose  : 
//=======================================================================
       
inline Standard_Boolean Data::MoreInterference () const
{ return (iInterf <= myNbPoints + 2*myNbSegments); }

//=======================================================================
//function : Interference
//purpose  : 
//=======================================================================

inline HLRAlgo_Interference & Data::Interference ()
{ return myIntf; }

//=======================================================================
//function :EdgeOfTheHidingFace
//purpose  : 
//=======================================================================

inline Standard_Boolean
  Data::EdgeOfTheHidingFace (const Standard_Integer,
                             const EdgeData& ED) const
{ return ED.HideCount() == myHideCount-1; }


}
}

#endif // _HLRBRep_Data_HeaderFile
