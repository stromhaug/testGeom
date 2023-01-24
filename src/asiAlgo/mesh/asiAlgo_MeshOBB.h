//-----------------------------------------------------------------------------
// Created on: 16 February 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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

#ifndef asiAlgo_MeshOBB_h
#define asiAlgo_MeshOBB_h

// asiAlgo includes
#include <asiAlgo_OBB.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <gp_Ax3.hxx>
#include <TopoDS_Solid.hxx>

//-----------------------------------------------------------------------------

//! Utility to build Oriented Bounding Box on mesh by finding eigen vectors
//! of a covariance matrix.
class asiAlgo_MeshOBB : public ActAPI_IAlgorithm
{
  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_MeshOBB, ActAPI_IAlgorithm)

public:

  //! Constructor.
  //! \param[in] mesh     the triangulation to build an OBB for.
  //! \param[in] progress the progress notifier.
  //! \param[in] plotter  the imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_MeshOBB(const Handle(Poly_Triangulation)& mesh,
                    ActAPI_ProgressEntry              progress = nullptr,
                    ActAPI_PlotterEntry               plotter = nullptr);

  //! Constructor.
  //! \param[in] part     the input part.
  //! \param[in] progress the progress entry.
  //! \param[in] plotter  the imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_MeshOBB(const TopoDS_Shape&  part,
                    ActAPI_ProgressEntry progress = nullptr,
                    ActAPI_PlotterEntry  plotter  = nullptr);

public:

  //! Builds OBB.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Perform();

public:

  //! \return resulting OBB.
  asiAlgo_EXPORT const asiAlgo_OBB&
    GetResult() const;

  //! \return resulting transformation matrix which defines the placement
  //!         of the oriented box.
  asiAlgo_EXPORT gp_Trsf
    GetResultTrsf() const;

  //! \return resulting OBB as a B-Rep box.
  asiAlgo_EXPORT TopoDS_Shape
    GetResultBox() const;

protected:

  //! Calculates local axes by covariance analysis on mesh.
  //! \param[out] xAxis      X axis.
  //! \param[out] yAxis      Y axis.
  //! \param[out] zAxis      Z axis.
  //! \param[out] meanVertex central vertex.
  void calculateByCovariance(gp_Ax1& xAxis,
                             gp_Ax1& yAxis,
                             gp_Ax1& zAxis,
                             gp_XYZ& meanVertex) const;

protected:

  Handle(Poly_Triangulation) m_input; //!< Input triangulation.
  asiAlgo_OBB                m_obb;   //!< Constructed OBB.

};

#endif
