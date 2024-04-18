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

#include <asiAlgo_HlrIntersectorOfIntConicCurveOfCInter.h>

#include <asiAlgo_HlrCurveTool.h>
#include <asiAlgo_HlrProjPCurOfCInter.h>
#include <asiAlgo_HlrImpParToolOfIntersectorOfIntConicCurveOfCInter.h>

#include <Standard_ConstructionError.hxx>
#include <IntCurve_IConicTool.hxx>
#include <IntRes2d_Domain.hxx>
#include <gp_Pnt2d.hxx>

using namespace asiAlgo::hlr;

//-----------------------------------------------------------------------------

#define ImpTool IntCurve_IConicTool
#define ImpTool_hxx <IntCurve_IConicTool.hxx>
#define ParCurve Standard_Address
#define ParCurve_hxx <Standard_Address.hxx>
#define ParTool CurveTool
#define ParTool_hxx <asiAlgo_HlrCurveTool.h>
#define ProjectOnPCurveTool ProjPCurOfCInter
#define ProjectOnPCurveTool_hxx <asiAlgo_HlrProjPCurOfCInter.h>
#define IntImpParGen_MyImpParTool ImpParToolOfIntersectorOfIntConicCurveOfCInter
#define IntImpParGen_MyImpParTool_hxx <asiAlgo_HlrImpParToolOfIntersectorOfIntConicCurveOfCInter.h>
#define IntImpParGen_Intersector IntersectorOfIntConicCurveOfCInter
#define IntImpParGen_Intersector_hxx <asiAlgo_HlrIntersectorOfIntConicCurveOfCInter.h>
#include <IntImpParGen_Intersector.gxx>

