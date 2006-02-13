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
#ifndef DisplayScheduleMethodTest_h
#define DisplayScheduleMethodTest_h

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#include <cppunit/extensions/HelperMacros.h>

#include "LiveSupport/Authentication/AuthenticationClientInterface.h"
#include "LiveSupport/Core/SessionId.h"
#include "BaseTestMethod.h"

namespace LiveSupport {
namespace Scheduler {

using namespace LiveSupport;
using namespace LiveSupport::Core;
using namespace LiveSupport::Authentication;

/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  Unit test for the DisplayScheduleMethod class.
 *
 *  @author  $Author$
 *  @version $Revision$
 *  @see DisplayScheduleMethod
 */
class DisplayScheduleMethodTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(DisplayScheduleMethodTest);
    CPPUNIT_TEST(firstTest);
    CPPUNIT_TEST(intervalTest);
    CPPUNIT_TEST_SUITE_END();

    private:

        /**
         *  The schedule used during the test.
         */
        Ptr<ScheduleInterface>::Ref     schedule;

        /**
         *  The authentication client produced by the factory.
         */
        Ptr<AuthenticationClientInterface>::Ref authentication;

        /**
         *  A session ID from the authentication client login() method.
         */
        Ptr<SessionId>::Ref                     sessionId;

        /**
         *  Insert some entries into the schedule to provide test data.
         */
        void
        insertEntries(void)                                 throw ();


    protected:

        /**
         *  A simple test.
         *
         *  @exception CPPUNIT_NS::Exception on test failures.
         */
        void
        firstTest(void)                         throw (CPPUNIT_NS::Exception);

        /**
         *  Look at some intervals, and check them against the test data.
         *
         *  @exception CPPUNIT_NS::Exception on test failures.
         */
        void
        intervalTest(void)                      throw (CPPUNIT_NS::Exception);

    public:
        
        /**
         *  Set up the environment for the test case.
         */
        void
        setUp(void)                                     throw ();

        /**
         *  Clean up the environment after the test case.
         */
        void
        tearDown(void)                                  throw ();
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */


} // namespace Scheduler
} // namespace LiveSupport

#endif // DisplayScheduleMethodTest_h

