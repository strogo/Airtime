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
 
 
    Author   : $Author$
    Version  : $Revision$
    Location : $URL$

------------------------------------------------------------------------------*/

/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#include <odbc++/statement.h>
#include <odbc++/preparedstatement.h>
#include <odbc++/resultset.h>

#include "LiveSupport/Db/Conversion.h"
#include "PostgresqlPlayLog.h"

using namespace odbc;

using namespace LiveSupport::Core;
using namespace LiveSupport::Db;
using namespace LiveSupport::Scheduler;

/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  The name of the config element for this class
 *----------------------------------------------------------------------------*/
const std::string PostgresqlPlayLog::configElementNameStr =
                                                        "postgresqlPlayLog";

/*------------------------------------------------------------------------------
 *  A statement to check if the database can be accessed.
 *----------------------------------------------------------------------------*/
const std::string PostgresqlPlayLog::check1Stmt = "SELECT 1";

/*------------------------------------------------------------------------------
 *  A statement to check if the log table exists.
 *----------------------------------------------------------------------------*/
const std::string PostgresqlPlayLog::logCountStmt =
                                        "SELECT COUNT(*) FROM playLog";

/*------------------------------------------------------------------------------
 *  The SQL create statement, used for installation.
 *----------------------------------------------------------------------------*/
const std::string PostgresqlPlayLog::createStmt =
    "CREATE TABLE playLog\n"
    "(\n"
    "   id            BIGINT      NOT NULL,\n"
    "   audioClipId   BIGINT      NOT NULL,\n"
    "   timestamp     TIMESTAMP   NOT NULL,\n"
    "\n"
    "   PRIMARY KEY(id)\n"
    ");";

/*------------------------------------------------------------------------------
 *  The SQL create statement, used for installation.
 *----------------------------------------------------------------------------*/
const std::string PostgresqlPlayLog::dropStmt =
    "DROP TABLE playLog;";

/*------------------------------------------------------------------------------
 *  The SQL statement for adding a play log entry.
 *  It's a simple insert.
 *----------------------------------------------------------------------------*/
const std::string PostgresqlPlayLog::addPlayLogEntryStmt =
    "INSERT INTO playLog(id, audioClipId, timeStamp) VALUES(?, ?, ?)";

/*------------------------------------------------------------------------------
 *  The SQL statement for querying scheduled entries for a time interval
 *  The parameters for this call are: from, to
 *  and returns the properties: audioClipId, timeStamp for all
 *  play log entries between from and to, ordered by timeStamp.
 *  TODO: the below query only lists entries starting inside [from:to[
 *        but what about entries starting before, but flowing into [from:to[ ?
 *----------------------------------------------------------------------------*/
const std::string PostgresqlPlayLog::getPlayLogEntriesStmt =
    "SELECT id, audioClipId, timeStamp FROM playLog WHERE "
    "(? <= timeStamp) AND (timeStamp < ?) "
    "ORDER BY timeStamp";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Configure the schedule.
 *----------------------------------------------------------------------------*/
void
PostgresqlPlayLog :: configure(const xmlpp::Element & element)
                                                throw (std::invalid_argument,
                                                       std::logic_error)
{
    if (element.get_name() != configElementNameStr) {
        std::string eMsg = "bad configuration element ";
        eMsg += element.get_name();
        throw std::invalid_argument(eMsg);
    }

    // nothing to do here, really
}


/*------------------------------------------------------------------------------
 *  Install the PostgresqlPlayLog.
 *----------------------------------------------------------------------------*/
void
PostgresqlPlayLog :: install(void)                     throw (std::exception)
{
    if (!isInstalled()) {
        Ptr<Connection>::Ref    conn;
        try {
            conn = cm->getConnection();
            Ptr<Statement>::Ref     stmt(conn->createStatement());
            stmt->execute(createStmt);
            cm->returnConnection(conn);
        } catch (std::exception &e) {
            if (conn) {
                cm->returnConnection(conn);
            }
            throw std::logic_error(e.what());
        }
    }
}


/*------------------------------------------------------------------------------
 *  Check to see if the PostgresqlPlayLog has already been installed.
 *----------------------------------------------------------------------------*/
