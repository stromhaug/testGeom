/***************************************************************************
 *   Copyright (c) OPEN CASCADE SAS                                        *
 *                                                                         *
 *   This file is part of Open CASCADE Technology software library.        *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 ***************************************************************************/

// Own include
#include <asiAsm_GLTFWriter.h>

// GlTF includes
#include <asiAsm_GLTFMaterialMap.h>
#include <asiAsm_GLTFFacePropertyExtractor.h>
//
#if defined USE_RAPIDJSON
  #include <asiAsm_GLTFJsonSerializer.h>
#endif

// asiAlgo includes
#include <asiAlgo_Utils.h>

// OpenCascade includes
#include <gp_Quaternion.hxx>
#include <NCollection_DataMap.hxx>
#include <OSD_OpenFile.hxx>
#include <OSD_File.hxx>
#include <OSD_Path.hxx>
#include <Poly_Triangulation.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFPrs.hxx>
#include <XCAFPrs_IndexedDataMapOfShapeStyle.hxx>

// Rapidjson includes
#if defined USE_RAPIDJSON
  #include <rapidjson/prettywriter.h>
  #include <rapidjson/ostreamwrapper.h>
#endif

using namespace asiAsm::xde;

namespace
{
  //! Writes three floating-point values.
  static void writeVec3(std::ostream& stream,
                        const gp_XYZ& _vec3)
  {
    Graphic3d_Vec3 vec3( float( _vec3.X() ), float( _vec3.Y() ), float( _vec3.Z() ) );
    stream.write( (const char*) vec3.GetData(), sizeof(vec3) );
  }

  //! Writes three floating-point values.
  static void writeVec3(std::ostream&         stream,
                        const Graphic3d_Vec3& _vec3)
  {
    stream.write( (const char*) _vec3.GetData(), sizeof(_vec3) );
  }

  //! Writes two floating-point values.
  static void writeVec2(std::ostream& stream,
                        const gp_XY&  _vec2)
  {
    Graphic3d_Vec2 vec2( float( _vec2.X() ), float( _vec2.Y() ) );
    stream.write( (const char*) vec2.GetData(), sizeof(vec2) );
  }

  //! Writes triangle indices.
  static void writeTriangle32(std::ostream&          stream,
                              const Graphic3d_Vec3i& tri)
  {
    stream.write( (const char*) tri.GetData(), sizeof(tri) );
  }

  //! Writes triangle indices.
  static void writeTriangle16(std::ostream&                     stream,
                              const NCollection_Vec3<uint16_t>& tri)
  {
    stream.write( (const char*) tri.GetData(), sizeof(tri) );
  }
}

//-----------------------------------------------------------------------------

glTFWriter::glTFWriter(const bool            isBinary,
                       ActAPI_ProgressEntry  progress,
                       ActAPI_PlotterEntry   plotter)
//
: ActAPI_IAlgorithm (progress, plotter),
  m_trsfFormat      (glTFWriterTrsfFormat_Compact),
  m_bIsBinary       (isBinary),
  m_binDataLen64    (0)
{
  m_CSTrsf.SetOutputLengthUnit(1.0); // meters
  m_CSTrsf.SetOutputCoordinateSystem(glTFCoordinateSystem_glTF);
}

//-----------------------------------------------------------------------------

glTFWriter::~glTFWriter()
{
  m_jsonWriter.reset();
}

//-----------------------------------------------------------------------------

void glTFWriter::writeBinDataNodes(std::ostream& binFile,
                                   int&          accessorNb) const
{
  t_Meshes2Primitives::Iterator itNodes2Primitives (m_dataProvider->GetSceneMeshes());
  for (; itNodes2Primitives.More(); itNodes2Primitives.Next())
  {
    NCollection_Vector<glTFPrimitive>::Iterator itPrm(itNodes2Primitives.Value());
    for (; itPrm.More(); itPrm.Next())
    {
      itPrm.ChangeValue().PosAccessor.Id = accessorNb++;
      itPrm.ChangeValue().PosAccessor.ByteOffset = (int64_t)binFile.tellp() - m_buffViewNodalPos.ByteOffset;

      NCollection_Vector<gp_XYZ>::Iterator itNodes(itPrm.Value().NodePositions);
      for (; itNodes.More(); itNodes.Next())
      {
        gp_XYZ& node = itNodes.ChangeValue();
        //gp_XYZ node(itNodes.Value());
        m_CSTrsf.TransformPosition(node);
        itPrm.ChangeValue().PosAccessor.BndBox.Add(Graphic3d_Vec3d(node.X(), node.Y(), node.Z()));
        writeVec3(binFile, node);
      }
    }
  }
}

//-----------------------------------------------------------------------------

void glTFWriter::writeBinDataNormals(std::ostream& binFile,
                                     int&          accessorNb) const
{
  t_Meshes2Primitives::Iterator itNodes2Primitives(m_dataProvider->GetSceneMeshes());
  for (; itNodes2Primitives.More(); itNodes2Primitives.Next())
  {
    NCollection_Vector<glTFPrimitive>::Iterator itPrm(itNodes2Primitives.Value());
    for (; itPrm.More(); itPrm.Next())
    {
      if (itPrm.Value().NodeNormals.Size() == 0)
        continue;

      itPrm.ChangeValue().NormAccessor.Id = accessorNb++;
      itPrm.ChangeValue().NormAccessor.ByteOffset = (int64_t)binFile.tellp() - m_buffViewNodalNorm.ByteOffset;
      NCollection_Vector<Graphic3d_Vec3>::Iterator itNormals(itPrm.Value().NodeNormals);
      for (; itNormals.More(); itNormals.Next())
      {
        //Graphic3d_Vec3 vecNormal(itNormals.Value());
        m_CSTrsf.TransformNormal(itNormals.ChangeValue());
        writeVec3(binFile, itNormals.Value());
      }
    }
  }
}

//-----------------------------------------------------------------------------

void glTFWriter::writeBinDataTextCoords(std::ostream& binFile,
                                        int&          accessorNb) const
{
  t_Meshes2Primitives::Iterator itNodes2Primitives(m_dataProvider->GetSceneMeshes());
  for (; itNodes2Primitives.More(); itNodes2Primitives.Next())
  {
    NCollection_Vector<glTFPrimitive>::Iterator itPrm(itNodes2Primitives.Value());
    for (; itPrm.More(); itPrm.Next())
    {
      if (itPrm.Value().NodeTextures.Size() == 0)
        continue;

      itPrm.ChangeValue().UVAccessor.Id = accessorNb++;
      itPrm.ChangeValue().UVAccessor.ByteOffset = (int64_t)binFile.tellp() - m_buffViewNodalTextCoord.ByteOffset;
      NCollection_Vector<gp_Pnt2d>::Iterator itTextures(itPrm.Value().NodeTextures);
      for (; itTextures.More(); itTextures.Next())
      {
        writeVec2(binFile, itTextures.Value().XY());
      }
    }
  }
}

//-----------------------------------------------------------------------------

