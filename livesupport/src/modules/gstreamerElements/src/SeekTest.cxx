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

#include <string>
#include <iostream>

#include <gst/gst.h>

#include "LiveSupport/GstreamerElements/autoplug.h"
#include "SeekTest.h"


using namespace LiveSupport::GstreamerElements;

/* ===================================================  local data structures */


/* ================================================  local constants & macros */

CPPUNIT_TEST_SUITE_REGISTRATION(SeekTest);

/**
 *  An mp3 test file.
 */
static const char *         mp3File = "var/5seccounter.mp3";

/**
 *  An Ogg Vorbis test file.
 */
static const char *         oggVorbisFile = "var/5seccounter.ogg";

/**
 *  An smil test file.
 */
static const char *         smilFile = "var/simple.smil";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Set up the test environment
 *----------------------------------------------------------------------------*/
void
SeekTest :: setUp(void)                         throw ()
{
}


/*------------------------------------------------------------------------------
 *  Clean up the test environment
 *----------------------------------------------------------------------------*/
void
SeekTest :: tearDown(void)                      throw ()
{
}


/*------------------------------------------------------------------------------
 *  Play an audio file
 *----------------------------------------------------------------------------*/
gint64
SeekTest :: playFile(const char   * audioFile,
                     gint64         seekTo,
                     gint64         playTo)
                                                throw (CPPUNIT_NS::Exception)
{
    GstElement    * pipeline;
    GstElement    * source;
    GstElement    * decoder;
    GstElement    * sink;
    GstCaps       * caps;
    GstMessage    * message;
    GstFormat       format;
    GstState        state;
    gint64          timeStart;
    gint64          timeEnd;
    gboolean        ret;

    /* initialize GStreamer */
    gst_init(0, 0);

    caps = gst_caps_new_simple("audio/x-raw-int",
                               "width", G_TYPE_INT, 16,
                               "depth", G_TYPE_INT, 16,
                               "endianness", G_TYPE_INT, G_BYTE_ORDER,
                               "signed", G_TYPE_BOOLEAN, TRUE,
                               "channels", G_TYPE_INT, 2,
                               "rate", G_TYPE_INT, 44100,
                               NULL);

    /* create elements */
    pipeline = gst_pipeline_new("audio-player");
    source   = gst_element_factory_make("filesrc", "source");
    sink     = gst_element_factory_make("alsasink", "alsa-output");

    g_object_set(G_OBJECT(source), "location", audioFile, NULL);

    decoder = ls_gst_autoplug_plug_source(source, "decoder", caps);

    if (!decoder) {
        gst_object_unref(GST_OBJECT(sink));
        gst_object_unref(GST_OBJECT(source));
        gst_object_unref(GST_OBJECT(pipeline));

        return 0LL;
    }

    gst_bin_add_many(GST_BIN(pipeline), source, decoder, sink, NULL);
    gst_element_link_many(source, decoder, sink, NULL);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // wait until the pipeline has reached the PLAYING state
    do {
        GstState    pending;
        
        message = gst_bus_poll(gst_pipeline_get_bus((GstPipeline*) pipeline),
                            (GstMessageType) (GST_MESSAGE_STATE_CHANGED),
                            -1);

        gst_element_get_state(pipeline,
                              &state,
                              &pending,
                              GST_CLOCK_TIME_NONE);
    } while (state != GST_STATE_PLAYING);

    // send the seek command
    if (playTo > 0) {
        ret = gst_element_seek(pipeline,
                               1.0,
                               GST_FORMAT_TIME,
                               GST_SEEK_FLAG_FLUSH,
                               GST_SEEK_TYPE_SET,
                               seekTo,
                               GST_SEEK_TYPE_SET,
                               playTo);
    } else {
        ret = gst_element_seek(pipeline,
                               1.0,
                               GST_FORMAT_TIME,
                               GST_SEEK_FLAG_FLUSH,
                               GST_SEEK_TYPE_SET,
                               seekTo,
                               GST_SEEK_TYPE_NONE,
                               GST_CLOCK_TIME_NONE);
    }
    CPPUNIT_ASSERT(ret);

    // wait until we actually reach the point we wanted to seek to
    do {
        message = gst_bus_poll(gst_pipeline_get_bus((GstPipeline*) pipeline),
                            (GstMessageType) (GST_MESSAGE_ANY),
                            -1);

        format = GST_FORMAT_TIME;
        gst_element_query_position(pipeline, &format, &timeStart);

    } while (timeStart < seekTo);

    // wait until EOS or an ERROR is reached
    message = gst_bus_poll(gst_pipeline_get_bus((GstPipeline*) pipeline),
                        (GstMessageType) (GST_MESSAGE_ERROR | GST_MESSAGE_EOS),
                        -1);

    format = GST_FORMAT_TIME;
    gst_element_query_position(pipeline, &format, &timeEnd);

    /* clean up nicely */
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT (pipeline));

    return timeEnd - timeStart;
}


