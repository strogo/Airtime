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
#include "SwitcherTest.h"


using namespace LiveSupport::GstreamerElements;

/* ===================================================  local data structures */


/* ================================================  local constants & macros */

CPPUNIT_TEST_SUITE_REGISTRATION(SwitcherTest);

/**
 *  Number of nanoseconds per second, in floating point format.
 */
#define NSEC_PER_SEC_FLOAT  1000000000.0

/**
 *  An mp3 test file.
 */
static const char *         mp3File = "var/5seccounter.mp3";

/**
 *  An ogg vorbis test file.
 */
static const char *         oggVorbisFile = "var/5seccounter.ogg";

/**
 *  A SMIL test file.
 */
static const char *         smilFile = "var/simple.smil";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Set up the test environment
 *----------------------------------------------------------------------------*/
void
SwitcherTest :: setUp(void)                         throw ()
{
}


/*------------------------------------------------------------------------------
 *  Clean up the test environment
 *----------------------------------------------------------------------------*/
void
SwitcherTest :: tearDown(void)                      throw ()
{
}


/*------------------------------------------------------------------------------
 *  Set all pads of an element flushing
 *----------------------------------------------------------------------------*/
static void
set_flushing(GstElement   * element)
{
    GstIterator   * it   = gst_element_iterate_pads(element);
    gboolean        done = FALSE;
    gpointer        item;

    while (!done) {
        switch (gst_iterator_next(it, &item)) {
            case GST_ITERATOR_OK: {
                GstPad    * pad = (GstPad*) item;
                GST_PAD_SET_FLUSHING(pad);
            } break;

            case GST_ITERATOR_RESYNC:
                gst_iterator_resync(it);
                break;

            case GST_ITERATOR_ERROR:
            case GST_ITERATOR_DONE:
            default:
                done = TRUE;
        }
    }

    gst_iterator_free(it);
}


/*------------------------------------------------------------------------------
 *  Play an audio file
 *----------------------------------------------------------------------------*/
gint64
SwitcherTest :: playFiles(const char     ** audioFiles,
                          unsigned int      noFiles,
                          const char      * sourceConfig)
                                                throw (CPPUNIT_NS::Exception)
{
    GstElement    * pipeline;
    GstElement    * switcher;
    GstElement    * sink;
    GstCaps       * caps;
    unsigned int    i;
    GstFormat       format;
    gint64          timePlayed;
    GstMessage    * message;

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
    switcher = gst_element_factory_make("switcher", "switcher");
    sink     = gst_element_factory_make("alsasink", "alsa-output");

    gst_bin_add_many(GST_BIN(pipeline), switcher, sink, NULL);
    gst_element_link(switcher, sink);

    g_object_set(G_OBJECT(switcher), "source-config", sourceConfig, NULL);

    for (i = 0; i < noFiles; ++i) {
        GstElement    * source;
        GstElement    * decoder;
        char            str[256];
        gboolean        ret;

        g_snprintf(str, 256, "source_%d", i);
        source   = gst_element_factory_make("filesrc", str);
        CPPUNIT_ASSERT(source);
        g_object_set(G_OBJECT(source), "location", audioFiles[i], NULL);

        g_snprintf(str, 256, "decoder_%d", i);
        decoder = ls_gst_autoplug_plug_source(source, str, caps);
        //decoder = gst_element_factory_make("mad", str);
        CPPUNIT_ASSERT(decoder);

        gst_bin_add_many(GST_BIN(pipeline), source, decoder, NULL);

        ret = gst_element_link_many(source, decoder, switcher, NULL);
        CPPUNIT_ASSERT(ret);
    }

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // this call will block until either an error or eos message comes
    message = gst_bus_poll(gst_pipeline_get_bus((GstPipeline*) pipeline),
                        (GstMessageType) (GST_MESSAGE_ERROR | GST_MESSAGE_EOS),
                        -1);

    g_printerr("got message: %s\n",
               gst_message_type_get_name(GST_MESSAGE_TYPE(message)));

    //gst_xml_write_file(pipeline, stdout);

    format = GST_FORMAT_TIME;
    gst_element_query_position(pipeline, &format, &timePlayed);

    /* clean up nicely */
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(pipeline));

    return timePlayed;
}


static GMutex     * mutex;
static GCond      * cond;


