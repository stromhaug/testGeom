//-----------------------------------------------------------------------------
// Created on: 05 July 2021
//-----------------------------------------------------------------------------
// Copyright (c) 2021-present, Julia Slyadneva
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
#include <asiAsm_GLTFXdeDataSourceProvider.h>

// GlTF includes
#include <asiAsm_GLTFMaterialMap.h>
#include <asiAsm_GLTFFacePropertyExtractor.h>
#include <asiAsm_GLTFCSysConverter.h>
#include <asiAsm_GLTFEntities.h>
#include <asiAsm_GLTFXdeVisualStyle.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// OpenCascade includes
#include <NCollection_DataMap.hxx>
#include <Poly_Triangulation.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFPrs.hxx>
#include <XCAFPrs_IndexedDataMapOfShapeStyle.hxx>
#include <XCAFPrs_DocumentExplorer.hxx>
#include <TColStd_MapOfAsciiString.hxx>
#include <TopoDS_Edge.hxx>

using namespace asiAsm::xde;

//! Reads name attribute.
static TCollection_AsciiString readNameAttribute(const Handle(XCAFDoc_ShapeTool)& ST,
                                                 const TDF_Label&                 refLabel,
                                                 const bool                       usePrototypeNames = false)
{
  TDF_Label lab;

  if (usePrototypeNames)
  {
    if (ST->IsReference(refLabel))
    {
      ST->GetReferredShape(refLabel, lab);
    }
    else
    {
      lab = refLabel;
    }
  }
  else
  {
    lab = refLabel;
  }

  Handle(TDataStd_Name) nodeName;
  //
  if (!lab.FindAttribute(TDataStd_Name::GetID(), nodeName))
  {
    return TCollection_AsciiString();
  }
  return TCollection_AsciiString(nodeName->Get());
}

//-----------------------------------------------------------------------------

glTFXdeDataSourceProvider::glTFXdeDataSourceProvider(const Handle(TDocStd_Document)& doc,
                                                       const TColStd_MapOfAsciiString& filter)
  : m_doc(doc),
    m_filter(filter),
    m_sceneStructure(),
    m_meshes()
{
}

//-----------------------------------------------------------------------------

glTFXdeDataSourceProvider::~glTFXdeDataSourceProvider()
{
}

//-----------------------------------------------------------------------------

void glTFXdeDataSourceProvider::Process(ActAPI_ProgressEntry progress)
{
  t_Node2Label solids;
  createSceneStructure(solids, progress);
  processSceneMeshes(solids, progress);
}

//-----------------------------------------------------------------------------

void glTFXdeDataSourceProvider::createSceneStructure(t_Node2Label&        solids,
                                                            ActAPI_ProgressEntry progress)
{
  m_sceneStructure.Clear();

  TDF_LabelSequence rootLabs;
  Handle(XCAFDoc_ShapeTool) shapeTool = XCAFDoc_DocumentTool::ShapeTool(m_doc->Main());
  shapeTool->GetFreeShapes(rootLabs);

  // Prepare full indexed map of scene nodes in correct order.
  gltf_SceneNodeMap scNodeMapWithChildren; // indexes starting from 1

  NCollection_DataMap<TDF_Label, glTFNode*, TDF_LabelMapHasher> label2Node;
  //
  XCAFPrs_DocumentExplorer docExp(m_doc, rootLabs, XCAFPrs_DocumentExplorerFlags_None);
  for (; docExp.More(); docExp.Next())
  {
    const XCAFPrs_DocumentNode& docNode = docExp.Current();
    glTFNode* node = m_sceneStructure.PrependNode();

    if (!label2Node.IsBound(docNode.Label))
      label2Node.Bind(docNode.Label, node);

    if (docExp.CurrentDepth() == 0)
      m_sceneStructure.MarkNodeAsRoot(node);

    //
    scNodeMapWithChildren.Add(docNode);
  }

  for (gltf_SceneNodeMap::Iterator snIt(scNodeMapWithChildren); snIt.More(); snIt.Next())
  {
    const XCAFPrs_DocumentNode& docNode = snIt.Value();
    glTFNode* node = label2Node.Find(docNode.Label);
    if (!node)
      continue;

    node->Name = readNameAttribute(shapeTool, docNode.Label);
    if (node->Name.IsEmpty())
    {
      node->Name = readNameAttribute(shapeTool, docNode.RefLabel);
    }

    if (docNode.IsAssembly)
    {
      for (TDF_ChildIterator childIt(docNode.RefLabel); childIt.More(); childIt.Next())
      {
        const TDF_Label& childLab = childIt.Value();
        if (childLab.IsNull())
          continue;

        glTFNode* childNode = label2Node.Find(childLab);
        if (childNode)
          node->Children.push_back(childNode);
      }
    }
    if (!docNode.LocalTrsf.IsIdentity())
    {
      node->Trsf = docNode.LocalTrsf.Transformation();
    }
    if (!docNode.IsAssembly)
    {
      // Mesh order of current node is equal to order of this node in scene nodes map
      int meshIdx = scNodeMapWithChildren.FindIndex(docNode.Id);
      if (meshIdx > 0)
      {
        node->MeshIndex = meshIdx - 1;
      }
      solids.Add( node, docNode.RefLabel );
    }
  }
}

