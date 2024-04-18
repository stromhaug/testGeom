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

#ifndef asiAlgo_HLRBRep_TheLocateExtPCOfTheProjPCurOfCInter_HeaderFile
#define asiAlgo_HLRBRep_TheLocateExtPCOfTheProjPCurOfCInter_HeaderFile

// asiAlgo includes
#include <asiAlgo_HlrPCLocFOfLocateExtPCOfProjPCurOfCInter.h>

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Standard_Address.hxx>

class Standard_DomainError;
class Standard_TypeMismatch;
class StdFail_NotDone;
class Extrema_POnCurv2d;
class gp_Pnt2d;
class gp_Vec2d;

namespace asiAlgo {
namespace hlr {

class CurveTool;
class PCLocFOfLocateExtPCOfProjPCurOfCInter;

class LocateExtPCOfProjPCurOfCInter
{
public:

  DEFINE_STANDARD_ALLOC

  asiAlgo_EXPORT LocateExtPCOfProjPCurOfCInter();

  //! Calculates the distance with a close point.
  //! The close point is defined by the parameter value U0.
  //! The function F(u)=distance(P,C(u)) has an extremum when g(u)=dF/du=0.
  //! The algorithm searches a zero near the close point.
  //! TolU is used to decide to stop the iterations.
  //! At the nth iteration, the criteria is:
  //! abs(Un - Un-1) < TolU.
  asiAlgo_EXPORT LocateExtPCOfProjPCurOfCInter(const gp_Pnt2d& P, const Standard_Address& C, const Standard_Real U0, const Standard_Real TolU);

  //! Calculates the distance with a close point.
  //! The close point is defined by the parameter value U0.
  //! The function F(u)=distance(P,C(u)) has an extremum when g(u)=dF/du=0.
  //! The algorithm searches a zero near the close point.
  //! Zeros are searched between Umin and Usup.
  //! TolU is used to decide to stop the iterations.
  //! At the nth iteration, the criteria is:
  //! abs(Un - Un-1) < TolU.
  asiAlgo_EXPORT LocateExtPCOfProjPCurOfCInter(const gp_Pnt2d& P, const Standard_Address& C, const Standard_Real U0, const Standard_Real Umin, const Standard_Real Usup, const Standard_Real TolU);

  //! sets the fields of the algorithm.
  asiAlgo_EXPORT void Initialize (const Standard_Address& C, const Standard_Real Umin, const Standard_Real Usup, const Standard_Real TolU);

  //! the algorithm is done with the point P.
  //! An exception is raised if the fields have not
  //! been initialized.
  asiAlgo_EXPORT void Perform (const gp_Pnt2d& P, const Standard_Real U0);

  //! Returns True if the distance is found.
  asiAlgo_EXPORT Standard_Boolean IsDone() const;

  //! Returns the value of the extremum square distance.
  asiAlgo_EXPORT Standard_Real SquareDistance() const;

  //! Returns True if the extremum distance is a minimum.
  asiAlgo_EXPORT Standard_Boolean IsMin() const;

  //! Returns the point of the extremum distance.
  asiAlgo_EXPORT const Extrema_POnCurv2d& Point() const;

private:

  Standard_Boolean myDone;
  Standard_Real mytolU;
  Standard_Real myumin;
  Standard_Real myusup;
  PCLocFOfLocateExtPCOfProjPCurOfCInter myF;

};

}
}

#endif // _HLRBRep_TheLocateExtPCOfTheProjPCurOfCInter_HeaderFile
