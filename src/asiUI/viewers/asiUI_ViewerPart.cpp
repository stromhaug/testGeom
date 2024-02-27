//-----------------------------------------------------------------------------
// Created on: 27 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
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
#include <asiUI_ViewerPart.h>

// asiAlgo includes
#include <asiAlgo_BuildHLR.h>
#include <asiAlgo_SuppressFeatures.h>
#include <asiAlgo_Timer.h>

// asiVisu includes
#include <asiVisu_NodeInfo.h>
#include <asiVisu_Utils.h>

// asiUI includes
#include <asiUI_ControlsPart.h>
#include <asiUI_DialogFindEdge.h>
#include <asiUI_DialogFindFace.h>
#include <asiUI_DialogFindVertex.h>
#include <asiUI_DialogRefineTessellation.h>
#include <asiUI_IV.h>

// asiEngine includes
#include <asiEngine_Part.h>

// VTK includes
#pragma warning(push, 0)
#include <vtkAssembly.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#pragma warning(pop)

// Qt-VTK includes
#pragma warning(push, 0)
#include <asiVisu_QVTKWidget.h>
#pragma warning(pop)

// Qt includes
#pragma warning(push, 0)
#include <QDesktopWidget>
#include <QVBoxLayout>
#pragma warning(pop)

// OCCT includes
#include <BRepPrimAPI_MakeBox.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

namespace
{
  void MakeHLR(const Handle(asiEngine_Model)&             model,
               const vtkSmartPointer<asiVisu_PrsManager>& prsMgr,
               const asiAlgo_BuildHLR::Mode               mode,
               const t_asciiString&                       name,
               const ActAPI_Color&                        color,
               ActAPI_ProgressEntry                       progress,
               ActAPI_PlotterEntry                        plotter)
  {
    // Read part shape.
    TopoDS_Shape partShape = model->GetPartNode()->GetShape();
    //
    if ( partShape.IsNull() )
      return;

    // Root Node stores global HLR settings.
    Handle(asiData_RootNode)
      rootNode = Handle(asiData_RootNode)::DownCast( model->GetRootNode() );

    // Read projection direction.
    double dX, dY, dZ;
    //
    prsMgr->GetRenderer()->GetActiveCamera()->GetViewPlaneNormal(dX, dY, dZ);

    TIMER_NEW
    TIMER_GO

    asiAlgo_BuildHLR::ClearThreads(progress);

    // Set a filter for the hidden edges.
    asiAlgo_BuildHLR::t_outputEdges filter;
    //
    if ( rootNode->IsEnabledHiddenInHlr() )
    {
      filter.OutputHiddenSharpEdges   = true;
      filter.OutputHiddenOutlineEdges = true;
      filter.OutputHiddenSmoothEdges  = true;
      filter.OutputHiddenIsoLines     = true;
      filter.OutputHiddenSewnEdges    = true;
    }

    // Build HLR. In precise mode, we use timeout to be able to fallback to the
    // discrete HLR if the precise one takes too long (i.e., exceeds the timeout
    // defined in the root Node).
    asiAlgo_BuildHLR buildHLR(partShape, progress, plotter);
    //
    if ( mode == asiAlgo_BuildHLR::Mode_Precise )
    {
      if ( !buildHLR.PerformParallel(gp_Dir(dX, dY, dZ), 0, rootNode->GetHlrTimeout(), filter) )
      {
        progress.SendLogMessage(LogErr(Normal) << "Cannot build HLR: try increasing timeout or use "
                                                  "\"Alt+H\" key combination for enforced discrete HLR.");
        return;
      }
    }
    else
    {
      if ( !buildHLR.Perform(gp_Dir(dX, dY, dZ), asiAlgo_BuildHLR::Mode_Discrete, filter) )
      {
        progress.SendLogMessage(LogErr(Normal) << "Cannot build HLR.");
        return;
      }
    }

    TIMER_FINISH
    TIMER_COUT_RESULT_NOTIFIER(progress, "HLR projection")

    // Draw the result with the default color.
    plotter.REDRAW_SHAPE(name, buildHLR.GetResult(), color);
  }

