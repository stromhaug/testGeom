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

#ifndef asiAlgo_HLRBRep_HeaderFile
#define asiAlgo_HLRBRep_HeaderFile

// asiAlgo includes
#include <asiAlgo.h>

// OpenCascade includes
#include <Standard_TypeDef.hxx>

class TopoDS_Edge;

namespace asiAlgo {
namespace hlr {

class Curve;

//! Hidden Line Removal algorithms on B-rep.
//!
//! The class PolyAlgo  is used to remove Hidden lines
//! on Shapes with Triangulations.
class HLR
{
public:

  asiAlgo_EXPORT static TopoDS_Edge
    MakeEdge(const Curve& ec, const Standard_Real U1, const Standard_Real U2);

  asiAlgo_EXPORT static TopoDS_Edge
    MakeEdge3d(const Curve& ec, const Standard_Real U1, const Standard_Real U2);

  asiAlgo_EXPORT static void
    PolyHLRAngleAndDeflection(const Standard_Real InAngl, Standard_Real& OutAngl, Standard_Real& OutDefl);

private:

friend class CurveTool;
friend class BCurveTool;
friend class Curve;
friend class SurfaceTool;
friend class BSurfaceTool;
friend class Surface;
friend class CLPropsATool;
friend class CLProps;
friend class SLPropsATool;
friend class SLProps;
friend class CInter;
friend class LineTool;
friend class InterCSurf;
friend class EdgeFaceTool;
friend class Intersector;
friend class EdgeData;
friend class FaceData;
friend class FaceIterator;
friend class Data;
friend class ShapeToHlr;
friend class HlrToShape;
friend class ShapeBounds;
friend class EdgeInterferenceTool;
friend class VertexList;
friend class EdgeIList;
friend class AreaLimit;
friend class EdgeBuilder;
friend class Hider;
friend class InternalAlgo;
friend class PreciseAlgo;
friend class PolyAlgo;
friend class BiPoint;
friend class BiPnt2d;
friend class PolyHlrToShape;
friend class CurveLocatorOfProjPCurOfCInter;
friend class PCLocFOfLocateExtPCOfProjPCurOfCInter;
friend class LocateExtPCOfProjPCurOfCInter;
friend class ProjPCurOfCInter;
friend class ImpParToolOfIntersectorOfIntConicCurveOfCInter;
friend class IntersectorOfIntConicCurveOfCInter;
friend class IntConicCurveOfCInter;
friend class IntConicCurveOfCInter;
friend class Polygon2dOfIntPCurvePCurveOfCInter;
friend class DistBetweenPCurvesOfIntPCurvePCurveOfCInter;
friend class ExactIntersectionPointOfIntPCurvePCurveOfCInter;
friend class IntPCurvePCurveOfCInter;
friend class PolygonOfInterCSurf;
friend class PolygonToolOfInterCSurf;
friend class PolyhedronOfInterCSurf;
friend class PolyhedronToolOfInterCSurf;
friend class InterferenceOfInterCSurf;
friend class CSFunctionOfInterCSurf;
friend class ExactInterCSurf;
friend class QuadCurvFuncOfQuadCurvExactInterCSurf;
friend class QuadCurvExactInterCSurf;

};

}
}

#endif
