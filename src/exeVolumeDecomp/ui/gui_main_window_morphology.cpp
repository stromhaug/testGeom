//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_main_window_morphology.h>

// Qt includes
#pragma warning(push, 0)
#include <QDesktopWidget>
#include <QDockWidget>
#pragma warning(pop)

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
gui_main_window_morphology::gui_main_window_morphology() : QMainWindow()
{
  this->createPartViewer();
  this->createDockWindows();

  this->setCentralWidget(m_widgets.wViewerPart);
  this->setWindowTitle("Morphology");
}

//! Destructor.
gui_main_window_morphology::~gui_main_window_morphology()
{}

//-----------------------------------------------------------------------------

//! Gets control on window close.
//! \param evt [in] event.
void gui_main_window_morphology::closeEvent(QCloseEvent* evt)
{
  // It seems that we have to destruct objects properly and manually in
  // order to avoid some side effects from VTK. E.g. if we don't kill the
  // widgets explicitly here, we may sometimes get a warning window of VTK
  // saying that it lacks some resources
  m_widgets.Release();
  //
  evt->accept();
}

//-----------------------------------------------------------------------------

//! Creates main (part) viewer.
void gui_main_window_morphology::createPartViewer()
{
  m_widgets.wViewerPart = new gui_viewer_part();

  // Desktop used for sizing
  QDesktopWidget desktop;
  const int side   = std::min( desktop.height(), desktop.width() );
  const int width  = side*0.25;
  const int height = side*0.25;
  //
  m_widgets.wViewerPart->setMinimumSize(width, height);
}

//! Creates main dockable widgets.
void gui_main_window_morphology::createDockWindows()
{
  // Desktop used for sizing
  QDesktopWidget desktop;
  const int side  = std::min( desktop.height(), desktop.width() );
  const int width = side*0.25;

  //---------------------------------------------------------------------------
  // Part controls
  QDockWidget* pDockCommon;
  {
    pDockCommon = new QDockWidget("Common", this);
    pDockCommon->setAllowedAreas(Qt::LeftDockWidgetArea);
    //
    m_widgets.wControlsPart = new gui_controls_part(pDockCommon);
    pDockCommon->setWidget(m_widgets.wControlsPart);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDockCommon);
  }
  //---------------------------------------------------------------------------
  // AAG features controls
  QDockWidget* pDockFeatures;
  {
    pDockFeatures = new QDockWidget("Features", this);
    pDockFeatures->setAllowedAreas(Qt::LeftDockWidgetArea);
    //
    m_widgets.wControlsFeatures = new gui_controls_features(pDockFeatures);
    pDockFeatures->setWidget(m_widgets.wControlsFeatures);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDockFeatures);
  }
  //---------------------------------------------------------------------------
  // Morphology controls
  QDockWidget* pDockMorphology;
  {
    pDockMorphology = new QDockWidget("Morphology", this);
    pDockMorphology->setAllowedAreas(Qt::LeftDockWidgetArea);
    //
    m_widgets.wControlsMorphology = new gui_controls_morphology(pDockMorphology);
    pDockMorphology->setWidget(m_widgets.wControlsMorphology);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDockMorphology);
  }
  //---------------------------------------------------------------------------
  // Object browser
  QDockWidget* pDockBrowser;
  {
    pDockBrowser = new QDockWidget("Model", this);
    pDockBrowser->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //
    m_widgets.wBrowser = new gui_object_browser(pDockBrowser);
    pDockBrowser->setWidget(m_widgets.wBrowser);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDockBrowser);
  }
  //---------------------------------------------------------------------------
  // Face Domain viewer
  {
    QDockWidget* pDock = new QDockWidget("Face Domain", this);
    pDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //
    m_widgets.wViewerDomain = new gui_viewer_domain(pDock);
    pDock->setWidget(m_widgets.wViewerDomain);
    pDock->setMinimumWidth(width);
    //
    this->addDockWidget(Qt::RightDockWidgetArea, pDock);
  }
  //---------------------------------------------------------------------------
  // Surface viewer
  {
    QDockWidget* pDock = new QDockWidget("Host Surface", this);
    pDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //
    m_widgets.wViewerSurface = new gui_viewer_surface(pDock);
    pDock->setWidget(m_widgets.wViewerSurface);
    pDock->setMinimumWidth(width);
    //
    this->addDockWidget(Qt::RightDockWidgetArea, pDock);
  }
  //---------------------------------------------------------------------------
  this->tabifyDockWidget(pDockBrowser,  pDockCommon);
  this->tabifyDockWidget(pDockCommon,   pDockFeatures);
  this->tabifyDockWidget(pDockFeatures, pDockMorphology);
}