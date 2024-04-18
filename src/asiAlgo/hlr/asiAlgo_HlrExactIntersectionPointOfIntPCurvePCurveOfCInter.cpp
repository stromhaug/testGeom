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

#include <asiAlgo_HlrExactIntersectionPointOfIntPCurvePCurveOfCInter.h>

#include <asiAlgo_HlrCurveTool.h>
#include <asiAlgo_HlrProjPCurOfCInter.h>
#include <asiAlgo_HlrIntPCurvePCurveOfCInter.h>
#include <asiAlgo_HlrPolygon2dOfIntPCurvePCurveOfCInter.h>
#include <asiAlgo_HlrDistBetweenPCurvesOfIntPCurvePCurveOfCInter.h>

using namespace asiAlgo::hlr;

//-----------------------------------------------------------------------------

#define TheCurve Standard_Address
#define TheCurve_hxx <Standard_Address.hxx>
#define TheCurveTool CurveTool
#define TheCurveTool_hxx <asiAlgo_HlrCurveTool.h>
#define TheProjPCur ProjPCurOfCInter
#define TheProjPCur_hxx <asiAlgo_HlrProjPCurOfCInter.h>
#define IntCurve_ThePolygon2d Polygon2dOfIntPCurvePCurveOfCInter
#define IntCurve_ThePolygon2d_hxx <asiAlgo_HlrPolygon2dOfIntPCurvePCurveOfCInter.h>
#define IntCurve_TheDistBetweenPCurves DistBetweenPCurvesOfIntPCurvePCurveOfCInter
#define IntCurve_TheDistBetweenPCurves_hxx <asiAlgo_HlrDistBetweenPCurvesOfIntPCurvePCurveOfCInter.h>
#define IntCurve_ExactIntersectionPoint ExactIntersectionPointOfIntPCurvePCurveOfCInter
#define IntCurve_ExactIntersectionPoint_hxx <asiAlgo_HLRBRep_ExactIntersectionPointOfTheIntPCurvePCurveOfCInter.h>
#define IntCurve_IntPolyPolyGen IntPCurvePCurveOfCInter
#define IntCurve_IntPolyPolyGen_hxx <asiAlgo_HlrIntPCurvePCurveOfCInter.h>
#include <IntCurve_ExactIntersectionPoint.gxx>

