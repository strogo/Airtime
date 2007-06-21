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
#include <stdexcept>

#include "LiveSupport/Widgets/WidgetFactory.h"
#include "LoginWindow.h"


using namespace Glib;

using namespace LiveSupport::Core;
using namespace LiveSupport::Widgets;
using namespace LiveSupport::GLiveSupport;

/* ===================================================  local data structures */


/* ================================================  local constants & macros */

namespace {

/*------------------------------------------------------------------------------
 *  The name of the glade file.
 *----------------------------------------------------------------------------*/
const Glib::ustring     gladeFileName = "LoginWindow.glade";

}

/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Constructor.
 *----------------------------------------------------------------------------*/
LoginWindow :: LoginWindow (Ptr<GLiveSupport>::Ref      gLiveSupport,
                            Ptr<ResourceBundle>::Ref    bundle,
                            const Glib::ustring &       gladeDir)
                                                                    throw ()
          : LocalizedObject(bundle),
            gLiveSupport(gLiveSupport),
            loggedIn(false)
{
    glade = Gnome::Glade::Xml::create(gladeDir + gladeFileName);

    // localize everything
    glade->get_widget("loginWindow", loginWindow);
    loginWindow->set_title(*getResourceUstring("windowTitle"));

    Gtk::Label *    userNameLabel;
    Gtk::Label *    passwordLabel;
    Gtk::Label *    languageLabel;
    glade->get_widget("userNameLabel", userNameLabel);
    glade->get_widget("passwordLabel", passwordLabel);
    glade->get_widget("languageLabel", languageLabel);
    userNameLabel->set_text(*getResourceUstring("userNameLabel"));
    passwordLabel->set_text(*getResourceUstring("passwordLabel"));
    languageLabel->set_text(*getResourceUstring("languageLabel"));

    // fill up the language list with the list of available languages
    glade->get_widget_derived("languageEntry", languageEntry);

    Ptr<const GLiveSupport::LanguageMap>::Ref
                        languages = gLiveSupport->getSupportedLanguages();
    for (GLiveSupport::LanguageMap::const_iterator
            it = languages->begin(); it != languages->end(); ++it) {
        Glib::ustring   language = it->first;
        languageEntry->append_text(language);
    }
    languageEntry->set_active(0);

    // connect signal handlers
    glade->connect_clicked("okButton", sigc::mem_fun(*this,
                                        &LoginWindow::onOkButtonClicked));
    glade->connect_clicked("cancelButton", sigc::mem_fun(*this,
                                        &LoginWindow::onCancelButtonClicked));

    // get a reference for the text entry fields
    glade->get_widget("userNameEntry", userNameEntry);
    glade->get_widget("passwordEntry", passwordEntry);
    
    // clear the status bar
    glade->get_widget("statusBar", statusBar);
    statusBar->set_text("");
    
    // set the OK button as the default action
    // FIXME: does not work for some reason
    loginWindow->set_default_response(Gtk::RESPONSE_OK);
}


/*------------------------------------------------------------------------------
 *  Destructor.
 *----------------------------------------------------------------------------*/
LoginWindow :: ~LoginWindow (void)                        throw ()
{
}


/*------------------------------------------------------------------------------
 *  Event handler for the OK button getting clicked.
 *----------------------------------------------------------------------------*/
void
LoginWindow :: onOkButtonClicked (void)                             throw ()
{
    statusBar->set_text(*getResourceUstring("pleaseWaitMsg"));
    loginWindow->set_sensitive(false);
    gLiveSupport->runMainLoop();    // redraw the window
    
    userNameText.reset(new Glib::ustring(userNameEntry->get_text()));
    passwordText.reset(new Glib::ustring(passwordEntry->get_text()));
    
    Ptr<const GLiveSupport::LanguageMap>::Ref   languages;
    languages = gLiveSupport->getSupportedLanguages();

    GLiveSupport::LanguageMap::const_iterator
        langSel = languages->find(languageEntry->get_active_text());
    selectedLocale.reset(new std::string(langSel->second));
    
    loggedIn = gLiveSupport->login(*getLogin(), *getPassword());
    
    if (loggedIn) {
        if (selectedLocale->size() > 0) {
            gLiveSupport->changeLanguage(selectedLocale);
        } else {
            // TODO: get and set default locale for user
        }
        
        gLiveSupport->createScratchpadWindow();
    }
    
    loginWindow->hide();
}


/*------------------------------------------------------------------------------
 *  Event handler for the cancel button getting clicked.
 *----------------------------------------------------------------------------*/
void
LoginWindow :: onCancelButtonClicked (void)                  throw ()
{
    loginWindow->hide();
}


/*------------------------------------------------------------------------------
 *  Show the window, and return whether the login was successful.
 *----------------------------------------------------------------------------*/
bool
LoginWindow :: run(void)                                            throw ()
{
    Gtk::Main::run(*loginWindow);
    return loggedIn;
}


