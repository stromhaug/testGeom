//-----------------------------------------------------------------------------
// Created on: 24 December 2020
//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Sergey Slyadnev
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

#ifndef asiAlgo_BuildHLR_h
#define asiAlgo_BuildHLR_h

// asiAlgo includes
#include <asiAlgo.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

//-----------------------------------------------------------------------------

//! Performs hidden line removal for the input shape. The result is returned
//! as a compound of edges representing the extracted feature lines.
class asiAlgo_BuildHLR : public ActAPI_IAlgorithm
{
  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_BuildHLR, ActAPI_IAlgorithm)

public:

  //! HLR mode.
  enum Mode
  {
    Mode_Precise = 0, //!< Precise.
    Mode_Discrete     //!< Discrete.
  };

  //! Settings to control which types of edges to output
  struct t_outputEdges
  {
    bool OutputVisibleSharpEdges;
    bool OutputVisibleSmoothEdges;
    bool OutputVisibleOutlineEdges;
    bool OutputVisibleSewnEdges;
    bool OutputVisibleIsoLines;
    bool OutputHiddenSharpEdges;
    bool OutputHiddenSmoothEdges;
    bool OutputHiddenOutlineEdges;
    bool OutputHiddenSewnEdges;
    bool OutputHiddenIsoLines;

    t_outputEdges()
    : OutputVisibleSharpEdges   (true),
      OutputVisibleSmoothEdges  (true),
      OutputVisibleOutlineEdges (true),
      OutputVisibleSewnEdges    (true),
      OutputVisibleIsoLines     (true),
      OutputHiddenSharpEdges    (false),
      OutputHiddenSmoothEdges   (false),
      OutputHiddenOutlineEdges  (false),
      OutputHiddenSewnEdges     (false),
      OutputHiddenIsoLines      (false)
    {}
  };

  //! Data to pass to a thread function.
  struct t_threadData
  {
    TopoDS_Shape  input;
    gp_Dir        dir;
    t_outputEdges style;
    TopoDS_Shape  output;

    t_threadData() = default;
  };

public:

  //! Ctor.
  //! \param[in] shape    B-rep shape of a CAD part to analyze.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_BuildHLR(const TopoDS_Shape&  shape,
                     ActAPI_ProgressEntry progress = nullptr,
                     ActAPI_PlotterEntry  plotter  = nullptr);

public:

  //! Performs HLR.
  //! \param[in] projectionDir the direction of projection to use.
  //! \param[in] mode          the HLR computation mode (precise is the default).
  //! \param[in] visibility    the projection styles.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Perform(const gp_Dir&        projectionDir,
            const Mode           mode       = Mode_Precise,
            const t_outputEdges& visibility = t_outputEdges());

  //! Runs HLR in parallel threads.
  //! \param[in] projectionDir the direction of projection to use.
  //! \param[in] timeout       the timeout for processing.
  //! \param[in] visibility    the projection styles.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    PerformParallel(const gp_Dir&        projectionDir,
                    const int            timeout_ms = 500,
                    const t_outputEdges& visibility = t_outputEdges());

public:

  //! \return the extracted feature lines.
  asiAlgo_EXPORT const TopoDS_Shape&
    GetResult() const;

protected:

  TopoDS_Shape m_input;  //!< Input shape.
  TopoDS_Shape m_result; //!< Result shape.

public:

  static t_threadData ThreadData[2];

};

#endif