  void MakeHLRBox(const Handle(asiEngine_Model)& model,
                  const asiAlgo_BuildHLR::Mode   mode,
                  const t_asciiString&           name,
                  const ActAPI_Color&            color,
                  ActAPI_ProgressEntry           progress,
                  ActAPI_PlotterEntry            plotter)
  {
    // Read part shape.
    TopoDS_Shape partShape = model->GetPartNode()->GetShape();
    //
    if ( partShape.IsNull() )
      return;

    // Root Node stores global HLR settings.
    Handle(asiData_RootNode)
      rootNode = Handle(asiData_RootNode)::DownCast( model->GetRootNode() );

    // Bounding box of the shape.
    double xMin, yMin, zMin, xMax, yMax, zMax;
    asiAlgo_Utils::Bounds(partShape, xMin, yMin, zMin, xMax, yMax, zMax, 0., false);

    const double padding = Min(xMax - xMin, Min(yMax - yMin, zMax - zMin))*0.2;
    //
    xMin -= padding;
    yMin -= padding;
    zMin -= padding;
    xMax += padding;
    yMax += padding;
    zMax += padding;

    // Create bounding box to draw it.
    TopoDS_Shape bndbox = BRepPrimAPI_MakeBox( gp_Pnt(xMin, yMin, zMin), gp_Pnt(xMax, yMax, zMax) );
    //
    plotter.REDRAW_SHAPE("bbox", bndbox, ActAPI_Color(40./255., 40./255., 40./255., Quantity_TOC_RGB), 1.0);

    // Projection directions.
    std::vector<gp_Dir> dirs;
    dirs.push_back(  gp::DX() );
    dirs.push_back( -gp::DX() );
    dirs.push_back(  gp::DY() );
    dirs.push_back( -gp::DY() );
    dirs.push_back(  gp::DZ() );
    dirs.push_back( -gp::DZ() );

    // Set a filter for the hidden edges.
    asiAlgo_BuildHLR::t_outputEdges filter;
    //
    if ( Handle(asiData_RootNode)::DownCast( model->GetRootNode() )->IsEnabledHiddenInHlr() )
    {
      filter.OutputHiddenSharpEdges   = true;
      filter.OutputHiddenOutlineEdges = true;
      filter.OutputHiddenSmoothEdges  = true;
      filter.OutputHiddenIsoLines     = true;
      filter.OutputHiddenSewnEdges    = true;
    }

    TIMER_NEW
    TIMER_GO

    asiAlgo_BuildHLR::ClearThreads(progress);

    // Prepare HLR tool.
    asiAlgo_BuildHLR buildHLR( partShape,
                               progress,
                               plotter );
    //
    size_t memchunk = 0;
    //
    for ( const auto& dir : dirs )
    {
      if ( mode == asiAlgo_BuildHLR::Mode_Precise )
      {
        if ( !buildHLR.PerformParallel(dir, memchunk, rootNode->GetHlrTimeout(), filter) )
        {
          progress.SendLogMessage(LogErr(Normal) << "Cannot build HLR: try increasing timeout or use "
                                                    "\"Alt+H\" key combination for enforced discrete HLR.");
          return;
        }
      }
      else
      {
        if ( !buildHLR.Perform(dir, asiAlgo_BuildHLR::Mode_Discrete, filter) )
        {
          progress.SendLogMessage(LogErr(Normal) << "Cannot build HLR.");
          return;
        }
      }
      //
      memchunk += 2;

      TopoDS_Shape proj = buildHLR.GetResult();

      // Prepare a local reference frame for the projection.
      double projXmin, projYmin, projZmin, projXmax, projYmax, projZmax;
      asiAlgo_Utils::Bounds(proj, projXmin, projYmin, projZmin, projXmax, projYmax, projZmax, 0., false);
      //
      gp_Ax3               T_B( gp_Pnt( (projXmin + projXmax)*0.5,
                                        (projYmin + projYmax)*0.5,
                                        (projZmin + projZmax)*0.5), dir );
      //
      tl::optional<gp_Ax3> T_A = asiAlgo_Utils::GetBboxSideFrame(dir, xMin, yMin, zMin, xMax, yMax, zMax);
      //
      if ( !T_A.has_value() )
        continue;

      gp_Trsf T = asiAlgo_Utils::GetAlignmentTrsf(*T_A, T_B);
      //
      proj.Move(T);

      std::string projName = name.ToCString();
      projName += "_";
      projName += asiAlgo_Utils::DirectionName(dir);

      // Draw the result with the default color.
      plotter.REDRAW_SHAPE(projName.c_str(), proj, color);
    }

    TIMER_FINISH
    TIMER_COUT_RESULT_NOTIFIER(progress, "HLR box projection")
  }
}

//-----------------------------------------------------------------------------

