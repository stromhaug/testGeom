// Created on: 1993-04-14
// Created by: Modelistation
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

#ifndef asiAlgo_HLRBRep_Surface_HeaderFile
#define asiAlgo_HLRBRep_Surface_HeaderFile

#include <asiAlgo_HlrCurve.h>
#include <asiAlgo_HlrBSurfaceTool.h>

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepAdaptor_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp_Vec.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Cone.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <gp_Ax1.hxx>
class TopoDS_Face;
class gp_Pnt;
class gp_Vec;
class gp_Pln;

namespace asiAlgo {
namespace hlr {

class Surface
{
public:

  DEFINE_STANDARD_ALLOC


  //! Creates an undefined surface with no face loaded.
  asiAlgo_EXPORT Surface();

  void Projector (const HLRAlgo_Projector* Proj)
  {
    myProj = Proj;
  }

  //! Returns the 3D Surface.
  asiAlgo_EXPORT BRepAdaptor_Surface& AccessSurface();

  //! Sets the 3D Surface to be projected.
  asiAlgo_EXPORT void SetSurface (const TopoDS_Face& F);

  //! returns true if it is a side face
  asiAlgo_EXPORT Standard_Boolean IsSide (const Standard_Real tolf, const Standard_Real toler) const;

  asiAlgo_EXPORT Standard_Boolean IsAbove (const Standard_Boolean back, const Curve* A, const Standard_Real tolC) const;

  asiAlgo_EXPORT Standard_Real FirstUParameter() const;

  asiAlgo_EXPORT Standard_Real LastUParameter() const;

  asiAlgo_EXPORT Standard_Real FirstVParameter() const;

  asiAlgo_EXPORT Standard_Real LastVParameter() const;

  asiAlgo_EXPORT GeomAbs_Shape UContinuity() const;

  asiAlgo_EXPORT GeomAbs_Shape VContinuity() const;

  //! If necessary, breaks the surface in U intervals of
  //! continuity    <S>.  And   returns  the  number  of
  //! intervals.
  asiAlgo_EXPORT Standard_Integer NbUIntervals (const GeomAbs_Shape S);

  //! If necessary, breaks the surface in V intervals of
  //! continuity    <S>.  And   returns  the  number  of
  //! intervals.
  asiAlgo_EXPORT Standard_Integer NbVIntervals (const GeomAbs_Shape S);

  asiAlgo_EXPORT GeomAbs_Shape UIntervalContinuity() const;

  asiAlgo_EXPORT GeomAbs_Shape VIntervalContinuity() const;

  asiAlgo_EXPORT Standard_Boolean IsUClosed() const;

  asiAlgo_EXPORT Standard_Boolean IsVClosed() const;

  asiAlgo_EXPORT Standard_Boolean IsUPeriodic() const;

  asiAlgo_EXPORT Standard_Real UPeriod() const;

  asiAlgo_EXPORT Standard_Boolean IsVPeriodic() const;

  asiAlgo_EXPORT Standard_Real VPeriod() const;

  //! Computes the point of parameters U,V on the surface.
  asiAlgo_EXPORT gp_Pnt Value (const Standard_Real U, const Standard_Real V) const;

  //! Computes the point of parameters U,V on the surface.
  asiAlgo_EXPORT void D0 (const Standard_Real U, const Standard_Real V, gp_Pnt& P) const;

  //! Computes the point  and the first derivatives on
  //! the surface.
  //! Raised   if  the continuity  of   the  current
  //! intervals is not C1.
  asiAlgo_EXPORT void D1 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V) const;

  //! Computes   the point,  the  first  and  second
  //! derivatives on the surface.
  //! Raised  if   the   continuity   of the current
  //! intervals is not C2.
  asiAlgo_EXPORT void D2 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV) const;