//-----------------------------------------------------------------------------

void glTFXdeDataSourceProvider::processSceneMeshes(t_Node2Label&         solids,
                                                          ActAPI_ProgressEntry  progress)
{
  m_meshes.Clear();

  Handle(XCAFDoc_ShapeTool) shapeTool = XCAFDoc_DocumentTool::ShapeTool(m_doc->Main());
  NCollection_DataMap<TopoDS_Shape, glTFXdeVisualStyle, TopTools_ShapeMapHasher> styles;

  t_Node2Label::Iterator itM(solids);
  for (; itM.More(); itM.Next())
  {
    readStyles(itM.Value(), styles);

    TopoDS_Shape shape;

    // If there's no shape in the XDE for the host label, we do nothing.
    if (!XCAFDoc_ShapeTool::GetShape(itM.Value(), shape) || shape.IsNull())
      continue;

    //* Gather 'face' primitives
    TopExp_Explorer expl(shape, TopAbs_FACE);
    for (; expl.More(); expl.Next())
    {
      glTFFacePropertyExtractor faceProperty(TopoDS::Face(expl.Current()));
      if (faceProperty.IsEmptyMesh())
      {
        // glTF does not permit empty meshes / primitive arrays.
        progress.SendLogMessage(LogWarn(Normal) << "gltf_XdeWriter skips node '%1' without meshes."
          << ::readNameAttribute(shapeTool, itM.Value()));

        continue;
      }

      glTFPrimitive facePrimitive;
      processFacePrimitive(faceProperty.Face(), facePrimitive);

      if (styles.IsBound(faceProperty.Face()))
      {
        facePrimitive.Style = styles(faceProperty.Face());
      }

      if (!m_meshes.Contains(itM.Key()))
      {
        m_meshes.Add(itM.Key(), NCollection_Vector<glTFPrimitive>());
      }

      m_meshes.ChangeFromKey(itM.Key()).Append(facePrimitive);
    }

    /* =========================
     *  Process edge primitives.
     * ========================= */
    TDF_LabelSequence subLabels;
    XCAFDoc_ShapeTool::GetSubShapes(itM.Value(), subLabels);
    for (TDF_LabelSequence::Iterator labelsIt(subLabels); labelsIt.More(); labelsIt.Next())
    {
      const TDF_Label& subShapeLabel = labelsIt.Value();

      // If there's no shape in the XDE for the host label, we do nothing.
      if (!XCAFDoc_ShapeTool::GetShape(subShapeLabel, shape) ||
           shape.IsNull() || shape.ShapeType() != TopAbs_EDGE)
        continue;

      TopoDS_Edge&         e = TopoDS::Edge(shape);
      glTFPrimitive edgePrimitive;
      if (!processEdgePrimitive(e, styles, edgePrimitive) )
        continue;

      if (!m_meshes.Contains(itM.Key()))
      {
        m_meshes.Add(itM.Key(), NCollection_Vector<glTFPrimitive>());
      }

      m_meshes.ChangeFromKey(itM.Key()).Append(edgePrimitive);
    }
  }
}

