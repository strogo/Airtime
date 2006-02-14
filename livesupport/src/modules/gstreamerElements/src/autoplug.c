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
 

    This code is based on the examples/manual/dynamic.c sample file
    provided in the gstreamer-0.8.10 source tarball, which is published
    under the GNU LGPL license.

 
    Author   : $Author$
    Version  : $Revision$
    Location : $URL$

------------------------------------------------------------------------------*/

/* ============================================================ include files */

#include <gst/gst.h>

#include "LiveSupport/GstreamerElements/autoplug.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */


/* ===============================================  local function prototypes */

/**
 *  Handle the event of new pads created for the decodebin.
 *
 *  @param element the element that the new pad was created on.
 *  @param pad the new pad.
 *  @param last TRUE if this is the last pad created for the element,
 *         FALSE otherwise
 *  @param userData user-specific data.
 */
static void
autoplug_newpad(GstElement    * element,
	            GstPad        * pad,
                gboolean        last,
	            gpointer        userData);


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Handle the event of a new pad being created on a decodebin.
 *----------------------------------------------------------------------------*/
static void
autoplug_newpad(GstElement    * element,
	            GstPad        * pad,
                gboolean        last,
	            gpointer        userData)
{
    GstCaps       * caps;
    GstStructure  * str;
    GstElement    * conv = (GstElement*) userData;
    GstPad        * convPad;

    g_return_if_fail(conv != NULL);

    GST_DEBUG("autoplugging %s:%s to %s",
              gst_element_get_name(element),
              gst_pad_get_name(pad),
              gst_element_get_name(conv));

    g_printerr("autoplugging %s:%s to %s in %s\n",
               gst_element_get_name(element),
               gst_pad_get_name(pad),
               gst_element_get_name(conv),
               gst_element_get_name(gst_element_get_parent(element)));

    /* only link once */
    convPad = gst_element_get_pad(conv, "sink");
    if (GST_PAD_IS_LINKED(convPad)) {
        g_object_unref(convPad);
        return;
    }

    /* check media type */
    caps = gst_pad_get_caps(pad);
    str  = gst_caps_get_structure(caps, 0);
    if (!g_strrstr(gst_structure_get_name(str), "audio")) {
        gst_caps_unref(caps);
        gst_object_unref(convPad);
        return;
    }
    gst_caps_unref(caps);

    /* link'n'play */
    gst_pad_link(pad, convPad);
}



/*------------------------------------------------------------------------------
 *  Filter the features that we're interested in.
 *----------------------------------------------------------------------------*/
GstElement *
ls_gst_autoplug_plug_source(GstElement        * source,
                            const gchar       * name,
                            const GstCaps     * caps)
{
    GstElement    * bin;
    GstElement    * dec;
    GstElement    * conv;
    GstPad        * pad;

    bin  = gst_bin_new(name);
    dec  = gst_element_factory_make ("decodebin", "decoder");
    conv = gst_element_factory_make ("audioconvert", "aconv");

    g_signal_connect(dec, "new-decoded-pad", G_CALLBACK(autoplug_newpad), conv);

    gst_bin_add_many(GST_BIN(bin), dec, conv, NULL);

    pad = gst_element_get_pad(dec, "sink");
    gst_element_add_pad(bin, gst_ghost_pad_new("sink", pad));
    gst_object_unref(pad);

    pad = gst_element_get_pad(conv, "src");
    gst_element_add_pad(bin, gst_ghost_pad_new("src", pad));
    gst_object_unref(pad);

    return bin;
}


/*------------------------------------------------------------------------------
 *  Return the current position of an autoplugged element
 *----------------------------------------------------------------------------*/
gint64
ls_gst_autoplug_get_position(GstElement       * element)
{
    GstFormat       format;
    gint64          position;

    if (!element || !GST_IS_BIN(element)) {
        return 0LL;
    }

    format = GST_FORMAT_TIME;
    if (!gst_element_query_position(element, &format, &position)
     || format != GST_FORMAT_TIME) {
        return 0LL;
    }

    return position;
}

