//-----------------------------------------------------------------------------
// Created on: 06 June 2022
//-----------------------------------------------------------------------------
// Copyright (c) 2022-present, Natalia Ermolaeva
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

#ifndef asiUI_DialogCommandsPluginItem_H
#define asiUI_DialogCommandsPluginItem_H

#include <asiUI_TreeItem.h>
#include <asiUI_TclPluginToCommands.h>

#include <NCollection_List.hxx>
#include <Standard.hxx>
#include <TopoDS_Shape.hxx>

class asiUI_DialogCommandsPluginItem;
typedef QExplicitlySharedDataPointer<asiUI_DialogCommandsPluginItem> asiUI_DialogCommandsPluginItemPtr;

//! Plugin item for groupping plugin for commands.
class asiUI_DialogCommandsPluginItem : public asiUI_TreeItem
{
public:

  //! Creates an item wrapped by a shared pointer
  static asiUI_DialogCommandsPluginItemPtr CreateItem(asiUI_TreeItemPtr parent, const int row, const int column)
    { return asiUI_DialogCommandsPluginItemPtr(new asiUI_DialogCommandsPluginItem (parent, row, column)); }

  //! Destructor
  virtual ~asiUI_DialogCommandsPluginItem() {}

  //! Returns command info
  //! \param rowId [in] item row index
  asiUI_TclCommandParsed value(const int rowId) const;

protected:

  //! Returns data value for the role.
  //! \param itemRole [in] a value role
  //! \return the value
  virtual QVariant initValue(const int itemRole) const Standard_OVERRIDE;

  //! \return number of children.
  virtual int initRowCount() const Standard_OVERRIDE;

  //! Creates a child item in the given position.
  //! \param row [in]    the child row position
  //! \param column [in] the child column position
  //! \return the created item
  virtual asiUI_TreeItemPtr createChild(int row, int column) Standard_OVERRIDE;

private:

  //! Constructor
  //! \param parent [in] a parent item
  //! \param row [in]    item row position
  //! \param column [in] item column position
  asiUI_DialogCommandsPluginItem(asiUI_TreeItemPtr parent, const int row, const int column)
  : asiUI_TreeItem (parent, row, column) {}
};

#endif