// Created on: 1993-07-02
// Created by: Laurent BUCHARD
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef asiAlgo_SurfaceTool_HeaderFile
#define asiAlgo_SurfaceTool_HeaderFile

// asiAlgo includes
#include <asiAlgo.h>

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <GeomAbs_Shape.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <gp_Pln.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Cone.hxx>
#include <gp_Torus.hxx>
#include <gp_Sphere.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>

class gp_Pnt;
class gp_Vec;
class Geom_BezierSurface;
class Geom_BSplineSurface;

namespace asiAlgo {
namespace hlr {

class SurfaceTool
{
public:

  DEFINE_STANDARD_ALLOC


  asiAlgo_EXPORT static Standard_Real FirstUParameter (const Standard_Address S);

  asiAlgo_EXPORT static Standard_Real FirstVParameter (const Standard_Address S);

  asiAlgo_EXPORT static Standard_Real LastUParameter (const Standard_Address S);

  asiAlgo_EXPORT static Standard_Real LastVParameter (const Standard_Address S);

  asiAlgo_EXPORT static Standard_Integer NbUIntervals (const Standard_Address S, const GeomAbs_Shape Sh);

  asiAlgo_EXPORT static Standard_Integer NbVIntervals (const Standard_Address S, const GeomAbs_Shape Sh);

  asiAlgo_EXPORT static void UIntervals (const Standard_Address S, TColStd_Array1OfReal& T, const GeomAbs_Shape Sh);

  asiAlgo_EXPORT static void VIntervals (const Standard_Address S, TColStd_Array1OfReal& T, const GeomAbs_Shape Sh);

  //! If <First> >= <Last>
  asiAlgo_EXPORT static Handle(Adaptor3d_Surface) UTrim (const Standard_Address S, const Standard_Real First, const Standard_Real Last, const Standard_Real Tol);

  //! If <First> >= <Last>
  asiAlgo_EXPORT static Handle(Adaptor3d_Surface) VTrim (const Standard_Address S, const Standard_Real First, const Standard_Real Last, const Standard_Real Tol);

  asiAlgo_EXPORT static Standard_Boolean IsUClosed (const Standard_Address S);

  asiAlgo_EXPORT static Standard_Boolean IsVClosed (const Standard_Address S);

  asiAlgo_EXPORT static Standard_Boolean IsUPeriodic (const Standard_Address S);

  asiAlgo_EXPORT static Standard_Real UPeriod (const Standard_Address S);

  asiAlgo_EXPORT static Standard_Boolean IsVPeriodic (const Standard_Address S);

  asiAlgo_EXPORT static Standard_Real VPeriod (const Standard_Address S);

  asiAlgo_EXPORT static gp_Pnt Value (const Standard_Address S, const Standard_Real u, const Standard_Real v);

  asiAlgo_EXPORT static void D0 (const Standard_Address S, const Standard_Real u, const Standard_Real v, gp_Pnt& P);

  asiAlgo_EXPORT static void D1 (const Standard_Address S, const Standard_Real u, const Standard_Real v, gp_Pnt& P, gp_Vec& D1u, gp_Vec& D1v);

  asiAlgo_EXPORT static void D2 (const Standard_Address S, const Standard_Real u, const Standard_Real v, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV);

  asiAlgo_EXPORT static void D3 (const Standard_Address S, const Standard_Real u, const Standard_Real v, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV, gp_Vec& D3U, gp_Vec& D3V, gp_Vec& D3UUV, gp_Vec& D3UVV);

  asiAlgo_EXPORT static gp_Vec DN (const Standard_Address S, const Standard_Real u, const Standard_Real v, const Standard_Integer Nu, const Standard_Integer Nv);

  asiAlgo_EXPORT static Standard_Real UResolution (const Standard_Address S, const Standard_Real R3d);

  asiAlgo_EXPORT static Standard_Real VResolution (const Standard_Address S, const Standard_Real R3d);

  asiAlgo_EXPORT static GeomAbs_SurfaceType GetType (const Standard_Address S);

  asiAlgo_EXPORT static gp_Pln Plane (const Standard_Address S);

  asiAlgo_EXPORT static gp_Cylinder Cylinder (const Standard_Address S);

  asiAlgo_EXPORT static gp_Cone Cone (const Standard_Address S);

  asiAlgo_EXPORT static gp_Torus Torus (const Standard_Address S);

  asiAlgo_EXPORT static gp_Sphere Sphere (const Standard_Address S);

  asiAlgo_EXPORT static Handle(Geom_BezierSurface) Bezier (const Standard_Address S);

  asiAlgo_EXPORT static Handle(Geom_BSplineSurface) BSpline (const Standard_Address S);

