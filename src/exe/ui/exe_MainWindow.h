//-----------------------------------------------------------------------------
// Created on: 07 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exe_MainWindow_h
#define exe_MainWindow_h

// asiUI includes
#include <asiUI_ControlsPartResponder.h>
#include <asiUI_ObjectBrowser.h>
#include <asiUI_ViewerDomain.h>
#include <asiUI_ViewerPart.h>
#include <asiUI_ViewerSurface.h>

// Qt includes
#pragma warning(push, 0)
#include <QCloseEvent>
#include <QMainWindow>
#pragma warning(pop)

//! Main window for Analysis Situs.
class exe_MainWindow : public QMainWindow
{
  Q_OBJECT

public:

  exe_MainWindow();
  virtual ~exe_MainWindow();

public:

  void closeEvent(QCloseEvent* evt);

private:

  void createPartViewer();
  void createDockWindows();

private slots:

  void onFacePicked();
  void onEdgePicked();

private:

  //! Widgets.
  struct t_widgets
  {
    asiUI_ObjectBrowser*         wBrowser;          //!< Object browser.
    asiUI_ViewerDomain*          wViewerDomain;     //!< Parametric domain viewer.
    asiUI_ViewerPart*            wViewerPart;       //!< Part viewer.
    asiUI_ViewerSurface*         wViewerSurface;    //!< Surface viewer.
    asiUI_ControlsPart*          wControlsPart;     //!< Part controls.
    asiUI_ControlsPartResponder* pControlsPartResp; //!< Responder for part controls.

    t_widgets() : wBrowser          (NULL),
                  wViewerDomain     (NULL),
                  wViewerPart       (NULL),
                  wViewerSurface    (NULL),
                  wControlsPart     (NULL),
                  pControlsPartResp (NULL)
    {}

    void Release()
    {
      delete wBrowser;          wBrowser          = NULL;
      delete wViewerDomain;     wViewerDomain     = NULL;
      delete wViewerPart;       wViewerPart       = NULL;
      delete wViewerSurface;    wViewerSurface    = NULL;
      delete wControlsPart;     wControlsPart     = NULL;
      delete pControlsPartResp; pControlsPartResp = NULL;
    }
  };

  t_widgets m_widgets;

};

#endif