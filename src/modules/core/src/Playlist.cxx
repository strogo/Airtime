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

#include <sstream>

#include "LiveSupport/Core/TimeConversion.h"
#include "LiveSupport/Core/Playlist.h"

using namespace boost::posix_time;

using namespace LiveSupport::Core;

/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/**
 *  The name of the config element for this class
 */
const std::string Playlist::configElementNameStr = "playlist";

/**
 *  The name of the attribute to get the id of the playlist.
 */
static const std::string    idAttrName = "id";

/**
 *  The name of the attribute to get the playlength of the playlist.
 */
static const std::string    playlengthAttrName = "playlength";

/**
 *  The name of the attribute to get the title of the playlist.
 */
static const std::string    titleAttrName = "title";

/**
 *  The name of playlist element child nodes.
 */
static const std::string    elementListAttrName = "playlistElement";

/**
 *  The name of the metadata child element.
 */
static const std::string    metadataElementName = "metadata";

/**
 *  The prefix of the extent (length) metadata element.
 */
static const std::string    extentElementPrefix = "dcterms";

/**
 *  The name of the extent (length) metadata element.
 */
static const std::string    extentElementName = "extent";

/**
 *  The prefix of the title metadata element.
 */
static const std::string    titleElementPrefix = "dc";

/**
 *  The name of the title metadata element.
 */
static const std::string    titleElementName = "title";

/**
 *  The prefix for the Live Support extension elements.
 */
static const std::string    liveSupportNamespacePrefix = "ls";

/**
 *  The prefix for the "xml:" prefix elements.
 */
static const std::string    xmlNamespacePrefix = "xml";

/**
 *  The URI identifier for the default namespace
 */
static const std::string    defaultNamespaceUri 
                            = "http://mdlf.org/livesupport/elements/1.0/";

/**
 *  The URI identifier for the "ls" prefix.
 */
static const std::string    liveSupportNamespaceUri 
                            = "http://mdlf.org/livesupport/elements/1.0/";

/**
 *  The URI identifier for the "dc" prefix
 */
static const std::string    dcNamespaceUri 
                            = "http://purl.org/dc/elements/1.1/";

/**
 *  The URI identifier for the "dcterms" prefix
 */
static const std::string    dctermsNamespaceUri 
                            = "http://purl.org/dc/terms/";

/**
 *  The URI identifier for the "xml" prefix
 */
static const std::string    xmlNamespaceUri 
                            = "http://www.w3.org/XML/1998/namespace";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Copy constructor.
 *----------------------------------------------------------------------------*/
Playlist :: Playlist(const Playlist & otherPlaylist)
                                            throw ()
                        : Playable(PlaylistType),
                          id        (otherPlaylist.id),
                          title     (otherPlaylist.title),
                          playlength(otherPlaylist.playlength),
                          uri       (otherPlaylist.uri),
                          token     (otherPlaylist.token)
{
    elementList.reset(new PlaylistElementListType);
    const_iterator  it;
    for (it = otherPlaylist.begin(); it != otherPlaylist.end(); ++it) {
        Ptr<PlaylistElement>::Ref   otherElement(new PlaylistElement(
                                                                *it->second ));
        elementList->insert(std::make_pair(it->first, otherElement));
    }
    
    if (otherPlaylist.savedCopy) {
        savedCopy.reset(new Playlist(*otherPlaylist.savedCopy));
    }
    
    if (otherPlaylist.xmlPlaylist) {
        xmlPlaylist.reset(new xmlpp::Document);
        xmlPlaylist->create_root_node_by_import(
                        otherPlaylist.xmlPlaylist->get_root_node(),
                        true);     // true == recursive
    }
}


/*------------------------------------------------------------------------------
 *  Test constructor without title.
 *----------------------------------------------------------------------------*/
