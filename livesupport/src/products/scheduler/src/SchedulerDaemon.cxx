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

#if HAVE_SIGNAL_H
#include <signal.h>
#else
#error "Need signal.h"
#endif

#if HAVE_SYS_STAT_H
#include <sys/stat.h>
#else
#error "Need sys/stat.h"
#endif


#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdio>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <odbc++/statement.h>
#include <odbc++/preparedstatement.h>
#include <odbc++/resultset.h>

#include "LiveSupport/Core/FileTools.h"
#include "LiveSupport/Db/ConnectionManagerFactory.h"
#include "LiveSupport/Db/Conversion.h"
#include "LiveSupport/Authentication/AuthenticationClientFactory.h"
#include "LiveSupport/StorageClient/StorageClientFactory.h"
#include "LiveSupport/PlaylistExecutor/AudioPlayerFactory.h"
#include "ScheduleFactory.h"
#include "PlayLogFactory.h"
#include "PlaylistEventContainer.h"

#include "SchedulerDaemon.h"

using namespace boost::posix_time;
using namespace odbc;
using namespace xmlpp;

using namespace LiveSupport;
using namespace LiveSupport::Core;
using namespace LiveSupport::Db;
using namespace LiveSupport::StorageClient;
using namespace LiveSupport::Scheduler;

/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/**
 *  The singleton instance of the Scheduler daemon object.
 */
Ptr<SchedulerDaemon>::Ref   SchedulerDaemon::schedulerDaemon;

namespace {

/**
 *  The name of the XML configuration element for the Scheduler daemon.
 */
const std::string confElement = "scheduler";

/**
 *  The name of the XML configuration element for the XmlRpcDaemon inside.
 */
const std::string xmlRpcDaemonConfElement = "xmlRpcDaemon";

/**
 *  The name of the config child element for the login and password
 */
const std::string    userConfigElementName = "user";

/**
 *  The name of the config element attribute for the login
 */
const std::string    userLoginAttrName = "login";

/**
 *  The name of the config element attribute for the password
 */
const std::string    userPasswordAttrName = "password";

/**
 *  The working backup state
 */
const std::string    workingState = "working";

/**
 *  The finished / success backup state
 */
const std::string    successState = "success";

/**
 *  The finished / failure backup state
 */
const std::string    failureState = "fault";

/**
 *  The name of the schedule export fie in the export tarbal;
 */
const std::string    scheduleExportFileName = "meta-inf/scheduler.xml";

}

/*------------------------------------------------------------------------------
 *  The SQL create statement, used for installation.
 *----------------------------------------------------------------------------*/
const std::string SchedulerDaemon::createStmt =
    "CREATE TABLE backup\n"
    "(\n"
    "   token       VARCHAR(64)     NOT NULL,\n"
    "   sessionId   VARCHAR(64)     NOT NULL,\n"
    "   status      VARCHAR(32)     NOT NULL,\n"
    "   fromTime    TIMESTAMP       NOT NULL,\n"
    "   toTime      TIMESTAMP       NOT NULL,\n"
    "\n"
    "   PRIMARY KEY(token)\n"
    ");";

/*------------------------------------------------------------------------------
 *  The SQL create statement, used for installation.
 *----------------------------------------------------------------------------*/
const std::string SchedulerDaemon::dropStmt =
    "DROP TABLE backup;";

/*------------------------------------------------------------------------------
 *  A statement to check if the database can be accessed.
 *----------------------------------------------------------------------------*/
const std::string SchedulerDaemon::check1Stmt = "SELECT 1";

/*------------------------------------------------------------------------------
 *  A statement to check if the backup table exists.
 *----------------------------------------------------------------------------*/
const std::string SchedulerDaemon::backupCountStmt =
                                        "SELECT COUNT(*) FROM backup";

/*------------------------------------------------------------------------------
 *  A statement to store a backup entry.
 *  - token - the token of the backup
 *  - status - the status of the backup, either 'working', 'success' or 'fault'
 *  - fromTime - the start time of the schedule backup
 *  - toTime - the end time of the schedule backup
 *----------------------------------------------------------------------------*/
const std::string SchedulerDaemon::storeBackupStmt =
             "INSERT INTO backup(token, sessionId, status, fromTime, toTime) "
                     "VALUES(?, ?, ?, ?, ?)";

