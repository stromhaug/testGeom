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

#ifndef asiAlgo_HLRBRep_EdgeData_HeaderFile
#define asiAlgo_HLRBRep_EdgeData_HeaderFile

// asiAlgo includes
#include <asiAlgo_HlrCurve.h>

#include <HLRAlgo_WiresBlock.hxx>

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <HLRAlgo_EdgeStatus.hxx>
#include <Standard_ShortReal.hxx>
#include <Standard_Real.hxx>

class TopoDS_Edge;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

namespace asiAlgo {
namespace hlr {

class EdgeData
{
public:

  DEFINE_STANDARD_ALLOC

  EdgeData() :
    myFlags(0),
    myHideCount(0)
  {
    Selected(Standard_True);
  }

  asiAlgo_EXPORT void Set (const Standard_Boolean Reg1, const Standard_Boolean RegN, const TopoDS_Edge& EG, const Standard_Integer V1, const Standard_Integer V2, const Standard_Boolean Out1, const Standard_Boolean Out2, const Standard_Boolean Cut1, const Standard_Boolean Cut2, const Standard_Real Start, const Standard_ShortReal TolStart, const Standard_Real End, const Standard_ShortReal TolEnd);

  asiAlgo_EXPORT Standard_Boolean Selected() const;

  asiAlgo_EXPORT void Selected (const Standard_Boolean B);

  asiAlgo_EXPORT Standard_Boolean Rg1Line() const;

  asiAlgo_EXPORT void Rg1Line (const Standard_Boolean B);

  asiAlgo_EXPORT Standard_Boolean RgNLine() const;

  asiAlgo_EXPORT void RgNLine (const Standard_Boolean B);

  asiAlgo_EXPORT Standard_Boolean Vertical() const;

  asiAlgo_EXPORT void Vertical (const Standard_Boolean B);

  asiAlgo_EXPORT Standard_Boolean Simple() const;

  asiAlgo_EXPORT void Simple (const Standard_Boolean B);

  asiAlgo_EXPORT Standard_Boolean OutLVSta() const;

  asiAlgo_EXPORT void OutLVSta (const Standard_Boolean B);

  asiAlgo_EXPORT Standard_Boolean OutLVEnd() const;

  asiAlgo_EXPORT void OutLVEnd (const Standard_Boolean B);

  asiAlgo_EXPORT Standard_Boolean CutAtSta() const;

  asiAlgo_EXPORT void CutAtSta (const Standard_Boolean B);

  asiAlgo_EXPORT Standard_Boolean CutAtEnd() const;

  asiAlgo_EXPORT void CutAtEnd (const Standard_Boolean B);

  asiAlgo_EXPORT Standard_Boolean VerAtSta() const;

  asiAlgo_EXPORT void VerAtSta (const Standard_Boolean B);

  asiAlgo_EXPORT Standard_Boolean VerAtEnd() const;

  asiAlgo_EXPORT void VerAtEnd (const Standard_Boolean B);

  asiAlgo_EXPORT Standard_Boolean AutoIntersectionDone() const;

  asiAlgo_EXPORT void AutoIntersectionDone (const Standard_Boolean B);

  asiAlgo_EXPORT Standard_Boolean Used() const;

  asiAlgo_EXPORT void Used (const Standard_Boolean B);

  asiAlgo_EXPORT Standard_Integer HideCount() const;

  asiAlgo_EXPORT void HideCount (const Standard_Integer I);

  asiAlgo_EXPORT Standard_Integer VSta() const;

  asiAlgo_EXPORT void VSta (const Standard_Integer I);

  asiAlgo_EXPORT Standard_Integer VEnd() const;

  asiAlgo_EXPORT void VEnd (const Standard_Integer I);

  void UpdateMinMax (const HLRAlgo_EdgesBlock::MinMaxIndices& theTotMinMax)
  {
    myMinMax = theTotMinMax;
  }

  HLRAlgo_EdgesBlock::MinMaxIndices& MinMax()
  {
    return myMinMax;
  }

  asiAlgo_EXPORT HLRAlgo_EdgeStatus& Status();

  asiAlgo_EXPORT Curve& ChangeGeometry();

  asiAlgo_EXPORT const Curve& Geometry() const;

    Curve* AccessCurve()
    {
      return &myGeometry;
    }

  asiAlgo_EXPORT Standard_ShortReal Tolerance() const;

protected:

  enum EMaskFlags
  {
    EMaskSelected = 1,
    EMaskUsed     = 2,
    EMaskRg1Line  = 4,
    EMaskVertical = 8,
    EMaskSimple   = 16,
    EMaskOutLVSta = 32,
    EMaskOutLVEnd = 64,
    EMaskIntDone  = 128,
    EMaskCutAtSta = 256,
    EMaskCutAtEnd = 512,
    EMaskVerAtSta = 1024,
    EMaskVerAtEnd = 2048,
    EMaskRgNLine  = 4096
  };

private:

  Standard_Integer myFlags;
  Standard_Integer myHideCount;
  Standard_Integer myVSta;
  Standard_Integer myVEnd;
  HLRAlgo_EdgesBlock::MinMaxIndices myMinMax;
  HLRAlgo_EdgeStatus myStatus;
  Curve myGeometry;
  Standard_ShortReal myTolerance;

};

//=======================================================================
//function : Selected
//purpose  : 
//=======================================================================

inline Standard_Boolean EdgeData::Selected () const
{ return (myFlags & EMaskSelected) != 0; }

//=======================================================================
//function : Selected
//purpose  : 
//=======================================================================

inline void EdgeData::Selected (const Standard_Boolean B)
{
  if (B) myFlags |=  EMaskSelected;
  else   myFlags &= ~EMaskSelected;
}

//=======================================================================
//function : Rg1Line
//purpose  : 
//=======================================================================

inline Standard_Boolean EdgeData::Rg1Line () const
{ return (myFlags & EMaskRg1Line) != 0; }

//=======================================================================
//function : Rg1Line
//purpose  : 
//=======================================================================

inline void EdgeData::Rg1Line (const Standard_Boolean B)
{
  if (B) myFlags |=  EMaskRg1Line;
  else   myFlags &= ~EMaskRg1Line;
}

//=======================================================================
//function : RgNLine
//purpose  : 
//=======================================================================

inline Standard_Boolean EdgeData::RgNLine () const
{ return (myFlags & EMaskRgNLine) != 0; }

//=======================================================================
//function : RgNLine
//purpose  : 
//=======================================================================

inline void EdgeData::RgNLine (const Standard_Boolean B)
{
  if (B) myFlags |=  EMaskRgNLine;
  else   myFlags &= ~EMaskRgNLine;
}

//=======================================================================
//function : Vertical
//purpose  : 
//=======================================================================

inline Standard_Boolean EdgeData::Vertical () const
{ return (myFlags & EMaskVertical) != 0; }

//=======================================================================
//function : Vertical
//purpose  : 
//=======================================================================

inline void EdgeData::Vertical(const Standard_Boolean B)
{
  if (B) myFlags |=  EMaskVertical;
  else   myFlags &= ~EMaskVertical;
}

//=======================================================================
//function : Simple
//purpose  : 
//=======================================================================

inline Standard_Boolean EdgeData::Simple () const
{ return (myFlags & EMaskSimple) != 0; }

//=======================================================================
//function : Simple
//purpose  : 
//=======================================================================

inline void EdgeData::Simple (const Standard_Boolean B)
{
  if (B) myFlags |=  EMaskSimple;
  else   myFlags &= ~EMaskSimple;
}

//=======================================================================
//function : OutLVSta
//purpose  : 
//=======================================================================

inline Standard_Boolean EdgeData::OutLVSta () const
{ return (myFlags & EMaskOutLVSta) != 0; }

//=======================================================================
//function : OutLVSta
//purpose  : 
//=======================================================================

inline void EdgeData::OutLVSta (const Standard_Boolean B)
{
  if (B) myFlags |=  EMaskOutLVSta;
  else   myFlags &= ~EMaskOutLVSta;
}

//=======================================================================
//function : OutLVEnd
//purpose  : 
//=======================================================================

inline Standard_Boolean EdgeData::OutLVEnd () const
{ return (myFlags & EMaskOutLVEnd) != 0; }

//=======================================================================
//function : OutLVEnd
//purpose  : 
//=======================================================================

inline void EdgeData::OutLVEnd (const Standard_Boolean B)
{
  if (B) myFlags |=  EMaskOutLVEnd;
  else   myFlags &= ~EMaskOutLVEnd;
}

//=======================================================================
//function : CutAtSta
//purpose  : 
//=======================================================================

inline Standard_Boolean EdgeData::CutAtSta () const
{ return (myFlags & EMaskCutAtSta) != 0; }

//=======================================================================
//function : CutAtSta
//purpose  : 
//=======================================================================

inline void EdgeData::CutAtSta (const Standard_Boolean B)
{
  if (B) myFlags |=  EMaskCutAtSta;
  else   myFlags &= ~EMaskCutAtSta;
}

//=======================================================================
//function : CutAtEnd
//purpose  : 
//=======================================================================

inline Standard_Boolean EdgeData::CutAtEnd () const
{ return (myFlags & EMaskCutAtEnd) != 0; }

//=======================================================================
//function : CutAtEnd
//purpose  : 
//=======================================================================

inline void EdgeData::CutAtEnd (const Standard_Boolean B)
{
  if (B) myFlags |=  EMaskCutAtEnd;
  else   myFlags &= ~EMaskCutAtEnd;
}

//=======================================================================
//function : VerAtSta
//purpose  : 
//=======================================================================

inline Standard_Boolean EdgeData::VerAtSta () const
{ return (myFlags & EMaskVerAtSta) != 0; }

//=======================================================================
//function : VerAtSta
//purpose  : 
//=======================================================================

inline void EdgeData::VerAtSta (const Standard_Boolean B)
{
  if (B) myFlags |=  EMaskVerAtSta;
  else   myFlags &= ~EMaskVerAtSta;
}

//=======================================================================
//function : VerAtEnd
//purpose  : 
//=======================================================================

inline Standard_Boolean EdgeData::VerAtEnd () const
{ return (myFlags & EMaskVerAtEnd) != 0; }

//=======================================================================
//function : VerAtEnd
//purpose  : 
//=======================================================================

inline void EdgeData::VerAtEnd (const Standard_Boolean B)
{
  if (B) myFlags |=  EMaskVerAtEnd;
  else   myFlags &= ~EMaskVerAtEnd;
}

//=======================================================================
//function : AutoIntersectionDone
//purpose  : 
//=======================================================================

inline Standard_Boolean EdgeData::AutoIntersectionDone () const
{ return (myFlags & EMaskIntDone) != 0; }

//=======================================================================
//function : AutoIntersectionDone
//purpose  : 
//=======================================================================

inline void 
EdgeData::AutoIntersectionDone (const Standard_Boolean B)
{
  if (B) myFlags |=  EMaskIntDone;
  else   myFlags &= ~EMaskIntDone;
}

//=======================================================================
//function : Used
//purpose  : 
//=======================================================================

inline Standard_Boolean EdgeData::Used () const
{ return (myFlags & EMaskUsed) != 0; }

//=======================================================================
//function : Used
//purpose  : 
//=======================================================================

inline void EdgeData::Used (const Standard_Boolean B)
{
  if (B) myFlags |=  EMaskUsed;
  else   myFlags &= ~EMaskUsed;
}

//=======================================================================
//function : HideCount
//purpose  : 
//=======================================================================

inline Standard_Integer EdgeData::HideCount () const
{ return myHideCount; }

//=======================================================================
//function : HideCount
//purpose  : 
//=======================================================================

inline void EdgeData::HideCount (const Standard_Integer I)
{ myHideCount = I; }

//=======================================================================
//function : VSta
//purpose  : 
//=======================================================================

inline Standard_Integer EdgeData::VSta() const
{ return myVSta; }

//=======================================================================
//function : VSta
//purpose  : 
//=======================================================================

inline void EdgeData::VSta (const Standard_Integer I)
{ myVSta = I; }

//=======================================================================
//function : VEnd
//purpose  : 
//=======================================================================

inline Standard_Integer EdgeData::VEnd () const
{ return myVEnd; }

//=======================================================================
//function : VEnd
//purpose  : 
//=======================================================================

inline void EdgeData::VEnd (const Standard_Integer I)
{ myVEnd = I; }

//=======================================================================
//function : Status
//purpose  : 
//=======================================================================

inline HLRAlgo_EdgeStatus & EdgeData::Status ()
{ return myStatus; }

//=======================================================================
//function : ChangeGeometry
//purpose  : 
//=======================================================================

inline Curve & EdgeData::ChangeGeometry ()
{ return myGeometry; }

//=======================================================================
//function : Geometry
//purpose  : 
//=======================================================================

inline const Curve & EdgeData::Geometry () const
{ return myGeometry; }

//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

inline Standard_ShortReal EdgeData::Tolerance () const
{ return myTolerance; }

}
}

#endif // _HLRBRep_EdgeData_HeaderFile