void GetPickedSubshapeIds(const Handle(asiVisu_PickerResult)& pick_res,
                          std::vector<int>&                   picked_subshape_IDs,
                          std::vector<ActAPI_DataObjectId>&   picked_node_IDs)
{
  Handle(asiVisu_CellPickerResult)
    cellPickRes = Handle(asiVisu_CellPickerResult)::DownCast(pick_res);

  const vtkSmartPointer<vtkActor>&  picked_actor  = cellPickRes->GetPickedActor();
  const TColStd_PackedMapOfInteger& subshape_mask = cellPickRes->GetPickedElementIds();

  if ( !picked_actor.GetPointer() )
    return; // Nothing selected

  // Retrieve Node information
  asiVisu_NodeInfo* pNodeInfo = asiVisu_NodeInfo::Retrieve(picked_actor);
  //
  if ( !pNodeInfo )
    return;

  // Retrieve the corresponding Node ID by picked Actor
  ActAPI_DataObjectId picked_node_id = pNodeInfo->GetNodeId();

  // Fill coherent collections of references: sub-shape IDs against owning Nodes
  for ( TColStd_MapIteratorOfPackedMapOfInteger maskIt(subshape_mask); maskIt.More(); maskIt.Next() )
  {
    picked_subshape_IDs.push_back( maskIt.Key() );
    picked_node_IDs.push_back(picked_node_id);
  }

#if defined COUT_DEBUG
  if ( picked_subshape_IDs.size() )
  {
    std::cout << "Picked sub-shapes:";
    for ( size_t k = 0; k < picked_subshape_IDs.size(); ++k )
    {
      std::cout << " " << picked_subshape_IDs[k] << " [" << picked_node_IDs[k].ToCString() << "]";
    }
    std::cout << std::endl;
  }
#endif
}

//-----------------------------------------------------------------------------

//! Creates a new instance of viewer.
//! \param[in] model             Data Model instance.
//! \param[in] enableInteraction enables/disables interaction mechanisms.
//! \param[in] isOffscreen       enables/disables offscreen rendering mode.
//! \param[in] progress          progress notifier.
//! \param[in] plotter           imperative plotter.
//! \param[in] parent            parent widget.
asiUI_ViewerPart::asiUI_ViewerPart(const Handle(asiEngine_Model)& model,
                                   const bool                     enableInteraction,
                                   const bool                     isOffscreen,
                                   ActAPI_ProgressEntry           progress,
                                   ActAPI_PlotterEntry            plotter,
                                   QWidget*                       parent)
