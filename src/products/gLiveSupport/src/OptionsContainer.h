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
 
 
    Author   : $Author $
    Version  : $Revision $
    Location : $URL $

------------------------------------------------------------------------------*/
#ifndef LiveSupport_GLiveSupport_OptionsContainer_h
#define LiveSupport_GLiveSupport_OptionsContainer_h

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#include <stdexcept>
#include <glibmm/ustring.h>
#include "libxml++/libxml++.h"

#include "LiveSupport/Core/Ptr.h"


namespace LiveSupport {
namespace GLiveSupport {

using namespace LiveSupport::Core;
    
/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  A container for the options in gLiveSupport.xml.
 *
 *  @author  $Author $
 *  @version $Revision $
 */
class OptionsContainer
{
    private:
        /**
         *  The XML document containing the options.
         */
        xmlpp::Document         optionsDocument;
        
        /**
         *  Default constructor.
         */
        OptionsContainer(void)                                      throw ()
        {
        }

        /**
         *  Return the first node matching an XPath string.
         *
         *  If there is no matching node, it returns a 0 pointer.
         *
         *  @param  xPath   the XPath of the node (from the root node)
         *  @return a pointer to the node found, or 0
         */
        xmlpp::Node *
        getNode(const Glib::ustring &   xPath)
                                                 throw (std::invalid_argument);
        
        
    public:
        /**
         *  Constructor with XML element parameter.
         *
         *  @param optionsElement   the XML element containing the options
         */
        OptionsContainer(const xmlpp::Element &   optionsElement)   throw ();
        
        /**
         *  The list of string options one can set.
         *  
         *  These are options of type Glib::ustring; any string is accepted
         *  as value, no range checking is done.
         *  
         *  For the moment, this is the only kind of option supported. 
         */
        typedef enum { outputPlayerDeviceName,
                       cuePlayerDeviceName }    OptionItemString;
        
        /**
         *  Set a string type option.
         *
         *  @param      value                   the new value of the option
         *  @exception  std::invalid_argument   if the option name is not found
         */
        void
        setOptionItem(OptionItemString                  optionItem,
                      Ptr<const Glib::ustring>::Ref     value)
                                                 throw (std::invalid_argument);
        
        /**
         *  Get a string type option.
         *
         *  @return     the value of the option
         *  @exception  std::invalid_argument   if the option name is not found
         */
        Ptr<Glib::ustring>::Ref
        getOptionItem(OptionItemString      optionItem)
                                                 throw (std::invalid_argument);
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */


} // namespace GLiveSupport
} // namespace LiveSupport

#endif // LiveSupport_GLiveSupport_OptionsContainer_h

