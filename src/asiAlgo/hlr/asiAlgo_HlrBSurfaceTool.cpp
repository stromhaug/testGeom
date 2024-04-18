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

#include <asiAlgo_HlrBSurfaceTool.h>

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>

using namespace asiAlgo::hlr;

//=======================================================================
//function : FirstUParameter
//purpose  :
//=======================================================================

Standard_Real BSurfaceTool::FirstUParameter(const BRepAdaptor_Surface& Surf)
{ return Surf.FirstUParameter(); }

//=======================================================================
//function : FirstVParameter
//purpose  :
//=======================================================================

Standard_Real BSurfaceTool::FirstVParameter(const BRepAdaptor_Surface& Surf)
{ return Surf.FirstVParameter(); }

//=======================================================================
//function : LastUParameter
//purpose  :
//=======================================================================

Standard_Real BSurfaceTool::LastUParameter(const BRepAdaptor_Surface& Surf)
{ return Surf.LastUParameter(); }

//=======================================================================
//function : LastVParameter
//purpose  :
//=======================================================================

Standard_Real BSurfaceTool::LastVParameter(const BRepAdaptor_Surface& Surf)
{ return Surf.LastVParameter(); }

//=======================================================================
//function : NbUIntervals
//purpose  :
//=======================================================================

Standard_Integer BSurfaceTool::NbUIntervals(const BRepAdaptor_Surface& Surf,
				   const GeomAbs_Shape S)
{ return Surf.NbUIntervals(S); }

//=======================================================================
//function : NbVIntervals
//purpose  :
//=======================================================================

Standard_Integer BSurfaceTool::NbVIntervals(const BRepAdaptor_Surface& Surf,
				   const GeomAbs_Shape S)
{ return Surf.NbVIntervals(S); }

//=======================================================================
//function : UIntervals
//purpose  :
//=======================================================================

void BSurfaceTool::UIntervals(const BRepAdaptor_Surface& Surf,
				 TColStd_Array1OfReal& Tab,
				 const GeomAbs_Shape S)
{ Surf.UIntervals(Tab,S); }

//=======================================================================
//function : VIntervals
//purpose  :
//=======================================================================

void BSurfaceTool::VIntervals(const BRepAdaptor_Surface& Surf,
                              TColStd_Array1OfReal& Tab,
                              const GeomAbs_Shape S)
{ Surf.VIntervals(Tab,S); }


//=======================================================================
//function : UTrim
//purpose  :
//=======================================================================

Handle(Adaptor3d_Surface)
  BSurfaceTool::UTrim(const BRepAdaptor_Surface& Surf,
                      const Standard_Real F,
                      const Standard_Real L,
                      const Standard_Real Tol)
{ return Surf.UTrim(F,L,Tol); }

//=======================================================================
//function : VTrim
//purpose  :
//=======================================================================

Handle(Adaptor3d_Surface)
  BSurfaceTool::VTrim(const BRepAdaptor_Surface& Surf,
                      const Standard_Real F,
                      const Standard_Real L,
                      const Standard_Real Tol)
{ return Surf.VTrim(F,L,Tol); }

//=======================================================================
//function : IsUClosed
//purpose  :
//=======================================================================

Standard_Boolean
  BSurfaceTool::IsUClosed(const BRepAdaptor_Surface& S)
{ return S.IsUClosed(); }

//=======================================================================
//function : IsVClosed
//purpose  :
//=======================================================================

Standard_Boolean
  BSurfaceTool::IsVClosed(const BRepAdaptor_Surface& S)
{ return S.IsVClosed(); }

//=======================================================================
//function : IsUPeriodic
//purpose  :
//=======================================================================

Standard_Boolean
  BSurfaceTool::IsUPeriodic(const BRepAdaptor_Surface& S)
{ return S.IsUPeriodic(); }

//=======================================================================
//function : UPeriod
//purpose  :
//=======================================================================

Standard_Real
  BSurfaceTool::UPeriod(const BRepAdaptor_Surface& S)
{ return S.UPeriod(); }

//=======================================================================
//function : IsVPeriodic
//purpose  :
//=======================================================================

Standard_Boolean
  BSurfaceTool::IsVPeriodic(const BRepAdaptor_Surface& S)
{ return S.IsVPeriodic(); }

//=======================================================================
//function : VPeriod
//purpose  :
//=======================================================================

Standard_Real
  BSurfaceTool::VPeriod(const BRepAdaptor_Surface& S)
{ return S.VPeriod(); }

//=======================================================================
//function : Value
//purpose  :
//=======================================================================

gp_Pnt
  BSurfaceTool::Value(const BRepAdaptor_Surface& S,
                      const Standard_Real U,
                      const Standard_Real V)
{ return S.Value(U,V); }

//=======================================================================
//function : D0
//purpose  :
//=======================================================================

void
BSurfaceTool::D0(const BRepAdaptor_Surface& S,
                 const Standard_Real U,
                 const Standard_Real V,
                 gp_Pnt& P)
{ S.D0(U,V,P); }

//=======================================================================
//function : D1
//purpose  :
//=======================================================================

