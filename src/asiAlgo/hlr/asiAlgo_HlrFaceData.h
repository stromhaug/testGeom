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

#ifndef asiAlgo_HLRBRep_FaceData_HeaderFile
#define asiAlgo_HLRBRep_FaceData_HeaderFile

// asiAlgo includes
#include <asiAlgo_HlrSurface.h>

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Standard_ShortReal.hxx>
#include <TopAbs_Orientation.hxx>
#include <Standard_Integer.hxx>

class HLRAlgo_WiresBlock;
class TopoDS_Face;

namespace asiAlgo {
namespace hlr {

class FaceData
{
public:

  DEFINE_STANDARD_ALLOC

  asiAlgo_EXPORT FaceData();

  //! <Or> is the orientation of the face.  <Cl> is true
  //! if the face  belongs to a  closed  volume. <NW> is
  //! the number of wires ( or block  of  edges ) of the
  //! face.
  asiAlgo_EXPORT void Set (const TopoDS_Face& FG, const TopAbs_Orientation Or, const Standard_Boolean Cl, const Standard_Integer NW);

  //! Set <NE> the number  of  edges of the wire  number
  //! <WI>.
  asiAlgo_EXPORT void SetWire (const Standard_Integer WI, const Standard_Integer NE);

  //! Set the edge number <EWI> of the  wire <WI>.
  asiAlgo_EXPORT void SetWEdge (const Standard_Integer WI, const Standard_Integer EWI, const Standard_Integer EI, const TopAbs_Orientation Or, const Standard_Boolean OutL, const Standard_Boolean Inte, const Standard_Boolean Dble, const Standard_Boolean IsoL);

  asiAlgo_EXPORT Standard_Boolean Selected() const;

  asiAlgo_EXPORT void Selected (const Standard_Boolean B);

  asiAlgo_EXPORT Standard_Boolean Back() const;

  asiAlgo_EXPORT void Back (const Standard_Boolean B);

  asiAlgo_EXPORT Standard_Boolean Side() const;

  asiAlgo_EXPORT void Side (const Standard_Boolean B);

  asiAlgo_EXPORT Standard_Boolean Closed() const;

  asiAlgo_EXPORT void Closed (const Standard_Boolean B);

  asiAlgo_EXPORT Standard_Boolean Hiding() const;

  asiAlgo_EXPORT void Hiding (const Standard_Boolean B);

  asiAlgo_EXPORT Standard_Boolean Simple() const;

  asiAlgo_EXPORT void Simple (const Standard_Boolean B);

  asiAlgo_EXPORT Standard_Boolean Cut() const;

  asiAlgo_EXPORT void Cut (const Standard_Boolean B);

  asiAlgo_EXPORT Standard_Boolean WithOutL() const;

  asiAlgo_EXPORT void WithOutL (const Standard_Boolean B);

  asiAlgo_EXPORT Standard_Boolean Plane() const;

  asiAlgo_EXPORT void Plane (const Standard_Boolean B);

  asiAlgo_EXPORT Standard_Boolean Cylinder() const;

  asiAlgo_EXPORT void Cylinder (const Standard_Boolean B);

  asiAlgo_EXPORT Standard_Boolean Cone() const;

  asiAlgo_EXPORT void Cone (const Standard_Boolean B);

  asiAlgo_EXPORT Standard_Boolean Sphere() const;

  asiAlgo_EXPORT void Sphere (const Standard_Boolean B);

  asiAlgo_EXPORT Standard_Boolean Torus() const;

  asiAlgo_EXPORT void Torus (const Standard_Boolean B);

  asiAlgo_EXPORT Standard_Real Size() const;

  asiAlgo_EXPORT void Size (const Standard_Real S);

  asiAlgo_EXPORT TopAbs_Orientation Orientation() const;

  asiAlgo_EXPORT void Orientation (const TopAbs_Orientation O);

  asiAlgo_EXPORT Handle(HLRAlgo_WiresBlock)& Wires();