void glTFWriter::writeBinDataNodalColors(std::ostream& binFile,
                                         int&          accessorNb) const
{
  t_Meshes2Primitives::Iterator itNodes2Primitives(m_dataProvider->GetSceneMeshes());
  for (; itNodes2Primitives.More(); itNodes2Primitives.Next())
  {
    NCollection_Vector<glTFPrimitive>::Iterator itPrm(itNodes2Primitives.Value());
    for (; itPrm.More(); itPrm.Next())
    {
      if (itPrm.Value().NodeColors.Size() == 0)
        continue;

      itPrm.ChangeValue().ColorAccessor.Id = accessorNb++;
      itPrm.ChangeValue().ColorAccessor.ByteOffset = (int64_t)binFile.tellp() - m_buffViewNodalColor.ByteOffset;
      NCollection_Vector<Graphic3d_Vec3>::Iterator itColors(itPrm.Value().NodeColors);
      for (; itColors.More(); itColors.Next())
      {
        writeVec3(binFile, itColors.Value());
      }
    }
  }
}

//-----------------------------------------------------------------------------

void glTFWriter::writeBinDataIndices(std::ostream& binFile,
                                     int&          accessorNb)
{
  t_Meshes2Primitives::Iterator itNodes2Primitives(m_dataProvider->GetSceneMeshes());
  for (; itNodes2Primitives.More(); itNodes2Primitives.Next())
  {
    NCollection_Vector<glTFPrimitive>::Iterator itPrm(itNodes2Primitives.Value());
    for (; itPrm.More(); itPrm.Next())
    {
      if (itPrm.Value().NodeIndices.Size() == 0)
        continue;

      itPrm.ChangeValue().IndAccessor.Id = accessorNb++;
      itPrm.ChangeValue().IndAccessor.ByteOffset = (int64_t)binFile.tellp() - m_buffViewIndices.ByteOffset;
      NCollection_Vector<Poly_Triangle>::Iterator itTriangles(itPrm.Value().NodeIndices);
      for (; itTriangles.More(); itTriangles.Next())
      {
        Poly_Triangle tri = itTriangles.Value();
        if (itPrm.Value().IndAccessor.ComponentType == glTFAccessorComponentType_UInt16)
        {
          writeTriangle16(binFile,
            NCollection_Vec3<uint16_t>((uint16_t)tri(1),
                                       (uint16_t)tri(2),
                                       (uint16_t)tri(3)));
        }
        else
        {
          writeTriangle32(binFile, Graphic3d_Vec3i(tri(1), tri(2), tri(3)));
        }
      }
      if (itPrm.Value().IndAccessor.ComponentType == glTFAccessorComponentType_UInt16)
      {
        // alignment by 4 bytes
        int64_t contentLen64 = (int64_t)binFile.tellp();
        //
        while (contentLen64 % 4 != 0)
        {
          binFile.write(" ", 1);
          ++contentLen64;
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------

bool glTFWriter::Perform(const TCollection_AsciiString&              filename, 
                         const Handle(glTFIDataSourceProvider)&      dataProvider,
                         const TColStd_IndexedDataMapOfStringString& fileInfo)
{
  m_filename = filename;
  TCollection_AsciiString dir, filenameShort, filenameShortBase, filenameExt;
  OSD_Path::FolderAndFileFromPath(filename, dir, filenameShort);
  asiAlgo_Utils::Str::FileNameAndExtension(filenameShort, filenameShortBase, filenameExt);

  m_binFilenameShort = filenameShortBase + ".bin" + (m_bIsBinary ? ".tmp" : "");
  m_binFilenameFull = !dir.IsEmpty() ? dir + m_binFilenameShort : m_binFilenameShort;

  if ( dataProvider.IsNull() )
    return false;

  m_dataProvider = dataProvider;
  m_dataProvider->Process(m_progress);

  if ( !this->writeBinData() )
    return false;

  return this->writeJson(fileInfo);
}

//-----------------------------------------------------------------------------

const glTFCSysConverter&
  glTFWriter::CoordinateSystemConverter() const
{
  return m_CSTrsf;
}

//-----------------------------------------------------------------------------

glTFCSysConverter&
  glTFWriter::ChangeCoordinateSystemConverter()
{
  return m_CSTrsf;
}

//-----------------------------------------------------------------------------

void glTFWriter::SetCoordinateSystemConverter(const glTFCSysConverter& converter)
{
  m_CSTrsf = converter;
}

//-----------------------------------------------------------------------------

bool glTFWriter::IsBinary() const
{
  return m_bIsBinary;
}

//-----------------------------------------------------------------------------

glTFWriterTrsfFormat
  glTFWriter::TransformationFormat() const
{
  return m_trsfFormat;
}

//-----------------------------------------------------------------------------

void glTFWriter::SetTransformationFormat(const glTFWriterTrsfFormat fmt)
{
  m_trsfFormat = fmt;
}

//-----------------------------------------------------------------------------

bool glTFWriter::IsForcedUVExport() const
{
  return m_bIsForcedUVExport;
}

//-----------------------------------------------------------------------------

void glTFWriter::SetForcedUVExport(const bool toForce)
{
  m_bIsForcedUVExport = toForce;
}

//-----------------------------------------------------------------------------

const glTFXdeVisualStyle&
  glTFWriter::DefaultStyle() const
{
  return m_defaultStyle;
}

//-----------------------------------------------------------------------------

void glTFWriter::SetDefaultStyle(const glTFXdeVisualStyle& style)
{
  m_defaultStyle = style;
}

//-----------------------------------------------------------------------------

bool glTFWriter::writeBinData()
{
  m_buffViewNodalPos.ByteOffset       = 0;
  m_buffViewNodalPos.ByteLength       = 0;

  m_buffViewNodalPos.ByteStride       = 12;
  m_buffViewNodalPos.Target           = glTFBufferViewTarget_ARRAY_BUFFER;

  m_buffViewNodalNorm.ByteOffset      = 0;
  m_buffViewNodalNorm.ByteLength      = 0;
  m_buffViewNodalNorm.ByteStride      = 12;
  m_buffViewNodalNorm.Target          = glTFBufferViewTarget_ARRAY_BUFFER;

  m_buffViewNodalTextCoord.ByteOffset = 0;
  m_buffViewNodalTextCoord.ByteLength = 0;
  m_buffViewNodalTextCoord.ByteStride = 8;
  m_buffViewNodalTextCoord.Target     = glTFBufferViewTarget_ARRAY_BUFFER;

  m_buffViewNodalColor.ByteOffset     = 0;
  m_buffViewNodalColor.ByteLength     = 0;
  m_buffViewNodalColor.ByteStride     = 12;
  m_buffViewNodalColor.Target         = glTFBufferViewTarget_ARRAY_BUFFER;

  m_buffViewIndices.ByteOffset        = 0;
  m_buffViewIndices.ByteLength        = 0;
  m_buffViewIndices.Target            = glTFBufferViewTarget_ELEMENT_ARRAY_BUFFER;

  m_binDataLen64 = 0;

  std::ofstream binFile;
  OSD_OpenStream(binFile, m_binFilenameFull.ToCString(), std::ios::out | std::ios::binary);
  //
  if ( !binFile.is_open()|| !binFile.good() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "File '%1' cannot be created."
                                             << m_binFilenameFull);
    return false;
  }

  int nbAccessors = 0;

  /* =================
   *  Write positions.
   * ================= */

  m_buffViewNodalPos.ByteOffset = binFile.tellp();
  //

  this->writeBinDataNodes(binFile, nbAccessors);

  if (!binFile.good())
  {
    m_progress.SendLogMessage(LogErr(Normal) << "File '%1' cannot be written." << m_binFilenameFull);
    return false;
  }

  m_buffViewNodalPos.ByteLength = (int64_t) binFile.tellp() - m_buffViewNodalPos.ByteOffset;

  /* ===============
   *  Write normals.
   * =============== */

  m_buffViewNodalNorm.ByteOffset = binFile.tellp();

  this->writeBinDataNormals(binFile, nbAccessors);

  if (!binFile.good())
  {
    m_progress.SendLogMessage(LogErr(Normal) << "File '%1' cannot be written." << m_binFilenameFull);
    return false;
  }

  m_buffViewNodalNorm.ByteLength = (int64_t) binFile.tellp() - m_buffViewNodalNorm.ByteOffset;

  /* ===========================
   *  Write texture coordinates.
   * =========================== */

  m_buffViewNodalTextCoord.ByteOffset = binFile.tellp();

  if (!m_bIsForcedUVExport)
  {
    this->writeBinDataTextCoords(binFile, nbAccessors);
  }

  if (!binFile.good())
  {
    m_progress.SendLogMessage(LogErr(Normal) << "File '%1' cannot be written." << m_binFilenameFull);
    return false;
  }

  m_buffViewNodalTextCoord.ByteLength = (int64_t) binFile.tellp() - m_buffViewNodalTextCoord.ByteOffset;

  /* ====================
   *  Write nodal colors.
   * ==================== */

  m_buffViewNodalColor.ByteOffset = binFile.tellp();

  this->writeBinDataNodalColors(binFile, nbAccessors);

  if (!binFile.good())
  {
    m_progress.SendLogMessage(LogErr(Normal) << "File '%1' cannot be written." << m_binFilenameFull);
    return false;
  }

  m_buffViewNodalColor.ByteLength = (int64_t)binFile.tellp() - m_buffViewNodalColor.ByteOffset;

  /* ===============
   *  Write indices.
   * =============== */

  m_buffViewIndices.ByteOffset = binFile.tellp();

  this->writeBinDataIndices(binFile, nbAccessors);

  if (!binFile.good())
  {
    m_progress.SendLogMessage(LogErr(Normal) << "File '%1' cannot be written." << m_binFilenameFull);
    return false;
  }

  m_buffViewIndices.ByteLength = (int64_t )binFile.tellp() - m_buffViewIndices.ByteOffset;

  /* ==========
   *  Finalize.
   * ========== */

  int buffViewId = 0;
  if ( m_buffViewNodalPos.ByteLength > 0 )
  {
    m_buffViewNodalPos.Id = buffViewId++;
  }
  if ( m_buffViewNodalNorm.ByteLength > 0 )
  {
    m_buffViewNodalNorm.Id = buffViewId++;
  }
  if ( m_buffViewNodalTextCoord.ByteLength > 0 )
  {
    m_buffViewNodalTextCoord.Id = buffViewId++;
  }
  if (m_buffViewNodalColor.ByteLength > 0)
  {
    m_buffViewNodalColor.Id = buffViewId++;
  }
  if (m_buffViewIndices.ByteLength > 0 )
  {
    m_buffViewIndices.Id = buffViewId++;
  }

  m_binDataLen64 = binFile.tellp();
  binFile.close();
  //
  if ( !binFile.good() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "File '%1' cannot be written." << m_binFilenameFull);
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------

bool glTFWriter::writeJson(const TColStd_IndexedDataMapOfStringString& fileInfo)
{
#if defined USE_RAPIDJSON
  m_jsonWriter.reset();

  const int binDatbufferId = 0;
  const int defSamplerId   = 0;
  const int defSceneId     = 0;

  const TCollection_AsciiString fileNameGltf = m_filename;
  std::ofstream gltfContentFile;
  OSD_OpenStream(gltfContentFile, fileNameGltf.ToCString(), std::ios::out | std::ios::binary);

  if ( !gltfContentFile.is_open() || !gltfContentFile.good())
  {
    m_progress.SendLogMessage(LogErr(Normal) << "File '%1' cannot be created." << fileNameGltf);
    return false;
  }

  if ( m_bIsBinary )
  {
    const char* magic         = "glTF";
    uint32_t    version       = 2;
    uint32_t    length        = 0;
    uint32_t    contentLength = 0;
    uint32_t    contentType   = 0x4E4F534A;

    gltfContentFile.write( magic,                        4                     );
    gltfContentFile.write( (const char*) &version,       sizeof(version)       );
    gltfContentFile.write( (const char*) &length,        sizeof(length)        );
    gltfContentFile.write( (const char*) &contentLength, sizeof(contentLength) );
    gltfContentFile.write( (const char*) &contentType,   sizeof(contentType)   );
  }

  // Prepare material map.
  glTFMaterialMap materialMap(m_filename, defSamplerId);
  materialMap.SetDefaultStyle(m_defaultStyle);

  // Root nodes indices start from 0.
  NCollection_Sequence<int> scRootIds;

  // Prepare JSON writer.
  rapidjson::OStreamWrapper fileStream(gltfContentFile);
  m_jsonWriter.reset( new glTFJsonSerializer(fileStream) );

  // Start writing.
  m_jsonWriter->StartObject();
  {
    // Write sections.
    this->writeAccessors   ();
    this->writeAnimations  ();
    this->writeAsset       (fileInfo);
    this->writeBufferViews (binDatbufferId);
    this->writeBuffers     ();
    this->writeExtensions  ();
    this->writeImages      (materialMap);
    this->writeMaterials   (materialMap);
    this->writeMeshes      (materialMap);
    this->writeNodes       ();
    this->writeSamplers    (materialMap);
    this->writeScene       (defSceneId);
    this->writeScenes      ();
    this->writeSkins       ();
    this->writeTextures    (materialMap);
  }
  m_jsonWriter->EndObject();

  if ( !m_bIsBinary )
  {
    gltfContentFile.close();
    if ( !gltfContentFile.good() )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "File '%1' cannot be written." << fileNameGltf);
      return false;
    }
    return true;
  }

  int64_t contentLen64 = (int64_t) gltfContentFile.tellp() - 20;
  while ( contentLen64 % 4 != 0 )
  {
    gltfContentFile.write(" ", 1);
    ++contentLen64;
  }

  const uint32_t binLength = (uint32_t) m_binDataLen64;
  const uint32_t binType   = 0x004E4942;
  //
  gltfContentFile.write( (const char*) &binLength, 4 );
  gltfContentFile.write( (const char*) &binType,   4 );

  const int64_t fullLen64 = contentLen64 + 20 + m_binDataLen64 + 8;
  //
  if ( fullLen64 < std::numeric_limits<uint32_t>::max() )
  {
    {
      std::ifstream binFile;
      OSD_OpenStream(binFile, m_binFilenameFull.ToCString(), std::ios::in | std::ios::binary);
      //
      if ( !binFile.is_open() || !binFile.good() )
      {
        m_progress.SendLogMessage(LogErr(Normal) << "File '%1' cannot be opened." << m_binFilenameFull);
        return false;
      }
      char buffer[4096];
      for ( ; binFile.good(); )
      {
        binFile.read(buffer, 4096);
        const int readLen = (int) binFile.gcount();
        if ( readLen == 0 )
        {
          break;
        }
        gltfContentFile.write(buffer, readLen);
      }
    }
    OSD_Path binFilePath (m_binFilenameFull);
    OSD_File(binFilePath).Remove();
    //
    if ( OSD_File(binFilePath).Exists() )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "Unable to remove temporary glTF content file '%1'." << m_binFilenameFull);
    }
  }
  else
  {
    m_progress.SendLogMessage(LogErr(Normal) << "glTF file content is too big for binary format.");
    return false;
  }

  const uint32_t length        = (uint32_t) fullLen64;
  const uint32_t contentLength = (uint32_t) contentLen64;
  //
  gltfContentFile.seekp (8);
  gltfContentFile.write ( (const char*) &length,        4 );
  gltfContentFile.write ( (const char*) &contentLength, 4 );

  gltfContentFile.close();
  if ( !gltfContentFile.good() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "File '%1' cannot be written." << fileNameGltf);
    return false;
  }

  m_jsonWriter.reset();
  return true;
#else
  // Suppress `unreferenced formal parameter` warning (C4100).
  (void) fileInfo;

  m_progress.SendLogMessage(LogErr(High) << "glTF export is impossible: you have to build "
                                            "Analysis Situs with rapidjson.");
  return false;
#endif
}

