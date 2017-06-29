//-----------------------------------------------------------------------------
// Created on: 31 March 2016
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
#include <asiAlgo_Logger.h>

// Active Data includes
#include <ActAux_TimeStamp.h>

// OCCT includes
#include <Standard_ProgramError.hxx>

//-----------------------------------------------------------------------------
// THREAD-UNSAFE methods
//-----------------------------------------------------------------------------

//! Default constructor.
asiAlgo_Logger::asiAlgo_Logger() : ActAPI_ILogger()
{}

//! Returns a copy of the list of messages in OCCT form and cleans up the
//! internal thread-safe TBB collection.
//! \return list of logging messages.
ActAPI_LogMessageList asiAlgo_Logger::PopMessageList()
{
  ActAPI_LogMessageList aResultList;
  ActAPI_LogMessage aMsg;
  while ( m_messageQueue.unsafe_size() > 0 )
  {
    m_messageQueue.try_pop(aMsg);
    aResultList.Append(aMsg);
  }
  return aResultList;
}

//! Checks whether the logger contains any error messages.
//! \return true/false.
unsigned int asiAlgo_Logger::HasErrors()
{
  ActAPI_LogMessageList aResultList = this->PopMessageList();
  for ( ActAPI_LogMessageList::Iterator it(aResultList); it.More(); it.Next() )
  {
    if ( it.Value().Severity == Severity_Error )
      return 1;
  }

  return 0;
}

//! Cleans up the internal collection of messages.
void asiAlgo_Logger::Clear()
{
  m_messageQueue.clear();
}

//-----------------------------------------------------------------------------
// THREAD-SAFE logging kernel methods
//-----------------------------------------------------------------------------

//! Appends a logging message with INFORMATION severity to the Logger queue.
//! \param theMessage   [in] message to add.
//! \param thePriority  [in] priority of the message.
//! \param theArguments [in] message arguments.
//! \param theTimeStamp [in] application-specific timestamp. Current timestamp
//!                          is used in case of NULL value passed.
void asiAlgo_Logger::Info(const TCollection_AsciiString&    theMessage,
                                 const ActAPI_LogMessagePriority   thePriority,
                                 const ActAPI_LogArguments&        theArguments,
                                 const Handle(Standard_Transient)& theTimeStamp)
{
  this->appendMessage(theMessage,
                      Severity_Information,
                      thePriority,
                      theArguments,
                      theTimeStamp.IsNull() ? ActAux_TimeStampTool::Generate().get() : theTimeStamp);
}

//! Appends a logging message with NOTICE severity to the Logger queue.
//! \param theMessage   [in] message to add.
//! \param thePriority  [in] priority of the message.
//! \param theArguments [in] message arguments.
//! \param theTimeStamp [in] application-specific timestamp. Current timestamp
//!                          is used in case of NULL value passed.
void asiAlgo_Logger::Notice(const TCollection_AsciiString&    theMessage,
                                   const ActAPI_LogMessagePriority   thePriority,
                                   const ActAPI_LogArguments&        theArguments,
                                   const Handle(Standard_Transient)& theTimeStamp)
{
  this->appendMessage(theMessage,
                      Severity_Notice,
                      thePriority,
                      theArguments,
                      theTimeStamp.IsNull() ? ActAux_TimeStampTool::Generate().get() : theTimeStamp);
}

//! Appends a logging message with WARNING severity to the Logger queue.
//! \param theMessage   [in] message to add.
//! \param thePriority  [in] priority of the message.
//! \param theArguments [in] message arguments.
//! \param theTimeStamp [in] application-specific timestamp. Current timestamp
//!                          is used in case of NULL value passed.
void asiAlgo_Logger::Warn(const TCollection_AsciiString&    theMessage,
                                 const ActAPI_LogMessagePriority   thePriority,
                                 const ActAPI_LogArguments&        theArguments,
                                 const Handle(Standard_Transient)& theTimeStamp)
{
  this->appendMessage(theMessage,
                      Severity_Warning,
                      thePriority,
                      theArguments,
                      theTimeStamp.IsNull() ? ActAux_TimeStampTool::Generate().get() : theTimeStamp);
}

//! Appends a logging message with ERROR severity to the Logger queue.
//! \param theMessage   [in] message to add.
//! \param thePriority  [in] priority of the message.
//! \param theArguments [in] message arguments.
//! \param theTimeStamp [in] application-specific timestamp. Current timestamp
//!                          is used in case of NULL value passed.
void asiAlgo_Logger::Error(const TCollection_AsciiString&    theMessage,
                                  const ActAPI_LogMessagePriority   thePriority,
                                  const ActAPI_LogArguments&        theArguments,
                                  const Handle(Standard_Transient)& theTimeStamp)
{
  this->appendMessage(theMessage,
                      Severity_Error,
                      thePriority,
                      theArguments,
                      theTimeStamp.IsNull() ? ActAux_TimeStampTool::Generate().get() : theTimeStamp);
}

//! Appends a logging message with the passed severity to the Logger queue.
//! \param theMessage   [in] message to add.
//! \param theSeverity  [in] severity of the message.
//! \param thePriority  [in] priority of the message.
//! \param theArguments [in] message arguments.
//! \param theTimeStamp [in] application-specific timestamp.
void asiAlgo_Logger::appendMessage(const TCollection_AsciiString&           theMessage,
                                          const ActAPI_LogMessageSeverity   theSeverity,
                                          const ActAPI_LogMessagePriority   thePriority,
                                          const ActAPI_LogArguments&        theArguments,
                                          const Handle(Standard_Transient)& theTimeStamp)
{
  std::cout << "\tLOGGER: " << theMessage.ToCString() << std::endl;
  m_messageQueue.push( ActAPI_LogMessage(thePriority,
                                         theSeverity,
                                         theMessage,
                                         theArguments,
                                         theTimeStamp) );
}
