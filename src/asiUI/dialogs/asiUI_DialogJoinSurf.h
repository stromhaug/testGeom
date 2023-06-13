//-----------------------------------------------------------------------------
// Created on: 26 May 2023
//-----------------------------------------------------------------------------
// Copyright (c) 2023-present, Sergey Slyadnev
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

#ifndef asiUI_DialogJoinSurf_h
#define asiUI_DialogJoinSurf_h

// asiUI includes
#include <asiUI_LineEdit.h>
#include <asiUI_ViewerPart.h>
#include <asiUI_WidgetFactory.h>

// asiEngine include
#include <asiEngine_Model.h>

// Qt includes
#include <Standard_WarningsDisable.hxx>
//
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
//
#include <Standard_WarningsRestore.hxx>

//-----------------------------------------------------------------------------

//! Dialog for joining neighbor surfaces into a single surface across their
//! common boundary.
class asiUI_DialogJoinSurf : public QDialog
{
  Q_OBJECT

public:

  //! Ctor.
  //! \param[in] wf       the widget factory.
  //! \param[in] model    the data model instance.
  //! \param[in] pViewer  the connected viewer.
  //! \param[in] progress the progress notifier.
  //! \param[in] plotter  the imperative plotter.
  //! \param[in] parent   the parent widget.
  asiUI_EXPORT
    asiUI_DialogJoinSurf(const Handle(asiUI_WidgetFactory)& wf,
                         const Handle(asiEngine_Model)&     model,
                         asiUI_ViewerPart*                  pViewer,
                         ActAPI_ProgressEntry               progress,
                         ActAPI_PlotterEntry                plotter,
                         QWidget*                           parent = nullptr);

  //! Dtor.
  asiUI_EXPORT virtual
    ~asiUI_DialogJoinSurf();

public slots:

  //! Reaction on face selection in the viewer.
  void onFacePicked();

  //! Reaction on clicking "Apply" button.
  void onApply();

  //! Reaction on visual diagnostics (on/off).
  void onDiagnostics();

protected:

  //! Widgets.
  struct t_widgets
  {
    QPushButton*    pApply;         //!< Performs surface concatenation.
    QPushButton*    pClose;         //!< Closes the dialog.
    asiUI_LineEdit* pFaces;         //!< Indices of the faces to join.
    asiUI_Datum*    pNumProfilesS1; //!< Number of profiles on the 1-st surface.
    asiUI_Datum*    pNumProfilesS2; //!< Number of profiles on the 2-nd surface.
    asiUI_Datum*    pNumGuides;     //!< Number of guide curves.
    asiUI_Datum*    pOffset;        //!< Offset from the common edge for smooth transitioning.
    QCheckBox*      pDiagnostics;   //!< Visual diagnostics on/off.

    //! Default ctor.
    t_widgets() : pApply         (nullptr),
                  pClose         (nullptr),
                  pFaces         (nullptr),
                  pNumProfilesS1 (nullptr),
                  pNumProfilesS2 (nullptr),
                  pNumGuides     (nullptr),
                  pOffset        (nullptr),
                  pDiagnostics   (nullptr)
    {}

    void Release()
    {
      delete pApply;         pApply         = nullptr;
      delete pClose;         pClose         = nullptr;
      delete pFaces;         pFaces         = nullptr;
      delete pNumProfilesS1; pNumProfilesS1 = nullptr;
      delete pNumProfilesS2; pNumProfilesS2 = nullptr;
      delete pNumGuides;     pNumGuides     = nullptr;
      delete pOffset;        pOffset        = nullptr;
      delete pDiagnostics;   pDiagnostics   = nullptr;
    }
  };

  t_widgets               m_widgets;      //!< UI controls.
  QVBoxLayout*            m_pMainLayout;  //!< Layout of the widget.
  asiUI_ViewerPart*       m_pViewer;      //!< External reference to viewer.
  Handle(asiEngine_Model) m_model;        //!< Data Model instance.
  bool                    m_bDiagnostics; //!< Visual diagnostics on/off.

  /* Diagnostics */

  ActAPI_ProgressEntry m_progress; //!< Progress notifier.
  ActAPI_PlotterEntry  m_plotter;  //!< Imperative plotter.

};

#endif // asiUI_DialogJoinSurf_h