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

    if (m_initialized) {
        return;
    }

    // initialize the gstreamer library
    if (!gst_init_check(0, 0, 0)) {
        throw std::runtime_error("couldn't initialize the gstreamer library");
    }

    // create the pipeline container (threaded)
    m_pipeline   = gst_pipeline_new("audio-player");

    // Set bus event handler
    gst_bus_add_watch(gst_pipeline_get_bus(GST_PIPELINE(m_pipeline)), busEventHandler, this);

    m_filesrc         = 0;
    m_decoder         = 0;
    m_audioconvert    = 0;
    m_audioresample   = 0;

    m_eos = false;

    // TODO: read the caps from the config file
    m_sinkCaps = gst_caps_new_simple("audio/x-raw-int",
                                   "width", G_TYPE_INT, 16,
                                   "depth", G_TYPE_INT, 16,
                                   "endiannes", G_TYPE_INT, G_BYTE_ORDER,
                                   "channels", G_TYPE_INT, 2,
                                   "rate", G_TYPE_INT, 44100,
                                   NULL);

    setAudioDevice(m_audioDevice);

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

    if (m_initialized) {
        gst_element_set_state(m_pipeline, GST_STATE_NULL);

        if (!gst_element_get_parent(m_audiosink)) {
            // delete manually, if audiosink wasn't added to the pipeline
            // for some reason
            gst_object_unref(GST_OBJECT(m_audiosink));
        }
        gst_object_unref(GST_OBJECT(m_pipeline));
        gst_caps_unref(m_sinkCaps);

        m_audiosink   = 0;
        m_initialized = false;
    }
}


/*------------------------------------------------------------------------------
 *  Attach an event listener.
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: attachListener(AudioPlayerEventListener*     eventListener)
                                                                    throw ()
{
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

    GstreamerPlayer* const player = (GstreamerPlayer*) self;

    ListenerVector::iterator    it  = player->m_listeners.begin();
    ListenerVector::iterator    end = player->m_listeners.end();
    while (it != end) {
        (*it)->onStop();
        ++it;
    }

    // false == Don't call this idle function again
    return false;
}


void
GstreamerPlayer :: eventHandler(GstPad*, GstEvent* event, gpointer self) throw()
{
    DEBUG_BLOCK

    GstreamerPlayer* const player = (GstreamerPlayer*) self;

    switch ( static_cast<int>(event->type) )
    {
    case GST_EVENT_EOS:
        if (player->isOpen() && player->isPlaying()) {
            debug() << "EOS reached\n";
            player->m_eos = true;
            // Important: We *must* use an idle function call here, so that the signal handler returns
            // before fireOnStopEvent() is executed.
            g_idle_add(fireOnStopEvent, self);
        }
        break;

    default:
        debug() << "** unknown event " << static_cast<int>(event->type) << endl;
        break;
    }
}


/*------------------------------------------------------------------------------
 * Bus event handler. Processes messages from the pipeline bus. 
 *----------------------------------------------------------------------------*/
gboolean
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


