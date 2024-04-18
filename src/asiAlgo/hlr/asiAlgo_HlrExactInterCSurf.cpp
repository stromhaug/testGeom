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

#include <asiAlgo_HlrExactInterCSurf.h>
#include <asiAlgo_HlrSurfaceTool.h>
#include <asiAlgo_HlrLineTool.h>
#include <asiAlgo_HlrCSFunctionOfInterCSurf.h>

#include <StdFail_NotDone.hxx>
#include <Standard_DomainError.hxx>
#include <gp_Lin.hxx>
#include <math_FunctionSetRoot.hxx>
#include <gp_Pnt.hxx>

using namespace asiAlgo::hlr;

//-----------------------------------------------------------------------------

#define ThePSurface Standard_Address
#define ThePSurface_hxx <Standard_Address.hxx>
#define ThePSurfaceTool SurfaceTool
#define ThePSurfaceTool_hxx <asiAlgo_HlrSurfaceTool.h>
#define TheCurve gp_Lin
#define TheCurve_hxx <gp_Lin.hxx>
#define TheCurveTool LineTool
#define TheCurveTool_hxx <asiAlgo_HlrLineTool.h>
#define TheFunction CSFunctionOfInterCSurf
#define TheFunction_hxx <asiAlgo_HlrCSFunctionOfInterCSurf.h>
#define IntImp_IntCS ExactInterCSurf
#define IntImp_IntCS_hxx <asiAlgo_HlrExactInterCSurf.h>
#include <IntImp_IntCS.gxx>