/*------------------------------------------------------------------------------
 *  Get a backup from the database.
 *  - token - the token of an existing backup
 *----------------------------------------------------------------------------*/
const std::string SchedulerDaemon::getBackupStmt =
             "SELECT token, sessionId, status, fromTime, toTime FROM backup "
                    "WHERE token = ?";

/*------------------------------------------------------------------------------
 *  A statement to update a backup entry.
 *  - status - the new status of the backup
 *  - token - the token of an existing backup
 *----------------------------------------------------------------------------*/
const std::string SchedulerDaemon::updateBackupStmt =
                                "UPDATE backup SET status = ? WHERE token = ?";

/*------------------------------------------------------------------------------
 *  A statement to delete a backup entry
 *  - token - the token of an existing backup
 *----------------------------------------------------------------------------*/
const std::string SchedulerDaemon::deleteBackupStmt =
                                        "DELETE FROM backup WHERE token = ?";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  The default constructor.
 *----------------------------------------------------------------------------*/
SchedulerDaemon :: SchedulerDaemon (void)                   throw ()
                        : XmlRpcDaemon()
{
    displayScheduleMethod.reset(new DisplayScheduleMethod());
    generatePlayReportMethod.reset(new GeneratePlayReportMethod());
    getSchedulerTimeMethod.reset(new GetSchedulerTimeMethod());
    getVersionMethod.reset(new GetVersionMethod());
    removeFromScheduleMethod.reset(new RemoveFromScheduleMethod());
    rescheduleMethod.reset(new RescheduleMethod());
    uploadPlaylistMethod.reset(new UploadPlaylistMethod());
    loginMethod.reset(new LoginMethod());
    logoutMethod.reset(new LogoutMethod());
    resetStorageMethod.reset(new ResetStorageMethod());
    createBackupOpenMethod.reset(new CreateBackupOpenMethod());
}


/*------------------------------------------------------------------------------
 *  Return the singleton instnace.
 *----------------------------------------------------------------------------*/
Ptr<SchedulerDaemon>::Ref
SchedulerDaemon :: getInstance (void)                       throw ()
{
    if (!schedulerDaemon) {
        schedulerDaemon.reset(new SchedulerDaemon());
    }

    return schedulerDaemon;
}


/*------------------------------------------------------------------------------
 *  Configure the scheduler daemon
 *----------------------------------------------------------------------------*/
