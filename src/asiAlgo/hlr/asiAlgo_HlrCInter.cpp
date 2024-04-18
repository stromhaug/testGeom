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

// Own include
#include <asiAlgo_HlrCInter.h>

// asiAlgo includes
#include <asiAlgo_HlrCurveTool.h>
#include <asiAlgo_HlrProjPCurOfCInter.h>
#include <asiAlgo_HlrCurveLocatorOfProjPCurOfCInter.h>
#include <asiAlgo_HlrLocateExtPCOfProjPCurOfCInter.h>
#include <asiAlgo_HlrIntConicCurveOfCInter.h>
#include <asiAlgo_HlrIntersectorOfIntConicCurveOfCInter.h>
#include <asiAlgo_HlrIntConicCurveOfCInter.h>
#include <asiAlgo_HlrIntPCurvePCurveOfCInter.h>
#include <asiAlgo_HlrPolygon2dOfIntPCurvePCurveOfCInter.h>
#include <asiAlgo_HlrDistBetweenPCurvesOfIntPCurvePCurveOfCInter.h>
#include <asiAlgo_HlrExactIntersectionPointOfIntPCurvePCurveOfCInter.h>

// OpenCascade includes
#include <Standard_ConstructionError.hxx>
#include <IntRes2d_Domain.hxx>

using namespace asiAlgo::hlr;

#define TheCurve Standard_Address
#define TheCurve_hxx <Standard_Address.hxx>
#define TheCurveTool CurveTool
#define TheCurveTool_hxx <asiAlgo_HlrCurveTool.h>
#define IntCurve_TheProjPCur ProjPCurOfCInter
#define IntCurve_TheProjPCur_hxx <asiAlgo_HlrProjPCurOfCInter.h>
#define IntCurve_TheCurveLocatorOfTheProjPCur CurveLocatorOfProjPCurOfCInter
#define IntCurve_TheCurveLocatorOfTheProjPCur_hxx <asiAlgo_HlrCurveLocatorOfProjPCurOfCInter.h>
#define IntCurve_TheLocateExtPCOfTheProjPCur LocateExtPCOfProjPCurOfCInter
#define IntCurve_TheLocateExtPCOfTheProjPCur_hxx <asiAlgo_HlrLocateExtPCOfProjPCurOfCInter.h>
#define IntCurve_TheCurveLocatorOfTheProjPCur CurveLocatorOfProjPCurOfCInter
#define IntCurve_TheCurveLocatorOfTheProjPCur_hxx <asiAlgo_HlrCurveLocatorOfProjPCurOfCInter.h>
#define IntCurve_TheLocateExtPCOfTheProjPCur LocateExtPCOfProjPCurOfCInter
#define IntCurve_TheLocateExtPCOfTheProjPCur_hxx <asiAlgo_HlrLocateExtPCOfProjPCurOfCInter.h>
#define IntCurve_TheIntConicCurve IntConicCurveOfCInter
#define IntCurve_TheIntConicCurve_hxx <asiAlgo_HlrIntConicCurveOfCInter.h>
#define IntCurve_TheIntersectorOfTheIntConicCurve IntersectorOfIntConicCurveOfCInter
#define IntCurve_TheIntersectorOfTheIntConicCurve_hxx <asiAlgo_HlrIntersectorOfIntConicCurveOfCInter.h>
#define IntCurve_TheIntersectorOfTheIntConicCurve IntersectorOfIntConicCurveOfCInter
#define IntCurve_TheIntersectorOfTheIntConicCurve_hxx <asiAlgo_HlrIntersectorOfIntConicCurveOfCInter.h>
#define IntCurve_IntConicCurve IntConicCurveOfCInter
#define IntCurve_IntConicCurve_hxx <asiAlgo_HlrIntConicCurveOfCInter.h>
#define IntCurve_TheIntPCurvePCurve IntPCurvePCurveOfCInter
#define IntCurve_TheIntPCurvePCurve_hxx <asiAlgo_HlrIntPCurvePCurveOfCInter.h>
#define IntCurve_ThePolygon2dOfTheIntPCurvePCurve Polygon2dOfIntPCurvePCurveOfCInter
#define IntCurve_ThePolygon2dOfTheIntPCurvePCurve_hxx <asiAlgo_HlrPolygon2dOfIntPCurvePCurveOfCInter.h>
#define IntCurve_TheDistBetweenPCurvesOfTheIntPCurvePCurve DistBetweenPCurvesOfIntPCurvePCurveOfCInter
#define IntCurve_TheDistBetweenPCurvesOfTheIntPCurvePCurve_hxx <asiAlgo_HlrDistBetweenPCurvesOfIntPCurvePCurveOfCInter.h>
#define IntCurve_ExactIntersectionPointOfTheIntPCurvePCurve ExactIntersectionPointOfIntPCurvePCurveOfCInter
#define IntCurve_ExactIntersectionPointOfTheIntPCurvePCurve_hxx <asiAlgo_HlrExactIntersectionPointOfIntPCurvePCurveOfCInter.h>
#define IntCurve_ThePolygon2dOfTheIntPCurvePCurve Polygon2dOfIntPCurvePCurveOfCInter
#define IntCurve_ThePolygon2dOfTheIntPCurvePCurve_hxx <asiAlgo_HlrPolygon2dOfIntPCurvePCurveOfCInter.h>
#define IntCurve_TheDistBetweenPCurvesOfTheIntPCurvePCurve DistBetweenPCurvesOfIntPCurvePCurveOfCInter
#define IntCurve_TheDistBetweenPCurvesOfTheIntPCurvePCurve_hxx <asiAlgo_HlrDistBetweenPCurvesOfIntPCurvePCurveOfCInter.h>
#define IntCurve_ExactIntersectionPointOfTheIntPCurvePCurve ExactIntersectionPointOfIntPCurvePCurveOfCInter
#define IntCurve_ExactIntersectionPointOfTheIntPCurvePCurve_hxx <asiAlgo_HlrExactIntersectionPointOfIntPCurvePCurveOfCInter.h>
#define IntCurve_IntCurveCurveGen CInter
#define IntCurve_IntCurveCurveGen_hxx <asiAlgo_HlrCInter.h>

#include <IntCurve_IntCurveCurveGen.gxx>