//-----------------------------------------------------------------------------

void glTFWriter::writeAccessors()
{
#if defined USE_RAPIDJSON
  Standard_ProgramError_Raise_if(m_jsonWriter.get() == nullptr, "Internal error: gltf_XdeWriter::writeAccessors()");

  m_jsonWriter->Key( glTFRootElementName(glTFRootElement_Accessors) );
  m_jsonWriter->StartArray();

  /* =================
   *  Write positions.
   * ================= */

  //
  for (t_Meshes2Primitives::Iterator itNodes2Primitives(m_dataProvider->GetSceneMeshes()); itNodes2Primitives.More(); itNodes2Primitives.Next())
  {
    NCollection_Vector<glTFPrimitive>::Iterator itPrm(itNodes2Primitives.Value());
    for (; itPrm.More(); itPrm.Next())
    {
      //
      this->writeNodalPositions(itPrm.Value());
    }
  }

  /* ===============
   *  Write normals.
   * =============== */

  //
  for (t_Meshes2Primitives::Iterator itNodes2Primitives(m_dataProvider->GetSceneMeshes()); itNodes2Primitives.More(); itNodes2Primitives.Next())
  {
    NCollection_Vector<glTFPrimitive>::Iterator itPrm(itNodes2Primitives.Value());
    for (; itPrm.More(); itPrm.Next())
    {
      //
      this->writeNodalNormals(itPrm.Value());
    }
  }

  /* ===========================
   *  Write texture coordinates.
   * =========================== */

  //
  for (t_Meshes2Primitives::Iterator itNodes2Primitives(m_dataProvider->GetSceneMeshes()); itNodes2Primitives.More(); itNodes2Primitives.Next())
  {
    NCollection_Vector<glTFPrimitive>::Iterator itPrm(itNodes2Primitives.Value());
    for (; itPrm.More(); itPrm.Next())
    {
      //
      this->writeNodalTextCoords(itPrm.Value());
    }
  }

  /* =============
   *  Write colors.
   * ============= */

   //
  for (t_Meshes2Primitives::Iterator itNodes2Primitives(m_dataProvider->GetSceneMeshes()); itNodes2Primitives.More(); itNodes2Primitives.Next())
  {
    NCollection_Vector<glTFPrimitive>::Iterator itPrm(itNodes2Primitives.Value());
    for (; itPrm.More(); itPrm.Next())
    {
      //
      this->writeNodalColors(itPrm.Value());
    }
  }

  /* ===============
   *  Write indices.
   * =============== */

  //
  for (t_Meshes2Primitives::Iterator itNodes2Primitives(m_dataProvider->GetSceneMeshes()); itNodes2Primitives.More(); itNodes2Primitives.Next())
  {
    NCollection_Vector<glTFPrimitive> primitives = itNodes2Primitives.Value();

    NCollection_Vector<glTFPrimitive>::Iterator itPrm(primitives);
    for (; itPrm.More(); itPrm.Next())
    {
      //
      this->writeNodalIndices(itPrm.Value());
    }
  }

  m_jsonWriter->EndArray();
#else
  m_progress.SendLogMessage(LogErr(High) << "glTF export is impossible: you have to build "
                                            "Analysis Situs with rapidjson.");
#endif
}