void
SchedulerDaemon :: configure(const xmlpp::Element    & element)
                                                throw (std::invalid_argument,
                                                       std::logic_error)
{
    if (element.get_name() != confElement) {
        std::string eMsg = "Bad configuration element ";
        eMsg += element.get_name();
        throw std::invalid_argument(eMsg);
    }

    xmlpp::Node::NodeList       nodes;
    const xmlpp::Element      * elem = 0;
    const xmlpp::Attribute    * attribute = 0;

    // read in the user data

    nodes = element.get_children(userConfigElementName);
    if (nodes.size() < 1) {
        throw std::invalid_argument("no user element");
    }
    elem = dynamic_cast<const xmlpp::Element*> (*nodes.begin());
    if (!(attribute = elem->get_attribute(userLoginAttrName))) {
        throw std::invalid_argument("missing login attribute");
    }
    login = attribute->get_value();
    if (!(attribute = elem->get_attribute(userPasswordAttrName))) {
        throw std::invalid_argument("missing password attribute");
    }
    password = attribute->get_value();

    // configure the ConnectionManagerFactory
    nodes =
         element.get_children(ConnectionManagerFactory::getConfigElementName());
    if (nodes.size() < 1) {
        throw std::invalid_argument("no connectionManagerFactory  element");
    }
    Ptr<ConnectionManagerFactory>::Ref cmf
                                = ConnectionManagerFactory::getInstance();
    cmf->configure( *((const xmlpp::Element*) *(nodes.begin())) );

    // configure the AuthenticationClientFactory
    nodes =
      element.get_children(AuthenticationClientFactory::getConfigElementName());
    if (nodes.size() < 1) {
        throw std::invalid_argument("no authenticationClientFactory element");
    }
    Ptr<AuthenticationClientFactory>::Ref acf
                                = AuthenticationClientFactory::getInstance();
    acf->configure( *((const xmlpp::Element*) *(nodes.begin())) );

    // configure the StorageClientFactory
    nodes = element.get_children(StorageClientFactory::getConfigElementName());
    if (nodes.size() < 1) {
        throw std::invalid_argument("no storageClientFactory element");
    }
    Ptr<StorageClientFactory>::Ref scf = StorageClientFactory::getInstance();
    scf->configure( *((const xmlpp::Element*) *(nodes.begin())) );

    // configure the AudioPlayerFactory
    nodes = element.get_children(AudioPlayerFactory::getConfigElementName());
    if (nodes.size() < 1) {
        throw std::invalid_argument("no audioPlayer element");
    }
    Ptr<AudioPlayerFactory>::Ref    apf = AudioPlayerFactory::getInstance();
    apf->configure( *((const xmlpp::Element*) *(nodes.begin())) );

    // configure the ScheduleFactory
    nodes = element.get_children(ScheduleFactory::getConfigElementName());
    if (nodes.size() < 1) {
        throw std::invalid_argument("no scheduleFactory element");
    }
    Ptr<ScheduleFactory>::Ref   sf = ScheduleFactory::getInstance();
    sf->configure( *((const xmlpp::Element*) *(nodes.begin())) );

    // configure the PlayLogFactory
    nodes = element.get_children(PlayLogFactory::getConfigElementName());
    if (nodes.size() < 1) {
        throw std::invalid_argument("no playLogFactory element");
    }
    Ptr<PlayLogFactory>::Ref   plf = PlayLogFactory::getInstance();
    plf->configure( *((const xmlpp::Element*) *(nodes.begin())) );

    // configure the XmlRpcDaemon
    nodes = element.get_children(XmlRpcDaemon::getConfigElementName());
    if (nodes.size() < 1) {
        throw std::invalid_argument("no xmlRpcDaemon element");
    }
    configureXmlRpcDaemon( *((const xmlpp::Element*) *(nodes.begin())) );

    // do some initialization, using the configured objects
    authentication    = acf->getAuthenticationClient();
    connectionManager = cmf->getConnectionManager();
    storage           = scf->getStorageClient();
    audioPlayer       = apf->getAudioPlayer();
    playLog           = plf->getPlayLog();
    schedule          = sf->getSchedule();
}


/*------------------------------------------------------------------------------
 *  Destructor.
 *----------------------------------------------------------------------------*/
SchedulerDaemon :: ~SchedulerDaemon(void)                       throw ()
{
    if (authentication.get() && sessionId.get()) {
        authentication->logout(sessionId);
    }
}

 
/*------------------------------------------------------------------------------
 *  Register our XML-RPC methods
 *----------------------------------------------------------------------------*/
void
SchedulerDaemon :: registerXmlRpcFunctions(
                            Ptr<XmlRpc::XmlRpcServer>::Ref  xmlRpcServer)
                                                    throw (std::logic_error)
{
    xmlRpcServer->addMethod(displayScheduleMethod.get());
    xmlRpcServer->addMethod(generatePlayReportMethod.get());
    xmlRpcServer->addMethod(getSchedulerTimeMethod.get());
    xmlRpcServer->addMethod(getVersionMethod.get());
    xmlRpcServer->addMethod(removeFromScheduleMethod.get());
    xmlRpcServer->addMethod(rescheduleMethod.get());
    xmlRpcServer->addMethod(uploadPlaylistMethod.get());
    xmlRpcServer->addMethod(loginMethod.get());
    xmlRpcServer->addMethod(logoutMethod.get());
    xmlRpcServer->addMethod(resetStorageMethod.get());
    xmlRpcServer->addMethod(createBackupOpenMethod.get());
}


/*------------------------------------------------------------------------------
 *  Install the scheduler daemon.
 *----------------------------------------------------------------------------*/
void
SchedulerDaemon :: install(void)                throw (std::exception)
{
    if (!isInstalled()) {
        // TODO: check if we have already been configured
        Ptr<ScheduleFactory>::Ref   sf = ScheduleFactory::getInstance();
        sf->install();
        Ptr<PlayLogFactory>::Ref    plf = PlayLogFactory::getInstance();
        plf->install();

        Ptr<Connection>::Ref    conn;
        try {
            conn = connectionManager->getConnection();
            Ptr<Statement>::Ref     stmt(conn->createStatement());
            stmt->execute(createStmt);
            connectionManager->returnConnection(conn);
        } catch (std::exception &e) {
            if (conn) {
                connectionManager->returnConnection(conn);
            }
            throw;
        }
    }
}


