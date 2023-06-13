//-----------------------------------------------------------------------------
// Created on: 12 May 2023
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

#ifndef asiAlgo_IntersectCurves_h
#define asiAlgo_IntersectCurves_h

// asiAlgo includes
#include <asiAlgo.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <BRepTools_History.hxx>
#include <Geom_BSplineCurve.hxx>
#include <math_Matrix.hxx>
#include <TopoDS_Edge.hxx>

//-----------------------------------------------------------------------------

//! Utility to intersect curves.
class asiAlgo_IntersectCurves : public ActAPI_IAlgorithm
{
  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_IntersectCurves, ActAPI_IAlgorithm)

public:

  //! Ctor.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_IntersectCurves(ActAPI_ProgressEntry progress = nullptr,
                            ActAPI_PlotterEntry  plotter  = nullptr);

public:

  //! Finds intersections between the passed curves.
  asiAlgo_EXPORT bool
    Perform(const std::vector<Handle(Geom_BSplineCurve)>& uCurves,
            const std::vector<Handle(Geom_BSplineCurve)>& vCurves,
            math_Matrix&                                  uParams,
            math_Matrix&                                  vParams);

protected:

  asiAlgo_EXPORT void
    getEdgeParameters(const TopoDS_Edge&               edge,
                      const Handle(BRepTools_History)& history,
                      std::vector<double>&             params);

};

#endif