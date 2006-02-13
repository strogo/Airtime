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

#include "LiveSupport/Core/TimeConversion.h"
#include "LiveSupport/GstreamerElements/autoplug.h"
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
    if (element.get_name() != configElementNameStr) {
        std::string eMsg = "Bad configuration element ";
        eMsg += element.get_name();
        throw std::invalid_argument(eMsg);
    }

    const xmlpp::Attribute    * attribute = 0;

    if ((attribute = element.get_attribute(audioDeviceName))) {
        audioDevice = attribute->get_value();
    }
}


/*------------------------------------------------------------------------------
 *  Initialize the Audio Player
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: initialize(void)                 throw (std::exception)
{
    if (initialized) {
        return;
    }

    // initialize the gstreamer library
    if (!gst_init_check(0, 0)) {
        throw std::runtime_error("couldn't initialize the gstreamer library");
    }

    // initialize the pipeline
    pipeline   = gst_thread_new("audio-player");
    // take ownership of the pipeline object
    gst_object_ref(GST_OBJECT(pipeline));
    gst_object_sink(GST_OBJECT(pipeline));

    g_signal_connect(pipeline, "error", G_CALLBACK(errorHandler), this);

    // TODO: read the caps from the config file
    sinkCaps = gst_caps_new_simple("audio/x-raw-int",
                                   "width", G_TYPE_INT, 16,
                                   "depth", G_TYPE_INT, 16,
                                   "endiannes", G_TYPE_INT, G_BYTE_ORDER,
                                   "channels", G_TYPE_INT, 2,
                                   "rate", G_TYPE_INT, 44100,
                                   NULL);

    setAudioDevice(audioDevice);

    // set up other variables
    initialized = true;
}


/*------------------------------------------------------------------------------
 *  Handler for gstreamer errors.
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: errorHandler(GstElement   * pipeline,
                                GstElement   * source,
                                GError       * error,
                                gchar        * debug,
                                gpointer       self)
                                                                throw ()
{
    // TODO: handle error
    std::cerr << "gstreamer error: " << error->message << std::endl;
}


/*------------------------------------------------------------------------------
 *  De-initialize the Gstreamer Player
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: deInitialize(void)                       throw ()
{
    if (initialized) {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_bin_sync_children_state(GST_BIN(pipeline));

        if (!gst_element_get_parent(audiosink)) {
            // delete manually, if audiosink wasn't added to the pipeline
            // for some reason
            gst_object_unref(GST_OBJECT(audiosink));
        }
        gst_object_unref(GST_OBJECT(pipeline));
        gst_caps_free(sinkCaps);

        audiosink   = 0;
        initialized = false;
    }
}


/*------------------------------------------------------------------------------
 *  Attach an event listener.
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: attachListener(AudioPlayerEventListener*     eventListener)
                                                                    throw ()
{
    listeners.push_back(eventListener);
}


/*------------------------------------------------------------------------------
 *  Detach an event listener.
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: detachListener(AudioPlayerEventListener*     eventListener)
                                                throw (std::invalid_argument)
{
    ListenerVector::iterator    it  = listeners.begin();
    ListenerVector::iterator    end = listeners.end();

    while (it != end) {
        if (*it == eventListener) {
            listeners.erase(it);
            return;
        }
        ++it;
    }

    throw std::invalid_argument("supplied event listener not found");
}


/*------------------------------------------------------------------------------
 *  Send the onStop event to all attached listeners.
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: fireOnStopEvent(void)                        throw ()
{
    ListenerVector::iterator    it  = listeners.begin();
    ListenerVector::iterator    end = listeners.end();

    while (it != end) {
        (*it)->onStop();
        ++it;
    }
}


/*------------------------------------------------------------------------------
 *  An EOS event handler, that will put the pipeline to EOS as well.
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: eosEventHandler(GstElement    * element,
                                   gpointer        self)
                                                                throw ()
{
    GstreamerPlayer   * player = (GstreamerPlayer*) self;

    gst_element_set_eos(player->pipeline);
    player->fireOnStopEvent();
}


/*------------------------------------------------------------------------------
 *  Specify which file to play
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: open(const std::string   fileUrl)
                                                throw (std::invalid_argument,
                                                       std::runtime_error)
{
    std::string     filePath;
    GstElement    * pipe;
    GstElement    * fakesink;
    gint64          position;

    if (isOpen()) {
        close();
    }

    if (fileUrl.find("file:") == 0) {
        filePath = fileUrl.substr(5, fileUrl.size());
    } else if (fileUrl.find("file://") == 0) {
        filePath = fileUrl.substr(7, fileUrl.size());
    } else {
        throw std::invalid_argument("badly formed URL or unsupported protocol");
    }

    g_object_ref(G_OBJECT(audiosink));
    gst_bin_remove(GST_BIN(pipeline), audiosink);

    filesrc    = gst_element_factory_make("filesrc", "file-source");
    g_object_set(G_OBJECT(filesrc), "location", filePath.c_str(), NULL);

    decoder = ls_gst_autoplug_plug_source(filesrc, "decoder", sinkCaps);

    if (!decoder) {
        throw std::invalid_argument(std::string("can't open URL ") + fileUrl);
    }

    // connect the decoder unto a fakesink, and iterate on it until the
    // first bytes come out. this is to make sure that _really_ all
    // initialiation is done at opening
    pipe     = gst_pipeline_new("pipe");
    fakesink = gst_element_factory_make("fakesink", "fakesink");
    g_object_ref(G_OBJECT(filesrc));
    g_object_ref(G_OBJECT(decoder));
    gst_element_link_many(decoder, fakesink, NULL);
    gst_bin_add_many(GST_BIN(pipe), filesrc, decoder, fakesink, NULL);

    gst_element_set_state(pipe, GST_STATE_PAUSED);
    gst_element_set_state(pipe, GST_STATE_PLAYING);

    position = 0LL;
    while (position == 0LL && gst_bin_iterate(GST_BIN(pipe))) {
        GstFormat   format = GST_FORMAT_DEFAULT;
        gst_element_query(fakesink, GST_QUERY_POSITION, &format, &position);
    }

    gst_element_set_state(pipe, GST_STATE_PAUSED);
    gst_bin_remove_many(GST_BIN(pipe), filesrc, decoder, NULL);
    gst_element_unlink(decoder, fakesink);
    gst_object_unref(GST_OBJECT(pipe));

    // connect the decoder to the real audio sink
    gst_element_link(decoder, audiosink);
    gst_bin_add_many(GST_BIN(pipeline), filesrc, decoder, audiosink, NULL);
    // connect the eos signal handler
    g_signal_connect(decoder, "eos", G_CALLBACK(eosEventHandler), this);

    if (gst_element_set_state(pipeline,GST_STATE_PAUSED) == GST_STATE_FAILURE) {
        close();
        // the error is most probably caused by not being able to open
        // the audio device (as it might be blocked by an other process
        throw std::runtime_error("can't open audio device " + audioDevice);
    }
    gst_bin_sync_children_state(GST_BIN(pipeline));
}


/*------------------------------------------------------------------------------
 *  Tell if we've been opened.
 *----------------------------------------------------------------------------*/
