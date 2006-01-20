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

#include <gtkmm.h>

#include "LiveSupport/Widgets/WidgetFactory.h"
#include "LiveSupport/Widgets/Button.h"
#include "LiveSupport/Widgets/ScrolledNotebook.h"
#include "LiveSupport/Widgets/EntryBin.h"
#include "OptionsWindow.h"


using namespace LiveSupport::Widgets;
using namespace LiveSupport::GLiveSupport;

/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/**
 *  The name of the window, used by the keyboard shortcuts (or by the .gtkrc).
 */
static const Glib::ustring  windowName = "optionsWindow";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Constructor.
 *----------------------------------------------------------------------------*/
OptionsWindow :: OptionsWindow (Ptr<GLiveSupport>::Ref    gLiveSupport,
                                Ptr<ResourceBundle>::Ref  bundle)
                                                                    throw ()
          : WhiteWindow("",
                        Colors::White,
                        WidgetFactory::getInstance()->getWhiteWindowCorners()),
            LocalizedObject(bundle),
            gLiveSupport(gLiveSupport)
{
    isChanged = false;

    Ptr<WidgetFactory>::Ref     wf = WidgetFactory::getInstance();
    
    try {
        set_title(*getResourceUstring("windowTitle"));
        
    } catch (std::invalid_argument &e) {
        // TODO: signal error
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }

    // build up the notepad for the various sections
    mainNotebook = Gtk::manage(new ScrolledNotebook);
    Gtk::Box *      soundSectionBox = constructSoundSection();
    Gtk::Box *      aboutSectionBox = constructAboutSection();

    try {
        mainNotebook->appendPage(*soundSectionBox,
                                 *getResourceUstring("soundSectionLabel"));
        mainNotebook->appendPage(*aboutSectionBox,
                                 *getResourceUstring("aboutSectionLabel"));

    } catch (std::invalid_argument &e) {
        // TODO: signal error
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }

    // build up the button box
    buttonBox = Gtk::manage(new Gtk::HButtonBox);
    buttonBox->set_layout(Gtk::BUTTONBOX_END);
    buttonBox->set_spacing(5);

    try {
        cancelButton = Gtk::manage(wf->createButton(
                                *getResourceUstring("cancelButtonLabel") ));
        applyButton  = Gtk::manage(wf->createButton(
                                *getResourceUstring("applyButtonLabel") ));
        okButton     = Gtk::manage(wf->createButton(
                                *getResourceUstring("okButtonLabel") ));
        
    } catch (std::invalid_argument &e) {
        // TODO: signal error
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }

    buttonBox->pack_start(*cancelButton);
    buttonBox->pack_start(*applyButton);
    buttonBox->pack_start(*okButton);

    // set up the main window
    Gtk::Box *      layout = Gtk::manage(new Gtk::VBox);
    layout->pack_start(*mainNotebook, Gtk::PACK_EXPAND_WIDGET, 5);
    layout->pack_start(*buttonBox,  Gtk::PACK_SHRINK, 5);
    add(*layout);

    // bind events
    cancelButton->signal_clicked().connect(sigc::mem_fun(*this,
                                &OptionsWindow::onCancelButtonClicked));
    applyButton->signal_clicked().connect(sigc::mem_fun(*this,
                                &OptionsWindow::onApplyButtonClicked));
    okButton->signal_clicked().connect(sigc::mem_fun(*this,
                                &OptionsWindow::onOkButtonClicked));

    // show everything
    set_name(windowName);
    set_default_size(350, 300);
    set_modal(false);
    property_window_position().set_value(Gtk::WIN_POS_NONE);
    
    show_all();
}


/*------------------------------------------------------------------------------
 *  Event handler for the Cancel button.
 *----------------------------------------------------------------------------*/
void
OptionsWindow :: onCancelButtonClicked(void)                        throw ()
{
    onCloseButtonClicked(false);
}


/*------------------------------------------------------------------------------
 *  Event handler for the Apply button.
 *----------------------------------------------------------------------------*/
void
OptionsWindow :: onApplyButtonClicked(void)                         throw ()
{
    Ptr<OptionsContainer>::Ref
            optionsContainer  = gLiveSupport->getOptionsContainer();

    // check for changes in the Sound tab
    Ptr<const Glib::ustring>::Ref
            oldCueDevice      = optionsContainer->getOptionItem(
                                    OptionsContainer::cuePlayerDeviceName );
    Ptr<const Glib::ustring>::Ref
            newCueDevice(new Glib::ustring(cuePlayerEntry->get_text()));
    
    if (*oldCueDevice != *newCueDevice) {
        optionsContainer->setOptionItem(
                                    OptionsContainer::cuePlayerDeviceName,
                                    newCueDevice );
    }
    
    Ptr<const Glib::ustring>::Ref
            oldOutputDevice   = optionsContainer->getOptionItem(
                                    OptionsContainer::outputPlayerDeviceName );
    Ptr<const Glib::ustring>::Ref
            newOutputDevice(new Glib::ustring(outputPlayerEntry->get_text()));
    
    if (*oldOutputDevice != *newOutputDevice) {
        optionsContainer->setOptionItem(
                                    OptionsContainer::outputPlayerDeviceName,
                                    newOutputDevice );
    }
}