//-----------------------------------------------------------------------------

void glTFWriter::writeNodalPositions(const glTFPrimitive& gltfPrm)
{
#if defined USE_RAPIDJSON
  Standard_ProgramError_Raise_if(m_jsonWriter.get() == nullptr, "Internal error: gltf_XdeWriter::writeNodalPositions()");

  if (gltfPrm.PosAccessor.Id == glTFAccessor::INVALID_ID )
    return;

  m_jsonWriter->StartObject();
  m_jsonWriter->Key   ("bufferView");
  m_jsonWriter->Int   (m_buffViewNodalPos.Id);
  m_jsonWriter->Key   ("byteOffset");
  m_jsonWriter->Int64 (gltfPrm.PosAccessor.ByteOffset);
  m_jsonWriter->Key   ("componentType");
  m_jsonWriter->Int   (gltfPrm.PosAccessor.ComponentType);
  m_jsonWriter->Key   ("count");
  m_jsonWriter->Int64 (gltfPrm.PosAccessor.Count);

  if (gltfPrm.PosAccessor.BndBox.IsValid() )
  {
    m_jsonWriter->Key ("max");
    m_jsonWriter->StartArray();
    m_jsonWriter->Double (gltfPrm.PosAccessor.BndBox.CornerMax().x() );
    m_jsonWriter->Double (gltfPrm.PosAccessor.BndBox.CornerMax().y() );
    m_jsonWriter->Double (gltfPrm.PosAccessor.BndBox.CornerMax().z() );
    m_jsonWriter->EndArray();

    m_jsonWriter->Key("min");
    m_jsonWriter->StartArray();
    m_jsonWriter->Double (gltfPrm.PosAccessor.BndBox.CornerMin().x() );
    m_jsonWriter->Double (gltfPrm.PosAccessor.BndBox.CornerMin().y() );
    m_jsonWriter->Double (gltfPrm.PosAccessor.BndBox.CornerMin().z() );
    m_jsonWriter->EndArray();
  }
  m_jsonWriter->Key    ("type");
  m_jsonWriter->String ("VEC3");

  m_jsonWriter->Key    ("name");
  m_jsonWriter->String ("Positions Accessor");

  m_jsonWriter->EndObject();
#else
  m_progress.SendLogMessage(LogErr(High) << "glTF export is impossible: you have to build "
                                            "Analysis Situs with rapidjson.");
#endif
}

