//-----------------------------------------------------------------------------
// Created on: 05 September 2017
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

#ifndef asiAlgo_TopoKill_h
#define asiAlgo_TopoKill_h

// asiAlgo includes
#include <asiAlgo.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <NCollection_IndexedMap.hxx>
#include <TopTools_ShapeMapHasher.hxx>

//! Utility for killing topological entities and reduction of topological
//! graph. Such reduction is possible in two ways:
//!
//! - Killing a sub-shape with all its occurrences in the parent shapes.
//! - Replacement of a sub-shape of a certain type with another (existing)
//!   sub-shape of the same type.
//!
//! To use this tool, the client code first invokes AskRemove() or
//! AskReplace() methods which formulate the intention of the client code.
//! Each request may return true of false depending on whether the request
//! is accepted or rejected by the tool. E.g. a request can be rejected if
//! it does not lead to topological reduction. Once the intention is
//! formulated, the client code invokes Apply() method to perform the
//! recorded modification.
class asiAlgo_TopoKill : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_TopoKill, ActAPI_IAlgorithm)

public:

  //! Constructor.
  //! \param[in] masterCAD full CAD model.
  //! \param[in] progress  Progress Notifier.
  //! \param[in] plotter   Imperative Plotter.
  asiAlgo_EXPORT
    asiAlgo_TopoKill(const TopoDS_Shape&  masterCAD,
                     ActAPI_ProgressEntry progress,
                     ActAPI_PlotterEntry  plotter);

public:

  //! Asks for removal of the given sub-shape.
  //! \param[in] subshape sub-shape to remove.
  //! \return true if the request is accepted, false -- otherwise.
  asiAlgo_EXPORT bool
    AskRemove(const TopoDS_Shape& subshape);

  //! Asks for replacement of the given sub-shape {what} with the given
  //! sub-shape {with}. Orientations in the parent shapes are preserved.
  //! \param[in] what sub-shape to replace.
  //! \param[in] with replacement sub-shape.
  //! \return true if the request is accepted, false -- otherwise.
  asiAlgo_EXPORT bool
    AskReplace(const TopoDS_Shape& what,
               const TopoDS_Shape& with);

  //! Checks whether the passed sub-shape is asked either for removal or
  //! for replacement. If so, one of the output Boolean flags is switched on
  //! to precise what actually is asked.
  //!
  //! \param[in]  subshape         sub-shape to check.
  //! \param[in]  checkRecursively indicates whether to check recursively
  //!                              the sub-shapes of the given shape.
  //! \param[out] forRemoval       indicates whether removal is asked.
  //! \param[out] forReplacement   indicates whether replacement is asked.
  //! \return true if anything is asked for the given sub-shape (and optionally
  //!         its children), false -- otherwise.
  asiAlgo_EXPORT bool
    IsAsked(const TopoDS_Shape& subshape,
            const bool          checkRecursively,
            bool&               forRemoval,
            bool&               forReplacement) const;

  //! Checks whether the passed sub-shape is asked either for removal or
  //! for replacement. If so, one of the output Boolean flags is switched on
  //! to precise what actually is asked.
  //!
  //! \param[in]  subshape       sub-shape to check.
  //! \param[out] forRemoval     indicates whether removal is asked.
  //! \param[out] forReplacement indicates whether replacement is asked.
  //! \return true if anything is asked for the given sub-shape,
  //!         false -- otherwise.
  asiAlgo_EXPORT bool
    IsAsked(const TopoDS_Shape& subshape,
            bool&               forRemoval,
            bool&               forReplacement) const;

public:

  //! Performs topological change.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Apply();

public:

  //! \return converted model.
  const TopoDS_Shape& GetResult() const
  {
    return m_result;
  }

protected:

  //! Recursive routine which builds a new topological graph in bottom-up
  //! fashion. The bottom-up construction avoids modification of topological
  //! entities (as they are created and then just added to their direct
  //! parents) which is prohibited in OCCT. The routine recreates all
  //! top-level structural topological elements (compounds, solids, shells)
  //! and reuses all boundary elements starting from faces. The faces whose
  //! indices are enumerated in the passed collection are inverted.
  //!
  //! \param[in]  root   root topological element to iterate recursively.
  //! \param[out] result newly created topological element.
  asiAlgo_EXPORT void
    buildTopoGraphLevel(const TopoDS_Shape& root,
                        TopoDS_Shape&       result) const;

// Input and output:
protected:

  TopoDS_Shape m_master; //!< Master CAD model.
  TopoDS_Shape m_result; //!< Result CAD model.

// Requests:
protected:

  // NOTICE: in both maps below, an unoriented hasher is used. This is because
  //         the nodes in a formal topology graph do not encode orientations.
  //         In formal topology graph, orientation is a property of an arc, i.e.
  //         it is a property of INCLUSION of one shape into another. Since
  //         in the topological killer we work only with nodes and preserve
  //         orientations, we do not need to distinguish between differently
  //         oriented entities.

  //! Sub-shapes to remove.
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> m_toRemove;

  //! Sub-shapes to replace.
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> m_toReplace;

};

#endif
