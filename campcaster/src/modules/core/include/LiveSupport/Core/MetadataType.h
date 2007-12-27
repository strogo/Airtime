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
#ifndef LiveSupport_Core_MetadataType_h
#define LiveSupport_Core_MetadataType_h

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#include <map>

#include "LiveSupport/Core/Ptr.h"
#include "LiveSupport/Core/Configurable.h"
#include "LiveSupport/Core/MetadataConstraint.h"


namespace LiveSupport {
namespace Core {


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

class MetadataTypeContainer;


/**
 *  A class for representing a metadata type.
 *
 *  This object has to be configured with an XML configuration element
 *  called metadataType. This may look like the following:
 *
 *  <pre><code>
 *  &lt;metadataType dcName          = "dc:creator"
 *                id3Tag          = "TPE2"
 *                localizationKey = "dc_creator"
 *                tab             = "main" &gt;
 *      &lt;constraint     type = "numericRange" &gt;
 *          &lt;value&gt;1&lt;/value&gt;
 *          &lt;value&gt;12&lt;/value&gt;
 *      &lt;/constraint&gt;
 *  &lt;/metadataType&gt;
 *  </code></pre>
 *
 *  The tab attribute (if present) must be one of "main", "music" or "voice"
 *  (all lowercase, case sensitive).
 *  This determines whether, at file upload, the metadata field appears in
 *  the Main, Music, or Voice tab.  If the attribute is omitted, the metadata
 *  field will appear in none of the tabs.
 *
 *  The optional constraint sub-element can give restrictions on the acceptable
 *  values for this type of metadata.  See the MetadataConstraint class for
 *  more information, including the DTD of the &lt;constraint&gt; element.
 *
 *  See the Studio config files for a list of all metadata types.
 *
 *  The DTD for the expected XML element looks like the following:
 *
 *  <pre><code>
 *  <!ELEMENT metadataType  (constraint?) >
 *  <!ATTLIST metadataType  dcName            NMTOKEN     #REQUIRED >
 *  <!ATTLIST metadataType  id3Tag            NMTOKEN     #IMPLIED  >
 *  <!ATTLIST metadataType  localizationKey   NMTOKEN     #REQUIRED >
 *  <!ATTLIST metadataType  tab               NMTOKEN     #IMPLIED  >
 *  </code></pre>
 *
 *
 *  @author  $Author$
 *  @version $Revision$
 *  @see MetadataTypeContainer
 */
class MetadataType : public Configurable
{
    friend class MetadataTypeContainer;
    
    public:
        /**
         *  An enumeration of all possible metadata categories.
         */
        typedef enum { noTab,
                       mainTab,
                       musicTab,
                       voiceTab }    TabType;


    private:
        /**
         *  The name of the configuration XML element used by MetadataType.
         */
        static const std::string    configElementNameStr;

        /**
         *  A reference to a metadata type container.
         */
        Ptr<MetadataTypeContainer>::Ref     container;

        /**
         *  The Dublic Core name of this metadata type.
         */
        Ptr<Glib::ustring>::Ref     dcName;

        /**
         *  The ID3v2 tag for this metadata type.
         */
        Ptr<Glib::ustring>::Ref     id3Tag;

        /**
         *  The localization key for this metadata type.
         */
        Ptr<Glib::ustring>::Ref     localizationKey;

        /**
         *  The localization key for this metadata type.
         */
        TabType                     tab;

        /**
         *  The constraint object, if any.
         */
        Ptr<MetadataConstraint>::Ref        constraint;


    protected:
        /**
         *  Default constructor.
         *
         *  @param container the container this metadata type is held in.
         */
        MetadataType(Ptr<MetadataTypeContainer>::Ref    container)
                                                                    throw ();

        /**
         *  Constructor.
         *
         *  @param container the container this metadata type is held in.
         *  @param dcName the Dublic Core metadata name.
         *  @param id3Tag the ID3v2 tag assciated with the metadata.
         *  @param localizationKey the key to get the localized name for
         *         the metadata
         *  @param tab  in which tab to show this kind of metadata in the
         *              Upload File window (default: none of them).
         */
        MetadataType(Ptr<MetadataTypeContainer>::Ref    container,
                     Glib::ustring                      dcName,
                     Glib::ustring                      id3Tag,
                     Glib::ustring                      localizationKey,
                     TabType                            tab = noTab)
                                                                    throw ();

        /**
         *  Return the name of the XML element this object expects
         *  to be sent to a call to configure().
         *  
         *  @return the name of the expected XML configuration element.
         */
        static const std::string
        getConfigElementName(void)                              throw ()
        {
            return configElementNameStr;
        }

        /**
         *  Configure the metadata object based on an XML configuration element.
         *
         *  @param element the XML configuration element.
         *  @exception std::invalid_argument of the supplied XML element
         *             contains bad configuration information
         */
        virtual void
        configure(const xmlpp::Element &element)
                                                throw (std::invalid_argument);


    public:
        /**
         *  A virtual destructor, as this class has virtual functions.
         */
        virtual
        ~MetadataType(void)                                     throw ()
        {
        }

        /**
         *  Return the Dublic Core name of the metadata type.
         *
         *  @return the Dublic Core name of the metadata type.
         */
        Ptr<const Glib::ustring>::Ref
        getDcName(void) const                               throw ()
        {
            return dcName;
        }

        /**
         *  Return the ID3v2 tag name for the metadata type.
         *
         *  @return the ID3v2 tag name for the metadata type, or a null
         *          pointer, if no ID3v2 tag exists for the metadata type.
         */
        Ptr<const Glib::ustring>::Ref
        getId3Tag(void) const                               throw ()
        {
            return id3Tag;
        }

        /**
         *  Return the localization key for the metadata type.
         *
         *  @return the localization key for the metadata type.
         */
        Ptr<const Glib::ustring>::Ref
        getLocalizationKey(void) const                      throw ()
        {
            return localizationKey;
        }

        /**
         *  Return the localized name for the metadata type.
         *
         *  @return the localized name for the metadata type.
         *  @exception std::invalid_argument if there is no localized
         *             name for this metadata type.
         */
        Ptr<const Glib::ustring>::Ref
        getLocalizedName(void) const            throw (std::invalid_argument);

        /**
         *  Return the tab that this metadata type should appear in,
         *  when an audio clip is uploaded in the Studio client.
         *
         *  @return the name of the tab for the metadata type.
         */
        TabType
        getTab(void) const                      throw ()
        {
            return tab;
        }

        /**
         *  Check that the given value satisfies the constraint.
         *  If the metadata type has no constraints, it returns true.
         *  If the constraint throws an exception, it returns false.
         *
         *  @param  value   the value to be checked against the constraint.
         *  @return true if the value satisfies the constraint.
         */
        bool
        check(Ptr<const Glib::ustring>::Ref     value) const        throw ();
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */


} // namespace Core
} // namespace LiveSupport

#endif // LiveSupport_Core_MetadataType_h

