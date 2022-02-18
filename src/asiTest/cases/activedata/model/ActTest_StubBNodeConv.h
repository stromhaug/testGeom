//-----------------------------------------------------------------------------
// Created on: 17 February 2022
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef ActTest_StubBNodeConv_HeaderFile
#define ActTest_StubBNodeConv_HeaderFile

// Active Data includes
#include <ActData_BaseNode.h>

DEFINE_STANDARD_HANDLE(ActTest_StubBNodeConv, ActData_BaseNode)

//! \ingroup AD_TEST
//!
//! B test Node representing expected data format after conversion.
class ActTest_StubBNodeConv : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(ActTest_StubBNodeConv, ActData_BaseNode)

  // Automatic registration of Node instance in the Nodal Factory.
  DEFINE_NODE_FACTORY(ActTest_StubBNodeConv, Instance)

public:

  //! IDs of the underlying Parameters.
  enum ParamId
  {
    PID_Name = ActData_BaseNode::UserParam_Last,
    PID_Int,     //!< As in original version.
    PID_NEWInt1, //!< NEW Parameter.
    PID_Real,    //!< As in original version.
    PID_TFunc,   //!< As in original version.
    PID_NEWInt2, //!< NEW Parameter.
    PID_RefList  //!< As in original version.
  };

public:

  static Handle(ActAPI_INode) Instance();

// Generic accessors:
public:

  virtual TCollection_ExtendedString
    GetName();
  
  virtual void
    SetName(const TCollection_ExtendedString& theName);

// Initialization and accessors:
public:

  void Init();

protected:

  //! Allocation is allowed only via Instance method.
  ActTest_StubBNodeConv();

};

#endif