static void
block_callback(GstPad     * pad,
               gboolean     blocked,
               gpointer     userData)
{
    /* don't do anything */
    g_printerr("switcher test block_callback on %s:%s: %d\n",
               gst_element_get_name(gst_pad_get_parent_element(pad)),
               gst_pad_get_name(pad),
               blocked);

    g_mutex_lock(mutex);
    if (cond != NULL) {
        g_cond_signal(cond);
    }
    g_mutex_unlock(mutex);
}



/*------------------------------------------------------------------------------
 *  Play some silence, than a part of the specificed file.
 *
 *  create the following element structure:
 *
 *  audiotestsrc -------------\
 *                            | -- switcher -- alsasink
 *  source -- decoder --------/
 *----------------------------------------------------------------------------*/
gint64
SwitcherTest :: playSilenceThenFile(const char    * audioFile,
                                    gint64          silenceDuration,
                                    gint64          startTime,
                                    gint64          endTime)
                                                throw (CPPUNIT_NS::Exception)
{
    GstElement    * pipeline;
    GstElement    * switcher;
    GstElement    * silence;
    GstElement    * source;
    GstElement    * decoder;
    GstElement    * sink;
    GstPad        * pad;
    GstCaps       * caps;
    char            str[256];
    GstFormat       format;
    gint64          timePlayed;
    GstMessage    * message;
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
    switcher = gst_element_factory_make("switcher", "switcher");
    sink     = gst_element_factory_make("alsasink", "alsa-output");

    gst_bin_add_many(GST_BIN(pipeline), switcher, sink, NULL);
    gst_element_link(switcher, sink);

    /* set the source config, according to the input */
    if (endTime >= 0) {
        g_snprintf(str, 256, "0[%lfs];1[%lfs]",
                             silenceDuration / NSEC_PER_SEC_FLOAT,
                             (endTime - startTime) / NSEC_PER_SEC_FLOAT);
    } else {
        g_snprintf(str, 256, "0[%lfs];1[]",
                             silenceDuration / NSEC_PER_SEC_FLOAT);
    }
    g_object_set(G_OBJECT(switcher), "source-config", str, NULL);

    /* put a silence there up front, before the audio files */
    silence = gst_element_factory_make("audiotestsrc", "silence");
    /* wave 4 is for silence */
    g_object_set(G_OBJECT(silence), "wave", 4, NULL);
    gst_bin_add(GST_BIN(pipeline), silence);
    gst_element_link(silence, switcher);

    /* create the input file */
    source   = gst_element_factory_make("filesrc", "source");
    CPPUNIT_ASSERT(source);
    g_object_set(G_OBJECT(source), "location", audioFile, NULL);

    /* create the decoder */
    decoder = ls_gst_autoplug_plug_source(source, "decoder", caps);
    CPPUNIT_ASSERT(decoder);
    gst_bin_add_many(GST_BIN(pipeline), source, decoder, NULL);
    ret = gst_element_link_many(source, decoder, switcher, NULL);
    CPPUNIT_ASSERT(ret);

    mutex = g_mutex_new();
    cond  = g_cond_new();

    g_mutex_lock(mutex);

    /* block on the switcher's src pad */
    pad = gst_element_get_pad(switcher, "src");
    gst_pad_set_blocked_async(pad, TRUE, block_callback, 0);

    gst_element_set_state(pipeline, GST_STATE_PAUSED);

    g_cond_wait(cond, mutex);
    g_cond_free(cond);

    gst_xml_write_file(decoder, stdout);

    /* do the seek on the decoder */
#if 0
    ret = ls_gst_autoplug_seek(decoder,
                               1.0,
                               GST_FORMAT_TIME,
                               GST_SEEK_FLAG_FLUSH,
                               GST_SEEK_TYPE_SET,
                               startTime,
                               GST_SEEK_TYPE_SET,
                               endTime);
    CPPUNIT_ASSERT(ret);
    ret = gst_element_seek(decoder,
                           1.0,
                           GST_FORMAT_TIME,
                           GST_SEEK_FLAG_FLUSH,
                           GST_SEEK_TYPE_SET,
                           startTime,
                           GST_SEEK_TYPE_SET,
                           endTime);
    CPPUNIT_ASSERT(ret);
#endif

#if 0
    /* send a seek event */
    ret = gst_element_seek(pipeline,
                           1.0,
                           GST_FORMAT_TIME,
                           GST_SEEK_FLAG_FLUSH,
                           GST_SEEK_TYPE_SET,
                           1LL * GST_SECOND,
                           GST_SEEK_TYPE_SET,
                           3LL * GST_SECOND);
    CPPUNIT_ASSERT(ret);
#endif

    /* now that prerolling is done, unblock the switcher's src pad */
    cond = g_cond_new();
    gst_pad_set_blocked_async(pad, FALSE, block_callback, 0);
    g_cond_wait(cond, mutex);
    g_cond_free(cond);
    cond = NULL;

    g_mutex_unlock(mutex);

    g_printerr("after all locking...\n");

    /* sleep a second */
    struct timespec   ts;
    ts.tv_sec  = 1;
    ts.tv_nsec = 0;
    nanosleep(&ts, NULL);

    ret = ls_gst_autoplug_seek(decoder,
                               1.0,
                               GST_FORMAT_TIME,
                               GST_SEEK_FLAG_FLUSH,
                               GST_SEEK_TYPE_SET,
                               startTime,
                               GST_SEEK_TYPE_SET,
                               endTime);
    CPPUNIT_ASSERT(ret);

    //gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // this call will block until either an error or eos message comes
    message = gst_bus_poll(gst_pipeline_get_bus((GstPipeline*) pipeline),
                        (GstMessageType) (GST_MESSAGE_ERROR | GST_MESSAGE_EOS),
                        -1);

    g_printerr("got message: %s\n",
               gst_message_type_get_name(GST_MESSAGE_TYPE(message)));

    //gst_xml_write_file(pipeline, stdout);

    format = GST_FORMAT_TIME;
    gst_element_query_position(pipeline, &format, &timePlayed);

    /* clean up nicely */
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(pipeline));

    g_mutex_free(mutex);
    mutex = NULL;

    return timePlayed;
}


