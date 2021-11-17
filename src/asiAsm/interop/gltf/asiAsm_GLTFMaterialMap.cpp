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
#include <asiAsm_GLTFMaterialMap.h>

// GlTF includes
#if defined USE_RAPIDJSON
  #include <asiAsm_GLTFJsonSerializer.h>
#endif

using namespace asiAsm::xde;

//-----------------------------------------------------------------------------

const Handle(Image_Texture)&
  glTFMaterialMap::baseColorTexture(const Handle(glTFMaterialAttr)& mat)
{
  static const Handle(Image_Texture) NullTexture;

  if ( mat.IsNull() )
  {
    return NullTexture;
  }

  if ( mat->HasPbrMaterial() && !mat->PbrMaterial().BaseColorTexture.IsNull() )
  {
    return mat->PbrMaterial().BaseColorTexture;
  }

  if ( mat->HasCommonMaterial() && !mat->CommonMaterial().DiffuseTexture.IsNull() )
  {
    return mat->CommonMaterial().DiffuseTexture;
  }

  return NullTexture;
}

//-----------------------------------------------------------------------------

glTFMaterialMap::glTFMaterialMap(const TCollection_AsciiString& filename,
                                 const int                      defSamplerId)
: glTFMaterialMapBase (filename),
  myWriter             (nullptr),
  myDefSamplerId       (defSamplerId),
  myNbImages           (0)
{
  myMatNameAsKey = false;
}

//-----------------------------------------------------------------------------

glTFMaterialMap::~glTFMaterialMap()
{}

//-----------------------------------------------------------------------------

void glTFMaterialMap::AddImages(glTFJsonSerializer*       writer,
                                const glTFXdeVisualStyle& style,
                                bool&                     isStarted)
{
  if ( writer == nullptr || style.GetMaterial().IsNull()|| style.GetMaterial()->IsEmpty() )
  {
    return;
  }

  this->addImage(writer, this->baseColorTexture( style.GetMaterial() ),               isStarted);
  this->addImage(writer, style.GetMaterial()->PbrMaterial().MetallicRoughnessTexture, isStarted);
  this->addImage(writer, style.GetMaterial()->PbrMaterial().NormalTexture,            isStarted);
  this->addImage(writer, style.GetMaterial()->PbrMaterial().EmissiveTexture,          isStarted);
  this->addImage(writer, style.GetMaterial()->PbrMaterial().OcclusionTexture,         isStarted);
}

//-----------------------------------------------------------------------------

void glTFMaterialMap::addImage(glTFJsonSerializer*          writer,
                               const Handle(Image_Texture)& texture,
                               bool&                        isStarted)
{
#if defined USE_RAPIDJSON
  if ( texture.IsNull() || myImageMap.IsBound1(texture) || myImageFailMap.Contains(texture) )
  {
    return;
  }

  TCollection_AsciiString aGltfImgKey = myNbImages;
  ++myNbImages;
  for (; myImageMap.IsBound2 (aGltfImgKey); ++myNbImages)
  {
    aGltfImgKey = myNbImages;
  }

  TCollection_AsciiString aTextureUri;
  if (!CopyTexture (aTextureUri, texture, aGltfImgKey))
  {
    myImageFailMap.Add (texture);
    return;
  }

  myImageMap.Bind (texture, aGltfImgKey);

  if (!isStarted)
  {
    writer->Key (glTFRootElementName (glTFRootElement_Images));
    writer->StartArray();
    isStarted = true;
  }

  writer->StartObject();
  {
    writer->Key ("uri");
    writer->String (aTextureUri.ToCString());
  }
  writer->EndObject();
#else
  // Suppress `unreferenced formal parameter` warning (C4100).
  (void) writer;
  (void) texture;
  (void) isStarted;
#endif
}

//-----------------------------------------------------------------------------

void glTFMaterialMap::AddMaterial(glTFJsonSerializer*       theWriter,
                                  const glTFXdeVisualStyle& theStyle,
                                  bool&                     theIsStarted)
{
#if defined USE_RAPIDJSON
  if (theWriter == NULL
   || ((theStyle.GetMaterial().IsNull() || theStyle.GetMaterial()->IsEmpty())
    && !(theStyle.IsSetColorSurf() || theStyle.IsSetColorCurve())))
  {
    return;
  }

  if (!theIsStarted)
  {
    theWriter->Key (glTFRootElementName (glTFRootElement_Materials));
    theWriter->StartArray();
    theIsStarted = true;
  }
  myWriter = theWriter;
  AddMaterial (theStyle);
  myWriter = NULL;
#else
  // Suppress `unreferenced formal parameter` warning (C4100).
  (void) theWriter;
  (void) theStyle;
  (void) theIsStarted;
#endif
}

//-----------------------------------------------------------------------------

