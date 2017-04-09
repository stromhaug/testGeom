//-----------------------------------------------------------------------------
// Created on: 30 March 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_PartDataProvider.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! ctor.
asiVisu_PartDataProvider::asiVisu_PartDataProvider()
: asiVisu_DataProvider()
{}

//-----------------------------------------------------------------------------

//! Constructor accepting the data source Node.
//! \param partNode [in] Part Node.
asiVisu_PartDataProvider::asiVisu_PartDataProvider(const Handle(asiData_PartNode)& partNode)
: asiVisu_DataProvider(), m_node(partNode)
{}

//-----------------------------------------------------------------------------

//! Returns ID of the Data Node which is being sourced by the visualization
//! pipeline. This ID is bound to the pipeline's actor in order to have a
//! back-reference from Presentation to Data Object.
//! \return Node ID.
ActAPI_DataObjectId asiVisu_PartDataProvider::GetNodeID() const
{
  return m_node->GetId();
}

//-----------------------------------------------------------------------------

//! \return Part Node.
Handle(asiData_PartNode) asiVisu_PartDataProvider::GetPartNode() const
{
  return m_node;
}

//-----------------------------------------------------------------------------

//! Returns the OCCT model to be visualized.
//! \return OCCT B-Rep model.
TopoDS_Shape asiVisu_PartDataProvider::GetShape() const
{
  return m_node->GetShape();
}

//-----------------------------------------------------------------------------

//! \return AAG.
Handle(asiAlgo_AAG) asiVisu_PartDataProvider::GetAAG() const
{
  return m_node->GetAAG();
}

//-----------------------------------------------------------------------------

double asiVisu_PartDataProvider::GetLinearDeflection() const
{
  return m_node->GetLinearDeflection();
}

//-----------------------------------------------------------------------------

double asiVisu_PartDataProvider::GetAngularDeflection() const
{
  return m_node->GetAngularDeflection();
}

//-----------------------------------------------------------------------------

//! \return copy.
Handle(asiVisu_PartDataProvider) asiVisu_PartDataProvider::Clone() const
{
  return new asiVisu_PartDataProvider(m_node);
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_PartDataProvider::translationSources() const
{
  ActParamStream params;

  params << m_node->Parameter(asiData_PartNode::PID_Geometry)
         << m_node->Parameter(asiData_PartNode::PID_AAG)
         << m_node->Parameter(asiData_PartNode::PID_TessLinDefl)
         << m_node->Parameter(asiData_PartNode::PID_TessAngDefl);

  return params.List;
}