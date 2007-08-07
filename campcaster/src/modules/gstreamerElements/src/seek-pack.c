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
#include "seek.h"
#include "seek-pack.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

#define NSEC_PER_SEC        1000000000LL
#define SEC_PER_MIN         60
#define SEC_PER_HOUR        3600
#define NSEC_PER_SEC_FLOAT  1000000000.0
#define SEC_PER_MIN_FLOAT   60.0
#define SEC_PER_HOUR_FLOAT  3600.0


/* ===============================================  local function prototypes */

/**
 *  Perform the seeks on the SeekPack, set by the initialization function.
 *
 *  @param seekPack the SeekPack to perform the seek on.
 *  @see #livesupport_seek_pack_init
 */
static void
livesupport_seek_pack_seek(LivesupportSeekPack    * seekPack);


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Create a new SeekPack.
 *----------------------------------------------------------------------------*/
LivesupportSeekPack *
livesupport_seek_pack_new(const gchar    * uniqueName,
                          const GstCaps  * caps)
{
    unsigned int            len      = strlen(uniqueName) + 64;
    gchar                 * str      = g_malloc(len);
    LivesupportSeekPack   * seekPack = g_malloc(sizeof(LivesupportSeekPack));

    seekPack->name      = g_strdup(uniqueName);

    seekPack->caps      = gst_caps_copy(caps);

    g_snprintf(str, len, "%s_seekPackSilence", uniqueName);
    seekPack->silence = gst_element_factory_make("audiotestsrc", str);
    /* wave 4 is for silence */
    g_object_set(G_OBJECT(seekPack->silence), "wave", 4, NULL);

    seekPack->source    = NULL;

    /* generate decoder later, by autoplugging */
    seekPack->decoder        = 0;
    seekPack->decoderScale   = 0;
    g_snprintf(str, len, "%s_seekPackSwitcher", uniqueName);
    seekPack->switcher = gst_element_factory_make("switcher", str);
    g_snprintf(str, len, "%s_seekPackBin", uniqueName);
    seekPack->bin       = gst_bin_new(str);
    g_free(str);

    g_object_set(G_OBJECT(seekPack->switcher), "caps", seekPack->caps, NULL);

    seekPack->silenceDuration   = 0LL;
    seekPack->startTime         = 0LL;
    seekPack->endTime           = 0LL;
    seekPack->duration          = 0LL;
    seekPack->positionAfterSeek = 0LL;
    seekPack->realEndTime       = 0LL;
    
    seekPack->sendingSilence = TRUE;

    GstPad* ghost = gst_ghost_pad_new("src", gst_element_get_pad(seekPack->switcher, "src"));
    gst_element_add_pad(seekPack->bin, ghost);

    return seekPack;
}


/*------------------------------------------------------------------------------
 *  Initialize a SeekPack.
 *----------------------------------------------------------------------------*/
void
livesupport_seek_pack_init(LivesupportSeekPack    * seekPack,
                           GstElement             * source,
                           gint64                   silenceDuration,
                           gint64                   startTime,
                           gint64                   endTime)
{
    printf("BEGIN: SEEK_PACK_INIT()\n");

    printf("Name of source element:\n");
    printf(gst_element_get_name(source));
    printf("\n");

    gchar           str[256];
    unsigned int    len      = strlen(seekPack->name) + 64;
    gchar         * name     = g_malloc(len);

    seekPack->source            = source;

    seekPack->silenceDuration   = silenceDuration;
    seekPack->startTime         = startTime;
    seekPack->endTime           = endTime;
    seekPack->duration          = endTime - startTime;
    seekPack->positionAfterSeek = 0LL;
    seekPack->realEndTime       = 0LL;

    if (seekPack->endTime >= 0) {
        g_snprintf(str, 256, "0[%lfs];1[%lfs]",
                             seekPack->silenceDuration / NSEC_PER_SEC_FLOAT,
                             seekPack->duration / NSEC_PER_SEC_FLOAT);
    } else {
        g_snprintf(str, 256, "0[%lfs];1[]",
                             seekPack->silenceDuration / NSEC_PER_SEC_FLOAT);
    }
    g_object_set(seekPack->switcher, "source-config", str, NULL);

    printf("********** TEST ***********\n");
    printf(name);
    g_snprintf(name, len, "%s_seekPackDecoder", seekPack->name);
    
    //FIXME we need a replacement for Akos's autoplugger here
    seekPack->decoder    = gst_element_factory_make("mad", "mp3decoder");
    //seekPack->decoder = ls_gst_autoplug_plug_source(seekPack->source, name, seekPack->caps);

    /* TODO: only add scale element if needed */
    g_snprintf(name, len, "%s_seekPackDecoderScale", seekPack->name);
    seekPack->decoderScale = gst_element_factory_make("audioresample", name);
    g_free(name);

    if (seekPack->decoder) {
        /* seek on the decoder, and link it up with the switcher */
        gst_element_link(seekPack->source, seekPack->decoder);
        livesupport_seek_pack_seek(seekPack);
    } else {
        printf("WARNING: Could not instantiate decoder element.\n");
        /* just fake the content with silence,
         * if it could not be auto-plugged */
        seekPack->decoder = gst_element_factory_make("audiotestsrc", "decoder");
        /* wave 4 is for silence */
        g_object_set(G_OBJECT(seekPack->decoder), "wave", 4, NULL);
    }

    /* put all inside the bin, and link up a ghost pad to switch's src pad */
    gst_bin_add_many(GST_BIN(seekPack->bin),
                     source,
                     seekPack->silence,
                     seekPack->source,
                     seekPack->decoder,
                     seekPack->decoderScale,
                     seekPack->switcher,
                     NULL);

    /* link up the elements */
    printf("Linking elements.\n");

    gst_element_link(seekPack->silence, seekPack->switcher);

    gst_element_link_many(source,
                          seekPack->decoder,
                          seekPack->decoderScale,
                          NULL);

    gst_element_link(seekPack->decoderScale, seekPack->switcher);

    printf("BEGIN: SEEK_PACK_INIT()\n");
}