Playlist :: Playlist(Ptr<UniqueId>::Ref            id,
                     Ptr<time_duration>::Ref       playlength,
                     Ptr<const std::string>::Ref   uri)
                                                           throw ()
                        : Playable(PlaylistType),
                          id(id),
                          uri(uri)
{
    this->title.reset(new Glib::ustring(""));
    
    elementList.reset(new PlaylistElementListType);

    setTitle(title);
    setPlaylength(playlength);
}


/*------------------------------------------------------------------------------
 *  Test constructor with title.
 *----------------------------------------------------------------------------*/
Playlist :: Playlist(Ptr<UniqueId>::Ref               id,
                     Ptr<const Glib::ustring>::Ref    title,
                     Ptr<time_duration>::Ref          playlength,
                     Ptr<const std::string>::Ref      uri)
                                                           throw ()
                        : Playable(PlaylistType),
                          id(id),
                          uri(uri)
{
    elementList.reset(new PlaylistElementListType);

    setTitle(title);
    setPlaylength(playlength);
}


/*------------------------------------------------------------------------------
 *  Convert to an XmlRpcValue.
 *----------------------------------------------------------------------------*/
Playlist :: operator XmlRpc::XmlRpcValue() const
                                                throw()
{
    XmlRpc::XmlRpcValue     xmlRpcValue;
    xmlRpcValue[configElementNameStr] = std::string(*getXmlDocumentString());
    
    return xmlRpcValue;
}


/*------------------------------------------------------------------------------
 *  Construct from an XmlRpcValue.
 *----------------------------------------------------------------------------*/
Playlist :: Playlist(XmlRpc::XmlRpcValue &  xmlRpcValue)
                                                throw (std::invalid_argument)
                        : Playable(PlaylistType)
{
    elementList.reset(new PlaylistElementListType);

    if (!xmlRpcValue.hasMember(configElementNameStr)) {
        throw std::invalid_argument("no playlist data found in XmlRpcValue");
    }
    
    xmlpp::DomParser    parser;
    try {
        parser.parse_memory(std::string(xmlRpcValue[configElementNameStr]));
    } catch (xmlpp::exception &e) {
        throw std::invalid_argument("error parsing XML document");
    }
    
    configure(*parser.get_document()->get_root_node());     // may throw
}


/*------------------------------------------------------------------------------
 *  Set the value of the title field.
 *----------------------------------------------------------------------------*/
void
Playlist :: setTitle(Ptr<const Glib::ustring>::Ref title)
                                                throw ()
{
    setMetadata(title, titleElementName, titleElementPrefix);
}


/*------------------------------------------------------------------------------
 *  Set the value of the playlength field (private).
 *----------------------------------------------------------------------------*/
void
Playlist :: setPlaylength(Ptr<time_duration>::Ref playlength)
                                                throw ()
{
    Ptr<const Glib::ustring>::Ref playlengthString(new const Glib::ustring(
                                        toFixedString(playlength) ));
    setMetadata(playlengthString, extentElementName, extentElementPrefix);
}


/*------------------------------------------------------------------------------
 *  Set the value of the playlength from a string (private).
 *----------------------------------------------------------------------------*/
void
Playlist :: setPlaylength(Ptr<const std::string>::Ref     timeString)
                                                throw (std::invalid_argument)
{
    try {
        playlength = TimeConversion::parseTimeDuration(timeString);
    } catch (std::exception &e) {
        std::string     eMsg = "bad time format in playlength: ";
        eMsg += e.what();
        throw std::invalid_argument(eMsg);
    }
}


/*------------------------------------------------------------------------------
 *  Create a playlist object based on an XML element.
 *----------------------------------------------------------------------------*/
