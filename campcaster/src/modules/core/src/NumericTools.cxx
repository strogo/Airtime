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

#include "LiveSupport/Core/NumericTools.h"


using namespace LiveSupport;
using namespace LiveSupport::Core;

/* ===================================================  local data structures */


/* ================================================  local constants & macros */


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Convert an integer to a string.
 *----------------------------------------------------------------------------*/
Glib::ustring
NumericTools :: itoa(int    number)                                 throw ()
{
    std::ostringstream  stream;
    stream << number;
    Glib::ustring       string = stream.str();
    return string;
}


/*------------------------------------------------------------------------------
 *  Add a number to the end of a string.
 *----------------------------------------------------------------------------*/
Glib::ustring
NumericTools :: addIndex(const Glib::ustring &      baseString,
                         int                        index)          throw ()
{
    std::ostringstream      stream;
    stream << baseString
           << (index + 1);
    return stream.str();
}