: asiUI_Viewer(progress, plotter, parent), m_model(model)
{
  // Initialize presentation manager along with QVTK widget
  m_prs_mgr = vtkSmartPointer<asiVisu_PrsManager>::New();
  //
  m_prs_mgr->SetModel(model);
  m_prs_mgr->Initialize(this, isOffscreen);
  m_prs_mgr->SetInteractionMode(asiVisu_PrsManager::InteractionMode_3D);
  m_prs_mgr->SetSelectionMode(SelectionMode_Face);

  // Widgets and layouts
  asiVisu_QVTKWidget* pViewer     = m_prs_mgr->GetQVTKWidget();
  QVBoxLayout*        pBaseLayout = new QVBoxLayout(this);

  // Create toolbar
  m_toolBar = new QToolBar(this);
  m_toolBar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  // Create actions
  this->createActions();

  // Configure layout
  pBaseLayout->addWidget(m_toolBar, 0);
  pBaseLayout->addWidget(pViewer,   1);
  //
  pBaseLayout->setSpacing(0);
  pBaseLayout->setContentsMargins(0, 0, 0, 0);

  /* ===================================
   *  Setting up picking infrastructure
   * =================================== */

  if ( enableInteraction )
  {
    // Initialize Callback instance for Pick operation
    m_pickCallback = vtkSmartPointer<asiUI_PickCallback>::New();
    m_pickCallback->SetViewer(this);
    m_pickCallback->SetModel(m_model);
    m_pickCallback->SetPickerTypes(PickerType_Cell);

    // Initialize Callback instance for handling events related to Part viewer
    m_partCallback = vtkSmartPointer<asiUI_PartCallback>::New();
    m_partCallback->SetViewer(this);

    // Set observer for picking
    if ( !m_prs_mgr->GetDefaultInteractorStyle()->HasObserver(EVENT_PICK_DEFAULT) )
      m_prs_mgr->GetDefaultInteractorStyle()->AddObserver(EVENT_PICK_DEFAULT, m_pickCallback);

    // Set observer for detection
    if ( !m_prs_mgr->GetDefaultInteractorStyle()->HasObserver(EVENT_DETECT_DEFAULT) )
      m_prs_mgr->GetDefaultInteractorStyle()->AddObserver(EVENT_DETECT_DEFAULT, m_pickCallback);

    // Set observer for finding face
    if ( !m_prs_mgr->GetDefaultInteractorStyle()->HasObserver(EVENT_FIND_FACE) )
      m_prs_mgr->GetDefaultInteractorStyle()->AddObserver(EVENT_FIND_FACE, m_partCallback);

    // Set observer for finding edge
    if ( !m_prs_mgr->GetDefaultInteractorStyle()->HasObserver(EVENT_FIND_EDGE) )
      m_prs_mgr->GetDefaultInteractorStyle()->AddObserver(EVENT_FIND_EDGE, m_partCallback);

    // Set observer for finding vertex
    if ( !m_prs_mgr->GetDefaultInteractorStyle()->HasObserver(EVENT_FIND_VERTEX) )
      m_prs_mgr->GetDefaultInteractorStyle()->AddObserver(EVENT_FIND_VERTEX, m_partCallback);

    // Set observer for tessellation refinement
    if ( !m_prs_mgr->GetDefaultInteractorStyle()->HasObserver(EVENT_REFINE_TESSELLATION) )
      m_prs_mgr->GetDefaultInteractorStyle()->AddObserver(EVENT_REFINE_TESSELLATION, m_partCallback);

    // Set observer for selecting all faces
    if ( !m_prs_mgr->GetDefaultInteractorStyle()->HasObserver(EVENT_SELECT_ALL) )
      m_prs_mgr->GetDefaultInteractorStyle()->AddObserver(EVENT_SELECT_ALL, m_partCallback);

    // Set observer for HLR
    if ( !m_prs_mgr->GetDefaultInteractorStyle()->HasObserver(EVENT_BUILD_HLR) )
      m_prs_mgr->GetDefaultInteractorStyle()->AddObserver(EVENT_BUILD_HLR, m_partCallback);

    // Set observer for discrete HLR
    if ( !m_prs_mgr->GetDefaultInteractorStyle()->HasObserver(EVENT_BUILD_HLR_DISCR) )
      m_prs_mgr->GetDefaultInteractorStyle()->AddObserver(EVENT_BUILD_HLR_DISCR, m_partCallback);

    // Set observer for HLR (box mode)
    if ( !m_prs_mgr->GetDefaultInteractorStyle()->HasObserver(EVENT_BUILD_HLR_BOX) )
      m_prs_mgr->GetDefaultInteractorStyle()->AddObserver(EVENT_BUILD_HLR_BOX, m_partCallback);

    // Set observer for discrete HLR (box mode)
    if ( !m_prs_mgr->GetDefaultInteractorStyle()->HasObserver(EVENT_BUILD_HLR_DISCR_BOX) )
      m_prs_mgr->GetDefaultInteractorStyle()->AddObserver(EVENT_BUILD_HLR_DISCR_BOX, m_partCallback);

    // Set observer for defeaturing
    if ( !m_prs_mgr->GetDefaultInteractorStyle()->HasObserver(EVENT_DEFEATURE) )
      m_prs_mgr->GetDefaultInteractorStyle()->AddObserver(EVENT_DEFEATURE, m_partCallback);

    // Get notified once a sub-shape is picked
    connect( m_pickCallback, SIGNAL( picked() ), this, SLOT( onSubShapesPicked() ) );
    connect( m_pickCallback, SIGNAL( picked() ), this, SLOT( onWhateverPicked() ) );

    // Get notified once a sub-shape is highlighted
    connect(m_pickCallback, SIGNAL(highlighted()), this, SLOT( onWhateverHighlighted() ) );

    // Get notified about part events
    connect( m_partCallback, SIGNAL( findFace() ),           this, SLOT( onFindFace() ) );
    connect( m_partCallback, SIGNAL( findEdge() ),           this, SLOT( onFindEdge() ) );
    connect( m_partCallback, SIGNAL( findVertex() ),         this, SLOT( onFindVertex() ) );
    connect( m_partCallback, SIGNAL( refineTessellation() ), this, SLOT( onRefineTessellation() ) );
    connect( m_partCallback, SIGNAL( buildHLR() ),           this, SLOT( onBuildHLR() ) );
    connect( m_partCallback, SIGNAL( buildHLRDiscr() ),      this, SLOT( onBuildHLRDiscr() ) );
    connect( m_partCallback, SIGNAL( buildHLRBox() ),        this, SLOT( onBuildHLRBox() ) );
    connect( m_partCallback, SIGNAL( buildHLRDiscrBox() ),   this, SLOT( onBuildHLRDiscrBox() ) );
    connect( m_partCallback, SIGNAL( selectAll() ),          this, SLOT( onSelectAll() ) );
    connect( m_partCallback, SIGNAL( defeature() ),          this, SLOT( onDefeature() ) );

    /* ===============================
     *  Setting up rotation callbacks
     * =============================== */

    // Initialize Callback instance for rotation
    m_rotoCallback = vtkSmartPointer<asiUI_RotationCallback>::New();
    m_rotoCallback->SetViewer(this);

    // Set observer for starting rotation
    if ( !m_prs_mgr->GetDefaultInteractorStyle()->HasObserver(EVENT_ROTATION_START) )
      m_prs_mgr->GetDefaultInteractorStyle()->AddObserver(EVENT_ROTATION_START, m_rotoCallback);

    // Set observer for ending rotation
    if ( !m_prs_mgr->GetDefaultInteractorStyle()->HasObserver(EVENT_ROTATION_END) )
      m_prs_mgr->GetDefaultInteractorStyle()->AddObserver(EVENT_ROTATION_END, m_rotoCallback);
  }

  /* ========================
   *  Initialize axes widget
   * ======================== */

  vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
  axes->GetZAxisShaftProperty()->SetColor(ZAXIS_R, ZAXIS_G, ZAXIS_B);
  axes->GetZAxisTipProperty()->SetColor(ZAXIS_R, ZAXIS_G, ZAXIS_B);

  vtkSmartPointer<vtkAssembly>  assm = vtkSmartPointer<vtkAssembly>::New();
  assm->AddPart(axes);
  //
  m_axesWidget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
  m_axesWidget->SetOrientationMarker(assm);
  //
  vtkRenderer* renderer = m_prs_mgr->GetRenderer();
  renderer->SetRenderWindow( m_prs_mgr->GetRenderWindow() );
  //
  m_axesWidget->SetCurrentRenderer( m_prs_mgr->GetRenderer() );
  m_axesWidget->SetInteractor( m_prs_mgr->GetRenderWindow()->GetInteractor() );
  m_axesWidget->SetEnabled(1);
  m_axesWidget->SetInteractive(0);
  m_axesWidget->SetViewport(0, 0, 0.25, 0.25);

  /* =====================================
   *  Finalize initial state of the scene
   * ===================================== */

  if ( pViewer )
  {
    pViewer->setContextMenuPolicy(Qt::CustomContextMenu);
    connect( pViewer, SIGNAL ( customContextMenuRequested(const QPoint&) ),
             this,    SLOT   ( onContextMenu(const QPoint&) ) );

    this->onResetView();
  }
}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_ViewerPart::~asiUI_ViewerPart()
{}