/*------------------------------------------------------------------------------
 *  A simple smoke test.
 *----------------------------------------------------------------------------*/
void
SwitcherTest :: mp3Test(void)
                                                throw (CPPUNIT_NS::Exception)
{
    gint64  timePlayed;
    char    str[256];

    timePlayed = playFiles(&mp3File, 1, "0[3s]");
    g_snprintf(str, 256, "time played: %" G_GINT64_FORMAT, timePlayed);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed > 2.9 * GST_SECOND);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed < 3.1 * GST_SECOND);
}


/*------------------------------------------------------------------------------
 *  Play a file until its end.
 *----------------------------------------------------------------------------*/
void
SwitcherTest :: mp3OpenEndedTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    gint64  timePlayed;
    char    str[256];

    timePlayed = playFiles(&mp3File, 1, "0[]");
    g_snprintf(str, 256, "time played: %" G_GINT64_FORMAT, timePlayed);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed > 4.9 * GST_SECOND);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed < 5.1 * GST_SECOND);
}


/*------------------------------------------------------------------------------
 *  Play a file until its end.
 *----------------------------------------------------------------------------*/
void
SwitcherTest :: mp3MultipleTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    const char    * testFiles[] = { mp3File, mp3File };
    gint64          timePlayed;
    char            str[256];

    timePlayed = playFiles(testFiles, 2, "0[2s];1[2s]");
    g_snprintf(str, 256, "time played: %" G_GINT64_FORMAT, timePlayed);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed > 3.9 * GST_SECOND);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed < 4.1 * GST_SECOND);
}


/*------------------------------------------------------------------------------
 *  Play a file until its end.
 *----------------------------------------------------------------------------*/
void
SwitcherTest :: mp3MultipleOpenEndedTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    const char    * testFiles[] = { mp3File, mp3File };
    gint64          timePlayed;
    char            str[256];

    timePlayed = playFiles(testFiles, 2, "0[2s];1[]");
    g_snprintf(str, 256, "time played: %" G_GINT64_FORMAT, timePlayed);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed > 6.9 * GST_SECOND);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed < 7.1 * GST_SECOND);
}


/*------------------------------------------------------------------------------
 *  A simple smoke test.
 *----------------------------------------------------------------------------*/
void
SwitcherTest :: oggVorbisTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    gint64  timePlayed;
    char    str[256];

    timePlayed = playFiles(&oggVorbisFile, 1, "0[3s]");
    g_snprintf(str, 256, "time played: %" G_GINT64_FORMAT, timePlayed);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed > 2.9 * GST_SECOND);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed < 3.1 * GST_SECOND);
}


