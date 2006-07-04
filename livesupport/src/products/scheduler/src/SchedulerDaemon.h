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
#ifndef SchedulerDaemon_h
#define SchedulerDaemon_h

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#else
#error "Need sys/types.h"
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#else
#error "Need unistd.h"
#endif

#include <string>
#include <stdexcept>
#include <libxml++/libxml++.h>
#include <XmlRpc.h>

#include "LiveSupport/Core/Ptr.h"
#include "LiveSupport/Core/Installable.h"
#include "LiveSupport/Core/Configurable.h"
#include "LiveSupport/Core/SessionId.h"
#include "LiveSupport/Db/ConnectionManagerInterface.h"
#include "LiveSupport/StorageClient/StorageClientInterface.h"
#include "LiveSupport/Authentication/AuthenticationClientInterface.h"
#include "LiveSupport/PlaylistExecutor/AudioPlayerInterface.h"
#include "LiveSupport/EventScheduler/EventScheduler.h"
#include "DisplayScheduleMethod.h"
#include "GeneratePlayReportMethod.h"
#include "GetSchedulerTimeMethod.h"
#include "GetVersionMethod.h"
#include "RemoveFromScheduleMethod.h"
#include "RescheduleMethod.h"
#include "ScheduleInterface.h"
#include "UploadPlaylistMethod.h"
#include "XmlRpcDaemon.h"
#include "LoginMethod.h"
#include "LogoutMethod.h"
#include "ResetStorageMethod.h"
#include "PlayLogInterface.h"


