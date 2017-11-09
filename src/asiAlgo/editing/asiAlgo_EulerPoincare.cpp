//-----------------------------------------------------------------------------
// Created on: 21 July 2016
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

// Own include
#include <asiAlgo_EulerPoincare.h>

// Geometry includes
#include <asiAlgo_Utils.h>

//! Checks the passed shape against Euler-Poincare formula. Notice that
//! genus of the manifold has to be selected by user as we have no idea
//! how to perform this tricky genus analysis.
//! \param shape [in] shape to check.
//! \param genus [in] genus of the manifold.
//! \return true if the check has passed, false -- otherwise.
bool asiAlgo_EulerPoincare::Check(const TopoDS_Shape& shape,
                                  const int           genus)
{
  // Summary
  int nbCompsolids = 0,
      nbCompounds  = 0,
      nbSolids     = 0,
      nbShells     = 0,
      nbFaces      = 0,
      nbWires      = 0,
      nbOuterWires = 0,
      nbInnerWires = 0,
      nbEdges      = 0,
      nbDegenEdges = 0,
      nbVertexes   = 0;

  // Extract summary
  asiAlgo_Utils::ShapeSummary(shape,
                           nbCompsolids,
                           nbCompounds,
                           nbSolids,
                           nbShells,
                           nbFaces,
                           nbWires,
                           nbOuterWires,
                           nbInnerWires,
                           nbEdges,
                           nbDegenEdges,
                           nbVertexes);

  // Prepare output string with the gathered summary
  std::cout << "Shape summary:\n";
  std::cout << "- nb compsolids: "        << nbCompsolids << "\n";
  std::cout << "- nb compounds: "         << nbCompounds  << "\n";
  std::cout << "- nb solids: "            << nbSolids     << "\n";
  std::cout << "- nb shells (s): "        << nbShells     << "\n";
  std::cout << "- nb faces (f): "         << nbFaces      << "\n";
  std::cout << "- nb wires: "             << nbWires      << "\n";
  std::cout << "\t- nb outer wires: "     << nbOuterWires << "\n";
  std::cout << "\t- nb inner wires (r): " << nbInnerWires << "\n";
  std::cout << "- nb edges (e - degen): " << nbEdges      << "\n";
  std::cout << "\t- nb degen edges: "     << nbDegenEdges << "\n";
  std::cout << "- nb vertices (v): "      << nbVertexes   << "\n";

  const int v = nbVertexes;
  const int e = nbEdges - nbDegenEdges;
  const int f = nbFaces;
  const int s = nbShells;
  const int h = genus;
  const int r = nbInnerWires;

  const int E = v - e + f - 2*(s - h) - r;

  std::cout << "Euler number (E = v - e + f - 2*(s - h) - r): " << E << std::endl;
  return (E == 0);
}
