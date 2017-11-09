//-----------------------------------------------------------------------------
// Created on: 05 April 2016
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
#include <asiVisu_RESurfacesDataProvider.h>

// asiData includes
#include <asiData_RESurfaceNode.h>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param surfaces_n [in] Surfaces Node.
asiVisu_RESurfacesDataProvider::asiVisu_RESurfacesDataProvider(const Handle(asiData_RESurfacesNode)& surfaces_n)
: asiVisu_DataProvider(), m_surfaces(surfaces_n)
{}

//-----------------------------------------------------------------------------

//! \return ID of the associated Data Node.
ActAPI_DataObjectId asiVisu_RESurfacesDataProvider::GetNodeID() const
{
  if ( m_surfaces.IsNull() || !m_surfaces->IsWellFormed() )
    return ActAPI_DataObjectId();

  return m_surfaces->GetId();
}

//-----------------------------------------------------------------------------

//! \return number of stored surfaces.
int asiVisu_RESurfacesDataProvider::GetNumOfSurfaces() const
{
  if ( m_surfaces.IsNull() || !m_surfaces->IsWellFormed() )
    return 0;

  // Iterate over the child Nodes which are the individual surfaces
  int nbSurfaces = 0;
  for ( Handle(ActAPI_IChildIterator) cit = m_surfaces->GetChildIterator(); cit->More(); cit->Next() )
  {
    Handle(asiData_RESurfaceNode) surface_n = Handle(asiData_RESurfaceNode)::DownCast( cit->Value() );
    //
    if ( surface_n.IsNull() || !surface_n->IsWellFormed() )
      continue;

    ++nbSurfaces;
  }

  return nbSurfaces;
}

//! Returns surface by its one-based index.
//! \param oneBased_idx [in] 1-based index of the surface to access.
//! \param uLimit       [out] U limit.
//! \param vLimit       [out] V limit.
//! \return surface.
Handle(Geom_Surface)
  asiVisu_RESurfacesDataProvider::GetSurface(const int oneBased_idx,
                                             double&   uLimit,
                                             double&   vLimit) const
{
  if ( m_surfaces.IsNull() || !m_surfaces->IsWellFormed() )
    return NULL;

  // Access individual surface
  Handle(asiData_RESurfaceNode)
    surface_n = Handle(asiData_RESurfaceNode)::DownCast( m_surfaces->GetChildNode(oneBased_idx) );
  //
  if ( surface_n.IsNull() || !surface_n->IsWellFormed() )
    return NULL;

  // Access limits and geometry
  surface_n->GetLimits(uLimit, vLimit);
  //
  return surface_n->GetSurface();
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_RESurfacesDataProvider::translationSources() const
{
  // Resulting Parameters
  ActParamStream out;

  if ( m_surfaces.IsNull() || !m_surfaces->IsWellFormed() )
    return out;

  // Iterate over the child Nodes which are the individual surfaces
  for ( Handle(ActAPI_IChildIterator) cit = m_surfaces->GetChildIterator(); cit->More(); cit->Next() )
  {
    Handle(asiData_RESurfaceNode) surface_n = Handle(asiData_RESurfaceNode)::DownCast( cit->Value() );
    //
    if ( surface_n.IsNull() || !surface_n->IsWellFormed() )
      continue;

    out << surface_n->Parameter(asiData_RESurfaceNode::PID_Surface);
  }

  return out;
}
