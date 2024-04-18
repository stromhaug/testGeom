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

#include <asiAlgo_HlrInterCSurf.h>

#include <asiAlgo_HlrLineTool.h>
#include <asiAlgo_HlrSurfaceTool.h>
#include <asiAlgo_HlrPolygonOfInterCSurf.h>
#include <asiAlgo_HlrPolygonToolOfInterCSurf.h>
#include <asiAlgo_HlrPolyhedronOfInterCSurf.h>
#include <asiAlgo_HlrPolyhedronToolOfInterCSurf.h>
#include <asiAlgo_HlrInterferenceOfInterCSurf.h>
#include <asiAlgo_HlrCSFunctionOfInterCSurf.h>
#include <asiAlgo_HlrExactInterCSurf.h>
#include <asiAlgo_HlrQuadCurvExactInterCSurf.h>
#include <asiAlgo_HlrQuadCurvFuncOfQuadCurvExactInterCSurf.h>

#include <gp_Lin.hxx>
#include <Bnd_BoundSortBox.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Parab.hxx>
#include <gp_Hypr.hxx>
#include <IntAna_IntConicQuad.hxx>
#include <Bnd_Box.hxx>

using namespace asiAlgo::hlr;

//-----------------------------------------------------------------------------

#define TheCurve gp_Lin
#define TheCurve_hxx <gp_Lin.hxx>
#define TheCurveTool LineTool
#define TheCurveTool_hxx <asiAlgo_HlrLineTool.h>
#define TheSurface Standard_Address
#define TheSurface_hxx <Standard_Address.hxx>
#define TheSurfaceTool SurfaceTool
#define TheSurfaceTool_hxx <asiAlgo_HlrSurfaceTool.h>
#define IntCurveSurface_ThePolygon PolygonOfInterCSurf
#define IntCurveSurface_ThePolygon_hxx <asiAlgo_HlrPolygonOfInterCSurf.h>
#define IntCurveSurface_ThePolygonTool PolygonToolOfInterCSurf
#define IntCurveSurface_ThePolygonTool_hxx <asiAlgo_HlrPolygonToolOfInterCSurf.h>
#define IntCurveSurface_ThePolyhedron PolyhedronOfInterCSurf
#define IntCurveSurface_ThePolyhedron_hxx <asiAlgo_HlrPolyhedronOfInterCSurf.h>
#define IntCurveSurface_ThePolyhedronTool PolyhedronToolOfInterCSurf
#define IntCurveSurface_ThePolyhedronTool_hxx <asiAlgo_HlrPolyhedronToolOfInterCSurf.h>
#define IntCurveSurface_TheInterference InterferenceOfInterCSurf
#define IntCurveSurface_TheInterference_hxx <asiAlgo_HlrInterferenceOfInterCSurf.h>
#define IntCurveSurface_TheCSFunction CSFunctionOfInterCSurf
#define IntCurveSurface_TheCSFunction_hxx <asiAlgo_HlrCSFunctionOfInterCSurf.h>
#define IntCurveSurface_TheExactInter ExactInterCSurf
#define IntCurveSurface_TheExactInter_hxx <asiAlgo_HlrExactInterCSurf.h>
#define IntCurveSurface_TheQuadCurvExactInter QuadCurvExactInterCSurf
#define IntCurveSurface_TheQuadCurvExactInter_hxx <asiAlgo_HlrQuadCurvExactInterCSurf.h>
#define IntCurveSurface_TheQuadCurvFuncOfTheQuadCurvExactInter QuadCurvFuncOfQuadCurvExactInterCSurf
#define IntCurveSurface_TheQuadCurvFuncOfTheQuadCurvExactInter_hxx <asiAlgo_HlrQuadCurvFuncOfQuadCurvExactInterCSurf.h>
#define IntCurveSurface_TheQuadCurvFuncOfTheQuadCurvExactInter QuadCurvFuncOfQuadCurvExactInterCSurf
#define IntCurveSurface_TheQuadCurvFuncOfTheQuadCurvExactInter_hxx <asiAlgo_HlrQuadCurvFuncOfQuadCurvExactInterCSurf.h>
#define IntCurveSurface_Inter InterCSurf
#define IntCurveSurface_Inter_hxx <asiAlgo_HlrInterCSurf.h>
#include <IntCurveSurface_Inter.gxx>

