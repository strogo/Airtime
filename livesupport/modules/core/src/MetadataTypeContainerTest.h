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
 
 
    Author   : $Author: fgerlits $
    Version  : $Revision: 1.3 $
    Location : $Source: /home/paul/cvs2svn-livesupport/newcvsrepo/livesupport/modules/core/src/MetadataTypeContainerTest.h,v $

------------------------------------------------------------------------------*/
#ifndef MetadataTypeContainerTest_h
#define MetadataTypeContainerTest_h

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#include <cppunit/extensions/HelperMacros.h>


namespace LiveSupport {
namespace Core {

/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  Unit test for the MetadataTypeContainer class.
 *
 *  @author  $Author: fgerlits $
 *  @version $Revision: 1.3 $
 *  @see MetadataTypeContainer
 */
class MetadataTypeContainerTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(MetadataTypeContainerTest);
    CPPUNIT_TEST(firstTest);
    CPPUNIT_TEST(iteratorTest);
    CPPUNIT_TEST(localizedTest);
    CPPUNIT_TEST_SUITE_END();

    protected:

        /**
         *  The resource bundle used by the container.
         */
        Ptr<ResourceBundle>::Ref            bundle;

        /**
         *  The metadata container used in the tests.
         */
        Ptr<MetadataTypeContainer>::Ref     container;

        /**
         *  A simple test.
         *
         *  @exception CPPUNIT_NS::Exception on test failures.
         */
        void
        firstTest(void)                         throw (CPPUNIT_NS::Exception);

        /**
         *  Testing the iterators available from the container.
         *
         *  @exception CPPUNIT_NS::Exception on test failures.
         */
        void
        iteratorTest(void)                      throw (CPPUNIT_NS::Exception);

        /**
         *  Test to see if localized metadata type names work.
         *
         *  @exception CPPUNIT_NS::Exception on test failures.
         */
        void
        localizedTest(void)                     throw (CPPUNIT_NS::Exception);


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


} // namespace Core
} // namespace LiveSupport

#endif // MetadataTypeContainerTest_h