  asiAlgo_EXPORT Surface& Geometry();

  asiAlgo_EXPORT Standard_ShortReal Tolerance() const;

protected:

  enum EMaskFlags
  {
    EMaskOrient   = 15,
    FMaskSelected = 16,
    FMaskBack     = 32,
    FMaskSide     = 64,
    FMaskClosed   = 128,
    FMaskHiding   = 256,
    FMaskSimple   = 512,
    FMaskCut      = 1024,
    FMaskWithOutL = 2048,
    FMaskPlane    = 4096,
    FMaskCylinder = 8192,
    FMaskCone     = 16384,
    FMaskSphere   = 32768,
    FMaskTorus    = 65536
  };

private:

  Standard_Integer myFlags;
  Handle(HLRAlgo_WiresBlock) myWires;
  Surface myGeometry;
  Standard_Real mySize;
  Standard_ShortReal myTolerance;

};

//=======================================================================
//function : Selected
//purpose  : 
//=======================================================================

inline Standard_Boolean FaceData::Selected() const
{ return (myFlags & FMaskSelected) != 0; }

//=======================================================================
//function : Selected
//purpose  : 
//=======================================================================

inline void FaceData::Selected(const Standard_Boolean B)
{
  if (B) myFlags |=  FMaskSelected;
  else   myFlags &= ~FMaskSelected;
}

//=======================================================================
//function : Back
//purpose  : 
//=======================================================================

inline Standard_Boolean FaceData::Back() const
{ return (myFlags & FMaskBack) != 0; }

//=======================================================================
//function : Back
//purpose  : 
//=======================================================================

inline void FaceData::Back(const Standard_Boolean B)
{
  if (B) myFlags |=  FMaskBack;
  else   myFlags &= ~FMaskBack;
}

//=======================================================================
//function : Side
//purpose  : 
//=======================================================================

inline Standard_Boolean FaceData::Side() const
{ return (myFlags & FMaskSide) != 0; }

//=======================================================================
//function : Side
//purpose  : 
//=======================================================================

inline void FaceData::Side(const Standard_Boolean B)
{
  if (B) myFlags |=  FMaskSide;
  else   myFlags &= ~FMaskSide;
}

//=======================================================================
//function : Closed
//purpose  : 
//=======================================================================

inline Standard_Boolean FaceData::Closed() const
{ return (myFlags & FMaskClosed) != 0; }

//=======================================================================
//function : Closed
//purpose  : 
//=======================================================================

inline void FaceData::Closed(const Standard_Boolean B)
{
  if (B) myFlags |=  FMaskClosed;
  else   myFlags &= ~FMaskClosed;
}

//=======================================================================
//function : Hiding
//purpose  : 
//=======================================================================

inline Standard_Boolean FaceData::Hiding() const
{ return (myFlags & FMaskHiding) != 0; }

//=======================================================================
//function : Hiding
//purpose  : 
//=======================================================================

inline void FaceData::Hiding(const Standard_Boolean B)
{
  if (B) myFlags |=  FMaskHiding;
  else   myFlags &= ~FMaskHiding;
}

//=======================================================================
//function : Simple
//purpose  : 
//=======================================================================

inline Standard_Boolean FaceData::Simple() const
{ return (myFlags & FMaskSimple) != 0; }

//=======================================================================
//function : Simple
//purpose  : 
//=======================================================================

inline void FaceData::Simple(const Standard_Boolean B)
{
  if (B) myFlags |=  FMaskSimple;
  else   myFlags &= ~FMaskSimple;
}

//=======================================================================
//function : Cut
//purpose  : 
//=======================================================================

inline Standard_Boolean FaceData::Cut() const
{ return (myFlags & FMaskCut) != 0; }

//=======================================================================
//function : Cut
//purpose  : 
//=======================================================================

inline void FaceData::Cut(const Standard_Boolean B)
{
  if (B) myFlags |=  FMaskCut;
  else   myFlags &= ~FMaskCut;
}

//=======================================================================
//function : WithOutL
//purpose  : 
//=======================================================================

inline Standard_Boolean FaceData::WithOutL() const
{ return (myFlags & FMaskWithOutL) != 0; }

//=======================================================================
//function : WithOutL
//purpose  : 
//=======================================================================

inline void FaceData::WithOutL(const Standard_Boolean B)
{
  if (B) myFlags |=  FMaskWithOutL;
  else   myFlags &= ~FMaskWithOutL;
}

//=======================================================================
//function : Plane
//purpose  : 
//=======================================================================

inline Standard_Boolean FaceData::Plane() const
{ return (myFlags & FMaskPlane) != 0; }

//=======================================================================
//function : Plane
//purpose  : 
//=======================================================================

inline void FaceData::Plane(const Standard_Boolean B)
{
  if (B) myFlags |=  FMaskPlane;
  else   myFlags &= ~FMaskPlane;
}

//=======================================================================
//function : Cylinder
//purpose  : 
//=======================================================================

inline Standard_Boolean FaceData::Cylinder() const
{ return (myFlags & FMaskCylinder) != 0; }

//=======================================================================
//function : Cylinder
//purpose  : 
//=======================================================================

inline void FaceData::Cylinder(const Standard_Boolean B)
{
  if (B) myFlags |=  FMaskCylinder;
  else   myFlags &= ~FMaskCylinder;
}

//=======================================================================
//function : Cone
//purpose  : 
//=======================================================================

inline Standard_Boolean FaceData::Cone() const
{ return (myFlags & FMaskCone) != 0; }

//=======================================================================
//function : Cone
//purpose  : 
//=======================================================================

inline void FaceData::Cone(const Standard_Boolean B)
{
  if (B) myFlags |=  FMaskCone;
  else   myFlags &= ~FMaskCone;
}

//=======================================================================
//function : Sphere
//purpose  : 
//=======================================================================

inline Standard_Boolean FaceData::Sphere() const
{ return (myFlags & FMaskSphere) != 0; }

//=======================================================================
//function : Sphere
//purpose  : 
//=======================================================================

inline void FaceData::Sphere(const Standard_Boolean B)
{
  if (B) myFlags |=  FMaskSphere;
  else   myFlags &= ~FMaskSphere;
}

//=======================================================================
//function : Torus
//purpose  : 
//=======================================================================

inline Standard_Boolean FaceData::Torus() const
{ return (myFlags & FMaskTorus) != 0; }

//=======================================================================
//function : Torus
//purpose  : 
//=======================================================================

inline void FaceData::Torus(const Standard_Boolean B)
{
  if (B) myFlags |=  FMaskTorus;
  else   myFlags &= ~FMaskTorus;
}

//=======================================================================
//function : Size
//purpose  : 
//=======================================================================

inline Standard_Real FaceData::Size() const
{ return mySize; }

//=======================================================================
//function : Size
//purpose  : 
//=======================================================================

inline void FaceData::Size(const Standard_Real S)
{ mySize = S; }

//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

inline TopAbs_Orientation FaceData::Orientation() const
{ return ((TopAbs_Orientation)(myFlags & EMaskOrient)); }

//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

inline void FaceData::Orientation(const TopAbs_Orientation O)
{
  myFlags &= ~EMaskOrient;
  myFlags |= (O & EMaskOrient);
}

//=======================================================================
//function : Wires
//purpose  : 
//=======================================================================

inline Handle(HLRAlgo_WiresBlock)& FaceData::Wires()
{ return myWires; }

//=======================================================================
//function : Geometry
//purpose  : 
//=======================================================================

inline Surface& FaceData::Geometry()
{ return myGeometry; }

//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

inline Standard_ShortReal FaceData::Tolerance () const
{ return myTolerance; }

}
}

#endif // _HLRBRep_FaceData_HeaderFile