void
Playlist :: configure(const xmlpp::Element    & element)
                                                throw (std::invalid_argument)
{
    if (element.get_name() != configElementNameStr) {
        std::string eMsg = "bad configuration element ";
        eMsg += element.get_name();
        throw std::invalid_argument(eMsg);
    }

    const xmlpp::Attribute    * attribute;

    if (!id) {
        if (!(attribute = element.get_attribute(idAttrName))) {
            std::string eMsg = "missing attribute ";
            eMsg += idAttrName;
            throw std::invalid_argument(eMsg);
        }
        id.reset(new UniqueId(attribute->get_value()));
    }
    
    if (!playlength
            && (attribute = element.get_attribute(playlengthAttrName))) {
        Ptr<Glib::ustring>::Ref playlengthString(new Glib::ustring(
                                                     attribute->get_value() ));
        setMetadata(playlengthString, extentElementName, extentElementPrefix);
    }
    
    if (!title
            && (attribute = element.get_attribute(titleAttrName))) {
        title.reset(new const Glib::ustring(attribute->get_value()));
        setMetadata(title, titleElementName, titleElementPrefix);
    }

    xmlpp::Node::NodeList  childNodes 
                           = element.get_children(elementListAttrName);
    xmlpp::Node::NodeList::iterator it = childNodes.begin();

    while (it != childNodes.end()) {
        Ptr<PlaylistElement>::Ref  newPlaylistElement(new PlaylistElement);
        const xmlpp::Element       * childElement 
                                   = dynamic_cast<const xmlpp::Element*> (*it);
        newPlaylistElement->configure(*childElement);    // may throw exception
        addPlaylistElement(newPlaylistElement);
        ++it;
    }

    childNodes  = element.get_children(metadataElementName);
    it          = childNodes.begin();

    if (it != childNodes.end()) {
        const xmlpp::Element    * metadataElement 
                                = dynamic_cast<const xmlpp::Element*> (*it);

        xmlPlaylist.reset(new xmlpp::Document);
        xmlpp::Element*     root = xmlPlaylist->create_root_node(
                                                        configElementNameStr);
        root->set_attribute(idAttrName, std::string(*id));
        root->import_node(metadataElement, true);    // true = recursive

        const xmlpp::Node::NodeList dataFieldList
                                    = metadataElement->get_children();
        xmlpp::Node::NodeList::const_iterator listIt = dataFieldList.begin();

        while (listIt != dataFieldList.end()) {
            const xmlpp::Node*  dataNode = *listIt;
            std::string         prefix   = dataNode->get_namespace_prefix();
            std::string         name     = dataNode->get_name();
            const xmlpp::Element*
                                dataElement 
                              = dynamic_cast<const xmlpp::Element*> (dataNode);
            if (!dataElement) {
                ++listIt;
                continue;
            }

            if (!playlength && prefix  == extentElementPrefix
                            && name    == extentElementName) {
                if (dataElement->has_child_text()) {
                    Ptr<const std::string>::Ref   playlengthString(
                                new std::string(dataElement->get_child_text()
                                                           ->get_content() ));
                    setPlaylength(playlengthString);
                } else {              // or just leave blank?  bad either way
                    playlength.reset(new time_duration(0,0,0,0));
                }
            }

            if (!title && prefix  == titleElementPrefix
                       && name    == titleElementName) {
                Glib::ustring       value;
                if (dataElement->has_child_text()) {
                    value = dataElement->get_child_text()->get_content();
                } else {
                    value = "";
                }
                Ptr<const Glib::ustring>::Ref ptrToValue(
                                                new const Glib::ustring(value));
                title = ptrToValue;
            }

            ++listIt;
        }
        
        ++it;
        if (it != childNodes.end()) {
            std::string eMsg = "more than one ";
            eMsg += metadataElementName;
            eMsg += " XML element";
            throw std::invalid_argument(eMsg);
        }
    }

    if (!playlength) {
        std::string eMsg = "missing attribute ";
        eMsg += playlengthAttrName;
        eMsg += " or metadata element ";
        eMsg += extentElementPrefix + ":" + extentElementName;
        throw std::invalid_argument(eMsg);
    }
    
    if (!title) {
        title.reset(new const Glib::ustring(""));
    }
}


/*------------------------------------------------------------------------------
 *  Add a new playlist element to the playlist.
 *----------------------------------------------------------------------------*/
