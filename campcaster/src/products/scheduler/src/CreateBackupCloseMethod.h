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
    Version  : $Revision: 2329 $
    Location : $URL: svn://code.campware.org/campcaster/trunk/campcaster/src/products/scheduler/src/CreateBackupCloseMethod.h $

------------------------------------------------------------------------------*/
#ifndef CreateBackupCloseMethod_h
#define CreateBackupCloseMethod_h

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#include <stdexcept>
#include <string>
#include <XmlRpcServerMethod.h>
#include <XmlRpcValue.h>
#include <XmlRpcException.h>

#include "LiveSupport/Core/Ptr.h"


namespace LiveSupport {
namespace Scheduler {

using namespace LiveSupport;
using namespace LiveSupport::Core;

/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  An XML-RPC method object to close the backup creation process.
 *
 *  The name of the method when called through XML-RPC is "createBackupClose".
 *
 *  The expected parameter is an XML-RPC structure with a single member:
 *  <ul>
 *      <li>token   - string - the token obtained from createBackupOpen </li>
 *  </ul>
 *
 *  In case of an error, a standard XML-RPC fault response is generated, 
 *  and a {&nbsp;faultCode, faultString&nbsp;} structure is returned.  The
 *  possible errors are:
 *  <ul>
 *     <li>4201 - invalid argument format </li>
 *     <li>4202 - missing token argument </li>
 *     <li>4210 - error reported by the scheduler daemon </li>
 *  </ul>
 *
 *  @author  $Author: fgerlits $
 *  @version $Revision: 2329 $
 */
class CreateBackupCloseMethod : public XmlRpc::XmlRpcServerMethod
{
    private:
        /**
         *  The name of this method, as it will be registered into the
         *  XML-RPC server.
         */
        static const std::string        methodName;

        /**
         *  The ID of this method for error reporting purposes.
         */
        static const int                errorId;


    public:
        /**
         *  A default constructor, for testing purposes.
         */
        CreateBackupCloseMethod(void)                                throw ()
                            : XmlRpc::XmlRpcServerMethod(methodName)
        {
        }

        /**
         *  Constuctor that registers the method with the server right away.
         *
         *  @param xmlRpcServer the XML-RPC server to register with.
         */
        CreateBackupCloseMethod(
                    Ptr<XmlRpc::XmlRpcServer>::Ref xmlRpcServer)
                                                                    throw ();

        /**
         *  Execute the createBackupOpen command on the Scheduler daemon.
         *
         *  @param parameters XML-RPC function call parameters
         *  @param returnValue the return value of the call (out parameter)
         */
        void
        execute(XmlRpc::XmlRpcValue &   parameters,
                XmlRpc::XmlRpcValue &   returnValue)
                                            throw (XmlRpc::XmlRpcException);
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */


} // namespace Scheduler
} // namespace LiveSupport

#endif // CreateBackupCloseMethod_h

