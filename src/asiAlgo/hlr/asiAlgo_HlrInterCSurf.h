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

#ifndef asiAlgo_HLRBRep_InterCSurf_HeaderFile
#define asiAlgo_HLRBRep_InterCSurf_HeaderFile

// asiAlgo includes
#include <asiAlgo.h>

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <IntCurveSurface_Intersection.hxx>
#include <Standard_Address.hxx>
#include <Standard_Real.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>

class gp_Lin;
class Bnd_BoundSortBox;
class gp_Circ;
class gp_Elips;
class gp_Parab;
class gp_Hypr;
class IntAna_IntConicQuad;
class Bnd_Box;

namespace asiAlgo {
namespace hlr {

class LineTool;
class SurfaceTool;
class PolygonOfInterCSurf;
class PolygonToolOfInterCSurf;
class PolyhedronOfInterCSurf;
class PolyhedronToolOfInterCSurf;
class InterferenceOfInterCSurf;
class CSFunctionOfInterCSurf;
class ExactInterCSurf;
class QuadCurvExactInterCSurf;
class QuadCurvFuncOfQuadCurvExactInterCSurf;

class InterCSurf : public IntCurveSurface_Intersection
{
public:

  DEFINE_STANDARD_ALLOC

  //! Empty Constructor
  asiAlgo_EXPORT InterCSurf();

  //! Compute the Intersection between the curve and the
  //! surface
  asiAlgo_EXPORT void Perform (const gp_Lin& Curve, const Standard_Address& Surface);

  //! Compute the Intersection  between the curve  and
  //! the surface. The   Curve is already  sampled and
  //! its polygon : <Polygon> is given.
  asiAlgo_EXPORT void Perform (const gp_Lin& Curve, const PolygonOfInterCSurf& Polygon, const Standard_Address& Surface);

  //! Compute the Intersection  between the curve  and
  //! the surface. The   Curve is already  sampled and
  //! its polygon : <Polygon> is given. The Surface is
  //! also sampled and <Polyhedron> is given.
  asiAlgo_EXPORT void Perform (const gp_Lin& Curve, const PolygonOfInterCSurf& ThePolygon, const Standard_Address& Surface, const PolyhedronOfInterCSurf& Polyhedron);

  //! Compute the Intersection  between the curve  and
  //! the surface. The   Curve is already  sampled and
  //! its polygon : <Polygon> is given. The Surface is
  //! also sampled and <Polyhedron> is given.
  asiAlgo_EXPORT void Perform (const gp_Lin& Curve, const PolygonOfInterCSurf& ThePolygon, const Standard_Address& Surface, const PolyhedronOfInterCSurf& Polyhedron, Bnd_BoundSortBox& BndBSB);

  //! Compute the Intersection  between the curve  and
  //! the surface. The Surface is already  sampled and
  //! its polyhedron : <Polyhedron> is given.
  asiAlgo_EXPORT void Perform (const gp_Lin& Curve, const Standard_Address& Surface, const PolyhedronOfInterCSurf& Polyhedron);

protected:

  //! Compute the Intersection between the curve and the
  //! surface
  asiAlgo_EXPORT void Perform (const gp_Lin& Curve, const Standard_Address& Surface, const Standard_Real U0, const Standard_Real V0, const Standard_Real U1, const Standard_Real V1);

  asiAlgo_EXPORT void InternalPerformCurveQuadric (const gp_Lin& Curve, const Standard_Address& Surface);

  asiAlgo_EXPORT void InternalPerform (const gp_Lin& Curve, const PolygonOfInterCSurf& Polygon, const Standard_Address& Surface, const PolyhedronOfInterCSurf& Polyhedron, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2);

  asiAlgo_EXPORT void InternalPerform (const gp_Lin& Curve, const PolygonOfInterCSurf& Polygon, const Standard_Address& Surface, const PolyhedronOfInterCSurf& Polyhedron, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2, Bnd_BoundSortBox& BSB);

  asiAlgo_EXPORT void InternalPerform (const gp_Lin& Curve, const PolygonOfInterCSurf& Polygon, const Standard_Address& Surface, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2);

  asiAlgo_EXPORT void PerformConicSurf (const gp_Lin& Line, const gp_Lin& Curve, const Standard_Address& Surface, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2);

  asiAlgo_EXPORT void PerformConicSurf (const gp_Circ& Circle, const gp_Lin& Curve, const Standard_Address& Surface, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2);

  asiAlgo_EXPORT void PerformConicSurf (const gp_Elips& Ellipse, const gp_Lin& Curve, const Standard_Address& Surface, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2);

  asiAlgo_EXPORT void PerformConicSurf (const gp_Parab& Parab, const gp_Lin& Curve, const Standard_Address& Surface, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2);

  asiAlgo_EXPORT void PerformConicSurf (const gp_Hypr& Hyper, const gp_Lin& Curve, const Standard_Address& Surface, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2);

  asiAlgo_EXPORT void AppendIntAna (const gp_Lin& Curve, const Standard_Address& Surface, const IntAna_IntConicQuad& InterAna);

  asiAlgo_EXPORT void AppendPoint (const gp_Lin& Curve, const Standard_Real w, const Standard_Address& Surface, const Standard_Real u, const Standard_Real v);

  asiAlgo_EXPORT void AppendSegment (const gp_Lin& Curve, const Standard_Real u0, const Standard_Real u1, const Standard_Address& Surface);

private:

  asiAlgo_EXPORT void DoSurface (const Standard_Address& surface, const Standard_Real u0, const Standard_Real u1, const Standard_Real v0, const Standard_Real v1, TColgp_Array2OfPnt& pntsOnSurface, Bnd_Box& boxSurface, Standard_Real& gap);

  asiAlgo_EXPORT void DoNewBounds (const Standard_Address& surface, const Standard_Real u0, const Standard_Real u1, const Standard_Real v0, const Standard_Real v1, const TColgp_Array2OfPnt& pntsOnSurface, const TColStd_Array1OfReal& X, const TColStd_Array1OfReal& Y, const TColStd_Array1OfReal& Z, TColStd_Array1OfReal& Bounds);

};

}
}

#endif // _HLRBRep_InterCSurf_HeaderFile
