//-----------------------------------------------------------------------------
// Created on: February 2012
//-----------------------------------------------------------------------------
// Copyright (c) 2017, OPEN CASCADE SAS
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
//    * Neither the name of OPEN CASCADE SAS nor the
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
//
// Web: http://analysissitus.org
//-----------------------------------------------------------------------------

// Own include
#include <ActData_ShapeParameter.h>

// Active Data includes
#include <ActData_Utils.h>

// OCCT includes
#include <Standard_ProgramError.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>

//-----------------------------------------------------------------------------
// Parameter
//-----------------------------------------------------------------------------

//! Default constructor.
ActData_ShapeParameter::ActData_ShapeParameter() : ActData_UserParameter()
{}

//! Ensures correct construction of the Parameter object, e.g. prevents
//! allocating it in stack memory.
//! \return Parameter instance.
Handle(ActData_ShapeParameter) ActData_ShapeParameter::Instance()
{
  return new ActData_ShapeParameter();
}

//! Sets shape.
//! \param theShape [in] shape to set.
//! \param theModType [in] modification type.
//! \param doResetValidity [in] indicates whether to reset validity flag.
//! \param doResetPending [in] indicates whether this Parameter must lose its
//!        PENDING (or out-dated) property.
void ActData_ShapeParameter::SetShape(const TopoDS_Shape& theShape,
                                      const ActAPI_ModificationType theModType,
                                      const Standard_Boolean doResetValidity,
                                      const Standard_Boolean doResetPending)
{
  if ( this->IsDetached() )
    Standard_ProgramError::Raise("Cannot access detached data");

  // NOTICE: there is a bug in OCAF. When you change orientation of a persistent
  //         shape and rewrite this shape in its corresponding TNaming_NamedShape
  //         attribute, this modification has no effect. To workaround the issue,
  //         we unbind the previous shape from OCAF.
  TopoDS_Shape prevShape = ActData_Utils::GetShapeValue(m_label, DS_Shape);
  //
  if ( !prevShape.IsNull() && prevShape.IsSame(theShape) )
    ActData_Utils::SetShapeValue( m_label, DS_Shape, TopoDS_Shape() );

  // Store new shape.
  ActData_Utils::SetShapeValue(m_label, DS_Shape, theShape);

  // Mark root label of the Parameter as modified (Touched, Impacted or Silent)
  SPRING_INTO_FUNCTION(theModType)
  // Reset Parameter's validity flag if requested
  RESET_VALIDITY(doResetValidity)
  // Reset Parameter's PENDING property
  RESET_PENDING(doResetPending);
}

//! Accessor for the stored shape.
//! \return stored shape.
TopoDS_Shape ActData_ShapeParameter::GetShape()
{
  if ( !this->IsWellFormed() )
    Standard_ProgramError::Raise("Data inconsistent");

  return ActData_Utils::GetShapeValue(m_label, DS_Shape);
}

//! Checks if this Parameter object is mapped onto CAF data structure in a
//! correct way.
//! \return true if the object is well-formed, false -- otherwise.
Standard_Boolean ActData_ShapeParameter::isWellFormed() const
{
  if ( !ActData_Utils::CheckLabelAttr( m_label, DS_Shape,
                                       TNaming_NamedShape::GetID() ) )
    return Standard_False;

  return Standard_True;
}

//! Returns Parameter type.
//! \return Parameter type.
Standard_Integer ActData_ShapeParameter::parameterType() const
{
  return Parameter_Shape;
}

//-----------------------------------------------------------------------------
// DTO construction
//-----------------------------------------------------------------------------

//! Populates Parameter from the passed DTO.
//! \param theDTO [in] DTO to source data from.
//! \param theModType [in] modification type.
//! \param doResetValidity [in] indicates whether validity flag must be
//!        reset or not.
//! \param doResetPending [in] indicates whether pending flag must be reset
//!        or not.
void ActData_ShapeParameter::setFromDTO(const Handle(ActData_ParameterDTO)& theDTO,
                                        const ActAPI_ModificationType theModType,
                                        const Standard_Boolean doResetValidity,
                                        const Standard_Boolean doResetPending)
{
  Handle(ActData_ShapeDTO) MyDTO = Handle(ActData_ShapeDTO)::DownCast(theDTO);
  this->SetShape(MyDTO->Shape, theModType, doResetValidity, doResetPending);
}

//! Creates and populates DTO.
//! \param theGID [in] ready-to-use GID for DTO.
//! \return constructed DTO instance.
Handle(ActData_ParameterDTO)
  ActData_ShapeParameter::createDTO(const ActAPI_ParameterGID& theGID)
{
  Handle(ActData_ShapeDTO) aRes = new ActData_ShapeDTO(theGID);
  aRes->Shape = this->GetShape();
  return aRes;
}
