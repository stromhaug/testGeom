//-----------------------------------------------------------------------------
// Created on: 07 November 2016 (99 years of October Revolution)
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef asiUI_ViewerDomainListener_h
#define asiUI_ViewerDomainListener_h

// asiUI includes
#include <asiUI_ViewerDomain.h>
#include <asiUI_ViewerHost.h>
#include <asiUI_ViewerPart.h>

// Qt includes
#include <QObject>

//! Default slots for domain viewer.
class asiUI_ViewerDomainListener : public QObject
{
  Q_OBJECT

public:

  asiUI_EXPORT
    asiUI_ViewerDomainListener(asiUI_ViewerPart*              wViewerPart,
                               asiUI_ViewerDomain*            wViewerDomain,
                               asiUI_ViewerHost*              wViewerSurface,
                               const Handle(asiEngine_Model)& model);

  asiUI_EXPORT virtual
    ~asiUI_ViewerDomainListener();

public:

  asiUI_EXPORT virtual void
    Connect();

protected:

  asiUI_ViewerPart*       m_wViewerPart;    //!< Part viewer.
  asiUI_ViewerDomain*     m_wViewerDomain;  //!< Face domain viewer.
  asiUI_ViewerHost*       m_wViewerSurface; //!< Surface viewer.
  Handle(asiEngine_Model) m_model;          //!< Data Model instance.

};

#endif
