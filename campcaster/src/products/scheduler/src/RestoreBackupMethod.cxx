/*------------------------------------------------------------------------------

    Copyright (c) 2004 Media Development Loan Fund
 
    This file is part of the Campcaster project.
    http://campcaster.campware.org/
    To report bugs, send an e-mail to bugs@campware.org
 
    Campcaster is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
  
    Campcaster is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 
    You should have received a copy of the GNU General Public License
    along with Campcaster; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
 
    Author   : $Author: fgerlits $
    Version  : $Revision$
    Location : $URL: svn+ssh://fgerlits@code.campware.org/home/svn/repo/livesupport/branches/scheduler_export/livesupport/src/products/scheduler/src/RestoreBackupMethod.cxx $

------------------------------------------------------------------------------*/

/* ============================================================ include files */

#include "LiveSupport/Core/XmlRpcTools.h"
#include "LiveSupport/Core/XmlRpcException.h"
#include "BackupFactory.h"

#include "RestoreBackupMethod.h"


using namespace LiveSupport;
using namespace LiveSupport::Core;

using namespace LiveSupport::Scheduler;

/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  The name of this XML-RPC method.
 *----------------------------------------------------------------------------*/
const std::string   RestoreBackupMethod::methodName = "restoreBackup";

/*------------------------------------------------------------------------------
 *  The ID of this method for error reporting purposes.
 *----------------------------------------------------------------------------*/
const int           RestoreBackupMethod::errorId    = 4500;


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Construct the method and register it right away.
 *----------------------------------------------------------------------------*/
RestoreBackupMethod :: RestoreBackupMethod (
                                Ptr<XmlRpc::XmlRpcServer>::Ref  xmlRpcServer)
                                                                    throw()
    : XmlRpc::XmlRpcServerMethod(methodName, xmlRpcServer.get())
{
}


/*------------------------------------------------------------------------------
 *  Execute the upload playlist method XML-RPC function call.
 *----------------------------------------------------------------------------*/
void
RestoreBackupMethod :: execute(XmlRpc::XmlRpcValue &     rootParameter,
                                  XmlRpc::XmlRpcValue &     returnValue)
                                                throw (XmlRpc::XmlRpcException)
{
    if (!rootParameter.valid() || rootParameter.size() != 1
                               || !rootParameter[0].valid()) {
        XmlRpcTools::markError(errorId+1, "invalid argument format", 
                               returnValue);
        return;
    }
    XmlRpc::XmlRpcValue         parameters = rootParameter[0];

    Ptr<SessionId>::Ref         sessionId;
    try{
        sessionId = XmlRpcTools::extractSessionId(parameters);
    } catch (std::invalid_argument &e) {
        XmlRpcTools::markError(errorId+20, 
                               "missing session ID argument",
                                returnValue);
        return;
    }
    
    Ptr<Glib::ustring>::Ref     path;
    try{
        path = XmlRpcTools::extractPath(parameters);
        
    } catch (std::invalid_argument &e) {
        XmlRpcTools::markError(errorId+2, 
                               "missing path argument",
                               returnValue);
        return;
    }
    
    Ptr<BackupFactory>::Ref     bf      = BackupFactory::getInstance();
    Ptr<BackupInterface>::Ref   backup  = bf->getBackup();
    try {
        backup->restoreBackup(sessionId, path);
        
    } catch (Core::XmlRpcException &e) {
        XmlRpcTools::markError(errorId+10, e.what(), returnValue);
        return;
    }
}

