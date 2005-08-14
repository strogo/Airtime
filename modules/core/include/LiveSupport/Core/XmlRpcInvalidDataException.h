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
    Version  : $Revision: 1.1 $
    Location : $Source: /home/paul/cvs2svn-livesupport/newcvsrepo/livesupport/modules/core/include/LiveSupport/Core/XmlRpcInvalidDataException.h,v $

------------------------------------------------------------------------------*/
#ifndef LiveSupport_Core_XmlRpcInvalidDataException_h
#define LiveSupport_Core_XmlRpcInvalidDataException_h

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
 *  Exception signaling an XML-RPC problem: invalid data returned by the method.
 *  This could be a malformed AudioClip or Playlist, for example.
 *
 *  @author  $Author: fgerlits $
 *  @version $Revision: 1.1 $
 */
class XmlRpcInvalidDataException : public XmlRpcException
{
    public:
        /**
         *  Constructor based on a string.
         *
         *  @param msg the message of the exception.
         */
        XmlRpcInvalidDataException(const std::string &msg)     throw ()
                    : XmlRpcException(msg)
        {
        }

        /**
         *  Constructor based on a parent exception.
         *
         *  @param parent the parent exception to this one.
         */
        XmlRpcInvalidDataException(const std::exception  & parent)
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
        XmlRpcInvalidDataException(const std::string    & msg,
                                       const std::exception & parent)
                                                                    throw ()
                : XmlRpcException(msg, parent)
        {
        }

        /**
         *  Virtual destructor.
         */
        ~XmlRpcInvalidDataException(void)                         throw ()
        {
        }

};


/* ================================================= external data structures */


/* ====================================================== function prototypes */


} // namespace Core
} // namespace LiveSupport

#endif // LiveSupport_Core_XmlRpcInvalidDataException_h
