//-----------------------------------------------------------------------------
// Created on: 08 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef asiData_IVSurfaceNode_h
#define asiData_IVSurfaceNode_h

// asiData includes
#include <asiData.h>

// Active Data includes
#include <ActData_BaseNode.h>

// OCCT includes
#include <Geom_Surface.hxx>

//-----------------------------------------------------------------------------
// Data Node representing a single surface in IV (Imperative Viewer)
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiData_IVSurfaceNode, ActData_BaseNode)

//! Data Node representing a single surface in IV (Imperative Viewer).
class asiData_IVSurfaceNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_IVSurfaceNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_IVSurfaceNode, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //------------------//
  // Common           //
  //------------------//
    PID_Name,         //!< Name of the Node.
  //------------------//
    PID_Geometry,     //!< Stored geometry.
    PID_ULimit,       //!< Value to limit possibly infinite domain in U (absolute).
    PID_VLimit,       //!< Value to limit possibly infinite domain in V (absolute).
  //------------------//
    PID_Last = PID_Name + ActData_BaseNode::RESERVED_PARAM_RANGE
  };

public:

  asiData_EXPORT static Handle(ActAPI_INode)
    Instance();

// Generic naming support:
public:

  asiData_EXPORT virtual TCollection_ExtendedString
    GetName();

  asiData_EXPORT virtual void
    SetName(const TCollection_ExtendedString& name);

// Handy accessors to the stored data:
public:

  asiData_EXPORT Handle(Geom_Surface)
    GetSurface() const;

  asiData_EXPORT void
    SetSurface(const Handle(Geom_Surface)& surface);

  asiData_EXPORT void
    SetLimits(const double uLimit, const double vLimit);

  asiData_EXPORT void
    GetLimits(double& uLimit, double& vLimit) const;

// Initialization:
public:

  asiData_EXPORT void
    Init();

protected:

  //! Allocation is allowed only via Instance method.
  asiData_EXPORT
    asiData_IVSurfaceNode();

};

#endif
