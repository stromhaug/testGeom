//-----------------------------------------------------------------------------
// Created on: 08 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2016-present, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef asiData_IVNode_h
#define asiData_IVNode_h

// Visualization includes
#include <asiData_IVCurves2dNode.h>
#include <asiData_IVCurvesNode.h>
#include <asiData_IVPoints2dNode.h>
#include <asiData_IVPointsNode.h>
#include <asiData_IVVectorsNode.h>
#include <asiData_IVSurfacesNode.h>
#include <asiData_IVTessNode.h>
#include <asiData_IVTextNode.h>
#include <asiData_IVTopoNode.h>

//-----------------------------------------------------------------------------

//! Root Node for all items visualized in the imperative viewer.
class asiData_IVNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_IVNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_IVNode, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //------------------//
  // Common           //
  //------------------//
    PID_Name,         //!< Name of the Node.
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

  asiData_EXPORT Handle(asiData_IVPoints2dNode)
    Points2d();

  asiData_EXPORT Handle(asiData_IVPointsNode)
    Points();

  asiData_EXPORT Handle(asiData_IVVectorsNode)
    Vectors();

  asiData_EXPORT Handle(asiData_IVCurves2dNode)
    Curves2d();

  asiData_EXPORT Handle(asiData_IVCurvesNode)
    Curves();

  asiData_EXPORT Handle(asiData_IVSurfacesNode)
    Surfaces();

  asiData_EXPORT Handle(asiData_IVTopoNode)
    Topology();

  asiData_EXPORT Handle(asiData_IVTessNode)
    Tessellation();

  asiData_EXPORT Handle(asiData_IVTextNode)
    Text();

// Initialization:
public:

  asiData_EXPORT void
    Init();

protected:

  //! Allocation is allowed only via Instance() method.
  asiData_EXPORT
    asiData_IVNode();

private:

  enum Child
  {
    Child_Points2d = 1,
    Child_Points,
    Child_Vectors,
    Child_Curves2d,
    Child_Curves,
    Child_Surfaces,
    Child_Topology,
    Child_Tessellation,
    Child_Text
  };

};

#endif
