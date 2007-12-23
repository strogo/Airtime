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
 
 
    Author   : $Author: fgerlits $
    Version  : $Revision: 2329 $
    Location : $URL: svn://code.campware.org/campcaster/branches/gstreamer-0.10/campcaster/src/modules/playlistExecutor/src/AudioPlayerFactory.cxx $

------------------------------------------------------------------------------*/

/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#include "LiveSupport/PlaylistExecutor/AudioPlayerFactory.h"
#include "GstreamerPlayer.h"


using namespace LiveSupport::Core;
using namespace LiveSupport::PlaylistExecutor;

/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  The name of the config element for this class
 *----------------------------------------------------------------------------*/
const std::string AudioPlayerFactory::configElementNameStr =
                                                    "audioPlayer";

/*------------------------------------------------------------------------------
 *  The singleton instance of AudioPlayerFactory
 *----------------------------------------------------------------------------*/
Ptr<AudioPlayerFactory>::Ref AudioPlayerFactory::singleton;


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Return the singleton instance to AudioPlayerFactory
 *----------------------------------------------------------------------------*/
Ptr<AudioPlayerFactory>::Ref
AudioPlayerFactory :: getInstance(void)                   throw ()
{
    if (!singleton.get()) {
        singleton.reset(new AudioPlayerFactory());
    }

    return singleton;
}


/*------------------------------------------------------------------------------
 *  Configure the connection manager factory.
 *----------------------------------------------------------------------------*/
void
AudioPlayerFactory :: configure(const xmlpp::Element & element)
                                                throw (std::invalid_argument,
                                                       std::logic_error)
{
    if (element.get_name() != configElementNameStr) {
        std::string eMsg = "Bad configuration element ";
        eMsg += element.get_name();
        throw std::invalid_argument(eMsg);
    }

    audioPlayer.reset();

    xmlpp::Node::NodeList   nodes;

    // try to look for a GstreamerPlayer configuration element
    nodes = element.get_children(GstreamerPlayer::getConfigElementName());
    if (nodes.size() >= 1) {
        const xmlpp::Element  * configElement =
                         dynamic_cast<const xmlpp::Element*> (*(nodes.begin()));
        Ptr<GstreamerPlayer>::Ref   gp(new GstreamerPlayer());
        gp->configure(*configElement);
        audioPlayer = gp;

        return;
    }

    throw std::invalid_argument("no audio player factories to configure");
}