/*------------------------------------------------------------------------------
 *  A simple smoke test.
 *----------------------------------------------------------------------------*/
void
SeekTest :: mp3Test(void)
                                                throw (CPPUNIT_NS::Exception)
{
    gint64  timePlayed;
    char    str[256];

    timePlayed = playFile(mp3File, 1LL * GST_SECOND, 4LL * GST_SECOND);
    g_snprintf(str, 256, "time played: %" G_GINT64_FORMAT, timePlayed);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed > 2.9 * GST_SECOND);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed < 3.1 * GST_SECOND);
}


/*------------------------------------------------------------------------------
 *  Seek and play until the end of the auido file.
 *----------------------------------------------------------------------------*/
void
SeekTest :: mp3OpenEndedTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    gint64  timePlayed;
    char    str[256];

    // see http://bugzilla.gnome.org/show_bug.cgi?id=308312
    // as why this seek is not precise
    timePlayed = playFile(mp3File, 2LL * GST_SECOND, -1LL);
    g_snprintf(str, 256, "time played: %" G_GINT64_FORMAT, timePlayed);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed > 2.9 * GST_SECOND);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed < 3.1 * GST_SECOND);
}

/*------------------------------------------------------------------------------
 *  A simple smoke test on an Ogg Vorbis file.
 *----------------------------------------------------------------------------*/
void
SeekTest :: oggVorbisTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    gint64  timePlayed;
    char    str[256];

    timePlayed = playFile(oggVorbisFile, 1LL * GST_SECOND, 4LL * GST_SECOND);
    g_snprintf(str, 256, "time played: %" G_GINT64_FORMAT, timePlayed);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed > 2.9 * GST_SECOND);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed < 3.1 * GST_SECOND);
}


/*------------------------------------------------------------------------------
 *  Seek and play an ogg vorbis file until the end of the auido file.
 *----------------------------------------------------------------------------*/
void
SeekTest :: oggVorbisOpenEndedTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    gint64  timePlayed;
    char    str[256];

    // see http://bugzilla.gnome.org/show_bug.cgi?id=308312
    // as why this seek is not precise
    timePlayed = playFile(oggVorbisFile, 2LL * GST_SECOND, -1LL);
    g_snprintf(str, 256, "time played: %" G_GINT64_FORMAT, timePlayed);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed > 2.9 * GST_SECOND);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed < 3.1 * GST_SECOND);
}


/*------------------------------------------------------------------------------
 *  A simple smoke test on a SMIL file.
 *----------------------------------------------------------------------------*/
void
SeekTest :: smilTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    gint64  timePlayed;
    char    str[256];

    timePlayed = playFile(smilFile, 1LL * GST_SECOND, 4LL * GST_SECOND);
    g_snprintf(str, 256, "time played: %" G_GINT64_FORMAT, timePlayed);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed > 2.9 * GST_SECOND);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed < 3.1 * GST_SECOND);
}


/*------------------------------------------------------------------------------
 *  Seek and play a SMIL file until the end of the auido file.
 *----------------------------------------------------------------------------*/
void
SeekTest :: smilOpenEndedTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    gint64  timePlayed;
    char    str[256];

    // see http://bugzilla.gnome.org/show_bug.cgi?id=308312
    // as why this seek is not precise
    timePlayed = playFile(oggVorbisFile, 2LL * GST_SECOND, -1LL);
    g_snprintf(str, 256, "time played: %" G_GINT64_FORMAT, timePlayed);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed > 2.9 * GST_SECOND);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed < 3.1 * GST_SECOND);
}


