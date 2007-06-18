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

#include "LiveSupport/Core/TimeConversion.h"
#include "LiveSupport/Widgets/WidgetFactory.h"
#include "LiveSupport/Widgets/Colors.h"

#include "NowPlaying.h"


using namespace LiveSupport::Core;
using namespace LiveSupport::Widgets;
using namespace LiveSupport::GLiveSupport;

/* ===================================================  local data structures */


/* ================================================  local constants & macros */

namespace {

/**
 *  This should be 1/4th of MasterPanelWindow::updateTimeConstant.
 */
const int   blinkingConstant = 5;

/**
 *  The string which identifies the Play stock image.
 */
const Glib::ustring     playStockImageName = "gtk-media-play";

/**
 *  The string which identifies the Pause stock image.
 */
const Glib::ustring     pauseStockImageName = "gtk-media-pause";

}

/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Constructor.
 *----------------------------------------------------------------------------*/
NowPlaying :: NowPlaying(Ptr<GLiveSupport>::Ref             gLiveSupport,
                         Ptr<ResourceBundle>::Ref           bundle,
                         Glib::RefPtr<Gnome::Glade::Xml>    glade)
                                                                    throw ()
          : LocalizedObject(bundle),
            glade(glade),
            gLiveSupport(gLiveSupport)
{
    glade->get_widget("playButton", playButton);
    glade->get_widget("stopButton", stopButton);

    playButton->signal_clicked().connect(sigc::mem_fun(*this,
                                        &NowPlaying::onPlayButtonClicked ));
    stopButton->signal_clicked().connect(sigc::mem_fun(*this,
                                        &NowPlaying::onStopButtonClicked ));

    isActive = false;
    isPaused = false;

    glade->get_widget("titleLabel", titleLabel);
    glade->get_widget("creatorLabel", creatorLabel);
    glade->get_widget("elapsedTimeLabel", elapsedTimeLabel);
    glade->get_widget("remainsTimeBox", remainsTimeBox);
    glade->get_widget("remainsTimeLabel", remainsTimeLabel);
    glade->get_widget("playlistLabel", playlistLabel);

    Gtk::Label *    elapsedTimeText;
    Gtk::Label *    remainsTimeText;
    glade->get_widget("elapsedTimeText", elapsedTimeText);
    glade->get_widget("remainsTimeText", remainsTimeText);
    try {
        elapsedTimeText->set_text(*getResourceUstring("elapsedTimeLabel"));
        remainsTimeText->set_text(*getResourceUstring("remainingTimeLabel"));
    } catch (std::invalid_argument &e) {
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }
    
    Ptr<Playable>::Ref      nullPointer;
    setPlayable(nullPointer);
}


/*------------------------------------------------------------------------------
 *  Set the title etc. of the playable shown in the widget.
 *----------------------------------------------------------------------------*/
void
NowPlaying :: setPlayable(Ptr<Playable>::Ref  playable)             throw ()
{
    if (playable) {
        if (!isActive || isPaused) {
            playButton->set_label(pauseStockImageName);
        }
        playButton->set_sensitive(true);
        stopButton->set_sensitive(true);

        this->playable = playable;
        isActive = true;
        isPaused = false;
        resetRemainsTimeState();
        onUpdateTime();
        
    } else {
        if (isActive && !isPaused) {
            playButton->set_label(playStockImageName);
            isActive = false;
        }
        playButton->set_sensitive(false);
        stopButton->set_sensitive(false);

        titleLabel->set_text("");
        creatorLabel->set_text("");
        elapsedTimeLabel->set_text("");
        remainsTimeLabel->set_text("");
        playlistLabel->set_text("");
        resetRemainsTimeState();
        this->playable.reset();
        this->currentInnerPlayable.reset();
    }
}


/*------------------------------------------------------------------------------
 *  Event handler for the Play button being clicked.
 *----------------------------------------------------------------------------*/
void
NowPlaying :: onPlayButtonClicked(void)                             throw ()
{
    if (isActive) {
        if (isPaused) {
            gLiveSupport->pauseOutputAudio();       // i.e., restart
            playButton->set_label(pauseStockImageName);
            isPaused = false;
        } else {
            gLiveSupport->pauseOutputAudio();
            playButton->set_label(playStockImageName);
            isPaused = true;
        }
    }
}


/*------------------------------------------------------------------------------
 *  Event handler for the Stop button being clicked.
 *----------------------------------------------------------------------------*/
void
NowPlaying :: onStopButtonClicked(void)                             throw ()
{
    if (isActive) {
        gLiveSupport->stopOutputAudio();    // triggers a call to GLiveSupport::
    }                                       // onStop(), which in turn calls
}                                           // setPlayable() with a 0 argument