void
Playlist::addPlaylistElement(Ptr<PlaylistElement>::Ref playlistElement)
                                                throw ()
{
    Ptr<const time_duration>::Ref  relativeOffset
                                   = playlistElement->getRelativeOffset();
    elementList->insert(std::make_pair(*relativeOffset, playlistElement));
}


/*------------------------------------------------------------------------------
 *  Add a new audio clip or sub-playlist to the playlist.
 *----------------------------------------------------------------------------*/
Ptr<UniqueId>::Ref
Playlist::addPlayable(Ptr<Playable>::Ref       playable,
                      Ptr<time_duration>::Ref  relativeOffset,
                      Ptr<FadeInfo>::Ref       fadeInfo)
                                                throw (std::invalid_argument)
{
    Ptr<AudioClip>::Ref     audioClip;
    Ptr<Playlist>::Ref      playlist;

    switch (playable->getType()) {
        case Playable::AudioClipType :
             audioClip = boost::dynamic_pointer_cast<AudioClip>(playable);
            return addAudioClip(audioClip, relativeOffset, fadeInfo);

        case Playable::PlaylistType :
             playlist = boost::dynamic_pointer_cast<Playlist>(playable);
            return addPlaylist(playlist, relativeOffset, fadeInfo);

        default :
            throw std::invalid_argument("Playable object is neither AudioClip"
                                        " nor Playlist?!");
    }
}


/*------------------------------------------------------------------------------
 *  Add a new audio clip to the playlist.
 *----------------------------------------------------------------------------*/
Ptr<UniqueId>::Ref
Playlist::addAudioClip(Ptr<AudioClip>::Ref      audioClip,
                       Ptr<time_duration>::Ref  relativeOffset,
                       Ptr<FadeInfo>::Ref       fadeInfo)
                                                throw ()
{
    Ptr<PlaylistElement>::Ref   playlistElement(new PlaylistElement(
                                    relativeOffset, audioClip, fadeInfo));
    elementList->insert(std::make_pair(*relativeOffset, playlistElement));
    
    Ptr<time_duration>::Ref     endOffset(new time_duration(
                                                *relativeOffset 
                                              + *audioClip->getPlaylength()));
    if (*endOffset > *playlength) {
        setPlaylength(endOffset);
    }
    
    return playlistElement->getId();
}


/*------------------------------------------------------------------------------
 *  Add a new sub-playlist to the playlist.
 *----------------------------------------------------------------------------*/
Ptr<UniqueId>::Ref
Playlist::addPlaylist(Ptr<Playlist>::Ref       playlist,
                      Ptr<time_duration>::Ref  relativeOffset,
                      Ptr<FadeInfo>::Ref       fadeInfo)
                                                throw ()
{
    Ptr<PlaylistElement>::Ref  playlistElement(new PlaylistElement(
                                   relativeOffset, playlist, fadeInfo));
    elementList->insert(std::make_pair(*relativeOffset, playlistElement));
    
    Ptr<time_duration>::Ref     endOffset(new time_duration(
                                                *relativeOffset 
                                              + *playlist->getPlaylength()));
    if (*endOffset > *playlength) {
        setPlaylength(endOffset);
    }
    
    return playlistElement->getId();
}


/*------------------------------------------------------------------------------
 *  Get an iterator pointing to a playlist element with a given ID.
 *----------------------------------------------------------------------------*/
Playlist::iterator
Playlist::find(Ptr<UniqueId>::Ref playlistElementId)
                                                throw ()
{
    Playlist::iterator  it = elementList->begin();
    while (it != elementList->end() 
            && *(it->second->getId()) != *playlistElementId) {
        ++it;
    }
    return it;
}


/*------------------------------------------------------------------------------
 *  Change the fade in / fade out info of a playlist element.
 *----------------------------------------------------------------------------*/