//-----------------------------------------------------------------------------

void glTFWriter::writeNodalNormals(const glTFPrimitive& gltfPrm)
{
#if defined USE_RAPIDJSON
  Standard_ProgramError_Raise_if(m_jsonWriter.get() == nullptr, "Internal error: gltf_XdeWriter::writeNodalNormals()");

  if (gltfPrm.NormAccessor.Id == glTFAccessor::INVALID_ID )
    return;

  m_jsonWriter->StartObject();
  m_jsonWriter->Key    ("bufferView");
  m_jsonWriter->Int    (m_buffViewNodalNorm.Id);
  m_jsonWriter->Key    ("byteOffset");
  m_jsonWriter->Int64  (gltfPrm.NormAccessor.ByteOffset);
  m_jsonWriter->Key    ("componentType");
  m_jsonWriter->Int    (gltfPrm.NormAccessor.ComponentType);
  m_jsonWriter->Key    ("count");
  m_jsonWriter->Int64  (gltfPrm.NormAccessor.Count);

  /* min/max values are optional, and not very useful for normals - skip them */

  m_jsonWriter->Key    ("type");
  m_jsonWriter->String ("VEC3");

  m_jsonWriter->Key("name");
  m_jsonWriter->String("Normals Accessor");

  m_jsonWriter->EndObject();
#else
  m_progress.SendLogMessage(LogErr(High) << "glTF export is impossible: you have to build "
                                            "Analysis Situs with rapidjson.");
#endif
}

//-----------------------------------------------------------------------------

void glTFWriter::writeNodalTextCoords(const glTFPrimitive& gltfPrm)
{
#if defined USE_RAPIDJSON
  Standard_ProgramError_Raise_if(m_jsonWriter.get() == nullptr, "Internal error: gltf_XdeWriter::writeNodalTextCoords()");

  if (gltfPrm.UVAccessor.Id == glTFAccessor::INVALID_ID )
    return;

  m_jsonWriter->StartObject();
  m_jsonWriter->Key    ("bufferView");
  m_jsonWriter->Int    (m_buffViewNodalTextCoord.Id);
  m_jsonWriter->Key    ("byteOffset");
  m_jsonWriter->Int64  (gltfPrm.UVAccessor.ByteOffset);
  m_jsonWriter->Key    ("componentType");
  m_jsonWriter->Int    (gltfPrm.UVAccessor.ComponentType);
  m_jsonWriter->Key    ("count");
  m_jsonWriter->Int64  (gltfPrm.UVAccessor.Count);

  /* min/max values are optional, and not very useful for UV coordinates - skip them */

  m_jsonWriter->Key    ("type");
  m_jsonWriter->String ("VEC2");

  m_jsonWriter->Key   ("name");
  m_jsonWriter->String("Textures Accessor");

  m_jsonWriter->EndObject();
#else
  m_progress.SendLogMessage(LogErr(High) << "glTF export is impossible: you have to build "
                                            "Analysis Situs with rapidjson.");
#endif
}

//-----------------------------------------------------------------------------

void glTFWriter::writeNodalColors(const glTFPrimitive& gltfPrm)
{
#if defined USE_RAPIDJSON
  Standard_ProgramError_Raise_if(m_jsonWriter.get() == nullptr, "Internal error: gltf_XdeWriter::writeNodalColors()");

  if (gltfPrm.ColorAccessor.Id == glTFAccessor::INVALID_ID)
    return;

  m_jsonWriter->StartObject();
  m_jsonWriter->Key("bufferView");
  m_jsonWriter->Int(m_buffViewNodalColor.Id);
  m_jsonWriter->Key("byteOffset");
  m_jsonWriter->Int64(gltfPrm.ColorAccessor.ByteOffset);
  m_jsonWriter->Key("componentType");
  m_jsonWriter->Int(gltfPrm.ColorAccessor.ComponentType);
  m_jsonWriter->Key("count");
  m_jsonWriter->Int64(gltfPrm.ColorAccessor.Count);

  m_jsonWriter->Key("type");
  m_jsonWriter->String("VEC3");

  m_jsonWriter->Key("name");
  m_jsonWriter->String("Colors Accessor");

  m_jsonWriter->EndObject();
#else
  m_progress.SendLogMessage(LogErr(High) << "glTF export is impossible: you have to build "
    "Analysis Situs with rapidjson.");
#endif
}

//-----------------------------------------------------------------------------

void glTFWriter::writeNodalIndices (const glTFPrimitive& gltfPrm)
{
#if defined USE_RAPIDJSON
  Standard_ProgramError_Raise_if(m_jsonWriter.get() == nullptr, "Internal error: gltf_XdeWriter::writeIndices()");

  if (gltfPrm.IndAccessor.Id == glTFAccessor::INVALID_ID )
    return;

  m_jsonWriter->StartObject();
  m_jsonWriter->Key    ("bufferView");
  m_jsonWriter->Int    (m_buffViewIndices.Id);
  m_jsonWriter->Key    ("byteOffset");
  m_jsonWriter->Int64  (gltfPrm.IndAccessor.ByteOffset);
  m_jsonWriter->Key    ("componentType");
  m_jsonWriter->Int    (gltfPrm.IndAccessor.ComponentType);
  m_jsonWriter->Key    ("count");
  m_jsonWriter->Int64  (gltfPrm.IndAccessor.Count);

  m_jsonWriter->Key    ("type");
  m_jsonWriter->String ("SCALAR");

  m_jsonWriter->Key("name");
  m_jsonWriter->String("Indices Accessor");

  m_jsonWriter->EndObject();
#else
  m_progress.SendLogMessage(LogErr(High) << "glTF export is impossible: you have to build "
                                            "Analysis Situs with rapidjson.");
#endif
}

//-----------------------------------------------------------------------------

void glTFWriter::writeAnimations()
{
  // TODO: NYI
}

//-----------------------------------------------------------------------------

void glTFWriter::writeAsset(const TColStd_IndexedDataMapOfStringString& fileInfo)
{
#if defined USE_RAPIDJSON
  Standard_ProgramError_Raise_if (m_jsonWriter.get() == nullptr, "Internal error: gltf_XdeWriter::writeAsset()");

  m_jsonWriter->Key    ( glTFRootElementName(glTFRootElement_Asset) );
  m_jsonWriter->StartObject();
  m_jsonWriter->Key    ("generator");
  m_jsonWriter->String (gltf_VendorName);
  m_jsonWriter->Key    ("version");
  m_jsonWriter->String ("2.0"); // glTF format version

  bool isStarted = false;
  for ( TColStd_IndexedDataMapOfStringString::Iterator kvIt(fileInfo); kvIt.More(); kvIt.Next() )
  {
    if ( !isStarted )
    {
      m_jsonWriter->Key("extras");
      m_jsonWriter->StartObject();
      isStarted = true;
    }
    m_jsonWriter->Key( kvIt.Key().ToCString() );
    m_jsonWriter->String( kvIt.Value().ToCString() );
  }
  if ( isStarted )
  {
    m_jsonWriter->EndObject();
  }

  m_jsonWriter->EndObject();
#else
  // Suppress `unreferenced formal parameter` warning (C4100).
  (void) fileInfo;

  m_progress.SendLogMessage(LogErr(High) << "glTF export is impossible: you have to build "
                                            "Analysis Situs with rapidjson.");
#endif
}

