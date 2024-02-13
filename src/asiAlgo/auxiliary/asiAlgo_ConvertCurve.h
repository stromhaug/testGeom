//-----------------------------------------------------------------------------
// Copyright (c) 2022-present, Sergey Kiselev
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

#ifndef asiAlgo_ConvertCurve_HeaderFile
#define asiAlgo_ConvertCurve_HeaderFile

// asiAlgo includes
#include <asiAlgo.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <Geom_Curve.hxx>
#include <TopoDS_Wire.hxx>

//-----------------------------------------------------------------------------

//! \ingroup ASI_MODELING
//!
//! Functions to perform conversion of spline and ellipse curves to arcs and lines.
namespace asiAlgo_ConvertCurve
{
  asiAlgo_EXPORT void
    Convert2ArcLines(TopoDS_Shape&        shape,
                     double               tolerance = 0.001,
                     ActAPI_ProgressEntry progress  = nullptr,
                     ActAPI_PlotterEntry  plotter   = nullptr);

  asiAlgo_EXPORT void
    Convert2Polyline(const TopoDS_Wire&   wire,
                     std::vector<gp_Pnt>& points);

  asiAlgo_EXPORT void
    Convert2Polyline(const TopoDS_Wire& wire,
                     TopoDS_Wire&       polyWire);

  asiAlgo_EXPORT void
    Convert2Polyline(TopoDS_Shape& shape);

  asiAlgo_EXPORT double
    CheckMaxGap(const TopoDS_Shape&  shape,
                ActAPI_ProgressEntry progress = nullptr,
                ActAPI_PlotterEntry  plotter  = nullptr);

  asiAlgo_EXPORT double
    CheckGaps(const TopoDS_Wire&   w,
              ActAPI_ProgressEntry progress = nullptr,
              ActAPI_PlotterEntry  plotter  = nullptr);

  asiAlgo_EXPORT void
    FixGaps(TopoDS_Shape&        shape,
            ActAPI_ProgressEntry progress = nullptr,
            ActAPI_PlotterEntry  plotter  = nullptr);

  asiAlgo_EXPORT bool
    FixGaps(const TopoDS_Wire&   input,
            TopoDS_Wire&         result,
            ActAPI_ProgressEntry progress = nullptr,
            ActAPI_PlotterEntry  plotter  = nullptr);
}

#endif
