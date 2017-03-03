//-----------------------------------------------------------------------------
// Created on: 19 September 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiData_ContourNode.h>

// asiAlgo includes
#include <asiAlgo_PointCloudUtils.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// Active Data (auxiliary) includes
#include <ActAux_ArrayUtils.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <ShapeExtend_WireData.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_ContourNode::asiData_ContourNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,      PID_Name);
  REGISTER_PARAMETER(RealArray, PID_Coords);
  REGISTER_PARAMETER(IntArray,  PID_Faces);
  REGISTER_PARAMETER(Bool,      PID_IsClosed);
  REGISTER_PARAMETER(Shape,     PID_Geometry);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_ContourNode::Instance()
{
  return new asiData_ContourNode();
}

//! Performs initial actions required to make the Node WELL-FORMED.
void asiData_ContourNode::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");

  // Set default values to primitive Parameters
  this->SetCoords   ( NULL );
  this->SetFaces    ( NULL );
  this->SetClosed   ( false );
  this->SetGeometry ( TopoDS_Shape() );
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_ContourNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void asiData_ContourNode::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! Sets coordinates of polyline points.
//! \param[in] coords coordinates to store.
void asiData_ContourNode::SetCoords(const Handle(HRealArray)& coords)
{
  ActParamTool::AsRealArray( this->Parameter(PID_Coords) )->SetArray(coords);
}

//! \return stored array.
Handle(HRealArray) asiData_ContourNode::GetCoords() const
{
  return ActParamTool::AsRealArray( this->Parameter(PID_Coords) )->GetArray();
}

//! Replaces a point with the given 0-based index with the passed
//! coordinates.
//! \param[in] zeroBasedIndex 0-based index of the target point.
//! \param[in] point          new coordinates.
void asiData_ContourNode::ReplacePoint(const int     zeroBasedIndex,
                                       const gp_Pnt& point)
{
  // Get array of coordinates
  Handle(ActData_RealArrayParameter)
    coords_p = ActParamTool::AsRealArray( this->Parameter(PID_Coords) );

  // Change values in the Real Array Parameter
  const int startIdx = zeroBasedIndex*3;
  coords_p->SetElement( startIdx,     point.X() );
  coords_p->SetElement( startIdx + 1, point.Y() );
  coords_p->SetElement( startIdx + 2, point.Z() );
}

//! Sets the array of face indices.
//! \param[in] indices indices to set.
void asiData_ContourNode::SetFaces(const Handle(HIntArray)& indices)
{
  ActParamTool::AsIntArray( this->Parameter(PID_Faces) )->SetArray(indices);
}

//! \return array of face indices.
Handle(HIntArray) asiData_ContourNode::GetFaces() const
{
  return ActParamTool::AsIntArray( this->Parameter(PID_Faces) )->GetArray();
}

//! Adds another point to the contour.
//! \param[in] point    point to add.
//! \param[in] face_idx index of the host face.
void asiData_ContourNode::AddPoint(const gp_XYZ& point,
                                   const int     face_idx)
{
  // Change the array of coordinates
  Handle(HRealArray)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Coords) )->GetArray();
  //
  coords = ActAux_ArrayUtils::Append<HRealArray, Handle(HRealArray), double>( coords, point.X() );
  coords = ActAux_ArrayUtils::Append<HRealArray, Handle(HRealArray), double>( coords, point.Y() );
  coords = ActAux_ArrayUtils::Append<HRealArray, Handle(HRealArray), double>( coords, point.Z() );
  //
  this->SetCoords(coords);

  // Change the array of face indices
  Handle(HIntArray)
    faces = ActParamTool::AsIntArray( this->Parameter(PID_Faces) )->GetArray();
  //
  faces = ActAux_ArrayUtils::Append<HIntArray, Handle(HIntArray), int>(faces, face_idx);
  //
  this->SetFaces(faces);
}

//! Sets closeness property for the contour.
//! \param[in] isClosed indicates whether the contour is
//!            closed or not.
void asiData_ContourNode::SetClosed(const bool isClosed)
{
  ActParamTool::AsBool( this->Parameter(PID_IsClosed) )->SetValue(isClosed);
}

//! \return true if the contour is defined as closed,
//!         false -- otherwise.
bool asiData_ContourNode::IsClosed() const
{
  return ActParamTool::AsBool( this->Parameter(PID_IsClosed) )->GetValue();
}