//-----------------------------------------------------------------------------

void glTFXdeDataSourceProvider::readStyles(const TDF_Label&  label,
                                            t_Shape2Style&    shapeStyles)
{
  // Get styles out of OCAF.
  TopLoc_Location                     dummyLoc;
  XCAFPrs_IndexedDataMapOfShapeStyle  styles;
  XCAFPrs::CollectStyleSettings(label, dummyLoc, styles);

  int nbTypes[TopAbs_SHAPE] = {};
  for (int tit = TopAbs_EDGE; tit >= TopAbs_COMPOUND; --tit)
  {
    // if during the first interation, the current shape type wasn't there, skip it.
    if ((tit < TopAbs_FACE) && (nbTypes[tit] == 0))
      continue;

    for (XCAFPrs_IndexedDataMapOfShapeStyle::Iterator sit(styles); sit.More(); sit.Next())
    {
      const TopoDS_Shape& keyShape = sit.Key();
      const TopAbs_ShapeEnum keyShapeType = keyShape.ShapeType();

      if (tit == TopAbs_EDGE)
      {
        ++nbTypes[keyShapeType];
      }
      if (tit != keyShapeType)
        continue;

      glTFXdeVisualStyle cafStyle = sit.Value();
      TopoDS_Shape keyShapeLocated = keyShape.Located(TopLoc_Location());
      //
      if (keyShapeType >= TopAbs_FACE)
      {
        shapeStyles.Bind(keyShapeLocated, cafStyle);
      }
      else
      {
        for (TopExp_Explorer fit(keyShapeLocated, TopAbs_FACE); fit.More(); fit.Next())
        {
          if (!shapeStyles.IsBound(fit.Current()))
          {
            shapeStyles.Bind(fit.Current(), cafStyle);
          }
        }
      }
    }
  }
}
//-----------------------------------------------------------------------------

