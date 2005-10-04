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
#include "AutoplugTest.h"


using namespace boost::posix_time;
using namespace LiveSupport::Core;
using namespace LiveSupport::GstreamerElements;


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

CPPUNIT_TEST_SUITE_REGISTRATION(AutoplugTest);

/**
 *  Define this if time statistics should be printed while running tests.
 */
#undef PRINT_TIMES

/**
 *  Define this if time statistics should be printed for each iteration.
 */
#undef PRINT_ITERATION_TIMES


/**
 *  An mp3 test file.
 */
static const char *         mp3TestFile = "var/5seccounter.mp3";

/**
 *  A 48 kHz mp3 test file.
 */
static const char *         mp3_48kHzTestFile = "var/48kHz.mp3";

/**
 *  An ogg vorbis test file.
 */
static const char *         oggTestFile = "var/5seccounter.ogg";

/**
 *  A 160kb/s ogg vorbis test file.
 */
static const char *         ogg160kbpsTestFile = "var/d160.ogg";

/**
 *  A SMIL test file.
 */
static const char *         smilTestFile = "var/simple.smil";

/**
 *  A SMIL test file, containing an ogg vorbis file.
 */
static const char *         oggSmilFile = "var/simple-ogg.smil";

/**
 *  A file we can't plug.
 */
static const char *         badFile = "src/AutoplugTest.cxx";

/**
 *  A very short file.
 */
static const char *         shortFile = "var/test-short.mp3";

/**
 *  A SMIL file referring to a very short file.
 */
static const char *         shortSmilFile = "var/short.smil";

/**
 *  An embedded SMIL file.
 */
static const char *         embeddedSmilFile = "var/embedded.smil";

/**
 *  A sequentially looking SMIL file.
 */
static const char *         sequentialSmilFile = "var/sequentialSmil.smil";

/**
 *  A long (in time) playlist
 */
static const char *         longSmilFile = "var/bach.smil";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Set up the test environment
 *----------------------------------------------------------------------------*/
void
AutoplugTest :: setUp(void)                         throw ()
{
}


/*------------------------------------------------------------------------------
 *  Clean up the test environment
 *----------------------------------------------------------------------------*/
void
AutoplugTest :: tearDown(void)                      throw ()
{
}


/*------------------------------------------------------------------------------
 *  Play an audio file
 *----------------------------------------------------------------------------*/
gint64
AutoplugTest :: playFile(const char   * audioFile)
                                                throw (CPPUNIT_NS::Exception)
{
    GstElement    * pipeline;
    GstElement    * source;
    GstElement    * decoder;
    GstElement    * sink;
    GstCaps       * caps;
    gint64          timePlayed;

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

    gst_element_link(decoder, sink);
    gst_bin_add_many(GST_BIN(pipeline), source, decoder, sink, NULL);

    gst_element_set_state(source, GST_STATE_PAUSED);
    gst_element_set_state(decoder, GST_STATE_PAUSED);
    gst_element_set_state(sink, GST_STATE_PAUSED);
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    while (gst_bin_iterate(GST_BIN(pipeline))) {
#ifdef PRINT_ITERATION_TIMES
        timePlayed = ls_gst_autoplug_get_position(decoder);
        std::cerr << "iteration time: " << timePlayed << std::endl;
#endif
    }

    timePlayed = ls_gst_autoplug_get_position(decoder);

    /* clean up nicely */
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(pipeline));

    return timePlayed;
}


/*------------------------------------------------------------------------------
 *  Open an audio file
 *----------------------------------------------------------------------------*/
Ptr<time_duration>::Ref
AutoplugTest :: openFile(const char   * audioFile)
                                                throw (CPPUNIT_NS::Exception)
{
    GstElement                * pipeline;
    GstElement                * source;
    GstElement                * decoder;
    GstElement                * sink;
    GstCaps                   * caps;
    Ptr<ptime>::Ref             start;
    Ptr<ptime>::Ref             end;
    Ptr<time_duration>::Ref     duration;

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

    start = TimeConversion::now();

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

        return duration;
    }

    gst_element_link(decoder, sink);
    gst_bin_add_many(GST_BIN(pipeline), source, decoder, sink, NULL);

    gst_element_set_state(source, GST_STATE_PAUSED);
    gst_element_set_state(decoder, GST_STATE_PAUSED);
    gst_element_set_state(sink, GST_STATE_PAUSED);
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    gst_bin_iterate(GST_BIN(pipeline));

    end = TimeConversion::now();

    /* clean up nicely */
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(pipeline));

    duration.reset(new time_duration(*end - *start));

    return duration;
}


/*------------------------------------------------------------------------------
 *  A simple smoke test.
 *----------------------------------------------------------------------------*/
void
AutoplugTest :: firstTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    gint64  timePlayed;
    char    str[256];

    timePlayed = playFile(mp3TestFile);
    g_snprintf(str, 256, "time played: %" G_GINT64_FORMAT, timePlayed);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed > 4.9 * GST_SECOND);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed < 5.1 * GST_SECOND);
}


/*------------------------------------------------------------------------------
 *  Test a 48kHz mp3 file
 *----------------------------------------------------------------------------*/
void
AutoplugTest :: mp3_48kHzTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    gint64  timePlayed;
    char    str[256];

    timePlayed = playFile(mp3_48kHzTestFile);
    g_snprintf(str, 256, "time played: %" G_GINT64_FORMAT, timePlayed);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed > 12.1 * GST_SECOND);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed < 12.3 * GST_SECOND);
}


