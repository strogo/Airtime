/*------------------------------------------------------------------------------

    Copyright (c) 2004 Media Development Loan Fund
 
    This file is part of the LiveSupport project.
    http://livesupport.campware.org/
    To report bugs, send an e-mail to bugs@campware.org
 
    LiveSupport is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
  
    LiveSupport is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 
    You should have received a copy of the GNU General Public License
    along with LiveSupport; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
 
    Author   : $Author$
    Version  : $Revision$
    Location : $URL$

------------------------------------------------------------------------------*/

/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#ifdef HAVE_TIME_H
#include <time.h>
#else
#error need time.h
#endif


#include <string>

#include "ScheduleInterface.h"
#include "ScheduleFactory.h"
#include "LiveSupport/Core/XmlRpcTools.h"

#include "RescheduleMethod.h"


using namespace boost;
using namespace boost::posix_time;

using namespace LiveSupport;
using namespace LiveSupport::Core;

using namespace LiveSupport::Scheduler;

/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  The name of this XML-RPC method.
 *----------------------------------------------------------------------------*/
const std::string RescheduleMethod::methodName = "reschedule";

/*------------------------------------------------------------------------------
 *  The ID of this method for error reporting purposes.
 *----------------------------------------------------------------------------*/
const int RescheduleMethod::errorId = 1300;


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Construct the method and register it right away.
 *----------------------------------------------------------------------------*/
RescheduleMethod :: RescheduleMethod (
                        Ptr<XmlRpc::XmlRpcServer>::Ref xmlRpcServer)   throw()
    : XmlRpc::XmlRpcServerMethod(methodName, xmlRpcServer.get())
{
}


/*------------------------------------------------------------------------------
 *  Execute the upload playlist method XML-RPC function call.
 *----------------------------------------------------------------------------*/
void
RescheduleMethod :: execute(XmlRpc::XmlRpcValue  & rootParameter,
                            XmlRpc::XmlRpcValue  & returnValue)
                                                throw (XmlRpc::XmlRpcException)
{
    if (!rootParameter.valid() || rootParameter.size() != 1
                               || !rootParameter[0].valid()) {
        XmlRpcTools::markError(errorId+1, "invalid argument format", 
                               returnValue);
        return;
    }
    XmlRpc::XmlRpcValue      parameters = rootParameter[0];

    Ptr<SessionId>::Ref      sessionId;
    try{
        sessionId = XmlRpcTools::extractSessionId(parameters);
    } catch (std::invalid_argument &e) {
        XmlRpcTools::markError(errorId+20, 
                               "missing session ID argument",
                                returnValue);
        return;
    }

    Ptr<UniqueId>::Ref  entryId;
    try {
        entryId = XmlRpcTools::extractScheduleEntryId(parameters);
    } catch (std::invalid_argument &e) {
        XmlRpcTools::markError(errorId+2, "missing schedule entry ID argument",
                               returnValue);
        return;
    }

    Ptr<ptime>::Ref     playschedule;
    try {
        playschedule = XmlRpcTools::extractPlayschedule(parameters);
    } catch (std::invalid_argument &e) {
        XmlRpcTools::markError(errorId+3, "missing playtime argument",
                               returnValue);
        return;
    }

    Ptr<UniqueId>::Ref  scheduleEntryId;

    Ptr<ScheduleFactory>::Ref   sf = ScheduleFactory::getInstance();
    Ptr<ScheduleInterface>::Ref schedule = sf->getSchedule();
    Ptr<ScheduleEntry>::Ref     currentlyPlaying;

    if (!schedule->scheduleEntryExists(entryId)) {
        XmlRpcTools::markError(errorId+4, "schedule entry not found",
                               returnValue);
        return;
    }
    currentlyPlaying = schedule->getCurrentlyPlaying();
    if (currentlyPlaying.get()
     && currentlyPlaying->getId()->getId() == entryId->getId()) {
        XmlRpcTools::markError(errorId+6,
                             "the entry to be rescheduled is currently playing",
                               returnValue);
        return;
    }
    try {
        schedule->reschedule(entryId, playschedule);
    } catch (std::invalid_argument &e) {
        XmlRpcTools::markError(errorId+5, e.what(),
                               returnValue);
        return;
    }
}

