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

#include <asiAlgo_HlrPCLocFOfLocateExtPCOfProjPCurOfCInter.h>
#include <asiAlgo_HlrCurveTool.h>

#include <Standard_OutOfRange.hxx>
#include <Standard_TypeMismatch.hxx>
#include <Extrema_POnCurv2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Extrema_SequenceOfPOnCurv2d.hxx>

using namespace asiAlgo::hlr;

//-----------------------------------------------------------------------------

#define Curve Standard_Address
#define Curve_hxx <Standard_Address.hxx>
#define Tool CurveTool
#define Tool_hxx <asiAlgo_HlrCurveTool.h>
#define POnC Extrema_POnCurv2d
#define POnC_hxx <Extrema_POnCurv2d.hxx>
#define Pnt gp_Pnt2d
#define Pnt_hxx <gp_Pnt2d.hxx>
#define Vec gp_Vec2d
#define Vec_hxx <gp_Vec2d.hxx>
#define Extrema_SeqPC Extrema_SequenceOfPOnCurv2d
#define Extrema_SeqPC_hxx <Extrema_SequenceOfPOnCurv2d.hxx>
#define Extrema_FuncExtPC PCLocFOfLocateExtPCOfProjPCurOfCInter
#define Extrema_FuncExtPC_hxx <asiAlgo_HlrPCLocFOfLocateExtPCOfProjPCurOfCInter.h>

#include <Extrema_FuncExtPC.gxx>
