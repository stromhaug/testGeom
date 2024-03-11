//-----------------------------------------------------------------------------
// Created on: 11 March 2024
//-----------------------------------------------------------------------------
// Copyright (c) 2024, Elizaveta Krylova
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

#ifndef asiVisu_TransformPolyDataFilter_h
#define asiVisu_TransformPolyDataFilter_h

// VTK includes
#include <vtkTransformPolyDataFilter.h>
#include <vtkFiltersGeneralModule.h>
#include <vtkPolyDataAlgorithm.h>

//! Filter performing point coordinates and associated point and cell normals and
//! vectors transformation. Other point and cell data is passed through the filter
//! unchanged. This filter is specialized for polygonal data. See vtkTransformFilter
//! for more general data. Unlike vtkTransformPolyDataFilter filter does not have
//! the error message about no input data (that exists in VTK 8.2).
class asiVisu_TransformPolyDataFilter : public vtkTransformPolyDataFilter
{
public:

  vtkTypeMacro(asiVisu_TransformPolyDataFilter, vtkPolyDataAlgorithm);
  static asiVisu_TransformPolyDataFilter *New();
  virtual void SetTransform(vtkAbstractTransform*);

  vtkSetMacro(OutputPointsPrecision,int);
  vtkGetMacro(OutputPointsPrecision,int);

protected:

  asiVisu_TransformPolyDataFilter();
  ~asiVisu_TransformPolyDataFilter() override;

protected:

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

protected:

  vtkAbstractTransform *Transform;
  int OutputPointsPrecision;

private:

  asiVisu_TransformPolyDataFilter(const asiVisu_TransformPolyDataFilter&) = delete;
  void operator=(const asiVisu_TransformPolyDataFilter&) = delete;
};

#endif
