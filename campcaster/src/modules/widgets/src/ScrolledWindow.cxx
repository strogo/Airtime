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

#include <iostream>
#include <gtkmm/viewport.h>

#include "LiveSupport/Widgets/WidgetFactory.h"
#include "LiveSupport/Widgets/ScrolledWindow.h"


using namespace LiveSupport::Core;
using namespace LiveSupport::Widgets;

/* ===================================================  local data structures */


/* ================================================  local constants & macros */


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Constructor.
 *----------------------------------------------------------------------------*/
ScrolledWindow :: ScrolledWindow(void)                              throw ()
    : Gtk::ScrolledWindow(),
      useShadowType(false)
{
}


/*------------------------------------------------------------------------------
 *  Destructor.
 *----------------------------------------------------------------------------*/
ScrolledWindow :: ~ScrolledWindow(void)                             throw ()
{
}


/*------------------------------------------------------------------------------
 *  Handle the realize event.
 *----------------------------------------------------------------------------*/
void
ScrolledWindow :: on_realize()                                      throw ()
{
    Gdk::Color      bgColor = Colors::getColor(Colors::WindowBackground);
    Widget *        child = get_child();    
    child->modify_bg(Gtk::STATE_NORMAL, bgColor);
    
    if (useShadowType) {
        Gtk::Viewport *     viewport = dynamic_cast<Gtk::Viewport*>(child);
        if (viewport) {
            viewport->set_shadow_type(shadowType);
        }
    }
    
    Gtk::ScrolledWindow::on_realize();
}