/*------------------------------------------------------------------------------
 * NewPad event handler. Links the decoder after decodebin's autoplugging. 
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer::newpadEventHandler(GstElement*, GstPad* pad, gboolean, gpointer self) throw ()
{
    DEBUG_BLOCK

    GstreamerPlayer* const player = (GstreamerPlayer*) self;
    GstPad* const audiopad = gst_element_get_pad(player->m_audioconvert, "sink");

    if (GST_PAD_IS_LINKED(audiopad)) {
        debug() << "audiopad is already linked. Unlinking old pad." << endl;
        gst_pad_unlink(audiopad, GST_PAD_PEER(audiopad));
    }

    gst_pad_link(pad, audiopad);
}


/*------------------------------------------------------------------------------
 * Preload a file, to speed up the subsequent open() call. 
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: preload(const std::string   fileUrl)
                                                throw (std::invalid_argument,
                                                       std::runtime_error)
{
    DEBUG_BLOCK

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
GstreamerPlayer :: open(const std::string   fileUrl)
                                                throw (std::invalid_argument,
                                                       std::runtime_error)
{
    // GStreamer pipeline overview:
    // filesrc -> decoder -> audioconvert -> audioresample -> audiosink

    DEBUG_BLOCK

    if (isOpen()) {
        close();
    }

    debug() << "Opening URL: " << fileUrl << endl;
    debug() << "Timestamp: " << *TimeConversion::now() << endl;

    std::string filePath;

    if (fileUrl.find("file://") == 0) {
        filePath = fileUrl.substr(7, fileUrl.size());
    }
    else if (fileUrl.find("file:") == 0) {
        filePath = fileUrl.substr(5, fileUrl.size());
    }
    else {
        throw std::invalid_argument("badly formed URL or unsupported protocol");
    }

    if (m_preloadThread) {
        debug() << "Waiting for Preloader thread to finish..." << endl;
        m_preloadThread->join();
    }

    const bool isSmil = fileUrl.substr(fileUrl.size()-5, fileUrl.size()) == ".smil" ? true : false;
    const bool isPreloaded = (m_preloadUrl == fileUrl);

//    if (isSmil) {
//        throw std::runtime_error("SMIL is not yet supported.");
//        return;
//    }

    if (isPreloaded)
        m_filesrc = m_preloadFilesrc;
    else {
        //m_filesrc = gst_element_make_from_uri(GST_URI_SRC, filePath.c_str(), NULL);
        m_filesrc    = gst_element_factory_make("filesrc", "file-source");
        g_object_set(G_OBJECT(m_filesrc), "location", filePath.c_str(), NULL);
    }

    // converts between different audio formats (e.g. bitrate) 
    m_audioconvert    = gst_element_factory_make("audioconvert", NULL);

    // scale the sampling rate, if necessary
    m_audioresample   = gst_element_factory_make("audioresample", NULL);

    // Due to bugs in the minimalaudiosmil element, it does not currently work with decodebin.
    // Therefore we instantiate it manually if the file has the .smil extension. 
    if (isSmil) {
        if (isPreloaded) {
            debug() << "Using preloaded SMIL element instance." << endl;
            m_decoder = m_preloadDecoder;
            gst_element_link(m_decoder, m_audioconvert);
        }
        else {
            debug() << "SMIL file detected." << endl;
            m_stopPreloader = false;
            m_decoder = gst_element_factory_make("minimalaudiosmil", NULL);
            g_object_set(G_OBJECT(m_decoder), "abort", &m_stopPreloader, NULL);
            gst_element_link_many(m_filesrc, m_decoder, m_audioconvert, NULL);
        }
        if (gst_element_get_parent(m_audiosink) == NULL)
            gst_bin_add(GST_BIN(m_pipeline), m_audiosink);
    }
    // Using GStreamer's decodebin autoplugger for everything else
    else {
        m_decoder = gst_element_factory_make("decodebin", NULL);

        GstPad *p;
        p = gst_element_get_pad (m_decoder, "sink");
        if (p) {
            gst_pad_add_event_probe (p, G_CALLBACK(eventHandler), this);
            gst_object_unref (p);
        }

        gst_bin_add_many(GST_BIN(m_pipeline), m_filesrc, m_decoder, NULL);
        gst_element_link(m_filesrc, m_decoder);
        g_signal_connect(m_decoder, "new-decoded-pad", G_CALLBACK(newpadEventHandler), this);
    }

    if (!m_decoder) {
        throw std::runtime_error("GstreamerPlayer: could not create decoder");
    }

    gst_bin_add_many(GST_BIN(m_pipeline), m_audioconvert, m_audioresample, NULL);

    gst_element_link_many(m_audioconvert, m_audioresample, m_audiosink, NULL);

    m_preloadUrl.clear();
    
    if (gst_element_set_state(m_pipeline,GST_STATE_PAUSED) == GST_STATE_CHANGE_FAILURE) {
        close();
        throw std::runtime_error("GstreamerPlayer: could not open file");
    }

    m_eos = false;
}


/*------------------------------------------------------------------------------
 *  Tell if we've been opened.
 *----------------------------------------------------------------------------*/
