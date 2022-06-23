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

// Own include
#include <ActTest_TreeFunctionParameter.h>

// Active Data includes
#include <ActData_BaseTreeFunction.h>

// OCCT includes
#include <TDF_ListIteratorOfLabelList.hxx>
#include <TDF_Tool.hxx>
#include <TFunction_DoubleMapIteratorOfDoubleMapOfIntegerLabel.hxx>
#include <TFunction_DriverTable.hxx>
#include <TFunction_Function.hxx>
#include <TFunction_GraphNode.hxx>
#include <TFunction_IFunction.hxx>
#include <TFunction_Iterator.hxx>
#include <TFunction_Scope.hxx>

#pragma warning(disable: 4127) // "Conditional expression is constant" by TEST_VERIFY
#pragma warning(disable: 4800) // "Standard_Boolean: forcing value to bool" by TEST_VERIFY

DEFINE_STANDARD_HANDLE(ActTest_DummyDriver, ActData_TreeFunctionDriver)

//! Dummy Function Driver. The instance of this class is not purposed to be
//! executed. It is just a stub required by Tree Function mechanism in order
//! provide execution connectivity between TDF Labels.
class ActTest_DummyDriver : public ActData_TreeFunctionDriver
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(ActTest_DummyDriver, ActData_TreeFunctionDriver)

public:

  //! Unique ID of the Function Driver generated by
  //! MS Visual Studio: "Tools -> Create GUID".
  //! \return GUID.
  static Standard_GUID GUID()
  {
    return "243CCE7D-3C26-4CEE-B53C-9C6BEB6E0BE5";
  }

};

//-----------------------------------------------------------------------------
// Business logic
//-----------------------------------------------------------------------------

