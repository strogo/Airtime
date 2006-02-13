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

#include <string>
#include <XmlRpcClient.h>
#include <XmlRpcValue.h>

#include "SchedulerDaemon.h"

#include "RpcRevertEditedPlaylistTest.h"


using namespace LiveSupport::Core;
using namespace LiveSupport::Scheduler;

/* ===================================================  local data structures */


/* ================================================  local constants & macros */

CPPUNIT_TEST_SUITE_REGISTRATION(RpcRevertEditedPlaylistTest);


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Set up the test environment
 *----------------------------------------------------------------------------*/
void
RpcRevertEditedPlaylistTest :: setUp(void)                         throw ()
{
    XmlRpc::XmlRpcValue     parameters;
    XmlRpc::XmlRpcValue     result;

    XmlRpc::XmlRpcClient    xmlRpcClient(getXmlRpcHost().c_str(),
                                         getXmlRpcPort(),
                                         "/RPC2",
                                         false);

    CPPUNIT_ASSERT(xmlRpcClient.execute("resetStorage", parameters, result));
    CPPUNIT_ASSERT(!xmlRpcClient.isFault());

    parameters["login"]     = "root";
    parameters["password"]  = "q";
    CPPUNIT_ASSERT(xmlRpcClient.execute("login", parameters, result));
    CPPUNIT_ASSERT(!xmlRpcClient.isFault());
    CPPUNIT_ASSERT(result.hasMember("sessionId"));

    xmlRpcClient.close();

    sessionId.reset(new SessionId(std::string(result["sessionId"])));
}


/*------------------------------------------------------------------------------
 *  Clean up the test environment
 *----------------------------------------------------------------------------*/
void
RpcRevertEditedPlaylistTest :: tearDown(void)                      throw ()
{
    XmlRpc::XmlRpcValue     parameters;
    XmlRpc::XmlRpcValue     result;

    XmlRpc::XmlRpcClient    xmlRpcClient(getXmlRpcHost().c_str(),
                                         getXmlRpcPort(),
                                         "/RPC2",
                                         false);

    parameters["sessionId"] = sessionId->getId();
    CPPUNIT_ASSERT(xmlRpcClient.execute("logout", parameters, result));
    CPPUNIT_ASSERT(!xmlRpcClient.isFault());

    xmlRpcClient.close();
}


/*------------------------------------------------------------------------------
 *  Just a very simple smoke test
 *----------------------------------------------------------------------------*/
void
RpcRevertEditedPlaylistTest :: firstTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    XmlRpc::XmlRpcValue             parameters;
    XmlRpc::XmlRpcValue             result;

    XmlRpc::XmlRpcClient    xmlRpcClient(getXmlRpcHost().c_str(),
                                         getXmlRpcPort(),
                                         "/RPC2",
                                         false);

    parameters["sessionId"]         = sessionId->getId();
    parameters["playlistId"]        = "0000000000000001";
    parameters["playlistElementId"] = "0000000000000101";

    CPPUNIT_ASSERT(xmlRpcClient.execute("revertEditedPlaylist", 
                                        parameters, result));
    CPPUNIT_ASSERT_MESSAGE("allowed to revert playlist without saving it first",
                           xmlRpcClient.isFault());
    CPPUNIT_ASSERT(result.hasMember("faultCode"));
    CPPUNIT_ASSERT(int(result["faultCode"]) == 804);    // no saved copy

    result.clear();
    CPPUNIT_ASSERT(xmlRpcClient.execute("openPlaylistForEditing", 
                                        parameters, result));
    CPPUNIT_ASSERT(!xmlRpcClient.isFault());
    
    result.clear();
    CPPUNIT_ASSERT(xmlRpcClient.execute("removeAudioClipFromPlaylist", 
                                        parameters, result));
    CPPUNIT_ASSERT(!xmlRpcClient.isFault());

    result.clear();                                     // can't remove it twice
    CPPUNIT_ASSERT(xmlRpcClient.execute("removeAudioClipFromPlaylist", 
                                        parameters, result));
    CPPUNIT_ASSERT_MESSAGE("allowed to remove the same playlist element twice",
                           xmlRpcClient.isFault());

    result.clear();
    CPPUNIT_ASSERT(xmlRpcClient.execute("revertEditedPlaylist", 
                                        parameters, result));
    CPPUNIT_ASSERT(!xmlRpcClient.isFault());

    result.clear();                                     // but now we can again
    CPPUNIT_ASSERT(xmlRpcClient.execute("removeAudioClipFromPlaylist", 
                                        parameters, result));
    CPPUNIT_ASSERT(!xmlRpcClient.isFault());

    result.clear();
    CPPUNIT_ASSERT(xmlRpcClient.execute("savePlaylist", 
                                        parameters, result));
    CPPUNIT_ASSERT(!xmlRpcClient.isFault());

    result.clear();
    CPPUNIT_ASSERT(xmlRpcClient.execute("revertEditedPlaylist", 
                                        parameters, result));
    CPPUNIT_ASSERT_MESSAGE(
                    "allowed to revert playlist after discarding saved copy",
                    xmlRpcClient.isFault());
    CPPUNIT_ASSERT(result.hasMember("faultCode"));
    CPPUNIT_ASSERT(int(result["faultCode"]) == 804);    // no saved copy

    xmlRpcClient.close();
}