//-----------------------------------------------------------------------------

void glTFWriter::writeBufferViews(const int binDataBufferId)
{
#if defined USE_RAPIDJSON
  Standard_ProgramError_Raise_if(m_jsonWriter.get() == nullptr, "Internal error: gltf_XdeWriter::writeBufferViews()");

  m_jsonWriter->Key( glTFRootElementName(glTFRootElement_BufferViews) );
  m_jsonWriter->StartArray();

  if ( m_buffViewNodalPos.Id != glTFAccessor::INVALID_ID )
  {
    m_jsonWriter->StartObject();
    m_jsonWriter->Key    ("buffer");
    m_jsonWriter->Int    (binDataBufferId);
    m_jsonWriter->Key    ("byteLength");
    m_jsonWriter->Int64  (m_buffViewNodalPos.ByteLength);
    m_jsonWriter->Key    ("byteOffset");
    m_jsonWriter->Int64  (m_buffViewNodalPos.ByteOffset);
    m_jsonWriter->Key    ("byteStride");
    m_jsonWriter->Int64  (m_buffViewNodalPos.ByteStride);
    m_jsonWriter->Key    ("target");
    m_jsonWriter->Int    (m_buffViewNodalPos.Target);
    m_jsonWriter->Key    ("name");
    m_jsonWriter->String ("Positions");
    m_jsonWriter->EndObject();
  }
  if ( m_buffViewNodalNorm.Id != glTFAccessor::INVALID_ID )
  {
    m_jsonWriter->StartObject();
    m_jsonWriter->Key    ("buffer");
    m_jsonWriter->Int    (binDataBufferId);
    m_jsonWriter->Key    ("byteLength");
    m_jsonWriter->Int64  (m_buffViewNodalNorm.ByteLength);
    m_jsonWriter->Key    ("byteOffset");
    m_jsonWriter->Int64  (m_buffViewNodalNorm.ByteOffset);
    m_jsonWriter->Key    ("byteStride");
    m_jsonWriter->Int64  (m_buffViewNodalNorm.ByteStride);
    m_jsonWriter->Key    ("target");
    m_jsonWriter->Int    (m_buffViewNodalNorm.Target);
    m_jsonWriter->Key    ("name");
    m_jsonWriter->String ("Normals");
    m_jsonWriter->EndObject();
  }
  if ( m_buffViewNodalTextCoord.Id != glTFAccessor::INVALID_ID )
  {
    m_jsonWriter->StartObject();
    m_jsonWriter->Key    ("buffer");
    m_jsonWriter->Int    (binDataBufferId);
    m_jsonWriter->Key    ("byteLength");
    m_jsonWriter->Int64  (m_buffViewNodalTextCoord.ByteLength);
    m_jsonWriter->Key    ("byteOffset");
    m_jsonWriter->Int64  (m_buffViewNodalTextCoord.ByteOffset);
    m_jsonWriter->Key    ("byteStride");
    m_jsonWriter->Int64  (m_buffViewNodalTextCoord.ByteStride);
    m_jsonWriter->Key    ("target");
    m_jsonWriter->Int    (m_buffViewNodalTextCoord.Target);
    m_jsonWriter->Key    ("name");
    m_jsonWriter->String ("Textures");
    m_jsonWriter->EndObject();
  }
  if (m_buffViewNodalColor.Id != glTFAccessor::INVALID_ID)
  {
    m_jsonWriter->StartObject();
    m_jsonWriter->Key     ("buffer");
    m_jsonWriter->Int     (binDataBufferId);
    m_jsonWriter->Key     ("byteLength");
    m_jsonWriter->Int64   (m_buffViewNodalColor.ByteLength);
    m_jsonWriter->Key     ("byteOffset");
    m_jsonWriter->Int64   (m_buffViewNodalColor.ByteOffset);
    m_jsonWriter->Key     ("byteStride");
    m_jsonWriter->Int64   (m_buffViewNodalColor.ByteStride);
    m_jsonWriter->Key     ("target");
    m_jsonWriter->Int     (m_buffViewNodalColor.Target);
    m_jsonWriter->Key     ("name");
    m_jsonWriter->String  ("Colors");
    m_jsonWriter->EndObject();
  }
  if ( m_buffViewIndices.Id != glTFAccessor::INVALID_ID )
  {
    m_jsonWriter->StartObject();
    m_jsonWriter->Key     ("buffer");
    m_jsonWriter->Int     (binDataBufferId);
    m_jsonWriter->Key     ("byteLength");
    m_jsonWriter->Int64   (m_buffViewIndices.ByteLength);
    m_jsonWriter->Key     ("byteOffset");
    m_jsonWriter->Int64   (m_buffViewIndices.ByteOffset);
    m_jsonWriter->Key     ("target");
    m_jsonWriter->Int     (m_buffViewIndices.Target);
    m_jsonWriter->Key     ("name");
    m_jsonWriter->String  ("Indices");
    m_jsonWriter->EndObject();
  }
  m_jsonWriter->EndArray();
#else
  // Suppress `unreferenced formal parameter` warning (C4100).
  (void) binDataBufferId;

  m_progress.SendLogMessage(LogErr(High) << "glTF export is impossible: you have to build "
                                            "Analysis Situs with rapidjson.");
#endif
}

//-----------------------------------------------------------------------------

void glTFWriter::writeBuffers()
{
#if defined USE_RAPIDJSON
  Standard_ProgramError_Raise_if(m_jsonWriter.get() == nullptr, "Internal error: gltf_XdeWriter::writeBuffers()");

  m_jsonWriter->Key( glTFRootElementName(glTFRootElement_Buffers) );
  m_jsonWriter->StartArray();
  {
    m_jsonWriter->StartObject();
    {
      m_jsonWriter->Key   ("byteLength");
      m_jsonWriter->Int64 (m_buffViewNodalPos.ByteLength + m_buffViewNodalNorm.ByteLength +
                           m_buffViewNodalTextCoord.ByteLength + m_buffViewIndices.ByteLength);
      if ( !m_bIsBinary )
      {
        m_jsonWriter->Key    ("uri");
        m_jsonWriter->String (m_binFilenameShort.ToCString());
      }
    }
    m_jsonWriter->EndObject();
  }
  m_jsonWriter->EndArray();
#else
  m_progress.SendLogMessage(LogErr(High) << "glTF export is impossible: you have to build "
                                            "Analysis Situs with rapidjson.");
#endif
}

//-----------------------------------------------------------------------------

void glTFWriter::writeExtensions()
{
  // TODO: NYI
}

//-----------------------------------------------------------------------------

