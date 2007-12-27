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
#include "LiveSupport/Widgets/WidgetConstants.h"


namespace LiveSupport {
namespace Widgets {

using namespace LiveSupport::Core;

/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  A factory to provide access to some Campcaster Widgets.
 *  As of Aug 2007, it is only used to retrieve the audio clip and playlist
 *  icon images.  It may be extended later, or it may be removed.
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
class WidgetFactory : public Configurable
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
         *  Return a smart pointer to a Gdk::Pixbuf holding a named image.
         *
         *  @return the image.
         */
        Glib::RefPtr<Gdk::Pixbuf>
        getPixbuf(WidgetConstants::ImageType   imageName)           throw ();
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */


} // namespace Widgets
} // namespace LiveSupport

#endif // LiveSupport_Widgets_WidgetFactory_h

