//-----------------------------------------------------------------------------
// Created on: 28 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef asiData_PartNode_h
#define asiData_PartNode_h

// asiData includes
#include <asiData_AAGParameter.h>
#include <asiData_BoundaryEdgesNode.h>
#include <asiData_ContourNode.h>
#include <asiData_CurveNode.h>
#include <asiData_EdgeNode.h>
#include <asiData_FaceContourNode.h>
#include <asiData_FaceNode.h>
#include <asiData_FaceNormsNode.h>
#include <asiData_SurfNode.h>

// Active Data includes
#include <ActData_ShapeParameter.h>

//-----------------------------------------------------------------------------
// Part Node
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiData_PartNode, ActData_BaseNode)

//! Node representing b-rep part.
class asiData_PartNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_PartNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_PartNode, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //------------------//
  // Common           //
  //------------------//
    PID_Name,         //!< Name of the Node.
  //------------------//
  // Geometry         //
  //------------------//
    PID_Geometry,     //!< B-Rep structure.
    PID_AAG,          //!< AAG structure.
  //------------------//
  // Presentation     //
  //------------------//
    PID_TessLinDefl,  //!< Linear deflection of visualization tessellation.
    PID_TessAngDefl,  //!< Angular deflection of visualization tessellation.
    PID_DisplayMode,  //!< Display mode.
    PID_HasColor,     //!< Indicates whether the Color Parameter is active.
    PID_Color,        //!< Color.
    PID_HasVertices,  //!< Indicates whether vertices are active.
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
    SetName(const TCollection_ExtendedString&);

// Handy accessors to the stored data:
public:

  asiData_EXPORT TopoDS_Shape
    GetShape() const;

  asiData_EXPORT Handle(asiAlgo_AAG)
    GetAAG() const;

  asiData_EXPORT void
    SetLinearDeflection(const double);

  asiData_EXPORT double
    GetLinearDeflection() const;

  asiData_EXPORT void
    SetAngularDeflection(const double);

  asiData_EXPORT double
    GetAngularDeflection() const;

  asiData_EXPORT void
    SetHasColor(const bool);

  asiData_EXPORT bool
    HasColor() const;

  asiData_EXPORT void
    SetColor(const int);

  asiData_EXPORT int
    GetColor() const;

  asiData_EXPORT void
    SetDisplayMode(const int);

  asiData_EXPORT int
    GetDisplayMode() const;

  asiData_EXPORT void
    SetHasVertices(const bool);

  asiData_EXPORT bool
    HasVertices() const;

  //-------------------------------------------------------------------------//

  asiData_EXPORT Handle(asiData_FaceNode)
    GetFaceRepresentation() const;

  asiData_EXPORT Handle(asiData_FaceNormsNode)
    GetNormsRepresentation() const;

  asiData_EXPORT Handle(asiData_SurfNode)
    GetSurfaceRepresentation() const;

  asiData_EXPORT Handle(asiData_FaceContourNode)
    GetContourRepresentation() const;

  asiData_EXPORT Handle(asiData_EdgeNode)
    GetEdgeRepresentation() const;

  asiData_EXPORT Handle(asiData_CurveNode)
    GetCurveRepresentation() const;

  asiData_EXPORT Handle(asiData_BoundaryEdgesNode)
    GetBoundaryEdgesRepresentation() const;

  asiData_EXPORT Handle(asiData_ContourNode)
    GetContour() const;

// Initialization:
public:

  asiData_EXPORT void
    Init();

protected:

  //! Allocation is allowed only via Instance method.
  asiData_EXPORT
    asiData_PartNode();

protected:

  //! We prohibit to set shape from external code as such update should
  //! normally include update of AAG. Therefore, there should be
  //! "smart update" defined at the upper level of software architecture.
  asiData_EXPORT void
    setShape(const TopoDS_Shape&);

  //! See comment for setShape() method.
  asiData_EXPORT void
    setAAG(const Handle(asiAlgo_AAG)&);

};

#endif