/*------------------------------------------------------------------------------
 *  Update the timer displays. This is called every second by the master panel.
 *----------------------------------------------------------------------------*/
void
NowPlaying :: onUpdateTime(void)                                    throw ()
{
    if (!isActive) {
        return;
    }

    remainsTimeCounter++;
    if (remainsTimeCounter == 2*blinkingConstant) {
        remainsTimeCounter = 0;
    }

    Ptr<time_duration>::Ref     elapsed;
    try {
        elapsed = gLiveSupport->getOutputAudioPosition();
        
    } catch (std::logic_error &e) {
        // just act as if nothing has happened
        return;
    }
    
    Ptr<time_duration>::Ref     totalLength
                                = TimeConversion::roundToNearestSecond(
                                                    playable->getPlaylength());
    Ptr<time_duration>::Ref     remains(new time_duration(
                                                    *totalLength - *elapsed));
    switch (remainsTimeState) {
        case TIME_GREEN :
            if (*remains <= seconds(20)) {
                remainsTimeState = TIME_YELLOW;
            }
            break;
        
        case TIME_YELLOW :
            if (*remains <= seconds(10)) {
                remainsTimeState = TIME_RED;
            }
            break;
        
        case TIME_RED :
            break;
    }
    setRemainsTimeColor(remainsTimeState);

    Ptr<Playable>::Ref          innerPlayable   = playable;
    Ptr<time_duration>::Ref     innerElapsed    = elapsed;
    Ptr<time_duration>::Ref     innerRemains    = remains;
    Glib::ustring               playlistInfo;
    bool                        isFirst      = true;
    
    while (innerPlayable->getType() == Playable::PlaylistType) {
        if (isFirst) {
            isFirst = false;
        } else {
            playlistInfo += "   >>>   ";
        }
        playlistInfo += *innerPlayable->getTitle();
        playlistInfo += " [";
        playlistInfo += *TimeConversion::timeDurationToHhMmSsString(
                                                innerRemains);
        playlistInfo += "/";
        playlistInfo += *TimeConversion::timeDurationToHhMmSsString(
                                                innerPlayable->getPlaylength());
        playlistInfo += "]";
        
        Ptr<PlaylistElement>::Ref   element
                                    = innerPlayable->getPlaylist()
                                                   ->findAtOffset(elapsed);
        if (!element) {
            break;
        }
        innerPlayable = element->getPlayable();
        *innerElapsed -= *element->getRelativeOffset();
        *innerRemains = *TimeConversion::roundToNearestSecond(
                                                innerPlayable->getPlaylength())
                      - *innerElapsed;
    }

    playlistLabel->set_text(playlistInfo);

    titleLabel->set_text(*innerPlayable->getTitle());

    Ptr<Glib::ustring>::Ref
                    creator = innerPlayable->getMetadata("dc:creator");
    if (creator) {
        creatorLabel->set_text(*creator);
    } else {
        creatorLabel->set_text("");
    }
    
    elapsedTimeLabel->set_text(*TimeConversion::timeDurationToHhMmSsString(
                                                        innerElapsed ));
    remainsTimeLabel->set_text(*TimeConversion::timeDurationToHhMmSsString(
                                                        innerRemains ));

    currentInnerPlayable = innerPlayable;
}


/*------------------------------------------------------------------------------
 *  Set the background color of the "remains time" label.
 *----------------------------------------------------------------------------*/
void
NowPlaying :: setRemainsTimeColor(RemainsTimeStateType  state)      throw ()
{
    bool        isBlinkOn = (remainsTimeCounter < blinkingConstant);

    if (isBlinkOn) {
        switch (state) {
            case TIME_GREEN:
                remainsTimeBox->unset_bg(Gtk::STATE_NORMAL);
                break;
                
            case TIME_YELLOW:
                remainsTimeBox->modify_bg(Gtk::STATE_NORMAL,
                                          Colors::getColor(Colors::Yellow));
                break;
                
            case TIME_RED:
                remainsTimeBox->modify_bg(Gtk::STATE_NORMAL,
                                          Colors::getColor(Colors::Red));
                break;
        }
    } else {
        remainsTimeBox->unset_bg(Gtk::STATE_NORMAL);
    }

    gLiveSupport->runMainLoop();
}


/*------------------------------------------------------------------------------
 *  Reset all remains-time-blinking related variables.
 *----------------------------------------------------------------------------*/
void
NowPlaying :: resetRemainsTimeState(void)                           throw ()
{
    remainsTimeState   = TIME_GREEN;
    remainsTimeCounter = 0;
    setRemainsTimeColor(TIME_GREEN);
}

