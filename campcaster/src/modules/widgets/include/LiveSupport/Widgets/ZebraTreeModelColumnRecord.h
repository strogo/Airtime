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
    Location : $URL: svn://code.campware.org/campcaster/trunk/campcaster/src/modules/widgets/include/LiveSupport/Widgets/ZebraTreeModelColumnRecord.h $

------------------------------------------------------------------------------*/
#ifndef LiveSupport_Widgets_ZebraTreeModelColumnRecord_h
#define LiveSupport_Widgets_ZebraTreeModelColumnRecord_h

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#include <gtkmm/treemodelcolumn.h>

namespace LiveSupport {
namespace Widgets {

/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  A basic column record class for tree models with colorable rows.
 *
 *  @author  $Author: fgerlits $
 *  @version $Revision: 2329 $
 */
class ZebraTreeModelColumnRecord : public Gtk::TreeModelColumnRecord
{
    public:
        /**
         *  The column for the color of the row.
         */
        Gtk::TreeModelColumn<int>       rowNumberColumn;

        /**
         *  Constructor.
         */
        ZebraTreeModelColumnRecord(void)                        throw ()
        {
            add(rowNumberColumn);
        }
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */


} // namespace Widgets
} // namespace LiveSupport

#endif // LiveSupport_Widgets_ZebraTreeModelColumnRecord_h

