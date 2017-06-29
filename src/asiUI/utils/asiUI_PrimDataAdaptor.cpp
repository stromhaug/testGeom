//-----------------------------------------------------------------------------
// Created on: 10 December 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_PrimDataAdaptor.h>

// A-Situs (GUI) includes
#include <asiUI_Common.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//! \return handled value as QVariant. This method takes care of conversion
//!         of the persistent data type to the corresponding Qt type.
QVariant asiUI_PrimDataAdaptor::GetValue() const
{
  if ( m_node.IsNull() || !m_node->IsWellFormed() )
    return QVariant();

  QVariant result;
  Handle(ActAPI_IUserParameter) P = m_node->Parameter(m_iPID);

  // Resolve the actual data type
  if ( P->GetParamType() == Parameter_Real )
  {
    Handle(ActData_RealParameter) PReal = ActParamTool::AsReal(P);
    result = PReal->GetValue();
  }
  else if ( P->GetParamType() == Parameter_AsciiString )
  {
    Handle(ActData_AsciiStringParameter) PStr = ActParamTool::AsAsciiString(P);
    result = AsciiStr2QStr( PStr->GetValue() );
  }
  else if ( P->GetParamType() == Parameter_Int )
  {
    Handle(ActData_IntParameter) PInt = ActParamTool::AsInt(P);
    result = PInt->GetValue();
  }
  else if ( P->GetParamType() == Parameter_Bool )
  {
    Handle(ActData_BoolParameter) PBool = ActParamTool::AsBool(P);
    result = PBool->GetValue();
  }
  else
    result = QVariant();

  return result;
}

//! Sets value for to the Data Model.
//! \param ui_val [in] value to set in Qt format.
void asiUI_PrimDataAdaptor::SetValue(const QVariant& ui_val) const
{
  if ( m_node.IsNull() || !m_node->IsWellFormed() )
    return;

  Handle(ActAPI_IUserParameter) P = m_node->Parameter(m_iPID);

  // Resolve data type
  if ( P->GetParamType() == Parameter_Real ) // Proceed with Real
  {
    Handle(ActData_RealParameter) PReal = ActParamTool::AsReal(P);
    PReal->SetValue( ui_val.toDouble() );
  }
  else if ( P->GetParamType() == Parameter_Int ) // Proceed with Integer
  {
    Handle(ActData_IntParameter) PInt = ActParamTool::AsInt(P);
    PInt->SetValue( ui_val.toInt() );
  }
  else if ( P->GetParamType() == Parameter_AsciiString ) // Proceed with ASCII String
  {
    Handle(ActData_AsciiStringParameter) PStr = ActParamTool::AsAsciiString(P);
    PStr->SetValue( QStr2AsciiStr( ui_val.toString() ) );
  }
  else if ( P->GetParamType() == Parameter_Bool ) // Proceed with Boolean
  {
    Handle(ActData_BoolParameter) PBool = ActParamTool::AsBool(P);
    PBool->SetValue( ui_val.toBool() ? 1 : 0 );
  }
}
