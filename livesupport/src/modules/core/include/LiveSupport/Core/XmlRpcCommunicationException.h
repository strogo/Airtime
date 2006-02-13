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
#ifndef LiveSupport_Core_XmlRpcCommunicationException_h
#define LiveSupport_Core_XmlRpcCommunicationException_h

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#include "LiveSupport/Core/XmlRpcException.h"

namespace LiveSupport {
namespace Core {


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  Exception signaling an XML-RPC problem: could not find the XML-RPC server,
 *  the server responded 'no such method', or the method response could not be
 *  parsed.
 *
 *  @author  $Author$
 *  @version $Revision$
 */
class XmlRpcCommunicationException : public XmlRpcException
{
    public:
        /**
         *  Constructor based on a string.
         *
         *  @param msg the message of the exception.
         */
        XmlRpcCommunicationException(const std::string &msg)        throw ()
                    : XmlRpcException(msg)
        {
        }

        /**
         *  Constructor based on a parent exception.
         *
         *  @param parent the parent exception to this one.
         */
        XmlRpcCommunicationException(const std::exception  & parent)
                                                                    throw ()
                : XmlRpcException(parent)
        {
        }

        /**
         *  Constructor based on a message ant a parent exception.
         *
         *  @param msg the message of the exception.
         *  @param parent the parent exception.
         */
        XmlRpcCommunicationException(const std::string    & msg,
                                     const std::exception & parent)
                                                                    throw ()
                : XmlRpcException(msg, parent)
        {
        }

        /**
         *  Virtual destructor.
         */
        ~XmlRpcCommunicationException(void)                         throw ()
        {
        }

};


/* ================================================= external data structures */


/* ====================================================== function prototypes */


} // namespace Core
} // namespace LiveSupport

#endif // LiveSupport_Core_XmlRpcCommunicationException_h

