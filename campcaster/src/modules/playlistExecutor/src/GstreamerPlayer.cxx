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
 
 
    Author   : $Author: mark $
    Version  : $Revision: 3108 $
    Location : $URL: svn://code.campware.org/campcaster/branches/gstreamer-0.10/campcaster/src/modules/playlistExecutor/src/GstreamerPlayer.cxx $

------------------------------------------------------------------------------*/

/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#define DEBUG_PREFIX "GstreamerPlayer"
#include "LiveSupport/Core/Debug.h"

#include "LiveSupport/Core/TimeConversion.h"

#include "GstreamerPlayer.h"


using namespace boost::posix_time;
using namespace LiveSupport::Core;
using namespace LiveSupport::PlaylistExecutor;

/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/**
 *  The name of the config element for this class
 */
const std::string GstreamerPlayer::configElementNameStr = "gstreamerPlayer";

/**
 *  The name of the audio device attribute.
 */
static const std::string    audioDeviceName = "audioDevice";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

static gboolean my_bus_callback (GstBus *bus, GstMessage *message, gpointer data)
{
    DEBUG_BLOCK

    GstreamerPlayer* const player = (GstreamerPlayer*) data;

    switch (GST_MESSAGE_TYPE (message)) {
        case GST_MESSAGE_EOS:
            debug() << "EOS reached\n";
            if(player->playNextSmil()){
                break;
            }
            player->close();
            player->m_eos = true;
            // Important: We *must* use an idle function call here, so that the signal handler returns
            // before fireOnStopEvent() is executed.
            g_print("MY TEST: my_bus_callback GST_MESSAGE_EOS firing GstreamerPlayer::fireOnStopEvent !!!!!!!!!!!!!!!!!!!!!!\n");
            g_idle_add(GstreamerPlayer::fireOnStopEvent, data);
        break;
    }
}
/*------------------------------------------------------------------------------
 *  Configure the Audio Player.
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: configure(const xmlpp::Element   &  element)
                                                throw (std::invalid_argument,
                                                       std::logic_error)
{
    DEBUG_FUNC_INFO

    if (element.get_name() != configElementNameStr) {
        std::string eMsg = "Bad configuration element ";
        eMsg += element.get_name();
        throw std::invalid_argument(eMsg);
    }

    const xmlpp::Attribute    * attribute = 0;

    if ((attribute = element.get_attribute(audioDeviceName))) {
        m_audioDevice = attribute->get_value();
    }
}


/*------------------------------------------------------------------------------
 *  Initialize the Audio Player
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: initialize(void)                 throw (std::exception)
{
    DEBUG_FUNC_INFO

    g_print("MY TEST: GstreamerPlayer :: initialize !!!!!!!!!!!!!!!!!!!!!!\n");

    if (m_initialized) {
        return;
    }

    // initialize the gstreamer library
    if (!gst_init_check(0, 0, 0)) {
        throw std::runtime_error("couldn't initialize the gstreamer library");
    }

    m_playContext = new GstreamerPlayContext();

    m_playContext->setParentData(this);


    m_eos = false;
    
//    g_signal_connect(m_pipeline, "error", G_CALLBACK(errorHandler), this);

/*    // TODO: read the caps from the config file
    m_sinkCaps = gst_caps_new_simple("audio/x-raw-int",
                                   "width", G_TYPE_INT, 16,
                                   "depth", G_TYPE_INT, 16,
                                   "endiannes", G_TYPE_INT, G_BYTE_ORDER,
                                   "channels", G_TYPE_INT, 2,
                                   "rate", G_TYPE_INT, 44100,
                                   NULL);
*/
    m_playContext->setAudioDevice(m_audioDevice);

    // set up other variables
    m_initialized = true;
}


/*------------------------------------------------------------------------------
 *  De-initialize the Gstreamer Player
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: deInitialize(void)                       throw ()
{
    DEBUG_FUNC_INFO

    g_print("MY TEST: GstreamerPlayer :: deInitialize !!!!!!!!!!!!!!!!!!!!!!\n");

    if (m_initialized) {
        m_playContext->closeContext();
        delete m_playContext;
        m_playContext = NULL;
        m_initialized = false;
    }
}


/*------------------------------------------------------------------------------
 *  Handler for gstreamer errors.
 *----------------------------------------------------------------------------*/
/*
void
GstreamerPlayer :: errorHandler(GstElement   * pipeline,
                                GstElement   * source,
                                GError       * error,
                                gchar        * debug,
                                gpointer       self)
                                                                throw ()
{
    DEBUG_BLOCK
    g_print("MY TEST: GstreamerPlayer :: errorHandler !!!!!!!!!!!!!!!!!!!!!!\n");

    GstreamerPlayer* const player = (GstreamerPlayer*) self;

    // We make sure that we don't send multiple error messages in a row to the GUI
    if (!player->m_errorWasRaised) {
        player->m_errorMessage.reset(new const Glib::ustring(error->message));
        player->m_errorWasRaised = true;

        std::cerr << "gstreamer error: " << error->message << std::endl;

        // Important: We *must* use an idle function call here, so that the signal handler returns 
        // before fireOnStopEvent() is executed.
        g_idle_add(fireOnStopEvent, self);
    }

}
*/