bool
GstreamerPlayer :: isOpen(void)                                 throw ()
{
    return decoder != 0;
}


/*------------------------------------------------------------------------------
 *  Get the length of the current audio clip.
 *----------------------------------------------------------------------------*/
Ptr<time_duration>::Ref
GstreamerPlayer :: getPlaylength(void)              throw (std::logic_error)
{
    Ptr<time_duration>::Ref   length;
    gint64                    ns;
    GstFormat                 format = GST_FORMAT_TIME;

    if (!isOpen()) {
        throw std::logic_error("player not open");
    }

    if (decoder
     && gst_element_query(decoder, GST_QUERY_TOTAL, &format, &ns)
     && format == GST_FORMAT_TIME) {

        // use microsec, as nanosec() is not found by the compiler (?)
        length.reset(new time_duration(microsec(ns / 1000LL)));
    } else {
        length.reset(new time_duration(microsec(0LL)));
    }

    return length;
}


/*------------------------------------------------------------------------------
 *  Get the current position of the current audio clip.
 *----------------------------------------------------------------------------*/
Ptr<time_duration>::Ref
GstreamerPlayer :: getPosition(void)                throw (std::logic_error)
{
    Ptr<time_duration>::Ref   length;
    gint64                    ns;

    if (!isOpen()) {
        throw std::logic_error("player not open");
    }

    ns = ls_gst_autoplug_get_position(decoder);
    length.reset(new time_duration(microsec(ns / 1000LL)));

    return length;
}