//! Performs test on accessing Function Driver GUID and other related TDF
//! Attributes which are forming the core of the OCCT TFunction mechanism.
//! \param funcID [in] ID of test function.
//! \return true if test is passed, false -- otherwise.
outcome ActTest_TreeFunctionParameter::accessFunction(const int funcID)
{
  /* ====================================
   *  Initialize underlying CAF document
   * ==================================== */

  TEST_PRINT_DECOR_L("Create new TREE FUNCTION Parameter");

  ActTest_DocAlloc docAlloc;
  Handle(TDocStd_Document) doc = docAlloc.Doc;

  // Root label
  TDF_Label label;

  doc->NewCommand();
  Handle(ActData_TreeFunctionParameter)
    param = ActParamTool::AsTreeFunction( createParameter(doc, Parameter_TreeFunction, label) );
  doc->CommitCommand();

  // Allocate and register the Function Driver
  Standard_GUID TEST_GUID = ActTest_DummyDriver::GUID();
  Handle(ActTest_DummyDriver) aDriver = new ActTest_DummyDriver();
  TFunction_DriverTable::Get()->AddDriver(TEST_GUID, aDriver);

  // Set dummy GUID to the target TDF Label. This Label is now said to
  // reference the corresponding Function Driver. When GUID is set, the
  // following issues are performed implicitly:
  // - Function Attribute is set to the target Label. This Attribute stores
  //   the actual passed GUID. This GUID must have a not-null reference to
  //   the Function Driver which should be previously bound to the GLOBAL
  //   STATIC map of Drivers.
  // - Graph Node Attribute is set to the target Label. The Attributes of this
  //   type form a dependency graph of Tree Functions. Traversing this
  //   dependency graph, application normally performs the execution of
  //   Tree Functions, deciding whether the next function is to be executed by
  //   consulting the modification state of its arguments.
  // - Function Scope Attribute is set to the root Label of the Document. This
  //   Attribute allows accessing the Functions by the TDF Labels they are
  //   bound to. Also, this Attribute provides an access to the global
  //   modification LogBook which is used to record all modification requests
  //   to the TDF Labels involved in Tree Function mechanism.
  doc->NewCommand();
  param->SetDriverGUID(TEST_GUID);
  doc->CommitCommand();

  /* ============================================================
   *  Create a dependency graph of the following structure:
   * ////////////////////////////////////////////////////////////
   *  THIS --> A --+
   *     |         | --> C --+
   *     + --> B --+         |
   *     |                   |
   *     + --> E             | --> D
   *     |                   |
   *     + --> F ------------+
   * ////////////////////////////////////////////////////////////
   *  THIS Parameter is a root one. Parameters A, B, E, F can be
   *  calculated only after THIS one. Parameter E brings no
   *  other dependencies and completes the corresponding
   *  execution branch so. Parameter C can be calculated only
   *  after A and B. Parameter D can be calculated only after
   *  C and F
   * ============================================================ */

  doc->NewCommand();

  // Short-cut for THIS Tree Function
  TDF_Label THISLabel = label;
  TCollection_AsciiString THISEntry;
  TDF_Tool::Entry(THISLabel, THISEntry);
  Handle(ActData_TreeFunctionParameter) thisParam = param;

  // Create Tree Function A
  TDF_Label ALabel;
  TCollection_AsciiString AEntry;
  Handle(ActAPI_IUserParameter)
    AGeneric = createParameter(doc, Parameter_TreeFunction, ALabel);
  Handle(ActData_TreeFunctionParameter)
    A = Handle(ActData_TreeFunctionParameter)::DownCast(AGeneric);
  TDF_Tool::Entry(ALabel, AEntry);
  A->SetDriverGUID(TEST_GUID);

  // Create Tree Function B
  TDF_Label BLabel;
  TCollection_AsciiString BEntry;
  Handle(ActAPI_IUserParameter)
    BGeneric = createParameter(doc, Parameter_TreeFunction, BLabel);
  Handle(ActData_TreeFunctionParameter)
    B = Handle(ActData_TreeFunctionParameter)::DownCast(BGeneric);
  TDF_Tool::Entry(BLabel, BEntry);
  B->SetDriverGUID(TEST_GUID);

  // Create Tree Function C
  TDF_Label CLabel;
  TCollection_AsciiString CEntry;
  Handle(ActAPI_IUserParameter)
    CGeneric = createParameter(doc, Parameter_TreeFunction, CLabel);
  Handle(ActData_TreeFunctionParameter)
    C = Handle(ActData_TreeFunctionParameter)::DownCast(CGeneric);
  TDF_Tool::Entry(CLabel, CEntry);
  C->SetDriverGUID(TEST_GUID);

  // Create Tree Function D
  TDF_Label DLabel;
  TCollection_AsciiString DEntry;
  Handle(ActAPI_IUserParameter)
    DGeneric = createParameter(doc, Parameter_TreeFunction, DLabel);
  Handle(ActData_TreeFunctionParameter)
    D = Handle(ActData_TreeFunctionParameter)::DownCast(DGeneric);
  TDF_Tool::Entry(DLabel, DEntry);
  D->SetDriverGUID(TEST_GUID);

  // Create Tree Function E
  TDF_Label ELabel;
  TCollection_AsciiString EEntry;
  Handle(ActAPI_IUserParameter)
    EGeneric = createParameter(doc, Parameter_TreeFunction, ELabel);
  Handle(ActData_TreeFunctionParameter)
    E = Handle(ActData_TreeFunctionParameter)::DownCast(EGeneric);
  TDF_Tool::Entry(ELabel, EEntry);
  E->SetDriverGUID(TEST_GUID);

  // Create Tree Function F
  TDF_Label FLabel;
  TCollection_AsciiString FEntry;
  Handle(ActAPI_IUserParameter)
    FGeneric = createParameter(doc, Parameter_TreeFunction, FLabel);
  Handle(ActData_TreeFunctionParameter)
    F = Handle(ActData_TreeFunctionParameter)::DownCast(FGeneric);
  TDF_Tool::Entry(FLabel, FEntry);
  F->SetDriverGUID(TEST_GUID);

  // Bind Arguments & Results for THIS
  thisParam->AddArgument(thisParam);
  thisParam->AddResult(thisParam);

  // Bind Arguments & Results for A
  A->AddArgument(thisParam);
  A->AddResult(A);

  // Bind Arguments & Results for B
  B->AddArgument(thisParam);
  B->AddResult(B);

  // Bind Arguments & Results for C
  C->AddArgument(A);
  C->AddArgument(B);
  C->AddResult(C);

  // Bind Arguments & Results for E
  E->AddArgument(thisParam);
  E->AddResult(E);

  // Bind Arguments & Results for F
  F->AddArgument(thisParam);
  F->AddResult(F);

  // Bind Arguments & Results for D
  D->AddArgument(F);
  D->AddArgument(C);
  D->AddResult(D);

  // Build Dependency Graph
  TFunction_IFunction::UpdateDependencies(label);

  doc->CommitCommand();

  /* ===========================================================
   *  Check that Function Attributes have been successfully set
   *  on Parameter THID
   * =========================================================== */

  // Verify self-check
  TEST_VERIFY( thisParam->IsWellFormed(), DescriptionFn(), funcID )

  // Access Driver GUID
  Handle(TFunction_Function) aFunctionAttr;
  THISLabel.FindAttribute(TFunction_Function::GetID(), aFunctionAttr);
  TEST_VERIFY( !aFunctionAttr.IsNull(), DescriptionFn(), funcID)
  const Standard_GUID& aGUID = aFunctionAttr->GetDriverGUID();

  // Verify GUID
  TEST_VERIFY( aGUID == TEST_GUID, DescriptionFn(), funcID )

  // Access Graph Node
  Handle(TFunction_GraphNode) aGraphNodeAttr;
  THISLabel.FindAttribute(TFunction_GraphNode::GetID(), aGraphNodeAttr);
  TEST_VERIFY( !aGraphNodeAttr.IsNull(), DescriptionFn(), funcID )

  // Access global Function Scope
  TDF_Label aDocRoot = THISLabel.Root();
  Handle(TFunction_Scope) aGlobalScopeAttr;
  Standard_Boolean isGlobalScopeBound =
    aDocRoot.FindAttribute(TFunction_Scope::GetID(), aGlobalScopeAttr);

  // Verify Scope
  TEST_VERIFY( isGlobalScopeBound, DescriptionFn(), funcID )
  TEST_VERIFY( !aGlobalScopeAttr.IsNull(), DescriptionFn(), funcID )

  /* ==================================================================
   *  Iterate over the dependency graph and verify the execution order
   * ================================================================== */

  // Updating of statuses requires a transactional scope
  doc->NewCommand();

  // Iterate over the entire collection of Labels involved in Tree Function
  // mechanism in order to set the initial execution statuses for them
  TFunction_DoubleMapIteratorOfDoubleMapOfIntegerLabel aScopeIt( aGlobalScopeAttr->GetFunctions() );
  for ( ; aScopeIt.More(); aScopeIt.Next() )
  {
    const TDF_Label& aNextLab = aScopeIt.Key2();
    Handle(TFunction_GraphNode) aNextGraphNode;
    if ( aNextLab.FindAttribute(TFunction_GraphNode::GetID(), aNextGraphNode) )
    {
      aNextGraphNode->SetStatus(TFunction_ES_NotExecuted);
    }
  }

  // Verify the entire number of Functions
  TEST_VERIFY( aGlobalScopeAttr->GetFunctions().Extent() == 7, DescriptionFn(), funcID )

  Standard_Integer ITERATION = 1;
  Standard_Integer EXPECTED_NB_FUNCTIONS[] = {1, 4, 1, 1};
  EntriesByLevels EXPECTED_FUNCTION_LABELS;

  // First iteration
  EXPECTED_FUNCTION_LABELS.Append( EntryList() );
  EXPECTED_FUNCTION_LABELS.ChangeValue(1).Append(THISEntry);

  // Second iteration
  EXPECTED_FUNCTION_LABELS.Append( EntryList() );
  EXPECTED_FUNCTION_LABELS.ChangeValue(2).Append(AEntry);
  EXPECTED_FUNCTION_LABELS.ChangeValue(2).Append(BEntry);
  EXPECTED_FUNCTION_LABELS.ChangeValue(2).Append(EEntry);
  EXPECTED_FUNCTION_LABELS.ChangeValue(2).Append(FEntry);

  // Third iteration
  EXPECTED_FUNCTION_LABELS.Append( EntryList() );
  EXPECTED_FUNCTION_LABELS.ChangeValue(3).Append(CEntry);

  // Fourth iteration
  EXPECTED_FUNCTION_LABELS.Append( EntryList() );
  EXPECTED_FUNCTION_LABELS.ChangeValue(4).Append(DEntry);

  // Iterate over the Dependency Graph using execution status of each
  // Graph Node
  TFunction_Iterator aFuncIt(label);
  aFuncIt.SetUsageOfExecutionStatus(Standard_True);
  for ( ; aFuncIt.More(); aFuncIt.Next() )
  {
    // These Tree Functions can be executed concurrently
    const TDF_LabelList& aCurrentFunctions = aFuncIt.Current();

    // Verify the number of Functions accessible for concurrent execution
    Standard_Integer aNbCurrent = aCurrentFunctions.Extent();
    TEST_VERIFY( aNbCurrent == EXPECTED_NB_FUNCTIONS[ITERATION - 1], DescriptionFn(), funcID )

    TDF_ListIteratorOfLabelList aCurrentIt(aCurrentFunctions);
    for ( ; aCurrentIt.More(); aCurrentIt.Next() )
    {
      TDF_Label aLab = aCurrentIt.Value();
      TCollection_AsciiString aLabEntry;
      TDF_Tool::Entry(aLab, aLabEntry);

      TEST_VERIFY( isLabelExpectedByLevel(ITERATION, aLabEntry, EXPECTED_FUNCTION_LABELS), DescriptionFn(), funcID )

      // Ok, succeeded
      aFuncIt.SetStatus(aLab, TFunction_ES_Succeeded);
    }

    ITERATION++;
  }

  doc->CommitCommand();
  return outcome(DescriptionFn(), funcID).success();
}

