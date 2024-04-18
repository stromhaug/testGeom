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

#include <asiAlgo_HlrCurveLocatorOfProjPCurOfCInter.h>
#include <asiAlgo_HlrCurveTool.h>

#include <Extrema_POnCurv2d.hxx>
#include <gp_Pnt2d.hxx>

using namespace asiAlgo::hlr;

//-----------------------------------------------------------------------------

#define Curve1 Standard_Address
#define Curve1_hxx <Standard_Address.hxx>
#define Tool1 CurveTool
#define Tool1_hxx <asiAlgo_HlrCurveTool.h>
#define Curve2 Standard_Address
#define Curve2_hxx <Standard_Address.hxx>
#define Tool2 CurveTool
#define Tool2_hxx <asiAlgo_HlrCurveTool.h>
#define POnC Extrema_POnCurv2d
#define POnC_hxx <Extrema_POnCurv2d.hxx>
#define Pnt gp_Pnt2d
#define Pnt_hxx <gp_Pnt2d.hxx>
#define Extrema_CurveLocator CurveLocatorOfProjPCurOfCInter
#define Extrema_CurveLocator_hxx <asiAlgo_HlrCurveLocatorOfProjPCurOfCInter.h>

#include <Extrema_CurveLocator.gxx>
