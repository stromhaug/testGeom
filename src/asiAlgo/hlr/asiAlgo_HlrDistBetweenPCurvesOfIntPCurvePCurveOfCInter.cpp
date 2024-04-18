// Created on: 1992-10-14
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <asiAlgo_HlrDistBetweenPCurvesOfIntPCurvePCurveOfCInter.h>
#include <asiAlgo_HlrCurveTool.h>

#include <math_Matrix.hxx>

using namespace asiAlgo::hlr;

//-----------------------------------------------------------------------------

#define TheCurve Standard_Address
#define TheCurve_hxx <Standard_Address.hxx>
#define TheCurveTool CurveTool
#define TheCurveTool_hxx <asiAlgo_HlrCurveTool.h>
#define IntCurve_DistBetweenPCurvesGen DistBetweenPCurvesOfIntPCurvePCurveOfCInter
#define IntCurve_DistBetweenPCurvesGen_hxx <asiAlgo_HlrDistBetweenPCurvesOfIntPCurvePCurveOfCInter.h>
#include <IntCurve_DistBetweenPCurvesGen.gxx>