/*------------------------------------------------------------------------------
 *  Destroy a SeekPack.
 *----------------------------------------------------------------------------*/
void
livesupport_seek_pack_destroy(LivesupportSeekPack     * seekPack)
{
    gst_element_set_state(seekPack->bin, GST_STATE_NULL);
    g_object_unref(seekPack->bin);
    gst_caps_unref(seekPack->caps);
    g_free(seekPack->name);
    g_free(seekPack);
}


/*------------------------------------------------------------------------------
 *  Link a SeekPack to another element.
 *----------------------------------------------------------------------------*/
gboolean
livesupport_seek_pack_link(LivesupportSeekPack    * seekPack,
                           GstElement             * element)
{
    return gst_element_link_filtered(seekPack->bin, element, seekPack->caps);
}


/*------------------------------------------------------------------------------
 *  Add a SeekPack to a bin.
 *----------------------------------------------------------------------------*/
void
livesupport_seek_pack_add_to_bin(LivesupportSeekPack      * seekPack,
                                 GstBin                   * bin)
{
    /* put an extra ref on our elements, as the bin will decrease the
     * ref when they are removed from there */
    g_object_ref(seekPack->bin);
    gst_bin_add(bin, seekPack->bin);
}


/*------------------------------------------------------------------------------
 *  Remove a SeekPack from a bin.
 *----------------------------------------------------------------------------*/
void
livesupport_seek_pack_remove_from_bin(LivesupportSeekPack     * seekPack,
                                      GstBin                  * bin)
{
    gst_bin_remove(bin, seekPack->bin);
}


/*------------------------------------------------------------------------------
 *  Set the state of a SeekPack.
 *----------------------------------------------------------------------------*/
void
livesupport_seek_pack_set_state(LivesupportSeekPack   * seekPack,
                                GstState                state)
{
    /* FIXME: resetting the source from PLAYING state would make it lose
     *        it's seek position */
    if (seekPack->silence) {
        gst_element_set_state(seekPack->silence, state);
    }
    if (seekPack->decoder) {
        gst_element_set_state(seekPack->decoder, state);
    }
    if (seekPack->decoderScale) {
        gst_element_set_state(seekPack->decoderScale, state);
    }
    if (seekPack->switcher) {
        gst_element_set_state(seekPack->switcher, state);
    }
    if (seekPack->bin) {
        gst_element_set_state(seekPack->bin, state);
    }
}


/*------------------------------------------------------------------------------
 *  Do the seeking on a SeekPack.
 *----------------------------------------------------------------------------*/
static void
livesupport_seek_pack_seek(LivesupportSeekPack    * seekPack)
{
    gboolean        ret;
    GstSeekType     seekType;

    seekType = (GstSeekType) GST_SEEK_TYPE_SET;
    GST_DEBUG("seeking on element");
    ret = livesupport_seek(seekPack->decoder, seekType, seekPack->startTime);
    GST_DEBUG("seek result: %d", ret);
}

