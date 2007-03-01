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

#ifdef HAVE_STRING_H
#include <string.h>
#else
#error need string.h
#endif


#include "oneshot-reader.h"


/* ===================================================  local data structures */

/**
 *  The arguments this element supports.
 */
enum {
    ARG_0,
    ARG_CONTENTS,
    ARG_LENGTH
};

/**
 *  Element details.
 */
static GstElementDetails livesupport_one_shot_reader_details =
             GST_ELEMENT_DETAILS("OneShotReader",
                                 "Generic",
                                 "A reader, reading the contents in one shot",
                                 "Campware <contact@campware.org>");

/**
 *  The parent class.
 */
static GstElementClass    * parent_class = NULL;

/**
 *  The sink factory template.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE (
                                                    "sink",
                                                    GST_PAD_SINK,
                                                    GST_PAD_ALWAYS,
                                                    GST_STATIC_CAPS_ANY);


/* ================================================  local constants & macros */

/**
 *  Debug category definition.
 */
GST_DEBUG_CATEGORY_STATIC(one_shot_reader_debug);


//FIXME
#define PACKAGE "foo"


/**
 *  The plugin definition.
 */
GST_PLUGIN_DEFINE(GST_VERSION_MAJOR,
                  GST_VERSION_MINOR,
                  "oneshotreaderplugin",
                  "A reader that reads all of the input on one go",
                  plugin_init,
                  "$Revision$",
                  "GPL",
                  "Campcaster",
                  "http://campcaster.campware.org/")


/* ===============================================  local function prototypes */

/**
 *  Get a specific property.
 *
 *  @param object the object to get the property from (a oneshotreader)
 *  @param propId the ID of the property
 *  @param value the value to return the property in (out parameter)
 *  @param pspec the parameter spec
 */
static void
get_property(GObject      * object,
             guint          propId,
             GValue       * value,
             GParamSpec   * pspec);

/**
 *  Read a stream into memory.
 *
 *  @param read the OneShotReader to read from.
 *  @param outbuffer the buffer to return the whole stream in, plus
 *                   an extra terminating NULL character.
 *                   must be freed after it's not needed.
 *  @param outlength the length of the returned buffer.
 */
static void
read_stream_into_memory(LivesupportOneShotReader  * reader,
                        guint8                   ** outbuffer,
                        guint32                   * outlength);


static GstFlowReturn
livesupport_one_shot_reader_sink_pad_chain(GstPad *pad, GstBuffer *buffer);


/**
 *  The state change function of the element.
 *
 *  @param element a OneShotReader element to change the state for.
 */
//static GstStateChangeReturn
//livesupport_one_shot_reader_change_state(GstElement   * element);

/**
 *  The dispose function of the element.
 *
 *  @param object a OneShotReader element to dispose of.
 */
static void
livesupport_one_shot_reader_dispose(GObject   * object);

/**
 *  Initialize a OneShotReader element.
 *
 *  @param reader the OneShotReader element to initialzie.
 */
static void
livesupport_one_shot_reader_init(LivesupportOneShotReader * reader);

/**
 *  Do base initialization on the element's class.
 *
 *  @param g_class the element's class.
 */
static void
livesupport_one_shot_reader_base_init(gpointer  g_class);

/**
 *  Initialize the element's class.
 *
 *  @param klass the element's class.
 */
static void
livesupport_one_shot_reader_class_init(LivesupportOneShotReaderClass * klass);


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Get a specific property.
 *----------------------------------------------------------------------------*/
static void
get_property(GObject      * object,
             guint          propId,
             GValue       * value,
             GParamSpec   * pspec)
{
    LivesupportOneShotReader * reader = LIVESUPPORT_ONE_SHOT_READER(object);

    switch (propId) {
        case ARG_CONTENTS:
            g_value_set_pointer(value, reader->contents);
            break;

        case ARG_LENGTH:
            g_value_set_uint(value, reader->length);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, propId, pspec);
            break;
    }
}


