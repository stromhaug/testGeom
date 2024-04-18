// Created on: 1997-04-17
// Created by: Christophe MARION
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef asiAlgo_HLRBRep_EdgeIList_HeaderFile
#define asiAlgo_HLRBRep_EdgeIList_HeaderFile

// asiAlgo includes
#include <asiAlgo.h>

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <HLRAlgo_InterferenceList.hxx>

class HLRAlgo_Interference;

namespace asiAlgo {
namespace hlr {

class EdgeInterferenceTool;

class EdgeIList
{
public:

  DEFINE_STANDARD_ALLOC


  //! Add the interference <I> to the list <IL>.
  asiAlgo_EXPORT static void AddInterference (HLRAlgo_InterferenceList& IL, const HLRAlgo_Interference& I, const EdgeInterferenceTool& T);

  //! Process complex transitions on the list IL.
  asiAlgo_EXPORT static void ProcessComplex (HLRAlgo_InterferenceList& IL, const EdgeInterferenceTool& T);

};

}
}

#endif // _HLRBRep_EdgeIList_HeaderFile