void glTFWriter::writeImages(glTFMaterialMap& materialMap)
{
#if defined USE_RAPIDJSON
  Standard_ProgramError_Raise_if(m_jsonWriter.get() == nullptr, "Internal error: gltf_XdeWriter::writeImages()");

  // empty section should NOT be written to avoid validator errors
  bool isStarted = false;
  //
  t_Meshes2Primitives::Iterator itNodes2Primitives(m_dataProvider->GetSceneMeshes());
  for (; itNodes2Primitives.More(); itNodes2Primitives.Next())
  {
    NCollection_Vector<glTFPrimitive>::Iterator itPrm(itNodes2Primitives.Value());
    for (; itPrm.More(); itPrm.Next())
    {
      materialMap.AddImages(m_jsonWriter.get(), itPrm.Value().Style, isStarted);
    }
  }
  if ( isStarted )
  {
    m_jsonWriter->EndArray();
  }
#else
  // Suppress `unreferenced formal parameter` warning (C4100).
  (void) materialMap;

  m_progress.SendLogMessage(LogErr(High) << "glTF export is impossible: you have to build "
                                            "Analysis Situs with rapidjson.");
#endif
}

//-----------------------------------------------------------------------------

void glTFWriter::writeMaterials(glTFMaterialMap& materialMap)
{
#if defined USE_RAPIDJSON
  Standard_ProgramError_Raise_if(m_jsonWriter.get() == nullptr, "Internal error: gltf_XdeWriter::writeMaterials()");

  // empty section should NOT be written to avoid validator errors
  bool isStarted = false;
  //
  t_Meshes2Primitives::Iterator itNodes2Primitives(m_dataProvider->GetSceneMeshes());
  for (; itNodes2Primitives.More(); itNodes2Primitives.Next())
  {
    NCollection_Vector<glTFPrimitive>::Iterator itPrm(itNodes2Primitives.Value());
    for (; itPrm.More(); itPrm.Next())
    {
      materialMap.AddMaterial(m_jsonWriter.get(), itPrm.Value().Style, isStarted);
    }
  }

  if (isStarted)
  {
    m_jsonWriter->EndArray();
  }
#else
  // Suppress `unreferenced formal parameter` warning (C4100).
  (void)materialMap;

  m_progress.SendLogMessage(LogErr(High) << "glTF export is impossible: you have to build "
    "Analysis Situs with rapidjson.");
#endif
}

//-----------------------------------------------------------------------------

void glTFWriter::writeMeshes(const glTFMaterialMap& materialMap)
{
#if defined USE_RAPIDJSON
  Standard_ProgramError_Raise_if(m_jsonWriter.get() == nullptr, "Internal error: gltf_XdeWriter::writeMeshes()");

  m_jsonWriter->Key( glTFRootElementName(glTFRootElement_Meshes) );
  m_jsonWriter->StartArray();

  t_Meshes2Primitives::Iterator itNodes2Primitives(m_dataProvider->GetSceneMeshes());
  for (; itNodes2Primitives.More(); itNodes2Primitives.Next())
  {
    const TCollection_AsciiString& nodeName = itNodes2Primitives.Key()->Name;

    // start "mesh" object
    m_jsonWriter->StartObject();
    m_jsonWriter->Key("name");
    m_jsonWriter->String(nodeName.ToCString());

    // start "primitives" array
    m_jsonWriter->Key("primitives");
    m_jsonWriter->StartArray();

    NCollection_Vector<glTFPrimitive>::Iterator itPrm(itNodes2Primitives.Value());
    for (; itPrm.More(); itPrm.Next())
    {
      const glTFPrimitive&      primitive = itPrm.Value();
      const TCollection_AsciiString matId = materialMap.FindMaterial(primitive.Style );

      m_jsonWriter->StartObject();
      {
        m_jsonWriter->Key("attributes");
        m_jsonWriter->StartObject();
        {
          if (primitive.NormAccessor.Id != glTFAccessor::INVALID_ID )
          {
            m_jsonWriter->Key("NORMAL");
            m_jsonWriter->Int(primitive.NormAccessor.Id);
          }
          m_jsonWriter->Key("POSITION");
          m_jsonWriter->Int(primitive.PosAccessor.Id);

          if (primitive.UVAccessor.Id != glTFAccessor::INVALID_ID )
          {
            m_jsonWriter->Key("TEXCOORD_0");
            m_jsonWriter->Int(primitive.UVAccessor.Id);
          }
          if (primitive.ColorAccessor.Id != glTFAccessor::INVALID_ID)
          {
            m_jsonWriter->Key("COLOR_0");
            m_jsonWriter->Int(primitive.ColorAccessor.Id);
          }
        }
        m_jsonWriter->EndObject();

        if (primitive.IndAccessor.Id != glTFAccessor::INVALID_ID)
        {
          m_jsonWriter->Key("indices");
          m_jsonWriter->Int(primitive.IndAccessor.Id);
        }

        if ( !matId.IsEmpty() )
        {
          m_jsonWriter->Key("material");
          m_jsonWriter->Int( matId.IntegerValue() );
        }
        m_jsonWriter->Key("mode");
        m_jsonWriter->Int((int)primitive.Mode);

        if (!primitive.Name.IsEmpty())
        {
          m_jsonWriter->Key("name");
          m_jsonWriter->String(primitive.Name.ToCString());
        }
      }
      m_jsonWriter->EndObject();
    }

    // end "primitives" array
    m_jsonWriter->EndArray();
    // end "mesh" object
    m_jsonWriter->EndObject();
  }
  m_jsonWriter->EndArray();
#else
  // Suppress `unreferenced formal parameter` warning (C4100).
  (void) materialMap;

  m_progress.SendLogMessage(LogErr(High) << "glTF export is impossible: you have to build "
                                            "Analysis Situs with rapidjson.");
#endif
}

//-----------------------------------------------------------------------------

