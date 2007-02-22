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
#ifndef LiveSupport_Widgets_WidgetFactory_h
#define LiveSupport_Widgets_WidgetFactory_h

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#include <stdexcept>
#include <gtkmm/image.h>

#include "LiveSupport/Core/Configurable.h"
#include "LiveSupport/Core/MetadataTypeContainer.h"

#include "LiveSupport/Widgets/WidgetConstants.h"
#include "LiveSupport/Widgets/CornerImages.h"
#include "LiveSupport/Widgets/ButtonImages.h"
#include "LiveSupport/Widgets/Button.h"
#include "LiveSupport/Widgets/ImageButton.h"
#include "LiveSupport/Widgets/ComboBoxText.h"
#include "LiveSupport/Widgets/MetadataComboBoxText.h"
#include "LiveSupport/Widgets/OperatorComboBoxText.h"
#include "LiveSupport/Widgets/BlueBin.h"
#include "LiveSupport/Widgets/EntryBin.h"
#include "LiveSupport/Widgets/DialogWindow.h"
#include "LiveSupport/Widgets/ZebraTreeView.h"
#include "LiveSupport/Widgets/DateTimeChooserWindow.h"


namespace LiveSupport {
namespace Widgets {

using namespace LiveSupport::Core;

/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  A factory to provide access to LiveSupport Widgets.
 *  
 *  The singleton instance of this class has to be configured with an XML
 *  element, which looks like the following:
 *
 *  <pre><code>
 *  &lt;widgetFactory&gt;   path = "path/to/widget/images/"
 *  &lt;/&gt;
 *  </code></pre>
 *
 *  The DTD for the above XML structure is:
 *
 *  <pre><code>
 *  <!ELEMENT widgetFactory   EMPTY >
 *  <!ATTLIST widgetFactory   path        CDATA   #REQUIRED >
 *  </code></pre>
 *
 *  @author  $Author$
 *  @version $Revision$
 */
class WidgetFactory :
                        virtual public Configurable
{
    private:
        /**
         *  The name of the configuration XML elmenent used by this object.
         */
        static const std::string                configElementNameStr;

        /**
         *  The singleton instance of this object.
         */
        static Ptr<WidgetFactory>::Ref          singleton;

        /**
         *  The path to load the images from for the widgets.
         */
        std::string                             path;

        /**
         *  The images for the standard button.
         */
        Ptr<ButtonImages>::Ref          buttonImages;

        /**
         *  The images for the tab button.
         */
        Ptr<ButtonImages>::Ref          tabButtonImages;

        /**
         *  The corner images for the blue bin.
         */
        Ptr<CornerImages>::Ref          blueBinImages;

        /**
         *  The corner images for the dark blue bin.
         */
        Ptr<CornerImages>::Ref          darkBlueBinImages;

        /**
         *  The corner images for the entry bin.
         */
        Ptr<CornerImages>::Ref          entryBinImages;

        /**
         *  The corner images for the white window.
         */
        Ptr<CornerImages>::Ref          whiteWindowImages;

        /**
         *  The combo box left image.
         */
        Glib::RefPtr<Gdk::Pixbuf>       comboBoxLeftImage;

        /**
         *  The combo box center image.
         */
        Glib::RefPtr<Gdk::Pixbuf>       comboBoxCenterImage;

        /**
         *  The combo box right image.
         */
        Glib::RefPtr<Gdk::Pixbuf>       comboBoxRightImage;

        /**
         *  A container holding the miscallenous image pixbuf references.
         */
        std::map<WidgetConstants::ImageType, Glib::RefPtr<Gdk::Pixbuf> >
                                        imageTypePixbufs;

        /**
         *  The default constructor.
         */
        WidgetFactory(void)                     throw ()
        {
        }

        /**
         *  Load an image relative the path, and signal error if not found.
         *
         *  @param imageName the name of the image, relative to path
         *  @return the loaded image
         *  @exception std::invalid_argument if the image was not found
         */
        Glib::RefPtr<Gdk::Pixbuf>
        loadImage(const std::string     imageName)
                                                throw (std::invalid_argument);

        /**
         *  Convert an integer to a string.
         *
         *  @param  number      the number to be converted.
         *  @param  minLength   the minimum length of the result; smaller
         *                      numbers get padded with zeroes (optional).
         *  @return the string value of the number (in base 10).
         */
        static Glib::ustring
        itoa(int    number,
             int    minLength = 0)                                  throw ();


    public:
        /**
         *  A virtual destructor, as this class has virtual functions.
         */
        virtual
        ~WidgetFactory(void)                                        throw ()
        {
        }

        /**
         *  Return the name of the XML element this object expects
         *  to be sent to a call to configure().
         *  
         *  @return the name of the expected XML configuration element.
         */
        static const std::string
        getConfigElementName(void)                                  throw ()
        {
            return configElementNameStr;
        }

        /**
         *  Returns the singleton instance of this object.
         *
         *  @return the singleton instance of this object.
         */
        static Ptr<WidgetFactory>::Ref
        getInstance()                                               throw ();

        /**
         *  Configure the object based on the XML element supplied.
         *
         *  @param element the XML element to configure the object from.
         *  @exception std::invalid_argument if the supplied XML element
         *             contains bad configuraiton information
         *  @exception std::logic_error if the object has already
         *             been configured, and can not be reconfigured.
         */
        virtual void
        configure(const xmlpp::Element    & element)
                                                throw (std::invalid_argument,
                                                       std::logic_error);

        /**
         *  Create and return a generic button.
         *  It is the reponsibility of the caller to dispose of the created
         *  object properly.
         *
         *  @param label the label shown inside the button.
         *  @param type the type of the button to create
         *  @return a button with the specified label.
         */
        Button *
        createButton(
            Gtk::Widget &                   label,
            WidgetConstants::ButtonType     type = WidgetConstants::pushButton)
                                                                    throw ();

        /**
         *  Create and return a button with a text label.
         *  It is the reponsibility of the caller to dispose of the created
         *  object properly.
         *
         *  @param label the label shown inside the button.
         *  @param type the type of the button to create
         *  @return a button with the specified label.
         */
        Button *
        createButton(
            const Glib::ustring &           label,
            WidgetConstants::ButtonType     type = WidgetConstants::pushButton)
                                                                    throw ();

        /**
         *  Create a stock button.
         *  It is the reponsibility of the caller to dispose of the created
         *  object properly.
         *
         *  @param type the type of the button.
         *  @return a button of the requested type, or 0
         */
        ImageButton *
        createButton(WidgetConstants::ImageButtonType    type)      throw ();

        /**
         *  Create a combo box that holds text entries.
         *  It is the reponsibility of the caller to dispose of the created
         *  object properly.
         *
         *  @return a combo box, that holds text entries.
         */
        ComboBoxText *
        createComboBoxText(void)                                    throw ();
        
        /**
         *  Create a metadata combo box that holds metadata types.
         *  It is the reponsibility of the caller to dispose of the created
         *  object properly.
         *
         *  @param  metadataTypes   a container of metadata types to display.
         *  @return a combo box, that holds metadata types.
         */
        MetadataComboBoxText *
        createMetadataComboBoxText(
                        Ptr<MetadataTypeContainer>::Ref  metadataTypes)
                                                                    throw ();

        /**
         *  Create a combo box that holds comparison operators.
         *  It is the reponsibility of the caller to dispose of the created
         *  object properly.
         *
         *  @param bundle   a localization bundle.
         *  @return a combo box, that holds comparison operators.
         */
        OperatorComboBoxText *
        createOperatorComboBoxText(
                        Ptr<ResourceBundle>::Ref    bundle)
                                                                    throw ();

        /**
         *  Create a combo box that holds a range of numeric entries.
         *  It is the reponsibility of the caller to dispose of the created
         *  object properly.
         *
         *  @param  lowerLimit  the lowest entry in the combo box.
         *  @param  upperLimit  the highest entry in the combo box.
         *  @param  minLength   the minimum length of the entries; smaller
         *                      numbers get padded with zeroes (optional).
         *  @return a combo box, that holds numeric entries.
         */
        ComboBoxText *
        createNumericComboBoxText(int   lowerLimit,
                                  int   upperLimit,
                                  int   minLength = 0)              throw ();
        
        /**
         *  Create and return a blue singular container.
         *  It is the reponsibility of the caller to dispose of the created
         *  object properly.
         *
         *  @return a blue singular container.
         */
        BlueBin *
        createBlueBin(void)                                         throw ();

        /**
         *  Create and return a dark blue singular container.
         *  It is the reponsibility of the caller to dispose of the created
         *  object properly.
         *
         *  @return a dark blue singular container.
         */
        BlueBin *
        createDarkBlueBin(void)                                     throw ();

        /**
         *  Create and return a singular container holding a text entry.
         *  It is the reponsibility of the caller to dispose of the created
         *  object properly.
         *
         *  @return a singular container holding a text entry.
         */
        EntryBin *
        createEntryBin(void)                                        throw ();

        /**
         *  Return the images for the white window.
         *
         *  @return the corner images for the white window.
         */
        Ptr<CornerImages>::Ref
        getWhiteWindowCorners(void)                                 throw ()
        {
            return whiteWindowImages;
        }

        /**
         *  Return a smart pointer to a Gdk::Pixbuf holding a named image.
         *
         *  @return the image.
         */
        Glib::RefPtr<Gdk::Pixbuf>
        getPixbuf(WidgetConstants::ImageType   imageName)           throw ();

        /**
         *  Create and return a container holding an image.
         *  It is the reponsibility of the caller to dispose of the created
         *  object properly.
         *
         *  @return the container holding the requested image.
         */
        Gtk::Image *
        createImage(WidgetConstants::ImageType   imageName)         throw ();

        /**
         *  Create and return a ZebraTreeView instance.
         *  It is the reponsibility of the caller to dispose of the created
         *  object properly.
         *
         *  @return the ZebraTreeView object.
         */
        ZebraTreeView *
        createTreeView(Glib::RefPtr<Gtk::TreeModel> treeModel)
                                                                    throw ();

        /**
         *  Create a DialogWindow.
         *  Good for displaying error messages, confirmation messages, etc.
         *  It is the reponsibility of the caller to dispose of the created
         *  object properly.
         *
         *  @param  message the message to include in the window.
         *  @param  bundle  localization for the button(s).
         *  @param  buttons a list of buttons; default is a single OK button.
         *  @return the DialogWindow object.
         */
        DialogWindow *
        createDialogWindow(Ptr<const Glib::ustring>::Ref    message,
                           Ptr<ResourceBundle>::Ref         bundle,
                           int              buttons = DialogWindow::okButton)
                                                                    throw ();

        /**
         *  Create a DateTimeChooserWindow.
         *  It lets the user select a date/time.
         *  It is the reponsibility of the caller to dispose of the created
         *  object properly.
         *
         *  @param  bundle  localization for the button(s).
         *  @return the DateTimeChooserWindow object.
         */
        DateTimeChooserWindow *
        createDateTimeChooserWindow(Ptr<ResourceBundle>::Ref    bundle)
                                                                    throw ();
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */


} // namespace Widgets
} // namespace LiveSupport

#endif // LiveSupport_Widgets_WidgetFactory_h