//-----------------------------------------------------------------------------

//! \return size hint.
QSize asiUI_ViewerPart::sizeHint() const
{
  QDesktopWidget desktop;
  const int side   = std::min( desktop.height(), desktop.width() );
  const int width  = (int) (side*0.25);
  const int height = (int) (side*0.5);

  QSize s(width, height);
  return s;
}

//-----------------------------------------------------------------------------

//! Updates viewer.
void asiUI_ViewerPart::Repaint()
{
  if ( m_prs_mgr->GetQVTKWidget() )
    m_prs_mgr->GetQVTKWidget()->GetRenderWindow()->Render();
}

//-----------------------------------------------------------------------------

//! Resets view.
void asiUI_ViewerPart::onResetView()
{
  asiVisu_Utils::ResetCamera( m_prs_mgr->GetRenderer() );
  //
  this->Repaint();
}

//-----------------------------------------------------------------------------

//! Fits view to the contents.
void asiUI_ViewerPart::onFitAll()
{
  asiVisu_Utils::AdjustCamera( m_prs_mgr->GetRenderer(),
                               m_prs_mgr->PropsByTrihedron() );
  //
  this->Repaint();
}

//-----------------------------------------------------------------------------

//! Callback for picking event.
void asiUI_ViewerPart::onWhateverPicked()
{
  // Access picking results.
  const asiVisu_ActualSelection&          sel      = m_prs_mgr->GetCurrentSelection();
  const Handle(asiVisu_CellPickerResult)& pick_res = sel.GetCellPickerResult(SelectionNature_Persistent);

  // We don't care of picking logic here and let the listener react.
  emit whateverPicked( pick_res.get() );
}

//-----------------------------------------------------------------------------