/*------------------------------------------------------------------------------
 *  Start playing
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: start(void)                      throw (std::logic_error)
{
    if (!isOpen()) {
        throw std::logic_error("GstreamerPlayer not opened yet");
    }

    if (!isPlaying()) {
        gst_element_set_state(audiosink, GST_STATE_PAUSED);
        gst_element_set_state(pipeline, GST_STATE_PLAYING);
    }
}


/*------------------------------------------------------------------------------
 *  Pause the player
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: pause(void)                      throw (std::logic_error)
{
    if (isPlaying()) {
        gst_element_set_state(pipeline, GST_STATE_PAUSED);
    }
}


/*------------------------------------------------------------------------------
 *  Tell if we're playing
 *----------------------------------------------------------------------------*/
bool
GstreamerPlayer :: isPlaying(void)                  throw ()
{
    return gst_element_get_state(pipeline) == GST_STATE_PLAYING;
}


/*------------------------------------------------------------------------------
 *  Stop playing
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: stop(void)                       throw (std::logic_error)
{
    if (!isOpen()) {
        throw std::logic_error("GstreamerPlayer not opened yet");
    }

    if (isPlaying()) {
        gst_element_set_state(pipeline, GST_STATE_READY);
    }
}
 

/*------------------------------------------------------------------------------
 *  Close the currently opened audio file.
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: close(void)                       throw ()
{
    if (isPlaying()) {
        stop();
    }

    gst_element_set_state(pipeline, GST_STATE_NULL);
    if (filesrc && decoder) {
        gst_element_unlink(filesrc, decoder);
    }
    if (decoder && audiosink) {
        gst_element_unlink(decoder, audiosink);
    }
    if (decoder) {
        gst_bin_remove(GST_BIN(pipeline), decoder);
    }
    if (filesrc) {
        gst_bin_remove(GST_BIN(pipeline), filesrc);
    }
    filesrc = 0;
    decoder = 0;
}


/*------------------------------------------------------------------------------
 *  Get the volume of the player.
 *----------------------------------------------------------------------------*/
unsigned int
GstreamerPlayer :: getVolume(void)                                  throw ()
{
    return 0;
}


/*------------------------------------------------------------------------------
 *  Set the volume of the player.
 *----------------------------------------------------------------------------*/
void
GstreamerPlayer :: setVolume(unsigned int   volume)                 throw ()
{
}


/*------------------------------------------------------------------------------
 *  Set the audio device.
 *----------------------------------------------------------------------------*/
bool
GstreamerPlayer :: setAudioDevice(const std::string &deviceName)       
                                                                throw ()
{
    bool    oss    = deviceName.find("/dev") == 0;
    bool    relink = false;

    if (deviceName.size() == 0) {
        return false;
    }

    if (audiosink) {
        bool    oldOss = g_strrstr(gst_element_get_name(audiosink), "osssink");
        relink = oss && !oldOss;
    }

    if (relink && audiosink) {
        if (decoder) {
            gst_element_unlink(decoder, audiosink);
        }
        gst_bin_remove(GST_BIN(pipeline), audiosink);
        // FIXME: why unref here? remove should unref already
        g_object_unref(G_OBJECT(audiosink));
        audiosink = 0;
    }

    if (!audiosink) {
        audiosink = oss
                  ? gst_element_factory_make("osssink", "osssink")
                  : gst_element_factory_make("alsasink", "alsasink");
        relink = true;
    }
    if (!audiosink) {
        return false;
    }

    // it's the same property, "device" for both alsasink and osssink
    g_object_set(G_OBJECT(audiosink), "device", deviceName.c_str(), NULL);

    if (relink) {
        if (decoder) {
            gst_element_link_filtered(decoder, audiosink, sinkCaps);
        }
        gst_bin_add(GST_BIN(pipeline), audiosink);
    }

    return true;
}

