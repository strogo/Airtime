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
 
 
    Author   : $Author: maroy $
    Version  : $Revision: 1.2 $
    Location : $Source: /home/paul/cvs2svn-livesupport/newcvsrepo/livesupport/modules/playlistExecutor/include/LiveSupport/PlaylistExecutor/AudioPlayerEventListener.h,v $

------------------------------------------------------------------------------*/
#ifndef LiveSupport_PlaylistExecutor_AudioPlayerEventListener_h
#define LiveSupport_PlaylistExecutor_AudioPlayerEventListener_h

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#include <exception>
#include <stdexcept>

#include "LiveSupport/Core/Ptr.h"


namespace LiveSupport {
namespace PlaylistExecutor {

using namespace boost;

using namespace LiveSupport::Core;

/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  An event listener interface, for catching events of an audio player.
 *
 *  @author  $Author: maroy $
 *  @version $Revision: 1.2 $
 *  @see AudioPlayerInterface
 */
class AudioPlayerEventListener
{
    public:
        /**
         *  A virtual destructor, as this class has virtual functions.
         */
        virtual
        ~AudioPlayerEventListener(void)                     throw ()
        {
        }

        /**
         *  Catch the event when playing has stopped.
         *  This will happen probably due to the fact that the
         *  audio clip has been played to its end.
         */
        virtual void
        onStop(void)                                        throw () = 0;
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */


} // namespace PlaylistExecutor
} // namespace LiveSupport


#endif // LiveSupport_PlaylistExecutor_AudioPlayerEventListener_h