/*------------------------------------------------------------------------------
 *  Play a file until its end.
 *----------------------------------------------------------------------------*/
void
SwitcherTest :: oggVorbisOpenEndedTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    gint64  timePlayed;
    char    str[256];

    timePlayed = playFiles(&oggVorbisFile, 1, "0[]");
    g_snprintf(str, 256, "time played: %" G_GINT64_FORMAT, timePlayed);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed > 4.9 * GST_SECOND);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed < 5.1 * GST_SECOND);
}


/*------------------------------------------------------------------------------
 *  Play a file until its end.
 *----------------------------------------------------------------------------*/
void
SwitcherTest :: oggVorbisMultipleTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    const char    * testFiles[] = { oggVorbisFile, oggVorbisFile };
    gint64          timePlayed;
    char            str[256];

    timePlayed = playFiles(testFiles, 2, "0[2s];1[2s]");
    g_snprintf(str, 256, "time played: %" G_GINT64_FORMAT, timePlayed);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed > 3.9 * GST_SECOND);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed < 4.1 * GST_SECOND);
}


/*------------------------------------------------------------------------------
 *  Play a file until its end.
 *----------------------------------------------------------------------------*/
void
SwitcherTest :: oggVorbisMultipleOpenEndedTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    const char    * testFiles[] = { oggVorbisFile, oggVorbisFile };
    gint64          timePlayed;
    char            str[256];

    timePlayed = playFiles(testFiles, 2, "0[2s];1[]");
    g_snprintf(str, 256, "time played: %" G_GINT64_FORMAT, timePlayed);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed > 6.9 * GST_SECOND);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed < 7.1 * GST_SECOND);
}


/*------------------------------------------------------------------------------
 *  Test to see if seeking works
 *----------------------------------------------------------------------------*/
void
SwitcherTest :: seekTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    gint64  timePlayed;
    char    str[256];

    timePlayed = playSilenceThenFile(mp3File,
                                     5LL * GST_SECOND,
                                     1LL * GST_SECOND,
                                     4LL * GST_SECOND);
    g_snprintf(str, 256, "time played: %" G_GINT64_FORMAT, timePlayed);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed > 4.9 * GST_SECOND);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed < 5.1 * GST_SECOND);
}


/*------------------------------------------------------------------------------
 *  A simple smoke test.
 *----------------------------------------------------------------------------*/
void
SwitcherTest :: smilTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    gint64  timePlayed;
    char    str[256];

    timePlayed = playFiles(&smilFile, 1, "0[3s]");
    g_snprintf(str, 256, "time played: %" G_GINT64_FORMAT, timePlayed);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed > 2.9 * GST_SECOND);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed < 3.1 * GST_SECOND);
}


/*------------------------------------------------------------------------------
 *  Play a file until its end.
 *----------------------------------------------------------------------------*/
void
SwitcherTest :: smilOpenEndedTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    gint64  timePlayed;
    char    str[256];

    timePlayed = playFiles(&smilFile, 1, "0[]");
    g_snprintf(str, 256, "time played: %" G_GINT64_FORMAT, timePlayed);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed > 4.9 * GST_SECOND);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed < 5.1 * GST_SECOND);
}


/*------------------------------------------------------------------------------
 *  Play a file until its end.
 *----------------------------------------------------------------------------*/
void
SwitcherTest :: smilMultipleTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    const char    * testFiles[] = { smilFile, smilFile };
    gint64          timePlayed;
    char            str[256];

    timePlayed = playFiles(testFiles, 2, "0[2s];1[2s]");
    g_snprintf(str, 256, "time played: %" G_GINT64_FORMAT, timePlayed);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed > 3.9 * GST_SECOND);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed < 4.1 * GST_SECOND);
}


/*------------------------------------------------------------------------------
 *  Play a file until its end.
 *----------------------------------------------------------------------------*/
void
SwitcherTest :: smilMultipleOpenEndedTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    const char    * testFiles[] = { smilFile, smilFile };
    gint64          timePlayed;
    char            str[256];

    timePlayed = playFiles(testFiles, 2, "0[2s];1[]");
    g_snprintf(str, 256, "time played: %" G_GINT64_FORMAT, timePlayed);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed > 6.9 * GST_SECOND);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed < 7.1 * GST_SECOND);
}