bool glTFXdeDataSourceProvider::processFacePrimitive(const TopoDS_Face&    face,
                                                            glTFPrimitive& facePrimitive)
{
  if ( face.IsNull() )
    return false;

  glTFFacePropertyExtractor faceProperty(face);
  if ( faceProperty.IsEmptyMesh() )
    return false;

  /* ====================
   *  Fill-in nodes data.
   * ==================== */
  facePrimitive.PosAccessor.Count         = faceProperty.NbNodes();
  facePrimitive.PosAccessor.Type          = glTFAccessorLayout_Vec3;
  facePrimitive.PosAccessor.ComponentType = glTFAccessorComponentType_Float32;

  const int nodeUpper = faceProperty.NodeUpper();
  for ( int nit = faceProperty.NodeLower(); nit <= nodeUpper; ++nit )
  {
    gp_XYZ node = faceProperty.NodeTransformed(nit).XYZ();
    //facePrimitive.NodePos.BndBox.Add(Graphic3d_Vec3d(node.X(), node.Y(), node.Z()));
    facePrimitive.NodePositions.Append(node);
  }

  /* ======================
   *  Fill-in normals data.
   * ====================== */
  facePrimitive.NormAccessor.Count          = facePrimitive.PosAccessor.Count;
  facePrimitive.NormAccessor.Type           = glTFAccessorLayout_Vec3;
  facePrimitive.NormAccessor.ComponentType  = glTFAccessorComponentType_Float32;

  for ( int nit = faceProperty.NodeLower(); nit <= nodeUpper; ++nit )
  {
    const gp_Dir norm = faceProperty.NormalTransformed(nit);
    Graphic3d_Vec3 vecNormal((float) norm.X(), (float) norm.Y(), (float) norm.Z());
    facePrimitive.NodeNormals.Append(vecNormal);
  }

  /* ======================
   *  Fill-in texture data.
   * ====================== */
  if ( faceProperty.HasTexCoords() )
  {
    glTFXdeVisualStyle style = facePrimitive.Style;
    if ( !style.GetMaterial().IsNull() )
    {
      if ( !glTFMaterialMap::baseColorTexture(style.GetMaterial()).IsNull()
        && !style.GetMaterial()->PbrMaterial().MetallicRoughnessTexture.IsNull()
        && !style.GetMaterial()->PbrMaterial().EmissiveTexture.IsNull()
        && !style.GetMaterial()->PbrMaterial().OcclusionTexture.IsNull()
        && !style.GetMaterial()->PbrMaterial().NormalTexture.IsNull() )
      {
        facePrimitive.UVAccessor.Count          = faceProperty.NbNodes();
        facePrimitive.UVAccessor.Type           = glTFAccessorLayout_Vec2;
        facePrimitive.UVAccessor.ComponentType  = glTFAccessorComponentType_Float32;

        const int nodeUpper1 = faceProperty.NodeUpper();
        for ( int nit = faceProperty.NodeLower(); nit <= nodeUpper1; ++nit )
        {
          gp_Pnt2d texCoord = faceProperty.NodeTexCoord(nit);
          texCoord.SetY(1.0 - texCoord.Y());
          facePrimitive.NodeTextures.Append(texCoord);
        }
      }
    }
  }

  /* =================
   *  Fill-in indices.
   * ================= */
  facePrimitive.IndAccessor.Count         = faceProperty.NbTriangles() * 3;
  facePrimitive.IndAccessor.Type          = glTFAccessorLayout_Scalar;
  facePrimitive.IndAccessor.ComponentType = facePrimitive.PosAccessor.Count > std::numeric_limits<uint16_t>::max()
                                            ? glTFAccessorComponentType_UInt32
                                            : glTFAccessorComponentType_UInt16;

  const int elemLower = faceProperty.ElemLower();
  const int elemUpper = faceProperty.ElemUpper();
  for ( int eit = elemLower; eit <= elemUpper; ++eit )
  {
    Poly_Triangle tri = faceProperty.TriangleOriented(eit);
    tri(1) -= elemLower;
    tri(2) -= elemLower;
    tri(3) -= elemLower;
    facePrimitive.NodeIndices.Append(tri);
  }

  return true;
}

//-----------------------------------------------------------------------------

bool glTFXdeDataSourceProvider::processEdgePrimitive(const TopoDS_Edge&    edge,
                                                            const t_Shape2Style&  styles,
                                                            glTFPrimitive& edgePrimitive)
{
  TopLoc_Location                       loc;
  Handle(Poly_Triangulation)            tri;
  Handle(Poly_PolygonOnTriangulation)   polygon;
  BRep_Tool::PolygonOnTriangulation(edge, polygon, tri, loc);

  if (polygon.IsNull())
    return false;

  edgePrimitive.Mode = glTFPrimitiveMode::glTFPrimitiveMode_LineStrip;
  if (styles.IsBound(edge))
  {
    edgePrimitive.Style = styles(edge);
  }

  edgePrimitive.PosAccessor.Count = polygon->Nodes().Size();
  edgePrimitive.PosAccessor.Type = glTFAccessorLayout_Vec3;
  edgePrimitive.PosAccessor.ComponentType = glTFAccessorComponentType_Float32;

  const TColStd_Array1OfInteger& indices = polygon->Nodes();
  int index = indices.Lower();
  for (; index <= indices.Upper(); ++index)
  {
    const gp_XYZ& node = tri->Node(indices[index]).Transformed(loc).XYZ();
    //edgePrimitive.NodePos.BndBox.Add(Graphic3d_Vec3d(node.X(), node.Y(), node.Z()));
    edgePrimitive.NodePositions.Append(node);
  }

  return true;
}