/*------------------------------------------------------------------------------
 *  Check to see if the scheduler has been installed.
 *----------------------------------------------------------------------------*/
bool
SchedulerDaemon :: isInstalled(void)            throw (std::exception)
{
    // TODO: check if we have already been configured
    Ptr<ScheduleFactory>::Ref   sf = ScheduleFactory::getInstance();
    Ptr<PlayLogFactory>::Ref    plf = PlayLogFactory::getInstance();

    if (!sf.get() || !plf.get()) {
        throw std::logic_error("coudln't initialize factories");
    }
    
    if (!(sf->isInstalled() && plf->isInstalled())) {
        return false;
    }

    Ptr<Connection>::Ref    conn;
    try {
        Ptr<Statement>::Ref     stmt;
        ResultSet             * res;

        conn = connectionManager->getConnection();

        // see if we can connect at all
        stmt.reset(conn->createStatement());
        stmt->execute(check1Stmt);
        res = stmt->getResultSet();
        if (!res->next() || (res->getInt(1) != 1)) {
            throw std::runtime_error("Can't connect to database");
        }

        // see if the schedule table exists
        try {
            stmt.reset(conn->createStatement());
            stmt->execute(backupCountStmt);
            res = stmt->getResultSet();
            if (!res->next() || (res->getInt(1) < 0)) {
                connectionManager->returnConnection(conn);
                return false;
            }
        } catch (std::exception &e) {
            connectionManager->returnConnection(conn);
            return false;
        }

        connectionManager->returnConnection(conn);
    } catch (std::exception &e) {
        if (conn) {
            connectionManager->returnConnection(conn);
        }
        throw;
    }

    return true;
}


/*------------------------------------------------------------------------------
 *  Install the scheduler daemon.
 *----------------------------------------------------------------------------*/
void
SchedulerDaemon :: uninstall(void)              throw (std::exception)
{
    // TODO: check if we have already been configured
    Ptr<Connection>::Ref    conn;
    try {
        conn = connectionManager->getConnection();
        Ptr<Statement>::Ref     stmt(conn->createStatement());
        stmt->execute(dropStmt);
        connectionManager->returnConnection(conn);
    } catch (std::exception &e) {
        if (conn) {
            connectionManager->returnConnection(conn);
        }
        throw;
    }

    Ptr<PlayLogFactory>::Ref    plf = PlayLogFactory::getInstance();
    try {
        plf->uninstall();
    } catch (std::exception &e) {
        // TODO: don't print but throw it instead
        std::cerr << e.what() << std::endl;
    }
    
    Ptr<ScheduleFactory>::Ref   sf = ScheduleFactory::getInstance();
    sf->uninstall();
}


/*------------------------------------------------------------------------------
 *  Execute daemon startup functions.
 *----------------------------------------------------------------------------*/
void
SchedulerDaemon :: startup (void)                   throw (std::logic_error)
{
    try {
        if (!isInstalled()) {
            install();
        }
    } catch (std::exception &e) {
        throw std::logic_error(std::string("database installation problem: ")
                               + e.what());
    }

    try {
        sessionId      = authentication->login(login, password);
    } catch (XmlRpcException &e) {
        throw std::logic_error(std::string("authentication problem: ")
                               + e.what());
    }

    try {
        audioPlayer->initialize();
    } catch (std::exception &e) {
        throw std::logic_error(std::string("audio player initialization "
                               "problem: ") + e.what());
    }
    if (!eventScheduler.get()) {
        Ptr<PlaylistEventContainer>::Ref    eventContainer;
        Ptr<time_duration>::Ref             granularity;
        eventContainer.reset(new PlaylistEventContainer(sessionId,
                                                        storage,
                                                        schedule,
                                                        audioPlayer,
                                                        playLog));
        // TODO: read granularity from config file
        granularity.reset(new time_duration(seconds(1)));

        eventScheduler.reset(
                new LiveSupport::EventScheduler::EventScheduler(eventContainer,
                                                                granularity));
    }
    eventScheduler->start();

    XmlRpcDaemon::startup();
}


