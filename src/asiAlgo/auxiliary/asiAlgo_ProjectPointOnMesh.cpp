//-----------------------------------------------------------------------------
// Created on: 29 September 2018
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

// Own include
#include <asiAlgo_ProjectPointOnMesh.h>

// asiAlgo includes
#include <asiAlgo_BVHIterator.h>

// OCCT includes
#include <Precision.hxx>

// Standard includes
#include <set>

//-----------------------------------------------------------------------------

const static double TOLER = Precision::Confusion();
const static double SQTOLER = TOLER * TOLER;

//-----------------------------------------------------------------------------

typedef int BVH_StackItem;

//! Projection information.
struct ProjectionInfoMesh
{
  //! index of the triangle.
  std::vector<int> myTriIdx;

  //! Actual index to fill.
  int myIdx;

  //! Resulting projected point.
  gp_Pnt myProjectedPoint;

  //! Current squared distance.
  double mySqDistance;

  //! Constructor. Sets default values.
  ProjectionInfoMesh()
    : myTriIdx(10),
      myIdx(-1),
      myProjectedPoint(Precision::Infinite(), Precision::Infinite(), Precision::Infinite()),
      mySqDistance(Precision::Infinite())
  {}

  //! Updates values.
  void UpdateValues(const gp_Pnt thePnt,
                    const double theSqDistance,
                    const int theIdx)
  {
    if (theSqDistance > mySqDistance + SQTOLER)
      return;
    else if (theSqDistance < mySqDistance - SQTOLER)
    {
      myProjectedPoint = thePnt;
      mySqDistance = theSqDistance;

      myIdx = 0;
      myTriIdx[myIdx] = theIdx;
    }
    else
    {
      myProjectedPoint = thePnt;
      mySqDistance = theSqDistance;

      ++myIdx;
      if (myIdx < myTriIdx.size())
        myTriIdx[myIdx] = theIdx;
      else
        myTriIdx.push_back(theIdx);
    }
  }
};

//-----------------------------------------------------------------------------

static void projectPntToTriangle(const gp_Pnt& thePnt,
                                 const asiAlgo_BVHFacets::t_facet& theTD,
                                 ProjectionInfoMesh& theProjection,
                                 const int theIdx)
{
  gp_Pnt aTri1(theTD.P0.x(), theTD.P0.y(), theTD.P0.z());
  gp_Pnt aTri2(theTD.P1.x(), theTD.P1.y(), theTD.P1.z());
  gp_Pnt aTri3(theTD.P2.x(), theTD.P2.y(), theTD.P2.z());

  gp_Pnt aProj;
  const double aSqDist = asiAlgo_BVHAlgo::squaredDistancePointTriangle(thePnt, aTri1, aTri2, aTri3, theTD.N, aProj);
  theProjection.UpdateValues(aProj, aSqDist, theIdx);
}

//-----------------------------------------------------------------------------

asiAlgo_ProjectPointOnMesh::asiAlgo_ProjectPointOnMesh(const Handle(asiAlgo_BVHFacets)& facets,
                                                       ActAPI_ProgressEntry             progress,
                                                       ActAPI_PlotterEntry              plotter)
: asiAlgo_BVHAlgo(facets, progress, plotter)
{}

//-----------------------------------------------------------------------------

void asiAlgo_ProjectPointOnMesh::IntersectLeaves(const BVH_Vec4i&    theLeaf,
                                                 const gp_Pnt&       theSourcePoint,
                                                 ProjectionInfoMesh& theProjection) const
{
  // Loop over the tentative facets
  for (int aTrgIdx = theLeaf.y(); aTrgIdx <= theLeaf.z(); ++aTrgIdx)
  {
    // Get triangle.
    const asiAlgo_BVHFacets::t_facet& aTD = m_facets->GetFacet(aTrgIdx);

    // Calculate projection and minimal distance between point and triangle.
    projectPntToTriangle(theSourcePoint, aTD, theProjection, aTrgIdx);
  }
}

//-----------------------------------------------------------------------------

gp_Pnt asiAlgo_ProjectPointOnMesh::Perform(const gp_Pnt& thePnt)
{
  ProjectionInfoMesh aProjected;

  const opencascade::handle<BVH_Tree<double, 4> >& BVH = m_facets->BVH();
  if ( BVH.IsNull() )
    return aProjected.myProjectedPoint;

  // Compute initial approximation of the solution.
  const int aSize = m_facets->Size();
  int anIdx[7] = { 0, aSize / 6, aSize / 3, aSize / 2, (aSize * 2) / 3, (aSize * 5) / 6, aSize - 1 };
  for (int i = 0; i < 7; ++i)
  {
    const asiAlgo_BVHFacets::t_facet& aTD = m_facets->GetFacet(anIdx[i]);

    // Calculate projection and minimal distance between point and triangle.
    projectPntToTriangle(thePnt, aTD, aProjected, anIdx[i]);
  }

  for (asiAlgo_BVHIterator it(BVH); it.More(); it.Next())
  {
    const BVH_Vec4i& aNodeData = it.Current();

    if (it.IsLeaf())
    {
      // Perform precise test
      IntersectLeaves(aNodeData, thePnt, aProjected);
    }
    else // sub-volume
    {
      const BVH_Vec4d& aMinPntLft1 = BVH->MinPoint(aNodeData.y());
      const BVH_Vec4d& aMaxPntLft1 = BVH->MaxPoint(aNodeData.y());
      const BVH_Vec4d& aMinPntRgh1 = BVH->MinPoint(aNodeData.z());
      const BVH_Vec4d& aMaxPntRgh1 = BVH->MaxPoint(aNodeData.z());

      bool anOut1 = isOut(aMinPntLft1, aMaxPntLft1, thePnt, Sqrt(aProjected.mySqDistance));
      bool anOut2 = isOut(aMinPntRgh1, aMaxPntRgh1, thePnt, Sqrt(aProjected.mySqDistance));

      if (anOut1)
        it.BlockLeft();

      if (anOut2)
        it.BlockRight();
    }
  }

  m_triIdx.clear();
  for(int i = 0; i <= aProjected.myIdx; ++i)
    m_triIdx.push_back(aProjected.myTriIdx[i]);

  return aProjected.myProjectedPoint;
}