/*------------------------------------------------------------------------------
 *  Event handler for the OK button.
 *----------------------------------------------------------------------------*/
void
OptionsWindow :: onOkButtonClicked(void)                            throw ()
{
    onApplyButtonClicked();
    onCloseButtonClicked(false);
}


/*------------------------------------------------------------------------------
 *  Event handler for the Close button.
 *----------------------------------------------------------------------------*/
void
OptionsWindow :: onCloseButtonClicked(bool     needConfirm)         throw ()
{
    if (needConfirm) {
        //TODO: add confirmation dialog
    }
    gLiveSupport->putWindowPosition(shared_from_this());
    hide();
}


/*------------------------------------------------------------------------------
 *  Construct the "About" section.
 *----------------------------------------------------------------------------*/
Gtk::VBox*
OptionsWindow :: constructAboutSection(void)                        throw ()
{
    Glib::ustring   aboutLabelContents;
    aboutLabelContents.append(PACKAGE_NAME);
    aboutLabelContents.append(" ");
    aboutLabelContents.append(PACKAGE_VERSION);
    aboutLabelContents.append("\n\n");
    try {
        aboutLabelContents.append(*formatMessage("reportBugsToText",
                                                 PACKAGE_BUGREPORT ));
    } catch (std::invalid_argument &e) {
        // TODO: signal error
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }
    Gtk::Label *    aboutLabel = Gtk::manage(
                                    new Gtk::Label(aboutLabelContents) );

    // make a new box and pack the components into it
    Gtk::VBox *     section = Gtk::manage(new Gtk::VBox);
    section->pack_start(*aboutLabel, Gtk::PACK_SHRINK, 5);
    
    return section;
}


/*------------------------------------------------------------------------------
 *  Construct the "Sound" section.
 *----------------------------------------------------------------------------*/
Gtk::VBox*
OptionsWindow :: constructSoundSection(void)                        throw ()
{
    Ptr<OptionsContainer>::Ref  optionsContainer
                                   = gLiveSupport->getOptionsContainer();
    Ptr<WidgetFactory>::Ref     wf = WidgetFactory::getInstance();
    
    Gtk::Table *    audioDeviceTable = Gtk::manage(new Gtk::Table);
    audioDeviceTable->set_row_spacings(10);
    audioDeviceTable->set_col_spacings(5);
    
    // display the settings for the cue player device
    Glib::ustring   cuePlayerLabelContents;
    try {
        cuePlayerLabelContents.append(*getResourceUstring("cueDeviceLabel"));
        
    } catch (std::invalid_argument &e) {
        // TODO: signal error
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }
    Gtk::Label *    cuePlayerLabel = Gtk::manage(
                                    new Gtk::Label(cuePlayerLabelContents) );
    audioDeviceTable->attach(*cuePlayerLabel, 0, 1, 0, 1);
    
    cuePlayerEntry = Gtk::manage(wf->createEntryBin());
    cuePlayerEntry->set_text(*optionsContainer->getOptionItem(
                                    OptionsContainer::cuePlayerDeviceName ));
    audioDeviceTable->attach(*cuePlayerEntry, 1, 2, 0, 1);
    
    // display the settings for the output player device
    Glib::ustring   outputPlayerLabelContents;
    try {
        outputPlayerLabelContents.append(*getResourceUstring(
                                                        "outputDeviceLabel"));
    } catch (std::invalid_argument &e) {
        // TODO: signal error
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }
    Gtk::Label *    outputPlayerLabel = Gtk::manage(
                                    new Gtk::Label(outputPlayerLabelContents) );
    audioDeviceTable->attach(*outputPlayerLabel, 0, 1, 1, 2);
    
    outputPlayerEntry = Gtk::manage(wf->createEntryBin());
    outputPlayerEntry->set_text(*optionsContainer->getOptionItem(
                                    OptionsContainer::outputPlayerDeviceName ));
    audioDeviceTable->attach(*outputPlayerEntry, 1, 2, 1, 2);

    // make a new box and pack the components into it
    Gtk::VBox *     section = Gtk::manage(new Gtk::VBox);
    section->pack_start(*audioDeviceTable,   Gtk::PACK_SHRINK, 5);
    
    return section;
}

