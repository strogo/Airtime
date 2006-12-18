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
 
 
    Author   : $Author $
    Version  : $Revision $
    Location : $URL $

------------------------------------------------------------------------------*/
#ifndef LiveSupport_Core_OptionsContainer_h
#define LiveSupport_Core_OptionsContainer_h

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
namespace Core {
    
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
    public:
        /**
         *  The list of string options one can set.
         *  
         *  These are options of type Glib::ustring; any string is accepted
         *  as value, no range checking is done.
         *  
         *  For the moment, this is the only kind of option supported. 
         */
        typedef enum { outputPlayerDeviceName,
                       cuePlayerDeviceName,
                       authenticationServer,
                       authenticationPort,
                       authenticationPath,
                       storageServer,
                       storagePort,
                       storagePath,
                       schedulerServer,
                       schedulerPort,
                       schedulerPath }  OptionItemString;
        

    private:
        /**
         *  The XML document containing the options.
         */
        xmlpp::Document                 optionsDocument;
        
        /**
         *  The file name (including path) used by writeToFile().
         */
        Ptr<const Glib::ustring>::Ref   configFileName;
        
        /**
         *  Remember if we have been touched.
         */
        bool                            touched;
        
        /**
         *  Default constructor.
         */
        OptionsContainer(void)                                      throw ()
        {
        }

        /**
         *  Find the node corresponding to an OptionItemString value.
         *
         *  If there is no matching node, it returns a 0 pointer.
         *
         *  @param  optionItem      the name of the item to find the node for
         *  @param  isAttribute     return parameter; is set to true if the
         *                              node is an attribute, false if it's
         *                              a CDATA text
         *  @return a pointer to the node found, or 0
         *  @exception  std::invalid_argument   thrown by getNode() [should
         *                                      never happen]
         */
        xmlpp::Node *
        selectNode(OptionItemString     optionItem,
                   bool &               isAttribute)
                                                throw (std::invalid_argument);
        
        /**
         *  Find the node corresponding to a keyboard shortcut.
         *
         *  If there is no matching node, it returns a 0 pointer.
         *
         *  @param  containerNo     the number of the KeyboardShortcutContainer
         *                              (starting with 1, as per XPath)
         *  @param  shortcutNo      the number of the KeyboardShortcut within
         *                              this container (also starting with 1)
         *  @return a pointer to the node found, or 0
         *  @exception  std::invalid_argument   thrown by getNode() [should
         *                                      never happen]
         */
        xmlpp::Node *
        selectKeyboardShortcutNode(int  containerNo,
                                   int  shortcutNo)
                                                throw (std::invalid_argument);
        
        /**
         *  Return the first node matching an XPath string.
         *
         *  If there is no matching node, it returns a 0 pointer.
         *
         *  @param  xPath   the XPath of the node (from the root node)
         *  @return a pointer to the node found, or 0
         *  @exception  std::invalid_argument   if the XPath is not well formed
         */
        xmlpp::Node *
        getNode(const Glib::ustring &   xPath)
                                                 throw (std::invalid_argument);
        
        
    public:
        /**
         *  Constructor with XML element parameter.
         *
         *  @param optionsElement   the XML element containing the options
         *  @param configFileName   the name (with path) of the configuration
         *                              file used by writeToFile()
         *  @see writeToFile()
         */
        OptionsContainer(const xmlpp::Element &         optionsElement,
                         Ptr<const Glib::ustring>::Ref  configFileName)
                                                                    throw ();
        
        /**
         *  Report if the object has been touched.
         *
         *  It returns true if there has been any calls to setOptionItem()
         *  since its construction or the last call to writeToFile().
         *
         *  @return whether the options have been touched
         */
        bool
        isTouched(void)                                             throw ()
        {
            return touched;
        }

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

        /**
         *  Set a keyboard shortcut type option.
         *
         *  @param      containerNo     which container to modify
         *  @param      shortcutNo      which shortcut to modify within this
         *                                  container
         *  @param      value           the name of the new shortcut key
         *  @exception  std::invalid_argument   if the shortcut is not found
         */
        void
        setKeyboardShortcutItem(int                             containerNo,
                                int                             shortcutNo,
                                Ptr<const Glib::ustring>::Ref   value)
                                                 throw (std::invalid_argument);
        
        /**
         *  Save the options to a file.
         *
         *  This writes the options in XML format to the file specified in the
         *  constructor.  The directory must already exist (it's OK if the file
         *  does not), otherwise nothing is written.
         */
        void
        writeToFile(void)                                           throw ();
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */


} // namespace Core
} // namespace LiveSupport

#endif // LiveSupport_Core_OptionsContainer_h