//! Callback for picking event.
void asiUI_ViewerPart::onSubShapesPicked()
{
  // Access picking results
  const asiVisu_ActualSelection&      sel      = m_prs_mgr->GetCurrentSelection();
  const Handle(asiVisu_PickerResult)& pick_res = sel.GetCellPickerResult(SelectionNature_Persistent);

  // Special processing for Part Node
  Handle(asiData_PartNode) geom_n = m_model->GetPartNode();
  //
  if ( geom_n.IsNull() || !geom_n->IsWellFormed() )
  {
    if ( pick_res->IsSelectionFace() )
      emit facePicked( pick_res.get() );
    //
    else if ( pick_res->IsSelectionEdge() )
      emit edgePicked( pick_res.get() );
    //
    else if ( pick_res->IsSelectionVertex() )
      emit vertexPicked( pick_res.get() );

    return; // No target Node to proceed with
  }

  if ( !m_prs_mgr->IsPresented(geom_n) )
    return;

  //---------------------------------------------------------------------------
  // Retrieve current selection
  //---------------------------------------------------------------------------

  if ( pick_res->IsSelectionFace() )
  {
    // Prepare arrays for selected elements
    std::vector<int>                 picked_face_IDs;
    std::vector<ActAPI_DataObjectId> picked_node_IDs;

    // Prepare cumulative set of all picked element IDs
    GetPickedSubshapeIds(pick_res, picked_face_IDs, picked_node_IDs);

    //-------------------------------------------------------------------------
    // Store active selection in the Data Model
    //-------------------------------------------------------------------------

    if ( !m_model->HasOpenCommand() )
      m_model->OpenCommand(); // tx start
    //
    {
      // Store index of the active face
      if ( picked_face_IDs.size() )
      {
        geom_n->GetFaceRepresentation()     ->SetSelectedFaces(picked_face_IDs);
        geom_n->GetFaceRepresentation()     ->SetUScaleCoeff  (1.);
        geom_n->GetFaceRepresentation()     ->SetVScaleCoeff  (1.);
        geom_n->GetNormsRepresentation()    ->SetSelectedFaces(picked_face_IDs);
        geom_n->GetSurfaceRepresentation()  ->SetSelectedFaces(picked_face_IDs);
        geom_n->GetContourRepresentation()  ->SetSelectedFaces(picked_face_IDs);
        geom_n->GetHatchingRepresentation() ->SetSelectedFaces(picked_face_IDs);
      }
      else // Reset stored indices
      {
        geom_n->GetFaceRepresentation()     ->SetSelectedFace(0);
        geom_n->GetFaceRepresentation()     ->SetUScaleCoeff (1.);
        geom_n->GetFaceRepresentation()     ->SetVScaleCoeff (1.);
        geom_n->GetNormsRepresentation()    ->SetSelectedFace(0);
        geom_n->GetSurfaceRepresentation()  ->SetSelectedFace(0);
        geom_n->GetContourRepresentation()  ->SetSelectedFace(0);
        geom_n->GetHatchingRepresentation() ->SetSelectedFace(0);
      }
    }
    if ( m_model->HasOpenCommand() )
      m_model->CommitCommand(); // tx commit

    //-------------------------------------------------------------------------
    // Notify
    //-------------------------------------------------------------------------

    emit facePicked( pick_res.get() );
  }
  else if ( pick_res->IsSelectionEdge() )
  {
    // Prepare arrays for selected elements
    std::vector<int>                 picked_edge_IDs;
    std::vector<ActAPI_DataObjectId> picked_node_IDs;

    // Prepare cumulative set of all picked element IDs
    GetPickedSubshapeIds(pick_res, picked_edge_IDs, picked_node_IDs);

    //-------------------------------------------------------------------------
    // Store active selection in the Data Model
    //-------------------------------------------------------------------------

    m_model->OpenCommand(); // tx start
    {
      // Store index of the active edge
      if ( picked_edge_IDs.size() )
      {
        geom_n->GetEdgeRepresentation()  ->SetSelectedEdge(picked_edge_IDs[0]);
        geom_n->GetCurveRepresentation() ->SetSelectedEdge(picked_edge_IDs[0]);
      }
      else // Reset stored indices
      {
        geom_n->GetEdgeRepresentation()  ->SetSelectedEdge(0);
        geom_n->GetCurveRepresentation() ->SetSelectedEdge(0);
      }
    }
    m_model->CommitCommand(); // tx commit

    //-------------------------------------------------------------------------
    // Notify
    //-------------------------------------------------------------------------

    emit edgePicked( pick_res.get() );
  }
  else if ( pick_res->IsSelectionVertex() )
  {
    // Prepare arrays for selected elements
    std::vector<int>                 picked_vertex_IDs;
    std::vector<ActAPI_DataObjectId> picked_node_IDs;

    // Prepare cumulative set of all picked element IDs
    GetPickedSubshapeIds(pick_res, picked_vertex_IDs, picked_node_IDs);

    //-------------------------------------------------------------------------
    // Store active selection in the Data Model
    //-------------------------------------------------------------------------

    m_model->OpenCommand(); // tx start
    {
      // Store index of the active edge
      if ( picked_vertex_IDs.size() )
      {
        geom_n->GetVertexRepresentation()->SetSelectedVertex(picked_vertex_IDs[0]);
      }
      else // Reset stored indices
      {
        geom_n->GetVertexRepresentation()->SetSelectedVertex(0);
      }
    }
    m_model->CommitCommand(); // tx commit

    //-------------------------------------------------------------------------
    // Notify
    //-------------------------------------------------------------------------

    emit vertexPicked( pick_res.get() );
  }
}

//-----------------------------------------------------------------------------

//! Callback for highlighting event.
void asiUI_ViewerPart::onWhateverHighlighted()
{
  // Access picking results
  const asiVisu_ActualSelection& sel = m_prs_mgr->GetCurrentSelection();
  const Handle(asiVisu_PickerResult)& highlight_res = sel.GetCellPickerResult(SelectionNature_Detection);

  //if (highlight_res->IsSelectionFace())
  //  emit faceHighlighted(highlight_res.get());
  ////
  //else if (highlight_res->IsSelectionEdge())
  //  emit edgeHighlighted(highlight_res.get());
  ////
  //else if (highlight_res->IsSelectionVertex())
  //  emit vertexHighlighted(highlight_res.get());

  /*else*/
    emit whateverHighlighted(highlight_res.get());

  return;
}

