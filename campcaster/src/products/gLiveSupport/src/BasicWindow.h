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
#ifndef BasicWindow_h
#define BasicWindow_h

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#include <unicode/resbund.h>
#include <gtkmm.h>
#include <libglademm.h>

#include "LiveSupport/Core/LocalizedObject.h"
#include "GLiveSupport.h"

namespace LiveSupport {
namespace GLiveSupport {

using namespace LiveSupport::Core;

/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  The common ancestor of all openable and closable windows in the GUI.
 *
 *  @author $Author$
 *  @version $Revision$
 */
class BasicWindow : public LocalizedObject
{
    protected:
        /**
         *  The Glade object, containing the visual design.
         */
        Glib::RefPtr<Gnome::Glade::Xml>     glade;

        /**
         *  The GLiveSupport object, holding the state of the application.
         */
        Ptr<GLiveSupport>::Ref              gLiveSupport;

        /**
         *  The window itself.
         */
        Gtk::Window *                       mainWindow;

    public:
        /**
         *  Constructor.
         *
         *  @param  gLiveSupport    the GLiveSupport application object.
         *  @param  bundle          the resource bundle holding the localized
         *                          resources for this window.
         */
        BasicWindow(Ptr<GLiveSupport>::Ref        gLiveSupport,
                    Ptr<ResourceBundle>::Ref      bundle)
                                                                    throw ();

        /**
         *  Virtual destructor.
         */
        virtual
        ~BasicWindow(void)                                          throw ()
        {
        }

        /**
         *  Restore the window position and show the window.
         */
        virtual void
        show(void)                                                  throw ();

        /**
         *  Save the window position and hide the window.
         */
        virtual void
        hide(void)                                                  throw ();

        /**
         *  Set the title of the window.
         *
         *  Adds the application's title to the title of the window shown
         *  on the task bar.
         *
         *  @param  title   the title of the window.
         */
        virtual void
        setTitle(const Glib::ustring &     title)                   throw ();

        /**
         *  Wrapper for Gtk::Window::is_visible().
         */
        virtual bool
        is_visible(void)                                            throw ()
        {
            return mainWindow->is_visible();
        }

        /**
         *  Wrapper for Gtk::Window::present().
         */
        virtual void
        present(void)                                               throw ()
        {
            mainWindow->present();
        }

        /**
         *  Wrapper for Gtk::Main::run(Gtk::Window &).
         */
        virtual void
        run(void)                                                   throw ()
        {
            Gtk::Main::run(*mainWindow);
        }

        /**
         *  Wrapper for Gtk::Window::set_transient_for().
         */
        virtual void
        set_transient_for(Gtk::Window &     parent)                 throw ()
        {
            mainWindow->set_transient_for(parent);
        }

};

/* ================================================= external data structures */


/* ====================================================== function prototypes */


} // namespace GLiveSupport
} // namespace LiveSupport

#endif // BasicWindow_h