/*------------------------------------------------------------------------------
 *  Shut down the daemon
 *----------------------------------------------------------------------------*/
void
SchedulerDaemon :: shutdown(void)               throw (std::logic_error)
{
    if (eventScheduler.get()) {
        eventScheduler->stop();
    }
    audioPlayer->deInitialize();

    XmlRpcDaemon::shutdown();
}


/*------------------------------------------------------------------------------
 *  Re-read the events from the event container.
 *----------------------------------------------------------------------------*/
void
SchedulerDaemon :: update (void)                throw (std::logic_error)
{
    // TODO: check if we've been configured
    if (eventScheduler.get()) {
        eventScheduler->update();
    }
}


/*------------------------------------------------------------------------------
 *  Start a backup process.
 *----------------------------------------------------------------------------*/
Ptr<Glib::ustring>::Ref
SchedulerDaemon :: createBackupOpen(Ptr<SessionId>::Ref        sessionId,
                                    Ptr<SearchCriteria>::Ref   criteria,
                                    Ptr<ptime>::Ref            fromTime,
                                    Ptr<ptime>::Ref            toTime)
                                                        throw (XmlRpcException)
{
    Ptr<Glib::ustring>::Ref     token;
    Ptr<Connection>::Ref        conn;
    bool                        result = false;

    // open up a backup process with the storage server
    token = storage->createBackupOpen(sessionId, criteria);

    // store the details of the backup, with a pending status
    try {
        conn = connectionManager->getConnection();
        Ptr<Timestamp>::Ref         timestamp;
        Ptr<PreparedStatement>::Ref pstmt(conn->prepareStatement(
                                                        storeBackupStmt));
        pstmt->setString(1, *token);
        pstmt->setString(2, sessionId->getId());
        pstmt->setString(3, asyncStateToString(
                                    StorageClientInterface::pendingState));

        timestamp = Conversion::ptimeToTimestamp(fromTime);
        pstmt->setTimestamp(4, *timestamp);

        timestamp = Conversion::ptimeToTimestamp(toTime);
        pstmt->setTimestamp(5, *timestamp);

        result = pstmt->executeUpdate() == 1;

        connectionManager->returnConnection(conn);
    } catch (std::exception &e) {
        if (conn) {
            connectionManager->returnConnection(conn);
        }
        throw std::invalid_argument(e.what());
    }

    if (!result) {
        throw std::invalid_argument("couldn't insert into database");
    }

    return token;
}


/*------------------------------------------------------------------------------
 *  Check on the status of a backup process.
 *----------------------------------------------------------------------------*/
StorageClientInterface::AsyncState
SchedulerDaemon :: createBackupCheck(
                            const Glib::ustring &             token,
                            Ptr<const Glib::ustring>::Ref &   url,
                            Ptr<const Glib::ustring>::Ref &   path,
                            Ptr<const Glib::ustring>::Ref &   errorMessage)
                                                        throw (XmlRpcException)
{
    Ptr<Connection>::Ref                conn;
    StorageClientInterface::AsyncState  status;
    Ptr<ptime>::Ref                     fromTime;
    Ptr<ptime>::Ref                     toTime;
    bool                                result;

    // first, check on the status ourselves
    try {
        Ptr<Timestamp>::Ref     timestamp;

        conn = connectionManager->getConnection();
        Ptr<PreparedStatement>::Ref pstmt(conn->prepareStatement(
                                                                getBackupStmt));

        pstmt->setString(1, token);

        Ptr<ResultSet>::Ref     rs(pstmt->executeQuery());
        if (rs->next()) {
            status = stringToAsyncState(rs->getString(3));
            
            timestamp.reset(new Timestamp(rs->getTimestamp(4)));
            fromTime = Conversion::timestampToPtime(timestamp);

            timestamp.reset(new Timestamp(rs->getTimestamp(5)));
            toTime = Conversion::timestampToPtime(timestamp);
        }

        connectionManager->returnConnection(conn);
    } catch (std::exception &e) {
        if (conn) {
            connectionManager->returnConnection(conn);
        }
        // TODO: report error
        return status;
    }

    if (status == StorageClientInterface::pendingState) {
        status = storage->createBackupCheck(token, url, path, errorMessage);

        if (status == StorageClientInterface::finishedState) {
            putScheduleExportIntoTar(path, fromTime, toTime);
        }
    }

    // update the status
    try {
        conn = connectionManager->getConnection();
        Ptr<Timestamp>::Ref         timestamp;
        Ptr<PreparedStatement>::Ref pstmt(conn->prepareStatement(
                                                        updateBackupStmt));
        pstmt->setString(1, asyncStateToString(status));
        pstmt->setString(2, token);

        result = pstmt->executeUpdate() == 1;

        connectionManager->returnConnection(conn);
    } catch (std::exception &e) {
        if (conn) {
            connectionManager->returnConnection(conn);
        }
        throw std::invalid_argument(e.what());
    }

    if (!result) {
        throw std::invalid_argument("couldn't insert into database");
    }

    return status;
}