//-----------------------------------------------------------------------------

//! Callback for face find request.
void asiUI_ViewerPart::onFindFace()
{
  // Run dialog (make it a static object to preserve the contents)
  static asiUI_DialogFindFace*
    wFindFace = new asiUI_DialogFindFace(m_model,
                                         this->PrsMgr(),
                                         this,
                                         m_progress,
                                         m_plotter);
  //
  wFindFace->show();
}

//-----------------------------------------------------------------------------

//! Callback for edge find request.
void asiUI_ViewerPart::onFindEdge()
{
  // Run dialog
  static asiUI_DialogFindEdge*
    wFindEdge = new asiUI_DialogFindEdge(m_model, this->PrsMgr(), this);
  //
  wFindEdge->show();
}

//-----------------------------------------------------------------------------

//! Callback for vertex find request.
void asiUI_ViewerPart::onFindVertex()
{
  // Run dialog
  static asiUI_DialogFindVertex*
    wFindVertex = new asiUI_DialogFindVertex(m_model, this->PrsMgr(), this);
  //
  wFindVertex->show();
}

//-----------------------------------------------------------------------------

//! Callback for tessellation refinement.
void asiUI_ViewerPart::onRefineTessellation()
{
  // Run dialog
  asiUI_DialogRefineTessellation*
    wRefineTessellation = new asiUI_DialogRefineTessellation(m_model, this->PrsMgr(), this);
  //
  wRefineTessellation->show();
}

//-----------------------------------------------------------------------------

//! Callback for constructing HLR representation.
void asiUI_ViewerPart::onBuildHLR()
{
  ::MakeHLR(m_model,
            this->PrsMgr(),
            asiAlgo_BuildHLR::Mode_Precise,
           "HLR",
            Color_White,
            m_progress,
            m_plotter);
}

//-----------------------------------------------------------------------------

//! Callback for constructing discrete HLR representation.
void asiUI_ViewerPart::onBuildHLRDiscr()
{
  ::MakeHLR(m_model,
            this->PrsMgr(),
            asiAlgo_BuildHLR::Mode_Discrete,
           "DHLR",
            ActAPI_Color(40./255., 190./255., 255./255., Quantity_TOC_RGB),
            m_progress,
            m_plotter);
}

//-----------------------------------------------------------------------------

//! Callback for constructing boxy HLR representation.
void asiUI_ViewerPart::onBuildHLRBox()
{
  ::MakeHLRBox(m_model,
               asiAlgo_BuildHLR::Mode_Precise,
              "HLR",
               Color_White,
               m_progress,
               m_plotter);
}

//-----------------------------------------------------------------------------

//! Callback for constructing discrete boxy HLR representation.
void asiUI_ViewerPart::onBuildHLRDiscrBox()
{
  ::MakeHLRBox(m_model,
               asiAlgo_BuildHLR::Mode_Discrete,
              "DHLR",
               ActAPI_Color(40./255., 190./255., 255./255., Quantity_TOC_RGB),
               m_progress,
               m_plotter);
}

//-----------------------------------------------------------------------------

void asiUI_ViewerPart::onContextMenu(const QPoint& pos)
{
  asiVisu_QVTKWidget* pViewer   = m_prs_mgr->GetQVTKWidget();
  QPoint              globalPos = pViewer->mapToGlobal(pos);

  emit contextMenu(globalPos);
}

//-----------------------------------------------------------------------------

void asiUI_ViewerPart::onDefaultView()
{
  asiVisu_Utils::ResetCamera( m_prs_mgr->GetRenderer() );
  asiVisu_Utils::AdjustCamera( m_prs_mgr->GetRenderer(), m_prs_mgr->PropsByTrihedron() );
  //
  this->Repaint();
}

//-----------------------------------------------------------------------------

void asiUI_ViewerPart::onBackView()
{
  asiVisu_Utils::CameraOnBack( m_prs_mgr->GetRenderer() );
  asiVisu_Utils::AdjustCamera( m_prs_mgr->GetRenderer(), m_prs_mgr->PropsByTrihedron() );
  //
  this->Repaint();
}

//-----------------------------------------------------------------------------

void asiUI_ViewerPart::onBottomView()
{
  asiVisu_Utils::CameraOnBottom( m_prs_mgr->GetRenderer() );
  asiVisu_Utils::AdjustCamera( m_prs_mgr->GetRenderer(), m_prs_mgr->PropsByTrihedron() );
  //
  this->Repaint();
}

//-----------------------------------------------------------------------------