void
Playlist::setFadeInfo(Ptr<UniqueId>::Ref    playlistElementId,
                      Ptr<FadeInfo>::Ref    fadeInfo)
                                                throw (std::invalid_argument)
{
    Playlist::iterator it = this->find(playlistElementId);

    if (it == this->end()) {
        throw std::invalid_argument("no playlist element with this ID");
    }

    it->second->setFadeInfo(fadeInfo);
}


/*------------------------------------------------------------------------------
 *  Remove a playlist element from the playlist.
 *----------------------------------------------------------------------------*/
void
Playlist::removePlaylistElement(Ptr<UniqueId>::Ref  playlistElementId)
                                                throw (std::invalid_argument)
{
    Playlist::iterator it = this->find(playlistElementId);
    
    if (it == this->end()) {
        throw std::invalid_argument("no playlist element with this ID");
    }

    elementList->erase(it);
}


/*------------------------------------------------------------------------------
 *  Validate the playlist.
 *----------------------------------------------------------------------------*/
bool
Playlist::valid(void) const                     throw ()
{
    time_duration   runningTime(0,0,0,0);

    PlaylistElementListType::const_iterator  it = elementList->begin();
    while (it != elementList->end()) {
        Ptr<PlaylistElement>::Ref   playlistElement = it->second;

        if (playlistElement->getType() == PlaylistElement::PlaylistType
                && !playlistElement->getPlaylist()->valid()) {
            return false;
        }

        time_duration   relativeOffset = *playlistElement->getRelativeOffset();
        if (runningTime < relativeOffset) {
            return false;           // found a gap in the playlist
        }

        time_duration   endTime = 
                            relativeOffset
                            + *playlistElement->getPlayable()->getPlaylength();
        if (endTime > runningTime) {
            runningTime = endTime;
        }
        
        ++it;
    }
    
    if (runningTime != *getPlaylength()) {
        return false;
    }
    
    return true;
}


/*------------------------------------------------------------------------------
 *  Create a saved copy of the playlist.
 *----------------------------------------------------------------------------*/
void
Playlist::createSavedCopy(void)                 throw ()
{
    savedCopy = Ptr<Playlist>::Ref(new Playlist(*this));
}


/*------------------------------------------------------------------------------
 *  Revert to a saved copy of the playlist.
 *----------------------------------------------------------------------------*/
void
Playlist::revertToSavedCopy(void)               throw (std::invalid_argument)
{
    if (savedCopy == 0) {
        throw (std::invalid_argument("playlist has no saved copy"));
    }

    this->id            = savedCopy->id;
    this->title         = savedCopy->title;
    this->playlength    = savedCopy->playlength;
    this->uri           = savedCopy->uri;
    this->token         = savedCopy->token;
    this->elementList   = savedCopy->elementList;
    this->xmlPlaylist   = savedCopy->xmlPlaylist;

    savedCopy.reset();
}


/*------------------------------------------------------------------------------
 *  Return the value of a metadata field.
 *----------------------------------------------------------------------------*/
Ptr<Glib::ustring>::Ref
Playlist :: getMetadata(const string &key) const
                                                throw ()
{
    std::string name, prefix;
    separateNameAndNameSpace(key, name, prefix);

    Ptr<Glib::ustring>::Ref value;

    if (! xmlPlaylist) {
        return value;
    }
    xmlpp::Element*         rootNode = xmlPlaylist->get_root_node();
    if (! rootNode) {
        return value;
    }
    xmlpp::Node::NodeList   rootList = rootNode->get_children(
                                                        metadataElementName);
    if (rootList.size() == 0) {
        return value;
    }
    
    xmlpp::Node*            metadata = rootList.front();
    xmlpp::Node::NodeList   nodeList = metadata->get_children(name);
    xmlpp::Node::NodeList::iterator it = nodeList.begin();
    
    while (it != nodeList.end()) {
        xmlpp::Node*        node = *it;
        if (node->get_namespace_prefix() == prefix) {
            xmlpp::Element*   element = dynamic_cast<xmlpp::Element*> (node);
            xmlpp::TextNode*  textNode = element->get_child_text();
            if (textNode) {
                value.reset(new Glib::ustring(textNode->get_content()));
            } else {
                value.reset(new Glib::ustring(""));
            }
            return value;
        }
        ++it;
    }

    return value;
}