/*------------------------------------------------------------------------------
 *  Close a backup process, and free up all resources.
 *----------------------------------------------------------------------------*/
void
SchedulerDaemon :: putScheduleExportIntoTar(
                        Ptr<const Glib::ustring>::Ref & path,
                        Ptr<ptime>::Ref                 fromTime,
                        Ptr<ptime>::Ref                 toTime)
                                                    throw (std::runtime_error)
{
    Ptr<Document>::Ref      document(new Document());
    Element               * root = document->create_root_node("scheduler");
    std::string             tmpFileName = FileTools::tempnam();

    // create the export, and write it to a temporary file
    schedule->exportScheduleEntries(root, fromTime, toTime);
    document->write_to_file(tmpFileName);

    try {
        FileTools::appendFileToTarball(*path,
                                       tmpFileName,
                                       scheduleExportFileName);
    } catch (std::runtime_error &e) {
        remove(tmpFileName.c_str());
        throw;
    }

    remove(tmpFileName.c_str());
}


/*------------------------------------------------------------------------------
 *  Close a backup process, and free up all resources.
 *----------------------------------------------------------------------------*/
void
SchedulerDaemon :: createBackupClose(const Glib::ustring &     token)
                                                        throw (XmlRpcException)
{
    Ptr<Connection>::Ref        conn;
    bool                        result;

    storage->createBackupClose(token);

    // delete the backup from our database
    try {
        conn = connectionManager->getConnection();
        Ptr<Timestamp>::Ref         timestamp;
        Ptr<PreparedStatement>::Ref pstmt(conn->prepareStatement(
                                                        deleteBackupStmt));
        pstmt->setString(1, token);

        result = pstmt->executeUpdate() == 1;

        connectionManager->returnConnection(conn);
    } catch (std::exception &e) {
        if (conn) {
            connectionManager->returnConnection(conn);
        }
        throw std::invalid_argument(e.what());
    }

    if (!result) {
        throw std::invalid_argument("couldn't insert into database");
    }
}


/*------------------------------------------------------------------------------
 *  Convert a string status to a StorageClientInterface::AsyncState.
 *----------------------------------------------------------------------------*/
StorageClientInterface::AsyncState
SchedulerDaemon :: stringToAsyncState(const std::string &      statusString)
                                                                    throw ()
{
    if (statusString == "working") {
        return StorageClientInterface::pendingState;
        
    } else if (statusString == "success") {
        return StorageClientInterface::finishedState;
    
    } else if (statusString == "fault") {
        return StorageClientInterface::failedState;
        
    } else {
        return StorageClientInterface::invalidState;
    }
}


/*------------------------------------------------------------------------------
 *  Convert a StorageClientInterface::AsyncState to a string.
 *----------------------------------------------------------------------------*/
std::string
SchedulerDaemon :: asyncStateToString(
                            StorageClientInterface::AsyncState   status)
                                                                    throw ()
{
    std::string     statusString;

    switch (status) {
        case StorageClientInterface::initState:
        case StorageClientInterface::pendingState:  statusString = "working";
                                                    break;
        
        case StorageClientInterface::finishedState: statusString = "success";
                                                    break;
        
        case StorageClientInterface::failedState:   statusString = "fault";
                                                    break;
        
        default:                                    statusString = "invalid";
                                                    break;
    }
    
    return statusString;
}

