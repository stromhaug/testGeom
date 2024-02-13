//-----------------------------------------------------------------------------
// Created on: 04 November 2023
//-----------------------------------------------------------------------------
// Copyright (c) 2023-present, Sergey Slyadnev
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

#ifndef asiAlgo_CheckContour_HeaderFile
#define asiAlgo_CheckContour_HeaderFile

// asiAlgo includes
#include <asiAlgo.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <TopoDS_Wire.hxx>

//-----------------------------------------------------------------------------

//! \ingroup ASI_MODELING
//!
//! Contour checking tool.
class asiAlgo_CheckContour : public ActAPI_IAlgorithm
{
public:

  //! Ctor.
  asiAlgo_EXPORT
    asiAlgo_CheckContour(ActAPI_ProgressEntry progress = nullptr,
                         ActAPI_PlotterEntry  plotter  = nullptr);

public:

  //! Returns true if the passed contour is a single wire. Returns false
  //! otherwise.
  //! \param[in] Contour the contour to check.
  //! \return true/false.
  asiAlgo_EXPORT bool
    IsSingleWire(const TopoDS_Shape& Contour) const;

  //! Checks whether the passed contour is connected or not.
  //! \param[in] Contour        the contour to check.
  //! \param[in] doCheckSharing the Boolean flag that indicates whether to check topological
  //!                           sharing. If not, the passed precision is used to check
  //!                           geometric coincidence.
  //! \param[in] prec           the precision to check geometric coincidence.
  //! \return true/false.
  asiAlgo_EXPORT bool
    IsConnectedWire(const TopoDS_Shape& Contour,
                    const bool          doCheckSharing,
                    const double        prec) const;

};

#endif
