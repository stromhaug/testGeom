//-----------------------------------------------------------------------------
// Created on: 14 February 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018, Sergey Slyadnev
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
//    * Neither the name of Sergey Slyadnev nor the
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
#include <asiUI_CurvaturePlot.h>

// asiVisu includes
#include <asiVisu_Utils.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// VTK includes
#include <vtkAxis.h>
#include <vtkChartXY.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkFloatArray.h>
#include <vtkPen.h>
#include <vtkPlot.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>
#include <vtkTable.h>
#include <vtkVersion.h>

// Qt includes
#include <QApplication>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param[in] progress progress notifier.
//! \param[in] plotter  imperative plotter.
//! \param[in] parent   parent object.
asiUI_CurvaturePlot::asiUI_CurvaturePlot(ActAPI_ProgressEntry           progress,
                                         ActAPI_PlotterEntry            plotter,
                                         QObject*                       parent)
: QObject        (parent),
  m_pWidget      (NULL),
  m_progress     (progress),
  m_plotter      (plotter)
{}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_CurvaturePlot::~asiUI_CurvaturePlot()
{}

//-----------------------------------------------------------------------------

//! Renders curvature plot.
//! \param[in] params     discretization parameters along the curve in question.
//! \param[in] curvatures curvature values.
void asiUI_CurvaturePlot::Render(const std::vector<double>& params,
                                 const std::vector<double>& curvatures)
{
  /* =================
   *  Fill data table
   * ================= */

  vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();

  vtkSmartPointer<vtkFloatArray> arrX = vtkSmartPointer<vtkFloatArray>::New();
  arrX->SetName("Parameter");
  table->AddColumn(arrX);

  vtkSmartPointer<vtkFloatArray> arrC = vtkSmartPointer<vtkFloatArray>::New();
  arrC->SetName("Curvature");
  table->AddColumn(arrC);

  // Fill data table.
  const int numPoints = int ( params.size() );
  table->SetNumberOfRows(numPoints);
  //
  for ( int i = 0; i < numPoints; ++i )
  {
    table->SetValue(i, 0, params[i]);
    table->SetValue(i, 1, curvatures[i]);
  }

  /* =======================
   *  Prepare visualization
   * ======================= */

  // Set up the view.
  m_contextView = vtkSmartPointer<vtkContextView>::New();
  //
  m_contextView->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
  //
  m_contextView->GetRenderWindow()->SetMultiSamples(8);

  // Create Qt window. NOTICE that interactor should be already given to
  // the render window at this stage.
  m_pWidget = new asiUI_VtkWindow();
  m_pWidget->SetRenderWindow( m_contextView->GetRenderWindow() );

  // Add multiple line plots, setting the colors, etc.
  vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();
  m_contextView->GetScene()->AddItem(chart);
  vtkPlot* line = chart->AddPlot(vtkChart::LINE);
  //
  line->SetInputData(table, 0, 1);
  line->SetColor(225, 0, 0, 255);
  line->SetWidth(2.0f);
  //
  line->GetXAxis()->SetTitle("Parameter");
  line->GetXAxis()->GetTitleProperties()->BoldOff();
  line->GetXAxis()->GetTitleProperties()->SetLineOffset(8);
  line->GetXAxis()->GetLabelProperties()->BoldOff();
  line->GetXAxis()->GetLabelProperties()->SetColor(0.25, 0.25, 0.25);
  line->GetXAxis()->GetLabelProperties()->SetLineOffset(4);
  //
  line->GetYAxis()->SetTitle("Curvature");
  line->GetYAxis()->GetTitleProperties()->BoldOff();
  line->GetYAxis()->GetTitleProperties()->SetLineOffset(8);
  line->GetYAxis()->GetLabelProperties()->BoldOff();
  line->GetYAxis()->GetLabelProperties()->SetColor(0.25, 0.25, 0.25);
  line->GetYAxis()->GetLabelProperties()->SetLineOffset(4);

  /* ===================
   *  Start interaction
   * =================== */

  // Set callback on mouse events: otherwise QVTKWindow is
  // not updated for some reason.
  connect( m_pWidget, SIGNAL( mouseEvent(QMouseEvent*) ),
           this,      SLOT( onMouseEvent() ) );
  //
  connect( m_pWidget, SIGNAL( wheelEventOccured() ),
           this,      SLOT( onMouseEvent() ) );

  m_contextView->GetRenderWindow()->SetWindowName("Curvature plot");
  //
  m_contextView->GetInteractor()->Initialize();
  //
  m_pWidget->resize(400, 400);
  m_pWidget->show();
}

//-----------------------------------------------------------------------------

void asiUI_CurvaturePlot::onMouseEvent()
{
  m_pWidget->repaint();
}