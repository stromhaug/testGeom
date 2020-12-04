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

#ifndef asiAsm_DaoSceneTree_h
#define asiAsm_DaoSceneTree_h

// dao includes
#include <asiAsm_DaoBase.h>

//-----------------------------------------------------------------------------

//! \ingroup ASIASM
//!
//! DAO for scene tree (ST) data structure.
class asiAsm_DaoSceneTree : public asiAsm_DaoBase
{
public:

  //! Default ctor.
  asiAsm_DaoSceneTree() : asiAsm_DaoBase() {}

  //! Ctor.
  //! \param[in] dbName   database name.
  //! \param[in] progress progress notifier.
  asiAsm_DaoSceneTree(const std::string&   dbName,
                      ActAPI_ProgressEntry progress) : asiAsm_DaoBase(dbName, progress) {}

private:

  //! \return table name.
  virtual const char* tableName() const
  {
    return asiAsm_Macro_Table_SceneTree;
  }

  //! Creates table.
  //! \return true in case of success, false -- otherwise.
  asiAsm_EXPORT virtual bool
    createTable() const;

};

#endif
