//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_PointsSource.h>

// asiVisu includes
#include <asiVisu_Utils.h>

// asiAlgo includes
#include <asiAlgo_PointCloudUtils.h>

// VTK includes
#include <vtkCellData.h>
#include <vtkDataObject.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

// Instantiate for allowed types
template class asiVisu_PointsSource<double>;
template class asiVisu_PointsSource<float>;

//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------

vtkStandardNewMacro(asiVisu_PointsSource<double>);
vtkStandardNewMacro(asiVisu_PointsSource<float>);

//! Default constructor.
template <typename REAL_TYPE>
asiVisu_PointsSource<REAL_TYPE>::asiVisu_PointsSource() : vtkPolyDataAlgorithm ()
{
  this->SetNumberOfInputPorts(0); // Connected directly to our own Data Provider
                                  // which has nothing to do with VTK pipeline
}

//! Destructor.
template <typename REAL_TYPE>
asiVisu_PointsSource<REAL_TYPE>::~asiVisu_PointsSource()
{}

//-----------------------------------------------------------------------------
// Kernel methods
//-----------------------------------------------------------------------------

//! Sets input points to visualize.
//! \param points [in] points to visualize.
template <typename REAL_TYPE>
void asiVisu_PointsSource<REAL_TYPE>::SetInputPoints(const Handle(asiAlgo_BaseCloud<REAL_TYPE>)& points)
{
  m_points = points;
  //
  this->Modified();
}

//-----------------------------------------------------------------------------

//! This is called by the superclass. Creates VTK polygonal data set
//! from the input arrays.
//! \param request      [in] information about data object.
//! \param inputVector  [in] the input data. As a data source is the start
//!                          stage of the VTK pipeline, the Input Vector is
//!                          empty and not used (no input port).
//! \param outputVector [in] the pointer to output data, that is filled
//!                          in this method.
//! \return status.
template <typename REAL_TYPE>
int asiVisu_PointsSource<REAL_TYPE>::RequestData(vtkInformation*        request,
                                                 vtkInformationVector** inputVector,
                                                 vtkInformationVector*  outputVector)
{
  if ( m_points.IsNull() )
  {
    vtkErrorMacro( << "Invalid domain: NULL point cloud" );
    return 0;
  }

  Handle(TColStd_HArray1OfReal) coords = asiAlgo_PointCloudUtils::AsRealArray(m_points);
  //
  if ( coords.IsNull() )
  {
    vtkErrorMacro( << "Invalid domain: NULL point cloud" );
    return 0;
  }

  /* ==============================
   *  Prepare involved collections
   * ============================== */

  vtkPolyData* polyOutput = vtkPolyData::GetData(outputVector);
  polyOutput->Allocate();

  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  polyOutput->SetPoints(points);

  //---------------------------------------------------------------------------

  for ( int i = coords->Lower(); i <= coords->Upper() - 2; i += 3 )
  {
    gp_Pnt P( coords->Value(i), coords->Value(i + 1), coords->Value(i + 2) );
    //
    this->registerVertex( this->registerGridPoint(P, polyOutput), polyOutput );
  }

  return Superclass::RequestData(request, inputVector, outputVector);
}

//! Adds the given point to the passed polygonal data set.
//! \param point    [in]     point to add.
//! \param polyData [in/out] polygonal data set being populated.
//! \return ID of the just added VTK point.
template <typename REAL_TYPE>
vtkIdType asiVisu_PointsSource<REAL_TYPE>::registerGridPoint(const gp_Pnt& point,
                                                             vtkPolyData*  polyData)
{
  // Access necessary arrays
  vtkPoints* points = polyData->GetPoints();

  // Push the point into VTK data set
  vtkIdType pid = points->InsertNextPoint( point.X(),
                                           point.Y(),
                                           point.Z() );

  return pid;
}

//! Adds a vertex cell into the polygonal data set.
//! \param n        [in]     index of the corresponding geometric point.
//! \param polyData [in/out] polygonal data set being populated.
//! \return ID of the just added VTK cell.
template <typename REAL_TYPE>
vtkIdType asiVisu_PointsSource<REAL_TYPE>::registerVertex(const vtkIdType n,
                                                          vtkPolyData*    polyData)
{
  std::vector<vtkIdType> nodes;
  nodes.push_back(n);
  //
  vtkIdType cellID = polyData->InsertNextCell(VTK_VERTEX, 1, &nodes[0]);
  //
  return cellID;
}
