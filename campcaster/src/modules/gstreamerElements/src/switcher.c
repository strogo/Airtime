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


#include <gst/gst.h>

#include "LiveSupport/GstreamerElements/autoplug.h"
#include "smil-util.h"
#include "switcher.h"


/* ===================================================  local data structures */

/**
 *  The properties of the Switcher element.
 */
enum {
    ARG_0,
    ARG_SOURCE_CONFIG,
    ARG_CAPS
};

/**
 *  The plugin registry definition.
GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "switcher",
    "A filter that connects to a swtich, and changes its source",
    plugin_init,
    "$Revision$",
    "GPL",
    "LiveSupport",
    "http://livesupport.campware.org/"
)
*/


/* ================================================  local constants & macros */

/**
 *  The parent class of the Switcher class.
 */
static GstElementClass    * parent_class = NULL;

/**
 *  The sink pad factory.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE(
                                                      "sink_%d",
                                                      GST_PAD_SINK,
                                                      GST_PAD_REQUEST,
                                                      GST_STATIC_CAPS("ANY"));

/**
 *  The source pad factory.
 */
static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE(
                                                      "src",
                                                      GST_PAD_SRC,
                                                      GST_PAD_ALWAYS,
                                                      GST_STATIC_CAPS("ANY"));


/* ===============================================  local function prototypes */

/**
 *  Initialize the Switcher class.
 *
 *  @param klass the class to initialize
 */
static void
livesupport_switcher_class_init(LivesupportSwitcherClass  * klass);

/**
 *  Base initialization for Switcher objects.
 *
 *  @param klass a Switcher class
 */
static void
livesupport_switcher_base_init(LivesupportSwitcherClass   * klass);

/**
 *  Initialize a Switcher object.
 *
 *  @param switcher the Switcher object to initialize.
 */
static void
livesupport_switcher_init(LivesupportSwitcher * switcher);

/**
 *  Destroy a Switcher object.
 *
 *  @param object the Switcher object to destroy.
 */
static void
livesupport_switcher_dispose(GObject * object);

/**
 *  The state change function.
 *
 *  @param elememt the element receiving the state change.
 *  @param transition the state change transition.
 *  @return the success of the state change.
 */
static GstStateChangeReturn
livesupport_switcher_change_state(GstElement      * element,
                                  GstStateChange    transition);

/**
 *  Return the capabilities of a pad.
 *
 *  @pad the pad to return the capabilities for.
 *  @return the capabilities of pad.
 */
static GstCaps *
livesupport_switcher_get_caps(GstPad      * pad);

/**
 *  Set the capabilities for this switcher element.
 *
 *  @param switcher the switcher to set the capabilities for.
 *  @param caps the capabilities to set.
 */
static void
livesupport_switcher_set_caps(LivesupportSwitcher     * switcher,
                              const GstCaps           * caps);

/**
 *  Set a property on a Switcher object.
 *
 *  @param object a Switcher object
 *  @param prop_id the property id
 *  @param value the value to set
 *  @param pspec the property specification
 */
static void
livesupport_switcher_set_property(GObject         * object,
                                  guint             prop_id,
                                  const GValue    * value,
					              GParamSpec      * pspec);

/**
 *  Get a property from a Switcher object.
 *
 *  @param object a Switcher object.
 *  @param prop_id the property id
 *  @param value the requested property (an out parameter)
 *  @param pspec the property specification
 */
static void
livesupport_switcher_get_property(GObject     * object,
                                  guint         prop_id,
                                  GValue      * value,
						          GParamSpec  * pspec);

/**
 *  The main chain function of the Switcher element.
 *
 *  @param pad the pad receiving some data.
 *  @param buf the data recieved.
 *  @return GST_FLOW_OK if all went well, other return values on problems.
 */
static GstFlowReturn
livesupport_switcher_chain(GstPad     * pad,
                           GstBuffer  * buf);

/**
 *  Switch to the source next in line.
 *  Call this function if it's time to switch to the next source.
 *
 *  @param switcher a Switcher object to swtich on.
 */
static void
switch_to_next_source(LivesupportSwitcher     * switcher);

/**
 *  Update the source configuration.
 *  Call this function when the sourceConfigList string has been updated.
 *
 *  @param switcher a Switcher object to perform the update on.
 */
static void
update_source_config(LivesupportSwitcher   * switcher);