/*------------------------------------------------------------------------------
 *  Read the whole stream into memory.
 *----------------------------------------------------------------------------*/
static void
read_stream_into_memory(LivesupportOneShotReader  * reader,
                        guint8                   ** outbuffer,
                        guint32                   * outlength)
{
    guint32         length;
    guint32         read;
    guint8        * buffer;

    *outbuffer = 0;
    *outlength = 0;

    if (!reader->adapter) {
        GST_ELEMENT_ERROR(GST_ELEMENT(reader),
                          CORE,
                          PAD,
                          ("missing adapter"),
                          (NULL));
        return;
    }


    /* put a 0 character at the end of the buffer */
    buffer[length] = '\0';

    *outbuffer = buffer;
    *outlength = length;
}


static GstFlowReturn
livesupport_one_shot_reader_sink_pad_chain (GstPad *pad, GstBuffer *buffer)
{
    printf("livesupport_one_shot_reader_sink_pad_chain()");

    LivesupportOneShotReader *this;
    GstAdapter *adapter;
    GstFlowReturn ret = GST_FLOW_OK;
    
    // will give the element an extra ref; remember to drop it 
    this = LIVESUPPORT_ONE_SHOT_READER(gst_pad_get_parent (pad));
    adapter = this->adapter;
    
    // put buffer into adapter
    gst_adapter_push (adapter, buffer);
    
    gst_object_unref (this);
    return ret;
}


#if 0
/*------------------------------------------------------------------------------
 *  The state change function of the element.
 *----------------------------------------------------------------------------*/
static GstStateChangeReturn
livesupport_one_shot_reader_change_state(GstElement   * element)
{
    LivesupportOneShotReader   * reader;

    reader = LIVESUPPORT_ONE_SHOT_READER(element);

    switch (GST_STATE_TRANSITION (element)) {
        case GST_STATE_NULL_TO_READY:
            break;

        case GST_STATE_READY_TO_PAUSED:
            reader->adapter = gst_adapter_new();
            break;

        case GST_STATE_PAUSED_TO_PLAYING:
            if (!reader->processed) {
                /* read the source document into memory */
                read_stream_into_memory(reader,
                                        &reader->contents,
                                        &reader->length);
                if (!reader->contents) {
                    GST_ELEMENT_ERROR(GST_ELEMENT(reader),
                                      STREAM,
                                      WRONG_TYPE,
                                      ("unable to process input"),
                                      (NULL));
                }
                reader->processed = TRUE;
            }
            break;

        case GST_STATE_PLAYING_TO_PAUSED:
            break;
            
        case GST_STATE_PAUSED_TO_READY:
            if (reader->bytestream) {
                gst_bytestream_destroy(reader->bytestream);
                reader->bytestream = 0;
            }
            break;
            
        case GST_STATE_READY_TO_NULL:
            break;

        default:
            break;
    }

    if (GST_ELEMENT_CLASS(parent_class)->change_state) {
        return GST_ELEMENT_CLASS(parent_class)->change_state(element);
    }

    return GST_STATE_SUCCESS;
}
#endif

/*------------------------------------------------------------------------------
 *  The dispose function.
 *----------------------------------------------------------------------------*/
static void
livesupport_one_shot_reader_dispose(GObject * object)
{
    LivesupportOneShotReader * reader = LIVESUPPORT_ONE_SHOT_READER(object);

    if (reader->adapter) {
        g_object_unref(G_OBJECT(reader->adapter));
        reader->adapter = 0;
    }
    if (reader->contents) {
        g_free(reader->contents);
        reader->contents = 0;
    }

    G_OBJECT_CLASS(parent_class)->dispose(object);
}


/*------------------------------------------------------------------------------
 *  Initialize a OneShotReader element.
 *----------------------------------------------------------------------------*/
