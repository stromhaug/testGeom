//-----------------------------------------------------------------------------
// Created on: 01 October 2018
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

#ifndef asiAlgo_FindCrossEdges_h
#define asiAlgo_FindCrossEdges_h

// asiAlgo includes
#include <asiAlgo_AAG.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

//-----------------------------------------------------------------------------

//! \ingroup ASI_AFR
//!
//! Utility to detect cross edges.
class asiAlgo_FindCrossEdges : public ActAPI_IAlgorithm
{
  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_FindCrossEdges, ActAPI_IAlgorithm)

public:

  //! Constructor.
  //! \param[in] masterCAD full CAD model.
  //! \param[in] aag       AAG.
  //! \param[in] progress  Progress Notifier.
  //! \param[in] plotter   Imperative Plotter.
  asiAlgo_EXPORT
    asiAlgo_FindCrossEdges(const TopoDS_Shape&        masterCAD,
                           const Handle(asiAlgo_AAG)& aag,
                           ActAPI_ProgressEntry       progress,
                           ActAPI_PlotterEntry        plotter);

  //! Constructor.
  //! \param[in] aag      AAG.
  //! \param[in] progress Progress Notifier.
  //! \param[in] plotter  Imperative Plotter.
  asiAlgo_EXPORT
    asiAlgo_FindCrossEdges(const Handle(asiAlgo_AAG)& aag,
                           ActAPI_ProgressEntry       progress,
                           ActAPI_PlotterEntry        plotter);

public:

  //! Performs detection of cross edges for the given face.
  //! \param[in] face_idx        index of the face of interest.
  //! \param[in] smoothEdges     smooth edges.
  //! \param[in] springEdges     spring edges.
  //! \param[in] candidateRadius candidate radius.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    PerformForFace(const int                         face_idx,
                   const TopTools_IndexedMapOfShape& smoothEdges,
                   const TopTools_IndexedMapOfShape& springEdges,
                   const double                      candidateRadius);

public:

  //! \return detected cross edges.
  const TopTools_IndexedMapOfShape& GetResultEdges() const
  {
    return m_result.edges;
  }

  //! \return indices of the detected cross edges.
  const TColStd_PackedMapOfInteger& GetResultIndices() const
  {
    return m_result.ids;
  }

  //! Cleans the result.
  void ClearResult()
  {
    m_result.edges.Clear();
    m_result.ids.Clear();
  }

protected:

  //! Extracts IDs for the given topological edges.
  //! \param[in]  edges   edges to get the IDs for.
  //! \param[out] indices indices of the edges of interest.
  void getIds(const TopTools_IndexedMapOfShape& edges,
              TColStd_PackedMapOfInteger&       indices) const;

protected:

  //-------------------------------------------------------------------------//
  // IN
  //-------------------------------------------------------------------------//

  TopoDS_Shape        m_master; //!< Master CAD model.
  Handle(asiAlgo_AAG) m_aag;    //!< AAG.

  //-------------------------------------------------------------------------//
  // OUT
  //-------------------------------------------------------------------------//

  struct
  {
    TopTools_IndexedMapOfShape edges; //!< Detected edges.
    TColStd_PackedMapOfInteger ids;   //!< Indices of the detected edges.
  } m_result;

};

#endif