//! Performs test on disconnecting Tree Function from CAF document. This
//! operation presumes that NOT all underlying CAF Attributes will be cleaned
//! up: the root TDF Label of Tree Function will be kept in order no to erase
//! the corresponding Function from Execution Graph.
//! \param funcID [in] ID of test function.
//! \return true if test is passed, false -- otherwise.
outcome ActTest_TreeFunctionParameter::disconnect_soft(const int funcID)
{
  /* ====================================
   *  Initialize underlying CAF document
   * ==================================== */

  TEST_PRINT_DECOR_L("Create new TREE FUNCTION Parameter");

  ActTest_DocAlloc docAlloc;
  Handle(TDocStd_Document) doc = docAlloc.Doc;

  // Root label
  TDF_Label label;

  doc->NewCommand();
  Handle(ActData_TreeFunctionParameter)
    param = ActParamTool::AsTreeFunction( createParameter(doc, Parameter_TreeFunction, label) );
  doc->CommitCommand();

  /* ====================================
   *  Initialize Tree Function Parameter
   * ==================================== */

  Standard_GUID TEST_GUID = ActTest_DummyDriver::GUID();
  Handle(ActTest_DummyDriver) aDriver = new ActTest_DummyDriver();
  TFunction_DriverTable::Get()->AddDriver(TEST_GUID, aDriver);

  doc->NewCommand();
  param->SetDriverGUID(TEST_GUID);
  doc->CommitCommand();

  /* ================================
   *  Verify Tree Function Parameter
   * ================================ */

  // Verify self-check
  TEST_VERIFY( param->IsWellFormed(), DescriptionFn(), funcID )

  // Access Function Attribute
  Handle(TFunction_Function) aFunctionAttr_BEFORE;
  label.FindAttribute(TFunction_Function::GetID(), aFunctionAttr_BEFORE);
  TEST_VERIFY( !aFunctionAttr_BEFORE.IsNull(), DescriptionFn(), funcID )

  // Access Graph Node Attribute
  Handle(TFunction_GraphNode) aGraphNodeAttr_BEFORE;
  label.FindAttribute(TFunction_GraphNode::GetID(), aGraphNodeAttr_BEFORE);
  TEST_VERIFY( !aGraphNodeAttr_BEFORE.IsNull(), DescriptionFn(), funcID )

  /* ==========================
   *  Disconnect Tree Function
   * ========================== */

  doc->NewCommand();
  param->DisconnectSoft();
  doc->CommitCommand();

  /* ================================
   *  Verify Tree Function Parameter
   * ================================ */

  // Verify self-check
  TEST_VERIFY( param->IsWellFormed(), DescriptionFn(), funcID )

  // Access Function Attribute
  Handle(TFunction_Function) aFunctionAttr_AFTER;
  label.FindAttribute(TFunction_Function::GetID(), aFunctionAttr_AFTER);
  TEST_VERIFY( !aFunctionAttr_AFTER.IsNull(), DescriptionFn(), funcID )

  // Access Graph Node Attribute
  Handle(TFunction_GraphNode) aGraphNodeAttr_AFTER;
  label.FindAttribute(TFunction_GraphNode::GetID(), aGraphNodeAttr_AFTER);
  TEST_VERIFY( !aGraphNodeAttr_AFTER.IsNull(), DescriptionFn(), funcID )

  return outcome(DescriptionFn(), funcID).success();
}