  asiAlgo_EXPORT static gp_Ax1 AxeOfRevolution (const Standard_Address S);

  asiAlgo_EXPORT static gp_Dir Direction (const Standard_Address S);

  asiAlgo_EXPORT static Handle(Adaptor3d_Curve) BasisCurve (const Standard_Address S);

  asiAlgo_EXPORT static Handle(Adaptor3d_Surface) BasisSurface (const Standard_Address S);

  asiAlgo_EXPORT static Standard_Real OffsetValue (const Standard_Address S);

  asiAlgo_EXPORT static Standard_Integer NbSamplesU (const Standard_Address S);

  asiAlgo_EXPORT static Standard_Integer NbSamplesV (const Standard_Address S);

  asiAlgo_EXPORT static Standard_Integer NbSamplesU (const Standard_Address S, const Standard_Real u1, const Standard_Real u2);

  asiAlgo_EXPORT static Standard_Integer NbSamplesV (const Standard_Address S, const Standard_Real v1, const Standard_Real v2);

};


inline Standard_Real SurfaceTool::FirstUParameter(const Standard_Address Surf){  return ((BRepAdaptor_Surface *)Surf)->FirstUParameter(); }
inline Standard_Real SurfaceTool::FirstVParameter(const Standard_Address Surf){  return ((BRepAdaptor_Surface *)Surf)->FirstVParameter();}
inline Standard_Real SurfaceTool::LastUParameter(const Standard_Address Surf){  return ((BRepAdaptor_Surface *)Surf)->LastUParameter();}
inline Standard_Real SurfaceTool::LastVParameter(const Standard_Address Surf){  return ((BRepAdaptor_Surface *)Surf)->LastVParameter();}

inline Standard_Integer SurfaceTool::NbUIntervals(const Standard_Address Surf,
							  const GeomAbs_Shape S){
  return ((BRepAdaptor_Surface *)Surf)->NbUIntervals(S);
}

inline Standard_Integer SurfaceTool::NbVIntervals(const Standard_Address Surf,
							  const GeomAbs_Shape S){
  return ((BRepAdaptor_Surface *)Surf)->NbVIntervals(S);
}

inline void SurfaceTool::UIntervals(const Standard_Address Surf,
							TColStd_Array1OfReal& Tab,
							const GeomAbs_Shape S){
  ((BRepAdaptor_Surface *)Surf)->UIntervals(Tab,S);
}

inline void SurfaceTool::VIntervals(const Standard_Address Surf,
						     TColStd_Array1OfReal& Tab,
						     const GeomAbs_Shape S){
  ((BRepAdaptor_Surface *)Surf)->VIntervals(Tab,S);
}


inline Handle(Adaptor3d_Surface) SurfaceTool::UTrim(const Standard_Address Surf,
								   const Standard_Real F,
								   const Standard_Real L,
								   const Standard_Real Tol) { 
  return ((BRepAdaptor_Surface *)Surf)->UTrim(F,L,Tol);
}

inline Handle(Adaptor3d_Surface) SurfaceTool::VTrim(const Standard_Address Surf,
								   const Standard_Real F,
								   const Standard_Real L,
								   const Standard_Real Tol) { 
  return ((BRepAdaptor_Surface *)Surf)->VTrim(F,L,Tol);
}




inline Standard_Boolean SurfaceTool::IsUClosed(const Standard_Address S)
{
  return ((BRepAdaptor_Surface *)S)->IsUClosed();
}

inline Standard_Boolean SurfaceTool::IsVClosed(const Standard_Address S)
{
  return ((BRepAdaptor_Surface *)S)->IsVClosed();
}

inline Standard_Boolean SurfaceTool::IsUPeriodic(const Standard_Address S)
{
  return ((BRepAdaptor_Surface *)S)->IsUPeriodic();
}

inline Standard_Real SurfaceTool::UPeriod(const Standard_Address S)
{
  return ((BRepAdaptor_Surface *)S)->UPeriod();
}

inline Standard_Boolean SurfaceTool::IsVPeriodic(const Standard_Address S)
{
  return ((BRepAdaptor_Surface *)S)->IsVPeriodic();
}

inline Standard_Real SurfaceTool::VPeriod(const Standard_Address S)
{
  return ((BRepAdaptor_Surface *)S)->VPeriod();
}

inline gp_Pnt SurfaceTool::Value(const Standard_Address S,
					 const Standard_Real U,
					 const Standard_Real V )
{
  return ((BRepAdaptor_Surface *)S)->Value(U,V);
}

inline void SurfaceTool::D0(const Standard_Address S,
				    const Standard_Real U,
				    const Standard_Real V,
				    gp_Pnt& P)
{
  ((BRepAdaptor_Surface *)S)->D0(U,V,P);
}

inline void SurfaceTool::D1(const Standard_Address S,
				    const Standard_Real U,
				    const Standard_Real V,
				    gp_Pnt& P,
				    gp_Vec& D1U,
				    gp_Vec& D1V) 
{
  ((BRepAdaptor_Surface *)S)->D1(U,V,P,D1U,D1V);
}

inline void SurfaceTool::D2(const Standard_Address S,
				    const Standard_Real U,
				    const Standard_Real V,
				    gp_Pnt& P,
				    gp_Vec& D1U,
				    gp_Vec& D1V,
				    gp_Vec& D2U,
				    gp_Vec& D2V,
				    gp_Vec& D2UV)
{
  ((BRepAdaptor_Surface *)S)->D2(U,V,P,D1U,D1V,D2U,D2V,D2UV);
}

inline void SurfaceTool::D3(const Standard_Address S,
				    const Standard_Real U,
				    const Standard_Real V,
				    gp_Pnt& P,
				    gp_Vec& D1U,
				    gp_Vec& D1V, 
				    gp_Vec& D2U,
				    gp_Vec& D2V,
				    gp_Vec& D2UV,
				    gp_Vec& D3U,
				    gp_Vec& D3V,
				    gp_Vec& D3UUV,
				    gp_Vec& D3UVV)
{
  ((BRepAdaptor_Surface *)S)->D3(U,V,P,D1U,D1V,D2U,D2V,D2UV,D3U,D3V,D3UUV,D3UVV);
}

inline gp_Vec SurfaceTool::DN(const Standard_Address S,
				      const Standard_Real U,
				      const Standard_Real V,
				      const Standard_Integer Nu,
				      const Standard_Integer Nv)
{
  return ((BRepAdaptor_Surface *)S)->DN(U,V,Nu,Nv);
}

inline Standard_Real SurfaceTool::UResolution(const Standard_Address S,
						      const Standard_Real R3d)
{
  return ((BRepAdaptor_Surface *)S)->UResolution(R3d);
}

inline Standard_Real SurfaceTool::VResolution(const Standard_Address S,
						      const Standard_Real R3d)
{
  return ((BRepAdaptor_Surface *)S)->VResolution(R3d);
}

inline GeomAbs_SurfaceType SurfaceTool::GetType(const Standard_Address S )
{
  return ((BRepAdaptor_Surface *)S)->GetType();
}

inline gp_Pln SurfaceTool::Plane(const Standard_Address S)
{
  return ((BRepAdaptor_Surface *)S)->Plane();
}

inline gp_Cylinder SurfaceTool::Cylinder(const Standard_Address S) 
{
  return ((BRepAdaptor_Surface *)S)->Cylinder();
}

inline gp_Cone SurfaceTool::Cone(const Standard_Address S) 
{
  return ((BRepAdaptor_Surface *)S)->Cone();
}

inline gp_Sphere SurfaceTool::Sphere(const Standard_Address S) 
{
  return ((BRepAdaptor_Surface *)S)->Sphere();
}

inline gp_Torus SurfaceTool::Torus(const Standard_Address S) 
{
  return ((BRepAdaptor_Surface *)S)->Torus();
}


inline Handle(Geom_BezierSurface) SurfaceTool::Bezier(const Standard_Address S) { 
  return(((BRepAdaptor_Surface *)S)->Bezier());
}

inline Handle(Geom_BSplineSurface) SurfaceTool::BSpline(const Standard_Address S) { 
  return(((BRepAdaptor_Surface *)S)->BSpline());
}


inline gp_Ax1  SurfaceTool::AxeOfRevolution(const Standard_Address S) { 
  return(((BRepAdaptor_Surface *)S)->AxeOfRevolution());
}

inline gp_Dir  SurfaceTool::Direction(const Standard_Address S) { 
  return(((BRepAdaptor_Surface *)S)->Direction());
}

inline Handle(Adaptor3d_Curve)  SurfaceTool::BasisCurve(const Standard_Address S) { 
  return(((BRepAdaptor_Surface *)S)->BasisCurve());
}

inline Handle(Adaptor3d_Surface) SurfaceTool::BasisSurface(const Standard_Address S) { 
  return(((BRepAdaptor_Surface *)S)->BasisSurface());
}

inline Standard_Real SurfaceTool::OffsetValue(const Standard_Address S) { 
  return(((BRepAdaptor_Surface *)S)->OffsetValue());
}

}
}

#endif // _SurfaceTool_HeaderFile