void asiUI_ViewerPart::onFrontView()
{
  asiVisu_Utils::CameraOnFront( m_prs_mgr->GetRenderer() );
  asiVisu_Utils::AdjustCamera( m_prs_mgr->GetRenderer(), m_prs_mgr->PropsByTrihedron() );
  //
  this->Repaint();
}

//-----------------------------------------------------------------------------

void asiUI_ViewerPart::onLeftView()
{
  asiVisu_Utils::CameraOnLeft( m_prs_mgr->GetRenderer() );
  asiVisu_Utils::AdjustCamera( m_prs_mgr->GetRenderer(), m_prs_mgr->PropsByTrihedron() );
  //
  this->Repaint();
}

//-----------------------------------------------------------------------------

void asiUI_ViewerPart::onRightView()
{
  asiVisu_Utils::CameraOnRight( m_prs_mgr->GetRenderer() );
  asiVisu_Utils::AdjustCamera( m_prs_mgr->GetRenderer(), m_prs_mgr->PropsByTrihedron() );
  //
  this->Repaint();
}

//-----------------------------------------------------------------------------

void asiUI_ViewerPart::onTopView()
{
  asiVisu_Utils::CameraOnTop( m_prs_mgr->GetRenderer() );
  asiVisu_Utils::AdjustCamera( m_prs_mgr->GetRenderer(), m_prs_mgr->PropsByTrihedron() );
  //
  this->Repaint();
}

//-----------------------------------------------------------------------------

void asiUI_ViewerPart::onSelectAll()
{
  asiEngine_Part partApi(m_model, m_prs_mgr, m_progress, m_plotter);

  Handle(asiAlgo_AAG) aag = partApi.GetAAG();
  //
  if ( aag.IsNull() )
    return;

  asiAlgo_Feature fids;
  aag->GetAllFaces(fids);

  partApi.HighlightFaces(fids);
}

//-----------------------------------------------------------------------------

void asiUI_ViewerPart::onDefeature()
{
  asiEngine_Part partApi(m_model, m_prs_mgr, m_progress, m_plotter);

  TopoDS_Shape initShape = partApi.GetShape();
  //
  if ( initShape.IsNull() )
    return;

  // Get selected faces.
  asiAlgo_Feature features, unsuppressed;
  partApi.GetHighlightedFaces(features);

  // Suppress.
  TopoDS_Shape              resShape;
  Handle(BRepTools_History) resHistory;
  //
  asiAlgo_SuppressFeatures suppress(m_progress);
  //
  if ( !suppress(initShape, features, true, resShape, unsuppressed, resHistory) )
    return;

  // Prepare native history.
  Handle(asiAlgo_History)
    history = asiAlgo_History::Create(initShape, resHistory);

  // Update data model.
  m_model->OpenCommand();
  {
    partApi.Update(resShape, history);
  }
  m_model->CommitCommand();
}

//-----------------------------------------------------------------------------

void asiUI_ViewerPart::createActions()
{
  QAction* pFitAllAction     = new QAction("Fit",    this);
  QAction* pResetViewAction  = new QAction("Reset",  this);
  QAction* pBackViewAction   = new QAction("Back",   this);
  QAction* pBottomViewAction = new QAction("Bottom", this);
  QAction* pFrontViewAction  = new QAction("Front",  this);
  QAction* pLeftViewAction   = new QAction("Left",   this);
  QAction* pRightViewAction  = new QAction("Right",  this);
  QAction* pTopViewAction    = new QAction("Top",    this);
  //
  connect( pFitAllAction,     SIGNAL( triggered() ), this, SLOT( onFitAll      () ) );
  connect( pResetViewAction,  SIGNAL( triggered() ), this, SLOT( onDefaultView () ) );
  connect( pBackViewAction,   SIGNAL( triggered() ), this, SLOT( onBackView    () ) );
  connect( pBottomViewAction, SIGNAL( triggered() ), this, SLOT( onBottomView  () ) );
  connect( pFrontViewAction,  SIGNAL( triggered() ), this, SLOT( onFrontView   () ) );
  connect( pLeftViewAction,   SIGNAL( triggered() ), this, SLOT( onLeftView    () ) );
  connect( pRightViewAction,  SIGNAL( triggered() ), this, SLOT( onRightView   () ) );
  connect( pTopViewAction,    SIGNAL( triggered() ), this, SLOT( onTopView     () ) );

  // Add action to the toolbar
  m_toolBar->addAction(pFitAllAction);
  m_toolBar->addAction(pResetViewAction);
  m_toolBar->addAction(pBackViewAction);
  m_toolBar->addAction(pBottomViewAction);
  m_toolBar->addAction(pFrontViewAction);
  m_toolBar->addAction(pLeftViewAction);
  m_toolBar->addAction(pRightViewAction);
  m_toolBar->addAction(pTopViewAction);
}