bool
GstreamerPlayer :: isOpen(void)                                 throw ()
{
    return m_decoder != 0;
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
    gint64                    ns;
    GstFormat                 format = GST_FORMAT_TIME;

    if (!isOpen()) {
        throw std::logic_error("player not open");
    }

    if (m_decoder && gst_element_query_duration(m_pipeline, &format, &ns) && format == GST_FORMAT_TIME) {
        // use microsec, as nanosec() is not found by the compiler (?)
        length.reset(new time_duration(microsec(ns / 1000LL)));
    } else {
        length.reset(new time_duration(microsec(0LL)));
    }

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
    gint64                    ns = 0;

    if (!isOpen()) {
        throw std::logic_error("player not open");
    }
    
    GstFormat fmt = GST_FORMAT_TIME;
    gst_element_query_position(m_pipeline, &fmt, &ns);
    
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

    if (!isOpen()) {
        throw std::logic_error("GstreamerPlayer not opened yet");
    }

    if (!isPlaying()) {
        GstStateChangeReturn ret;
        ret = gst_element_set_state(m_pipeline, GST_STATE_PLAYING);

        if (ret == GST_STATE_CHANGE_FAILURE )
            error() << "State change to PLAYING failed." << endl; 
    }
    else
        error() << "Already playing!" << endl;
}


