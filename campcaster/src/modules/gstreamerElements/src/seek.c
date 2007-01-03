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

#include "seek.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */


/* ===============================================  local function prototypes */

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


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Seek on an element.
 *----------------------------------------------------------------------------*/
static gboolean
seek_element(GstElement   * element,
             GstSeekType    seekType,
             gint64         seekTime)
{
    return gst_element_seek(element,
                            1.0,
                            GST_FORMAT_TIME,
                            GST_SEEK_FLAG_FLUSH,
                            seekType,
                            seekTime,
                            GST_SEEK_TYPE_NONE,
                            GST_CLOCK_TIME_NONE);
}
/*------------------------------------------------------------------------------
 *  Seek on an element.
 *----------------------------------------------------------------------------*/
gboolean
livesupport_seek(GstElement   * element,
                 GstSeekType    seekType,
                 gint64         seekTime)
{
    return seek_element(element, seekType, seekTime);
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

    seekType = (GstSeekType) GST_SEEK_TYPE_SET;
    seekTime = seconds * GST_SECOND;

    return livesupport_seek(element, seekType, seekTime);
}

