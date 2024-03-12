//-----------------------------------------------------------------------------
// Created on: 07 March 2024
//-----------------------------------------------------------------------------
// Copyright (c) 2024-present, Julia Slyadneva
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

#pragma once

// asiAsm includes
#include <asiAsm_XdeDoc.h>

// asiAlgo includes
#include <asiAlgo.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

//-----------------------------------------------------------------------------

namespace asiAsm {
namespace xde {

//! \ingroup ASIASM
//!
//! Breaks down a CAD assembly into a plain list of parts with BOM.
class BreakDown : public ActAPI_IAlgorithm
{
  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(BreakDown, ActAPI_IAlgorithm)

public:

  //! Constructor.
  //! \param[in] progress the progress entry.
  //! \param[in] plotter  the imperative plotter.
  asiAsm_EXPORT
    BreakDown(ActAPI_ProgressEntry progress = nullptr,
              ActAPI_PlotterEntry  plotter  = nullptr);

public:

  //! Sets compound expansion mode on/off.
  void SetExpandCompounds(const bool on)
  {
    m_bExpandCompounds = on;
  }

  //! Sets filename prefix to use.
  void SetFilenamePrefix(const t_asciiString& prefix)
  {
    m_prefix = prefix;
  }

public:

  //! Breaks down the passed assembly.
  //! \return true in case of success, false -- otherwise.
  asiAsm_EXPORT bool
    Perform(const Handle(Doc)&   xdeDoc,
            const t_asciiString& outputDir);

private:

  //! Indicates whether to expand compound parts down to their constituent
  //! non-compound shapes.
  bool m_bExpandCompounds;

  //! Sets the prefix to use for the destination files per parts.
  t_asciiString m_prefix;

};

}
}
