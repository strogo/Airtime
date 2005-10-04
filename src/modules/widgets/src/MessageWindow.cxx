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

#include "LiveSupport/Widgets/WidgetFactory.h"
#include "LiveSupport/Widgets/Colors.h"
#include "MessageWindow.h"


using namespace LiveSupport::Widgets;

/* ===================================================  local data structures */


/* ================================================  local constants & macros */


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Constructor.
 *----------------------------------------------------------------------------*/
MessageWindow :: MessageWindow (Ptr<Glib::ustring>::Ref message)
                                                                    throw ()
          : WhiteWindow("",
                        Colors::White,
                        WidgetFactory::getInstance()->getWhiteWindowCorners(),
                        WhiteWindow::isModal)
{
    Ptr<WidgetFactory>::Ref  widgetFactory = WidgetFactory::getInstance();

    messageLabel = Gtk::manage(new Gtk::Label(*message));
    Gtk::Box *          messageLabelBox(new Gtk::HBox);
    messageLabelBox->pack_start(*messageLabel, false, false, 5);

    // init the okButton
    // TODO: localize the OK text on the button
    okButton = Gtk::manage(widgetFactory->createButton("OK"));
    okButton->signal_clicked().connect(sigc::mem_fun(*this,
                                            &MessageWindow::onOkButtonClicked));
    Gtk::ButtonBox *    okButtonBox = Gtk::manage(new Gtk::HButtonBox);
    okButtonBox->pack_start(*okButton, 5, 5);

    layout = Gtk::manage(new Gtk::VBox());

    layout->pack_start(*messageLabelBox, true, true, 5);
    layout->pack_start(*okButtonBox, false, false, 5);

    add(*layout);
    show_all();
}


/*------------------------------------------------------------------------------
 *  Destructor.
 *----------------------------------------------------------------------------*/
MessageWindow :: ~MessageWindow (void)                          throw ()
{
}


/*------------------------------------------------------------------------------
 *  Event handler for the OK button clicked
 *----------------------------------------------------------------------------*/
void
MessageWindow :: onOkButtonClicked(void)                        throw ()
{
    hide();
}

