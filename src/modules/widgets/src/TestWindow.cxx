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
 
 
    Author   : $Author$
    Version  : $Revision$
    Location : $URL$

------------------------------------------------------------------------------*/

/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#include <iostream>
#include <gtkmm/main.h>

#include "LiveSupport/Widgets/WidgetFactory.h"
#include "LiveSupport/Widgets/Colors.h"
#include "TestWindow.h"


using namespace LiveSupport::Widgets;

/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/**
 *  The name of the configuration file for the resource bundle.
 */
static const std::string bundleConfigFileName = "etc/resourceBundle.xml";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Constructor.
 *----------------------------------------------------------------------------*/
TestWindow :: TestWindow (void)
                                                                throw ()
          : WhiteWindow("test window",
                        Colors::White,
                        WidgetFactory::getInstance()->getWhiteWindowCorners())
{
    Ptr<WidgetFactory>::Ref  widgetFactory = WidgetFactory::getInstance();

    // init the imageButtons
    hugeImageButton = Gtk::manage(
                    widgetFactory->createButton(WidgetFactory::hugePlayButton));
    cuePlayImageButton = Gtk::manage(
                    widgetFactory->createButton(WidgetFactory::cuePlayButton));
    cuePlayImageButton->signal_clicked().connect(sigc::mem_fun(*this,
                                            &TestWindow::onPlayButtonClicked));
    cueStopImageButton = Gtk::manage(
                    widgetFactory->createButton(WidgetFactory::cueStopButton));
    cueStopImageButton->signal_clicked().connect(sigc::mem_fun(*this,
                                            &TestWindow::onStopButtonClicked));

    // create a button
    button = Gtk::manage(widgetFactory->createButton("Hello, World!"));
    button->signal_clicked().connect(sigc::mem_fun(*this,
                                                &TestWindow::onButtonClicked));

    // create a combo box
    comboBoxText = Gtk::manage(widgetFactory->createComboBoxText());
    comboBoxText->append_text("item1");
    comboBoxText->append_text("long item2");
    comboBoxText->append_text("very very very long item3");
    comboBoxText->set_active_text("item2");

    // create a text entry, ant put it inside a blue bin
    entryBin = Gtk::manage(widgetFactory->createEntryBin());
    entry    = entryBin->getEntry();

    // create a notebook
    notebook = Gtk::manage(new Notebook());
    notebook->appendPage(*button, "first page");
    notebook->appendPage(*comboBoxText, "second page");
    notebook->appendPage(*entryBin, "third page");

    // create a blue container
    blueBin = Gtk::manage(widgetFactory->createBlueBin());

    // create and set up the layout
    layout = Gtk::manage(new Gtk::Table());
    layout->attach(*hugeImageButton,    0, 1, 0, 1);
    layout->attach(*cuePlayImageButton, 1, 2, 0, 1);
    layout->attach(*notebook,           0, 2, 1, 2);
    blueBin->add(*layout);
    add(*blueBin);
    show_all();
    layout->attach(*cueStopImageButton, 1, 2, 0, 1);
    
    Ptr<ResourceBundle>::Ref    resourceBundle;
    try {
        Ptr<xmlpp::DomParser>::Ref  parser(
                              new xmlpp::DomParser(bundleConfigFileName, true));
        const xmlpp::Document * document = parser->get_document();
        const xmlpp::Element  * root     = document->get_root_node();

        resourceBundle = LocalizedObject::getBundle(*root);

    } catch (std::invalid_argument &e) {
        std::cerr << "semantic error in bundle configuration file:\n"
                  << e.what() << std::endl;
        exit(1);
    } catch (std::exception &e) {
        std::cerr << "XML error in bundle configuration file:\n"
                  << e.what() << std::endl;
        exit(1);
    }

    Ptr<Glib::ustring>::Ref     confirmationMessage(new Glib::ustring(
                                                            "Are you sure?" ));
    dialogWindow.reset(new DialogWindow(confirmationMessage,
                                        DialogWindow::noButton |
                                        DialogWindow::yesButton,
                                        resourceBundle ));
}


/*------------------------------------------------------------------------------
 *  Destructor.
 *----------------------------------------------------------------------------*/
TestWindow :: ~TestWindow (void)                                throw ()
{
}


/*------------------------------------------------------------------------------
 *  Event handler for the button being clicked.
 *----------------------------------------------------------------------------*/
void
TestWindow :: onButtonClicked(void)                                 throw ()
{
    Ptr<WidgetFactory>::Ref     wf = WidgetFactory::getInstance();
    Ptr<Glib::ustring>::Ref     message(new Glib::ustring("Hello, World!"));

    WhiteWindow   * helloWindow = wf->createMessageWindow(message);
    Gtk::Main::run(*helloWindow);
    delete helloWindow;
}


/*------------------------------------------------------------------------------
 *  Change the image from "play" to "stop" on the button when clicked.
 *----------------------------------------------------------------------------*/
void
TestWindow :: onPlayButtonClicked(void)                         throw ()
{
    cuePlayImageButton->hide();
    cueStopImageButton->show();
}


/*------------------------------------------------------------------------------
 *  Change the image from "stop" to "play" on the button when clicked.
 *----------------------------------------------------------------------------*/
void
TestWindow :: onStopButtonClicked(void)                         throw ()
{
    cueStopImageButton->hide();
    cuePlayImageButton->show();
}


/*------------------------------------------------------------------------------
 *  Override the close button functionality.
 *----------------------------------------------------------------------------*/
void
TestWindow :: onCloseButtonClicked(void)                        throw ()
{
    DialogWindow::ButtonType    result = dialogWindow->run();
    switch (result) {
        case DialogWindow::noButton:
                    return;

        case DialogWindow::yesButton:
                    hide();
                    break;

        default:    std::cerr << "This can never happen." << std::endl;
    }
}
    
