//-----------------------------------------------------------------------------
// Created on: 24 August 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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

#ifndef asiAlgo_RepatchFaces_h
#define asiAlgo_RepatchFaces_h

// asiAlgo includes
#include <asiAlgo.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <BRepTools_History.hxx>
#include <TopoDS_Face.hxx>

//-----------------------------------------------------------------------------

//! \ingroup ASI_MODELING
//!
//! Utility to repatch faces.
class asiAlgo_RepatchFaces : public ActAPI_IAlgorithm
{
public:

  asiAlgo_EXPORT
    asiAlgo_RepatchFaces(const TopoDS_Shape&  shape,
                         ActAPI_ProgressEntry progress,
                         ActAPI_PlotterEntry  plotter);

public:

  asiAlgo_EXPORT bool
    Perform(const TColStd_PackedMapOfInteger& faceIds);

public:

  //! \return result shape.
  const TopoDS_Shape& GetResult() const
  {
    return m_result;
  }

  //! \return modification history.
  const Handle(BRepTools_History)& GetHistory() const
  {
    return m_history;
  }

  //! Sets the fairing coefficient [0,1].
  //! \param[in] lambda the coefficient to set.
  void SetFairingCoeff(const double lambda)
  {
    m_fLambda = lambda;
  }

protected:

  asiAlgo_EXPORT bool
    repatchGroup(const TColStd_PackedMapOfInteger& faceIds,
                 const TopTools_IndexedMapOfShape& facesMap,
                 TopoDS_Shape&                     newfFace);

protected:

  TopoDS_Shape              m_input;   //!< Master model.
  TopoDS_Shape              m_result;  //!< Result.
  Handle(BRepTools_History) m_history; //!< Modification history.
  double                    m_fLambda; //!< Fairing coefficient.

};

#endif