  //! Computes the point,  the first, second and third
  //! derivatives on the surface.
  //! Raised  if   the   continuity   of the current
  //! intervals is not C3.
  asiAlgo_EXPORT void D3 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV, gp_Vec& D3U, gp_Vec& D3V, gp_Vec& D3UUV, gp_Vec& D3UVV) const;

  //! Computes  the  derivative of order   Nu  in the
  //! direction U and Nv in the  direction  V  at the point P(U,
  //! V).
  //! Raised if the current U  interval is not not CNu
  //! and the current V interval is not CNv.
  //! Raised if Nu + Nv < 1 or Nu < 0 or Nv < 0.
  asiAlgo_EXPORT gp_Vec DN (const Standard_Real U, const Standard_Real V, const Standard_Integer Nu, const Standard_Integer Nv) const;

  //! Returns the type of the surface : Plane, Cylinder,
  //! Cone,      Sphere,        Torus,    BezierSurface,
  //! BSplineSurface,               SurfaceOfRevolution,
  //! SurfaceOfExtrusion, OtherSurface
  asiAlgo_EXPORT GeomAbs_SurfaceType GetType() const;

  asiAlgo_EXPORT gp_Pln Plane() const;

  asiAlgo_EXPORT gp_Cylinder Cylinder() const;

  asiAlgo_EXPORT gp_Cone Cone() const;

  asiAlgo_EXPORT gp_Sphere Sphere() const;

  asiAlgo_EXPORT gp_Torus Torus() const;

  asiAlgo_EXPORT Standard_Integer UDegree() const;

  asiAlgo_EXPORT Standard_Integer NbUPoles() const;

  asiAlgo_EXPORT Standard_Integer VDegree() const;

  asiAlgo_EXPORT Standard_Integer NbVPoles() const;

  asiAlgo_EXPORT Standard_Integer NbUKnots() const;

  asiAlgo_EXPORT Standard_Integer NbVKnots() const;

  asiAlgo_EXPORT gp_Ax1 Axis() const;

private:

  //! returns true if it is a side face
  asiAlgo_EXPORT Standard_Boolean SideRowsOfPoles (const Standard_Real tol, const Standard_Integer nbuPoles, const Standard_Integer nbvPoles, TColgp_Array2OfPnt& Pnt) const;

  BRepAdaptor_Surface mySurf;
  GeomAbs_SurfaceType myType;
  const HLRAlgo_Projector* myProj;

};


//=======================================================================
//function : Surface
//purpose  :
//=======================================================================

inline BRepAdaptor_Surface & Surface::AccessSurface()
{ return mySurf; }

//=======================================================================
//function : FirstUParameter
//purpose  :
//=======================================================================

inline Standard_Real  Surface::FirstUParameter()const
{ return BSurfaceTool::FirstUParameter(mySurf); }

//=======================================================================
//function : LastUParameter
//purpose  :
//=======================================================================

inline Standard_Real  Surface::LastUParameter()const
{ return BSurfaceTool::LastUParameter(mySurf); }

//=======================================================================
//function : FirstVParameter
//purpose  :
//=======================================================================

inline Standard_Real  Surface::FirstVParameter()const
{ return BSurfaceTool::FirstVParameter(mySurf); }

//=======================================================================
//function : LastVParameter
//purpose  :
//=======================================================================

inline Standard_Real  Surface::LastVParameter()const
{ return BSurfaceTool::LastVParameter(mySurf); }

//=======================================================================
//function : UContinuity
//purpose  :
//=======================================================================

inline GeomAbs_Shape  Surface::UContinuity()const
{ return BSurfaceTool::UContinuity(mySurf); }

//=======================================================================
//function : VContinuity
//purpose  :
//=======================================================================

inline GeomAbs_Shape  Surface::VContinuity()const
{ return BSurfaceTool::VContinuity(mySurf); }

//=======================================================================
//function : NbUIntervals
//purpose  :
//=======================================================================

inline Standard_Integer
Surface::NbUIntervals(const GeomAbs_Shape S)
{ return BSurfaceTool::NbUIntervals(mySurf,S); }

//=======================================================================
//function : NbVIntervals
//purpose  :
//=======================================================================

inline Standard_Integer
Surface::NbVIntervals(const GeomAbs_Shape S)
{ return BSurfaceTool::NbVIntervals(mySurf,S); }

//=======================================================================
//function : IsUClosed
//purpose  :
//=======================================================================

inline Standard_Boolean  Surface::IsUClosed()const
{ return BSurfaceTool::IsUClosed(mySurf); }

//=======================================================================
//function : IsVClosed
//purpose  :
//=======================================================================

inline Standard_Boolean  Surface::IsVClosed()const
{ return BSurfaceTool::IsVClosed(mySurf); }

//=======================================================================
//function : IsUPeriodic
//purpose  :
//=======================================================================

inline Standard_Boolean  Surface::IsUPeriodic()const
{ return BSurfaceTool::IsUPeriodic(mySurf); }

//=======================================================================
//function : UPeriod
//purpose  :
//=======================================================================

inline Standard_Real  Surface::UPeriod()const
{ return BSurfaceTool::UPeriod(mySurf); }

//=======================================================================
//function : IsVPeriodic
//purpose  :
//=======================================================================