/*------------------------------------------------------------------------------
 *  Set the value of a metadata field (public).
 *----------------------------------------------------------------------------*/
void
Playlist :: setMetadata(Ptr<const Glib::ustring>::Ref value, 
                        const std::string &key)
                                                throw (std::invalid_argument)
{
    std::string name, prefix;
    separateNameAndNameSpace(key, name, prefix);
    setMetadata(value, name, prefix);
}


/*------------------------------------------------------------------------------
 *  Set the value of a metadata field (private).
 *----------------------------------------------------------------------------*/
void
Playlist :: setMetadata(Ptr<const Glib::ustring>::Ref value, 
                        const std::string &name, const std::string &prefix)
                                                throw (std::invalid_argument)
{
    if (prefix == extentElementPrefix && name == extentElementName) {
        Ptr<const std::string>::Ref     valueString(new const std::string(
                                                                        *value));
        setPlaylength(valueString);
    }
    
    if (prefix == titleElementPrefix && name == titleElementName) {
        title = value;
    }

    // create a new xmlpp::Document for the metadata if necessary
    if (! xmlPlaylist) {
        xmlPlaylist.reset(new xmlpp::Document);
    }
    xmlpp::Element*         rootNode = xmlPlaylist->get_root_node();
    if (! rootNode) {
        rootNode = xmlPlaylist->create_root_node(configElementNameStr);
        if (id) {
            rootNode->set_attribute(idAttrName, std::string(*id));
        }
    }
    xmlpp::Node::NodeList   rootList = rootNode->get_children(
                                                        metadataElementName);
    xmlpp::Element*         metadata;
    if (rootList.size() > 0) {
        metadata = dynamic_cast<xmlpp::Element*> (rootList.front());
    } else {
        metadata = rootNode->add_child(metadataElementName);
        metadata->set_namespace_declaration(defaultNamespaceUri);
        metadata->set_namespace_declaration(liveSupportNamespaceUri, 
                                            liveSupportNamespacePrefix);
        metadata->set_namespace_declaration(dcNamespaceUri, 
                                            titleElementPrefix);
        metadata->set_namespace_declaration(dctermsNamespaceUri, 
                                            extentElementPrefix);
        metadata->set_namespace_declaration(xmlNamespaceUri, 
                                            xmlNamespacePrefix);
    }

    // find the element to be modified
    xmlpp::Node::NodeList   nodeList    = metadata->get_children(name);
    xmlpp::Node::NodeList::iterator it  = nodeList.begin();
    xmlpp::Element*         element     = 0;

    while (it != nodeList.end()) {
        xmlpp::Node*        node = *it;
        if (node->get_namespace_prefix() == prefix) {
            element = dynamic_cast<xmlpp::Element*> (nodeList.front());
            break;
        }
        ++it;
    }
    
    // or add it if it did not exist before
    if (it == nodeList.end()) {
        element = metadata->add_child(name);
        try {
            element->set_namespace(prefix);
        }
        catch (xmlpp::exception &e) {
        // this namespace has not been declared; well OK, do nothing then
        }
    }
    
    element->set_child_text(*value);
}


/*------------------------------------------------------------------------------
 *  Return a string containing the essential fields of this object, in XML.
 *----------------------------------------------------------------------------*/
Ptr<Glib::ustring>::Ref
Playlist :: getXmlElementString(void) const     throw ()
{
    Ptr<Glib::ustring>::Ref     xmlString(new Glib::ustring);
    
    xmlString->append("<");
    xmlString->append(configElementNameStr + " ");
    xmlString->append(idAttrName + "=\"" 
                                 + std::string(*getId()) 
                                 + "\" ");
    xmlString->append(Glib::ustring(titleAttrName) + "=\"" 
                                                   + *getTitle()
                                                   + "\" ");
    xmlString->append(playlengthAttrName + "=\"" 
                                         + toFixedString(getPlaylength())
                                         + "\"/>");
    
    return xmlString;
}