void glTFWriter::writeNodes()
{
#if defined USE_RAPIDJSON
  Standard_ProgramError_Raise_if(m_jsonWriter.get() == nullptr, "Internal error: gltf_XdeWriter::writeNodes()");

  //* Write scene nodes using prepared map for correct order of array members
  m_jsonWriter->Key(glTFRootElementName(glTFRootElement_Nodes));
  m_jsonWriter->StartArray();

  const glTFSceneStructure& sceneNodeStrt = m_dataProvider->GetSceneStructure();
  for (auto node : sceneNodeStrt.GetNodes())
  {
    m_jsonWriter->StartObject();
    {
      if (node->Children.size() > 0)
      {
        m_jsonWriter->Key("children");
        m_jsonWriter->StartArray();
        {
          for (auto child : node->Children)
          {
            m_jsonWriter->Int(sceneNodeStrt.GetIndex(child));
          }
        }
        m_jsonWriter->EndArray();
      }
    }
    if (!node->Trsf.IsIdentity())
    {
      gp_Trsf trsf = node->Trsf.Transformation();

      if (trsf.Form() != gp_Identity)
      {
        m_CSTrsf.TransformTransformation(trsf);

        const gp_Quaternion qn = trsf.GetRotation();
        const bool hasRotation = Abs(qn.X()) > gp::Resolution()
          || Abs(qn.Y()) > gp::Resolution()
          || Abs(qn.Z()) > gp::Resolution()
          || Abs(qn.W() - 1.0) > gp::Resolution();
        //
        const double  scaleFactor = trsf.ScaleFactor();
        const bool    hasScale = Abs(scaleFactor - 1.0) > Precision::Confusion();
        const gp_XYZ& translPart = trsf.TranslationPart();
        const bool    hasTranslation = translPart.SquareModulus() > gp::Resolution();

        glTFWriterTrsfFormat trsfFormat = m_trsfFormat;
        if (m_trsfFormat == glTFWriterTrsfFormat_Compact)
        {
          trsfFormat = hasRotation && hasScale && hasTranslation
            ? glTFWriterTrsfFormat_Mat4
            : glTFWriterTrsfFormat_TRS;
        }

        if (trsfFormat == glTFWriterTrsfFormat_Mat4)
        {
          // write full matrix
          Graphic3d_Mat4 mat4;
          trsf.GetMat4(mat4);
          //
          if (!mat4.IsIdentity())
          {
            m_jsonWriter->Key("matrix");
            m_jsonWriter->StartArray();
            //
            for (int icol = 0; icol < 4; ++icol)
            {
              for (int irow = 0; irow < 4; ++irow)
              {
                m_jsonWriter->Double(mat4.GetValue(irow, icol));
              }
            }
            //
            m_jsonWriter->EndArray();
          }
        }
        else // TRS
        {
          if (hasRotation)
          {
            m_jsonWriter->Key("rotation");
            m_jsonWriter->StartArray();
            m_jsonWriter->Double(qn.X());
            m_jsonWriter->Double(qn.Y());
            m_jsonWriter->Double(qn.Z());
            m_jsonWriter->Double(qn.W());
            m_jsonWriter->EndArray();
          }
          if (hasScale)
          {
            m_jsonWriter->Key("scale");
            m_jsonWriter->StartArray();
            m_jsonWriter->Double(scaleFactor);
            m_jsonWriter->Double(scaleFactor);
            m_jsonWriter->Double(scaleFactor);
            m_jsonWriter->EndArray();
          }
          if (hasTranslation)
          {
            m_jsonWriter->Key("translation");
            m_jsonWriter->StartArray();
            m_jsonWriter->Double(translPart.X());
            m_jsonWriter->Double(translPart.Y());
            m_jsonWriter->Double(translPart.Z());
            m_jsonWriter->EndArray();
          }
        }
      }
    }
    if (node->MeshIndex != glTFNode::INVALID_ID)
    {
      m_jsonWriter->Key("mesh");
      m_jsonWriter->Int(node->MeshIndex);
    }

    m_jsonWriter->Key("name");
    m_jsonWriter->String(node->Name.ToCString());

    m_jsonWriter->EndObject();
  }
  m_jsonWriter->EndArray();
#else
  m_progress.SendLogMessage(LogErr(High) << "glTF export is impossible: you have to build "
    "Analysis Situs with rapidjson.");
#endif
}

//-----------------------------------------------------------------------------

void glTFWriter::writeSamplers(const glTFMaterialMap& materialMap)
{
#if defined USE_RAPIDJSON
  Standard_ProgramError_Raise_if(m_jsonWriter.get() == nullptr, "Internal error: gltf_XdeWriter::writeSamplers()");

  if ( materialMap.NbImages() == 0 )
    return;

  m_jsonWriter->Key( glTFRootElementName(glTFRootElement_Samplers) );
  m_jsonWriter->StartArray();
  {
    m_jsonWriter->StartObject();
    {
      //m_jsonWriter->Key ("magFilter");
      //m_jsonWriter->Int (9729);
      //m_jsonWriter->Key ("minFilter");
      //m_jsonWriter->Int (9729);
    }
    m_jsonWriter->EndObject();
  }
  m_jsonWriter->EndArray();
#else
  // Suppress `unreferenced formal parameter` warning (C4100).
  (void) materialMap;

  m_progress.SendLogMessage(LogErr(High) << "glTF export is impossible: you have to build "
                                            "Analysis Situs with rapidjson.");
#endif
}

//-----------------------------------------------------------------------------

void glTFWriter::writeScene(const int defSceneId)
{
#if defined USE_RAPIDJSON
  Standard_ProgramError_Raise_if(m_jsonWriter.get() == nullptr, "Internal error: gltf_XdeWriter::writeScene()");

  m_jsonWriter->Key( glTFRootElementName(glTFRootElement_Scene) );
  m_jsonWriter->Int(defSceneId);
#else
  // Suppress `unreferenced formal parameter` warning (C4100).
  (void) defSceneId;

  m_progress.SendLogMessage(LogErr(High) << "glTF export is impossible: you have to build "
                                            "Analysis Situs with rapidjson.");
#endif
}

//-----------------------------------------------------------------------------

void glTFWriter::writeScenes()
{
#if defined USE_RAPIDJSON
  Standard_ProgramError_Raise_if(m_jsonWriter.get() == nullptr, "Internal error: gltf_XdeWriter::writeScenes()");

  m_jsonWriter->Key( glTFRootElementName(glTFRootElement_Scenes) );
  m_jsonWriter->StartArray();
  {
    m_jsonWriter->StartObject();
    m_jsonWriter->Key("nodes");
    m_jsonWriter->StartArray();
    //
    const glTFSceneStructure& structure = m_dataProvider->GetSceneStructure();
    for (auto root : structure.GetRoots())
    {
      m_jsonWriter->Int(structure.GetIndex(root));
    }
    //
    m_jsonWriter->EndArray();
    m_jsonWriter->EndObject();
  }
  m_jsonWriter->EndArray();
#else
  m_progress.SendLogMessage(LogErr(High) << "glTF export is impossible: you have to build "
                                            "Analysis Situs with rapidjson.");
#endif
}

//-----------------------------------------------------------------------------

void glTFWriter::writeSkins()
{
  // TODO: NYI
}

//-----------------------------------------------------------------------------

void glTFWriter::writeTextures(glTFMaterialMap& materialMap)
{
#if defined USE_RAPIDJSON
  Standard_ProgramError_Raise_if(m_jsonWriter.get() == nullptr, "Internal error: gltf_XdeWriter::writeTextures()");

  // empty section should not be written to avoid validator errors
  bool isStarted = false;
  //
  t_Meshes2Primitives::Iterator itNodes2Primitives(m_dataProvider->GetSceneMeshes());
  for (; itNodes2Primitives.More(); itNodes2Primitives.Next())
  {
    NCollection_Vector<glTFPrimitive>::Iterator itPrm(itNodes2Primitives.Value());
    for (; itPrm.More(); itPrm.Next())
    {
      materialMap.AddTextures( m_jsonWriter.get(), itPrm.Value().Style, isStarted );
    }
  }
  if ( isStarted )
  {
    m_jsonWriter->EndArray();
  }
#else
  // Suppress `unreferenced formal parameter` warning (C4100).
  (void) materialMap;

  m_progress.SendLogMessage(LogErr(High) << "glTF export is impossible: you have to build "
                                            "Analysis Situs with rapidjson.");
#endif
}