inline Standard_Boolean  Surface::IsVPeriodic()const
{ return BSurfaceTool::IsVPeriodic(mySurf); }

//=======================================================================
//function : VPeriod
//purpose  :
//=======================================================================

inline Standard_Real  Surface::VPeriod()const
{ return BSurfaceTool::VPeriod(mySurf); }

//=======================================================================
//function : D0
//purpose  :
//=======================================================================

inline void Surface::D0(const Standard_Real U,
					 const Standard_Real V,
					 gp_Pnt& P) const
{ BSurfaceTool::D0(mySurf,U,V,P); }

//=======================================================================
//function : D1
//purpose  :
//=======================================================================

inline void Surface::D1(const Standard_Real U,
					 const Standard_Real V,
   gp_Pnt& P,
   gp_Vec& D1U,
   gp_Vec& D1V) const
{ BSurfaceTool::D1(mySurf,U,V,P,D1U,D1V); }

//=======================================================================
//function : D2
//purpose  :
//=======================================================================

inline void Surface::D2(const Standard_Real U,
					 const Standard_Real V,
   gp_Pnt& P,
   gp_Vec& D1U,
   gp_Vec& D1V,
   gp_Vec& D2U,
   gp_Vec& D2V,
   gp_Vec& D2UV) const
{ BSurfaceTool::D2(mySurf,U,V,P,D1U,D1V,D2U,D2V,D2UV); }

//=======================================================================
//function : D3
//purpose  :
//=======================================================================

inline void Surface::D3(const Standard_Real U,
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
   gp_Vec& D3UVV) const
{ BSurfaceTool::D3
    (mySurf,U,V,P,D1U,D1V,D2U,D2V,D2UV,D3U,D3V,D3UUV,D3UVV); }

//=======================================================================
//function : DN
//purpose  :
//=======================================================================

inline gp_Vec Surface::DN(const Standard_Real U,
					   const Standard_Real V,
					   const Standard_Integer Nu,
					   const Standard_Integer Nv) const
{ return BSurfaceTool::DN(mySurf,U,V,Nu,Nv); }

//=======================================================================
//function : GetType
//purpose  :
//=======================================================================

inline GeomAbs_SurfaceType  Surface::GetType()const
{ return myType; }

//=======================================================================
//function : Cylinder
//purpose  :
//=======================================================================

inline gp_Cylinder  Surface::Cylinder()const
{ return BSurfaceTool::Cylinder(mySurf); }

//=======================================================================
//function : Cone
//purpose  :
//=======================================================================

inline gp_Cone  Surface::Cone()const
{ return BSurfaceTool::Cone(mySurf); }

//=======================================================================
//function : Sphere
//purpose  :
//=======================================================================

inline gp_Sphere  Surface::Sphere()const
{ return BSurfaceTool::Sphere(mySurf); }

//=======================================================================
//function : Torus
//purpose  :
//=======================================================================

inline gp_Torus  Surface::Torus()const
{ return BSurfaceTool::Torus(mySurf); }

//=======================================================================
//function : UDegree
//purpose  :
//=======================================================================

inline Standard_Integer  Surface::UDegree()const
{ return BSurfaceTool::UDegree(mySurf); }

//=======================================================================
//function : NbUPoles
//purpose  :
//=======================================================================

inline Standard_Integer  Surface::NbUPoles()const
{ return BSurfaceTool::NbUPoles(mySurf); }

//=======================================================================
//function : VDegree
//purpose  :
//=======================================================================

inline Standard_Integer  Surface::VDegree()const
{ return BSurfaceTool::VDegree(mySurf); }

//=======================================================================
//function : NbVPoles
//purpose  :
//=======================================================================

inline Standard_Integer  Surface::NbVPoles()const
{ return BSurfaceTool::NbVPoles(mySurf); }


//=======================================================================
//function : NbUKnots
//purpose  :
//=======================================================================

inline Standard_Integer  Surface::NbUKnots()const
{ return BSurfaceTool::NbUKnots(mySurf); }


//=======================================================================
//function : NbVKnots
//purpose  :
//=======================================================================

inline Standard_Integer  Surface::NbVKnots()const
{ return BSurfaceTool::NbVKnots(mySurf); }

//=======================================================================
//function : Axis
//purpose  :
//=======================================================================

inline gp_Ax1  Surface::Axis()const
{ return BSurfaceTool::AxeOfRevolution(mySurf); }

}
}

#endif // _HLRBRep_Surface_HeaderFile
