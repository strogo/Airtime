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
    Version  : $Revision$
    Location : $URL: svn+ssh://fgerlits@code.campware.org/home/svn/repo/livesupport/trunk/livesupport/src/modules/core/include/LiveSupport/Core/FileTools.h $

------------------------------------------------------------------------------*/
#ifndef LiveSupport_Core_FileTools_h
#define LiveSupport_Core_FileTools_h

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#include <stdexcept>

#include "LiveSupport/Core/Ptr.h"


namespace LiveSupport {
namespace Core {

using namespace LiveSupport;
using namespace LiveSupport::Core;

/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  A collection of tools for handling files and URLs.
 *
 *  @author  $Author: fgerlits $
 *  @version $Revision$
 */
class FileTools
{
    public:
        /**
         *  Copy the contents of a URL to a local file.
         *
         *  @param  url     the URL to read.
         *  @param  path    the local path where the file is saved.
         *  @exception  std::runtime_error  on errors.
         */
        static void
        copyUrlToFile(const std::string &   url,
                      const std::string &   path)
                                                throw (std::runtime_error);

        /**
         *  Upload the contents of a local file to a writable URL.
         *
         *  @param  path    the local path where the file is.
         *  @param  url     the URL to write.
         *  @exception  std::runtime_error  on errors.
         */
        static void
        copyFileToUrl(const std::string &   path,
                      const std::string &   url)
                                                throw (std::runtime_error);
};

/* ================================================= external data structures */


/* ====================================================== function prototypes */


} // namespace Core
} // namespace LiveSupport

#endif // LiveSupport_Core_FileTools_h