/*------------------------------------------------------------------------------
 *  An ogg vorbis test.
 *----------------------------------------------------------------------------*/
void
AutoplugTest :: oggVorbisTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    gint64  timePlayed;
    char    str[256];

    timePlayed = playFile(oggTestFile);
    g_snprintf(str, 256, "time played: %" G_GINT64_FORMAT, timePlayed);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed > 4.9 * GST_SECOND);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed < 5.1 * GST_SECOND);
}


/*------------------------------------------------------------------------------
 *  An ogg vorbis test.
 *----------------------------------------------------------------------------*/
void
AutoplugTest :: oggVorbis160kbpsTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    gint64  timePlayed;
    char    str[256];

    timePlayed = playFile(ogg160kbpsTestFile);
    g_snprintf(str, 256, "time played: %" G_GINT64_FORMAT, timePlayed);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed > 2.1 * GST_SECOND);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed < 2.4 * GST_SECOND);
}


/*------------------------------------------------------------------------------
 *  A SMIL test.
 *----------------------------------------------------------------------------*/
void
AutoplugTest :: smilTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    gint64  timePlayed;
    char    str[256];

    timePlayed = playFile(smilTestFile);
    g_snprintf(str, 256, "time played: %" G_GINT64_FORMAT, timePlayed);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed > 4.9 * GST_SECOND);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed < 5.1 * GST_SECOND);
}


/*------------------------------------------------------------------------------
 *  Test somethign we can't plug.
 *----------------------------------------------------------------------------*/
void
AutoplugTest :: negativeTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    gint64  timePlayed;
    char    str[256];

    timePlayed = playFile(badFile);
    g_snprintf(str, 256, "time played: %" G_GINT64_FORMAT, timePlayed);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed == 0LL);
}


/*------------------------------------------------------------------------------
 *  Test on a very short file.
 *----------------------------------------------------------------------------*/
void
AutoplugTest :: shortTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    gint64  timePlayed;
    char    str[256];

    timePlayed = playFile(shortFile);
    g_snprintf(str, 256, "time played: %" G_GINT64_FORMAT, timePlayed);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed > 1.9 * GST_SECOND);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed < 2.1 * GST_SECOND);
}


/*------------------------------------------------------------------------------
 *  Test on a SMIL file referring to a very short file.
 *----------------------------------------------------------------------------*/
void
AutoplugTest :: shortSmilTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    gint64  timePlayed;
    char    str[256];

    timePlayed = playFile(shortSmilFile);
    g_snprintf(str, 256, "time played: %" G_GINT64_FORMAT, timePlayed);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed > 1.9 * GST_SECOND);
    CPPUNIT_ASSERT_MESSAGE(str, timePlayed < 2.1 * GST_SECOND);
}


/*------------------------------------------------------------------------------
 *  See how long it takes to open some playlists
 *----------------------------------------------------------------------------*/
void
AutoplugTest :: playlistOpenTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    Ptr<time_duration>::Ref     duration;

    duration = openFile(mp3TestFile);
#ifdef PRINT_TIMES
    std::cerr << "duration for " << mp3TestFile << ": "
              << *duration << std::endl;
#endif

    duration = openFile(oggTestFile);
#ifdef PRINT_TIMES
    std::cerr << "duration for " << oggTestFile << ": "
              << *duration << std::endl;
#endif

    duration = openFile(smilTestFile);
#ifdef PRINT_TIMES
    std::cerr << "duration for " << smilTestFile << ": "
              << *duration << std::endl;
#endif

    duration = openFile(oggSmilFile);
#ifdef PRINT_TIMES
    std::cerr << "duration for " << oggSmilFile << ": "
              << *duration << std::endl;
#endif

    duration = openFile(shortSmilFile);
#ifdef PRINT_TIMES
    std::cerr << "duration for " << shortSmilFile << ": "
              << *duration << std::endl;
#endif

    duration = openFile(embeddedSmilFile);
#ifdef PRINT_TIMES
    std::cerr << "duration for " << embeddedSmilFile << ": "
              << *duration << std::endl;
#endif

    duration = openFile(sequentialSmilFile);
#ifdef PRINT_TIMES
    std::cerr << "duration for " << sequentialSmilFile << ": "
              << *duration << std::endl;
#endif

    duration = openFile(longSmilFile);
#ifdef PRINT_TIMES
    std::cerr << "duration for " << longSmilFile << ": "
              << *duration << std::endl;
#endif
}


/*------------------------------------------------------------------------------
 *  A test to see if play duration is reported properly.
 *----------------------------------------------------------------------------*/
void
AutoplugTest :: playDurationTest(void)
                                                throw (CPPUNIT_NS::Exception)
{
    gint64      duration;

    duration = playFile(mp3TestFile);
#ifdef PRINT_TIMES
    std::cerr << "duration for " << mp3TestFile << ": "
              << duration << std::endl;
#endif

    duration = playFile(sequentialSmilFile);
#ifdef PRINT_TIMES
    std::cerr << "duration for " << sequentialSmilFile << ": "
              << duration << std::endl;
#endif

    duration = playFile(embeddedSmilFile);
#ifdef PRINT_TIMES
    std::cerr << "duration for " << embeddedSmilFile << ": "
              << duration << std::endl;
#endif
}

