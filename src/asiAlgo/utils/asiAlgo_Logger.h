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

#ifndef asiAlgo_Logger_h
#define asiAlgo_Logger_h

// Analysis Situs includes
#include <asiAlgo.h>

// Active Data includes
#include <ActAPI_ILogger.h>

// TBB includes
#include <concurrent_queue.h>

DEFINE_STANDARD_HANDLE(asiAlgo_Logger, ActAPI_ILogger)

//! Logger for algorithmic messages.
class asiAlgo_Logger : public ActAPI_ILogger
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_Logger, ActAPI_ILogger)

// Methods to use in single-threaded context:
public:

  asiAlgo_EXPORT
    asiAlgo_Logger();

  asiAlgo_EXPORT ActAPI_LogMessageList
    PopMessageList();

  asiAlgo_EXPORT void
    Clear();

  asiAlgo_EXPORT unsigned int
    HasErrors();

// THREAD-SAFE logging kernel methods:
public:

  asiAlgo_EXPORT virtual void
    Info(const TCollection_AsciiString&    theMessage,
         const ActAPI_LogMessagePriority   thePriority  = Priority_Normal,
         const ActAPI_LogArguments&        theArguments = ActAPI_LogArguments(),
         const Handle(Standard_Transient)& theTimeStamp = NULL);

  asiAlgo_EXPORT virtual void
    Notice(const TCollection_AsciiString&    theMessage,
           const ActAPI_LogMessagePriority   thePriority  = Priority_Normal,
           const ActAPI_LogArguments&        theArguments = ActAPI_LogArguments(),
           const Handle(Standard_Transient)& theTimeStamp = NULL);

  asiAlgo_EXPORT virtual void
    Warn(const TCollection_AsciiString&    theMessage,
         const ActAPI_LogMessagePriority   thePriority  = Priority_Normal,
         const ActAPI_LogArguments&        theArguments = ActAPI_LogArguments(),
         const Handle(Standard_Transient)& theTimeStamp = NULL);

  asiAlgo_EXPORT virtual void
    Error(const TCollection_AsciiString&    theMessage,
          const ActAPI_LogMessagePriority   thePriority  = Priority_Normal,
          const ActAPI_LogArguments&        theArguments = ActAPI_LogArguments(),
          const Handle(Standard_Transient)& theTimeStamp = NULL);

// Internal methods:
protected:

  virtual void appendMessage(const TCollection_AsciiString&    theMessage,
                             const ActAPI_LogMessageSeverity   theSeverity,
                             const ActAPI_LogMessagePriority   thePriority,
                             const ActAPI_LogArguments&        theArguments,
                             const Handle(Standard_Transient)& theTimeStamp);

private:

  //! Short-cut for queue of messages in TBB thread-safe form.
  typedef tbb::concurrent_queue<ActAPI_LogMessage> _MessageQueue;

private:

  _MessageQueue m_messageQueue; //!< Storage for messages.

};

#endif