bool
PostgresqlPlayLog :: isInstalled(void)                 throw (std::exception)
{
    Ptr<Connection>::Ref    conn;
    try {
        Ptr<Statement>::Ref     stmt;
        ResultSet             * res;

        conn = cm->getConnection();

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
            stmt->execute(logCountStmt);
            res = stmt->getResultSet();
            if (!res->next() || (res->getInt(1) < 0)) {
                cm->returnConnection(conn);
                return false;
            }
        } catch (std::exception &e) {
            cm->returnConnection(conn);
            return false;
        }

        cm->returnConnection(conn);
    } catch (std::exception &e) {
        if (conn) {
            cm->returnConnection(conn);
        }
        throw;
    }

    return true;
}


/*------------------------------------------------------------------------------
 *  Uninstall the PostgresqlPlayLog.
 *----------------------------------------------------------------------------*/
void
PostgresqlPlayLog :: uninstall(void)                   throw (std::exception)
{
    Ptr<Connection>::Ref    conn;
    try {
        conn = cm->getConnection();
        Ptr<Statement>::Ref     stmt(conn->createStatement());
        stmt->execute(dropStmt);
        cm->returnConnection(conn);
    } catch (std::exception &e) {
        if (conn) {
            cm->returnConnection(conn);
        }
        throw std::logic_error(e.what());
    }
}


/*------------------------------------------------------------------------------
 *  Add a new play log entry
 *----------------------------------------------------------------------------*/
Ptr<UniqueId>::Ref
PostgresqlPlayLog :: addPlayLogEntry(
                        Ptr<const UniqueId>::Ref  audioClipId,
                        Ptr<const ptime>::Ref     clipTimestamp)
                                                throw (std::invalid_argument)
{
    Ptr<Connection>::Ref    conn;
    bool                    result = false;
    Ptr<UniqueId>::Ref      id;

    try {
        conn = cm->getConnection();
        Ptr<Timestamp>::Ref     timestamp;
        Ptr<PreparedStatement>::Ref pstmt(conn->prepareStatement(
                                                        addPlayLogEntryStmt));
        id = UniqueId::generateId();
        pstmt->setLong(1, id->getId());

        pstmt->setLong(2, audioClipId->getId());
 
        timestamp = Conversion::ptimeToTimestamp(clipTimestamp);
        pstmt->setTimestamp(3, *timestamp);

        result = (pstmt->executeUpdate() == 1);

        cm->returnConnection(conn);
    } catch (std::exception &e) {
        if (conn) {
            cm->returnConnection(conn);
        }
        throw std::invalid_argument(e.what());
    }

    if (!result) {
        throw std::invalid_argument("couldn't insert into database");
    }

    return id;
}


/*------------------------------------------------------------------------------
 *  Get the play log entries for a given time interval
 *----------------------------------------------------------------------------*/
Ptr<std::vector<Ptr<PlayLogEntry>::Ref> >::Ref
PostgresqlPlayLog :: getPlayLogEntries(
                                    Ptr<const ptime>::Ref  fromTime,
                                    Ptr<const ptime>::Ref  toTime)
                                                throw (std::invalid_argument)
{
    Ptr<Connection>::Ref                                    conn;
    Ptr<std::vector<Ptr<PlayLogEntry>::Ref> >::Ref    result(
                            new std::vector<Ptr<PlayLogEntry>::Ref>());

    try {
        conn = cm->getConnection();
        Ptr<Timestamp>::Ref         timestamp;
        Ptr<PreparedStatement>::Ref pstmt(conn->prepareStatement(
                                            getPlayLogEntriesStmt));
        timestamp = Conversion::ptimeToTimestamp(fromTime);
        pstmt->setTimestamp(1, *timestamp);
        timestamp = Conversion::ptimeToTimestamp(toTime);
        pstmt->setTimestamp(2, *timestamp);

        Ptr<ResultSet>::Ref     rs(pstmt->executeQuery());
        while (rs->next()) {
            Ptr<UniqueId>::Ref      id(new UniqueId(rs->getLong(1)));
            Ptr<UniqueId>::Ref      audioClipId(new UniqueId(rs->getLong(2)));

            *timestamp = rs->getTimestamp(3);
            Ptr<ptime>::Ref clipTimestamp 
                            = Conversion::timestampToPtime(timestamp);

            Ptr<PlayLogEntry>::Ref entry(new PlayLogEntry(id,
                                                          audioClipId,
                                                          clipTimestamp));
            result->push_back(entry);
        }

        cm->returnConnection(conn);
    } catch (std::exception &e) {
        if (conn) {
            cm->returnConnection(conn);
        }
        throw std::invalid_argument("couldn't get play log entries");
    }

    return result;
}