/**
 *  Create a new pad upon request.
 *
 *  @param element a Switcher element, for which to create the new pas.
 *  @param template the pad template to use for the new pad.
 *  @param name the requested name of the new pad.
 *  @return the new, requested pad.
 */
static GstPad *
request_new_pad(GstElement        * element,
                GstPadTemplate    * template,
                const gchar       * name);


/**
 *  Callback function for the gst_pad_set_blocked_async() call.
 *
 *  @param pad the pad that is blocked.
 *  @param blocked TRUE if blocked, FALSE if unblocked
 *  @param userData user supplied data
 */
static void
block_callback(GstPad     * pad,
               gboolean     blocked,
               gpointer     userData);


/**
 *  Return the supported query types.
 *
 *  @param pad the pad to return the query types for.
 *  @return an array of supported query types.
 */
static const GstQueryType *
get_srcpad_query_types(GstPad     * pad);


/**
 *  The query function for the source pad.
 *
 *  @param pad the pad that is being queried.
 *  @param query the query to perform.
 *  @return TRUE if the query could be performed, FALSE otherwise.
 */
static gboolean
query_srcpad(GstPad   * pad,
             GstQuery * query);


/**
 *  Function to handle events that come in from all the pads.
 *
 *  @param pad the pad the brings the event.
 *  @param event the event to handle.
 *  @return TRUE if the event was handled, FALSE otherwise.
 */
static gboolean
event_handler(GstPad      * pad,
              GstEvent    * event);


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Return the type structure for the Switcher plugin.
 *----------------------------------------------------------------------------*/
GType
livesupport_switcher_get_type(void)
{
    static GType plugin_type = 0;

    if (!plugin_type) {
        static const GTypeInfo plugin_info = {
            sizeof (LivesupportSwitcherClass),
            (GBaseInitFunc) livesupport_switcher_base_init,
            NULL,
            (GClassInitFunc) livesupport_switcher_class_init,
            NULL,
            NULL,
            sizeof (LivesupportSwitcher),
            0,
            (GInstanceInitFunc) livesupport_switcher_init,
        };

        plugin_type = g_type_register_static(GST_TYPE_ELEMENT,
	                                         "LivesupportSwitcher",
	                                         &plugin_info, 0);
    }

    return plugin_type;
}


/*------------------------------------------------------------------------------
 *  Do base initialization for the Switcher class.
 *----------------------------------------------------------------------------*/
static void
livesupport_switcher_base_init(LivesupportSwitcherClass   * klass)
{
    static GstElementDetails plugin_details = {
        "Switcher",
        "Generic/Switcher",
       "A plugin that is connected to a switch element, and changes its source",
        "Akos Maroy <maroy@campware.org>"
    };
    GstElementClass   * element_class = GST_ELEMENT_CLASS (klass);

    gst_element_class_add_pad_template(element_class,
                                gst_static_pad_template_get(&src_factory));
    gst_element_class_add_pad_template(element_class,
	                            gst_static_pad_template_get(&sink_factory));
    gst_element_class_set_details(element_class, &plugin_details);
}


/*------------------------------------------------------------------------------
 *  Initialize the plugin's class.
 *----------------------------------------------------------------------------*/
