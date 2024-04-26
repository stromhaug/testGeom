//-----------------------------------------------------------------------------
// Created on: 24 August 2017
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

// cmdEngine includes
#include <cmdEngine.h>

// asiTcl includes
#include <asiTcl_PluginMacro.h>

// asiEngine includes
#include <asiEngine_Clearance.h>
#include <asiEngine_Curve.h>
#include <asiEngine_Editing.h>
#include <asiEngine_Isomorphism.h>
#include <asiEngine_Part.h>
#include <asiEngine_Thickness.h>
#include <asiEngine_TolerantShapes.h>

// asiAlgo includes
#include <asiAlgo_AAGIterator.h>
#include <asiAlgo_AttrBlendCandidate.h>
#include <asiAlgo_BlendType.h>
#include <asiAlgo_CheckDihedralAngle.h>
#include <asiAlgo_CheckValidity.h>
#include <asiAlgo_CheckVertexVexity.h>
#include <asiAlgo_CompleteEdgeLoop.h>
#include <asiAlgo_ComputeNegativeVolume.h>
#include <asiAlgo_ExtractFeatures.h>
#include <asiAlgo_FeatureAttrBaseFace.h>
#include <asiAlgo_FeatureType.h>
#include <asiAlgo_FindVisibleFaces.h>
#include <asiAlgo_MeshConvert.h>
#include <asiAlgo_PointInPoly.h>
#include <asiAlgo_RecognizeBlends.h>
#include <asiAlgo_RecognizeCanonical.h>
#include <asiAlgo_RecognizeCavities.h>
#include <asiAlgo_RecognizeConvexHull.h>
#include <asiAlgo_RecognizeDrillHoles.h>
#include <asiAlgo_RTCD.h>
#include <asiAlgo_SampleFace.h>
#include <asiAlgo_Timer.h>
#include <asiAlgo_Utils.h>

// asiVisu includes
#include <asiVisu_Utils.h>

// asiUI includes
#include <asiUI_DialogEuler.h>
#include <asiUI_IV.h>
#include <asiUI_PartGraph.h>
#include <asiUI_Plot2d.h>

#ifdef USE_MOBIUS
  #include <mobius/cascade_BSplineCurve.h>
  #include <mobius/cascade_BSplineSurface.h>

  using namespace mobius;
#endif

// OCCT includes
#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRepBndLib.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepTools.hxx>
#include <GeomLProp_CLProps.hxx>
#include <GeomLProp_SLProps.hxx>
#include <GProp_GProps.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

// VTK includes
#pragma warning(push, 0)
#include <vtkBMPWriter.h>
#pragma warning(pop)

//-----------------------------------------------------------------------------

namespace
{
  //! Axes vector field.
  struct t_axField
  {
    Handle(asiAlgo_BaseCloud<double>) origins;
    Handle(asiAlgo_BaseCloud<double>) vectors;

    //! Default ctor.
    t_axField() = default;

    //! Allocates heap memory for the field.
    void Alloc()
    {
      origins = new asiAlgo_BaseCloud<double>;
      vectors = new asiAlgo_BaseCloud<double>;
    }
  };
}

//-----------------------------------------------------------------------------

//! \brief Function to filter the extracted blend candidates by type.
class cmdEngine_ExtractBlendsFilter : public asiAlgo_ExtractFeaturesFilter
{
public:

  // OCCT RTTI.
  DEFINE_STANDARD_RTTI_INLINE(cmdEngine_ExtractBlendsFilter, asiAlgo_ExtractFeaturesFilter)

public:

  //! Ctor accepting the target blend type.
  //! \param[in] blendType type of interest.
  cmdEngine_ExtractBlendsFilter(const asiAlgo_BlendType blendType)
  : asiAlgo_ExtractFeaturesFilter(), m_blendType(blendType)
  {}

public:

  //! If the returned flag is true, the attribute and its corresponding
  //! face is accepted.
  //! \param[in] attr AAG node attribute to check.
  //! \return true to accepts, false -- to deny.
  virtual bool operator()(const Handle(asiAlgo_FeatureAttrFace)& attr) const
  {
    Handle(asiAlgo_AttrBlendCandidate)
      bcAttr = Handle(asiAlgo_AttrBlendCandidate)::DownCast(attr);
    //
    if ( bcAttr.IsNull() )
      return false;

    // Filter by type.
    if ( bcAttr->Kind != m_blendType )
      return false;

    return true;
  }

private:

  asiAlgo_BlendType m_blendType; //!< Blend type of interest.

};

//-----------------------------------------------------------------------------

