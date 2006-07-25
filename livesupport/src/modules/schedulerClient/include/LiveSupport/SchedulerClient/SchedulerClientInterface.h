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
#ifndef LiveSupport_SchedulerClient_SchedulerClientInterface_h
#define LiveSupport_SchedulerClient_SchedulerClientInterface_h

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#include <stdexcept>
#include <vector>
#include "boost/date_time/posix_time/posix_time.hpp"

#include "LiveSupport/Core/Ptr.h"
#include "LiveSupport/Core/UniqueId.h"
#include "LiveSupport/Core/SessionId.h"
#include "LiveSupport/Core/ScheduleEntry.h"
#include "LiveSupport/Core/XmlRpcException.h"
#include "LiveSupport/Core/Playlist.h"
#include "LiveSupport/Core/AudioClip.h"
#include "LiveSupport/Core/SearchCriteria.h"
#include "LiveSupport/Core/AsyncState.h"

namespace LiveSupport {
namespace SchedulerClient {

using namespace boost::posix_time;
using namespace LiveSupport::Core;


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  An interface to access the scheduler daemon as a client.
 *
 *  @author  $Author$
 *  @version $Revision$
 */
class SchedulerClientInterface
{
    public:
        /**
         *  Return the XML-RPC host the client connects to.
         *
         *  @return the XML-RPC host the client connects to.
         */
        virtual Ptr<const std::string>::Ref
        getXmlRpcHost(void) const                   throw ()        = 0;

        /**
         *  Return the XML-RPC port the client connects to.
         *
         *  @return the XML-RPC port the client connects to.
         */
        virtual unsigned int
        getXmlRpcPort(void) const                   throw ()        = 0;

        /**
         *  Return the XML-RPC URI prefix used when connecting to the scheduler.
         *
         *  @return the XML-RPC URI prefix.
         */
        virtual Ptr<const std::string>::Ref
        getXmlRpcUriPrefix(void) const              throw ()        = 0;

        /**
         *  Return the version string for the scheduler this client
         *  is connected to.
         *
         *  @return the version string of the scheduler daemon.
         *  @exception XmlRpcException in case of XML-RPC errors.
         */
        virtual Ptr<const std::string>::Ref
        getVersion(void)                        throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Return the current time at the scheduler server.
         *
         *  @return the current time at the scheduler server.
         *  @exception XmlRpcException in case of XML-RPC errors.
         */
        virtual Ptr<const ptime>::Ref
        getSchedulerTime(void)                  throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Schedule a playlist at a given time.
         *
         *  @param sessionId a valid, authenticated session id.
         *  @param playlistId the id of the playlist to schedule.
         *  @param playtime the time for which to schedule.
         *  @return the schedule entry id for which the playlist has been
         *          scheduled.
         *  @exception XmlRpcException in case of XML-RPC errors.
         */
        virtual Ptr<UniqueId>::Ref
        uploadPlaylist(Ptr<SessionId>::Ref  sessionId,
                       Ptr<UniqueId>::Ref   playlistId,
                       Ptr<ptime>::Ref      playtime)
                                                    throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Return the scheduled entries for a specified time interval.
         *
         *  @param sessionId a valid, authenticated session id.
         *  @param from the start of the interval, inclusive
         *  @param to the end of the interval, exclusive
         *  @return a vector of the schedule entries for the time period.
         *  @exception XmlRpcException in case of XML-RPC errors.
         */
        virtual Ptr<std::vector<Ptr<ScheduleEntry>::Ref> >::Ref
        displaySchedule(Ptr<SessionId>::Ref sessionId,
                        Ptr<ptime>::Ref     from,
                        Ptr<ptime>::Ref     to)
                                                    throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Remove a scheduled item.
         *
         *  @param sessionId a valid, authenticated session id.
         *  @param scheduledEntryId the id of the scheduled entry to remove.
         *  @exception XmlRpcException in case of XML-RPC errors.
         */
        virtual void
        removeFromSchedule(Ptr<SessionId>::Ref  sessionId,
                           Ptr<UniqueId>::Ref   scheduleEntryId)
                                                    throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Start to create a backup by calling the storage, and also
         *  adding a backup of the schedule.
         *  To check if the backup procedure is still pending, call
         *  createBackupCheck() regularly.
         *  Make sure to close the backup by calling createBackupClose().
         *
         *  @param sessionId a valid session ID to use for accessing the
         *         storage
         *  @param criteria the criteria to use for backing up the storage
         *  @param fromTime entries are included in the schedule export starting
         *         from this time.
         *  @param toTime entries as included in the schedule export 
         *         up to but not including this time.
         *  @return a token, which can be used to query the backup process.
         *  @exception XmlRpcException on XML-RPC issues.
         *  @see #createBackupCheck
         *  @see #createBackupClose
         */
        virtual Ptr<Glib::ustring>::Ref
        createBackupOpen(Ptr<SessionId>::Ref        sessionId,
                         Ptr<SearchCriteria>::Ref   criteria,
                         Ptr<ptime>::Ref            fromTime,
                         Ptr<ptime>::Ref            toTime)
                                                throw (XmlRpcException)
                                                                        = 0;
        
        /**
         *  Check the status of a storage backup.
         *
         *  @param  token   the identifier of this backup task.
         *  @param  url     return parameter;
         *                      if the status is "success", it contains the 
         *                      URL of the created backup file.
         *  @param  path    return parameter;
         *                      if the status is "success", it contains the
         *                      local access path of the created backup file.
         *  @param  errorMessage    return parameter;
         *                      if the status is "fault", it contains the
         *                      fault string.
         *  @return the state of the backup process: one of pendingState,
         *                      finishedState, or failedState.
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call.
         *  @see #createBackupOpen
         *  @see #createBackupClose
         */
        virtual AsyncState
        createBackupCheck(const Glib::ustring &             token,
                          Ptr<const Glib::ustring>::Ref &   url,
                          Ptr<const Glib::ustring>::Ref &   path,
                          Ptr<const Glib::ustring>::Ref &   errorMessage)
                                                throw (XmlRpcException)
                                                                        = 0;
        
        /**
         *  Close the storage backup process.
         *  Frees up all resources allocated to the backup.
         *
         *  @param  token           the identifier of this backup task.
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call.
         *  @see #createBackupOpen
         *  @see #createBackupCheck
         */
        virtual void
        createBackupClose(const Glib::ustring &     token)
                                                throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Restore a schedule backup.
         *
         *  All playlist IDs contained in the backup should already be in the
         *  storage.  If this is a combined backup, with both storage and 
         *  schedule components, then restore this backup to the storage
         *  first, and then call this function.
         *  
         *  @param  sessionId   a valid session ID to identify the user.
         *  @param  path        the location of the archive to upload.
         *  @exception  XmlRpcException     if there is an error.
         */
        virtual void
        restoreBackup(Ptr<SessionId>::Ref               sessionId,
                      Ptr<const Glib::ustring>::Ref     path)
                                                throw (XmlRpcException)
                                                                        = 0;
        
        /**
         *  A virtual destructor, as this class has virtual functions.
         */
        virtual
        ~SchedulerClientInterface(void)             throw ()
        {
        }
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */


} // namespace SchedulerClient
} // namespace LiveSupport

#endif // LiveSupport_SchedulerClient_SchedulerClientInterface_h