/*------------------------------------------------------------------------------
 *  Attach an event listener.
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: attachListener(AudioPlayerEventListener*     eventListener)
                                                                    throw ()
{
    g_print("MY TEST: GstreamerPlayer :: attachListener !!!!!!!!!!!!!!!!!!!!!!\n");
    m_listeners.push_back(eventListener);
}


/*------------------------------------------------------------------------------
 *  Detach an event listener.
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: detachListener(AudioPlayerEventListener*     eventListener)
                                                throw (std::invalid_argument)
{
    ListenerVector::iterator    it  = m_listeners.begin();
    ListenerVector::iterator    end = m_listeners.end();

    while (it != end) {
        if (*it == eventListener) {
            m_listeners.erase(it);
            return;
        }
        ++it;
    }

    throw std::invalid_argument("supplied event listener not found");
}


/*------------------------------------------------------------------------------
 *  Send the onStop event to all attached listeners.
 *----------------------------------------------------------------------------*/
gboolean
GstreamerPlayer :: fireOnStopEvent(gpointer self)                       throw ()
{
    DEBUG_BLOCK

    g_print("MY TEST: GstreamerPlayer :: fireOnStopEvent !!!!!!!!!!!!!!!!!!!!!!\n");

    GstreamerPlayer* const player = (GstreamerPlayer*) self;

    ListenerVector::iterator    it  = player->m_listeners.begin();
    ListenerVector::iterator    end = player->m_listeners.end();
    while (it != end) {
        g_print("MY TEST: GstreamerPlayer :: fireOnStopEvent iteration !!!!!!!!!!!!!!!!!!!!!!\n");
        (*it)->onStop(player->m_errorMessage);
        ++it;
    }

    player->m_errorMessage.reset();

    // false == Don't call this idle function again
    return false;
}



/*------------------------------------------------------------------------------
 * Bus event handler. Processes messages from the pipeline bus. 
 *----------------------------------------------------------------------------*/
/*gboolean
GstreamerPlayer::busEventHandler(GstBus*, GstMessage* msg, gpointer self) throw()
{
    return GST_BUS_DROP;
#if 0
    DEBUG_FUNC_INFO

    //GstreamerPlayer* const player = (GstreamerPlayer*) self;

    switch (GST_MESSAGE_TYPE(msg))
    {
    case GST_MESSAGE_ERROR:
        GError* error;
        gchar* debugs;

        gst_message_parse_error(msg,&error,&debugs);
        debug() << "ERROR RECEIVED IN BUS_CB <" << error->message << ">" << endl;;
        break;

    default:
        break;
    }

    gst_message_unref(msg);
    return GST_BUS_DROP;
#endif
}
*/

/*------------------------------------------------------------------------------
 * Preload a file, to speed up the subsequent open() call. 
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: preload(const std::string   fileUrl)
                                                throw (std::invalid_argument,
                                                       std::runtime_error)
{
    DEBUG_BLOCK

    g_print("MY TEST: GstreamerPlayer :: preload = %s !!!!!!!!!!!!!!!!!!!!!!\n", fileUrl.c_str());
#if 0
    if (m_preloadThread) {
        m_preloadThread->stop();
        m_preloadThread->join();
        m_preloadThread.reset();
    }

    Ptr<Preloader>::Ref loader;
    loader.reset(new Preloader(this, fileUrl));

    m_preloadThread.reset(new Thread(loader));
    m_preloadThread->start();
#endif
}


/*------------------------------------------------------------------------------
 *  Specify which file to play
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: open(const std::string   fileUri)
                                                throw (std::invalid_argument,
                                                       std::runtime_error)
{
    DEBUG_BLOCK

    g_print("MY TEST: GstreamerPlayer :: open file = %s !!!!!!!!!!!!!!!!!!!!!!\n", fileUri.c_str());

    if (isOpen()) {
        close();
    }

    debug() << "Opening URL: " << fileUri << endl;
    debug() << "Timestamp: " << *TimeConversion::now() << endl;


    if (fileUri.find(std::string(".smil")) != std::string::npos) {
        m_smilHandler = new SmilHandler();
        m_smilHandler->openSmilFile(fileUri.c_str());
        AudioDescription *audioDescription = m_smilHandler->getNext();
        g_print("MY TEST: m_playContext->openSource %s !!!!!!!!!!!!!!!!!!!!!!\n", audioDescription->m_src);
        m_playContext->openSource(audioDescription);
    }else{
        g_print("MY TEST: m_playContext->openSource %s !!!!!!!!!!!!!!!!!!!!!!\n", fileUri.c_str());
        m_playContext->openSource(fileUri.c_str());
    }

    m_open=true;
    m_eos = false;
}

bool 
GstreamerPlayer :: playNextSmil(void)                                    throw ()
{
    if(m_smilHandler == NULL) return false;
    AudioDescription *audioDescription = m_smilHandler->getNext();
    if(audioDescription == NULL){
        delete m_smilHandler;
        m_smilHandler = NULL;
        return false;
    }
    g_print("MY TEST: m_playContext->openSource %s !!!!!!!!!!!!!!!!!!!!!!\n", audioDescription->m_src);
    m_playContext->openSource(audioDescription);
    m_playContext->playContext();
}


/*------------------------------------------------------------------------------
 *  Tell if we've been opened.
 *----------------------------------------------------------------------------*/
