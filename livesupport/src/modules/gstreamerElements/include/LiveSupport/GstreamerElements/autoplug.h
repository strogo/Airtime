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
#ifndef LiveSupport_GstreamerElements_Autoplug_h
#define LiveSupport_GstreamerElements_Autoplug_h

/**
 *  @file
 *  Functions for autoplugging gstreamer elements based on their MIME types.
 *
 *  @author $Author$
 *  @version $Revision$
 */

#ifdef __cplusplus
extern "C" {
#endif


/* ============================================================ include files */

#include <gst/gst.h>


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */


/* ====================================================== function prototypes */

/**
 *  Autoplug a source element, that contains some form of audio.
 *  The result will be a gstreamer element, that is linked with
 *  source, and produces raw audio on its src pad as it output.
 *
 *  @param source the source to autoplug.
 *  @param name the name of the new element.
 *  @param caps the capabilities expected from the returned element,
 *         on its src pad.
 *  @return a gstreamer element already linked to source, that produces
 *          the audio provided by source in audio/x-raw-int or
 *          audio/x-raw-float format, as needed.
 */
GstElement *
ls_gst_autoplug_plug_source(GstElement        * source,
                            const gchar       * name,
                            const GstCaps     * caps);

/**
 *  Return the current position in a previously autoplugged element.
 *  This is a workaround function, as querying the element returned by
 *  ls_gst_autoplug_plug_source() with the standard gstreamer calls
 *  will not give satisfactory results.
 *
 *  @param element a GstElement that was returned by a previous call to
 *         ls_gst_autoplug_plug_source()
 *  @return the current position, in nanoseconds
 */
gint64
ls_gst_autoplug_get_position(GstElement       * element);


/**
 *  Seek on an autoplugged element.
 *  The semantics of this call are the same as gst_element_seek().
 *
 *  @param element a GstElement to send the event to.
 *  @param rate The new playback rate
 *  @param format The format of the seek values
 *  @param flags The optional seek flags.
 *  @param curType The type and flags for the new current position
 *  @param cur The value of the new current position
 *  @param stopType The type and flags for the new stop position
 *  @param stop The value of the new stop position
 *  @return TRUE if the event was handled. MT safe.
 */
gboolean
ls_gst_autoplug_seek(GstElement       * element,
                     gdouble            rate,
                     GstFormat          format,
                     GstSeekFlags       flags,
                     GstSeekType        curType,
                     gint64             cur,
                     GstSeekType        stopType,
                     gint64             stop);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LiveSupport_GstreamerElements_Autoplug_h */

