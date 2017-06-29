//-----------------------------------------------------------------------------
// Created on: 06 December 2016
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

// Own include
#include <asiData_RENormalsNode.h>

// asiAlgo includes
#include <asiAlgo_PointCloudUtils.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_RENormalsNode::asiData_RENormalsNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,      PID_Name);
  REGISTER_PARAMETER(RealArray, PID_Normals);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_RENormalsNode::Instance()
{
  return new asiData_RENormalsNode();
}

//! Performs initial actions required to make Node WELL-FORMED.
void asiData_RENormalsNode::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");
  //
  ActParamTool::AsRealArray( this->Parameter(PID_Normals) )->SetArray(NULL);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_RENormalsNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void asiData_RENormalsNode::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy API
//-----------------------------------------------------------------------------

//! Returns normal vector by a given zero-based index.
//! \param zeroBasedIndex [in] zero-based index.
//! \return normal vector.
gp_Vec asiData_RENormalsNode::GetNormal(const int zeroBasedIndex) const
{
  Handle(TColStd_HArray1OfReal)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Normals) )->GetArray();

  const int coordIdx = zeroBasedIndex*3;

  return gp_Vec( coords->Value(coordIdx + 0),
                 coords->Value(coordIdx + 1),
                 coords->Value(coordIdx + 2) );
}

//! \return stored normals in form of base cloud.
Handle(asiAlgo_BaseCloud<double>) asiData_RENormalsNode::GetNormals() const
{
  Handle(TColStd_HArray1OfReal)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Normals) )->GetArray();

  if ( coords.IsNull() )
    return NULL;

  // TODO: get rid of copying here
  return asiAlgo_PointCloudUtils::AsPointCloud(coords);
}

//! \return stored normals with float coordinates.
Handle(asiAlgo_BaseCloud<float>) asiData_RENormalsNode::GetNormalsf() const
{
  Handle(TColStd_HArray1OfReal)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Normals) )->GetArray();

  if ( coords.IsNull() )
    return NULL;

  // TODO: get rid of copying here
  return asiAlgo_PointCloudUtils::AsPointCloudf(coords);
}

//! Stores normals.
//! \param normals [in] normals to store.
void asiData_RENormalsNode::SetNormals(const Handle(asiAlgo_BaseCloud<double>)& normals)
{
  // TODO: get rid of copying here
  Handle(TColStd_HArray1OfReal)
    coords = normals.IsNull() ? NULL : asiAlgo_PointCloudUtils::AsRealArray(normals);

  // Store
  ActParamTool::AsRealArray( this->Parameter(PID_Normals) )->SetArray(coords);
}

//! Stores normals with float coordinates.
//! \param normals [in] normals to store.
void asiData_RENormalsNode::SetNormalsf(const Handle(asiAlgo_BaseCloud<float>)& normals)
{
  // TODO: get rid of copying here
  Handle(TColStd_HArray1OfReal)
    coords = normals.IsNull() ? NULL : asiAlgo_PointCloudUtils::AsRealArray(normals);

  // Store
  ActParamTool::AsRealArray( this->Parameter(PID_Normals) )->SetArray(coords);
}

//! \return true if the stored normal field is empty, false -- otherwise.
bool asiData_RENormalsNode::IsEmpty() const
{
  Handle(TColStd_HArray1OfReal)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Normals) )->GetArray();

  return coords.IsNull();
}