//! Performs test on complete disconnecting Tree Function from CAF document.
//! This operation presumes that all underlying CAF Attributes will be cleaned
//! up along with all references to the root TDF Label of Tree Function.
//! \param funcID [in] ID of test function.
//! \return true if test is passed, false -- otherwise.
outcome ActTest_TreeFunctionParameter::disconnect_full(const int funcID)
{
  /* ====================================
   *  Initialize underlying CAF document
   * ==================================== */

  TEST_PRINT_DECOR_L("Create new TREE FUNCTION Parameter");

  ActTest_DocAlloc docAlloc;
  Handle(TDocStd_Document) doc = docAlloc.Doc;

  // Root label
  TDF_Label label;

  doc->NewCommand();
  Handle(ActData_TreeFunctionParameter)
    param = ActParamTool::AsTreeFunction( createParameter(doc, Parameter_TreeFunction, label) );
  doc->CommitCommand();

  /* ====================================
   *  Initialize Tree Function Parameter
   * ==================================== */

  Standard_GUID TEST_GUID = ActTest_DummyDriver::GUID();
  Handle(ActTest_DummyDriver) aDriver = new ActTest_DummyDriver();
  TFunction_DriverTable::Get()->AddDriver(TEST_GUID, aDriver);

  doc->NewCommand();
  param->SetDriverGUID(TEST_GUID);
  doc->CommitCommand();

  /* ================================
   *  Verify Tree Function Parameter
   * ================================ */

  // Verify self-check
  TEST_VERIFY( param->IsWellFormed(), DescriptionFn(), funcID )

  // Access Function Attribute
  Handle(TFunction_Function) aFunctionAttr_BEFORE;
  label.FindAttribute(TFunction_Function::GetID(), aFunctionAttr_BEFORE);
  TEST_VERIFY( !aFunctionAttr_BEFORE.IsNull(), DescriptionFn(), funcID )

  // Access Graph Node Attribute
  Handle(TFunction_GraphNode) aGraphNodeAttr_BEFORE;
  label.FindAttribute(TFunction_GraphNode::GetID(), aGraphNodeAttr_BEFORE);
  TEST_VERIFY( !aGraphNodeAttr_BEFORE.IsNull(), DescriptionFn(), funcID )

  /* ==========================
   *  Disconnect Tree Function
   * ========================== */

  doc->NewCommand();
  param->Disconnect();
  doc->CommitCommand();

  /* ================================
   *  Verify Tree Function Parameter
   * ================================ */

  // Verify self-check
  TEST_VERIFY( param->IsWellFormed(), DescriptionFn(), funcID )

  // Access Function Attribute
  Handle(TFunction_Function) aFunctionAttr_AFTER;
  label.FindAttribute(TFunction_Function::GetID(), aFunctionAttr_AFTER);
  TEST_VERIFY( aFunctionAttr_AFTER.IsNull(), DescriptionFn(), funcID )

  // Access Graph Node Attribute
  Handle(TFunction_GraphNode) aGraphNodeAttr_AFTER;
  label.FindAttribute(TFunction_GraphNode::GetID(), aGraphNodeAttr_AFTER);
  TEST_VERIFY( aGraphNodeAttr_AFTER.IsNull(), DescriptionFn(), funcID )

  return outcome(DescriptionFn(), funcID).success();
}

#pragma warning(default: 4127) // "Conditional expression is constant" by TEST_VERIFY
#pragma warning(default: 4800) // "Standard_Boolean: forcing value to bool" by TEST_VERIFY