bool
GstreamerPlayer :: isOpen(void)                                 throw ()
{
    return m_open;
}


/*------------------------------------------------------------------------------
 *  Get the length of the current audio clip.
 *  Currently not used by the Studio, but may be used later on.
 *----------------------------------------------------------------------------*/
Ptr<time_duration>::Ref
GstreamerPlayer :: getPlaylength(void)              throw (std::logic_error)
{
    DEBUG_BLOCK

    Ptr<time_duration>::Ref   length;
    gint64                    ns = m_playContext->getPlayLength();

    g_print("MY TEST: GstreamerPlayer :: getPlaylength = %d !!!!!!!!!!!!!!!!!!!!!!\n", ns);

    if (!isOpen()) {
        throw std::logic_error("player not open");
    }

    length.reset(new time_duration(microsec(ns / 1000LL)));

    debug() << length << endl;
    return length;
}


/*------------------------------------------------------------------------------
 *  Get the current position of the current audio clip.
 *----------------------------------------------------------------------------*/
Ptr<time_duration>::Ref
GstreamerPlayer :: getPosition(void)                throw (std::logic_error)
{
    Ptr<time_duration>::Ref   length;

    if (!isOpen()) {
        throw std::logic_error("player not open");
    }

    gint64                    ns = m_playContext->getPosition();

//    g_print("MY TEST: GstreamerPlayer :: getPosition = %d !!!!!!!!!!!!!!!!!!!!!!\n", ns);

    length.reset(new time_duration(microseconds(ns / 1000LL)));

    return length;
}


/*------------------------------------------------------------------------------
 *  Start playing
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: start(void)                      throw (std::logic_error)
{
    DEBUG_BLOCK
    g_print("MY TEST: GstreamerPlayer :: start !!!!!!!!!!!!!!!!!!!!!!\n");

    if (!isOpen()) {
        throw std::logic_error("GstreamerPlayer not opened yet");
    }

    if (!isPlaying()) {
        m_playContext->playContext();
    }else{
        error() << "Already playing!" << endl;
    }
}


/*------------------------------------------------------------------------------
 *  Pause the player
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: pause(void)                      throw (std::logic_error)
{
    DEBUG_BLOCK

    if (isPlaying()) {
        m_playContext->pauseContext();
    }
}


/*------------------------------------------------------------------------------
 *  Tell if we're playing
 *----------------------------------------------------------------------------*/
bool
GstreamerPlayer :: isPlaying(void)                  throw ()
{
    if (m_eos) {
        m_eos = false;
        m_playContext->stopContext();
        return false;
    }
    bool b = m_playContext->isPlaying();
    g_print("MY TEST: GstreamerPlayer :: isPlaying = %d !!!!!!!!!!!!!!!!!!!!!!\n", b);
    return b;
}


/*------------------------------------------------------------------------------
 *  Stop playing
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: stop(void)                       throw (std::logic_error)
{
    DEBUG_BLOCK
    g_print("MY TEST: GstreamerPlayer :: stop !!!!!!!!!!!!!!!!!!!!!!\n");

    if (!isOpen()) {
        throw std::logic_error("GstreamerPlayer not opened yet");
    }

    if (isPlaying()) {
        m_playContext->stopContext();
    }
}
 

/*------------------------------------------------------------------------------
 *  Close the currently opened audio file.
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: close(void)                       throw (std::logic_error)
{
    DEBUG_BLOCK

    m_playContext->closeContext();

    m_open            = false;
}


/*------------------------------------------------------------------------------
 *  Get the volume of the player. *Unimplemented*: Feature is currently not used.
 *----------------------------------------------------------------------------*/
unsigned int
GstreamerPlayer :: getVolume(void)                                  throw ()
{
    return 0;
}


/*------------------------------------------------------------------------------
 *  Set the volume of the player. *Unimplemented*: Feature is currently not used.
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: setVolume(unsigned int   volume)                 throw ()
{}


/*------------------------------------------------------------------------------
 *  Set the audio device.
 *----------------------------------------------------------------------------*/
bool
GstreamerPlayer :: setAudioDevice(const std::string &deviceName)       
                                                                throw ()
{
    DEBUG_BLOCK
    g_print("MY TEST: GstreamerPlayer :: setAudioDevice !!!!!!!!!!!!!!!!!!!!!!\n");

    debug() << "Using device: " << deviceName << endl;

    if (deviceName.size() == 0) {
        return false;
    }

    m_playContext->setAudioDevice(deviceName);

    return true;
}

