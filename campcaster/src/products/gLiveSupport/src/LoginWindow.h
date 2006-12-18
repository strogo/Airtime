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
#ifndef LoginWindow_h
#define LoginWindow_h

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#include <string>

#include <unicode/resbund.h>

#include <gtkmm/window.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/label.h>
#include <gtkmm/entry.h>
#include <gtkmm/table.h>
#include <gtkmm/combo.h>

#include "LiveSupport/Core/Ptr.h"
#include "LiveSupport/Core/LocalizedObject.h"
#include "LiveSupport/Widgets/EntryBin.h"
#include "LiveSupport/Widgets/ComboBoxText.h"
#include "GuiWindow.h"
#include "GLiveSupport.h"

namespace LiveSupport {
namespace GLiveSupport {

using namespace LiveSupport::Core;
using namespace LiveSupport::Widgets;

/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  A window, handling user login.
 *
 *  @author $Author$
 *  @version $Revision$
 */
class LoginWindow : public GuiWindow
{

    protected:
        /**
         *  The table, which provides the layout for the window.
         */
        Gtk::Table                * table;

        /**
         *  The login label in the window.
         */
        Gtk::Label            * loginLabel;

        /**
         *  The password label in the window.
         */
        Gtk::Label            * passwordLabel;

        /**
         *  The container for the login text entry area.
         */
        EntryBin              * loginEntryBin;

        /**
         *  The login text entry area.
         */
        Gtk::Entry            * loginEntry;

        /**
         *  The container for the password text entry area.
         */
        EntryBin              * passwordEntryBin;

        /**
         *  The password text entry area.
         */
        Gtk::Entry            * passwordEntry;

        /**
         *  The drop-down list to select the desired language.
         */
        ComboBoxText          * languageList;

        /**
         *  The horizontal box for the buttons.
         */
        Gtk::HButtonBox       * buttonBox;

        /**
         *  The OK button.
         */
        Button                * okButton;

        /**
         *  The Cancel button.
         */
        Button                * cancelButton;

        /**
         *  The status bar.
         */
        Gtk::Label            * statusBar;

        /**
         *  The login text, that was entered by the user.
         */
        Ptr<Glib::ustring>::Ref     loginText;

        /**
         *  The password text, that was entered by the user.
         */
        Ptr<Glib::ustring>::Ref     passwordText;

        /**
         *  The locale / language selected by the user.
         */
        Ptr<std::string>::Ref       selectedLocale;

        /**
         *  Flag to show that the user has successfully logged in.
         */
        bool                        loggedIn;

        /**
         *  Signal handler for the ok button clicked.
         */
        virtual void
        onOkButtonClicked(void)                             throw ();

        /**
         *  Signal handler for the cancel button clicked.
         */
        virtual void
        onCancelButtonClicked(void)                         throw ();

        /**
         *  Set the text of the status bar.
         */
        virtual void
        setStatusBarText(Ptr<const Glib::ustring>::Ref  text)
                                                            throw ();


    public:
        /**
         *  Constructor.
         *
         *  @param  gLiveSupport    the gLiveSupport object, containing
         *                          all the vital info.
         *  @param  bundle          the resource bundle holding the localized
         *                          resources for this window.
         *  @param windowOpenerButton   the button which was pressed to open
         *                              this window.
         */
        LoginWindow(Ptr<GLiveSupport>::Ref      gLiveSupport,
                    Ptr<ResourceBundle>::Ref    bundle,
                    Button *                    windowOpenerButton)
                                                            throw ();

        /**
         *  Virtual destructor.
         */
        virtual
        ~LoginWindow(void)                                  throw ();

        /**
         *  Get the login entered by the user.
         *
         *  @return the login entered by the user.
         */
        Ptr<const Glib::ustring>::Ref
        getLogin(void) const                                throw ()
        {
            return loginText;
        }

        /**
         *  Get the password entered by the user.
         *
         *  @return the password entered by the user.
         */
        Ptr<const Glib::ustring>::Ref
        getPassword(void) const                             throw ()
        {
            return passwordText;
        }

        /**
         *  Get the locale selected by the user.
         *
         *  @return the locale selected by the user. if this is an empty
         *          string, the user selected the default locale.
         */
        Ptr<const std::string>::Ref
        getSelectedLocale(void) const                       throw ()
        {
            return selectedLocale;
        }

        /**
         *  Show the window, and return whether the login was successful.
         *
         *  @return true if the login was successful.
         */
        bool
        run(void)                                           throw ();
};

/* ================================================= external data structures */


/* ====================================================== function prototypes */


} // namespace GLiveSupport
} // namespace LiveSupport

#endif // LoginWindow_h

