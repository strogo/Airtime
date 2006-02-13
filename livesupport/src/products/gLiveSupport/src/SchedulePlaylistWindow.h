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
#ifndef SchedulePlaylistWindow_h
#define SchedulePlaylistWindow_h

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#include <string>

#include <boost/date_time/gregorian/gregorian.hpp>

#include <unicode/resbund.h>

#include <gtkmm.h>

#include "LiveSupport/Core/Ptr.h"
#include "LiveSupport/Core/LocalizedObject.h"
#include "LiveSupport/Widgets/WhiteWindow.h"
#include "LiveSupport/Widgets/EntryBin.h"
#include "LiveSupport/Widgets/Button.h"
#include "GLiveSupport.h"

namespace LiveSupport {
namespace GLiveSupport {

using namespace LiveSupport::Core;

/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  The window, for scheduling a specific playlist.
 *
 *  The rough layout of the window is:
 *  <code><pre>
 *  +--- scheduler window ----------------------------+
 *  | +--- plalyist name ---------------------------+ |
 *  | +--- calendar --------------------------------+ |
 *  | |                                             | |
 *  | +---------------------------------------------+ |
 *  | +--- time input ------------------------------+ |
 *  | | hour: +- hour -+  minute: +- minute -+      | |
 *  | +---------------------------------------------+ |
 *  | +-- schedule button --------------------------+ |
 *  | +-- close button -----------------------------+ |
 *  +-------------------------------------------------+
 *  </pre></code>
 *
 *  @author $Author$
 *  @version $Revision$
 */
class SchedulePlaylistWindow : public WhiteWindow,
                               public LocalizedObject
{

    protected:

        /**
         *  The GLiveSupport object, holding the state of the application.
         */
        Ptr<GLiveSupport>::Ref      gLiveSupport;

        /**
         *  The playlist to schedule.
         */
        Ptr<Playlist>::Ref          playlist;

        /**
         *  The main container in the window.
         */
        Gtk::Table                * layout;

        /**
         *  The label displaying the name of the playlist to schedule.
         */
        Gtk::Label                * playlistLabel;

        /**
         *  The calendar to select a specific date from.
         */
        Gtk::Calendar             * calendar;

        /**
         *  The hour label.
         */
        Gtk::Label                * hourLabel;

        /**
         *  The entry field for hour.
         */
        EntryBin                 * hourEntry;

        /**
         *  The minute label.
         */
        Gtk::Label                * minuteLabel;

        /**
         *  The minute entry field.
         */
        EntryBin                  * minuteEntry;

        /**
         *  The schedule button.
         */
        Button                    * scheduleButton;

        /**
         *  The close button.
         */
        Button                    * closeButton;

        /**
         *  Signal handler for the schedule button clicked.
         */
        virtual void
        onScheduleButtonClicked(void)                           throw ();

        /**
         *  Signal handler for the close button clicked.
         */
        virtual void
        onCloseButtonClicked(void)                              throw ();


    public:
        /**
         *  Constructor.
         *
         *  @param gLiveSupport the GLiveSupport, application object.
         *  @param bundle the resource bundle holding the localized
         *         resources for this window
         *  @param playlist the playlist to schedule.
         */
        SchedulePlaylistWindow(Ptr<GLiveSupport>::Ref       gLiveSupport,
                               Ptr<ResourceBundle>::Ref     bundle,
                               Ptr<Playlist>::Ref           playlist)
                                                                    throw ();

        /**
         *  Virtual destructor.
         */
        virtual
        ~SchedulePlaylistWindow(void)                               throw ();

};

/* ================================================= external data structures */


/* ====================================================== function prototypes */


} // namespace GLiveSupport
} // namespace LiveSupport

#endif // SchedulePlaylistWindow_h

