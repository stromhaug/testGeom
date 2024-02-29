//-----------------------------------------------------------------------------
// Created on: 03 April 2020
//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Sergey Slyadnev
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

// Own include
#include <asiVisu_ThicknessPrs.h>

// asiVisu includes
#include <asiVisu_MeshEScalarPipeline.h>
#include <asiVisu_MeshResultUtils.h>
#include <asiVisu_ThicknessDataProvider.h>
#include <asiVisu_SphereDataProvider.h>
#include <asiVisu_SpherePipeline.h>
#include <asiVisu_MeshEScalarFilter.h>

// VTK includes
#pragma warning(push, 0)
#include <vtkMapper.h>
#include <vtkProperty.h>
#include <vtkCellData.h>
#pragma warning(pop)

//-----------------------------------------------------------------------------

asiVisu_ThicknessPrs::asiVisu_ThicknessPrs(const Handle(ActAPI_INode)& N)
: asiVisu_DefaultPrs(N)
{
  Handle(asiData_ThicknessNode)
    TN = Handle(asiData_ThicknessNode)::DownCast(N);

  // Create Data Provider.
  Handle(asiVisu_ThicknessDataProvider) TDP = new asiVisu_ThicknessDataProvider(TN);
  Handle(asiVisu_SphereDataProvider)    SDS = new asiVisu_SphereDataProvider(TN);

  // Create Pipeline.
  this->addPipeline        ( Pipeline_Main, new asiVisu_MeshEScalarPipeline );
  this->assignDataProvider ( Pipeline_Main, TDP);

  this->addPipeline        ( Pipeline_Sphere, new asiVisu_SpherePipeline );
  this->assignDataProvider ( Pipeline_Sphere, SDS);

  //
  this->GetPipeline(Pipeline_Sphere)->Actor()->SetVisibility(0);

  // Initialize scalar bar.
  m_scalarBarWidget = vtkSmartPointer<vtkScalarBarWidget>::New();
  asiVisu_MeshResultUtils::InitScalarBarWidget(m_scalarBarWidget, 0);
}

//-----------------------------------------------------------------------------

void asiVisu_ThicknessPrs::afterUpdatePipelines() const
{
  m_scalarBarWidget->On();

  // Pipeline for the field.
  Handle(asiVisu_MeshEScalarPipeline)
    pl = Handle(asiVisu_MeshEScalarPipeline)::DownCast( this->GetPipeline(Pipeline_Main) );

  // Initialize scalar bar actor.
  m_scalarBarWidget->GetScalarBarActor()->SetLookupTable( pl->Mapper()->GetLookupTable() );
  m_scalarBarWidget->GetScalarBarActor()->SetTitle("Thickness distribution");

  Handle(asiVisu_SpherePipeline)
    spl = Handle(asiVisu_SpherePipeline)::DownCast(this->GetPipeline(Pipeline_Sphere));
  //spl->Actor()->GetProperty()->SetColor(0., 208., 255.);
  spl->Actor()->GetProperty()->SetLineWidth(3.);
}

//-----------------------------------------------------------------------------

void asiVisu_ThicknessPrs::renderPipelines(vtkRenderer* renderer) const
{
  if ( !m_scalarBarWidget->GetCurrentRenderer() )
  {
    m_scalarBarWidget->SetInteractor( renderer->GetRenderWindow()->GetInteractor() );
    m_scalarBarWidget->SetDefaultRenderer(renderer);
    m_scalarBarWidget->SetCurrentRenderer(renderer);
  }
}

//-----------------------------------------------------------------------------

void asiVisu_ThicknessPrs::deRenderPipelines(vtkRenderer*) const
{
  m_scalarBarWidget->Off();
}

//-----------------------------------------------------------------------------

//! Callback for highlighting.
void asiVisu_ThicknessPrs::highlight(vtkRenderer*                        renderer,
                                     const Handle(asiVisu_PickerResult)& pickRes,
                                     const asiVisu_SelectionNature       /*selNature*/) const
{
  Handle(asiVisu_CellPickerResult)
    cellPickerRes = Handle(asiVisu_CellPickerResult)::DownCast(pickRes);

  const TColStd_PackedMapOfInteger& cellIds = cellPickerRes->GetPickedCellIds();

  if ( cellIds.IsEmpty() )
    return;

  this->GetPipeline(Pipeline_Sphere)->Actor()->SetVisibility(1);

  const int cellId = cellIds.GetMinimalMapped();

  Handle(asiVisu_MeshEScalarPipeline)
    mainPl = Handle(asiVisu_MeshEScalarPipeline)::DownCast(this->GetPipeline(Pipeline_Main));

  // Get cell data for the current mesh.
  vtkDataSet* outputCD = mainPl->Actor()->GetMapper()->GetInput();

  // Get cell with cellId.
  vtkCell* cell = outputCD->GetCell(cellId);

  // Get scalar.
  auto scalar = outputCD->GetCellData()->GetScalars()->GetVariantValue(cellId).ToDouble();

  // Get points of the current cell.
  double p0[3];
  double p1[3];
  double p2[3];

  vtkPoints* pnts = cell->GetPoints();

  pnts->GetPoint(0, p0);
  pnts->GetPoint(1, p1);
  pnts->GetPoint(2, p2);

  // Pipeline for the field.
  Handle(asiVisu_SpherePipeline)
    pl = Handle(asiVisu_SpherePipeline)::DownCast( this->GetPipeline(Pipeline_Sphere) );

  Handle(asiVisu_SphereDataProvider) ds = Handle(asiVisu_SphereDataProvider)::DownCast(dataProvider(Pipeline_Sphere));
  // Set data to sphere data provider.
  ds->SetScalarAndCoords(scalar, p0, p1, p2);

  pl->SetInput(ds);

  renderer->AddActor( pl->Actor() );
}

//-----------------------------------------------------------------------------

//! Callback for highlighting reset.
void asiVisu_ThicknessPrs::unHighlight(vtkRenderer*                  renderer,
                                       const asiVisu_SelectionNature /*selNature*/) const
{
  // Pipeline for the field.
  Handle(asiVisu_SpherePipeline)
    pl = Handle(asiVisu_SpherePipeline)::DownCast(this->GetPipeline(Pipeline_Sphere));

  renderer->RemoveActor(pl->Actor());
}
