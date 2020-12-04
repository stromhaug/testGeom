//-----------------------------------------------------------------------------
// Created on: 22 September 2020 (*)
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Sergey Slyadnev
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

#ifndef asiAsm_ServicePopulateDatabase_h
#define asiAsm_ServicePopulateDatabase_h

// asiAsm includes
#include <asiAsm_XdeDoc.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

//-----------------------------------------------------------------------------

namespace asiAsm_Service {

  class Model;

  //! Ctor.
  //! \param[in,out] asiAsmModel asiAsm model to populate.
  //! \param[in]     xdeModel  XDE model as a data source.
  //! \param[in,out] progress progress notifier.
  //! \return true in case of success, false -- otherwise.
  asiAsm_EXPORT bool
    PopulateDatabase(const Handle(Model)&         asiAsmModel,
                     const Handle(asiAsm_XdeDoc)& xdeModel,
                     ActAPI_ProgressEntry         progress = NULL);

} // asiAsm_Service

#endif