void
BSurfaceTool::D1(const BRepAdaptor_Surface& S,
                 const Standard_Real U,
                 const Standard_Real V,
                 gp_Pnt& P,
                 gp_Vec& D1U,
                 gp_Vec& D1V)
{ S.D1(U,V,P,D1U,D1V); }

//=======================================================================
//function : D2
//purpose  :
//=======================================================================

 void
BSurfaceTool::D2(const BRepAdaptor_Surface& S,
			 const Standard_Real U,
			 const Standard_Real V,
			 gp_Pnt& P,
			 gp_Vec& D1U,
			 gp_Vec& D1V,
			 gp_Vec& D2U,
			 gp_Vec& D2V,
			 gp_Vec& D2UV)
{ S.D2(U,V,P,D1U,D1V,D2U,D2V,D2UV); }

//=======================================================================
//function : D3
//purpose  :
//=======================================================================

 void
BSurfaceTool::D3(const BRepAdaptor_Surface& S,
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
{ S.D3(U,V,P,D1U,D1V,D2U,D2V,D2UV,D3U,D3V,D3UUV,D3UVV); }

//=======================================================================
//function : DN
//purpose  :
//=======================================================================

 gp_Vec
BSurfaceTool::DN(const BRepAdaptor_Surface& S,
			 const Standard_Real U,
			 const Standard_Real V,
			 const Standard_Integer Nu,
			 const Standard_Integer Nv)
{ return S.DN(U,V,Nu,Nv); }

//=======================================================================
//function : UResolution
//purpose  :
//=======================================================================

 Standard_Real
BSurfaceTool::UResolution(const BRepAdaptor_Surface& S,
				  const Standard_Real R3d)
{ return S.UResolution(R3d); }

//=======================================================================
//function : VResolution
//purpose  :
//=======================================================================

 Standard_Real
BSurfaceTool::VResolution(const BRepAdaptor_Surface& S,
				  const Standard_Real R3d)
{ return S.VResolution(R3d); }

//=======================================================================
//function : GetType
//purpose  :
//=======================================================================

 GeomAbs_SurfaceType
BSurfaceTool::GetType(const BRepAdaptor_Surface& S )
{ return S.GetType(); }

//=======================================================================
//function : Plane
//purpose  :
//=======================================================================

 gp_Pln
BSurfaceTool::Plane(const BRepAdaptor_Surface& S)
{ return S.Plane(); }

//=======================================================================
//function : Cylinder
//purpose  :
//=======================================================================

 gp_Cylinder
BSurfaceTool::Cylinder(const BRepAdaptor_Surface& S)
{ return S.Cylinder(); }

//=======================================================================
//function : Cone
//purpose  :
//=======================================================================

 gp_Cone
BSurfaceTool::Cone(const BRepAdaptor_Surface& S)
{ return S.Cone(); }

//=======================================================================
//function : Sphere
//purpose  :
//=======================================================================

 gp_Sphere
BSurfaceTool::Sphere(const BRepAdaptor_Surface& S)
{ return S.Sphere(); }

//=======================================================================
//function : Torus
//purpose  :
//=======================================================================

 gp_Torus
BSurfaceTool::Torus(const BRepAdaptor_Surface& S)
{ return S.Torus(); }

//=======================================================================
//function : Bezier
//purpose  :
//=======================================================================

 Handle(Geom_BezierSurface)
     BSurfaceTool::Bezier(const BRepAdaptor_Surface& S)
{ return(S.Bezier()); }

//=======================================================================
//function : BSpline
//purpose  :
//=======================================================================

 Handle(Geom_BSplineSurface)
     BSurfaceTool::BSpline(const BRepAdaptor_Surface& S)
{ return(S.BSpline()); }

//=======================================================================
//function : AxeOfRevolution
//purpose  :
//=======================================================================

 gp_Ax1
BSurfaceTool::AxeOfRevolution(const BRepAdaptor_Surface& S)
{ return(S.AxeOfRevolution()); }

//=======================================================================
//function : Direction
//purpose  :
//=======================================================================

 gp_Dir
BSurfaceTool::Direction(const BRepAdaptor_Surface& S)
{ return(S.Direction()); }

//=======================================================================
//function : BasisCurve
//purpose  :
//=======================================================================

 Handle(Adaptor3d_Curve)
     BSurfaceTool::BasisCurve(const BRepAdaptor_Surface& S)
{ return(S.BasisCurve()); }

//=======================================================================
//function : UContinuity
//purpose  :
//=======================================================================

 GeomAbs_Shape
BSurfaceTool::UContinuity(const BRepAdaptor_Surface& S)
{ return(S.UContinuity()); }

//=======================================================================
//function : VContinuity
//purpose  :
//=======================================================================

 GeomAbs_Shape
BSurfaceTool::VContinuity(const BRepAdaptor_Surface& S)
{ return(S.VContinuity()); }

//=======================================================================
//function : UDegree
//purpose  :
//=======================================================================

 Standard_Integer
BSurfaceTool::UDegree(const BRepAdaptor_Surface& S)
{ return(S.UDegree()); }

//=======================================================================
//function : NbUPoles
//purpose  :
//=======================================================================

 Standard_Integer
BSurfaceTool::NbUPoles(const BRepAdaptor_Surface& S)
{ return(S.NbUPoles()); }

//=======================================================================
//function : NbUKnots
//purpose  :
//=======================================================================

 Standard_Integer
BSurfaceTool::NbUKnots(const BRepAdaptor_Surface& S)
{ return(S.NbUKnots()); }

//=======================================================================
//function : IsURational
//purpose  :
//=======================================================================

 Standard_Boolean
BSurfaceTool::IsURational(const BRepAdaptor_Surface& S)
{ return(S.IsURational()); }

//=======================================================================
//function : VDegree
//purpose  :
//=======================================================================

 Standard_Integer
BSurfaceTool::VDegree(const BRepAdaptor_Surface& S)
{ return(S.VDegree()); }

//=======================================================================
//function : NbVPoles
//purpose  :
//=======================================================================

 Standard_Integer
BSurfaceTool::NbVPoles(const BRepAdaptor_Surface& S)
{ return(S.NbVPoles()); }

//=======================================================================
//function : NbVKnots
//purpose  :
//=======================================================================

 Standard_Integer
BSurfaceTool::NbVKnots(const BRepAdaptor_Surface& S)
{ return(S.NbVKnots()); }

//=======================================================================
//function : IsVRational
//purpose  :
//=======================================================================

 Standard_Boolean
BSurfaceTool::IsVRational(const BRepAdaptor_Surface& S)
{ return(S.IsVRational()); }


//=======================================================================
//function : NbSamplesU
//purpose  :
//=======================================================================
Standard_Integer
BSurfaceTool::NbSamplesU(const BRepAdaptor_Surface& S)
{
  Standard_Integer nbs;
  GeomAbs_SurfaceType typS = S.GetType();
  switch(typS) {
  case GeomAbs_Plane:
    {
      nbs = 2;
    }
    break;
  case GeomAbs_BezierSurface:
    {
      nbs =  3 + S.NbUPoles();
    }
    break;
  case GeomAbs_BSplineSurface:
    {
      nbs = S.NbUKnots();
      nbs*= S.UDegree();
      if(nbs < 2) nbs=2;

    }
    break;
  case GeomAbs_Torus:
    {
      nbs = 20;
    }
    break;
  case GeomAbs_Cylinder:
  case GeomAbs_Cone:
  case GeomAbs_Sphere:
  case GeomAbs_SurfaceOfRevolution:
  case GeomAbs_SurfaceOfExtrusion:
    {
      nbs = 10;
    }
    break;

  default:
    {
      nbs = 10;
    }
    break;
  }
  return(nbs);
}

//=======================================================================
//function : NbSamplesV
//purpose  :
//=======================================================================

Standard_Integer
BSurfaceTool::NbSamplesV(const BRepAdaptor_Surface& S)
{
  Standard_Integer nbs;
  GeomAbs_SurfaceType typS = S.GetType();
  switch(typS) {
  case GeomAbs_Plane:
    {
      nbs = 2;
    }
    break;
  case GeomAbs_BezierSurface:
    {
      nbs =  3 + S.NbVPoles();
    }
    break;
  case GeomAbs_BSplineSurface:
    {
      nbs = S.NbVKnots();
      nbs*= S.VDegree();
      if(nbs < 2) nbs=2;

    }
    break;
  case GeomAbs_Cylinder:
  case GeomAbs_Cone:
  case GeomAbs_Sphere:
  case GeomAbs_Torus:
  case GeomAbs_SurfaceOfRevolution:
  case GeomAbs_SurfaceOfExtrusion:
    {
      nbs = 15;
    }
    break;

  default:
    {
      nbs = 10;
    }
    break;
  }
  return(nbs);
}

//=======================================================================
//function : NbSamplesU
//purpose  :
//=======================================================================

Standard_Integer
BSurfaceTool::NbSamplesU(const BRepAdaptor_Surface& S,
				 const Standard_Real u1,
				 const Standard_Real u2)
{
  Standard_Integer nbs = NbSamplesU(S);
  Standard_Integer n = nbs;
  if(nbs>10) {
    Standard_Real uf = FirstUParameter(S);
    Standard_Real ul = LastUParameter(S);
    n*= (Standard_Integer)((u2-u1)/(uf-ul));
    if(n>nbs) n = nbs;
    if(n<5)   n = 5;
  }
  return(n);
}

//=======================================================================
//function : NbSamplesV
//purpose  :
//=======================================================================

Standard_Integer
BSurfaceTool::NbSamplesV(const BRepAdaptor_Surface& S,
				 const Standard_Real v1,
				 const Standard_Real v2)
{
  Standard_Integer nbs = NbSamplesV(S);
  Standard_Integer n = nbs;
  if(nbs>10) {
    Standard_Real vf = FirstVParameter(S);
    Standard_Real vl = LastVParameter(S);
    n*= (Standard_Integer)((v2-v1)/(vf-vl));
    if(n>nbs) n = nbs;
    if(n<5)   n = 5;
  }
  return(n);
}
