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

#include <asiAlgo_HlrIntConicCurveOfCInterCore.h>
#include <asiAlgo_HlrCurveTool.h>
#include <asiAlgo_HlrProjPCurOfCInter.h>
#include <asiAlgo_HlrIntersectorOfIntConicCurveOfCInter.h>
#include <asiAlgo_HlrImpParToolOfIntersectorOfIntConicCurveOfCInter.h>

#include <IntCurve_IConicTool.hxx>
#include <gp_Lin2d.hxx>
#include <IntRes2d_Domain.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Hypr2d.hxx>

using namespace asiAlgo::hlr;

//-----------------------------------------------------------------------------

#define TheImpTool IntCurve_IConicTool
#define TheImpTool_hxx <IntCurve_IConicTool.hxx>
#define ThePCurve Standard_Address
#define ThePCurve_hxx <Standard_Address.hxx>
#define ThePCurveTool CurveTool
#define ThePCurveTool_hxx <asiAlgo_HlrCurveTool.h>
#define TheProjPCur ProjPCurOfCInter
#define TheProjPCur_hxx <asiAlgo_HlrProjPCurOfCInter.h>
#define IntCurve_TheIntersector IntersectorOfIntConicCurveOfCInter
#define IntCurve_TheIntersector_hxx <asiAlgo_HlrIntersectorOfIntConicCurveOfCInter.h>
#define IntCurve_MyImpParToolOfTheIntersector ImpParToolOfIntersectorOfIntConicCurveOfCInter
#define IntCurve_MyImpParToolOfTheIntersector_hxx <asiAlgo_HlrImpParToolOfIntersectorOfIntConicCurveOfCInter.h>
#define IntCurve_IntConicCurveGen IntConicCurveOfCInterCore
#define IntCurve_IntConicCurveGen_hxx <asiAlgo_HlrIntConicCurveOfCInterCore.h>
#include <IntCurve_IntConicCurveGen.gxx>

