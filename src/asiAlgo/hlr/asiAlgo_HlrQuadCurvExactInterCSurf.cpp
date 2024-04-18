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

#include <asiAlgo_HlrQuadCurvExactInterCSurf.h>

#include <asiAlgo_HlrSurfaceTool.h>
#include <asiAlgo_HlrLineTool.h>
#include <asiAlgo_HlrQuadCurvFuncOfQuadCurvExactInterCSurf.h>

#include <gp_Lin.hxx>

using namespace asiAlgo::hlr;

//-----------------------------------------------------------------------------

#define TheSurface Standard_Address
#define TheSurface_hxx <Standard_Address.hxx>
#define TheSurfaceTool SurfaceTool
#define TheSurfaceTool_hxx <asiAlgo_HlrSurfaceTool.h>
#define TheCurve gp_Lin
#define TheCurve_hxx <gp_Lin.hxx>
#define TheCurveTool LineTool
#define TheCurveTool_hxx <asiAlgo_HlrLineTool.h>
#define IntCurveSurface_TheQuadCurvFunc QuadCurvFuncOfQuadCurvExactInterCSurf
#define IntCurveSurface_TheQuadCurvFunc_hxx <asiAlgo_HlrQuadCurvFuncOfQuadCurvExactInterCSurf.h>
#define IntCurveSurface_QuadricCurveExactInter QuadCurvExactInterCSurf
#define IntCurveSurface_QuadricCurveExactInter_hxx <asiAlgo_HlrQuadCurvExactInterCSurf.h>
#include <IntCurveSurface_QuadricCurveExactInter.gxx>