/*------------------------------------------------------------------------------
 *  Pause the player
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: pause(void)                      throw (std::logic_error)
{
    DEBUG_BLOCK

    if (isPlaying()) {
        gst_element_set_state(m_pipeline, GST_STATE_PAUSED);
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
        gst_element_set_state(m_pipeline, GST_STATE_READY);
        return false;
    }
        
    GstState state;
    GstState pending;

    gst_element_get_state(m_pipeline, &state, &pending, 50000000);

    return state == GST_STATE_PLAYING;
}


/*------------------------------------------------------------------------------
 *  Stop playing
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: stop(void)                       throw (std::logic_error)
{
    DEBUG_BLOCK

    if (!isOpen()) {
        throw std::logic_error("GstreamerPlayer not opened yet");
    }

    if (isPlaying()) {
        gst_element_set_state(m_pipeline, GST_STATE_READY);
    }
}
 

/*------------------------------------------------------------------------------
 *  Close the currently opened audio file.
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: close(void)                       throw (std::logic_error)
{
    DEBUG_BLOCK

    if (isPlaying()) {
        stop();
    }

    gst_element_set_state(m_pipeline, GST_STATE_NULL);

    // Unlink elements:
    if (m_filesrc && m_decoder) {
        gst_element_unlink(m_filesrc, m_decoder);
    }
    if (m_decoder && m_audioconvert) {
        gst_element_unlink(m_decoder, m_audioconvert);
    }
    if (m_audioconvert && m_audioresample ) {
        gst_element_unlink(m_audioconvert, m_audioresample);
    }
    if (m_audioresample && m_audiosink) {
        gst_element_unlink(m_audioresample, m_audiosink);
    }

    // Remove elements from pipeline:
    if (m_audioresample) {
        gst_bin_remove(GST_BIN(m_pipeline), m_audioresample);
    }
    if (m_audioconvert) {
        gst_bin_remove(GST_BIN(m_pipeline), m_audioconvert);
    }
    if (m_decoder) {
        gst_bin_remove(GST_BIN(m_pipeline), m_decoder);
    }
    if (m_filesrc) {
        gst_bin_remove(GST_BIN(m_pipeline), m_filesrc);
    }
    //if (m_audiosink && gst_element_get_parent(m_audiosink) == GST_OBJECT(m_pipeline)) {
    //    gst_object_ref(GST_OBJECT(m_audiosink));
    //    gst_bin_remove(GST_BIN(m_pipeline), m_audiosink);
    //}

    m_filesrc         = 0;
    m_decoder         = 0;
    m_audioconvert    = 0;
    m_audioresample   = 0;
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

    debug() << "Using device: " << deviceName << endl;

    if (deviceName.size() == 0) {
        return false;
    }

    const bool oss = deviceName.find("/dev") == 0;

    if (m_audiosink) {
        debug() << "Destroying old sink." << endl;
        if (m_audioresample) {
            gst_element_unlink(m_audioresample, m_audiosink);
        }
        if (gst_element_get_parent(m_audiosink) == NULL)
            gst_object_unref(GST_OBJECT(m_audiosink));
        else
            gst_bin_remove(GST_BIN(m_pipeline), m_audiosink);
        m_audiosink = 0;
    }

    if (!m_audiosink) {
        m_audiosink = (oss ? gst_element_factory_make("osssink", "osssink") : gst_element_factory_make("alsasink", "alsasink"));
        gst_bin_add(GST_BIN(m_pipeline), m_audiosink);
    }
    if (!m_audiosink) {
        return false;
    }

    // it's the same property, "device" for both alsasink and osssink
    g_object_set(G_OBJECT(m_audiosink), "device", deviceName.c_str(), NULL);

    if (m_audioresample) {
        //gst_element_link_filtered(m_audioresample, m_audiosink, m_sinkCaps);
        gst_element_link(m_audioresample, m_audiosink);
    }

    return true;
}



//////////////////////////////////////////////////////////////////////////////
// CLASS Preloader
//////////////////////////////////////////////////////////////////////////////

Preloader::Preloader(GstreamerPlayer* player, const std::string url) throw()
    : RunnableInterface()
    , m_player(player)
    , m_fileUrl(url)
{
    DEBUG_FUNC_INFO

    player->m_stopPreloader = false;
}


Preloader::~Preloader() throw()
{
    DEBUG_FUNC_INFO
}


void Preloader::run() throw()
{
#if 0
    DEBUG_BLOCK

    GstreamerPlayer* const p = m_player;
    const std::string fileUrl(m_fileUrl);

    const bool isSmil = fileUrl.substr(fileUrl.size()-5, fileUrl.size()) == ".smil" ? true : false;
    if (!isSmil)
        return;

    debug() << "Preloading SMIL file: " << fileUrl << endl;

    std::string filePath;

    if (fileUrl.find("file://") == 0) {
        filePath = fileUrl.substr(7, fileUrl.size());
    }
    else if (fileUrl.find("file:") == 0) {
        filePath = fileUrl.substr(5, fileUrl.size());
    }
    else {
        return;
    }

    if (!p->m_preloadUrl.empty()) {
        p->m_preloadUrl.clear();
        g_object_unref(G_OBJECT(p->m_preloadFilesrc));
        g_object_unref(G_OBJECT(p->m_preloadDecoder));
    }

    p->m_preloadFilesrc = gst_element_factory_make("filesrc", NULL);
    g_object_set(G_OBJECT(p->m_preloadFilesrc), "location", filePath.c_str(), NULL);

    p->m_preloadDecoder = gst_element_factory_make("minimalaudiosmil", NULL);
    g_object_set(G_OBJECT(p->m_preloadDecoder), "abort", &p->m_stopPreloader, NULL);

    GstElement* pipe     = gst_pipeline_new("pipe");
    GstElement* fakesink = gst_element_factory_make("fakesink", "fakesink");
    gst_element_link_many(p->m_preloadFilesrc, p->m_preloadDecoder, fakesink, NULL);
    gst_bin_add_many(GST_BIN(pipe), p->m_preloadFilesrc, p->m_preloadDecoder, fakesink, NULL);

    gst_element_set_state(pipe, GST_STATE_PLAYING);

    gint64 position = 0LL;
    while (position == 0LL && !p->m_stopPreloader && gst_bin_iterate(GST_BIN(pipe))) {
        GstFormat   format = GST_FORMAT_DEFAULT;
        gst_element_query(fakesink, GST_QUERY_POSITION, &format, &position);
    }

    gst_element_set_state(pipe, GST_STATE_PAUSED);

    if (p->m_stopPreloader) {
        debug() << "Aborting preloader, per request." << endl;
        g_object_unref(G_OBJECT(p->m_preloadFilesrc));
        g_object_unref(G_OBJECT(p->m_preloadDecoder));
        return;
    }

    g_object_ref(G_OBJECT(p->m_preloadFilesrc));
    g_object_ref(G_OBJECT(p->m_preloadDecoder));
    gst_bin_remove_many(GST_BIN(pipe), p->m_preloadFilesrc, p->m_preloadDecoder, NULL);
    gst_element_unlink(p->m_preloadFilesrc, fakesink);
    gst_object_unref(GST_OBJECT(pipe));
   
    p->m_preloadUrl = fileUrl;

    p->m_preloadThread.reset();
#endif
}


void Preloader::signal(int) throw()
{}


void Preloader::stop() throw()
{
    DEBUG_FUNC_INFO

    m_player->m_stopPreloader = true;
}


