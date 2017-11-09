//-----------------------------------------------------------------------------
// Created on: 21 March 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef asiAlgo_FeatureAttrAngle_h
#define asiAlgo_FeatureAttrAngle_h

// Feature includes
#include <asiAlgo_FeatureAttrAdjacency.h>
#include <asiAlgo_FeatureAngle.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiAlgo_FeatureAttrAngle, asiAlgo_FeatureAttrAdjacency)

//! Attribute storing information about feature angle between faces.
class asiAlgo_FeatureAttrAngle : public asiAlgo_FeatureAttrAdjacency
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_FeatureAttrAngle, asiAlgo_FeatureAttrAdjacency)

public:

  //! Creates feature angle attribute.
  asiAlgo_FeatureAttrAngle()
  : asiAlgo_FeatureAttrAdjacency(), m_angle(Angle_Undefined)
  {}

  //! Creates feature angle attribute.
  //! \param[in] angleProp angle property.
  asiAlgo_FeatureAttrAngle(const asiAlgo_FeatureAngle angleProp)
  : asiAlgo_FeatureAttrAdjacency(), m_angle(angleProp)
  {}

  //! Creates feature angle attribute with common edges.
  //! \param[in] a           angle property.
  //! \param[in] commonEdges common edges.
  asiAlgo_FeatureAttrAngle(const asiAlgo_FeatureAngle               a,
                           const TopTools_IndexedMapOfShape& commonEdges)
  : asiAlgo_FeatureAttrAdjacency(commonEdges), m_angle(a)
  {}

public:

  //! \return static GUID associated with this type of attribute.
  static const Standard_GUID& GUID()
  {
    static Standard_GUID guid("ADCC7A78-E062-4CE2-B394-7F25DDFD555E");
    return guid;
  }

  //! \return GUID associated with this type of attribute.
  virtual const Standard_GUID& GetGUID() const
  {
    return GUID();
  }

public:

  //! \return angle between faces.
  asiAlgo_FeatureAngle GetAngle() const { return m_angle; }

  //! Sets angle between faces.
  //! \param[in] a the angle value.
  void SetAngle(const asiAlgo_FeatureAngle a) { m_angle = a; }

protected:

  asiAlgo_FeatureAngle m_angle; //!< Angle property.

};

#endif