int ENGINE_Explode(const Handle(asiTcl_Interp)& interp,
                   int                          argc,
                   const char**                 argv)
{
  if ( argc != 1 && argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Process input flags.
  const bool isNoLocation = interp->HasKeyword(argc, argv, "noloc");
  const bool isVertex     = interp->HasKeyword(argc, argv, "vertex");
  const bool isEdge       = interp->HasKeyword(argc, argv, "edge");
  const bool isWire       = interp->HasKeyword(argc, argv, "wire");
  const bool isFace       = interp->HasKeyword(argc, argv, "face");
  const bool isShell      = interp->HasKeyword(argc, argv, "shell");
  const bool isSolid      = interp->HasKeyword(argc, argv, "solid");

  if ( isNoLocation && (isVertex ||
                        isEdge   ||
                        isFace   ||
                        isShell  ||
                        isSolid) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot use '-noloc' flag with subshape qualifiers.");
    return TCL_ERROR;
  }

  // Disable visualization for efficiency.
  Handle(asiUI_IV)
    IV = Handle(asiUI_IV)::DownCast( interp->GetPlotter().Access() );
  //
  if ( !IV.IsNull() )
  {
    IV->BROWSER_OFF();
    IV->VISUALIZATION_OFF();
  }

  // Get Part Node and shape.
  Handle(asiData_PartNode) partNode = cmdEngine::model->GetPartNode();
  //
  TopoDS_Shape partShape = partNode->GetShape();

  if ( isNoLocation || (argc == 1) )
  {
    // Explode.
    for ( TopoDS_Iterator it(partShape, true, !isNoLocation); it.More(); it.Next() )
    {
      const TopoDS_Shape& subShape = it.Value();

      // Generate name.
      TCollection_AsciiString name = asiAlgo_Utils::ShapeTypeStr(subShape).c_str();

      // Draw imperatively (populates Data Model).
      interp->GetPlotter().DRAW_SHAPE(subShape, Color_White, name);

      // Set tessellation properties.
      if ( !IV.IsNull() )
      {
        Handle(asiData_IVTopoItemNode)
          topoNode = Handle(asiData_IVTopoItemNode)::DownCast( IV->GetLastNode() );
        //
        if ( !topoNode.IsNull() )
        {
          cmdEngine::model->OpenCommand();
          {
            topoNode->SetLinearDeflection  ( partNode->GetLinearDeflection() );
            topoNode->SetAngularDeflection ( partNode->GetAngularDeflection() );
          }
          cmdEngine::model->CommitCommand();
        }
      }
    }
  }
  else
  {
    // Get qualifier of sub-shape.
    TopAbs_ShapeEnum subshapeType;

    if ( isVertex )
      subshapeType = TopAbs_VERTEX;
    //
    else if ( isEdge )
      subshapeType = TopAbs_EDGE;
    //
    else if ( isWire )
      subshapeType = TopAbs_WIRE;
    //
    else if ( isFace )
      subshapeType = TopAbs_FACE;
    //
    else if ( isShell )
      subshapeType = TopAbs_SHELL;
    //
    else if ( isSolid )
      subshapeType = TopAbs_SOLID;
    //
    else
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Unexpected sub-shape qualifier.");
      return TCL_ERROR;
    }

    // Explode.
    for ( TopExp_Explorer exp(partShape, subshapeType); exp.More(); exp.Next() )
    {
      const TopoDS_Shape& subShape = exp.Current();

      // Generate name.
      TCollection_AsciiString name = asiAlgo_Utils::ShapeTypeStr(subShape).c_str();

      // Draw imperatively (populates Data Model).
      interp->GetPlotter().DRAW_SHAPE(subShape, Color_White, name);

      // Set tessellation properties.
      if ( !IV.IsNull() )
      {
        Handle(asiData_IVTopoItemNode)
          topoNode = Handle(asiData_IVTopoItemNode)::DownCast( IV->GetLastNode() );
        //
        if ( !topoNode.IsNull() )
        {
          cmdEngine::model->OpenCommand();
          {
            topoNode->SetLinearDeflection  ( partNode->GetLinearDeflection() );
            topoNode->SetAngularDeflection ( partNode->GetAngularDeflection() );
          }
          cmdEngine::model->CommitCommand();
        }
      }
    }
  }

  // Enable back UI updates.
  if ( !IV.IsNull() )
  {
    IV->BROWSER_ON();
    IV->VISUALIZATION_ON();
  }

  // Update browser.
  if ( !cmdEngine::cf.IsNull() && cmdEngine::cf->ObjectBrowser )
    cmdEngine::cf->ObjectBrowser->Populate();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_PrintSummary(const Handle(asiTcl_Interp)& interp,
                        int                          argc,
                        const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node and shape.
  Handle(asiData_PartNode) partNode = cmdEngine::model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part is not initialized.");
    return TCL_OK;
  }
  //
  TopoDS_Shape partShape = partNode->GetShape();

  // Build explicit topology graph
  asiAlgo_TopoGraph topograph(partShape);

  int numCompounds  = 0,
      numCompSolids = 0,
      numSolids     = 0,
      numShells     = 0,
      numFaces      = 0,
      numWires      = 0,
      numEdges      = 0,
      numVertices   = 0;
  //
  topograph.CalculateSummary(numCompounds,
                             numCompSolids,
                             numSolids,
                             numShells,
                             numFaces,
                             numWires,
                             numEdges,
                             numVertices);

  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "=============================================");
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Summary of part's contents");
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "---------------------------------------------");
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "\tNum. compounds:  %1" << numCompounds);
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "\tNum. compsolids: %1" << numCompSolids);
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "\tNum. solids:     %1" << numSolids);
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "\tNum. shells:     %1" << numShells);
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "\tNum. faces:      %1" << numFaces);
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "\tNum. wires:      %1" << numWires);
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "\tNum. edges:      %1" << numEdges);
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "\tNum. vertices:   %1" << numVertices);

  // Check naming.
  Handle(asiAlgo_Naming) naming = partNode->GetNaming();
  //
  if ( !naming.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogInfo(Normal) << "\t... Naming service is active.");
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_GetSummary(const Handle(asiTcl_Interp)& interp,
                      int                          argc,
                      const char**                 argv)
{
  if ( argc != 9 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node and shape.
  Handle(asiData_PartNode) partNode = cmdEngine::model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part is not initialized.");
    return TCL_OK;
  }
  //
  TopoDS_Shape partShape = partNode->GetShape();

  // Build explicit topology graph
  asiAlgo_TopoGraph topograph(partShape);

  int numCompounds  = 0,
      numCompSolids = 0,
      numSolids     = 0,
      numShells     = 0,
      numFaces      = 0,
      numWires      = 0,
      numEdges      = 0,
      numVertices   = 0;
  //
  topograph.CalculateSummary(numCompounds,
                             numCompSolids,
                             numSolids,
                             numShells,
                             numFaces,
                             numWires,
                             numEdges,
                             numVertices);

  // Set Tcl variables.
  int varIdx = 0;
  //
  interp->SetVarFundamental<int>(argv[++varIdx], numCompounds);
  interp->SetVarFundamental<int>(argv[++varIdx], numCompSolids);
  interp->SetVarFundamental<int>(argv[++varIdx], numSolids);
  interp->SetVarFundamental<int>(argv[++varIdx], numShells);
  interp->SetVarFundamental<int>(argv[++varIdx], numFaces);
  interp->SetVarFundamental<int>(argv[++varIdx], numWires);
  interp->SetVarFundamental<int>(argv[++varIdx], numEdges);
  interp->SetVarFundamental<int>(argv[++varIdx], numVertices);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_PrintSummaryGeom(const Handle(asiTcl_Interp)& interp,
                            int                          argc,
                            const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node and shape.
  Handle(asiData_PartNode) partNode = cmdEngine::model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part is not initialized.");
    return TCL_OK;
  }
  //
  TopoDS_Shape partShape = partNode->GetShape();

  asiAlgo_GeomSummary summary;
  //
  asiAlgo_Utils::GeomSummary(partShape,
                             summary);

  summary.Print( "Summary of part's geometric contents", interp->GetProgress() );

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_GetSummaryGeom(const Handle(asiTcl_Interp)& interp,
                          int                          argc,
                          const char**                 argv)
{
  if ( argc != 19 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node and shape.
  Handle(asiData_PartNode) partNode = cmdEngine::model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part is not initialized.");
    return TCL_OK;
  }
  //
  TopoDS_Shape partShape = partNode->GetShape();

  asiAlgo_GeomSummary summary;
  //
  asiAlgo_Utils::GeomSummary(partShape,
                             summary);

  // Set Tcl variables.
  int varIdx = 0;
  //
  interp->SetVarFundamental<int>(argv[++varIdx], summary.nbSurfBezier);
  interp->SetVarFundamental<int>(argv[++varIdx], summary.nbSurfSpl);
  interp->SetVarFundamental<int>(argv[++varIdx], summary.nbSurfConical);
  interp->SetVarFundamental<int>(argv[++varIdx], summary.nbSurfCyl);
  interp->SetVarFundamental<int>(argv[++varIdx], summary.nbSurfOffset);
  interp->SetVarFundamental<int>(argv[++varIdx], summary.nbSurfSph);
  interp->SetVarFundamental<int>(argv[++varIdx], summary.nbSurfLinExtr);
  interp->SetVarFundamental<int>(argv[++varIdx], summary.nbSurfOfRevol);
  interp->SetVarFundamental<int>(argv[++varIdx], summary.nbSurfToroidal);
  interp->SetVarFundamental<int>(argv[++varIdx], summary.nbSurfPlane);
  interp->SetVarFundamental<int>(argv[++varIdx], summary.nbCurveBezier);
  interp->SetVarFundamental<int>(argv[++varIdx], summary.nbCurveSpline);
  interp->SetVarFundamental<int>(argv[++varIdx], summary.nbCurveCircle);
  interp->SetVarFundamental<int>(argv[++varIdx], summary.nbCurveEllipse);
  interp->SetVarFundamental<int>(argv[++varIdx], summary.nbCurveHyperbola);
  interp->SetVarFundamental<int>(argv[++varIdx], summary.nbCurveLine);
  interp->SetVarFundamental<int>(argv[++varIdx], summary.nbCurveOffset);
  interp->SetVarFundamental<int>(argv[++varIdx], summary.nbCurveParabola);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_FaceAddr(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  const int fidx = atoi(argv[1]);
  //
  if ( fidx < 1 )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Face index should be 1-based.");
    return TCL_OK;
  }

  // Get Part Node.
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();

  // Get face.
  const TopoDS_Face& face = part_n->GetAAG()->GetFace(fidx);

  // Output.
  interp->GetProgress().SendLogMessage( LogInfo(Normal) << "Face %1: %2"
                                                        << fidx
                                                        << asiAlgo_Utils::ShapeAddr(face).c_str() );
  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckDist(const Handle(asiTcl_Interp)& interp,
                     int                          argc,
                     const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node.
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();

  // Get topological item to imprint.
  Handle(asiData_IVTopoItemNode)
    topoItem_n = Handle(asiData_IVTopoItemNode)::DownCast( cmdEngine::model->FindNodeByName(argv[1]) );
  //
  if ( topoItem_n.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find topological object with name %1." << argv[1]);
    return TCL_OK;
  }

  TIMER_NEW
  TIMER_GO

  BRepExtrema_DistShapeShape distSS(part_n->GetShape(),
                                    topoItem_n->GetShape(),
                                    Extrema_ExtFlag_MIN);

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Shape-shape distance")

  if ( !distSS.IsDone() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Extrema not done.");
    return TCL_OK;
  }

  // Get number of solutions.
  const int numSol = distSS.NbSolution();
  //
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Number of extrema solutions: %1." << numSol);

  for ( int k = 1; k <= numSol; ++k )
  {
    const gp_Pnt& P1 = distSS.PointOnShape1(k);
    const gp_Pnt& P2 = distSS.PointOnShape2(k);

    interp->GetPlotter().DRAW_POINT(P1, Color_Red, "dist_P1");
    interp->GetPlotter().DRAW_POINT(P2, Color_Red, "dist_P2");
    interp->GetPlotter().DRAW_LINK(P1, P2, Color_Red, "dist_P1P2");
  }

  interp->GetProgress().SendLogMessage( LogInfo(Normal) << "Distance: %1."
                                                        << distSS.Value() );

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckCurvature(const Handle(asiTcl_Interp)& interp,
                          int                          argc,
                          const char**                 argv)
{
  if ( argc > 6 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  /* ================
   *  Prepare inputs
   * ================ */

  // Find control keys.
  bool noPlot           = false;
  bool noAlongCurvature = false;
  //
  for ( int i = 1; i < argc; ++i )
  {
    if ( interp->IsKeyword(argv[i], "noplot") )
      noPlot = true;

    if ( interp->IsKeyword(argv[i], "noalong") )
      noAlongCurvature = true;
  }

  // Get numerical values.
  int    numPts      = 100;
  double scaleFactor = 1.0;
  double amplFactor  = 1.0;
  //
  if ( argc >= 2 )
    numPts = atoi(argv[1]);
  if ( argc >= 3 )
    scaleFactor = atof(argv[2]);
  if ( argc >= 4 )
    amplFactor = atof(argv[3]);

  // Get Part Node to access the selected edge.
  Handle(asiData_PartNode) partNode = cmdEngine::model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part Node is null or ill-defined.");
    return TCL_OK;
  }
  //
  TopoDS_Shape                      partShape = partNode->GetShape();
  const TopTools_IndexedMapOfShape& subShapes = partNode->GetAAG()->RequestMapOfSubShapes();

  // Curve Node is expected.
  Handle(asiData_CurveNode) curveNode = partNode->GetCurveRepresentation();
  //
  if ( curveNode.IsNull() || !curveNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Curve Node is null or ill-defined.");
    return TCL_OK;
  }

  // Get ID of the selected edge.
  const int edgeIdx = curveNode->GetSelectedEdge();
  //
  if ( edgeIdx <= 0 )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Please, select edge first.");
    return TCL_OK;
  }

  // Get host curve of the selected edge.
  const TopoDS_Shape& edgeShape = subShapes(edgeIdx);
  //
  if ( edgeShape.ShapeType() != TopAbs_EDGE )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Unexpected topological type of the selected edge.");
    return TCL_OK;
  }

  /* ==========================
   *  Evaluate curvature combs
   * ========================== */

  // Create curvature combs.
  asiEngine_Curve CurveAPI( cmdEngine::model, interp->GetProgress(), interp->GetPlotter() );
  //
  Handle(asiData_CurvatureCombsNode) combsNode;
  //
  cmdEngine::model->OpenCommand();
  {
    // Create persistent object.
    combsNode = CurveAPI.CreateOrUpdateCurvatureCombs(partNode,
                                                      curveNode,
                                                      scaleFactor,
                                                      amplFactor,
                                                      numPts);

    cmdEngine::model->FuncExecuteAll();
  }
  cmdEngine::model->CommitCommand();

  // Actualize.
  if ( cmdEngine::cf )
  {
    cmdEngine::cf->ObjectBrowser->Populate();
    cmdEngine::cf->ViewerPart->PrsMgr()->Actualize(combsNode);
  }

  /* ======================
   *  Build curvature plot
   * ====================== */

  if ( cmdEngine::cf && !noPlot )
  {
    // Open curvature plot.
    asiUI_Plot2d*
      cPlot = new asiUI_Plot2d( interp->GetProgress(),
                                interp->GetPlotter() );
    //
    std::vector<double> params;
    std::vector<double> curvatures;
    combsNode->GetParameters(params);
    combsNode->GetCurvatures(curvatures);
    cPlot->Render(params, curvatures, "Parameter", "Curvature", "Curvature Plot");
  }

  /* ========================================
   *  Check along-curvature (if faces exist)
   * ======================================== */

  if ( !noAlongCurvature )
  {
    // Get owner faces.
    TopTools_IndexedDataMapOfShapeListOfShape M;
    TopExp::MapShapesAndAncestors(partShape, TopAbs_EDGE, TopAbs_FACE, M);
    //
    const TopTools_ListOfShape& edgeFaces = M.FindFromKey(edgeShape);
    //
    for ( TopTools_ListIteratorOfListOfShape fit(edgeFaces); fit.More(); fit.Next() )
    {
      const TopoDS_Face& faceShape = TopoDS::Face( fit.Value() );

      // Evaluate along curvature.
      double k;
      if ( !asiAlgo_Utils::EvaluateAlongCurvature(faceShape, TopoDS::Edge(edgeShape), k) )
      {
        interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot evaluate along-curvature.");
        continue;
      }
      else
      {
        interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Along curvature: %1." << k);

        if ( Abs(k) < 1.e-5 )
          interp->GetProgress().SendLogMessage( LogInfo(Normal) << "Curvature radius is infinite." );
        else
          interp->GetProgress().SendLogMessage( LogInfo(Normal) << "Curvature radius: %1." << Abs(1.0 / k) );
      }
    }
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckContinuity(const Handle(asiTcl_Interp)& interp,
                           int                          argc,
                           const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  /* ================
   *  Prepare inputs
   * ================ */

  // Get Part Node to access the selected face.
  Handle(asiData_PartNode) partNode = cmdEngine::model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part Node is null or ill-defined.");
    return TCL_OK;
  }
  //
  TopoDS_Shape                      partShape = partNode->GetShape();
  const TopTools_IndexedMapOfShape& subShapes = partNode->GetAAG()->RequestMapOfSubShapes();

  // Surf Node is expected.
  Handle(asiData_SurfNode) surfNode = partNode->GetSurfaceRepresentation();
  //
  if ( surfNode.IsNull() || !surfNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Surface Node is null or ill-defined.");
    return TCL_OK;
  }

  // Get ID of the selected face.
  const int faceIdx = surfNode->GetAnySelectedFace();
  //
  if ( faceIdx <= 0 )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Please, select face first.");
    return TCL_OK;
  }

  // Get host surface of the selected face.
  const TopoDS_Shape& faceShape = subShapes(faceIdx);
  //
  if ( faceShape.ShapeType() != TopAbs_FACE )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Unexpected topological type of the selected face.");
    return TCL_OK;
  }
  //
  Handle(Geom_Surface)
    surface = BRep_Tool::Surface( TopoDS::Face(faceShape) );

  // Only B-surfaces are allowed.
  Handle(Geom_BSplineSurface)
    t_bsurf = Handle(Geom_BSplineSurface)::DownCast(surface);
  //
  if ( t_bsurf.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Only B-surfaces are allowed in this function.");
    return TCL_OK;
  }

  /* =====================
   *  Evaluate continuity
   * ===================== */

  const TColStd_Array1OfReal&    uKnots = t_bsurf->UKnots();
  const TColStd_Array1OfReal&    vKnots = t_bsurf->VKnots();
  const TColStd_Array1OfInteger& uMults = t_bsurf->UMultiplicities();
  const TColStd_Array1OfInteger& vMults = t_bsurf->VMultiplicities();

  // Draw U defects.
  for ( int i = 1; i <= uKnots.Length(); ++i )
  {
    const double u     = uKnots(i);
    const int    uMult = uMults(i);

    if ( uMult > 1 )
    {
      Handle(Geom_Curve) iso = t_bsurf->UIso(u);

      TCollection_AsciiString isoName("mult=");
      isoName += uMult;
      isoName += " u=";
      isoName += u;

      interp->GetPlotter().REDRAW_CURVE(isoName, iso, Color_Red, true);
    }
  }

  // Draw V defects.
  for ( int i = 1; i <= vKnots.Length(); ++i )
  {
    const double v     = vKnots(i);
    const int    vMult = vMults(i);

    if ( vMult > 1 )
    {
      Handle(Geom_Curve) iso = t_bsurf->VIso(v);

      TCollection_AsciiString isoName("mult=");
      isoName += vMult;
      isoName += " v=";
      isoName += v;

      interp->GetPlotter().REDRAW_CURVE(isoName, iso, Color_Red, true);
    }
  }

  // TODO

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckEuler(const Handle(asiTcl_Interp)& interp,
                      int                          argc,
                      const char**                 argv)
{
  if ( argc != 1 && argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  if ( cmdEngine::cf && (argc == 1) )
  {
    asiUI_DialogEuler* pEuler = new asiUI_DialogEuler( cmdEngine::model,
                                                       interp->GetProgress() );
    //
    pEuler->show();
  }
  else
  {
    const int genus = atoi(argv[1]);

    // Calculate the Euler-Poincare property for the active part.
    const bool
      isOk = asiEngine_Editing( cmdEngine::model,
                                interp->GetProgress(),
                                interp->GetPlotter() ).CheckEulerPoincare(genus);
    //
    if ( !isOk )
      interp->GetProgress().SendLogMessage(LogWarn(Normal) << "Euler-Poincare property does not hold.");

    // Append result to the interpreter.
    *interp << isOk;
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_DumpTopographDot(const Handle(asiTcl_Interp)& interp,
                            int                          argc,
                            const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node.
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();

  // Access or create topograph.
  Handle(asiAlgo_TopoGraph) topograph;
  //
  if ( part_n->HasNaming() )
    topograph = part_n->GetNaming()->GetTopoGraph(); // With names.
  else
    topograph = new asiAlgo_TopoGraph( part_n->GetShape() ); // Without names.

  // Dump to buffer.
  std::stringstream buff;
  topograph->Dump(buff);

  // Dump to file.
  std::ofstream FILE(argv[1]);
  if ( !FILE.is_open() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot open file for writing.");
    return TCL_OK;
  }
  //
  FILE << buff.str().c_str();
  //
  FILE.close();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_EvalCurve(const Handle(asiTcl_Interp)& interp,
                     int                          argc,
                     const char**                 argv)
{
  if ( argc < 4 || argc > 5 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Find Curve Node by name.
  Handle(ActAPI_INode) node = cmdEngine::model->FindNodeByName(argv[1]);
  //
  Handle(asiData_IVCurveNode)
    curveNode = Handle(asiData_IVCurveNode)::DownCast(node);
  //
  if ( curveNode.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Node '%1' is not a curve."
                                                        << argv[1]);
    return TCL_OK;
  }

  // Get curve.
  double f, l;
  Handle(Geom_Curve) occtCurve = curveNode->GetCurve(f, l);
  //
  if ( occtCurve.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "The curve in question is null.");
    return TCL_OK;
  }

  // Get parameter value.
  const double u = atof(argv[2]);

  // Get order.
  const int order = atoi(argv[3]);
  //
  if ( order < 0 )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Negative order is meaningless.");
    return TCL_OK;
  }

  // Check whether Mobius evaluation is requested.
  bool isMobius = interp->HasKeyword(argc, argv, "mobius");

  // Evaluate curve.
  TCollection_AsciiString Message("\n========================================");
  Message                      += "\n Curve evaluation";
  Message                      += "\n----------------------------------------";
  //
  if ( !isMobius )
  {
    Message += "\nMethod: OpenCascade";

    if ( order == 0 )
    {
      gp_Pnt eval_P;
      occtCurve->D0(u, eval_P);
      //
      interp->GetPlotter().REDRAW_POINT("eval_P", eval_P, Color_Yellow);
      //
      Message += "\nP(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.Z() ).c_str();
      Message += ")";
    }
    else if ( order == 1 )
    {
      gp_Pnt eval_P;
      gp_Vec eval_D1;
      occtCurve->D1(u, eval_P, eval_D1);
      //
      interp->GetPlotter().REDRAW_POINT("eval_P", eval_P, Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D1", eval_P, eval_D1, Color_Yellow);
      //
      Message += "\nP(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.Z() ).c_str();
      Message += ")";
      //
      Message += "\nD1(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1.Z() ).c_str();
      Message += ")";
    }
    else if ( order == 2 )
    {
      gp_Pnt eval_P;
      gp_Vec eval_D1, eval_D2;
      occtCurve->D2(u, eval_P, eval_D1, eval_D2);
      //
      interp->GetPlotter().REDRAW_POINT("eval_P", eval_P, Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D1", eval_P, eval_D1, Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D2", eval_P, eval_D2, Color_Yellow);
      //
      Message += "\nP(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.Z() ).c_str();
      Message += ")";
      //
      Message += "\nD1(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1.Z() ).c_str();
      Message += ")";
      //
      Message += "\nD2(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2.Z() ).c_str();
      Message += ")";
    }
    else if ( order == 3 )
    {
      gp_Pnt eval_P;
      gp_Vec eval_D1, eval_D2, eval_D3;
      occtCurve->D3(u, eval_P, eval_D1, eval_D2, eval_D3);
      //
      interp->GetPlotter().REDRAW_POINT("eval_P", eval_P, Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D1", eval_P, eval_D1, Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D2", eval_P, eval_D2, Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D3", eval_P, eval_D3, Color_Yellow);
      //
      Message += "\nP(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.Z() ).c_str();
      Message += ")";
      //
      Message += "\nD1(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1.Z() ).c_str();
      Message += ")";
      //
      Message += "\nD2(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2.Z() ).c_str();
      Message += ")";
      //
      Message += "\nD3(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D3.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D3.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D3.Z() ).c_str();
      Message += ")";
    }
    else
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Order %1 is not supported for OpenCascade method."
                                                          << order);
      return TCL_OK;
    }

    // Add the centre of curvature.
    if ( order >= 2 )
    {
      GeomLProp_CLProps clProp( occtCurve, 2, Precision::Confusion() );
      clProp.SetParameter(u);
      //
      if ( !clProp.IsTangentDefined() )
      {
        interp->GetProgress().SendLogMessage(LogErr(Normal) << "Tangent is not defined."
                                                            << order);
        return TCL_OK;
      }

      // Curvature.
      const double K = clProp.Curvature();
      if ( Abs(K) > Precision::Confusion() )
      {
        const double r_onCurve = 1/Abs(K); // Radius of curvature.

        // Calculate the center of curvature.
        gp_Pnt center_onCurve;
        clProp.CentreOfCurvature(center_onCurve);

        interp->GetPlotter().REDRAW_POINT("centre_K", center_onCurve, Color_White);

        // Build the osculating circle manually.
        gp_Dir tang_onCurve, norm_onCurve;
        clProp.Tangent(tang_onCurve);
        clProp.Normal(norm_onCurve);
        gp_Dir axDir_ofCircle = norm_onCurve ^ tang_onCurve; // Cross product.
        gp_Ax2 ax_ofCircle(center_onCurve, axDir_ofCircle, norm_onCurve);

        // Construct the osculating circle.
        interp->GetPlotter().REDRAW_CURVE( "osculatingCircle",
                                           new Geom_Circle(ax_ofCircle, r_onCurve),
                                           Color_White, true );
      }
    }
  }
  else
  {
#ifndef USE_MOBIUS
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Mobius module is disabled.");
    return TCL_ERROR;
#else
    Message += "\nMethod: Mobius";

    Handle(Geom_BSplineCurve)
      occtBCurve = Handle(Geom_BSplineCurve)::DownCast(occtCurve);
    //
    if ( occtBCurve.IsNull() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "The curve in question is not a B-curve.");
      return TCL_OK;
    }

    // Convert to Mobius curve.
    cascade_BSplineCurve converter(occtBCurve);
    converter.DirectConvert();
    //
    const t_ptr<t_bcurve>&
      mobCurve = converter.GetMobiusCurve();

    // Evaluate.
    if ( order == 0 )
    {
      t_xyz eval_P;
      mobCurve->Eval(u, eval_P);
      //
      interp->GetPlotter().REDRAW_POINT("eval_P",
                                        gp_Pnt( eval_P.X(), eval_P.Y(), eval_P.Z() ),
                                        Color_Yellow);
      //
      Message += "\nP(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.Z() ).c_str();
      Message += ")";
    }
    else if ( order == 1 )
    {
      t_xyz eval_P;
      mobCurve->Eval(u, eval_P);
      //
      t_xyz eval_D1;
      mobCurve->Eval_Dk(u, 1, eval_D1);
      //
      interp->GetPlotter().REDRAW_POINT("eval_P",
                                        gp_Pnt( eval_P.X(), eval_P.Y(), eval_P.Z() ),
                                        Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D1",
                                            gp_Pnt( eval_P.X(), eval_P.Y(), eval_P.Z() ),
                                            gp_Vec( eval_D1.X(), eval_D1.Y(), eval_D1.Z() ),
                                            Color_Yellow);
      //
      Message += "\nP(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.Z() ).c_str();
      Message += ")";
      //
      Message += "\nD1(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1.Z() ).c_str();
      Message += ")";
    }
    else if ( order == 2 )
    {
      t_xyz eval_P;
      mobCurve->Eval(u, eval_P);
      //
      t_xyz eval_D1;
      mobCurve->Eval_Dk(u, 1, eval_D1);
      //
      t_xyz eval_D2;
      mobCurve->Eval_Dk(u, 2, eval_D2);
      //
      interp->GetPlotter().REDRAW_POINT("eval_P",
                                        gp_Pnt( eval_P.X(), eval_P.Y(), eval_P.Z() ),
                                        Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D1",
                                            gp_Pnt( eval_P.X(), eval_P.Y(), eval_P.Z() ),
                                            gp_Vec( eval_D1.X(), eval_D1.Y(), eval_D1.Z() ),
                                            Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D2",
                                            gp_Pnt( eval_P.X(), eval_P.Y(), eval_P.Z() ),
                                            gp_Vec( eval_D2.X(), eval_D2.Y(), eval_D2.Z() ),
                                            Color_Yellow);
      //
      Message += "\nP(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.Z() ).c_str();
      Message += ")";
      //
      Message += "\nD1(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1.Z() ).c_str();
      Message += ")";
      //
      Message += "\nD2(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2.Z() ).c_str();
      Message += ")";
    }
    else if ( order == 3 )
    {
      t_xyz eval_P;
      mobCurve->Eval(u, eval_P);
      //
      t_xyz eval_D1;
      mobCurve->Eval_Dk(u, 1, eval_D1);
      //
      t_xyz eval_D2;
      mobCurve->Eval_Dk(u, 2, eval_D2);
      //
      t_xyz eval_D3;
      mobCurve->Eval_Dk(u, 3, eval_D3);
      //
      interp->GetPlotter().REDRAW_POINT("eval_P",
                                        gp_Pnt( eval_P.X(), eval_P.Y(), eval_P.Z() ),
                                        Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D1",
                                            gp_Pnt( eval_P.X(), eval_P.Y(), eval_P.Z() ),
                                            gp_Vec( eval_D1.X(), eval_D1.Y(), eval_D1.Z() ),
                                            Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D2",
                                            gp_Pnt( eval_P.X(), eval_P.Y(), eval_P.Z() ),
                                            gp_Vec( eval_D2.X(), eval_D2.Y(), eval_D2.Z() ),
                                            Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D3",
                                            gp_Pnt( eval_P.X(), eval_P.Y(), eval_P.Z() ),
                                            gp_Vec( eval_D3.X(), eval_D3.Y(), eval_D3.Z() ),
                                            Color_Yellow);
      //
      Message += "\nP(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.Z() ).c_str();
      Message += ")";
      //
      Message += "\nD1(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1.Z() ).c_str();
      Message += ")";
      //
      Message += "\nD2(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2.Z() ).c_str();
      Message += ")";
      //
      Message += "\nD3(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D3.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D3.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D3.Z() ).c_str();
      Message += ")";
    }
    else
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Order %1 is not supported for Mobius method."
                                                          << order);
      return TCL_OK;
    }
#endif
  }
  Message += "\n----------------------------------------";
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << Message);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_EvalSurf(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc < 5 || argc > 6 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Find Surface Node by name.
  Handle(ActAPI_INode) node = cmdEngine::model->FindNodeByName(argv[1]);
  //
  if ( node.IsNull() || !node->IsKind( STANDARD_TYPE(asiData_IVSurfaceNode) ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Node '%1' is not a surface."
                                                        << argv[1]);
    return TCL_OK;
  }
  //
  Handle(asiData_IVSurfaceNode)
    surfNode = Handle(asiData_IVSurfaceNode)::DownCast(node);

  // Get surface.
  Handle(Geom_Surface) occtSurface = surfNode->GetSurface();
  //
  if ( occtSurface.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "The surface in question is null.");
    return TCL_OK;
  }

  // Get parameter values.
  const double u = atof(argv[2]);
  const double v = atof(argv[3]);

  // Get order.
  const int order = atoi(argv[4]);
  //
  if ( order < 0 )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Negative order is meaningless.");
    return TCL_OK;
  }

  // Check whether Mobius evaluation is requested.
  bool isMobius = interp->HasKeyword(argc, argv, "mobius");

  // Evaluate surface.
  TCollection_AsciiString Message("\n========================================");
  Message                      += "\n Surface evaluation";
  Message                      += "\n----------------------------------------";
  //
  if ( !isMobius )
  {
    Message += "\nMethod: OpenCascade";

    if ( order == 0 )
    {
      gp_Pnt eval_P;
      occtSurface->D0(u, v, eval_P);
      //
      interp->GetPlotter().REDRAW_POINT("eval_P", eval_P, Color_Yellow);
      //
      Message += "\nP(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.Z() ).c_str();
      Message += ")";
    }
    else if ( order == 1 )
    {
      gp_Pnt eval_P;
      gp_Vec eval_D1u, eval_D1v;
      occtSurface->D1(u, v, eval_P, eval_D1u, eval_D1v);
      //
      interp->GetPlotter().REDRAW_POINT("eval_P", eval_P, Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D1u", eval_P, eval_D1u, Color_Red);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D1v", eval_P, eval_D1v, Color_Green);
      //
      Message += "\nP(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.Z() ).c_str();
      Message += ")";
      //
      Message += "\nD1u(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1u.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1u.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1u.Z() ).c_str();
      Message += ")";
      //
      Message += "\nD1v(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1v.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1v.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1v.Z() ).c_str();
      Message += ")";
    }
    else if ( order == 2 )
    {
      gp_Pnt eval_P;
      gp_Vec eval_D1u, eval_D1v, eval_D2u, eval_D2v, eval_D2uv;
      occtSurface->D2(u, v, eval_P, eval_D1u, eval_D1v, eval_D2u, eval_D2v, eval_D2uv);
      //
      interp->GetPlotter().REDRAW_POINT("eval_P", eval_P, Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D1u", eval_P, eval_D1u, Color_Red);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D1v", eval_P, eval_D1v, Color_Green);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D2u", eval_P, eval_D2u, Color_Red);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D2v", eval_P, eval_D2v, Color_Green);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D2uv", eval_P, eval_D2uv, Color_Yellow);
      //
      Message += "\nP(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.Z() ).c_str();
      Message += ")";
      //
      Message += "\nD1u(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1u.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1u.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1u.Z() ).c_str();
      Message += ")";
      //
      Message += "\nD1v(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1v.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1v.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1v.Z() ).c_str();
      Message += ")";
      //
      Message += "\nD2u(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2u.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2u.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2u.Z() ).c_str();
      Message += ")";
      //
      Message += "\nD2v(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2v.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2v.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2v.Z() ).c_str();
      Message += ")";
      //
      Message += "\nD2uv(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2uv.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2uv.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2uv.Z() ).c_str();
      Message += ")";

      // Evaluate curvature.
      GeomLProp_SLProps slProps(occtSurface,
                                u, v, 2,
                                Precision::Confusion());
      //
      if ( slProps.IsCurvatureDefined() )
      {
        const double minK = slProps.MinCurvature();
        const double maxK = slProps.MaxCurvature();
        const double gK   = slProps.GaussianCurvature();
        const double mK   = slProps.MeanCurvature();

        Message += "\nMin curvature: ";
        Message += asiAlgo_Utils::Str::ToString<double>(minK).c_str();
        Message += "\nMax curvature: ";
        Message += asiAlgo_Utils::Str::ToString<double>(maxK).c_str();
        Message += "\nGaussian curvature: ";
        Message += asiAlgo_Utils::Str::ToString<double>(gK).c_str();
        Message += "\nMean curvature: ";
        Message += asiAlgo_Utils::Str::ToString<double>(mK).c_str();

        gp_Dir maxCurvatureDir, minCurvatureDir;
        slProps.CurvatureDirections(maxCurvatureDir, minCurvatureDir);

        interp->GetPlotter().REDRAW_AXES("principalCurvatures", eval_P, maxCurvatureDir, minCurvatureDir, eval_D1u^eval_D1v, 1);
      }
    }
    else
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Order %1 is not supported for OpenCascade method."
                                                          << order);
      return TCL_OK;
    }
  }
  else
  {
#ifndef USE_MOBIUS
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Mobius module is disabled.");
    return TCL_ERROR;
#else
    Message += "\nMethod: Mobius";

    Handle(Geom_BSplineSurface)
      occtBSurface = Handle(Geom_BSplineSurface)::DownCast(occtSurface);
    //
    if ( occtBSurface.IsNull() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "The surface in question is not a B-surface.");
      return TCL_OK;
    }

    // Convert to Mobius surface.
    cascade_BSplineSurface converter(occtBSurface);
    converter.DirectConvert();
    //
    const t_ptr<t_bsurf>&
      mobSurface = converter.GetMobiusSurface();

    // Evaluate.
    if ( order == 0 )
    {
      t_xyz eval_P;
      mobSurface->Eval(u, v, eval_P);
      //
      interp->GetPlotter().REDRAW_POINT("eval_P",
                                        gp_Pnt( eval_P.X(), eval_P.Y(), eval_P.Z() ),
                                        Color_Yellow);
      //
      Message += "\nP(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.Z() ).c_str();
      Message += ")";
    }
    else if ( order == 1 )
    {
      t_xyz eval_P, eval_D1u, eval_D1v;
      mobSurface->Eval_D1(u, v, eval_P, eval_D1u, eval_D1v);
      //
      interp->GetPlotter().REDRAW_POINT("eval_P",
                                         gp_Pnt(eval_P.X(), eval_P.Y(), eval_P.Z()),
                                         Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D1u",
                                             gp_Pnt(eval_P.X(), eval_P.Y(), eval_P.Z()),
                                             gp_Vec(eval_D1u.X(), eval_D1u.Y(), eval_D1u.Z()),
                                             Color_Red);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D1v",
                                             gp_Pnt(eval_P.X(), eval_P.Y(), eval_P.Z()),
                                             gp_Vec(eval_D1v.X(), eval_D1v.Y(), eval_D1v.Z()),
                                             Color_Green);
      //
      Message += "\nP(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.Z() ).c_str();
      Message += ")";
      //
      Message += "\nD1u(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1u.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1u.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1u.Z() ).c_str();
      Message += ")";
      //
      Message += "\nD1v(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1v.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1v.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1v.Z() ).c_str();
      Message += ")";
    }
    else if ( order == 2 )
    {
      t_xyz eval_P, eval_D1u, eval_D1v, eval_D2u, eval_D2v, eval_D2uv;
      mobSurface->Eval_D2(u, v, eval_P, eval_D1u, eval_D1v, eval_D2u, eval_D2v, eval_D2uv);
      //
      //
      interp->GetPlotter().REDRAW_POINT("eval_P",
                                         gp_Pnt(eval_P.X(), eval_P.Y(), eval_P.Z()),
                                         Color_Yellow);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D1u",
                                             gp_Pnt(eval_P.X(), eval_P.Y(), eval_P.Z()),
                                             gp_Vec(eval_D1u.X(), eval_D1u.Y(), eval_D1u.Z()),
                                             Color_Red);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D1v",
                                             gp_Pnt(eval_P.X(), eval_P.Y(), eval_P.Z()),
                                             gp_Vec(eval_D1v.X(), eval_D1v.Y(), eval_D1v.Z()),
                                             Color_Green);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D2u",
                                             gp_Pnt(eval_P.X(), eval_P.Y(), eval_P.Z()),
                                             gp_Vec(eval_D2u.X(), eval_D2u.Y(), eval_D2u.Z()),
                                             Color_Red);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D2v",
                                             gp_Pnt(eval_P.X(), eval_P.Y(), eval_P.Z()),
                                             gp_Vec(eval_D2v.X(), eval_D2v.Y(), eval_D2v.Z()),
                                             Color_Green);
      interp->GetPlotter().REDRAW_VECTOR_AT("eval_D2uv",
                                             gp_Pnt(eval_P.X(), eval_P.Y(), eval_P.Z()),
                                             gp_Vec(eval_D2uv.X(), eval_D2uv.Y(), eval_D2uv.Z()),
                                             Color_Yellow);
      //
      Message += "\nP(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_P.Z() ).c_str();
      Message += ")";
      //
      Message += "\nD1u(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1u.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1u.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1u.Z() ).c_str();
      Message += ")";
      //
      Message += "\nD1v(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1v.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1v.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D1v.Z() ).c_str();
      Message += ")";
      //
      Message += "\nD2u(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2u.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2u.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2u.Z() ).c_str();
      Message += ")";
      //
      Message += "\nD2v(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2v.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2v.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2v.Z() ).c_str();
      Message += ")";
      //
      Message += "\nD2uv(X, Y, Z): (";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2uv.X() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2uv.Y() ).c_str();
      Message += ", ";
      Message += asiAlgo_Utils::Str::ToString<double>( eval_D2uv.Z() ).c_str();
      Message += ")";
    }
    else
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Order %1 is not supported for Mobius method."
                                                          << order);
      return TCL_OK;
    }
