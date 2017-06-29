//-----------------------------------------------------------------------------
// Created on: 21 April 2016
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

#ifndef asiAlgo_ClassifyPointFace_h
#define asiAlgo_ClassifyPointFace_h

// Geometry includes
#include <asiAlgo_Classifier.h>

// OCCT includes
#include <IntTools_FClass2d.hxx>
#include <TopoDS_Face.hxx>

// Active Data includes
#include <ActAPI_IProgressNotifier.h>

//! Point-Face classifier.
class asiAlgo_ClassifyPointFace
{
public:

  asiAlgo_EXPORT
    asiAlgo_ClassifyPointFace(const TopoDS_Face& F,
                              const double       inaccuracy,
                              const double       precision);

public:

  asiAlgo_EXPORT asiAlgo_Membership
    operator()(const gp_Pnt2d&      PonS,
               ActAPI_ProgressEntry Journal);

  asiAlgo_EXPORT asiAlgo_Membership
    operator()(const gp_Pnt&        P,
               const bool           checkGap,
               ActAPI_ProgressEntry Journal);

  asiAlgo_EXPORT asiAlgo_Membership
    operator()(const gp_Pnt&        P,
               const bool           checkGap,
               gp_Pnt2d&            UV,
               ActAPI_ProgressEntry Journal);

protected:

  TopoDS_Face       m_F;           //!< Working face.
  double            m_fPrecision;  //!< Precision of optimization method.
  double            m_fInaccuracy; //!< Inaccuracy in the input geometry.
  IntTools_FClass2d m_fclass;      //!< Internal classifier.

};

#endif
