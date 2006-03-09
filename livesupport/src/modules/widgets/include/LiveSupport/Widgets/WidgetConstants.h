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
 
 
    Author   : $Author $
    Version  : $Revision $
    Location : $URL $

------------------------------------------------------------------------------*/
#ifndef LiveSupport_Widgets_WidgetConstants_h
#define LiveSupport_Widgets_WidgetConstants_h

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

namespace LiveSupport {
namespace Widgets {

/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  A collection of constants used by the widgets.
 *
 *  Constants which are either used by more than one widget or used by
 *  the WidgetFactory class are collected here.  This way widget headers
 *  do not need to include each other's or WidgetFactory's header.
 *
 *  @author  $Author $
 *  @version $Revision $
 */
class WidgetConstants
{
    public:
        /**
         *  The types of available buttons.
         */
        typedef enum { pushButton, tabButton }      ButtonType;

        /**
         *  The types of available image buttons.
         */
        typedef enum { deleteButton, plusButton, minusButton,
                       smallPlayButton, smallPauseButton, smallStopButton,
                       hugePlayButton, 
                       cuePlayButton, cueStopButton,
                       masterPlayButton, masterPauseButton, masterStopButton,
                       windowMinimizeButton, windowMaximizeButton,
                                             windowCloseButton }
                                                    ImageButtonType;

        /**
         *  The list of available miscellaneous images.
         */
        typedef enum { resizeImage,
                       scratchpadWindowTitleImage,
                       searchWindowTitleImage,
                       liveModeWindowTitleImage,
                       playlistsWindowTitleImage,
                       schedulerWindowTitleImage,
                       audioClipIconImage,
                       playlistIconImage }
                                                    ImageType;
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */


} // namespace Widgets
} // namespace LiveSupport

#endif // LiveSupport_Widgets_WidgetConstants_h