static void
livesupport_one_shot_reader_init(LivesupportOneShotReader * reader)
{
    reader->sinkpad = gst_pad_new("sink", GST_PAD_SINK);
    gst_element_add_pad(GST_ELEMENT(reader), reader->sinkpad);

    gst_pad_set_chain_function(reader->sinkpad, livesupport_one_shot_reader_sink_pad_chain);

//    gst_pad_set_link_function(reader->sinkpad,
//                              GST_DEBUG_FUNCPTR(gst_pad_proxy_pad_link));
    gst_pad_set_getcaps_function(reader->sinkpad,
                                 GST_DEBUG_FUNCPTR(gst_pad_proxy_getcaps));

    reader->adapter    = 0;
    reader->contents   = 0;
    reader->length     = 0L;

}


/*------------------------------------------------------------------------------
 *  Do base initialization on the element's class.
 *----------------------------------------------------------------------------*/
static void
livesupport_one_shot_reader_base_init(gpointer  g_class)
{
    GstElementClass *element_class = GST_ELEMENT_CLASS(g_class);

    gst_element_class_set_details(element_class,
                                  &livesupport_one_shot_reader_details);

    gst_element_class_add_pad_template(element_class,
                                    gst_static_pad_template_get(&sink_factory));
}


/*------------------------------------------------------------------------------
 *  Initialize the element's class.
 *----------------------------------------------------------------------------*/
static void
livesupport_one_shot_reader_class_init(LivesupportOneShotReaderClass * klass)
{
    GObjectClass      * gobject_class;
    GstElementClass   * gstelement_class;

    gobject_class    = (GObjectClass *) klass;
    gstelement_class = (GstElementClass *) klass;

    parent_class = g_type_class_ref(GST_TYPE_ELEMENT);

    gobject_class->dispose         = livesupport_one_shot_reader_dispose;
//    gstelement_class->change_state = livesupport_one_shot_reader_change_state;

    g_object_class_install_property(gobject_class,
                                    ARG_CONTENTS,
                                    g_param_spec_pointer("contents",
                                                         "contents",
                                                   "the contents read "
                                                   ", a 0-terminated array",
                                                         G_PARAM_READABLE));
    g_object_class_install_property(gobject_class,
                                    ARG_LENGTH,
                                    g_param_spec_uint("length",
                                                      "length",
                                              "the length of the contents read",
                                                      0,
                                                      G_MAXUINT,
                                                      0,
                                                      G_PARAM_READABLE));

    gobject_class->get_property = get_property;
}


/*------------------------------------------------------------------------------
 *  Return the appropriate type for the element.
 *----------------------------------------------------------------------------*/
GType
livesupport_one_shot_reader_get_type(void)
{
    static GType one_shot_reader_type = 0;

    if (!one_shot_reader_type) {
        static const GTypeInfo one_shot_reader_info = {
            sizeof (LivesupportOneShotReaderClass),
            livesupport_one_shot_reader_base_init,
            NULL,
            (GClassInitFunc) livesupport_one_shot_reader_class_init,
            NULL,
            NULL,
            sizeof (LivesupportOneShotReader),
            0,
            (GInstanceInitFunc) livesupport_one_shot_reader_init,
        };

        one_shot_reader_type = g_type_register_static(GST_TYPE_ELEMENT,
                                                     "LivesupportOneShotReader",
                                                     &one_shot_reader_info,
                                                     0);

        GST_DEBUG_CATEGORY_INIT(one_shot_reader_debug,
                                "oneshotreader",
                                0,
                                "a one-shot reader");
    }

    return one_shot_reader_type;
}


/*------------------------------------------------------------------------------
 *  The plugin initialization function.
 *----------------------------------------------------------------------------*/
static gboolean
plugin_init(GstPlugin * plugin)
{
    printf("REGISTERING.");

    return gst_element_register(plugin,
                                "oneshotreader",
                                GST_RANK_NONE,
                                LIVESUPPORT_TYPE_ONE_SHOT_READER);
}

