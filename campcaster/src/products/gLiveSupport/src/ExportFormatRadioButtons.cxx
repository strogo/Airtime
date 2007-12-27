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

#include "ExportFormatRadioButtons.h"


using namespace LiveSupport::Core;
using namespace LiveSupport::StorageClient;
using namespace LiveSupport::GLiveSupport;

/* ===================================================  local data structures */


/* ================================================  local constants & macros */


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Constructor.
 *----------------------------------------------------------------------------*/
ExportFormatRadioButtons :: ExportFormatRadioButtons(GuiObject *    parent)
                                                                    throw ()
          : GuiComponent(parent)
{
    glade->get_widget("internalFormatRadioButton1", internalFormatRadioButton);
    glade->get_widget("smilFormatRadioButton1", smilFormatRadioButton);
    internalFormatRadioButton->set_label(*getResourceUstring(
                                                    "internalFormatName"));
    smilFormatRadioButton->set_label(*getResourceUstring(
                                                    "smilFormatName"));
}


/*------------------------------------------------------------------------------
 *  Return the format which is currently selected.
 *----------------------------------------------------------------------------*/
StorageClientInterface::ExportFormatType
ExportFormatRadioButtons :: getFormat(void)                         throw ()
{
    if (internalFormatRadioButton->get_active()) {
        return StorageClientInterface::internalFormat;
    
    } else if (smilFormatRadioButton->get_active()) {
        return StorageClientInterface::smilFormat;
    
    } else {
        std::cerr << "impossible value in ExportFormatRadioButtons::getFormat()"
                  << std::endl;
        std::exit(1);
    }
}

