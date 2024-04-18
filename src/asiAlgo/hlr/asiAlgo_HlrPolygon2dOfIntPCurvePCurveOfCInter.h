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

#ifndef asiAlgo_HLRBRep_ThePolygon2dOfTheIntPCurvePCurveOfCInter_HeaderFile
#define asiAlgo_HLRBRep_ThePolygon2dOfTheIntPCurvePCurveOfCInter_HeaderFile

// asiAlgo includes
#include <asiAlgo.h>

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Standard_Boolean.hxx>
#include <Intf_Polygon2d.hxx>
#include <Standard_Address.hxx>

class Standard_OutOfRange;
class IntRes2d_Domain;
class Bnd_Box2d;
class gp_Pnt2d;

namespace asiAlgo {
namespace hlr {

class CurveTool;

class Polygon2dOfIntPCurvePCurveOfCInter  : public Intf_Polygon2d
{
public:

  DEFINE_STANDARD_ALLOC


  //! Compute a polygon on the domain of the curve.
  asiAlgo_EXPORT Polygon2dOfIntPCurvePCurveOfCInter(const Standard_Address& Curve, const Standard_Integer NbPnt, const IntRes2d_Domain& Domain, const Standard_Real Tol);

  //! The current polygon is modified if most
  //! of the  points of the  polygon  are
  //! outside  the  box  <OtherBox>.  In this
  //! situation, bounds are computed to build
  //! a polygon inside or near the OtherBox.
  asiAlgo_EXPORT void ComputeWithBox (const Standard_Address& Curve, const Bnd_Box2d& OtherBox);

 asiAlgo_EXPORT  virtual Standard_Real DeflectionOverEstimation() const Standard_OVERRIDE;

  asiAlgo_EXPORT void SetDeflectionOverEstimation (const Standard_Real x);

  asiAlgo_EXPORT void Closed (const Standard_Boolean clos);

  //! Returns True if the polyline is closed.
    virtual Standard_Boolean Closed () const Standard_OVERRIDE { return ClosedPolygon; }

  //! Give the number of Segments in the polyline.
  asiAlgo_EXPORT virtual Standard_Integer NbSegments() const Standard_OVERRIDE;

  //! Returns the points of the segment <Index> in the Polygon.
  asiAlgo_EXPORT virtual void Segment (const Standard_Integer theIndex, gp_Pnt2d& theBegin, gp_Pnt2d& theEnd) const Standard_OVERRIDE;

  //! Returns the parameter (On the curve)
  //! of the first point of the Polygon
  asiAlgo_EXPORT Standard_Real InfParameter() const;

  //! Returns the parameter (On the curve)
  //! of the last point of the Polygon
  asiAlgo_EXPORT Standard_Real SupParameter() const;

  asiAlgo_EXPORT Standard_Boolean AutoIntersectionIsPossible() const;

  //! Give an approximation of the parameter on the curve
  //! according to the discretization of the Curve.
  asiAlgo_EXPORT Standard_Real ApproxParamOnCurve (const Standard_Integer Index, const Standard_Real ParamOnLine) const;

  asiAlgo_EXPORT Standard_Integer CalculRegion (const Standard_Real x, const Standard_Real y, const Standard_Real x1, const Standard_Real x2, const Standard_Real y1, const Standard_Real y2) const;

  asiAlgo_EXPORT void Dump() const;

private:

  Standard_Real TheDeflection;
  Standard_Integer NbPntIn;
  Standard_Integer TheMaxNbPoints;
  TColgp_Array1OfPnt2d ThePnts;
  TColStd_Array1OfReal TheParams;
  TColStd_Array1OfInteger TheIndex;
  Standard_Boolean ClosedPolygon;
  Standard_Real Binf;
  Standard_Real Bsup;

};


//======================================================================
  inline Standard_Real Polygon2dOfIntPCurvePCurveOfCInter::DeflectionOverEstimation() const {
    return(TheDeflection);
  }//======================================================================
  inline void Polygon2dOfIntPCurvePCurveOfCInter::SetDeflectionOverEstimation
          (const Standard_Real x) {
    TheDeflection = x;
    myBox.Enlarge(TheDeflection);
  }
//======================================================================
  inline void Polygon2dOfIntPCurvePCurveOfCInter::Closed(const Standard_Boolean flag) {
    ClosedPolygon = flag;
  }
//======================================================================
  inline Standard_Integer Polygon2dOfIntPCurvePCurveOfCInter::NbSegments(void) const {
    return((ClosedPolygon)? NbPntIn : NbPntIn-1);
  }
//======================================================================
  inline Standard_Real Polygon2dOfIntPCurvePCurveOfCInter::InfParameter() const {
    return(TheParams.Value(TheIndex(1)));
  }
//======================================================================
  inline Standard_Real Polygon2dOfIntPCurvePCurveOfCInter::SupParameter() const {
    return(TheParams.Value(TheIndex(NbPntIn)));
  }
//======================================================================

//======================================================================
  inline  Standard_Integer Polygon2dOfIntPCurvePCurveOfCInter::CalculRegion(const Standard_Real x,
                                                               const Standard_Real y,
                                                               const Standard_Real x1,
                                                               const Standard_Real x2,
                                                               const Standard_Real y1,
                                                               const Standard_Real y2) const {
    Standard_Integer r;
    if(x<x1) { r=1; }  else { if(x>x2) { r=2; } else { r=0; } }
    if(y<y1) { r|=4; } else { if(y>y2) { r|=8; } }
    return(r);
  }

}
}

#endif // _HLRBRep_ThePolygon2dOfTheIntPCurvePCurveOfCInter_HeaderFile