void glTFMaterialMap::AddTextures(glTFJsonSerializer*       theWriter,
                                  const glTFXdeVisualStyle& theStyle,
                                  bool&                     theIsStarted)
{
#if defined USE_RAPIDJSON
  if (theWriter == NULL
   || theStyle.GetMaterial().IsNull()
   || theStyle.GetMaterial()->IsEmpty())
  {
    return;
  }

  addTexture (theWriter, baseColorTexture (theStyle.GetMaterial()), theIsStarted);
  addTexture (theWriter, theStyle.GetMaterial()->PbrMaterial().MetallicRoughnessTexture, theIsStarted);
  addTexture (theWriter, theStyle.GetMaterial()->PbrMaterial().NormalTexture,    theIsStarted);
  addTexture (theWriter, theStyle.GetMaterial()->PbrMaterial().EmissiveTexture,  theIsStarted);
  addTexture (theWriter, theStyle.GetMaterial()->PbrMaterial().OcclusionTexture, theIsStarted);
#else
  // Suppress `unreferenced formal parameter` warning (C4100).
  (void) theWriter;
  (void) theStyle;
  (void) theIsStarted;
#endif
}

//-----------------------------------------------------------------------------

void glTFMaterialMap::addTexture(glTFJsonSerializer*          theWriter,
                                 const Handle(Image_Texture)& theTexture,
                                 bool&                        theIsStarted)
{
#if defined USE_RAPIDJSON
  if (theTexture.IsNull()
  ||  myTextureMap.Contains (theTexture)
  || !myImageMap  .IsBound1 (theTexture))
  {
    return;
  }

  const TCollection_AsciiString anImgKey = myImageMap.Find1 (theTexture);
  myTextureMap.Add (theTexture);
  if (anImgKey.IsEmpty())
  {
    return;
  }

  if (!theIsStarted)
  {
    theWriter->Key (glTFRootElementName (glTFRootElement_Textures));
    theWriter->StartArray();
    theIsStarted = true;
  }

  theWriter->StartObject();
  {
    theWriter->Key ("sampler");
    theWriter->Int (myDefSamplerId); // mandatory field by specs
    theWriter->Key ("source");
    theWriter->Int (anImgKey.IntegerValue());
  }
  theWriter->EndObject();
#else
  // Suppress `unreferenced formal parameter` warning (C4100).
  (void) theWriter;
  (void) theTexture;
  (void) theIsStarted;
#endif
}

//-----------------------------------------------------------------------------

TCollection_AsciiString
  glTFMaterialMap::AddMaterial(const glTFXdeVisualStyle& theStyle)
{
  return glTFMaterialMapBase::AddMaterial(theStyle);
}

//-----------------------------------------------------------------------------