//! Returns B-Rep representation of the contour.
//! \param[in] useCache whether to use cached B-Rep.
//! \return contour converted to shape.
TopoDS_Wire asiData_ContourNode::AsShape(const bool useCache) const
{
  // If there is an alternative representation, let's return it
  TopoDS_Shape derivedRep;
  if ( useCache )
    derivedRep = this->GetGeometry();
  //
  if ( !derivedRep.IsNull() )
    return TopoDS::Wire(derivedRep);

  // If there is no any cached B-Rep, let's build a polyline from the
  // original points
  Handle(asiAlgo_BaseCloud<double>)
    points = asiAlgo_PointCloudUtils::AsPointCloud( this->GetCoords() );
  //
  if ( !points->GetNumberOfElements() )
    return TopoDS_Wire();

  double x0, y0, z0;
  points->GetElement(0, x0, y0, z0);
  //
  TopoDS_Wire   result;
  const int     nPts    = points->GetNumberOfElements();
  TopoDS_Vertex V_first = BRepBuilderAPI_MakeVertex( gp_Pnt(x0, y0, z0) );
  //
  if ( nPts == 1 )
  {
    TopoDS_Edge E;
    BRep_Builder B;
    B.MakeEdge(E);
    B.Degenerated(E, true);
    B.Add(E, V_first);

    B.MakeWire(result);
    B.Add(result, E);
  }
  else
  {
    TopoDS_Vertex                V_prev = V_first, V_last;
    Handle(ShapeExtend_WireData) WD     = new ShapeExtend_WireData;
    //
    for ( int p = 1; p < nPts; ++p )
    {
      double xp, yp, zp;
      points->GetElement(p, xp, yp, zp);
      //
      TopoDS_Vertex V = BRepBuilderAPI_MakeVertex( gp_Pnt(xp, yp, zp) );
      TopoDS_Edge   E = BRepBuilderAPI_MakeEdge(V_prev, V);
      //
      WD->Add(E);
      //
      if ( p == nPts - 1 )
        V_last = V;

      V_prev = V;
    }
    //
    if ( this->IsClosed() )
    {
      TopoDS_Edge E = BRepBuilderAPI_MakeEdge(V_last, V_first);
      WD->Add(E);
    }

    result = WD->WireAPIMake();
  }

  return result;
}

//! Re-packages all coordinates to a convenient sequence structure.
//! \param[out] points result of re-packaging.
//! \param[out] faces  indices of the corresponding faces.
void asiData_ContourNode::AsPointsOnFaces(TColgp_SequenceOfPnt&      points,
                                          TColStd_SequenceOfInteger& faces) const
{
  // Get face indices
  Handle(HIntArray) faceIndices = this->GetFaces();

  // Get coordinates as point cloud
  Handle(asiAlgo_BaseCloud<double>) pcloud = asiAlgo_PointCloudUtils::AsPointCloud( this->GetCoords() );
  const int                         nPts   = pcloud->GetNumberOfElements();
  //
  for ( int p = 0; p < nPts; ++p )
  {
    double xp, yp, zp;
    pcloud->GetElement(p, xp, yp, zp);

    points.Append( gp_Pnt(xp, yp, zp) );
    faces.Append( faceIndices->Value(p) );
  }
}

//! Sets new points and face indices.
//! \param[in] points collection of points.
//! \param[in] faces  collection of face indices.
void asiData_ContourNode::SetPointsOnFaces(const TColgp_SequenceOfPnt&      points,
                                           const TColStd_SequenceOfInteger& faces)
{
  const int numPts = points.Length();

  Handle(HRealArray) coords = new HRealArray(0, numPts*3 - 1);
  Handle(HIntArray)  ifaces = new HIntArray(0, numPts - 1);

  // Fill arrays with data
  for ( int pidx = 1; pidx <= points.Length(); ++pidx )
  {
    coords->ChangeValue( (pidx - 1)*3 )     = points(pidx).X();
    coords->ChangeValue( (pidx - 1)*3 + 1 ) = points(pidx).Y();
    coords->ChangeValue( (pidx - 1)*3 + 2 ) = points(pidx).Z();
    ifaces->ChangeValue( pidx - 1 )         = faces(pidx);
  }

  // Set arrays
  this->SetCoords(coords);
  this->SetFaces(ifaces);
}

//! Sets explicit geometry for the contour.
//! \param[in] shape B-Rep geometry to set.
void asiData_ContourNode::SetGeometry(const TopoDS_Shape& shape)
{
  ActParamTool::AsShape( this->Parameter(PID_Geometry) )->SetShape(shape);
}

//! \return stored B-Rep geometry.
TopoDS_Shape asiData_ContourNode::GetGeometry() const
{
  return ActParamTool::AsShape( this->Parameter(PID_Geometry) )->GetShape();
}
