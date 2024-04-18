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

#include <asiAlgo_HlrInterferenceOfInterCSurf.h>

#include <asiAlgo_HlrPolygonOfInterCSurf.h>
#include <asiAlgo_HlrPolygonToolOfInterCSurf.h>
#include <asiAlgo_HlrPolyhedronOfInterCSurf.h>
#include <asiAlgo_HlrPolyhedronToolOfInterCSurf.h>

#include <gp_Lin.hxx>
#include <Bnd_BoundSortBox.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>

using namespace asiAlgo::hlr;

//-----------------------------------------------------------------------------

#define Polygon3d PolygonOfInterCSurf
#define Polygon3d_hxx <asiAlgo_HlrPolygonOfInterCSurf.h>
#define ToolPolygon3d PolygonToolOfInterCSurf
#define ToolPolygon3d_hxx <asiAlgo_HlrPolygonToolOfInterCSurf.h>
#define Polyhedron PolyhedronOfInterCSurf
#define Polyhedron_hxx <asiAlgo_HLRBRep_ThePolyhedronOfInterCSurf.h>
#define ToolPolyh PolyhedronToolOfInterCSurf
#define ToolPolyh_hxx <asiAlgo_HlrPolyhedronToolOfInterCSurf.h>
#define Intf_InterferencePolygonPolyhedron InterferenceOfInterCSurf
#define Intf_InterferencePolygonPolyhedron_hxx <asiAlgo_HlrInterferenceOfInterCSurf.h>
#include <Intf_InterferencePolygonPolyhedron.gxx>

