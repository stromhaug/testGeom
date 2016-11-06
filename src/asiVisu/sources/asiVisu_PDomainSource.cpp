//-----------------------------------------------------------------------------
// Created on: 30 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_PDomainSource.h>

// A-Situs (visualization) includes
#include <asiVisu_PCurveSource.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkAppendPolyData.h>
#include <vtkCellData.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>

// OCCT includes
#include <BRepTools.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------

vtkStandardNewMacro(asiVisu_PDomainSource);

//! Default constructor.
asiVisu_PDomainSource::asiVisu_PDomainSource() : vtkPolyDataAlgorithm()
{
  this->SetNumberOfInputPorts(0); // Connected directly to our own Data Provider
                                  // which has nothing to do with VTK pipeline
}

//! Destructor.
asiVisu_PDomainSource::~asiVisu_PDomainSource()
{
}

//-----------------------------------------------------------------------------
// Kernel methods
//-----------------------------------------------------------------------------

//! Sets input geometry.
//! \param face [in] face to render the parametric portrait for.
void asiVisu_PDomainSource::SetFace(const TopoDS_Face& face)
{
  m_face = face;
}

//-----------------------------------------------------------------------------

//! This is called by the superclass. Creates VTK polygonal data set
//! from the input arrays.
//! \param request      [in] information about data object.
//! \param inputVector  [in] the input data. As a data source is the start
//!                          stage of the VTK pipeline, theInputVector is
//!                          empty and not used (no input port).
//! \param outputVector [in] the pointer to output data, that is filled
//!                          in this method.
//! \return status.
int asiVisu_PDomainSource::RequestData(vtkInformation*        request,
                                     vtkInformationVector** inputVector,
                                     vtkInformationVector*  outputVector)
{
  if ( m_face.IsNull() )
  {
    vtkErrorMacro( << "Invalid domain: NULL face" );
    return 0;
  }

  /* ==============================
   *  Prepare involved collections
   * ============================== */

  vtkPolyData* polyOutput = vtkPolyData::GetData(outputVector);
  polyOutput->Allocate();

  // Compute tip size for the orientation markers
  const double tip_size = this->computeTipSize();

  // Append filter
  vtkSmartPointer<vtkAppendPolyData>
    appendFilter = vtkSmartPointer<vtkAppendPolyData>::New();

  // Explode by edges
  int edgeId = 0;
  for ( TopExp_Explorer exp(m_face.Oriented(TopAbs_FORWARD), TopAbs_EDGE); exp.More(); exp.Next() )
  {
    ++edgeId;

    // Allocate Data Source
    vtkSmartPointer<asiVisu_PCurveSource>
      pcurveSource = vtkSmartPointer<asiVisu_PCurveSource>::New();

    // Access edge
    const TopoDS_Edge& E = TopoDS::Edge( exp.Current() );

    // Initialize data source
    pcurveSource->SetEdgeOnFace(E, m_face);
    pcurveSource->SetTipSize(tip_size);
    pcurveSource->SetPedigreeId(edgeId);

    // Append poly data
    appendFilter->AddInputConnection( pcurveSource->GetOutputPort() );
  }

  appendFilter->Update();
  //
  polyOutput->DeepCopy( appendFilter->GetOutput() );

  return Superclass::RequestData(request, inputVector, outputVector);
}

//-----------------------------------------------------------------------------

//! Computes size for orientation tip glyph. This size is decided for the
//! whole face to have it identical for all edges.
//! \return tip size.
double asiVisu_PDomainSource::computeTipSize() const
{
  double uMin, uMax, vMin, vMax;
  BRepTools::UVBounds(m_face, uMin, uMax, vMin, vMax);

  // Take a ratio of a bounding diagonal
  return ( gp_XY(uMax, vMax) - gp_XY(uMin, vMin) ).Modulus() * 0.025;
}