namespace LiveSupport {
namespace Scheduler {

using namespace LiveSupport;
using namespace LiveSupport::Core;
using namespace LiveSupport::Authentication;
using namespace LiveSupport::Db;
using namespace LiveSupport::StorageClient;
using namespace LiveSupport::Scheduler;
using namespace LiveSupport::PlaylistExecutor;

/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  Scheduler daemon main class.
 *  This class is responsible for starting, running and stopping the
 *  Scheduler daemon.
 *
 *  The SchedulerDaemon has to configured by an XML element called
 *  scheduler. This element contains configuration elements for the
 *  compontents used by the scheduler. The configuration file looks
 *  like the following:
 *
 *  <pre><code>
 *  &lt;scheduler&gt;
 *      &lt;user login="userid" password="pwd" /&gt;
 *      &lt;connectionManagerFactory&gt;
 *          ...
 *      &lt;/connectionManagerFactory&gt;
 *      &lt;authenticationClientFactory&gt;
 *          ...
 *      &lt;/authenticationClientFactory&gt;
 *      &lt;storageClientFactory&gt;
 *          ...
 *      &lt;/storageClientFactory&gt;
 *      &lt;scheduleFactory&gt;
 *          ...
 *      &lt;/scheduleFactory&gt;
 *      &lt;playLogFactory&gt;
 *          ...
 *      &lt;/playLogFactory&gt;
 *      &lt;xmlRpcDaemon&gt;
 *          ...
 *      &lt;/xmlRpcDaemon&gt;
 *  &lt;/scheduler&gt;
 *  </code></pre>
 *
 *  The user element holds creditentials for accessing the storage,
 *  configured below.
 *
 *  For details on the included elements, see the corresponding documentation
 *  for XmlRpcDaemon, StorageClientFactory, ConnectionManagerFactory
 *  ScheduleFactory and AuthenticationClientFactory.
 *
 *  The DTD for the above element is the following:
 *
 *  <pre><code>
 *  &lt;!ELEMENT scheduler (user,
 *                       connectionManagerFactory,
 *                       authenticationClientFactory,
 *                       storageClientFactory,
 *                       scheduleFactory,
 *                       playLogFactory,
 *                       audioPlayer,
 *                       xmlRpcDaemon) &gt;
 *  </code></pre>
 *
 *  @author  $Author$
 *  @version $Revision$
 *  @see ConnectionManagerFactory
 *  @see AuthenticationClientFactory
 *  @see StorageClientFactory
 *  @see ScheduleFactory
 *  @see XmlRpcDaemon
 */
class SchedulerDaemon : public Installable,
                        public Configurable,
                        public XmlRpcDaemon
{
    private:

        /**
         *  The SQL create statement used in the installation step.
         */
        static const std::string    createStmt;

        /**
         *  The SQL drop statement used in the uninstallation step.
         */
        static const std::string    dropStmt;

        /**
         *  A SQL statement to check if the database can be accessed.
         */
        static const std::string    check1Stmt;

        /**
         *  A SQL statement to count the number of backups
         */
        static const std::string    backupCountStmt;

        /**
         *  A SQL statement to create a backup entry.
         */
        static const std::string    createBackupStmt;

        /**
         *  A SQL statement to store a backup entry.
         */
        static const std::string    storeBackupStmt;

        /**
         *  A SQL statement to retrieve a backup entry.
         */
        static const std::string    getBackupStmt;

        /**
         *  A SQL statement to update a backup entry.
         */
        static const std::string    updateBackupStmt;

        /**
         *  A SQL statement to delete a backup entry.
         */
        static const std::string    deleteBackupStmt;

        /**
         *  The singleton instance of the scheduler daemon.
         */
        static Ptr<SchedulerDaemon>::Ref            schedulerDaemon;

        /**
         *  The authentication client.
         */
        Ptr<AuthenticationClientInterface>::Ref     authentication;

        /**
         *  The connection manager used by the scheduler.
         */
        Ptr<ConnectionManagerInterface>::Ref        connectionManager;

        /**
         *  The storage client.
         */
        Ptr<StorageClientInterface>::Ref            storage;

        /**
         *  The schedule used by the scheduler daemon.
         */
        Ptr<ScheduleInterface>::Ref                 schedule;

        /**
         *  The session id for the scheduler user.
         */
        Ptr<SessionId>::Ref                         sessionId;

        /**
         *  The event scheduler.
         */
        Ptr<LiveSupport::EventScheduler::EventScheduler>::Ref
                                                    eventScheduler;

        /**
         *  The audio player.
         */
        Ptr<AudioPlayerInterface>::Ref              audioPlayer;

        /**
         *  The play logging facility.
         */
        Ptr<PlayLogInterface>::Ref                  playLog;

        /**
         *  The displayScheduleMethod the daemon is providing.
         */
        Ptr<DisplayScheduleMethod>::Ref         displayScheduleMethod;

        /**
         *  The generatePlayReportMethod the daemon is providing.
         */
        Ptr<GeneratePlayReportMethod>::Ref      generatePlayReportMethod;

        /**
         *  The getSchedulerTimeMethod the daemon is providing.
         */
        Ptr<GetSchedulerTimeMethod>::Ref        getSchedulerTimeMethod;

        /**
         *  The getVersion the daemon is providing.
         */
        Ptr<GetVersionMethod>::Ref              getVersionMethod;

        /**
         *  The removeFromScheduleMethod the daemon is providing.
         */
        Ptr<RemoveFromScheduleMethod>::Ref      removeFromScheduleMethod;

        /**
         *  The rescheduleMethod the daemon is providing.
         */
        Ptr<RescheduleMethod>::Ref              rescheduleMethod;

        /**
         *  The uploadPlaylistMethod the daemon is providing.
         */
        Ptr<UploadPlaylistMethod>::Ref          uploadPlaylistMethod;

        /**
         *  The loginMethod the daemon is providing.
         */
        Ptr<LoginMethod>::Ref                   loginMethod;

        /**
         *  The logoutMethod the daemon is providing.
         */
        Ptr<LogoutMethod>::Ref                  logoutMethod;

        /**
         *  The resetStorageMethod the daemon is providing.
         */
        Ptr<ResetStorageMethod>::Ref            resetStorageMethod;

        /**
         *  The login to the authentication system.
         */
        std::string                             login;

        /**
         *  The password to the authentication system.
         */
        std::string                             password;

        /**
         *  Default constructor.
         */
        SchedulerDaemon (void)                                      throw ();


        /**
         *  Insert a schedule export XML file into an existing tarball.
         *
         *  @param path the file path to the existing tarball.
         *  @param fromTime the time to generate the XML export from
         *  @param toTime the time to generate the XML export to
         *  @throws std::runtime_error on file / tarball handling issues.
         */
        void
        putScheduleExportIntoTar(
                            Ptr<const Glib::ustring>::Ref & path,
                            Ptr<ptime>::Ref                 fromTime,
                            Ptr<ptime>::Ref                 toTime)
                                                    throw (std::runtime_error);
        
        /**
         *  Convert a string status to a StorageClientInterface::AsyncState.
         *  It converts
         *  <ul>
         *      <li> "working"      -> pendingState </li>
         *      <li> "success"      -> finishedState </li>
         *      <li> "fault"        -> failedState </li>
         *      <li> anything else  -> invalidState <li>
         *  </ul>
         */
        StorageClientInterface::AsyncState
        stringToAsyncState(const std::string &      statusString)   throw ();
        
        /**
         *  Convert a StorageClientInterface::AsyncState to a string.
         *  It converts
         *  <ul>
         *      <li> initState or pendingState    -> "working" </li>
         *      <li> finishedState                -> "success" </li>
         *      <li> failedState                  -> "fault"   </li>
         *      <li> anything else                -> "invalid" </li>
         *  </ul>
         */
        std::string
        asyncStateToString(StorageClientInterface::AsyncState   status)
                                                                    throw ();


    protected:

        /**
         *  Register your XML-RPC functions by implementing this function.
         */
        virtual void
        registerXmlRpcFunctions(Ptr<XmlRpc::XmlRpcServer>::Ref  xmlRpcServer)
                                                    throw (std::logic_error);

        /**
         *  Execute any calls when the daemon is starting up.
         *  All resources allocated here should be freed up in shutdown().
         *
         *  @exception std::logic_error if startup could not succeed.
         *  @see #shutdown
         */
        virtual void
        startup (void)                              throw (std::logic_error);


    public:

        /**
         *  Virtual destructor.
         */
        virtual
        ~SchedulerDaemon(void)                          throw ();

        /**
         *  Return a pointer to the singleton instance of SchedulerDaemon.
         *
         *  @return a pointer to the singleton instance of SchedulerDaemon
         */
        static Ptr<SchedulerDaemon>::Ref
        getInstance (void)                              throw ();

        /**
         *  Configure the scheduler daemon based on the XML element
         *  supplied.
         *
         *  @param element the XML element to configure the scheduler
         *                 daemon from.
         *  @exception std::invalid_argument if the supplied XML element
         *             contains bad configuraiton information
         *  @exception std::logic_error if the scheduler daemon has already
         *             been configured.
         */
        void
        configure(const xmlpp::Element    & element)
                                                throw (std::invalid_argument,
                                                       std::logic_error);

        /**
         *  Return the connection manager used by the scheduler.
         *
         *  @return the connection manager used by the scheduler.
         */
        Ptr<ConnectionManagerInterface>::Ref
        getConnectionManager(void)                      throw ()
        {
            return connectionManager;
        }

        /**
         *  Return the storage client used by the scheduler.
         *
         *  @return the storage client used by the scheduler.
         */
        Ptr<StorageClientInterface>::Ref
        getStorage(void)                                throw ()
        {
            return storage;
        }

        /**
         *  Return the authentication client used by the scheduler.
         *
         *  @return the authentication client used by the scheduler.
         */
        Ptr<AuthenticationClientInterface>::Ref
        getAuthentication(void)                         throw ()
        {
            return authentication;
        }

        /**
         *  Return the schedule used by the scheduler.
         *
         *  @return the schedule used by the scheduler.
         */
        Ptr<ScheduleInterface>::Ref
        getSchedule(void)                               throw ()
        {
            return schedule;
        }

        /**
         *  Return the play log used by the scheduler.
         *
         *  @return the play log used by the scheduler.
         */
        Ptr<PlayLogInterface>::Ref
        getPlayLog(void)                                throw ()
        {
            return playLog;
        }

        /**
         *  Return the audio player used by the scheduler.
         *
         *  @return the audio player used by the scheduler.
         */
        Ptr<AudioPlayerInterface>::Ref
        getAudioPlayer(void)                            throw ()
        {
            return audioPlayer;
        }

        /**
         *  Install the component.
         *  This step involves creating the environment in which the component
         *  will run. This may be creation of coniguration files,
         *  database tables, etc.
         *
         *  @exception std::exception on installation problems.
         */
        virtual void
        install(void)                           throw (std::exception);

        /**
         *  Check to see if the component has already been installed.
         *
         *  @return true if the component is properly installed,
         *          false otherwise
         *  @exception std::exception on generic problems
         */
        virtual bool
        isInstalled(void)                       throw (std::exception);

        /**
         *  Uninstall the component.
         *  Removes all the resources created in the install step.
         *
         *  @exception std::exception on unistallation problems.
         */
        virtual void
        uninstall(void)                         throw (std::exception);

        /**
         *  Shut down the daemon.
         *  This function is public only because the signal handler
         *  needs visibility to this function, which will call it.
         *  A call to stop() will trigger a signal that will call shutdown().
         *
         *  @exception std::logic_error if the daemon has not
         *             yet been configured.
         */
        virtual void
        shutdown (void)                             throw (std::logic_error);

        /**
         *  Re-read the scheduled events.
         *  Call this when the events in the schedule under the event container
         *  have changed.
         *
         *  @exception std::logic_error if the daemon has not
         *             yet been configured.
         */
        virtual void
        update(void)                                throw (std::logic_error);

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
                                                throw (XmlRpcException);

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
        virtual StorageClientInterface::AsyncState
        createBackupCheck(const Glib::ustring &             token,
                          Ptr<const Glib::ustring>::Ref &   url,
                          Ptr<const Glib::ustring>::Ref &   path,
                          Ptr<const Glib::ustring>::Ref &   errorMessage)
                                                throw (XmlRpcException);

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
                                                throw (XmlRpcException);

};


/* ================================================= external data structures */


/* ====================================================== function prototypes */


} // namespace Scheduler
} // namespace LiveSupport

#endif // SchedulerDaemon_h

