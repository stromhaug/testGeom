//-----------------------------------------------------------------------------
// Created on: 06 October 2018
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
//    * Neither the name of Sergey Slyadnev nor the
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

#ifndef asiEngine_RE_h
#define asiEngine_RE_h

// asiEngine includes
#include <asiEngine_Base.h>

//! API for the data model operations related to reverse engineering.
class asiEngine_RE : public asiEngine_Base
{
public:

  //! ctor.
  //! \param[in] model    Data Model instance.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiEngine_RE(const Handle(asiEngine_Model)& model,
               ActAPI_ProgressEntry           progress,
               ActAPI_PlotterEntry            plotter)
  : asiEngine_Base(model, progress, plotter) {}

public:

  //! \return newly created Topology Node.
  asiEngine_EXPORT Handle(asiData_ReTopoNode)
    Create_Topo();

  //! Creates Contour Node under the given ReTopology Node.
  //! \return newly created active Contour Node.
  asiEngine_EXPORT Handle(asiData_ContourNode)
    Create_Contour(const Handle(asiData_ReTopoNode)& topo_n);

  //! Creates RePatches Node under the given ReTopology Node.
  //! \return newly created Node.
  asiEngine_EXPORT Handle(asiData_RePatchesNode)
    Create_Patches(const Handle(asiData_ReTopoNode)& topo_n);

  //! Creates ReEdges Node under the given ReTopology Node.
  //! \return newly created Node.
  asiEngine_EXPORT Handle(asiData_ReEdgesNode)
    Create_Edges(const Handle(asiData_ReTopoNode)& topo_n);

  //! Creates ReVertices Node under the given ReTopology Node.
  //! \return newly created Node.
  asiEngine_EXPORT Handle(asiData_ReVerticesNode)
    Create_Vertices(const Handle(asiData_ReTopoNode)& topo_n);

public:

  asiEngine_EXPORT Handle(asiData_ContourNode)
    GetOrCreate_Contour();

};

#endif
