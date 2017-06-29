//-----------------------------------------------------------------------------
// Created on: 13 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef asiVisu_MeshEVectorFilter_h
#define asiVisu_MeshEVectorFilter_h

// Visualization includes
#include <asiVisu_Common.h>

// Active Data (auxiliary) includes
#include <ActAux_Common.h>

// VTK includes
#include <vtkDoubleArray.h>
#include <vtkIdList.h>
#include <vtkPolyDataAlgorithm.h>

//! Filter populating VTK cell vectors with domain data.
class asiVisu_MeshEVectorFilter : public vtkPolyDataAlgorithm
{
public:

  vtkTypeMacro(asiVisu_MeshEVectorFilter, vtkPolyDataAlgorithm);

public:

  static asiVisu_MeshEVectorFilter* New();

  //! Sets elemental vectors.
  //! \param theElemIDs    [in] involved mesh element IDs.
  //! \param theElemData   [in] vectors for the involved mesh elements.
  //! \param theMaxModulus [in] maximum modulus.
  void SetVectorArrays(const Handle(HIntArray)&  theElemIDs,
                       const Handle(HRealArray)& theElemData,
                       const double              theMaxModulus = 1.0)
  {
    m_vectorMap->Clear();

    if ( !theElemIDs.IsNull() && !theElemData.IsNull() )
    {
      for ( int i = theElemIDs->Lower(); i <= theElemIDs->Upper(); i++ )
      {
        int anID = theElemIDs->Value(i);
        if ( anID == -1 )
          continue;

        asiVisu_VectorTuple aVecTuple( theElemData->Value(3 * i),
                                       theElemData->Value(3 * i + 1),
                                       theElemData->Value(3 * i + 2) );
        m_vectorMap->Bind(anID, aVecTuple);
      }
    }

    m_fMaxModulus = theMaxModulus;
  }

  //! Gets maximum scalar value obtained during the filtering process.
  //! \return max scalar value
  double GetMaxScalar() const
  {
    return m_fMaxScalar;
  }

  //! Gets minimal scalar value obtained during the filtering process.
  //! \return min scalar value
  double GetMinScalar() const
  {
    return m_fMinScalar;
  }

private:

  asiVisu_MeshEVectorFilter();

  ~asiVisu_MeshEVectorFilter();

public:

  virtual int FillInputPortInformation(int thePort, vtkInformation* theInfo);

  virtual int RequestData(vtkInformation*        theInfo,
                          vtkInformationVector** theInput,
                          vtkInformationVector*  theOutput);

private:

  bool vectorForElem (const int theElemID, asiVisu_VectorTuple& theVecTuple);
  void adjustMinMax  (const asiVisu_VectorTuple& theVecTuple);

private:

  //! Prohibited copy constructor.
  //! \param theFilter [in] another filter.
  asiVisu_MeshEVectorFilter(const asiVisu_MeshEVectorFilter& theFilter); // Not implemented.

  //! Prohibited assignment operator.
  //! \param theFilter [in] another filter.
  void operator=(const asiVisu_MeshEVectorFilter& theFilter); // Not implemented.

private:

  //! Maximum modulus.
  double m_fMaxModulus;

  //! Vectors associated with their owning IDs.
  Handle(asiVisu_HVectorMap) m_vectorMap;

  //! Maximum scalar value obtained during the filtering process.
  double m_fMaxScalar;

  //! Minimal scalar value obtained during the filtering process.
  double m_fMinScalar;

};

#endif
