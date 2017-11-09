//-----------------------------------------------------------------------------
// Created on: 21 April 2016
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

#ifndef asiAlgo_PurifyCloud_h
#define asiAlgo_PurifyCloud_h

// Common includes
#include <asiAlgo_CommonCloud.h>

// OCCT includes
#include <gp_XYZ.hxx>
#include <NCollection_CellFilter.hxx>

//-----------------------------------------------------------------------------
// Two-dimensional inspector
//-----------------------------------------------------------------------------

//! Inspector tool (see OCCT Cell Filter) for purification of
//! points which are coincident with a certain tolerance.
class asiAlgo_Inspector2d
{
public:

  //! Cloud dimension.
  enum { Dimension = 2 };

  //! Point index is used as a cell ID.
  typedef int Target;

  //! Point type.
  typedef asiAlgo_CommonCloud<gp_XY> Point;

  //! Constructor accepting the tolerance.
  //! \param tol    [in] tolerance to set.
  //! \param points [in] working collection of points.
  asiAlgo_Inspector2d(const double                    tol,
                    const asiAlgo_CommonCloud2dVec& points)
  : m_fTol(tol*tol),
    m_points(points)
  {}

  //! Cleans up the list of resulting indices.
  void ClearResList()
  {
    m_resIndices.Clear();
  }

  //! Sets current point to search for coincidence.
  //! \param target [in] current target.
  //! \param XY [in] point to set as current.
  void SetCurrent(const int target, const Point& XY)
  {
    m_iCurrentTarget = target;
    m_current        = XY;
  }

  //! Returns the list of indices adjacent with the current.
  //! \return list of indices.
  const NCollection_Map<int>& Indices2Exclude() const
  {
    return m_resIndices;
  }

  //! Implementation of inspection method.
  //! \param target [in] target.
  //! \return verdict (remove or keep).
  NCollection_CellFilter_Action Inspect(const int target)
  {
    if ( m_iCurrentTarget != target )
    {
      const Point& P     = m_points.at(target);
      const double delta = (P.Coord - m_current.Coord).SquareModulus();

      if ( delta < m_fTol )
      {
        m_resIndices.Add(target);
        return CellFilter_Purge;
      }
    }

    return CellFilter_Keep;
  }

  //! Auxiliary method to shift point by each coordinate on given value.
  //! Useful for preparing a points range for Inspect with tolerance.
  Point Shift(const Point& thePnt, double theTol) const
  {
    Point P(thePnt);
    P.Coord.SetX( P.Coord.X() + theTol );
    P.Coord.SetY( P.Coord.Y() + theTol );
    return P;
  }

  //! Returns coordinates.
  //! \param i      [in] 0-based index of coordinate.
  //! \param thePnt [in] point to access coordinates for.
  //! \return requested coordinate.
  static double Coord(int i, const Point& thePnt)
  {
    return thePnt.Coord.Coord(i + 1);
  }

private:

  double                   m_fTol;           //!< Tolerance to use.
  NCollection_Map<int>     m_resIndices;     //!< Indices to exclude.
  asiAlgo_CommonCloud2dVec m_points;         //!< Points kept for comparison.
  asiAlgo_CommonCloud<gp_XY> m_current;        //!< Current point.
  int                      m_iCurrentTarget; //!< Current target.

};

//-----------------------------------------------------------------------------
// Purification algorithm
//-----------------------------------------------------------------------------

//! Auxiliary functions for purification of point clouds. These tools
//! allow reducing the number of points by getting rid of those which
//! fall into the given tolerant sphere.
class asiAlgo_PurifyCloud
{
public:

  //! Performs purification on the passed 2D point cloud.
  //! \param tol    [in] tolerance to use.
  //! \param source [in] source point cloud.
  //! \param result [in] resulting point cloud.
  static void Perform2d(const double                    tol,
                        const asiAlgo_CommonCloud2dVec& source,
                        asiAlgo_CommonCloud2dVec&       result)
  {
    Purify<asiAlgo_CommonCloud2dVec,
           asiAlgo_Inspector2d>(tol, source, result);
  }

public:

  template<typename CloudType,
           typename InspectorType>
  static void Purify(const double     tol,
                     const CloudType& source,
                     CloudType&       result)
  {
    const size_t source_size = source.size();
    if ( !source_size )
      return;

    NCollection_CellFilter<InspectorType> CellFilter(tol);
    InspectorType Inspector(tol, source);

    for ( size_t p_idx = 0; p_idx < source.size(); ++p_idx )
    {
      const InspectorType::Point& P = source.at(p_idx);
      CellFilter.Add( (int) p_idx, P );
    }

    NCollection_Map<int> Traversed;
    int next_idx = 0;
    unsigned int allTraversed = 0;
    do
    {
      if ( Traversed.Contains(next_idx) )
        continue;

      Traversed.Add(next_idx);
      const InspectorType::Point& P = source.at(next_idx);

      Inspector.SetCurrent( (int) next_idx, P );
      InspectorType::Point P_min = Inspector.Shift(P, -tol);
      InspectorType::Point P_max = Inspector.Shift(P, tol);
      CellFilter.Inspect(P_min, P_max, Inspector);

      // Remaining indices
      int nonTraversed = -1;
      const NCollection_Map<int>& indices2Exclude = Inspector.Indices2Exclude();
      for ( size_t i = 0; i < source_size; ++i )
      {
        if ( indices2Exclude.Contains( (int) i ) )
          continue;

        if ( !Traversed.Contains( (int) i ) )
        {
          nonTraversed = (int) i;
          break;
        }
      }

      if ( nonTraversed == -1 )
        allTraversed = 1;
      else
        next_idx = nonTraversed;
    }
    while ( !allTraversed );

    //----------------------------------------------------
    // Copy remaining points to another (resulting) cloud
    //----------------------------------------------------

    const NCollection_Map<int>& indices2Exclude = Inspector.Indices2Exclude();
    const int indices_size = indices2Exclude.Extent();
    if ( !indices_size )
      result = source;

    for ( size_t i = 0; i < source_size; ++i )
    {
      if ( indices2Exclude.Contains( (int) i ) )
        continue;

      const InspectorType::Point& P = source.at(i);
      result.push_back(P);
    }
  }

private:

  asiAlgo_PurifyCloud ()                         {}
  asiAlgo_PurifyCloud (const asiAlgo_PurifyCloud&) {}
  void operator=    (const asiAlgo_PurifyCloud&) {}

};

#endif