#endif
  }
  Message += "\n----------------------------------------";
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << Message);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_EvalIsoU(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc != 3 && argc != 5 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Find Surface Node by name.
  Handle(ActAPI_INode) node = cmdEngine::model->FindNodeByName(argv[1]);
  //
  if ( node.IsNull() || !node->IsKind( STANDARD_TYPE(asiData_IVSurfaceNode) ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Node '%1' is not a surface."
                                                        << argv[1]);
    return TCL_OK;
  }
  //
  Handle(asiData_IVSurfaceNode)
    surfNode = Handle(asiData_IVSurfaceNode)::DownCast(node);

  // Get surface.
  Handle(Geom_Surface) occtSurface = surfNode->GetSurface();
  //
  if ( occtSurface.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "The surface in question is null.");
    return TCL_OK;
  }

  // Get parameter values.
  const double isoParam = atof(argv[2]);

  // Get the optional parameter on curve.
  double paramOnCurve = 0;
  const bool hasParamOnCurve = interp->GetKeyValue(argc, argv, "v", paramOnCurve);

  // Extract isoparametric curve.
  Handle(Geom_Curve) iso = occtSurface->UIso(isoParam);
  //
  interp->GetPlotter().REDRAW_CURVE("uiso", iso, Color_Red);

  // Evaluate curve at the given parameter value.
  if ( hasParamOnCurve )
  {
    gp_Pnt P = iso->Value(paramOnCurve);
    //
    interp->GetPlotter().REDRAW_POINT("P", P, Color_Green);
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_EvalIsoV(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc != 3 && argc != 5 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Find Surface Node by name.
  Handle(ActAPI_INode) node = cmdEngine::model->FindNodeByName(argv[1]);
  //
  if ( node.IsNull() || !node->IsKind( STANDARD_TYPE(asiData_IVSurfaceNode) ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Node '%1' is not a surface."
                                                        << argv[1]);
    return TCL_OK;
  }
  //
  Handle(asiData_IVSurfaceNode)
    surfNode = Handle(asiData_IVSurfaceNode)::DownCast(node);

  // Get surface.
  Handle(Geom_Surface) occtSurface = surfNode->GetSurface();
  //
  if ( occtSurface.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "The surface in question is null.");
    return TCL_OK;
  }

  // Get parameter values.
  const double isoParam = atof(argv[2]);

  // Get the optional parameter on curve.
  double paramOnCurve = 0;
  const bool hasParamOnCurve = interp->GetKeyValue(argc, argv, "u", paramOnCurve);

  // Extract isoparametric curve.
  Handle(Geom_Curve) iso = occtSurface->VIso(isoParam);
  //
  interp->GetPlotter().REDRAW_CURVE("viso", iso, Color_Green);

  // Evaluate curve at the given parameter value.
  if ( hasParamOnCurve )
  {
    gp_Pnt P = iso->Value(paramOnCurve);
    //
    interp->GetPlotter().REDRAW_POINT("P", P, Color_Red);
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckToler(const Handle(asiTcl_Interp)& interp,
                      int                          argc,
                      const char**                 argv)
{
  if ( argc != 1 && argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  const int numRanges = ( (argc == 2) ? atoi(argv[1]) : 10 );

  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  // Attempt to get the highlighted sub-shapes. If no sub-shapes are selected,
  // we work on the full part.
  TopTools_IndexedMapOfShape selectedSubShapes;
  //
  if ( cmdEngine::cf && cmdEngine::cf->ViewerPart )
  {
    asiEngine_Part PartAPI( M,
                            cmdEngine::cf->ViewerPart->PrsMgr(),
                            interp->GetProgress(),
                            interp->GetPlotter() );
    //
    PartAPI.GetHighlightedSubShapes(selectedSubShapes);
  }

  // Choose shape to check.
  TopoDS_Shape shape2Check;
  //
  if ( selectedSubShapes.Extent() )
  {
    TopoDS_Compound comp;
    BRep_Builder().MakeCompound(comp);
    //
    for ( int k = 1; k <= selectedSubShapes.Extent(); ++k )
      BRep_Builder().Add( comp, selectedSubShapes(k) );

    shape2Check = comp;
  }
  else
  {
    shape2Check = M->GetPartNode()->GetShape();
  }

  // Prepare API to analyze tolerances.
  asiEngine_TolerantShapes TolInfo( M,
                                    cmdEngine::cf->ViewerPart->PrsMgr(),
                                    interp->GetProgress(),
                                    interp->GetPlotter() );

  // Perform tolerance analysis.
  M->OpenCommand();
  {
    TolInfo.Clean_All();
    TolInfo.Populate(shape2Check, numRanges);
  }
  M->CommitCommand();

  // Update UI.
  if ( cmdEngine::cf )
    cmdEngine::cf->ObjectBrowser->Populate();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckLength(const Handle(asiTcl_Interp)& interp,
                       int                          argc,
                       const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  // Attempt to get the highlighted sub-shapes.
  TColStd_PackedMapOfInteger selectedEdgeIds;
  //
  if ( cmdEngine::cf && cmdEngine::cf->ViewerPart )
  {
    asiEngine_Part PartAPI( M,
                            cmdEngine::cf->ViewerPart->PrsMgr(),
                            interp->GetProgress(),
                            interp->GetPlotter() );
    //
    PartAPI.GetHighlightedEdges(selectedEdgeIds);
  }

  // Get total length.
  double len = 0.0;
  for ( TColStd_MapIteratorOfPackedMapOfInteger eit(selectedEdgeIds); eit.More(); eit.Next() )
  {
    const int edgeId = eit.Key();

    // Get edge.
    const TopoDS_Shape&
      edge = M->GetPartNode()->GetAAG()->RequestMapOfEdges()(edgeId);

    // Calculate global properties.
    GProp_GProps props;
    BRepGProp::LinearProperties(edge, props);
    len += props.Mass();
  }

  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Length: %1." << len);
  *interp << len;

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckArea(const Handle(asiTcl_Interp)& interp,
                     int                          argc,
                     const char**                 argv)
{
  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  // Access selected faces (if any).
  asiAlgo_Feature selected;
  //
  if ( !cmdEngine::cf.IsNull() )
  {
    asiEngine_Part( cmdEngine::cf->Model,
                    cmdEngine::cf->ViewerPart->PrsMgr() ).GetHighlightedFaces(selected);
  }

  // Get the face in question.
  int fid = 0;
  interp->GetKeyValue<int>(argc, argv, "fid", fid);
  //
  if ( fid ) selected.Add(fid);

  // Get total area.
  double area = 0.0;
  for ( TColStd_MapIteratorOfPackedMapOfInteger fit(selected); fit.More(); fit.Next() )
  {
    const int faceId = fit.Key();

    // Get face.
    const TopoDS_Shape&
      face = M->GetPartNode()->GetAAG()->GetMapOfFaces()(faceId);

    // Calculate global properties.
    GProp_GProps props;
    BRepGProp::SurfaceProperties(face, props);
    area += props.Mass();
  }

  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Area: %1." << area);
  *interp << area;

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckVolume(const Handle(asiTcl_Interp)& interp,
                       int                          argc,
                       const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get part.
  Handle(asiData_PartNode)
    partNode = cmdEngine::model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part Node is null or ill-defined.");
    return TCL_OK;
  }
  //
  TopoDS_Shape partShape = partNode->GetShape();

  // Get all solids of the part.
  TopTools_IndexedMapOfShape allSolids;
  TopExp::MapShapes(partShape, TopAbs_SOLID, allSolids);

  if ( allSolids.Extent() > 1 )
  {
    interp->GetProgress().SendLogMessage(LogWarn(Normal) << "The active part contains more than one solid. "
                                                            "Try 'explode' command.");
  }
  else if ( !allSolids.Extent() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "The active part does not contain any solid.");
    return TCL_ERROR;
  }

  // Get total volume.
  double volume = 0.0;
  for ( int k = 1; k <= allSolids.Extent(); ++k )
  {
    TopoDS_Solid solid = TopoDS::Solid( allSolids(k) );

    // Calculate global properties.
    GProp_GProps props;
    BRepGProp::VolumeProperties(solid, props);
    volume += props.Mass();
  }

  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Volume: %1." << volume);
  *interp << volume;

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckAABB(const Handle(asiTcl_Interp)& interp,
                     int                          argc,
                     const char**                 argv)
{
  // Get shape to analyze.
  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );
  //
  TopoDS_Shape partShape = M->GetPartNode()->GetShape();

  TIMER_NEW
  TIMER_GO

  Bnd_Box bbox;
  //
  if ( interp->HasKeyword(argc, argv, "opt") )
    BRepBndLib::AddOptimal(partShape, bbox, interp->HasKeyword(argc, argv, "tris"), false);
  else
    BRepBndLib::Add(partShape, bbox, interp->HasKeyword(argc, argv, "tris"));

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Compute AABB")

  std::vector<double> dim = { fabs( bbox.CornerMax().X() - bbox.CornerMin().X() ),
                              fabs( bbox.CornerMax().Y() - bbox.CornerMin().Y() ),
                              fabs( bbox.CornerMax().Z() - bbox.CornerMin().Z() ) };

  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Dx: %1." << dim[0]);
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Dy: %1." << dim[1]);
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Dz: %1." << dim[2]);

  // Protect from degenerated bbox.
  if ( ( bbox.CornerMin().X() - bbox.CornerMax().X() ) < Precision::Confusion() ||
       ( bbox.CornerMin().Y() - bbox.CornerMax().Y() ) < Precision::Confusion() ||
       ( bbox.CornerMin().Z() - bbox.CornerMax().Z() ) < Precision::Confusion() )
  {
    bbox.Enlarge( Precision::Confusion() );
  }

  // Create bounding box to draw it.
  TopoDS_Shape bndbox = BRepPrimAPI_MakeBox( gp_Pnt( bbox.CornerMin().X(),
                                                     bbox.CornerMin().Y(),
                                                     bbox.CornerMin().Z() ),
                                             gp_Pnt( bbox.CornerMax().X(),
                                                     bbox.CornerMax().Y(),
                                                     bbox.CornerMax().Z() ) );
  //
  interp->GetPlotter().REDRAW_SHAPE("bbox", bndbox, Color_Yellow, 1.0, true);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckValidity(const Handle(asiTcl_Interp)& interp,
                         int                          argc,
                         const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  asiAlgo_CheckValidity checker( interp->GetProgress(), interp->GetPlotter() );

  // Get Part Node.
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();

  // Get Part shape.
  TopoDS_Shape partSh = part_n->GetShape();

  if ( !checker.CheckBasic(partSh) )
  {
    *interp << 0;
  }
  else
  {
    *interp << 1;
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckFinite(const Handle(asiTcl_Interp)& interp,
                       int                          argc,
                       const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node.
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();

  // Get Part shape.
  TopoDS_Shape partSh = part_n->GetShape();

  // Get all contained solids.
  TopTools_IndexedMapOfShape solids;
  TopExp::MapShapes(partSh, TopAbs_SOLID, solids);
  //
  if ( solids.IsEmpty() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "There are no solids in the part.");
    return TCL_ERROR;
  }

  asiAlgo_CheckValidity checker;

  // Check contained solids.
  bool isOneSolid = (solids.Extent() == 1);
  bool areAllOk   = true;
  int  solidIdx   = 0;
  //
  for ( int k = 1; k <= solids.Extent(); ++k )
  {
    const TopoDS_Solid& solid = TopoDS::Solid( solids(k) );
    solidIdx++;

    // Check next solid.
    const bool isNextOk = checker.IsFinite(solid);
    //
    if ( !isNextOk && areAllOk )
      areAllOk = false;

    // Print message.
    if ( isOneSolid )
    {
      if ( isNextOk )
        interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Solid is finite.");
      else
        interp->GetProgress().SendLogMessage(LogWarn(Normal) << "Solid is infinite.");
    }
    else
    {
      if ( isNextOk )
        interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Solid %1 is finite."
                                                             << solidIdx);
      else
        interp->GetProgress().SendLogMessage(LogWarn(Normal) << "Solid %1 is infinite."
                                                             << solidIdx);
    }
  }

  // Add checker result to the interpretor.
  *interp << (areAllOk ? 1 : 0);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckContours(const Handle(asiTcl_Interp)& interp,
                         int                          argc,
                         const char**                 argv)
{
  if ( argc != 1 && argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Read tolerance.
  double globTolerance = ( (argc == 2) ? atof(argv[1]) : 0.0 );

  // Get Part Node.
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();

  // Get Part shape.
  TopoDS_Shape partSh = part_n->GetShape();

  asiAlgo_CheckValidity checker;

  // Check each face individually.
  bool isOk = true;
  //
  for ( TopExp_Explorer exp(partSh, TopAbs_FACE); exp.More(); exp.Next() )
  {
    const TopoDS_Face& face = TopoDS::Face( exp.Current() );

    // Set default tolerance.
    double locTolerance;
    if ( globTolerance )
      locTolerance = globTolerance;
    else
      locTolerance = checker.GetMaxTolerance(face)*5.0;

    // Check closeness.
    if ( !checker.HasAllClosedWires(face, locTolerance) )
    {
      isOk = false;
      break;
    }
  }

  if ( isOk )
    interp->GetProgress().SendLogMessage(LogInfo(Normal) << "All faces have closed contours.");
  else
    interp->GetProgress().SendLogMessage(LogWarn(Normal) << "Some contours contain gaps.");

  *interp << (isOk ? 1 : 0);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_GetTolerance(const Handle(asiTcl_Interp)& interp,
                        int                          argc,
                        const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node.
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();

  // Return max tolerance to the interpreter.
  *interp << asiAlgo_CheckValidity::MaxTolerance( part_n->GetShape() );

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_GetCurveStrain(const Handle(asiTcl_Interp)& interp,
                          int                          argc,
                          const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Find Curve Node by name.
  Handle(ActAPI_INode) node = cmdEngine::model->FindNodeByName(argv[1]);
  //
  if ( node.IsNull() || !node->IsKind( STANDARD_TYPE(asiData_IVCurveNode) ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Node '%1' is not a curve."
                                                        << argv[1]);
    return TCL_OK;
  }
  //
  Handle(asiData_IVCurveNode)
    curveNode = Handle(asiData_IVCurveNode)::DownCast(node);

  // Get curve.
  double f, l;
  Handle(Geom_Curve) occtCurve = curveNode->GetCurve(f, l);
  //
  if ( occtCurve.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "The curve in question is null.");
    return TCL_OK;
  }

  // Calculate strain energy.
  double energy = 0;
  if ( !asiAlgo_Utils::CalculateStrainEnergy(occtCurve, energy) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot calculate strain energy.");
    return TCL_OK;
  }

  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Approximate strain energy: %1."
                                                       << energy);

  // Add to interpreter.
  *interp << energy;

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_GetSurfaceBending(const Handle(asiTcl_Interp)& interp,
                             int                          argc,
                             const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Find Surface Node by name.
  Handle(ActAPI_INode) node = cmdEngine::model->FindNodeByName(argv[1]);
  //
  if ( node.IsNull() || !node->IsKind( STANDARD_TYPE(asiData_IVSurfaceNode) ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Node '%1' is not a surface."
                                                        << argv[1]);
    return TCL_OK;
  }
  //
  Handle(asiData_IVSurfaceNode)
    surfNode = Handle(asiData_IVSurfaceNode)::DownCast(node);

  // Get surface.
  Handle(Geom_Surface) occtSurf = surfNode->GetSurface();
  //
  if ( occtSurf.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "The surface in question is null.");
    return TCL_OK;
  }

  // Calculate bending energy.
  double energy = 0;
  if ( !asiAlgo_Utils::CalculateBendingEnergy(occtSurf, energy) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot calculate bending energy.");
    return TCL_OK;
  }

  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Approximate bending energy: %1."
                                                       << energy);

  // Add to interpreter.
  *interp << energy;

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckEdgeVexity(const Handle(asiTcl_Interp)& interp,
                           int                          argc,
                           const char**                 argv)
{
  if ( argc != 2 && argc != 3 && argc != 4 && argc != 6 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node and shape.
  Handle(asiData_PartNode) part_n    = cmdEngine::model->GetPartNode();
  TopoDS_Shape             partShape = part_n->GetShape();

  // Check whether naming service is active.
  const bool hasNaming = !part_n->GetNaming().IsNull();

  // Check whether "smooth" type is allowed.
  const bool allowSmooth = interp->HasKeyword(argc, argv, "allow-smooth");

  // Get tolerance for recognition of smooth edges.
  double smoothToler = 0.0;
  TCollection_AsciiString smoothTolerStr;
  //
  if ( interp->GetKeyValue(argc, argv, "smooth-toler", smoothTolerStr) )
    smoothToler = smoothTolerStr.RealValue();

  // Edge to check.
  TopoDS_Edge edge;

  // Check if naming service is active. If so, the user may ask to access
  // a sub-shape in question by its unique name.
  if ( hasNaming && argc == 3 )
  {
    if ( !interp->IsKeyword(argv[1], "name") )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Keyword '-name' is expected.");
      return TCL_ERROR;
    }
    else
    {
      TCollection_AsciiString name(argv[2]);
      //
      TopoDS_Shape subshape = part_n->GetNaming()->GetShape(name);
      //
      if ( subshape.IsNull() || subshape.ShapeType() != TopAbs_EDGE )
      {
        interp->GetProgress().SendLogMessage(LogErr(Normal) << "The passed sub-shape is null "
                                                               "or not of a proper type.");
        return TCL_ERROR;
      }
      //
      edge = TopoDS::Edge(subshape);
    }
  }
  else // Naming is not used to access the argument.
  {
    const int ssidx = atoi(argv[1]);
    //
    if ( ssidx < 1 )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Sub-shape index should be 1-based.");
      return TCL_ERROR;
    }

    // Get map of sub-shapes with respect to those the passed index is relevant.
    TopTools_IndexedMapOfShape subShapesOfType;
    part_n->GetAAG()->RequestMapOf(TopAbs_EDGE, subShapesOfType);

    // Get sub-shape in question.
    edge = TopoDS::Edge( subShapesOfType(ssidx) );
  }

  interp->GetPlotter().REDRAW_SHAPE("edge", edge, Color_White, 1.0, true);

  // Get faces owning the edge in question.
  TopTools_IndexedDataMapOfShapeListOfShape edgeFaceMap;
  TopExp::MapShapesAndAncestors(part_n->GetShape(), TopAbs_EDGE, TopAbs_FACE, edgeFaceMap);
  //
  if ( !edgeFaceMap.Contains(edge) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "No faces for edge.");
    return TCL_ERROR;
  }
  //
  const TopTools_ListOfShape& faces = edgeFaceMap.FindFromKey(edge);
  //
  if ( faces.Extent() != 2 )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Edge should belong to exactly two faces.");
    return TCL_ERROR;
  }
  //
  const TopoDS_Face& F = TopoDS::Face( faces.First() );
  const TopoDS_Face& G = TopoDS::Face( faces.Last() );

  /* ===============
   *  Perform check
   * =============== */

  TIMER_NEW
  TIMER_GO

  // Prepare algorithm.
  asiAlgo_CheckDihedralAngle dihAngle( interp->GetProgress(),
                                       interp->GetPlotter() );
  //
  dihAngle.SetCommonEdge(edge);

  // Check.
  const asiAlgo_FeatureAngleType
    angleType = dihAngle.AngleBetweenFaces(F, G, allowSmooth, smoothToler);

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Check dihedral angle")

  // Add as a result to interpreter.
  *interp << angleType;

  interp->GetProgress().SendLogMessage( LogInfo(Normal) << "Edge vexity: %1."
                                                        << asiAlgo_FeatureAngle::ToString(angleType) );

  // Update UI.
  if ( cmdEngine::cf && cmdEngine::cf->ViewerPart )
  {
    double colorR, colorG, colorB;
    asiVisu_Utils::ColorForFeatureAngle(angleType, colorR, colorG, colorB);
    //
    interp->GetPlotter().REDRAW_SHAPE("vexity",
                                       edge,
                                       ActAPI_Color(colorR,
                                                    colorG,
                                                    colorB,
                                                    Quantity_TOC_RGB),
                                       1.0,
                                       true);
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_DrawCP(const Handle(asiTcl_Interp)& interp,
                  int                          argc,
                  const char**                 argv)
{
  if ( argc != 4 && argc != 5 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Find Surface Node by name.
  Handle(ActAPI_INode) node = cmdEngine::model->FindNodeByName(argv[1]);
  //
  if ( node.IsNull() || !node->IsKind( STANDARD_TYPE(asiData_IVSurfaceNode) ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Node '%1' is not a surface."
                                                        << argv[1]);
    return TCL_OK;
  }
  //
  Handle(asiData_IVSurfaceNode)
    surfNode = Handle(asiData_IVSurfaceNode)::DownCast(node);

  // Get parametric surface.
  Handle(Geom_BSplineSurface)
    occtSurf = Handle(Geom_BSplineSurface)::DownCast( surfNode->GetSurface() );
  //
  if ( occtSurf.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "The surface is null or not a spline surface.");
    return TCL_ERROR;
  }

  // Get indices of a control point in question.
  const int i = atoi(argv[2]);
  const int j = atoi(argv[3]);

  const TColgp_Array2OfPnt& poles = occtSurf->Poles();
  //
  const int numRows = poles.ColLength();
  const int numCols = poles.RowLength();
  //
  if ( i < 0 || i >= numRows )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "First index if out of range [0, %1]." << (numRows - 1));
    return TCL_ERROR;
  }
  //
  if ( j < 0 || j >= numCols )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Second index if out of range [0, %1]." << (numCols - 1));
    return TCL_ERROR;
  }

  // Check whether Mobius operator is requested.
  gp_Pnt P;
  //
  const bool isMobius = interp->HasKeyword(argc, argv, "mobius");
  //
  if ( isMobius )
  {
#if defined USE_MOBIUS
    // Convert to Mobius surface.
    cascade_BSplineSurface converter(occtSurf);
    converter.DirectConvert();
    //
    const t_ptr<t_bsurf>& mobSurf = converter.GetMobiusSurface();

    const t_xyz& mobP = mobSurf->GetPoles()[i][j];
    //
    P.SetX( mobP.X() );
    P.SetY( mobP.Y() );
    P.SetZ( mobP.Z() );
#else
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Mobius is not available.");
    return TCL_ERROR;
#endif
  }
  else
  {
    P = poles(i + 1, j + 1);
  }

  // Draw control point.
  TCollection_AsciiString PName("P_");
  PName += i;
  PName += "_";
  PName += j;
  //
  interp->GetPlotter().REDRAW_POINT(PName, P, isMobius ? Color_Blue : Color_Red);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckOpenEdges(const Handle(asiTcl_Interp)& interp,
                          int                          argc,
                          const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get part.
  Handle(asiData_PartNode)
    partNode = cmdEngine::model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part Node is null or ill-defined.");
    return TCL_OK;
  }
  //
  TopoDS_Shape partShape = partNode->GetShape();

  // Build map of edges to extract open ("naked") ones.
  TopTools_IndexedDataMapOfShapeListOfShape edgesFaces;
  TopExp::MapShapesAndAncestors(partShape, TopAbs_EDGE, TopAbs_FACE, edgesFaces);

  // Find open edges.
  Handle(TopTools_HSequenceOfShape) openEdgesSeq = new TopTools_HSequenceOfShape;
  //
  for ( int k = 1; k <= edgesFaces.Extent(); ++k )
  {
    const TopTools_ListOfShape& faces = edgesFaces(k);
    //
    if ( faces.Extent() == 1 )
    {
      const TopoDS_Edge& E = TopoDS::Edge( edgesFaces.FindKey(k) );
      //
      if ( BRep_Tool::Degenerated(E) )
        continue;

      openEdgesSeq->Append(E);
    }
  }

  // Compose border wires from the naked edges.
  Handle(TopTools_HSequenceOfShape) borderWires;
  ShapeAnalysis_FreeBounds::ConnectEdgesToWires(openEdgesSeq, 1e-3, 0, borderWires);

  // Dump free wires.
  if ( !borderWires.IsNull() )
  {
    for ( TopTools_HSequenceOfShape::Iterator wit(*borderWires); wit.More(); wit.Next() )
    {
      const TopoDS_Wire& wire = TopoDS::Wire( wit.Value() );

      // Calculate global properties.
      GProp_GProps props;
      BRepGProp::LinearProperties(wire, props);
      const double wireLen = props.Mass();

      interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Length of the next border wire: %1." << wireLen);
      interp->GetPlotter().DRAW_SHAPE(wire, Color_Red, 1.0, true, "free_wire");
    }
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckVerticesOri(const Handle(asiTcl_Interp)& interp,
                            int                          argc,
                            const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get part.
  Handle(asiData_PartNode)
    partNode = cmdEngine::model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part Node is null or ill-defined.");
    return TCL_OK;
  }
  //
  TopoDS_Shape partShape = partNode->GetShape();

  // Build map of edges to check them one-by-one.
  TopTools_IndexedMapOfShape edgesMap;
  TopExp::MapShapes(partShape, TopAbs_EDGE, edgesMap);

  asiAlgo_CheckValidity checker;

  // Check edges.
  for ( int k = 1; k <= edgesMap.Extent(); ++k )
  {
    const TopoDS_Edge& edge = TopoDS::Edge( edgesMap(k) );

    if ( !checker.HasDistinctVertexOrientations(edge) )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Edge %1 has non-distinguishable vertices."
                                                          << k);
      //
      *interp << false;

      return TCL_OK;
    }
  }

  // Everything is fine.
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "All edges have distinguishable vertices.");
  //
  *interp << true;

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckInternalLocations(const Handle(asiTcl_Interp)& interp,
                                  int                          argc,
                                  const char**                 argv)
{
  if ( argc != 1 && argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  const bool findFaces = interp->HasKeyword(argc, argv, "f");

  // Get part.
  Handle(asiData_PartNode)
    partNode = cmdEngine::model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part Node is null or ill-defined.");
    return TCL_OK;
  }
  //
  TopoDS_Shape partShape = partNode->GetShape();

  // Check and result.
  *interp << asiAlgo_Utils::HasInternalLocations(partShape, findFaces ? TopAbs_FACE : TopAbs_SHAPE);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_RecognizeBlends(const Handle(asiTcl_Interp)& interp,
                           int                          argc,
                           const char**                 argv)
{
  // Attempt to read the seed face.
  int fid = 0;
  TCollection_AsciiString fidStr;
  //
  if ( interp->GetKeyValue(argc, argv, "fid", fidStr) )
    fid = fidStr.IntegerValue();

  // Attempt to read the blend radius.
  double maxRadius = 1e100;
  TCollection_AsciiString maxRadiusStr;
  //
  if ( interp->GetKeyValue(argc, argv, "radius", maxRadiusStr) )
    maxRadius = maxRadiusStr.RealValue();

  // Get EBF/VBF qualifier.
  const bool isEbf = interp->HasKeyword(argc, argv, "ebf");
  const bool isVbf = interp->HasKeyword(argc, argv, "vbf");

  // Whether to store the recognition result.
  const bool isStore = interp->HasKeyword(argc, argv, "store");

  double rDevPerc = 1.0;
  interp->GetKeyValue<double>(argc, argv, "cdev", rDevPerc);

  // Get part.
  Handle(asiData_PartNode)
    partNode = cmdEngine::model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part Node is null or ill-defined.");
    return TCL_OK;
  }
  //
  TopoDS_Shape        partShape = partNode->GetShape();
  Handle(asiAlgo_AAG) partAAG   = partNode->GetAAG();

  // Perform recognition.
  asiAlgo_RecognizeBlends recognizer( partShape,
                                      partAAG,
                                      interp->GetProgress()/*,
                                      interp->GetPlotter() */);
  //
  if ( !recognizer.Perform(fid, maxRadius) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Recognition failed.");
    return TCL_ERROR;
  }

  // Get the detected faces.
  TColStd_PackedMapOfInteger resIndices;
  //
  if ( (!isEbf && !isVbf) || (isEbf && isVbf) )
  {
    resIndices = recognizer.GetResultIndices();
  }
  else
  {
    // Prepare tool to extract features from AAG.
    asiAlgo_ExtractFeatures extractor(interp->GetProgress(), nullptr);
    extractor.RegisterFeatureType( FeatureType_BlendOrdinary,
                                   asiAlgo_AttrBlendCandidate::GUID() );

    // Use extraction filter.
    Handle(cmdEngine_ExtractBlendsFilter)
      filter = new cmdEngine_ExtractBlendsFilter(isEbf ? BlendType_Ordinary : BlendType_Vertex);

    // Extract features.
    Handle(asiAlgo_ExtractFeaturesResult) featureRes;
    //
    if ( !extractor.Perform(partAAG, featureRes, filter) )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Feature extraction failed.");
      return TCL_ERROR;
    }
    //
    featureRes->GetFaceIndices(resIndices);
  }

  asiEngine_Part partApi( cmdEngine::model,
                          cmdEngine::cf.IsNull() ? nullptr : cmdEngine::cf->ViewerPart->PrsMgr() );

  // Highlight the detected faces.
  if ( !cmdEngine::cf.IsNull() && cmdEngine::cf->ViewerPart )
    partApi.HighlightFaces(resIndices);

  if ( isStore )
  {
    // Extract chains.
    std::vector<asiAlgo_BlendChain> chains;
    //
    recognizer.GetChains(chains, rDevPerc);

    // Perform data model modification.
    cmdEngine::model->OpenCommand();
    {
      int chainId = 0;
      //
      for ( const auto& chain : chains )
      {
        const int numFeatures = partApi.GetNumOfFeatures();

        // Get feature to store the recognition result.
        Handle(asiData_FeatureNode)
          featureNode = partApi.FindFeature(numFeatures + 1, true);
        //
        featureNode->SetName( TCollection_AsciiString("Blend chain ") + (++chainId) );

        // Store indices.
        featureNode->SetMask(chain.first);
      }
    }
    cmdEngine::model->CommitCommand();

    // Update object browser.
    if ( cmdEngine::cf && cmdEngine::cf->ObjectBrowser )
      cmdEngine::cf->ObjectBrowser->Populate();
  }

  // Dump to result.
  *interp << resIndices;

  return TCL_OK;
}
//-----------------------------------------------------------------------------

int ENGINE_RecognizeHoles(const Handle(asiTcl_Interp)& interp,
                          int                          argc,
                          const char**                 argv)
{
  /* ==============
   *  Get prepared.
   * ============== */

  // Get part.
  Handle(asiData_PartNode)
    partNode = cmdEngine::model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part Node is null or ill-defined.");
    return TCL_OK;
  }
  //
  TopoDS_Shape        partShape = partNode->GetShape();
  Handle(asiAlgo_AAG) G         = partNode->GetAAG();

  // Read max allowed radius.
  double r = Precision::Infinite();
  interp->GetKeyValue<double>(argc, argv, "radius", r);
  //
  if ( Abs(r) < gp::Resolution() )
    r = Precision::Infinite();

  /* =========================
   *  Recognize drilled holes.
   * ========================= */

  TIMER_NEW
  TIMER_GO

  // Recognize cavities.
  asiAlgo_RecognizeDrillHoles recHoles( G, true,
                                        interp->GetProgress(),
                                        interp->GetPlotter() );
  //
  if ( !recHoles.Perform(r) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Drilled holes recognition failed.");
    return TCL_ERROR;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Recognize drilled holes")

  // Get the extracted face indices.
  const asiAlgo_Feature& resIndices = recHoles.GetResultIndices();

  /* ==========
   *  Finalize.
   * ========== */

  asiEngine_Part partApi( cmdEngine::model,
                          cmdEngine::cf.IsNull() ? nullptr : cmdEngine::cf->ViewerPart->PrsMgr() );

  // Highlight the detected faces.
  if ( !cmdEngine::cf.IsNull() && cmdEngine::cf->ViewerPart )
    partApi.HighlightFaces(resIndices);

  // Dump to result.
  *interp << resIndices;

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_RecognizeCavities(const Handle(asiTcl_Interp)& interp,
                             int                          argc,
                             const char**                 argv)
{
  /* ==============
   *  Get prepared.
   * ============== */

  // Get part.
  Handle(asiData_PartNode)
    partNode = cmdEngine::model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part Node is null or ill-defined.");
    return TCL_OK;
  }
  //
  TopoDS_Shape        partShape = partNode->GetShape();
  Handle(asiAlgo_AAG) G         = partNode->GetAAG();

  // Read max allowed feature size.
  double maxSize = Precision::Infinite();
  interp->GetKeyValue<double>(argc, argv, "size", maxSize);
  //
  if ( Abs(maxSize) < gp::Resolution() )
    maxSize = Precision::Infinite();

  /* ==============================
   *  Recognize arbitrary cavities.
   * ============================== */

  TIMER_NEW
  TIMER_GO

  // Recognize cavities.
  asiAlgo_RecognizeCavities recCavities( G,
                                         interp->GetProgress(),
                                         interp->GetPlotter() );
  //
  recCavities.SetMaxSize(maxSize);
  //
  if ( !recCavities.Perform() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cavity recognition failed.");
    return TCL_ERROR;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Recognize cavities")

  // Get the extracted face indices.
  const asiAlgo_Feature& resIndices = recCavities.GetResultIndices();

  // Print more details, if asked.
  if ( interp->HasKeyword(argc, argv, "details") )
  {
    const std::vector< std::pair<asiAlgo_Feature, asiAlgo_Feature> >&
      cavities = recCavities.GetCavities();

    std::string lbl;
    lbl += "\n----------------------";
    lbl += "\n Feature -> base faces";
    lbl += "\n----------------------";
    //
    for ( const auto& tuple : cavities )
    {
      const asiAlgo_Feature& feature = tuple.first;
      const asiAlgo_Feature& bases   = tuple.second;

      lbl += "\n";
      lbl += asiAlgo_Utils::Json::FromFeature(feature);
      lbl += " -> ";
      lbl += asiAlgo_Utils::Json::FromFeature(bases);
    }

    interp->GetProgress().SendLogMessage( LogInfo(Normal) << lbl.c_str() );
  }

  /* ==========
   *  Finalize.
   * ========== */

  asiEngine_Part partApi( cmdEngine::model,
                          cmdEngine::cf.IsNull() ? nullptr : cmdEngine::cf->ViewerPart->PrsMgr() );

  // Highlight the detected faces.
  if ( !cmdEngine::cf.IsNull() && cmdEngine::cf->ViewerPart )
    partApi.HighlightFaces(resIndices);

  // Dump to result.
  *interp << resIndices;

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_DrawPlot(const Handle(asiTcl_Interp)& interp,
                    int                          argc,
                    const char**                 argv)
{
  if ( argc < 5 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  const bool isLogScale = interp->HasKeyword(argc, argv, "log");

  int numFunc = 1; // One by default.
  interp->GetKeyValue<int>(argc, argv, "numFunc", numFunc);

  // Prepare table of values to plot.
  std::vector<double> xvec;
  std::vector< std::vector<double> > fxvec;
  //
  for ( int k = 0; k < numFunc; ++k )
    fxvec.push_back( std::vector<double>() );

  // Collect data.
  for ( int k = 1; k < argc; k += (numFunc + 1) )
  {
    if ( interp->IsKeyword(argv[k], "log") ) continue;
    if ( interp->IsKeyword(argv[k], "numFunc") ) break;

    // Coordinate value.
    const double x = atof(argv[k]);

    // Function values.
    for ( int j = 1; j <= numFunc; ++j )
    {
      const double fx = atof(argv[k + j]);
      fxvec[j - 1].push_back(fx);
    }

    xvec.push_back(x);
  }

  // Open curvature plot.
  asiUI_Plot2d*
    cPlot = new asiUI_Plot2d( interp->GetProgress(),
                              interp->GetPlotter() );
  //
  cPlot->SetLogScale(isLogScale);
  //
  cPlot->Render(xvec, fxvec, "x", "fx", "Plot 2d");

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_ShowAAG(const Handle(asiTcl_Interp)& interp,
                   int                          argc,
                   const char**                 argv)
{
  if ( !cmdEngine::cf || !cmdEngine::cf->ViewerPart )
  {
    interp->GetProgress().SendLogMessage(LogWarn(Normal) << "Part viewer is not available.");
    return TCL_OK;
  }

  // Get options [-remove-sel] [-collapse-sel] [-add-vertex-adj].
  const bool removeSelected   = interp->HasKeyword(argc, argv, "remove-sel");
  const bool collapseSelected = interp->HasKeyword(argc, argv, "collapse-sel");
  const bool addVertAdj       = interp->HasKeyword(argc, argv, "add-vertex-adj");
  const bool copyGraph        = collapseSelected || addVertAdj;

  // CHeck if there is a filename for JSON dump.
  std::string filename;
  const bool doDumpFs = interp->GetKeyValue(argc, argv, "filename", filename);

  // Get part.
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(cmdEngine::model, part_n, part) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part is not initialized.");
    return TCL_ERROR;
  }

  // Get AAG.
  Handle(asiAlgo_AAG) aag = part_n->GetAAG();

  // Access selected faces (if any).
  TColStd_PackedMapOfInteger selected;
  asiEngine_Part( cmdEngine::model, cmdEngine::cf->ViewerPart->PrsMgr() ).GetHighlightedFaces(selected);

  if ( removeSelected )
  {
    aag->PushSubgraphX(selected);
  }

  if ( copyGraph )
  {
    aag->PushSubgraph(); // Make a copy of the adjacency matrix.
  }

  if ( addVertAdj )
  {
    aag->AddVertexAdjacencyArcs();
  }

  if ( collapseSelected )
  {
    aag->Collapse(selected);
  }

  // Show graph.
  asiUI_PartGraph* pGraphView = new asiUI_PartGraph( cmdEngine::model,
                                                     cmdEngine::cf->ViewerPart,
                                                     interp->GetProgress() );
  //
  pGraphView->RenderAdjacency(aag);

  // Dump to file.
  if ( doDumpFs )
  {
    std::ofstream filestream(filename);
    //
    if ( !filestream.is_open() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "File '%1' cannot be opened for writing."
                                                          << filename);
      return TCL_ERROR;
    }
    //
    aag->DumpJSON(filestream);
    filestream << "\n";
    //
    filestream.close();
  }

  // Undo modifications.
  if ( removeSelected || copyGraph )
  {
    aag->PopSubgraph();
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_IsolateRealParts(const Handle(asiTcl_Interp)& interp,
                            int                          argc,
                            const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(cmdEngine::model, part_n, part) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part is not initialized.");
    return TCL_ERROR;
  }

  // Isolate.
  TopTools_ListOfShape realParts;
  asiAlgo_Utils::IsolateRealParts(part, realParts);

  // Draw.
  for ( TopTools_ListIteratorOfListOfShape lit(realParts); lit.More(); lit.Next() )
  {
    const TopoDS_Shape& realPart = lit.Value();

    // Generate name and draw.
    TCollection_AsciiString name = asiAlgo_Utils::ShapeTypeStr(realPart).c_str();
    interp->GetPlotter().DRAW_SHAPE(realPart, name);
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckAlongCurvature(const Handle(asiTcl_Interp)& interp,
                               int                          argc,
                               const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  // Attempt to get the highlighted sub-shapes.
  TColStd_PackedMapOfInteger selectedEdgeIds;
  //
  if ( cmdEngine::cf && cmdEngine::cf->ViewerPart )
  {
    asiEngine_Part PartAPI( M,
                            cmdEngine::cf->ViewerPart->PrsMgr(),
                            interp->GetProgress(),
                            interp->GetPlotter() );
    //
    PartAPI.GetHighlightedEdges(selectedEdgeIds);
  }

  if ( selectedEdgeIds.Extent() != 1 )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Exactly one edge should be selected.");
    return TCL_ERROR;
  }

  const int edgeId = selectedEdgeIds.GetMinimalMapped();

  // Get edge.
  const TopoDS_Edge&
    edge = TopoDS::Edge( M->GetPartNode()->GetAAG()->RequestMapOfEdges()(edgeId) );

  // Get faces.
  const TopTools_IndexedDataMapOfShapeListOfShape&
    EF = M->GetPartNode()->GetAAG()->RequestMapOfEdgesFaces();
  //
  const TopTools_ListOfShape& ownerFaces = EF.FindFromIndex(edgeId);

  // Check on-surface curvature for the edge and its every owning face.
  for ( TopTools_ListIteratorOfListOfShape lif(ownerFaces); lif.More(); lif.Next() )
  {
    const TopoDS_Face& ownerFace = TopoDS::Face( lif.Value() );

    // Get face ID.
    const int faceId = M->GetPartNode()->GetAAG()->GetFaceId(ownerFace);

    // Evaluate curvature.
    double k;
    if ( !asiAlgo_Utils::EvaluateAlongCurvature(ownerFace, edge, k) )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot evaluate on-surface curvature "
                                                             "for the edge %1 on face %2."
                                                          << edgeId << faceId);
      return TCL_ERROR;
    }

    // Radius of curvature.
    const double r = ( Abs(k) > 1.e-4 ? 1/Abs(k) : Precision::Infinite() );

    interp->GetProgress().SendLogMessage(LogInfo(Normal) << "On-surface curvature for the edge %1 on face %2 "
                                                            "in the midpoint is %3 (radius: %4)."
                                                         << edgeId << faceId << k << r);
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_InvertPointSurf(const Handle(asiTcl_Interp)& interp,
                           int                          argc,
                           const char**                 argv)
{
#if defined USE_MOBIUS
  if ( argc != 5 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  const double px = atof(argv[2]);
  const double py = atof(argv[3]);
  const double pz = atof(argv[4]);
  //
  interp->GetPlotter().REDRAW_POINT("p", gp_Pnt(px, py, pz), Color_Yellow);

  // Find Node by name.
  Handle(ActAPI_INode) node = cmdEngine::model->FindNodeByName(argv[1]);
  //
  if ( node.IsNull() || !node->IsKind( STANDARD_TYPE(asiData_IVSurfaceNode) ) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Node '%1' is not a surface."
                                                        << argv[1]);
    return TCL_OK;
  }
  //
  Handle(asiData_IVSurfaceNode)
    surfNode = Handle(asiData_IVSurfaceNode)::DownCast(node);

  // Get surface.
  Handle(Geom_Surface) occtSurface = surfNode->GetSurface();
  //
  if ( occtSurface.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "The surface in question is null.");
    return TCL_ERROR;
  }

  // Depending on the surface type, use different inversion approaches.
  double ures = 0., vres = 0.;
  //
  if ( occtSurface->IsKind( STANDARD_TYPE(Geom_BSplineSurface) ) )
  {
    // Convert to Mobius B-surface.
    t_ptr<t_bsurf>
      mobSurf = cascade::GetMobiusBSurface( Handle(Geom_BSplineSurface)::DownCast(occtSurface) );

    // Set diagnostic tools.
    t_ptr<asiUI_IVMobius> ivMob = new asiUI_IVMobius( interp->GetPlotter().Access() );
    //
    mobSurf->SetDiagnosticTools( nullptr, core_PlotterEntry(ivMob) );

    // Invert point.
    t_uv projUV;
    //
    if ( !mobSurf->InvertPoint(t_xyz(px, py, pz), projUV) )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Point inversion onto B-surface failed.");
      return TCL_ERROR;
    }

    // Set result.
    ures = projUV.U();
    vres = projUV.V();
  }
  else if ( occtSurface->IsKind( STANDARD_TYPE(Geom_Plane) ) )
  {
    // Convert to Mobius plane.
    t_ptr<t_plane>
      mobSurf = cascade::GetMobiusPlane( Handle(Geom_Plane)::DownCast(occtSurface) );

    // Invert point.
    t_uv projUV;
    //
    mobSurf->InvertPoint(t_xyz(px, py, pz), projUV);

    // Set result.
    ures = projUV.U();
    vres = projUV.V();
  }
  else
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Non-supported surface type.");
    return TCL_ERROR;
  }

  // Evaluate surface for the obtained (u,v) coordinates.
  gp_Pnt P = occtSurface->Value(ures, vres);
  //
  interp->GetPlotter().REDRAW_POINT("proj", P, Color_Green);
  interp->GetPlotter().REDRAW_LINK("plink", gp_Pnt(px, py, pz), P, Color_Red);

  // Dump the result to the notifier.
  interp->GetProgress().SendLogMessage( LogInfo(Normal) << "Projection (u, v) = (%1, %2)."
                                                        << ures << vres );

  return TCL_OK;
#else
  (void) argc;
  (void) argv;

  interp->GetProgress().SendLogMessage(LogErr(Normal) << "Mobius is not available.");
  return TCL_ERROR;
#endif
}

//-----------------------------------------------------------------------------

int ENGINE_CheckPartContains(const Handle(asiTcl_Interp)& interp,
                             int                          argc,
                             const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node.
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();

  // Get topological item to check.
  Handle(asiData_IVTopoItemNode)
    topoItem_n = Handle(asiData_IVTopoItemNode)::DownCast( cmdEngine::model->FindNodeByName(argv[1]) );
  //
  if ( topoItem_n.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find topological object with name %1." << argv[1]);
    return TCL_ERROR;
  }

  *interp << asiAlgo_Utils::Contains( part_n->GetShape(), topoItem_n->GetShape() );
  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckPartMetadata(const Handle(asiTcl_Interp)& interp,
                             int                          argc,
                             const char**                 argv)
{
  (void) argc;
  (void) argv;

  // Get Part Node.
  Handle(asiData_PartNode) part_n = cmdEngine::model->GetPartNode();

  // Part shape.
  TopoDS_Shape partShape = part_n->GetShape();

  // Get Metadata Node.
  Handle(asiData_MetadataNode) meta_n = part_n->GetMetadata();

  // Get all shapes registered in the Metadata Node.
  asiData_MetadataAttr::t_shapeColorMap map;
  meta_n->GetShapeColorMap(map);
  //
  bool allFine = true;
  //
  for ( int k = 1; k <= map.Extent(); ++k )
  {
    const TopoDS_Shape& subshape = map.FindKey(k);
    //
    if ( !asiAlgo_Utils::Contains(partShape, subshape) )
    {
      allFine = false;
      break;
    }
  }

  *interp << allFine;
  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckSelfInter(const Handle(asiTcl_Interp)& interp,
                          int                          argc,
                          const char**                 argv)
{
  const bool isInteractive = (argc == 1);

  // Get Part Node to access the selected faces.
  Handle(asiData_PartNode) partNode = cmdEngine::model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part Node is null or ill-defined.");
    return TCL_ERROR;
  }
  Handle(asiAlgo_AAG) aag   = partNode->GetAAG();
  TopoDS_Shape        shape = partNode->GetShape();

  // Get index of the base face.
  int fid = 0;
  //
  if ( isInteractive )
  {
    TColStd_PackedMapOfInteger fids;
    //
    asiEngine_Part partAPI( cmdEngine::model, cmdEngine::cf->ViewerPart->PrsMgr() );
    partAPI.GetHighlightedFaces(fids);

    if ( fids.Extent() != 1 )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Please, select a single 'guess face'.");
      return TCL_ERROR;
    }
    //
    fid = fids.GetMinimalMapped();
  }
  else
  {
    TCollection_AsciiString argStr(argv[1]);
    //
    if ( !argStr.IsIntegerValue() )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "The passed face ID is not an integer value.");
      return TCL_ERROR;
    }

    fid = atoi(argv[1]);
    //
    if ( !partNode->GetAAG()->HasFace(fid) )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Face %1 does not exist in the working part."
                                                          << fid);
      return TCL_ERROR;
    }
  }

  // Get face in question.
  const TopoDS_Face& face = partNode->GetAAG()->GetFace(fid);

  TIMER_NEW
  TIMER_GO

  // Check domain self-intersections.
  asiAlgo_CheckValidity checker( interp->GetProgress(),
                                 interp->GetPlotter() );
  //
  *interp << checker.HasDomainSelfIntersections(face, false);

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "check-self-inter")

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_RecognizeBaseFaces(const Handle(asiTcl_Interp)& interp,
                              int                          argc,
                              const char**                 argv)
{
  if ( argc != 1 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get part.
  Handle(asiData_PartNode) partNode = cmdEngine::model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part Node is null or ill-defined.");
    return TCL_OK;
  }
  //
  TopoDS_Shape        partShape = partNode->GetShape();
  Handle(asiAlgo_AAG) partAAG   = partNode->GetAAG();

  // Loop over the AAG.
  for ( asiAlgo_AAGRandomIterator it(partAAG); it.More(); it.Next() )
  {
    const int fid = it.GetFaceId();

    // Get face to check the number of wires.
    const TopoDS_Face& face = partAAG->GetFace(fid);

    // Get the number of wires.
    TopTools_IndexedMapOfShape wires;
    TopExp::MapShapes(face, TopAbs_WIRE, wires);
    //
    const int numWires = wires.Extent();

    if ( numWires > 1 )
    {
      // Hook an attribute.
      Handle(asiAlgo_FeatureAttrBaseFace) attr = new asiAlgo_FeatureAttrBaseFace(0);
      //
      partAAG->SetNodeAttribute(fid, attr);
    }
  }

  // Prepare tool to extract features from AAG.
  asiAlgo_ExtractFeatures extractor(interp->GetProgress(), nullptr);
  extractor.RegisterFeatureType( FeatureType_UNDEFINED,
                                 asiAlgo_FeatureAttrBaseFace::GUID() );

  // Extract features.
  TColStd_PackedMapOfInteger resIndices;
  //
  Handle(asiAlgo_ExtractFeaturesResult) featureRes;
  if ( !extractor.Perform(partAAG, featureRes) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Feature extraction failed.");
    return TCL_ERROR;
  }
  //
  featureRes->GetFaceIndices(resIndices);

  // Highlight the detected faces.
  if ( !cmdEngine::cf.IsNull() && cmdEngine::cf->ViewerPart )
    asiEngine_Part( cmdEngine::model,
                    cmdEngine::cf->ViewerPart->PrsMgr() ).HighlightFaces(resIndices);

  // Dump to result.
  *interp << resIndices;

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_GetOuterWire(const Handle(asiTcl_Interp)& interp,
                        int                          argc,
                        const char**                 argv)
{
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  if ( !cmdEngine::cf->ViewerPart )
  {
    interp->GetProgress().SendLogMessage(LogWarn(Normal) << "Part viewer is not available.");
    return TCL_OK;
  }

  // Get part.
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(cmdEngine::model, part_n, part) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part is not initialized.");
    return TCL_ERROR;
  }

  // Get AAG.
  Handle(asiAlgo_AAG) aag = part_n->GetAAG();

  // Access selected faces (if any).
  TColStd_PackedMapOfInteger selected;
  asiEngine_Part( cmdEngine::model, cmdEngine::cf->ViewerPart->PrsMgr() ).GetHighlightedFaces(selected);

  // Get outer wire for each face.
  for ( TColStd_PackedMapOfInteger::Iterator fit(selected); fit.More(); fit.Next() )
  {
    const int          fid  = fit.Key();
    const TopoDS_Face& face = aag->GetFace(fid);

    TopoDS_Wire wire = BRepTools::OuterWire(face);

    if ( selected.Extent() == 1 )
      interp->GetPlotter().REDRAW_SHAPE(argv[1], wire, Color_Default, 1., true);
    else
      interp->GetPlotter().DRAW_SHAPE(wire, Color_Default, 1., true, argv[1]);
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckThickness(const Handle(asiTcl_Interp)& interp,
                          int                          argc,
                          const char**                 argv)
{
  // Get Data Model instance.
  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  // Get owner.
  Handle(ActAPI_INode) ownerNode;
  //
  ActAPI_DataObjectId ownerId;
  interp->GetKeyValue(argc, argv, "owner", ownerId);
  //
  if ( ownerId.IsEmpty() )
    ownerNode = M->GetPartNode();
  else
    ownerNode = M->FindNode(ownerId);
  //
  if ( ownerNode.IsNull() || !ownerNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Owner node is null or inconsistent.");
    return TCL_ERROR;
  }

  // Thickness analysis API.
  asiEngine_Thickness thicknessApi( M,
                                    interp->GetProgress(),
                                    interp->GetPlotter() );
  //
  Handle(asiData_ThicknessNode) TN;

  // Analyze thickness.
  M->OpenCommand();
  {
    TN = thicknessApi.CreateThickness(ownerNode);

    // Execute deps.
    M->FuncExecuteAll();
  }
  M->CommitCommand();

  // Update UI.
  if ( cmdEngine::cf )
  {
    if ( cmdEngine::cf->ObjectBrowser )
      cmdEngine::cf->ObjectBrowser->Populate();
    if ( cmdEngine::cf->ViewerPart )
      cmdEngine::cf->ViewerPart->PrsMgr()->Actualize(TN);
  }

  // Return the minimal detected wall thickness.
  (*interp) << ActParamTool::AsReal( TN->Parameter(asiData_ThicknessNode::PID_ScalarMin) )->GetValue();

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckClearance(const Handle(asiTcl_Interp)& interp,
                          int                          argc,
                          const char**                 argv)
{
  // Get Data Model instance.
  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  // Get owner.
  Handle(ActAPI_INode) ownerNode;
  //
  ActAPI_DataObjectId ownerId;
  interp->GetKeyValue(argc, argv, "owner", ownerId);
  //
  if ( ownerId.IsEmpty() )
    ownerNode = M->GetPartNode();
  else
    ownerNode = M->FindNode(ownerId);
  //
  if ( ownerNode.IsNull() || !ownerNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Owner node is null or inconsistent.");
    return TCL_ERROR;
  }

  // Clearance analysis API.
  asiEngine_Clearance clearanceApi( M,
                                    interp->GetProgress(),
                                    interp->GetPlotter() );
  //
  Handle(asiData_ClearanceNode) CN;

  // Analyze clearance.
  M->OpenCommand();
  {
    CN = clearanceApi.CreateClearance(ownerNode);

    // Execute deps.
    M->FuncExecuteAll();
  }
  M->CommitCommand();

  // Update UI.
  if ( cmdEngine::cf )
  {
    if ( cmdEngine::cf->ObjectBrowser )
      cmdEngine::cf->ObjectBrowser->Populate();
    if ( cmdEngine::cf->ViewerPart )
      cmdEngine::cf->ViewerPart->PrsMgr()->Actualize(CN);
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_FindVisibleFaces(const Handle(asiTcl_Interp)& interp,
                            int                          argc,
                            const char**                 argv)
{
  // Get part.
  Handle(asiData_PartNode) partNode = cmdEngine::model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part Node is null or ill-defined.");
    return TCL_OK;
  }
  //
  TopoDS_Shape partShape = partNode->GetShape();

  // Find visible faces.
  asiAlgo_FindVisibleFaces FindVisible( partShape,
                                        interp->GetProgress(),
                                        interp->GetPlotter() );
  //
  if ( !FindVisible.Perform() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find visible faces.");
    return TCL_ERROR;
  }

  // Get visible faces.
  TColStd_PackedMapOfInteger resIndices;
  FindVisible.GetResultFaces(resIndices, (argc > 1) ? atof(argv[1]) : 0.1 );

  // Highlight the detected faces.
  if ( !cmdEngine::cf.IsNull() && cmdEngine::cf->ViewerPart )
    asiEngine_Part( cmdEngine::model,
                    cmdEngine::cf->ViewerPart->PrsMgr() ).HighlightFaces(resIndices);

  // Dump to result.
  *interp << resIndices;

  return TCL_OK;
}
//-----------------------------------------------------------------------------

int ENGINE_FindIsomorphisms(const Handle(asiTcl_Interp)& interp,
                            int                          argc,
                            const char**                 argv)
{
  Handle(asiEngine_Model)
    model = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  /* ===================
   *  Find isomorphisms.
   * =================== */

  asiEngine_Isomorphism isomorphism( cmdEngine::model,
                                     interp->GetProgress(),
                                     interp->GetPlotter() );

  // Compose flags.
  int flags = 0;
  //
  if ( interp->HasKeyword(argc, argv, "dump") )
    flags |= asiEngine_Isomorphism::Verbose;
  if ( interp->HasKeyword(argc, argv, "noconvex") )
    flags |= asiEngine_Isomorphism::ExcludeConvexOnly;
  if ( interp->HasKeyword(argc, argv, "nobase") )
    flags |= asiEngine_Isomorphism::ExcludeBase;

  TIMER_NEW
  TIMER_GO

  // Find isomorphisms.
  TColStd_PackedMapOfInteger featureFaces;
  if ( !isomorphism.Compute(argv[1], featureFaces, flags) )
  {
    return TCL_ERROR;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Find isomorphisms")

  /* =======================
   *  Finalize computations.
   * ======================= */

  interp->GetProgress().SendLogMessage( LogInfo(Normal) << "Found %1 feature face(s)."
                                                        << featureFaces.Extent() );

  // Highlight the detected faces.
  if ( !cmdEngine::cf.IsNull() && cmdEngine::cf->ViewerPart )
    asiEngine_Part( cmdEngine::model,
                    cmdEngine::cf->ViewerPart->PrsMgr() ).HighlightFaces(featureFaces);

  // Dump to result.
  *interp << featureFaces;

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_PrintAttrs(const Handle(asiTcl_Interp)& interp,
                      int                          argc,
                      const char**                 argv)
{
  // Get part's AAG.
  Handle(asiData_PartNode)
    partNode = cmdEngine::cf->Model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part Node is null or ill-defined.");
    return TCL_OK;
  }
  //
  Handle(asiAlgo_AAG) G = partNode->GetAAG();

  // Access selected faces (if any).
  asiAlgo_Feature selected;
  //
  if ( !cmdEngine::cf.IsNull() )
  {
    asiEngine_Part( cmdEngine::cf->Model,
                    cmdEngine::cf->ViewerPart->PrsMgr() ).GetHighlightedFaces(selected);
  }

  // Get the face in question.
  int fid = 0;
  interp->GetKeyValue<int>(argc, argv, "fid", fid);
  //
  if ( fid ) selected.Add(fid);

  // Dump attributes for each face.
  TCollection_AsciiString lbl;
  //
  for ( asiAlgo_Feature::Iterator fit(selected); fit.More(); fit.Next() )
  {
    const int f = fit.Key();

    if ( !f || !G->HasFace(f) )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Face %1 looks invalid."
                                                          << f);
      return TCL_ERROR;
    }

    // Dump all nodal attributes associated with the face in question.
    if ( !G->HasNodeAttributes(f) )
      continue;
    //
    const asiAlgo_AAG::t_attr_set& attrs = G->GetNodeAttributes(f);
    //
    lbl += "\n";
    lbl += "Face ";
    lbl += f;
    //
    for ( asiAlgo_AAG::t_attr_set::Iterator ait(attrs); ait.More(); ait.Next() )
    {
      const Handle(asiAlgo_FeatureAttr)& attr = ait.GetAttr();

      lbl += "\n + ";
      lbl += attr->DynamicType()->Name();

      TCollection_AsciiString inlineDump = attr->DumpInline();

      if ( !inlineDump.IsEmpty() )
      {
        lbl += " // ";
        lbl += inlineDump;
      }
    }
  }
  //
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << lbl);

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_BuildFaceGrid(const Handle(asiTcl_Interp)& interp,
                         int                          argc,
                         const char**                 argv)
{
  // Get part's AAG.
  Handle(asiData_PartNode)
    partNode = cmdEngine::model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part Node is null or ill-defined.");
    return TCL_OK;
  }
  //
  Handle(asiAlgo_AAG) G = partNode->GetAAG();

  // Access selected faces (if any).
  asiAlgo_Feature selected;
  //
  asiEngine_Part partApi(cmdEngine::model,
                         (cmdEngine::cf && cmdEngine::cf->ViewerPart) ? cmdEngine::cf->ViewerPart->PrsMgr() : nullptr);
  if ( !cmdEngine::cf.IsNull() )
  {
    partApi.GetHighlightedFaces(selected);
  }

  int fid = 0;
  TCollection_AsciiString fidStr;
  if ( interp->GetKeyValue(argc, argv, "fid", fidStr) )
  {
    fid = fidStr.IntegerValue();
  }

  if ( fid >= 1 )
  {
    selected.Add(fid);
  }

  if ( selected.Extent() == 1 )
  {
    const TopoDS_Face& face = G->GetFace( selected.GetMinimalMapped() );

    /* Experiment with discrete classifier */

    TopoDS_Wire wire = asiAlgo_Utils::OuterWire(face);

    std::vector<gp_XY> polygon;
    asiAlgo_SampleFace::Wire2Polygon(wire, face, polygon);

    interp->GetPlotter().REDRAW_CURVE2D("polygon", asiAlgo_Utils::PolylineAsSpline(polygon), Color_Yellow);

    /* Sampling */

    int numBins = 10;
    interp->GetKeyValue(argc, argv, "num", numBins);

    /* PMC algorithm */

    const bool isHaines = interp->HasKeyword(argc, argv, "haines");
    const bool isDiscr  = interp->HasKeyword(argc, argv, "discr");

    asiAlgo_SampleFace::PmcAlgo pmcAlgo;
    //
    if ( isHaines )
      pmcAlgo = asiAlgo_SampleFace::PmcAlgo_Haines;
    else if ( isDiscr )
      pmcAlgo = asiAlgo_SampleFace::PmcAlgo_Discrete;
    else
      pmcAlgo = asiAlgo_SampleFace::PmcAlgo_Precise;

    TIMER_NEW
    TIMER_GO

    // Sample the face.
    asiAlgo_SampleFace sampleFace( face,
                                   interp->GetProgress(),
                                   interp->GetPlotter() );
    //
    sampleFace.SetSquare  ( interp->HasKeyword(argc, argv, "square") );
    sampleFace.SetPmcAlgo ( pmcAlgo );
    //
    if ( !sampleFace.Perform(numBins, numBins) )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Failed to sample the face.");
      return TCL_ERROR;
    }
    //
    const opencascade::handle< asiAlgo_UniformGrid<float, asiAlgo_FaceProbe> >& grid       = sampleFace.GetResult();
    const Handle(asiAlgo::discr::Model)&                                        discrModel = sampleFace.GetDiscrModel();

    TIMER_FINISH
    TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Build face grid")

    // Proceed with image dump.
    std::string filename;
    if ( interp->GetKeyValue(argc, argv, "filename", filename) )
    {
      vtkNew<vtkImageData> image;

      // Specify the size of the image data
      image->SetDimensions(numBins, numBins, 1);
      image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);
      int* dims = image->GetDimensions();

      // Fill the image
      for ( int i = 0; i < dims[0]; ++i )
      {
        for ( int j = 0; j < dims[1]; ++j )
        {
          unsigned char*
            pixel = static_cast<unsigned char*>( image->GetScalarPointer(i, j, 0) );
          //
          const float isIn = grid->pArray[i][j][0].s;

          pixel[0] = isIn ? 0 : 255;
          pixel[1] = isIn ? 0 : 255;
          pixel[2] = isIn ? 0 : 255;
        }
      }

      vtkNew<vtkBMPWriter> bmpWriter;
      bmpWriter->SetFileName( filename.c_str() );
      bmpWriter->SetInputData(image);
      bmpWriter->Write();
    }

    // Modify data model and actualize scene.
    Handle(asiData_Grid2dNode)    gridNode;
    Handle(asiData_DiscrFaceNode) discrFaceNode;
    //
    cmdEngine::model->OpenCommand();
    {
      gridNode = partApi.FindFaceGrid2d(true);
      gridNode->SetUniformGrid(grid);
      //
      discrFaceNode = partApi.FindDiscrFace(true);
      discrFaceNode->SetDiscrModel(discrModel);
      discrFaceNode->SetSelectedFace(selected.GetMinimalMapped());
    }
    cmdEngine::model->CommitCommand();

    if ( cmdEngine::cf && cmdEngine::cf->ObjectBrowser )
      cmdEngine::cf->ObjectBrowser->Populate(); // As new node might appear.
    if ( cmdEngine::cf && cmdEngine::cf->ViewerDomain )
      cmdEngine::cf->ViewerDomain->PrsMgr()->Actualize(gridNode);
    if ( cmdEngine::cf && cmdEngine::cf->ViewerPart )
      cmdEngine::cf->ViewerPart->PrsMgr()->Actualize(discrFaceNode);

    // Draw the sampled points in 3D.
    interp->GetPlotter().REDRAW_POINTS("grid 3D", sampleFace.GetResult3d()->GetCoordsArray(), Color_Red);
  }
  else
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Please, select exactly one face to proceed with this command.");
    return TCL_ERROR;
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_RecognizeHull(const Handle(asiTcl_Interp)& interp,
                         int                          argc,
                         const char**                 argv)
{
  /* ==============
   *  Get prepared.
   * ============== */

  // Get part's AAG.
  Handle(asiData_PartNode)
    partNode = cmdEngine::model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part Node is null or ill-defined.");
    return TCL_OK;
  }

  // Get part shape.
  TopoDS_Shape shape = partNode->GetShape();
  //
  if ( shape.IsNull() ) // Contract check.
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "The input shape is null.");
    return TCL_ERROR;
  }

  // Get AAG.
  Handle(asiAlgo_AAG) aag = partNode->GetAAG();

  // Tolerance.
  double tol = 0.1;
  interp->GetKeyValue(argc, argv, "toler", tol);

  // Number of probe points.
  int numPts = 20;
  interp->GetKeyValue(argc, argv, "num", numPts);

  // Whether to draw the visual debugging entities.
  const bool toDraw = interp->HasKeyword(argc, argv, "draw");

  // RTCD halfspaces.
  const bool isRTCD = interp->HasKeyword(argc, argv, "rtcd");

  /* =====================
   *  Perform recognition.
   * ===================== */

  asiAlgo_RecognizeConvexHull recognizer(aag,
                                         interp->GetProgress(),
                                         toDraw ? interp->GetPlotter() : nullptr);

  // Configure the recognizer.
  recognizer.SetGridResolution ( numPts );
  recognizer.SetTolerance      ( tol );
  recognizer.SetUseHaines      ( interp->HasKeyword(argc, argv, "haines") );

  TIMER_NEW
  TIMER_GO

  if ( !recognizer.Perform() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Failed to recognize convex-hull faces.");
    return TCL_ERROR;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Convex-hull recognition")

  // Access the recognized faces.
  const asiAlgo_Feature& ids = recognizer.GetResultIndices();

  // Highlight.
  if ( !cmdEngine::cf.IsNull() )
    asiEngine_Part( cmdEngine::cf->Model,
                    cmdEngine::cf->ViewerPart->PrsMgr() ).HighlightFaces(ids);

  // Return from Tcl function.
  *interp << ids;

  // Print out.
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Convex-hull faces: %1" << ids);

  // Draw hull.
  interp->GetPlotter().REDRAW_TRIANGULATION("hull", recognizer.GetHullMesh(), Color_Default, 1.0);

  if ( isRTCD )
  {
    asiAlgo_ConvexHull chull = recognizer.GetHull();
    //
    chull.ComputeHullPlanes();

    for ( const auto& plane : chull.Halfspaces )
    {
      Handle(Geom_Plane) occPlane = plane.ConvertToOpenCascade();

      // Move the anchor points.
      if ( plane.anchor.has_value() )
      {
        gp_Trsf T;
        T.SetTranslation( occPlane->Location(), plane.anchor->ConvertToOpenCascade() );
        //
        occPlane->Transform(T);
      }

      interp->GetPlotter().DRAW_SURFACE( occPlane, Color_White, "halfspace" );
    }
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckFacets(const Handle(asiTcl_Interp)& interp,
                       int                          argc,
                       const char**                 argv)
{
  (void) argc;
  (void) argv;

  Handle(asiData_PartNode)
    partNode = cmdEngine::model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part Node is null or ill-defined.");
    return TCL_OK;
  }

  // Get part shape.
  TopoDS_Shape shape = partNode->GetShape();
  //
  if ( shape.IsNull() ) // Contract check.
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "The input shape is null.");
    return TCL_ERROR;
  }

  asiAlgo_CheckValidity checker( interp->GetProgress(),
                                 interp->GetPlotter() );

  const bool isOk = checker.CheckTriangulation(shape);

  if ( !isOk )
  {
    interp->GetProgress().SendLogMessage(LogWarn(Normal) << "Facets look invalid.");
  }
  else
  {
    interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Facets look OK.");
  }

  *interp << isOk;

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckVertexVexity(const Handle(asiTcl_Interp)& interp,
                             int                          argc,
                             const char**                 argv)
{
  // Get part's AAG.
  Handle(asiData_PartNode)
    partNode = Handle(asiEngine_Model)::DownCast( interp->GetModel() )->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part Node is null or ill-defined.");
    return TCL_OK;
  }
  //
  Handle(asiAlgo_AAG) G = partNode->GetAAG();

  int fid = 0;
  const bool isFidPassed = interp->GetKeyValue(argc, argv, "fid", fid);

  // Access selected faces (if any).
  asiAlgo_Feature selected;
  //
  if ( !fid && !cmdEngine::cf.IsNull() )
  {
    asiEngine_Part partApi( cmdEngine::cf->Model,
                            cmdEngine::cf->ViewerPart->PrsMgr() );

    partApi.GetHighlightedFaces(selected);

    if ( selected.Extent() == 1 )
    {
      fid = selected.GetMinimalMapped();
    }
    else
    {
      interp->GetProgress().SendLogMessage(LogWarn(Normal) << "Please, select one face.");
      return TCL_ERROR;
    }
  }

  if ( !fid )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Face to check is not defined.");
    return TCL_ERROR;
  }

  asiAlgo_CheckVertexVexity::t_vexityMap vexity;

  // Prepare the algorithm and collect edges to check.
  asiAlgo_CheckVertexVexity algo( G,
                                  interp->GetProgress(),
                                  nullptr );
  //
  algo.CheckContours(fid, vexity);

  // Collect smooth, convex and concave vertices.
  BRep_Builder bbuilder;
  TopoDS_Compound compSmooth, compConcave, compConvex;
  //
  bbuilder.MakeCompound(compSmooth);
  bbuilder.MakeCompound(compConcave);
  bbuilder.MakeCompound(compConvex);
  //
  int numConvex = 0, numConcave = 0, numSmooth = 0;
  //
  for ( asiAlgo_CheckVertexVexity::t_vexityMap::Iterator vit(vexity);
        vit.More(); vit.Next() )
  {
    const TopoDS_Vertex&           V = vit.Key();
    const asiAlgo_FeatureAngleType X = vit.Value();

    if ( X == FeatureAngleType_Smooth )
    {
      bbuilder.Add(compSmooth, V);
      numSmooth++;
    }
    else if ( X == FeatureAngleType_Concave )
    {
      bbuilder.Add(compConcave, V);
      numConcave++;
    }
    else if ( X == FeatureAngleType_Convex )
    {
      bbuilder.Add(compConvex, V);
      numConvex++;
    }
  }

  interp->GetPlotter().REDRAW_SHAPE("smooth",  compSmooth,  Color_LightGray, 1., true);
  interp->GetPlotter().REDRAW_SHAPE("concave", compConcave, Color_Red,       1., true);
  interp->GetPlotter().REDRAW_SHAPE("convex",  compConvex,  Color_Green,     1., true);

  if ( !isFidPassed && (argc == 4) || isFidPassed && (argc == 6) )
  {
    // Set Tcl variables.
    int varIdx = isFidPassed ? 2 : 0;
    //
    interp->SetVarFundamental<int>(argv[++varIdx], numConvex);
    interp->SetVarFundamental<int>(argv[++varIdx], numConcave);
    interp->SetVarFundamental<int>(argv[++varIdx], numSmooth);
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckDistanceToBbox(const Handle(asiTcl_Interp)& interp,
                               int                          argc,
                               const char**                 argv)
{
  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  // Get part shape.
  Handle(asiData_PartNode)
    partNode = cmdEngine::cf->Model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part Node is null or ill-defined.");
    return TCL_OK;
  }
  //
  TopoDS_Shape partSh = partNode->GetShape();

  // Access selected faces (if any).
  asiAlgo_Feature selected;
  //
  if ( !cmdEngine::cf.IsNull() )
  {
    asiEngine_Part( cmdEngine::cf->Model,
                    cmdEngine::cf->ViewerPart->PrsMgr() ).GetHighlightedFaces(selected);
  }

  // Get the face in question.
  int fid = 0;
  interp->GetKeyValue<int>(argc, argv, "fid", fid);
  //
  if ( fid ) selected.Add(fid);

  // Compute AABB based on mesh.
  Bnd_Box bbox;
  asiAlgo_Utils::Bounds(partSh, true, true, bbox);
  //
  RTCD::AABB aabb(bbox);

  /* For diagnostic dump */
  t_axField                         distAxes;
  Handle(asiAlgo_BaseCloud<double>) distHits = new asiAlgo_BaseCloud<double>;
  distAxes.Alloc();

  // Iterate over all faces of interest.
  double minDist = DBL_MAX;
  //
  for ( TColStd_MapIteratorOfPackedMapOfInteger fit(selected); fit.More(); fit.Next() )
  {
    const int faceId = fit.Key();

    // Get face.
    const TopoDS_Face&
      face = TopoDS::Face( M->GetPartNode()->GetAAG()->GetMapOfFaces()(faceId) );

    // Get face points using its triangulation.
    std::vector< std::pair<int, gp_Ax1> > probes;
    //
    if ( !asiAlgo_Utils::GetFacePointsByFacets(face, 0., probes) )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot sample points on the face %1." << fid);
      return TCL_ERROR;
    }

    for ( const auto& probe : probes )
    {
      const gp_Pnt& xyz = probe.second.Location();
      const gp_Dir& N   = probe.second.Direction();

      distAxes.origins->AddElement( xyz );
      distAxes.vectors->AddElement( N.XYZ() );

      // Compose a probe point.
      RTCD::Point  p( xyz.X(), xyz.Y(), xyz.Z() );
      RTCD::Vector d( N.X(),   N.Y(),   N.Z() );

      // Test w.r.t. AABB.
      double tmin, tmax, dd = 0;
      RTCD::Point q;
      //
      if ( RTCD::IntersectRayAABB(p, d, aabb, tmin, tmax) )
      {
        double t = (tmin < 0) ? tmax : tmin; // Negative `t` is in reversed direction.
        q = p + d * t;

        distHits->AddElement(q.x, q.y, q.z);

        dd = (q - p).Modulus();

        if ( dd < minDist )
          minDist = dd;
      }
      else
      {
        interp->GetProgress().SendLogMessage(LogWarn(Normal) << "Cannot compute distance from the face %1." << fid);
        continue;
      }
    }

    TCollection_AsciiString distAxName("distAxes "), distHitName("distHit ");
    distAxName += fid;
    distHitName += fid;

    interp->GetPlotter().REDRAW_POINTS  (distAxName,  distAxes.origins->GetCoordsArray(), Color_Yellow);
    interp->GetPlotter().REDRAW_VECTORS (distAxName,  distAxes.origins->GetCoordsArray(), distAxes.vectors->GetCoordsArray(), Color_Violet);
    interp->GetPlotter().REDRAW_POINTS  (distHitName, distHits->GetCoordsArray(), Color_Red);

    interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Min distance to AABB from face %1: %2." << fid << minDist);
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckDistanceToCHull(const Handle(asiTcl_Interp)& interp,
                                int                          argc,
                                const char**                 argv)
{
  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  // Get part shape.
  Handle(asiData_PartNode)
    partNode = cmdEngine::cf->Model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part Node is null or ill-defined.");
    return TCL_OK;
  }
  //
  TopoDS_Shape partSh = partNode->GetShape();

  // Access selected faces (if any).
  asiAlgo_Feature selected;
  //
  if ( !cmdEngine::cf.IsNull() )
  {
    asiEngine_Part( cmdEngine::cf->Model,
                    cmdEngine::cf->ViewerPart->PrsMgr() ).GetHighlightedFaces(selected);
  }

  // Get the face in question.
  int fid = 0;
  interp->GetKeyValue<int>(argc, argv, "fid", fid);
  //
  if ( fid ) selected.Add(fid);

  // Read directions.
  std::vector<gp_Dir> dirs;
  int dirIdx = -1;
  //
  if ( interp->HasKeyword(argc, argv, "dir", dirIdx) )
  {
    int k = dirIdx;
    //
    while ( (k + 1 < argc) && !interp->IsKeyword(argv[++k]) )
    {
      const double dirx = atof(argv[k]);
      const double diry = atof(argv[++k]);
      const double dirz = atof(argv[++k]);

      gp_Vec vec(dirx, diry, dirz);
      //
      if ( vec.Magnitude() < gp::Resolution() )
      {
        interp->GetProgress().SendLogMessage(LogErr(Normal) << "The direction vector (%1, %2, %3) has zero modulus."
                                                            << dirx << diry << dirz);
        return TCL_ERROR;
      }

      dirs.push_back( vec.XYZ() );
    }
  }

  // Compute convex hull.
  asiAlgo_RecognizeConvexHull recCHull(partSh);
  //
  if ( !recCHull.Perform() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Failed to compute convex hull.");
    return TCL_ERROR;
  }

  asiAlgo_ConvexHull chull = recCHull.GetHull();

  // Extract halfspaces.
  chull.ComputeHullPlanes();

  /* For diagnostic dump */
  t_axField                         distAxes;
  Handle(asiAlgo_BaseCloud<double>) distHits = new asiAlgo_BaseCloud<double>;
  distAxes.Alloc();

  // Iterate over all faces of interest.
  double minDist = DBL_MAX;
  //
  for ( TColStd_MapIteratorOfPackedMapOfInteger fit(selected); fit.More(); fit.Next() )
  {
    const int faceId = fit.Key();

    // Get face.
    const TopoDS_Face&
      face = TopoDS::Face( M->GetPartNode()->GetAAG()->GetMapOfFaces()(faceId) );

    // Get face points using its triangulation.
    std::vector< std::pair<int, gp_Ax1> > probes;
    //
    if ( !asiAlgo_Utils::GetFacePointsByFacets(face, 0., probes) )
    {
      interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot sample points on the face %1." << fid);
      return TCL_ERROR;
    }

    for ( auto& probe : probes )
    {
      if ( !dirs.empty() )
        probe.second = gp_Ax1( probe.second.Location(), dirs[0] );

      const gp_Pnt& xyz = probe.second.Location();
      const gp_Dir& N   = probe.second.Direction();

      distAxes.origins->AddElement( xyz );
      distAxes.vectors->AddElement( N.XYZ() );

      // Compose a probe point.
      RTCD::Point  p( xyz.X(), xyz.Y(), xyz.Z() );
      RTCD::Vector d( N.X(),   N.Y(),   N.Z() );

      // Test w.r.t. convex hull.
      double tmin, tmax, dd = 0;
      RTCD::Point q;
      //
      if ( RTCD::IntersectRayPolyhedron(p, d, chull.Halfspaces, tmin, tmax) )
      {
        double t = tmax;
        q = p + d * t;

        distHits->AddElement(q.x, q.y, q.z);

        dd = (q - p).Modulus();

        if ( dd < minDist )
          minDist = dd;
      }
      else
      {
        interp->GetProgress().SendLogMessage(LogWarn(Normal) << "Cannot compute distance from the face %1." << fid);
        continue;
      }
    }

    TCollection_AsciiString distAxName("distAxes "), distHitName("distHit ");
    distAxName += fid;
    distHitName += fid;

    interp->GetPlotter().REDRAW_POINTS  (distAxName,  distAxes.origins->GetCoordsArray(), Color_Yellow);
    interp->GetPlotter().REDRAW_VECTORS (distAxName,  distAxes.origins->GetCoordsArray(), distAxes.vectors->GetCoordsArray(), Color_Violet);
    interp->GetPlotter().REDRAW_POINTS  (distHitName, distHits->GetCoordsArray(), Color_Red);

    interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Min distance to polyhedron from face %1: %2." << fid << minDist);
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_CheckCanonical(const Handle(asiTcl_Interp)& interp,
                          int                          argc,
                          const char**                 argv)
{
  // Get part.
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(cmdEngine::cf->Model, part_n, part) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part is not initialized.");
    return TCL_ERROR;
  }

  // Get AAG.
  Handle(asiAlgo_AAG) aag = part_n->GetAAG();

  // Access selected faces (if any).
  TColStd_PackedMapOfInteger selected;
  //
  asiEngine_Part( cmdEngine::cf->Model,
                  cmdEngine::cf->ViewerPart->PrsMgr() ).GetHighlightedFaces(selected);
  //
  if ( selected.IsEmpty() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Please, select a face to check.");
    return TCL_ERROR;
  }

  // Recognition tolerance.
  double toler = 1.e-3;
  interp->GetKeyValue<double>(argc, argv, "toler", toler);

  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Canonical recognition tolerance: %1."
                                                       << toler);

  // Check the selected faces.
  for ( TColStd_PackedMapOfInteger::Iterator fit(selected); fit.More(); fit.Next() )
  {
    const int          fid  = fit.Key();
    const TopoDS_Face& face = aag->GetFace(fid);

    // Get the host surface.
    Handle(Geom_Surface) surface = BRep_Tool::Surface(face);

    // Check canonical.
    Handle(Standard_Type)
      type = asiAlgo_RecognizeCanonical::CheckType( face, toler,
                                                    interp->GetProgress(),
                                                    interp->GetPlotter() );
    //
    if ( !type.IsNull() )
    {
      interp->GetProgress().SendLogMessage( LogInfo(Normal) << "The actual type of the face %1 is %2."
                                                            << fid << surface->DynamicType()->Name() );
      //
      interp->GetProgress().SendLogMessage( LogNotice(Normal) << "The recognized type for the face %1 is %2."
                                                              << fid << type->Name() );
    }
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_NegativeVolume(const Handle(asiTcl_Interp)& interp,
                          int                          argc,
                          const char**                 argv)
{
  // Get the part's AAG.
  Handle(asiData_PartNode)
    partNode = cmdEngine::model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part Node is null or ill-defined.");
    return TCL_ERROR;
  }
  //
  Handle(asiAlgo_AAG) aag = partNode->GetAAG();

  // Access the selected faces (if any and if UI facilities are available).
  asiAlgo_Feature selected;
  //
  if ( interp->HasKeyword(argc, argv, "fids") )
  {
    std::vector<int> values;
    if ( interp->CollectValues(argc, argv, "fids", values) )
    {
      std::vector<int>::const_iterator itV = values.cbegin();
      for ( ; itV != values.cend(); ++itV )
      {
        selected.Add(*itV);
      }
    }
  }
  else if ( !cmdEngine::cf.IsNull() )
  {
    Handle(asiUI_IV) iv = Handle(asiUI_IV)::DownCast( interp->GetPlotter().Access() );

    if ( !iv.IsNull() )
    {
      asiEngine_Part( iv->GetModel(),
                      iv->GetPrsMgr3d() ).GetHighlightedFaces(selected);
    }
  }

  if ( selected.IsEmpty() )
  {
    interp->GetProgress().SendLogMessage(LogNotice(Normal) << "No face selected.");
    return TCL_ERROR;
  }
  else
  {
    interp->GetProgress().SendLogMessage(LogNotice(Normal) << "The accessibility check will be done "
                                                              "for the face(s) [%1]." << selected);
  }

  const bool isOneSolid = interp->HasKeyword(argc, argv, "oneSolid");

  TIMER_NEW
    TIMER_GO

  ComputeNegativeVolumeAlgo algo(aag, selected, interp->GetProgress(), interp->GetPlotter(), isOneSolid);

  if (!algo.Perform())
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Volume calculation failed.");
    return TCL_ERROR;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Computes negative volumes")

  std::vector<std::tuple<asiAlgo_Feature,
                         TopoDS_Shape,
                         double>> negativeVolumes = algo.GetNegativeVolumes();

  for ( int index = 0; index < (int) (negativeVolumes.size()); ++index )
  {
    TopoDS_Compound featureComp;
    BRep_Builder bb;
    bb.MakeCompound(featureComp);
    asiAlgo_Feature featureFaceIds = std::get<0>(negativeVolumes[index]);
    asiAlgo_Feature::Iterator itFFIds(featureFaceIds);
    for ( ; itFFIds.More(); itFFIds.Next() )
    {
      bb.Add(featureComp, aag->GetFace(itFFIds.Key()));
    }

    if ( !(std::get<1>(negativeVolumes[index])).IsNull() && !featureComp.IsNull())
    {
      interp->GetProgress().SendLogMessage(LogNotice(Normal) << "Volume = %1 " << std::get<2>(negativeVolumes[index]));
      interp->GetPlotter().DRAW_SHAPE(std::get<1>(negativeVolumes[index]), "negativeVolumeShape");
      interp->GetPlotter().DRAW_SHAPE(featureComp, "feature");
    }
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

int ENGINE_GetDominatingPlane(const Handle(asiTcl_Interp)& interp,
                              int                          argc,
                              const char**                 argv)
{
  if ( argc != 4 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get Part Node and shape.
  Handle(asiData_PartNode) partNode = cmdEngine::model->GetPartNode();
  //
  if ( partNode.IsNull() || !partNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part is not initialized.");
    return TCL_OK;
  }
  //
  Handle(asiAlgo_AAG) G = partNode->GetAAG();

  // Iterate over all faces.
  int    resFid  = 0;
  double maxArea = 0;
  gp_Dir norm    = gp::DZ();
  //
  for( asiAlgo_AAGRandomIterator it(G); it.More(); it.Next() )
  {
    const int          fid  = it.GetFaceId();
    const TopoDS_Face& face = G->GetFace(fid);
    Handle(Geom_Plane) pln;

    if ( asiAlgo_Utils::IsPlanar(face, pln) )
    {
      const double faceArea = asiAlgo_Utils::CacheFaceArea(fid, G);
      //
      if ( faceArea > maxArea )
      {
        resFid  = fid;
        maxArea = faceArea;
        norm    = pln->Axis().Direction();
      }
    }
  }

  // Set Tcl variables.
  int varIdx = 0;
  //
  interp->SetVarFundamental<int>( argv[++varIdx], norm.X() );
  interp->SetVarFundamental<int>( argv[++varIdx], norm.Y() );
  interp->SetVarFundamental<int>( argv[++varIdx], norm.Z() );

  return TCL_OK;
}

//-----------------------------------------------------------------------------

void cmdEngine::Commands_Inspection(const Handle(asiTcl_Interp)&      interp,
                                    const Handle(Standard_Transient)& cmdEngine_NotUsed(data))
{
  static const char* group = "cmdEngine";

  //-------------------------------------------------------------------------//
  interp->AddCommand("explode",
    //
    "explode [{-vertex | -edge | -wire | -face | -shell | -solid | -noloc}]\n"
    "\t Explodes the active part to subshapes of interest. If no subshape\n"
    "\t qualifier is passed, this command explodes the part to its direct\n"
    "\t children (e.g. edges for wire, wires for face, etc.). If the '-noloc'\n"
    "\t flag is passed, locations are not accumulated on explode.",
    //
    __FILE__, group, ENGINE_Explode);

  //-------------------------------------------------------------------------//
  interp->AddCommand("print-summary",
    //
    "print-summary\n"
    "\t Prints summary (number of sub-shapes) for the active part.",
    //
    __FILE__, group, ENGINE_PrintSummary);

  //-------------------------------------------------------------------------//
  interp->AddCommand("get-summary",
    //
    "get-summary <compounds> <compsolids> <solids> <shells> <faces> <wires> <edges> <vertices>\n"
    "\t Returns summary (number of sub-shapes) to the specified output variables.",
    //
    __FILE__, group, ENGINE_GetSummary);

  //-------------------------------------------------------------------------//
  interp->AddCommand("print-summary-geom",
    //
    "print-summary-geom\n"
    "\t Prints the summary of geometric entities for the active part.",
    //
    __FILE__, group, ENGINE_PrintSummaryGeom);

  //-------------------------------------------------------------------------//
  interp->AddCommand("get-summary-geom",
    //
    "get-summary-geom <nbSurfBezier>   <nbSurfSpl>        <nbSurfConical> "
    "                 <nbSurfCyl>      <nbSurfOffset>     <nbSurfSph>     "
    "                 <nbSurfLinExtr>  <nbSurfOfRevol>    <nbSurfToroidal>"
    "                 <nbSurfPlane>                                       "
    "                 <nbCurveBezier>  <nbCurveSpline>    <nbCurveCircle> "
    "                 <nbCurveEllipse> <nbCurveHyperbola> <nbCurveLine>   "
    "                 <nbCurveOffset>  <nbCurveParabola>                  "
    "\n"
    "\t Returns the summary of geometric entities to the specified output variables.\n"
    "\t The rectangular trimmed surfaces are not accounted directly and rather inspected\n"
    "\t deeper for their basic surfaces. The same applies to the trimmed curves. This\n"
    "\t function helps to identify how canonical the input geometry is.",
    //
    __FILE__, group, ENGINE_GetSummaryGeom);

  //-------------------------------------------------------------------------//
  interp->AddCommand("face-addr",
    //
    "face-addr <faceIndex>\n"
    "\t Prints physical address of the given face.",
    //
    __FILE__, group, ENGINE_FaceAddr);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-dist",
    //
    "check-dist <varName>\n"
    "\t Computes distance between the part and the given topological object.",
    //
    __FILE__, group, ENGINE_CheckDist);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-curvature",
    //
    "check-curvature [<numPts> [<scaleFactor> [<curvAmpl>]]] [-noplot] [-noalong]\n"
    "\n"
    "\t Checks curvature of the selected edge. As a result, curvature combs\n"
    "\t are visualized in 3D. You can control its scale factor with\n"
    "\t <scaleFactor> argument and also its density with <numPts> argument.\n"
    "\t To bring out the salient features of the comb, <curvAmpl> amplification\n"
    "\t factor can be used.\n"
    "\n"
    "\t If the '-noplot' key is passed, the curvature plot is not\n"
    "\t constructed. If the '-noalong' key is passed, the along-curvature value for\n"
    "\t the selected edges is not computed.",
    //
    __FILE__, group, ENGINE_CheckCurvature);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-continuity",
    //
    "check-continuity\n"
    "\t Checks continuity of the selected face.",
    //
    __FILE__, group, ENGINE_CheckContinuity);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-euler",
    //
    "check-euler [<genus>]\n"
    "\t Opens dialog to check Euler-Poincare property of the active part.\n"
    "\t If <genus> parameter is not specified, this command will open a prompt\n"
    "\t dialog to ask the user to type genus.",
    //
    __FILE__, group, ENGINE_CheckEuler);

  //-------------------------------------------------------------------------//
  interp->AddCommand("dump-topograph-dot",
    //
    "dump-topograph-dot <filename>\n"
    "\t Dumps topology graph to DOT file (can be opened by Graphviz).",
    //
    __FILE__, group, ENGINE_DumpTopographDot);

  //-------------------------------------------------------------------------//
  interp->AddCommand("eval-curve",
    //
    "eval-curve <curveName> <u> <order> [-mobius]\n"
    "\t Evaluates curve <curveName> for the given parameter value <u>.\n"
    "\t If the '-mobius' keyword is used, evaluation is performed using Mobius\n"
    "\t functions. The argument <order> specifies the order of derivatives\n"
    "\t to evaluate (0 for value evaluation).",
    //
    __FILE__, group, ENGINE_EvalCurve);

  //-------------------------------------------------------------------------//
  interp->AddCommand("eval-surf",
    //
    "eval-surf <surfName> <u> <v> <order> [-mobius]\n"
    "\t Evaluates surface <surfName> for the given parameter pair <u, v>.\n"
    "\t If the '-mobius' keyword is used, evaluation is performed using Mobius\n"
    "\t functions. The argument <order> specifies the order of derivatives\n"
    "\t to evaluate (0 for value evaluation).",
    //
    __FILE__, group, ENGINE_EvalSurf);

  //-------------------------------------------------------------------------//
  interp->AddCommand("eval-iso-u",
    //
    "eval-iso-u <surfName> <u> [-v <v>]\n"
    "\t Evaluates an isoparametric curve `Cu(t)` on the surface <surfName>.\n"
    "\t The optional parameter <v> can be passed to evaluate `Cu(v)`.",
    //
    __FILE__, group, ENGINE_EvalIsoU);

  //-------------------------------------------------------------------------//
  interp->AddCommand("eval-iso-v",
    //
    "eval-iso-v <surfName> <v> [-u <u>]\n"
    "\t Evaluates an isoparametric curve `Cv(t)` on the surface <surfName>.\n"
    "\t The optional parameter <u> can be passed to evaluate `Cv(u)`.",
    //
    __FILE__, group, ENGINE_EvalIsoV);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-toler",
    //
    "check-toler [<numRanges>]\n"
    "\t Checks local tolerances in the part shape and distributes sub-shapes\n"
    "\t by distinct tolerance ranges. You can control the fineness of these tolerance ranges\n"
    "\t using the <numRanges> optional argument (10 by default).",
    //
    __FILE__, group, ENGINE_CheckToler);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-length",
    //
    "check-length\n"
    "\t Checks length of the selected edges.",
    //
    __FILE__, group, ENGINE_CheckLength);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-area",
    //
    "check-area  [-fid <faceId>]\n"
    "\t Checks area of the passed or selected faces.",
    //
    __FILE__, group, ENGINE_CheckArea);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-volume",
    //
    "check-volume\n"
    "\t Checks volume of the active part.",
    //
    __FILE__, group, ENGINE_CheckVolume);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-aabb",
    //
    "check-aabb [-opt] [-tris]\n"
    "\t Checks axis-aligned bounding box (AABB) of the active part. If the '-opt'\n"
    "\t key is passed, the bounding box will be optimized to better fit the\n"
    "\t shape. This option can be computationally heavy though as it implies\n"
    "\t numerical optimization.\n"
    "\n"
    "\t If the '-tris' key is passed, the algorithm will\n"
    "\t use triangulation (facets) of the part to compute its AABB.",
    //
    __FILE__, group, ENGINE_CheckAABB);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-validity",
    //
    "check-validity\n"
    "\t Checks validity of the part shape.",
    //
    __FILE__, group, ENGINE_CheckValidity);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-finite",
    //
    "check-finite\n"
    "\t Checks finiteness of the part shape.",
    //
    __FILE__, group, ENGINE_CheckFinite);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-contours",
    //
    "check-contours [<tolerance>]\n"
    "\t Checks whether all contours of the part shape are closed or not.",
    //
    __FILE__, group, ENGINE_CheckContours);

  //-------------------------------------------------------------------------//
  interp->AddCommand("get-tolerance",
    //
    "get-tolerance\n"
    "\t Returns max geometric tolerance of the part shape.",
    //
    __FILE__, group, ENGINE_GetTolerance);

  //-------------------------------------------------------------------------//
  interp->AddCommand("get-curve-strain",
    //
    "get-curve-strain <curveName>\n"
    "\t Returns strain energy of the passed curve.",
    //
    __FILE__, group, ENGINE_GetCurveStrain);

  //-------------------------------------------------------------------------//
  interp->AddCommand("get-surface-bending",
    //
    "get-surface-bending <surfName>\n"
    "\t Computes bending energy of the passed surface.",
    //
    __FILE__, group, ENGINE_GetSurfaceBending);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-edge-vexity",
    //
    "check-edge-vexity {<edgeIndex> | -name <edgeName>}\n"
    "\t Analyzes convexity/concavity of the given edge.",
    //
    __FILE__, group, ENGINE_CheckEdgeVexity);

  //-------------------------------------------------------------------------//
  interp->AddCommand("draw-cp",
    //
    "draw-cp <surfName> <i_index> <j_index> [-mobius]\n"
    "\t Draws control point of a free-form surface.",
    //
    __FILE__, group, ENGINE_DrawCP);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-open-edges",
    //
    "check-open-edges\n"
    "\t Checks part for open edges.",
    //
    __FILE__, group, ENGINE_CheckOpenEdges);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-vertices-ori",
    //
    "check-vertices-ori\n"
    "\t Checks if the vertices of the part are distinguishable by their orientation\n"
    "\t flags. A valid edge should have one FORWARD vertex and one REVERSED\n"
    "\t vertex to denote its topological extremities. As a result of some\n"
    "\t improperly implemented modeling operators, this rule can be broken.",
    //
    __FILE__, group, ENGINE_CheckVerticesOri);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-internal-locations",
    //
    "check-internal-locations [-f]\n"
    "\t Checks if the part contains any internal non-identity locations.\n"
    "\t If the '-f' key is passed, the located faces will be searched for.",
    //
    __FILE__, group, ENGINE_CheckInternalLocations);

  //-------------------------------------------------------------------------//
  interp->AddCommand("recognize-blends",
    //
    "recognize-blends [-radius <r>] [-fid <id>] [{-ebf | -vbf}] [-store [-cdev <perc>]]\n"
    "\n"
    "\t Recognizes all blend faces in the active part. The optional\n"
    "\t '-fid' key allows to specify the face ID to start recognition from.\n"
    "\t The optional '-radius' key allows to limit the recognized radius.\n"
    "\t The optional '-ebf|-vbf' keys allow finding the blend faces of\n"
    "\t a certain type (EBF = edge-blend face, VBF = vertex-blend face).\n"
    "\n"
    "\t If the '-store' key is passed, the recognized blend chains will be stored\n"
    "\t as series of features under the Part Node. If this flag is supplemented\n"
    "\t with '-cdev' (chain deviation) flag, then the following <perc> value is used\n"
    "\t to regroup the fillet faces into the chains of equal radii.",
    //
    __FILE__, group, ENGINE_RecognizeBlends);

  //-------------------------------------------------------------------------//
  interp->AddCommand("recognize-holes",
    //
    "recognize-holes [-radius <r>]\n"
    "\t Recognizes drilled holes.",
    //
    __FILE__, group, ENGINE_RecognizeHoles);

  //-------------------------------------------------------------------------//
  interp->AddCommand("recognize-cavities",
    //
    "recognize-cavities [-size <maxSize>]\n"
    "\t Recognizes arbitrary cavities in the active part. If the '-size'\n"
    "\t key is passed, it allows to limit the size of a cavity feature\n"
    "\t being recognized. The size is computed simply as the max dimension of\n"
    "\t the axis-aligned bounding box (AABB) of all feature faces. If the\n"
    "\t '-size' key is not passed or the <maxSize> value is zero, then no\n"
    "\t constraint on the feature size is imposed.\n\n"
    ""
    "\t If the '-details' flag is passed, the result of recognition is\n"
    "\t printed to the logger in a more detailed way.",
    //
    __FILE__, group, ENGINE_RecognizeCavities);

  //-------------------------------------------------------------------------//
  interp->AddCommand("draw-plot",
    //
    "draw-plot <x1> <f1_1> [<f2_1> ...] <x2> <f1_2> [<f2_2> ...] [...] [-log] [-numFunc <numFunc>]\n"
    "\t Draws two-dimensional plot of the given values.\n"
    "\t If the <numFunc> value is passed with the '-numFunc' key, then\n"
    "\t several functions can be plotted at once.",
    //
    __FILE__, group, ENGINE_DrawPlot);

  //-------------------------------------------------------------------------//
  interp->AddCommand("show-aag",
    //
    "show-aag [-remove-sel] [-collapse-sel] [-add-vertex-adj] [-filename <json-filename>]\n"
    "\t Visualizes AAG for the active part. If the '-remove-sel' flag is passed,\n"
    "\t the selected faces will be excluded from the AAG with all their incident\n"
    "\t arcs. If the '-collapse-sel' flag is passed, the AAG nodes of the selected\n"
    "\t faces will be collapsed while all neighbor relations will be transmitted\n"
    "\t to the surrounding nodes of the collapsed face.\n"
    "\n"
    "\t If the '-add-vertex-adj' flag is passed, vertex-adjacency relations are\n"
    "\t added as AAG arcs for the faces having common vertices but no common edges.\n"
    "\n"
    "\t Use the '-filename' option followed by a JSON filename to dump the visualized\n"
    "\t AAG with all applied modifiers (collapse, delete, added vertex-adjacency arcs)\n"
    "\t to a JSON file.",
    //
    __FILE__, group, ENGINE_ShowAAG);

  //-------------------------------------------------------------------------//
  interp->AddCommand("isolate-real-parts",
    //
    "isolate-real-parts\n"
    "\t Takes out all non-located entities.",
    //
    __FILE__, group, ENGINE_IsolateRealParts);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-along-curvature",
    //
    "check-along-curvature\n"
    "\t Checks on-surface curvature for the selected edge.",
    //
    __FILE__, group, ENGINE_CheckAlongCurvature);

  //-------------------------------------------------------------------------//
  interp->AddCommand("invert-point-surf",
    //
    "invert-point-surf <surf> <x> <y> <z>\n"
    "\t Inverts (projects) the passed point onto the given surface.",
    //
    __FILE__, group, ENGINE_InvertPointSurf);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-part-contains",
    //
    "check-part-contains <shapeName>\n"
    "\t Checks whether the part contains the shape <shapeName> as its subshape.",
    //
    __FILE__, group, ENGINE_CheckPartContains);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-part-metadata",
    //
    "check-part-metadata\n"
    "\t Checks whether the part contains all shapes registered in the Metadata Node as its subshapes.",
    //
    __FILE__, group, ENGINE_CheckPartMetadata);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-self-inter",
    //
    "check-self-inter [<fid>]\n"
    "\t Checks the parametric domain of the passed/selected face for\n"
    "\t self-intersections.",
    //
    __FILE__, group, ENGINE_CheckSelfInter);

  //-------------------------------------------------------------------------//
  interp->AddCommand("recognize-base-faces",
    //
    "recognize-base-faces\n"
    "\t Recognizes all base faces, i.e., the faces where features may exist.",
    //
    __FILE__, group, ENGINE_RecognizeBaseFaces);

  //-------------------------------------------------------------------------//
  interp->AddCommand("get-outer-wire",
    //
    "get-outer-wire <res>\n"
    "\t Extracts outer wire of the selected face.",
    //
    __FILE__, group, ENGINE_GetOuterWire);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-thickness",
    //
    "check-thickness [-owner <ownerId>]\n"
    "\t Checks the thickness distribution over the passed owner shape or mesh.\n"
    "\t Returns the minimal detected wall thickness.",
    //
    __FILE__, group, ENGINE_CheckThickness);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-clearance",
    //
    "check-clearance [-owner <ownerId>]\n"
    "\t Checks the clearance distribution over the passed owner shape.",
    //
    __FILE__, group, ENGINE_CheckClearance);

  //-------------------------------------------------------------------------//
  interp->AddCommand("find-visible-faces",
    //
    "find-visible-faces\n"
    "\t Finds visible faces.",
    //
    __FILE__, group, ENGINE_FindVisibleFaces);

  //-------------------------------------------------------------------------//
  interp->AddCommand("find-isomorphisms",
    //
    "find-isomorphisms <varShape>\n"
    "\t Solves subgraph isomorphism problem for the part shape\n"
    "\t and the passed feature descriptor encoded by <varShape>.",
    //
    __FILE__, group, ENGINE_FindIsomorphisms);

  //-------------------------------------------------------------------------//
  interp->AddCommand("print-attrs",
    //
    "print-attrs [-fid <faceId>]\n"
    "\t Prints all AAG attributes available for the face with the given ID\n"
    "\t or for the interactively selected faces.",
    //
    __FILE__, group, ENGINE_PrintAttrs);

  //-------------------------------------------------------------------------//
  interp->AddCommand("build-face-grid",
    //
    "build-face-grid [-num <numBins>] [-filename <filename>] [-fid <faceID>] [-square] [-haines|-discr]\n"
    "\n"
    "\t Builds a uniform UV grid for the interactively selected face.\n"
    "\t Pass the number of bins to control how fine sampling is going to be.\n"
    "\t If the filename is passed, the sampled face is converted to vtkImageData\n"
    "\t and dumped as a bitmap image. Pass the '-square' keyword to force the\n"
    "\t decomposition domain be of a squared shape.\n"
    "\n"
    "\t For efficient computation, pass the '-discr' keyword. This option turns the\n"
    "\t face of interest into a discrete representation and uses extremely fast\n"
    "\t two-dimensional classifier.",
    //
    __FILE__, group, ENGINE_BuildFaceGrid);

  //-------------------------------------------------------------------------//
  interp->AddCommand("recognize-hull",
    //
    "recognize-hull [-toler <tol>] [-num <numPts>] [-draw] [-rtcd]\n"
    "\t Builds convex hull of the workpiece and selects all faces lying on it.\n"
    "\t Pass tolerance with the '-toler' keyword to specify the precision of PMC\n"
    "\t over the boundary. Use the '-num' keyword to specify the number of sampling\n"
    "\t points for the faces. Pass the '-draw' keyword to dump visual debugging\n"
    "\t entities, such as the convex hull, BVH, projection points, etc. Pass the\n"
    "\t '-rtcd' flag to draw halfspace planes",
    //
    __FILE__, group, ENGINE_RecognizeHull);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-facets",
    //
    "check-facets\n"
    "\t Checks triangulation distributed by CAD faces. Returns 0 (zero) to the"
    "\t Tcl interpreter if the facets are broken and 1 if the facets"
    "\t are good.",
    //
    __FILE__, group, ENGINE_CheckFacets);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-vertex-vexity",
    //
    "check-vertex-vexity [-fid <fid>] [<numConvex> <numConcave> <numSmooth>]\n"
    "\t Checks convexity of all vertices in the given face. You can pass the\n"
    "\t optional variable names <numConvex>, <numConcave> and <numSmooth> to get\n"
    "\t the corresponding quantities of convex, concave and smooth vertices in the\n"
    "\t face of interest.",
    //
    __FILE__, group, ENGINE_CheckVertexVexity);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-distance-to-bbox",
    //
    "check-distance-to-bbox [-fid <fid>]\n"
    "\t Checks distance from the selected (or specified) face to AABB.",
    //
    __FILE__, group, ENGINE_CheckDistanceToBbox);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-distance-to-chull",
    //
    "check-distance-to-chull [-fid <fid>] [-dir <x> <y> <z>]\n"
    "\t Checks distance from the selected (or specified) face to convex hull.",
    //
    __FILE__, group, ENGINE_CheckDistanceToCHull);

  //-------------------------------------------------------------------------//
  interp->AddCommand("check-canonical",
    //
    "check-canonical [-toler <tolerance>]\n"
    "\t Checks if the selected face is of canonical type.",
    //
    __FILE__, group, ENGINE_CheckCanonical);

  //-------------------------------------------------------------------------//
  interp->AddCommand("compute-negative-volume",
    //
    "compute-negative-volume [-oneSolid] [-fids <fid1> <fid2> ...]\n"
    "\t Computes negative volumes.",
    //
    __FILE__, group, ENGINE_NegativeVolume);

  //-------------------------------------------------------------------------//
  interp->AddCommand("get-dominating-plane",
    //
    "get-dominating-plane <nx> <ny> <nz> [-orient]\n"
    "\t Attempts to find the largest planar face in the model and returns\n"
    "\t the components of its normal vector.",
    //
    __FILE__, group, ENGINE_GetDominatingPlane);
}
