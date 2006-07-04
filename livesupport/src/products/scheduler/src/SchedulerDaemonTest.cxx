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

#if HAVE_UNISTD_H
#include <unistd.h>
#else
#error "Need unistd.h"
#endif


#include <string>

#include "SchedulerDaemon.h"
#include "SchedulerDaemonTest.h"


using namespace LiveSupport::Scheduler;

/* ===================================================  local data structures */


/* ================================================  local constants & macros */

CPPUNIT_TEST_SUITE_REGISTRATION(SchedulerDaemonTest);


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Set up the test environment
 *----------------------------------------------------------------------------*/
void
SchedulerDaemonTest :: setUp(void)                              throw ()
{
    Ptr<SchedulerDaemon>::Ref   daemon = SchedulerDaemon::getInstance();
    try {
        Ptr<StorageClientInterface>::Ref    storage = daemon->getStorage();
        storage->reset();

    } catch (std::invalid_argument &e) {
        CPPUNIT_FAIL("semantic error in configuration file");
    } catch (xmlpp::exception &e) {
        CPPUNIT_FAIL("error parsing configuration file");
    } catch (std::exception &e) {
        CPPUNIT_FAIL(e.what());
    }

    authentication = daemon->getAuthentication();
    try {
        sessionId = authentication->login("root", "q");
    } catch (XmlRpcException &e) {
        std::string eMsg = "could not log in:\n";
        eMsg += e.what();
        CPPUNIT_FAIL(eMsg);
    }
}


/*------------------------------------------------------------------------------
 *  Clean up the test environment
 *----------------------------------------------------------------------------*/
void
SchedulerDaemonTest :: tearDown(void)                           throw ()
{
    authentication->logout(sessionId);
    sessionId.reset();
    authentication.reset();
}


/*------------------------------------------------------------------------------
 *  Test to see if the singleton Hello object is accessible
 *----------------------------------------------------------------------------*/
void
SchedulerDaemonTest :: getSingleton(void)       throw (CPPUNIT_NS::Exception)
{
    Ptr<SchedulerDaemon>::Ref   daemon = SchedulerDaemon::getInstance();

    CPPUNIT_ASSERT( daemon.get() );
}


/*------------------------------------------------------------------------------
 *  Test to see if the scheduler starts and stops OK
 *----------------------------------------------------------------------------*/
void
SchedulerDaemonTest :: testStartStop(void)      throw (CPPUNIT_NS::Exception)
{
    Ptr<SchedulerDaemon>::Ref   daemon = SchedulerDaemon::getInstance();

    CPPUNIT_ASSERT( daemon.get() );
    CPPUNIT_ASSERT( !(daemon->isRunning()) );
    daemon->start();
    sleep(3);
    CPPUNIT_ASSERT( daemon->isRunning() );
    daemon->stop();
    sleep(3);
    CPPUNIT_ASSERT( !(daemon->isRunning()) );
}


/*------------------------------------------------------------------------------
 *  Test to see if the backup works as expected
 *----------------------------------------------------------------------------*/
void
SchedulerDaemonTest :: testBackup(void)         throw (CPPUNIT_NS::Exception)
{
    Ptr<SchedulerDaemon>::Ref   daemon = SchedulerDaemon::getInstance();

    Ptr<SearchCriteria>::Ref    criteria(new SearchCriteria);
    criteria->setLimit(10);
    Ptr<ptime>::Ref from(new ptime(time_from_string("2004-07-23 10:00:00")));
    Ptr<ptime>::Ref to(new ptime(time_from_string("2004-07-23 11:00:00")));

    Ptr<Glib::ustring>::Ref     token;
    CPPUNIT_ASSERT_NO_THROW(
        token = daemon->createBackupOpen(sessionId, criteria, from, to);
    );
    CPPUNIT_ASSERT(token);

    Ptr<const Glib::ustring>::Ref   url;
    Ptr<const Glib::ustring>::Ref   path;
    Ptr<const Glib::ustring>::Ref   errorMessage;
    Ptr<Glib::ustring>::Ref         status;
    int     iterations = 20;
    do {
        std::cerr << "-/|\\"[iterations%4] << '\b';
        sleep(1);
        CPPUNIT_ASSERT_NO_THROW(
            status = daemon->createBackupCheck(*token, url, path, errorMessage);
        );
        CPPUNIT_ASSERT(status);
        CPPUNIT_ASSERT(*status == "working"
                         || *status == "success"
                         || *status == "fault");
    } while (--iterations && *status == "working");
    CPPUNIT_ASSERT_EQUAL(std::string("success"), std::string(*status));
    // TODO: test accessibility of the URL?
    
    CPPUNIT_ASSERT_NO_THROW(
        daemon->createBackupClose(*token);
    );
    // TODO: test existence of schedule backup in tarball
}


