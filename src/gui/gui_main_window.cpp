//-----------------------------------------------------------------------------
// Created on: 07 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <gui_main_window.h>

// Qt includes
#pragma warning(push, 0)
#include <QDesktopWidget>
#include <QDockWidget>
#pragma warning(pop)

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
gui_main_window::gui_main_window() : QMainWindow()
{
  this->createPartViewer();
  this->createStatusBar();
  this->createDockWindows();

  this->setCentralWidget(m_widgets.wViewerPart);
  this->setWindowTitle("Analysis Situs");
}

//! Destructor.
gui_main_window::~gui_main_window()
{}

//-----------------------------------------------------------------------------

//! Creates main (part) viewer.
void gui_main_window::createPartViewer()
{
  m_widgets.wViewerPart = new gui_viewer_part();

  // Desktop used for sizing
  QDesktopWidget desktop;
  const int side   = std::min( desktop.height(), desktop.width() );
  const int width  = side*0.6;
  const int height = side*0.6;
  //
  m_widgets.wViewerPart->setMinimumSize(width, height);
}

//! Creates status bar.
void gui_main_window::createStatusBar()
{
}

//! Creates main dockable widgets.
void gui_main_window::createDockWindows()
{
  // Desktop used for sizing
  QDesktopWidget desktop;
  const int side  = std::min( desktop.height(), desktop.width() );
  const int width = side*0.4;

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
  // Object browser
  {
    QDockWidget* pDock = new QDockWidget("Stored Objects", this);
    pDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //
    m_widgets.wBrowser = new gui_object_browser(pDock);
    pDock->setWidget(m_widgets.wBrowser);
    //
    this->addDockWidget(Qt::LeftDockWidgetArea, pDock);
  }
}