void glTFMaterialMap::DefineMaterial(const glTFXdeVisualStyle&      theStyle,
                                     const TCollection_AsciiString& /*theKey*/,
                                     const TCollection_AsciiString& theName)
{
#if defined USE_RAPIDJSON
  if (myWriter == NULL)
  {
    Standard_ProgramError::Raise ("glTFMaterialMap::DefineMaterial() should be called with JSON Writer");
    return;
  }

  glTFMaterialPbr aPbrMat;
  const bool hasMaterial = !theStyle.GetMaterial().IsNull()
                        && !theStyle.GetMaterial()->IsEmpty();
  if (hasMaterial)
  {
    aPbrMat = theStyle.GetMaterial()->ConvertToPbrMaterial();
  }
  else if (!myDefaultStyle.GetMaterial().IsNull()
         && myDefaultStyle.GetMaterial()->HasPbrMaterial())
  {
    aPbrMat = myDefaultStyle.GetMaterial()->PbrMaterial();
  }
  if (theStyle.IsSetColorSurf())
  {
    aPbrMat.BaseColor.SetRGB (theStyle.GetColorSurf());
    if (theStyle.GetColorSurfRGBA().Alpha() < 1.0f)
    {
      aPbrMat.BaseColor.SetAlpha (theStyle.GetColorSurfRGBA().Alpha());
    }
  }
  else if (theStyle.IsSetColorCurve())
  {
    aPbrMat.BaseColor.SetRGB(theStyle.GetColorCurve());
  }
  myWriter->StartObject();
  {
    myWriter->Key ("name");
    myWriter->String (theName.ToCString());

    myWriter->Key ("pbrMetallicRoughness");
    myWriter->StartObject();
    {
      myWriter->Key ("baseColorFactor");
      myWriter->StartArray();
      {
        myWriter->Double (aPbrMat.BaseColor.GetRGB().Red());
        myWriter->Double (aPbrMat.BaseColor.GetRGB().Green());
        myWriter->Double (aPbrMat.BaseColor.GetRGB().Blue());
        myWriter->Double (aPbrMat.BaseColor.Alpha());
      }
      myWriter->EndArray();

      if (const Handle(Image_Texture)& aBaseTexture = baseColorTexture (theStyle.GetMaterial()))
      {
        if (myImageMap.IsBound1 (aBaseTexture))
        {
          myWriter->Key ("baseColorTexture");
          myWriter->StartObject();
          {
            myWriter->Key ("index");
            const TCollection_AsciiString& anImageIdx = myImageMap.Find1 (aBaseTexture);
            if (!anImageIdx.IsEmpty())
            {
              myWriter->Int (anImageIdx.IntegerValue());
            }
          }
          myWriter->EndObject();
        }
      }

      if (hasMaterial
       || aPbrMat.Metallic != 1.0f)
      {
        myWriter->Key ("metallicFactor");
        myWriter->Double (aPbrMat.Metallic);
      }

      if (!aPbrMat.MetallicRoughnessTexture.IsNull()
        && myImageMap.IsBound1 (aPbrMat.MetallicRoughnessTexture))
      {
        myWriter->Key ("metallicRoughnessTexture");
        myWriter->StartObject();
        {
          myWriter->Key ("index");
          const TCollection_AsciiString& anImageIdx = myImageMap.Find1 (aPbrMat.MetallicRoughnessTexture);
          if (!anImageIdx.IsEmpty())
          {
            myWriter->Int (anImageIdx.IntegerValue());
          }
        }
        myWriter->EndObject();
      }

      if (hasMaterial
       || aPbrMat.Roughness != 1.0f)
      {
        myWriter->Key ("roughnessFactor");
        myWriter->Double (aPbrMat.Roughness);
      }
    }
    myWriter->EndObject();

    if (theStyle.GetMaterial().IsNull()
     || theStyle.GetMaterial()->IsDoubleSided())
    {
      myWriter->Key ("doubleSided");
      myWriter->Bool (true);
    }

    const Graphic3d_AlphaMode anAlphaMode = !theStyle.GetMaterial().IsNull() ? theStyle.GetMaterial()->AlphaMode() : Graphic3d_AlphaMode_BlendAuto;
    switch (anAlphaMode)
    {
      case Graphic3d_AlphaMode_BlendAuto:
      {
        if (aPbrMat.BaseColor.Alpha() < 1.0f)
        {
          myWriter->Key ("alphaMode");
          myWriter->String ("BLEND");
        }
        break;
      }
      case Graphic3d_AlphaMode_Opaque:
      {
        break;
      }
      case Graphic3d_AlphaMode_Mask:
      {
        myWriter->Key ("alphaMode");
        myWriter->String ("MASK");
        break;
      }
      case Graphic3d_AlphaMode_Blend:
      {
        myWriter->Key ("alphaMode");
        myWriter->String ("BLEND");
        break;
      }
    }
    if (!theStyle.GetMaterial().IsNull()
      && theStyle.GetMaterial()->AlphaCutOff() != 0.5f)
    {
      myWriter->Key ("alphaCutoff");
      myWriter->Double (theStyle.GetMaterial()->AlphaCutOff());
    }

    if ( !aPbrMat.EmissiveFactor.IsEqual( Graphic3d_Vec3 (0.0f, 0.0f, 0.0f) ) )
    {
      myWriter->Key ("emissiveFactor");
      myWriter->StartArray();
      {
        myWriter->Double (aPbrMat.EmissiveFactor.r());
        myWriter->Double (aPbrMat.EmissiveFactor.g());
        myWriter->Double (aPbrMat.EmissiveFactor.b());
      }
      myWriter->EndArray();
    }
    if (!aPbrMat.EmissiveTexture.IsNull()
      && myImageMap.IsBound1 (aPbrMat.EmissiveTexture))
    {
      myWriter->Key ("emissiveTexture");
      myWriter->StartObject();
      {
        myWriter->Key ("index");
        const TCollection_AsciiString& anImageIdx = myImageMap.Find1 (aPbrMat.EmissiveTexture);
        if (!anImageIdx.IsEmpty())
        {
          myWriter->Int (anImageIdx.IntegerValue());
        }
      }
      myWriter->EndObject();
    }

    if (!aPbrMat.NormalTexture.IsNull()
      && myImageMap.IsBound1 (aPbrMat.NormalTexture))
    {
      myWriter->Key ("normalTexture");
      myWriter->StartObject();
      {
        myWriter->Key ("index");
        const TCollection_AsciiString& anImageIdx = myImageMap.Find1 (aPbrMat.NormalTexture);
        if (!anImageIdx.IsEmpty())
        {
          myWriter->Int (anImageIdx.IntegerValue());
        }
      }
      myWriter->EndObject();
    }

    if (!aPbrMat.OcclusionTexture.IsNull()
      && myImageMap.IsBound1 (aPbrMat.OcclusionTexture))
    {
      myWriter->Key ("occlusionTexture");
      myWriter->StartObject();
      {
        myWriter->Key ("index");
        const TCollection_AsciiString& anImageIdx = myImageMap.Find1 (aPbrMat.OcclusionTexture);
        if (!anImageIdx.IsEmpty())
        {
          myWriter->Int (anImageIdx.IntegerValue());
        }
      }
      myWriter->EndObject();
    }
  }
  myWriter->EndObject();
#else
  // Suppress `unreferenced formal parameter` warning (C4100).
  (void) theStyle;
  (void) theName;
#endif
}