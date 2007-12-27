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
#ifndef GstreamerPlayContext_h
#define GstreamerPlayContext_h

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#include <gst/gst.h>
#include "SmilHandler.h"

/*------------------------------------------------------------------------------
*  User of this class must implement my_bus_callback function to receive bus callbacks.
*----------------------------------------------------------------------------*/
static gboolean
my_bus_callback (GstBus     *bus,
         GstMessage *message,
         gpointer    data);

/**
 *  A class to play audio files using Gstreamer library.
 *
 *  Usage sequence:
 *
 *  create an instance of GstreamerPlayContext object
 *  call setParentData to provide data to be returned by my_bus_callback
 *  call setAudioDevice on it
 *  call openSource on it
 *  call playContext on it
 *
 *  when done, call closeContext on it
 *  destroy instance
 *
 *  @author  $Author$
 *  @version $Revision$
 */
class GstreamerPlayContext
{
    GstElement *m_pipeline;
    GstElement *m_source;
    GstElement *m_decoder;
    GstElement *m_sink;
    gpointer m_data;
    AudioDescription *m_audioDescription;

public:

    GstreamerPlayContext(){
        m_pipeline = NULL;
        m_source = NULL;
        m_decoder = NULL;
        m_sink = NULL;
        m_data = NULL;
        m_audioDescription = NULL;
    }

    ~GstreamerPlayContext(){
    }

    void closeContext(){
        if(m_pipeline != NULL){
            gst_element_set_state (m_pipeline, GST_STATE_NULL);
            gst_bin_remove (GST_BIN (m_pipeline), m_sink);
            m_sink=NULL;
            gst_object_unref(GST_OBJECT(m_pipeline));
            m_pipeline = NULL;
            if(m_audioDescription != NULL){
                delete m_audioDescription;
                m_audioDescription = NULL;
            }
        }
    }

    void playContext(){
        gst_element_set_state (m_pipeline, GST_STATE_PLAYING);
    }

    void pauseContext(){
        gst_element_set_state (m_pipeline, GST_STATE_PAUSED);
    }

    void stopContext(){
        gst_element_set_state (m_pipeline, GST_STATE_READY);
    }

    void setParentData(gpointer data){
        m_data = data;
    }
    /*------------------------------------------------------------------------------
    *  Set the audio device.
    *----------------------------------------------------------------------------*/
    bool setAudioDevice(const std::string &deviceName) throw() {
        if(m_sink!=NULL){
            return false;//sink can only be created once per instance
        }

        GstPad *audiopad;

        const bool oss = deviceName.find("/dev") == 0;

        m_sink = gst_bin_new ("audiobin");
        GstElement *conv = gst_element_factory_make ("audioconvert", "aconv");
        audiopad = gst_element_get_pad (conv, "sink");

        GstElement *sink = (oss ? gst_element_factory_make("osssink", NULL) : gst_element_factory_make("alsasink", NULL));
        g_object_set(G_OBJECT(sink), "device", deviceName.c_str(), NULL);

        gst_bin_add_many (GST_BIN (m_sink), conv, sink, NULL);
        gst_element_link (conv, sink);
        gst_element_add_pad (m_sink, gst_ghost_pad_new ("sink", audiopad));
        gst_object_unref (audiopad);
    }
    /*------------------------------------------------------------------------------
    *  Set the audio device.
    *----------------------------------------------------------------------------*/
    GstElement* getAudioBin() {
        return m_sink;
    }
    /*------------------------------------------------------------------------------
    *  Opens source element for the file name given.
    *----------------------------------------------------------------------------*/
    bool openSource(const gchar *fileUri) throw() {
        if(m_sink!=NULL || m_pipeline != NULL){
            closeContext();
        }
        m_source = gst_element_make_from_uri (GST_URI_SRC, fileUri, NULL);
//         m_source = gst_element_factory_make ("filesrc", NULL);
//         g_object_set (G_OBJECT (m_source), "location", fileUri, NULL);

        if(m_sink == NULL){
            setAudioDevice("default");
        }
        prepareDecodebin();
        preparePipeline();
        return true;
    }
    /*------------------------------------------------------------------------------
    *  Opens source element for the file name given.
    *----------------------------------------------------------------------------*/
    bool openSource(AudioDescription *audioDescription) throw() {
        if(audioDescription == NULL) return false;
        m_audioDescription = audioDescription;
        return openSource(m_audioDescription->m_src);
    }
    /*------------------------------------------------------------------------------
    *  Returns current stream's duration.
    *----------------------------------------------------------------------------*/
    gint64 getPlayLength() throw() {
        gint64 ns = 0LL;
        if(m_pipeline!=NULL){
            GstFormat format = GST_FORMAT_TIME;
            gst_element_query_duration(m_pipeline, &format, &ns);
            if(format != GST_FORMAT_TIME){
                ns = 0LL;
            }
        }
        return ns;
    }
    /*------------------------------------------------------------------------------
    *  Returns current stream's position.
    *----------------------------------------------------------------------------*/
    gint64 getPosition() throw() {
        gint64 ns = 0LL;
        if(m_pipeline!=NULL){
            GstFormat format = GST_FORMAT_TIME;
            gst_element_query_position(m_pipeline, &format, &ns);
            if(format != GST_FORMAT_TIME){
                ns = 0LL;
            }
        }
        return ns;
    }
    /*------------------------------------------------------------------------------
    *  Checks if the stream is currently playing.
    *----------------------------------------------------------------------------*/
    bool isPlaying() throw() {
        GstState state;
        GstState pending;
        gst_element_get_state(m_pipeline, &state, &pending, 50000000);
        return state == GST_STATE_PLAYING;
    }

private:

    /*------------------------------------------------------------------------------
    *  Prepare empty pipeline.
    *----------------------------------------------------------------------------*/
    bool preparePipeline(){
        if(m_pipeline!=NULL){
            return false;//pipeline can only be created once per instance
        }
        m_pipeline = gst_pipeline_new ("pipeline");

        GstBus *bus = gst_pipeline_get_bus (GST_PIPELINE (m_pipeline));
        gst_bus_add_watch (bus, my_bus_callback, m_data);
        gst_object_unref (bus);
        //link up all elements in the pipeline
        gst_bin_add_many (GST_BIN (m_pipeline), m_source, m_decoder, NULL);
        gst_element_link (m_source, m_decoder);
        gst_bin_add (GST_BIN (m_pipeline), m_sink);
    }
    /*------------------------------------------------------------------------------
    *  Prepare decode bin.
    *----------------------------------------------------------------------------*/
    bool prepareDecodebin(){
        if(m_pipeline!=NULL){
            return false;//pipeline can only be created once per instance
        }
        m_decoder = gst_element_factory_make ("decodebin", NULL);
        g_signal_connect (m_decoder, "new-decoded-pad", G_CALLBACK (cb_newpad), this);
    }
    /*------------------------------------------------------------------------------
    *  New pad signal callback.
    *----------------------------------------------------------------------------*/
    static void cb_newpad (GstElement *decodebin, GstPad *pad, gboolean last, gpointer data)
    {
        GstCaps *caps;
        GstStructure *str;
        GstPad *audiopad;
        audiopad = gst_element_get_pad (((GstreamerPlayContext*)data)->getAudioBin(), "sink");
        if (GST_PAD_IS_LINKED (audiopad)) {
            g_object_unref (audiopad);
            return;
        }
        caps = gst_pad_get_caps (pad);
        str = gst_caps_get_structure (caps, 0);
        if (!g_strrstr (gst_structure_get_name (str), "audio")) {
            gst_caps_unref (caps);
            gst_object_unref (audiopad);
            return;
        }
        gst_caps_unref (caps);
        gst_pad_link (pad, audiopad);
    }
};



#endif // GstreamerPlayContext_h