static void
livesupport_switcher_class_init(LivesupportSwitcherClass  * klass)
{
    GObjectClass      * gobject_class;
    GstElementClass   * element_class;

    gobject_class = (GObjectClass*) klass;
    element_class = (GstElementClass*) klass;

    parent_class = g_type_class_ref(GST_TYPE_ELEMENT);

    gobject_class->set_property = livesupport_switcher_set_property;
    gobject_class->get_property = livesupport_switcher_get_property;

    g_object_class_install_property(gobject_class,
                                    ARG_SOURCE_CONFIG,
                                    g_param_spec_string("source-config",
                                                        "source config",
                                                        "source config",
                                                        "",
                                                        G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class,
                                    ARG_CAPS,
                                    g_param_spec_pointer("caps",
                                                         "pad capabilites",
                             "fix all switcher capabilities to supplied value",
                                                        G_PARAM_READWRITE));

    gobject_class->dispose      = livesupport_switcher_dispose;

    element_class->request_new_pad = request_new_pad;
    element_class->change_state    = livesupport_switcher_change_state;
}


/*------------------------------------------------------------------------------
 *  Callback function for the gst_pad_set_blocked_async() call
 *----------------------------------------------------------------------------*/
static void
block_callback(GstPad     * pad,
               gboolean     blocked,
               gpointer     userData)
{
    /* don't do anything */
    g_printerr("switcher block_callback on %s:%s: %d\n",
               gst_element_get_name(gst_pad_get_parent_element(pad)),
               gst_pad_get_name(pad),
               blocked);
}


/*------------------------------------------------------------------------------
 *  Return the list of supported query types by the src pad.
 *----------------------------------------------------------------------------*/
static const GstQueryType *
get_srcpad_query_types(GstPad     * pad)
{
    static const GstQueryType query_types[] = { GST_QUERY_POSITION,
                                                0
                                              };

    return query_types;
}


/*------------------------------------------------------------------------------
 *  Query the src pad
 *----------------------------------------------------------------------------*/
static gboolean
query_srcpad(GstPad   * pad,
             GstQuery * query)
{
    LivesupportSwitcher   * switcher = (LivesupportSwitcher*)
                                                    gst_pad_get_parent(pad);

    switch (query->type) {
        case GST_QUERY_POSITION: {
            gint64      cur;
            GstFormat   format;

            gst_query_parse_position(query, &format, NULL);

            switch (format) {
                case GST_FORMAT_DEFAULT:
                    format = GST_FORMAT_TIME;
                    /* intentional fallthrough */

                case GST_FORMAT_TIME:
                    cur = switcher->elapsedTime;
                    break;

                case GST_FORMAT_BYTES:
                    cur = switcher->offset;
                    break;

                default:
                    return FALSE;
            }

            gst_query_set_position(query, format, cur);
        } break;

        case GST_QUERY_DURATION:
            /* TODO */
            /* intentional fallthrough */

        default:
            return FALSE;
    }

    return TRUE;
}


/*------------------------------------------------------------------------------
 *  Create a new request pad.
 *----------------------------------------------------------------------------*/
static GstPad *
request_new_pad(GstElement        * element,
                GstPadTemplate    * template,
                const gchar       * name)
{
    LivesupportSwitcher   * switcher;
    GstPad                * pad;

    g_return_val_if_fail(element != NULL, NULL);
    g_return_val_if_fail(LIVESUPPORT_IS_SWITCHER(element), NULL);

    switcher = LIVESUPPORT_SWITCHER(element);

    if (template->direction != GST_PAD_SINK) {
        GST_DEBUG("only sink pads are created on request");
        return NULL;
    }

    pad = gst_pad_new_from_template(template, name);
    gst_pad_set_getcaps_function(pad, livesupport_switcher_get_caps);

    gst_element_add_pad(element, pad);
    /* TODO: catch the pad remove event, and remove the pad from this
     *       list as well */
    switcher->sinkpadList = g_list_append(switcher->sinkpadList, pad);

    if (GST_PAD_CAPS(switcher->srcpad)) {
        gst_pad_set_caps(pad, GST_PAD_CAPS(switcher->srcpad));
    }

    gst_pad_set_chain_function(pad, livesupport_switcher_chain);
    gst_pad_set_event_function(pad, event_handler);

    return pad;
}


/*------------------------------------------------------------------------------
 *  The event handler for all the pads.
 *----------------------------------------------------------------------------*/
static gboolean
event_handler(GstPad      * pad,
              GstEvent    * event)
{
    LivesupportSwitcher   * switcher;

    switcher = (LivesupportSwitcher*) gst_pad_get_parent(pad);

    g_printerr("got %s event from  %s:%s\n",
               gst_event_type_get_name(GST_EVENT_TYPE(event)),
               gst_element_get_name(gst_pad_get_parent_element(pad)),
               gst_pad_get_name(pad));

    switch (GST_EVENT_TYPE(event)) {
        case GST_EVENT_EOS:
            switch_to_next_source(switcher);
            break;

        default:
            gst_pad_event_default(pad, event);
    }

    return TRUE;
}


/*------------------------------------------------------------------------------
 *  Initialize a Switcher object.
 *----------------------------------------------------------------------------*/
static void
livesupport_switcher_init(LivesupportSwitcher * switcher)
{
    GstElementClass   * klass = GST_ELEMENT_GET_CLASS(switcher);

    switcher->srcpad = gst_pad_new_from_template (
	                        gst_element_class_get_pad_template(klass, "src"),
                            "src");
    gst_pad_set_getcaps_function(switcher->srcpad,
                                 livesupport_switcher_get_caps);

    gst_element_add_pad(GST_ELEMENT(switcher), switcher->srcpad);

    gst_pad_set_query_type_function(switcher->srcpad, get_srcpad_query_types);
    gst_pad_set_query_function(switcher->srcpad, query_srcpad);

    gst_pad_set_event_function(switcher->srcpad, event_handler);

    switcher->caps             = NULL;
    switcher->sinkpadList      = NULL;
    switcher->elapsedTime      = 0LL;
    switcher->offset           = 0LL;
    switcher->switchTime       = 0LL;
    switcher->eos              = FALSE;
    switcher->sourceConfig     = 0;
    switcher->sourceConfigList = NULL;
}


/*------------------------------------------------------------------------------
 *  Destroy a Switcher object.
 *----------------------------------------------------------------------------*/
static void
livesupport_switcher_dispose(GObject * object)
{
    LivesupportSwitcher   * switcher = LIVESUPPORT_SWITCHER(object);

    g_return_if_fail(LIVESUPPORT_IS_SWITCHER(switcher));

    if (switcher->sinkpadList) {
        g_list_free(switcher->sinkpadList);
    }
    if (switcher->sourceConfig) {
        g_free(switcher->sourceConfig);
    }
    if (switcher->sourceConfigList) {
        GList     * element;
       
        for (element = g_list_first(switcher->sourceConfigList);
             element;
             element = g_list_next(element)) {

            g_free(element->data);
        }
        g_list_free(switcher->sourceConfigList);
    }

    if (switcher->caps) {
        gst_caps_unref(switcher->caps);
    }

    G_OBJECT_CLASS(parent_class)->dispose(object);
}


/*------------------------------------------------------------------------------
 *  Return the capabilities of the switcher element.
 *----------------------------------------------------------------------------*/
static GstCaps *
livesupport_switcher_get_caps(GstPad      * pad)
{
    LivesupportSwitcher   * switcher =
                                  LIVESUPPORT_SWITCHER(gst_pad_get_parent(pad));

    return switcher->caps != NULL
         ? gst_caps_copy(switcher->caps)
         : gst_pad_proxy_getcaps(pad);
}


/*------------------------------------------------------------------------------
 *  Handle state changes.
 *----------------------------------------------------------------------------*/
static GstStateChangeReturn
livesupport_switcher_change_state(GstElement      * element,
                                  GstStateChange    transition)
{
    /* TODO: instead of locking peer elements here, catch the link
     *       signal, and lock there */

    LivesupportSwitcher   * switcher = LIVESUPPORT_SWITCHER(element);

    g_return_val_if_fail(LIVESUPPORT_IS_SWITCHER(switcher),
                         GST_STATE_CHANGE_FAILURE);

    switch (transition) {
        case GST_STATE_CHANGE_NULL_TO_READY:
            break;

        case GST_STATE_CHANGE_READY_TO_PAUSED: {
            GList                             * elem;
            LivesupportSwitcherSourceConfig   * config = NULL;

            /* if there s no current config yet, select the first one */
            if (switcher->currentConfig == NULL) {

g_printerr("size of config list: %d\n",
           g_list_length(switcher->sourceConfigList));
                
                switcher->currentConfig =
                                    g_list_first(switcher->sourceConfigList);
                if (switcher->currentConfig) {
                    config = (LivesupportSwitcherSourceConfig*)
                                                switcher->currentConfig->data;

g_printerr("currentConfig: %p, config: %p\n",
           switcher->currentConfig, config);

                    switcher->switchTime = config->duration;
                    if (!config->sinkPad) {
                        if (!(config->sinkPad = g_list_nth_data(
                                                        switcher->sinkpadList,
                                                        config->sourceId))) {
                            GST_ELEMENT_ERROR(GST_ELEMENT(switcher),
                                         RESOURCE,
                                         NOT_FOUND,
                                         ("can't find sinkpad for first sink"),
                                         (NULL));
                        }
                    }
                }
            }

            /* lock all elements linked to our sinks, so that we
             * control their states directly */
            for (elem = g_list_first(switcher->sinkpadList);
                 elem;
                 elem = g_list_next(elem)) {

                GstPad    * sinkPad = (GstPad*) elem->data;
                GstPad    * peerPad = gst_pad_get_peer(sinkPad);

                if (GST_IS_GHOST_PAD(peerPad)) {
                    peerPad = gst_ghost_pad_get_target((GstGhostPad*) peerPad);
                }

g_printerr("config: %p, sinkPad: %p\n",
config, peerPad);

                /* unblock the pad for the current config, block the rest */
                if (config && config->sinkPad == sinkPad) {
                    g_printerr("unblocking a pad\n");
                    gst_pad_set_blocked_async(peerPad, FALSE, block_callback,0);
                    //gst_pad_set_blocked(peerPad, FALSE);
                } else {
                    g_printerr("blocking a pad\n");
                    gst_pad_set_blocked_async(peerPad, FALSE, block_callback,0);
                    //gst_pad_set_blocked(peerPad, TRUE);
                }
            }
        } break;

        case GST_STATE_CHANGE_PAUSED_TO_PLAYING: 
            break;

        case GST_STATE_CHANGE_PLAYING_TO_PAUSED:
            break;

        case GST_STATE_CHANGE_PAUSED_TO_READY:
            break;

        default:
            break;
    }

    return GST_ELEMENT_CLASS(parent_class)->change_state(element, transition);
}


/*------------------------------------------------------------------------------
 *  Switch to the source next in line.
 *----------------------------------------------------------------------------*/
static void
switch_to_next_source(LivesupportSwitcher     * switcher)
{
    LivesupportSwitcherSourceConfig   * oldConfig;
    LivesupportSwitcherSourceConfig   * newConfig;

    if (switcher->currentConfig == 0) {
        /* mark EOS, as there are no more sources to switch to */
        switcher->eos = TRUE;
        gst_pad_push_event(switcher->srcpad, gst_event_new_eos());

        return;
    }

    oldConfig = (LivesupportSwitcherSourceConfig*)
                                                switcher->currentConfig->data;

    if ((switcher->currentConfig = g_list_next(switcher->currentConfig))) {
        GstPad    * peerPad;

        GST_INFO("switching from source %d, duration: %" G_GINT64_FORMAT,
                oldConfig->sourceId, oldConfig->duration);

        newConfig = (LivesupportSwitcherSourceConfig*)
                                                switcher->currentConfig->data;

        if (!newConfig->sinkPad) {
            if (!(newConfig->sinkPad = g_list_nth_data(switcher->sinkpadList,
                                                       newConfig->sourceId))) {
                GST_ELEMENT_ERROR(GST_ELEMENT(switcher),
                                  RESOURCE,
                                  NOT_FOUND,
                                  ("can't find sinkpad for next sink"),
                                  (NULL));
            }
        }

        switcher->switchTime = oldConfig->duration >= 0
                             ? switcher->switchTime + newConfig->duration
                             : switcher->elapsedTime + newConfig->duration;

        g_printerr("blocking and unblocking\n");
        peerPad = gst_pad_get_peer(oldConfig->sinkPad);
        if (GST_IS_GHOST_PAD(peerPad)) {
            peerPad = gst_ghost_pad_get_target((GstGhostPad*) peerPad);
        }
        gst_pad_set_blocked_async(peerPad, TRUE, block_callback, 0);

        peerPad = gst_pad_get_peer(newConfig->sinkPad);
        if (GST_IS_GHOST_PAD(peerPad)) {
            peerPad = gst_ghost_pad_get_target((GstGhostPad*) peerPad);
        }
        gst_pad_set_blocked_async(peerPad, FALSE, block_callback, 0);

    } else {
        /* mark EOS, as there are no more sources to switch to */
        GST_INFO("no more sources after source %d, duration: %" G_GINT64_FORMAT,
                oldConfig->sourceId, oldConfig->duration);
        switcher->eos = TRUE;
        gst_pad_push_event(switcher->srcpad, gst_event_new_eos());
    }
}


/*------------------------------------------------------------------------------
 *  The main chain function.
 *----------------------------------------------------------------------------*/
static GstFlowReturn
livesupport_switcher_chain(GstPad     * pad,
                           GstBuffer  * buf)
{
    LivesupportSwitcher               * switcher;
    LivesupportSwitcherSourceConfig   * config = NULL;

    g_return_val_if_fail(pad != NULL, GST_FLOW_ERROR);
    g_return_val_if_fail(buf != NULL, GST_FLOW_ERROR);

    switcher = (LivesupportSwitcher*) gst_pad_get_parent(pad);
    g_return_val_if_fail(LIVESUPPORT_IS_SWITCHER(switcher), GST_FLOW_ERROR);

    if (switcher->eos) {
        gst_pad_push_event(switcher->srcpad, gst_event_new_eos());

        /* return an unexpected response, as we're in EOS */
        return GST_FLOW_UNEXPECTED;
    }

    if (switcher->currentConfig == NULL) {
        GstPad    * peerPad;

        /* if this is the very first call to the loop function */
        switcher->currentConfig = g_list_first(switcher->sourceConfigList);
        config                  = (LivesupportSwitcherSourceConfig*)
                                                switcher->currentConfig->data;
        switcher->switchTime = config->duration;
        if (!config->sinkPad) {
            if (!(config->sinkPad = g_list_nth_data(switcher->sinkpadList,
                                                    config->sourceId))) {
                GST_ELEMENT_ERROR(GST_ELEMENT(switcher),
                                  RESOURCE,
                                  NOT_FOUND,
                                  ("can't find sinkpad for first sink"),
                                  (NULL));
            }
        }
        peerPad = gst_pad_get_peer(config->sinkPad);
        if (GST_IS_GHOST_PAD(peerPad)) {
            peerPad = gst_ghost_pad_get_target((GstGhostPad*) peerPad);
        }
        gst_pad_set_blocked_async(peerPad, FALSE, block_callback, 0);
    } else {
        config = (LivesupportSwitcherSourceConfig*)
                                                switcher->currentConfig->data;
    }


/* report positions for debug purposes */
{
    GstBin        * parent = (GstBin*) gst_element_get_parent(switcher);
    GstIterator   * it     = gst_bin_iterate_elements(parent);
    gboolean        done   = FALSE;
    gpointer        item;

    while (!done) {
        switch (gst_iterator_next(it, &item)) {
            case GST_ITERATOR_OK: {
                GstElement    * el = (GstElement*) item;
                GstFormat       format = GST_FORMAT_TIME;
                gint64          position;
                GstState        state;
                gboolean        ret;

                if (g_strrstr(gst_element_get_name(el), "decoder")) {
                    position = ls_gst_autoplug_get_position(el);
                    ret      = 1;
                } else {
                    ret = gst_element_query_position(el, &format, &position);
                    if (!ret) {
                        format = GST_FORMAT_BYTES;
                        ret = gst_element_query_position(el, &format,&position);
                    }
                }

                gst_element_get_state(el, &state, 0, 0);
                g_printerr("element %s"
                           ", position: %" G_GINT64_FORMAT " (%d)"
                           ", state: %d\n",
                           GST_ELEMENT_NAME(el), position, ret, state);
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


    if (pad != config->sinkPad) {
        /* just don't do anything on input from not the active pad */
        return GST_FLOW_OK;
    }

    if (config->duration < 0LL) {
        /* handle config->duration == -1LL (play until EOS) */
        if (GST_BUFFER_DURATION(buf) != GST_CLOCK_TIME_NONE) {
            switcher->elapsedTime += GST_BUFFER_DURATION(buf);
            GST_BUFFER_TIMESTAMP(buf) = switcher->elapsedTime;
        }
        switcher->offset += GST_BUFFER_SIZE(buf);
        GST_INFO("elapsed time: %" G_GINT64_FORMAT, switcher->elapsedTime);

        GST_BUFFER_OFFSET(buf)    = switcher->offset;

        /* just push out the incoming buffer without touching it */
        gst_pad_push(switcher->srcpad, buf);

        return GST_FLOW_OK;
    }

    if (GST_BUFFER_DURATION(buf) != GST_CLOCK_TIME_NONE) {
        switcher->elapsedTime += GST_BUFFER_DURATION(buf);
    }
    switcher->offset += GST_BUFFER_SIZE(buf);
    GST_INFO("elapsed time: %" G_GINT64_FORMAT, switcher->elapsedTime);

    if (switcher->elapsedTime >= switcher->switchTime) {
        /* time to switch to the next source */
        switch_to_next_source(switcher);
    }

    GST_INFO("pushing data size: %d, duration: %" G_GINT64_FORMAT,
             GST_BUFFER_SIZE(buf), GST_BUFFER_DURATION(buf));

    GST_BUFFER_TIMESTAMP(buf) = switcher->elapsedTime;
    GST_BUFFER_OFFSET(buf)    = switcher->offset;

    /* just push out the incoming buffer without touching it */
    gst_pad_push(switcher->srcpad, buf);

    return GST_FLOW_OK;
}


/*------------------------------------------------------------------------------
 *  Update the source config.
 *----------------------------------------------------------------------------*/
static void
update_source_config(LivesupportSwitcher   * switcher)
{
    gchar    ** tokens;
    gchar     * token;
    guint       i = 0;
    GList     * listElem;

    /* first free the config list */
    for (listElem = g_list_first(switcher->sourceConfigList);
         listElem;
         listElem = g_list_next(listElem)) {
        g_free(listElem->data);
    }
    g_list_free(switcher->sourceConfigList);
    switcher->sourceConfigList = NULL;
    switcher->currentConfig    = NULL;
    listElem                   = NULL;

    tokens  = g_strsplit(switcher->sourceConfig, ";", 0);
    while ((token = tokens[i++])) {
        gchar                             * durationStr;
        gint                                len;
        LivesupportSwitcherSourceConfig   * config;
        gboolean                            found;

        len         = strlen(token);
        durationStr = g_malloc(sizeof(gchar) * len + 1);
        config      = g_malloc(sizeof(LivesupportSwitcherSourceConfig));
        found       = FALSE;

        /* token formats can be:
         *
         * id[]             play the whole thing, until EOS
         * id[duration]     play from begin for duration time
         */
        if (g_str_has_suffix(token, "[]")
         && sscanf(token, "%d[]", &config->sourceId) == 1) {
            config->duration = -1LL;
            found            = TRUE;
        } else if (sscanf(token, "%d[%[^]]]",
                          &config->sourceId, durationStr) == 2) {
            config->duration = smil_clock_value_to_nanosec(durationStr);
            found            = config->duration >= 0;
        }
        /* don't fill in the sinkPad yet, as it may be added later */
        config->sinkPad = NULL;

        g_free(durationStr);

        if (!found) {
            g_free(config);
            continue;
        }

        switcher->sourceConfigList = g_list_append(switcher->sourceConfigList,
                                                   config);
        listElem = g_list_last(switcher->sourceConfigList);
    }

    g_strfreev(tokens);
}


/*------------------------------------------------------------------------------
 *  Set the capabilities of this switcher element.
 *----------------------------------------------------------------------------*/
static void
livesupport_switcher_set_caps(LivesupportSwitcher     * switcher,
                              const GstCaps           * caps)
{
    if (!switcher || !LIVESUPPORT_IS_SWITCHER(switcher)
     || !caps || !GST_IS_CAPS(caps)) {

        return;
    }

    if (switcher->caps) {
        gst_caps_unref(switcher->caps);
        switcher->caps = 0;
    }

    switcher->caps = gst_caps_copy(caps);
}


/*------------------------------------------------------------------------------
 *  Set a property.
 *----------------------------------------------------------------------------*/
static void
livesupport_switcher_set_property(GObject         * object,
                                  guint             prop_id,
                                  const GValue    * value,
                                  GParamSpec      * pspec)
{
    LivesupportSwitcher   * switcher;

    g_return_if_fail(LIVESUPPORT_IS_SWITCHER(object));
    switcher = LIVESUPPORT_SWITCHER(object);

    switch (prop_id) {
        case ARG_SOURCE_CONFIG:
            if (switcher->sourceConfig) {
                g_free(switcher->sourceConfig);
            }
            switcher->sourceConfig = g_value_dup_string(value);
            update_source_config(switcher);
            break;

        case ARG_CAPS:
            livesupport_switcher_set_caps(switcher, g_value_get_pointer(value));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }
}


/*------------------------------------------------------------------------------
 *  Get a property.
 *----------------------------------------------------------------------------*/
static void
livesupport_switcher_get_property(GObject     * object,
                                  guint         prop_id,
                                  GValue      * value,
                                  GParamSpec  * pspec)
{
    LivesupportSwitcher   * switcher;

    g_return_if_fail(LIVESUPPORT_IS_SWITCHER(object));
    switcher = LIVESUPPORT_SWITCHER(object);

    switch (prop_id) {
        case ARG_SOURCE_CONFIG:
            g_value_set_string(value, switcher->sourceConfig);
            break;

        case ARG_CAPS:
            g_value_set_pointer(value, switcher->caps);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }
}

/*------------------------------------------------------------------------------
 *  Initialize the plugin.
 *----------------------------------------------------------------------------*/
static gboolean
plugin_init(GstPlugin     * plugin)
{
  return gst_element_register(plugin,
                              "switcher",
			                  GST_RANK_NONE,
			                  LIVESUPPORT_TYPE_SWITCHER);
}

