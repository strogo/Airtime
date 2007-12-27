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

#include "util.h"
#include "seek.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/**
 *  Define PAD_PUSH if seeking on an src pad should be done with
 *  pushin a new seek event down the pad.
 *  Otherwise, the event will be sent normally.
 */
#undef PAD_PUSH

/**
 *  Define SEEK_ELEMENT if seeking should be done by seeking on an element
 *  itself. Otherwise, the seek will be done on the first src pad of
 *  the element.
 */
#undef SEEK_ELEMENT


/* ===============================================  local function prototypes */

#ifdef SEEK_ELEMENT
/**
 *  Seek on an element.
 *
 *  @param element the element to seek on.
 *  @param seekType the type of seek.
 *  @param seekTime the seek time in nanoseconds.
 *  @return TRUE if seeking was successfult, FALSE otherwise.
 */
static gboolean
seek_element(GstElement   * element,
             GstSeekType    seekType,
             gint64         seekTime);
#endif

#ifndef SEEK_ELEMENT
/**
 *  Seek on the first src pad of an element.
 *
 *  @param element the element to seek on.
 *  @param seekType the type of seek.
 *  @param seekTime the time of seek, in nanoseconds.
 *  @return TRUE if seeking was successfult, FALSE otherwise.
 */
static gboolean
seek_src_pad(GstElement   * element,
             GstSeekType    seekType,
             gint64         seekTime);
#endif


/* =============================================================  module code */

#ifdef SEEK_ELEMENT
/*------------------------------------------------------------------------------
 *  Seek on an element.
 *----------------------------------------------------------------------------*/
static gboolean
seek_element(GstElement   * element,
             GstSeekType    seekType,
             gint64         seekTime)
{
    return gst_element_seek(element, seekType, seekTime);
}
#endif


#ifndef SEEK_ELEMENT
/*------------------------------------------------------------------------------
 *  Seek on the first src pad of an element.
 *----------------------------------------------------------------------------*/
static gboolean
seek_src_pad(GstElement   * element,
             GstSeekType    seekType,
             gint64         seekTime)
{
    GstPad    * pad;
    GstEvent  * seek;

    if ((pad  = get_src_pad(element))) {
        seek = gst_event_new_seek(seekType, seekTime);
#ifdef PAD_PUSH
        gst_pad_push(pad, GST_DATA(seek));
        return TRUE;
#else
        return gst_pad_send_event(pad, seek);
#endif
    }

    GST_WARNING("element doesn't have a src pad");
    return FALSE;
}
#endif


/*------------------------------------------------------------------------------
 *  Seek on an element.
 *----------------------------------------------------------------------------*/
gboolean
livesupport_seek(GstElement   * element,
                 GstSeekType    seekType,
                 gint64         seekTime)
{
#ifdef SEEK_ELEMENT
    return seek_element(element, seekType, seekTime);
#else
    return seek_src_pad(element, seekType, seekTime);
#endif
}


/*------------------------------------------------------------------------------
 *  Helper function to make it easy to seek for a number of seconds.
 *----------------------------------------------------------------------------*/
gboolean
livesupport_seek_seconds(GstElement   * element,
                         gint64         seconds)
{
    GstSeekType     seekType;
    gint64          seekTime;

    seekType = (GstSeekType) (GST_FORMAT_TIME |
                              GST_SEEK_METHOD_SET |
                              GST_SEEK_FLAG_FLUSH);
    seekTime = seconds * GST_SECOND;

    return livesupport_seek(element, seekType, seekTime);
}