/*------------------------------------------------------------------------------
 *  Return a string containing an XML representation of this playlist.
 *----------------------------------------------------------------------------*/
Ptr<Glib::ustring>::Ref
Playlist :: getXmlDocumentString() const        throw ()
{
    Ptr<xmlpp::Document>::Ref   localDocument;

    if (xmlPlaylist) {
        localDocument = xmlPlaylist;
    } else {
        localDocument.reset(new xmlpp::Document());
        xmlpp::Element* rootNode = localDocument->create_root_node(
                                                        configElementNameStr);
        if (id) {
            rootNode->set_attribute(idAttrName, std::string(*id));
        }
        
        xmlpp::Element*     metadata = rootNode->add_child(metadataElementName);
        metadata->set_namespace_declaration(defaultNamespaceUri);
        metadata->set_namespace_declaration(liveSupportNamespaceUri, 
                                            liveSupportNamespacePrefix);
        metadata->set_namespace_declaration(dcNamespaceUri, 
                                            titleElementPrefix);
        metadata->set_namespace_declaration(dctermsNamespaceUri, 
                                            extentElementPrefix);
        metadata->set_namespace_declaration(xmlNamespaceUri, 
                                            xmlNamespacePrefix);
    }

    Glib::ustring               playlistElementsXmlString("\n");
    Playlist::const_iterator    it = this->begin();
    while (it != this->end()) {
        playlistElementsXmlString += *(it->second->getXmlElementString());
        playlistElementsXmlString += '\n';
        ++it;
    }

    Ptr<Glib::ustring>::Ref metadataString(new Glib::ustring(
                                            localDocument->write_to_string() ));
    unsigned int    insertPosition = metadataString->find(metadataElementName);
    insertPosition = metadataString->rfind('<', insertPosition);
    metadataString->insert(insertPosition, playlistElementsXmlString);

    return metadataString;
}


/*------------------------------------------------------------------------------
 *  Eliminate the gaps in the playlist.
 *----------------------------------------------------------------------------*/
bool
Playlist :: eliminateGaps(void)                 throw ()
{
    bool    didSomething = false;

    Playlist::const_iterator    it = this->begin();
    while (it != this->end()) {
        Ptr<PlaylistElement>::Ref   playlistElement = it->second;
        if (playlistElement->getType() == PlaylistElement::PlaylistType) {
            Ptr<Playlist>::Ref      playlist = playlistElement->getPlaylist();
            didSomething |= playlist->eliminateGaps();
        }
        ++it;
    }
    
    time_duration   position(0,0,0,0);
    time_duration   gapsFound(0,0,0,0);
    
    it = this->begin();
    while (it != this->end()) {
        Ptr<PlaylistElement>::Ref   playlistElement = it->second;
        Ptr<time_duration>::Ref     startTime 
                                        = playlistElement->getRelativeOffset();
        Ptr<time_duration>::Ref     newStartTime;
        
        if (*startTime - gapsFound > position) {
            newStartTime.reset(new time_duration(position));
            playlistElement->setRelativeOffset(newStartTime);
            gapsFound = *startTime - position;
            didSomething = true;
            
        } else if (gapsFound.total_microseconds() != 0) {
            newStartTime.reset(new time_duration(*startTime - gapsFound));
            playlistElement->setRelativeOffset(newStartTime);
            
        } else {
            newStartTime = startTime;
        }
        
        position = *newStartTime + *playlistElement->getPlayable()
                                                   ->getPlaylength();
        ++it;
    }
    
    if (didSomething || position != *getPlaylength()) {
        Ptr<time_duration>::Ref     newPlaylength(new time_duration(position));
        setPlaylength(newPlaylength);
        return true;
    } else {
        return false;
    }
}

