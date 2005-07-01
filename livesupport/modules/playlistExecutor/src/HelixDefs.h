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
    Version  : $Revision: 1.1 $
    Location : $Source: /home/paul/cvs2svn-livesupport/newcvsrepo/livesupport/modules/playlistExecutor/src/Attic/HelixDefs.h,v $

------------------------------------------------------------------------------*/
#ifndef HelixDefs_h
#define HelixDefs_h

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */


/* ================================================================ constants */


/* =================================================================== macros */

/*------------------------------------------------------------------------------
 *  This is a list of defines used by the Helix Library.
 *  Some of these are Linux-specific, watch out when porting to other
 *  platforms.
 *----------------------------------------------------------------------------*/
#ifndef HELIX_CONFIG_MS_PSDK
#define HELIX_CONFIG_MS_PSDK 1
#endif
#ifndef HELIX_FEATURE_3GPP
#define HELIX_FEATURE_3GPP 1
#endif
#ifndef HELIX_FEATURE_ADVANCEDGROUPMGR
#define HELIX_FEATURE_ADVANCEDGROUPMGR 1
#endif
#ifndef HELIX_FEATURE_ALT_URL
#define HELIX_FEATURE_ALT_URL 1
#endif
#ifndef HELIX_FEATURE_ASM
#define HELIX_FEATURE_ASM 1
#endif
#ifndef HELIX_FEATURE_AUDIO
#define HELIX_FEATURE_AUDIO 1
#endif
#ifndef HELIX_FEATURE_AUDIOHOOK
#define HELIX_FEATURE_AUDIOHOOK 1
#endif
#ifndef HELIX_FEATURE_AUDIO_AIFF
#define HELIX_FEATURE_AUDIO_AIFF 1
#endif
#ifndef HELIX_FEATURE_AUDIO_AU
#define HELIX_FEATURE_AUDIO_AU 1
#endif
#ifndef HELIX_FEATURE_AUDIO_CODEC_28_8
#define HELIX_FEATURE_AUDIO_CODEC_28_8 1
#endif
#ifndef HELIX_FEATURE_AUDIO_CODEC_AMRNB
#define HELIX_FEATURE_AUDIO_CODEC_AMRNB 1
#endif
#ifndef HELIX_FEATURE_AUDIO_CODEC_AMRWB
#define HELIX_FEATURE_AUDIO_CODEC_AMRWB 1
#endif
#ifndef HELIX_FEATURE_AUDIO_CODEC_GECKO
#define HELIX_FEATURE_AUDIO_CODEC_GECKO 1
#endif
#ifndef HELIX_FEATURE_AUDIO_CODEC_INTERLEAVE_ALL
#define HELIX_FEATURE_AUDIO_CODEC_INTERLEAVE_ALL 1
#endif
#ifndef HELIX_FEATURE_AUDIO_CODEC_O5_6
#define HELIX_FEATURE_AUDIO_CODEC_O5_6 1
#endif
#ifndef HELIX_FEATURE_AUDIO_CODEC_QCELP
#define HELIX_FEATURE_AUDIO_CODEC_QCELP 1
#endif
#ifndef HELIX_FEATURE_AUDIO_CODEC_RAAC
#define HELIX_FEATURE_AUDIO_CODEC_RAAC 1
#endif
#ifndef HELIX_FEATURE_AUDIO_CODEC_SIPRO
#define HELIX_FEATURE_AUDIO_CODEC_SIPRO 1
#endif
#ifndef HELIX_FEATURE_AUDIO_CODEC_TOKYO
#define HELIX_FEATURE_AUDIO_CODEC_TOKYO 1
#endif
#ifndef HELIX_FEATURE_AUDIO_INACCURATESAMPLING
#define HELIX_FEATURE_AUDIO_INACCURATESAMPLING 1
#endif
#ifndef HELIX_FEATURE_AUDIO_INCOMPLETESAMPLE
#define HELIX_FEATURE_AUDIO_INCOMPLETESAMPLE 1
#endif
#ifndef HELIX_FEATURE_AUDIO_MPA_LAYER1
#define HELIX_FEATURE_AUDIO_MPA_LAYER1 1
#endif
#ifndef HELIX_FEATURE_AUDIO_MPA_LAYER2
#define HELIX_FEATURE_AUDIO_MPA_LAYER2 1
#endif
#ifndef HELIX_FEATURE_AUDIO_MPA_LAYER3
#define HELIX_FEATURE_AUDIO_MPA_LAYER3 1
#endif
#ifndef HELIX_FEATURE_AUDIO_MPEG4
#define HELIX_FEATURE_AUDIO_MPEG4 1
#endif
#ifndef HELIX_FEATURE_AUDIO_MULTIPLAYER_PAUSE
#define HELIX_FEATURE_AUDIO_MULTIPLAYER_PAUSE 1
#endif
#ifndef HELIX_FEATURE_AUDIO_POSTMIXHOOK
#define HELIX_FEATURE_AUDIO_POSTMIXHOOK 1
#endif
#ifndef HELIX_FEATURE_AUDIO_PREMIXHOOK
#define HELIX_FEATURE_AUDIO_PREMIXHOOK 1
#endif
#ifndef HELIX_FEATURE_AUDIO_RALF
#define HELIX_FEATURE_AUDIO_RALF 1
#endif
#ifndef HELIX_FEATURE_AUDIO_REAL
#define HELIX_FEATURE_AUDIO_REAL 1
#endif
#ifndef HELIX_FEATURE_AUDIO_WAVE
#define HELIX_FEATURE_AUDIO_WAVE 1
#endif
#ifndef HELIX_FEATURE_AUTHENTICATION
#define HELIX_FEATURE_AUTHENTICATION 1
#endif
#ifndef HELIX_FEATURE_AUTOUPGRADE
#define HELIX_FEATURE_AUTOUPGRADE 1
#endif
#ifndef HELIX_FEATURE_BASICGROUPMGR
#define HELIX_FEATURE_BASICGROUPMGR 1
#endif
#ifndef HELIX_FEATURE_CHUNKRES
#define HELIX_FEATURE_CHUNKRES 1
#endif
#ifndef HELIX_FEATURE_CLIENT
#define HELIX_FEATURE_CLIENT 1
#endif
#ifndef HELIX_FEATURE_COOKIES
#define HELIX_FEATURE_COOKIES 1
#endif
#ifndef HELIX_FEATURE_CORECOMM
#define HELIX_FEATURE_CORECOMM 1
#endif
#ifndef HELIX_FEATURE_CROSSFADE
#define HELIX_FEATURE_CROSSFADE 1
#endif
#ifndef HELIX_FEATURE_DBG_LOG
#define HELIX_FEATURE_DBG_LOG 1
#endif
#ifndef HELIX_FEATURE_DIRECT_SOUND
#define HELIX_FEATURE_DIRECT_SOUND 1
#endif
#ifndef HELIX_FEATURE_DRM
#define HELIX_FEATURE_DRM 1
#endif
#ifndef HELIX_FEATURE_DTDR_AUDIO_DECODER
#define HELIX_FEATURE_DTDR_AUDIO_DECODER 1
#endif
#ifndef HELIX_FEATURE_DTDR_DECRYPTER
#define HELIX_FEATURE_DTDR_DECRYPTER 1
#endif
#ifndef HELIX_FEATURE_DTDR_ENCODER
#define HELIX_FEATURE_DTDR_ENCODER 1
#endif
#ifndef HELIX_FEATURE_DTDR_MIXER
#define HELIX_FEATURE_DTDR_MIXER 1
#endif
#ifndef HELIX_FEATURE_EVENTMANAGER
#define HELIX_FEATURE_EVENTMANAGER 1
#endif
#ifndef HELIX_FEATURE_FIFOCACHE
#define HELIX_FEATURE_FIFOCACHE 1
#endif
#ifndef HELIX_FEATURE_FILESYSTEMMGR
#define HELIX_FEATURE_FILESYSTEMMGR 1
#endif
#ifndef HELIX_FEATURE_FRAGMENTBUFFER
#define HELIX_FEATURE_FRAGMENTBUFFER 1
#endif
#ifndef HELIX_FEATURE_FULLGUID
#define HELIX_FEATURE_FULLGUID 1
#endif
#ifndef HELIX_FEATURE_GAINTOOL
#define HELIX_FEATURE_GAINTOOL 1
#endif
#ifndef HELIX_FEATURE_GIF_BROKENIMAGE
#define HELIX_FEATURE_GIF_BROKENIMAGE 1
#endif
#ifndef HELIX_FEATURE_GROUPMGR
#define HELIX_FEATURE_GROUPMGR 1
#endif
#ifndef HELIX_FEATURE_HTTPCLOAK
#define HELIX_FEATURE_HTTPCLOAK 1
#endif
#ifndef HELIX_FEATURE_HTTP_GZIP
#define HELIX_FEATURE_HTTP_GZIP 1
#endif
#ifndef HELIX_FEATURE_HYPER_NAVIGATE
#define HELIX_FEATURE_HYPER_NAVIGATE 1
#endif
#ifndef HELIX_FEATURE_IGNORE_SIGPIPE
#define HELIX_FEATURE_IGNORE_SIGPIPE 1
#endif
#ifndef HELIX_FEATURE_ISMA
#define HELIX_FEATURE_ISMA 1
#endif
#ifndef HELIX_FEATURE_LIMITER
#define HELIX_FEATURE_LIMITER 1
#endif
#ifndef HELIX_FEATURE_MASTERTAC
#define HELIX_FEATURE_MASTERTAC 1
#endif
#ifndef HELIX_FEATURE_MEDIAMARKER
#define HELIX_FEATURE_MEDIAMARKER 1
#endif
#ifndef HELIX_FEATURE_MEMMAP_IO
#define HELIX_FEATURE_MEMMAP_IO 1
#endif
#ifndef HELIX_FEATURE_META
#define HELIX_FEATURE_META 1
#endif
#ifndef HELIX_FEATURE_MIXER
#define HELIX_FEATURE_MIXER 1
#endif
#ifndef HELIX_FEATURE_MP3FF_LENIENT
#define HELIX_FEATURE_MP3FF_LENIENT 1
#endif
#ifndef HELIX_FEATURE_MP3FF_ONDEMANDMETAINFO
#define HELIX_FEATURE_MP3FF_ONDEMANDMETAINFO 1
#endif
#ifndef HELIX_FEATURE_MP3FF_SHOUTCAST
#define HELIX_FEATURE_MP3FF_SHOUTCAST 1
#endif
#ifndef HELIX_FEATURE_MP4_FILEFORMAT_ALTERNATES
#define HELIX_FEATURE_MP4_FILEFORMAT_ALTERNATES 1
#endif
#ifndef HELIX_FEATURE_NESTEDMETA
#define HELIX_FEATURE_NESTEDMETA 1
#endif
#ifndef HELIX_FEATURE_NETCHECK
#define HELIX_FEATURE_NETCHECK 1
#endif
#ifndef HELIX_FEATURE_NETINTERFACES
#define HELIX_FEATURE_NETINTERFACES 1
#endif
#ifndef HELIX_FEATURE_NETSERVICES_SHIM
#define HELIX_FEATURE_NETSERVICES_SHIM 1
#endif
#ifndef HELIX_FEATURE_NEXTGROUPMGR
#define HELIX_FEATURE_NEXTGROUPMGR 1
#endif
#ifndef HELIX_FEATURE_OPTIMIZED_SCHEDULER
#define HELIX_FEATURE_OPTIMIZED_SCHEDULER 1
#endif
#ifndef HELIX_FEATURE_OVERLAYMGR
#define HELIX_FEATURE_OVERLAYMGR 1
#endif
#ifndef HELIX_FEATURE_PAC
#define HELIX_FEATURE_PAC 1
#endif
#ifndef HELIX_FEATURE_PACKETHOOKMGR
#define HELIX_FEATURE_PACKETHOOKMGR 1
#endif
#ifndef HELIX_FEATURE_PLAYBACK_LOCAL
#define HELIX_FEATURE_PLAYBACK_LOCAL 1
#endif
#ifndef HELIX_FEATURE_PLAYBACK_NET
#define HELIX_FEATURE_PLAYBACK_NET 1
#endif
#ifndef HELIX_FEATURE_PLAYERNAVIGATOR
#define HELIX_FEATURE_PLAYERNAVIGATOR 1
#endif
#ifndef HELIX_FEATURE_PLUGINHANDLER2
#define HELIX_FEATURE_PLUGINHANDLER2 1
#endif
#ifndef HELIX_FEATURE_PNA
#define HELIX_FEATURE_PNA 1
#endif
#ifndef HELIX_FEATURE_PREFERENCES
#define HELIX_FEATURE_PREFERENCES 1
#endif
#ifndef HELIX_FEATURE_PREFETCH
#define HELIX_FEATURE_PREFETCH 1
#endif
#ifndef HELIX_FEATURE_PROGDOWN
#define HELIX_FEATURE_PROGDOWN 1
#endif
#ifndef HELIX_FEATURE_PROXYMGR
#define HELIX_FEATURE_PROXYMGR 1
#endif
#ifndef HELIX_FEATURE_RAREND_ADV_PACKET_FEEDER
#define HELIX_FEATURE_RAREND_ADV_PACKET_FEEDER 1
#endif
#ifndef HELIX_FEATURE_RAREND_BANDWIDTH_LISTER
#define HELIX_FEATURE_RAREND_BANDWIDTH_LISTER 1
#endif
#ifndef HELIX_FEATURE_RAREND_PREREDSTONE_SUPPORT
#define HELIX_FEATURE_RAREND_PREREDSTONE_SUPPORT 1
#endif
#ifndef HELIX_FEATURE_RAREND_SURESTREAM
#define HELIX_FEATURE_RAREND_SURESTREAM 1
#endif
#ifndef HELIX_FEATURE_RDT
#define HELIX_FEATURE_RDT 1
#endif
#ifndef HELIX_FEATURE_RECORDCONTROL
#define HELIX_FEATURE_RECORDCONTROL 1
#endif
#ifndef HELIX_FEATURE_REGISTRY
#define HELIX_FEATURE_REGISTRY 1
#endif
#ifndef HELIX_FEATURE_RESAMPLER
#define HELIX_FEATURE_RESAMPLER 1
#endif
#ifndef HELIX_FEATURE_RESOURCEMGR
#define HELIX_FEATURE_RESOURCEMGR 1
#endif
#ifndef HELIX_FEATURE_REVERTER
#define HELIX_FEATURE_REVERTER 1
#endif
#ifndef HELIX_FEATURE_RMFF_BANDWIDTH_NEGOTIATOR
#define HELIX_FEATURE_RMFF_BANDWIDTH_NEGOTIATOR 1
#endif
#ifndef HELIX_FEATURE_RMFF_DYNAMICASM
#define HELIX_FEATURE_RMFF_DYNAMICASM 1
#endif
#ifndef HELIX_FEATURE_RMFF_INDEXTABLE
#define HELIX_FEATURE_RMFF_INDEXTABLE 1
#endif
#ifndef HELIX_FEATURE_RMFF_LEGACYAUDIO
#define HELIX_FEATURE_RMFF_LEGACYAUDIO 1
#endif
#ifndef HELIX_FEATURE_RMFF_LICENSING
#define HELIX_FEATURE_RMFF_LICENSING 1
#endif
#ifndef HELIX_FEATURE_RMFF_LIVEINFO
#define HELIX_FEATURE_RMFF_LIVEINFO 1
#endif
#ifndef HELIX_FEATURE_RMFF_MULTIRATE
#define HELIX_FEATURE_RMFF_MULTIRATE 1
#endif
#ifndef HELIX_FEATURE_RMFF_ONDEMANDMETAINFO
#define HELIX_FEATURE_RMFF_ONDEMANDMETAINFO 1
#endif
#ifndef HELIX_FEATURE_RMFF_TIMEOFFSET_FIXUP
#define HELIX_FEATURE_RMFF_TIMEOFFSET_FIXUP 1
#endif
#ifndef HELIX_FEATURE_RMFF_TIMEOFFSET_HANDLER
#define HELIX_FEATURE_RMFF_TIMEOFFSET_HANDLER 1
#endif
#ifndef HELIX_FEATURE_RMFF_VIEWSOURCE
#define HELIX_FEATURE_RMFF_VIEWSOURCE 1
#endif
#ifndef HELIX_FEATURE_RTP
#define HELIX_FEATURE_RTP 1
#endif
#ifndef HELIX_FEATURE_SECURECONN
#define HELIX_FEATURE_SECURECONN 1
#endif
#ifndef HELIX_FEATURE_SETSRCPROPS
#define HELIX_FEATURE_SETSRCPROPS 1
#endif
#ifndef HELIX_FEATURE_SINKCONTROL
#define HELIX_FEATURE_SINKCONTROL 1
#endif
#ifndef HELIX_FEATURE_SMARTERNETWORK
#define HELIX_FEATURE_SMARTERNETWORK 1
#endif
#ifndef HELIX_FEATURE_SMIL1
#define HELIX_FEATURE_SMIL1 1
#endif
#ifndef HELIX_FEATURE_SMIL2
#define HELIX_FEATURE_SMIL2 1
#endif
#ifndef HELIX_FEATURE_SMIL2_ANIMATION
#define HELIX_FEATURE_SMIL2_ANIMATION 1
#endif
#ifndef HELIX_FEATURE_SMIL2_BRUSH
#define HELIX_FEATURE_SMIL2_BRUSH 1
#endif
#ifndef HELIX_FEATURE_SMIL2_MULTIWINDOWLAYOUT
#define HELIX_FEATURE_SMIL2_MULTIWINDOWLAYOUT 1
#endif
#ifndef HELIX_FEATURE_SMIL2_TRANSITIONS
#define HELIX_FEATURE_SMIL2_TRANSITIONS 1
#endif
#ifndef HELIX_FEATURE_SMIL2_VALIDATION
#define HELIX_FEATURE_SMIL2_VALIDATION 1
#endif
#ifndef HELIX_FEATURE_SMIL_REPEAT
#define HELIX_FEATURE_SMIL_REPEAT 1
#endif
#ifndef HELIX_FEATURE_SMIL_SITE
#define HELIX_FEATURE_SMIL_SITE 1
#endif
#ifndef HELIX_FEATURE_SMIL_SOUNDLEVEL
#define HELIX_FEATURE_SMIL_SOUNDLEVEL 1
#endif
#ifndef HELIX_FEATURE_STATS
#define HELIX_FEATURE_STATS 1
#endif
#ifndef HELIX_FEATURE_SYSTEMREQUIRED
#define HELIX_FEATURE_SYSTEMREQUIRED 1
#endif
#ifndef HELIX_FEATURE_THREADSAFE_MEMMAP_IO
#define HELIX_FEATURE_THREADSAFE_MEMMAP_IO 1
#endif
#ifndef HELIX_FEATURE_TIMEDTEXT
#define HELIX_FEATURE_TIMEDTEXT 1
#endif
#ifndef HELIX_FEATURE_TRANSPORT_MULTICAST
#define HELIX_FEATURE_TRANSPORT_MULTICAST 1
#endif
#ifndef HELIX_FEATURE_TURBOPLAY
#define HELIX_FEATURE_TURBOPLAY 1
#endif
#ifndef HELIX_FEATURE_VIDEO
#define HELIX_FEATURE_VIDEO 1
#endif
#ifndef HELIX_FEATURE_VIDEO_CODEC_RV10
#define HELIX_FEATURE_VIDEO_CODEC_RV10 1
#endif
#ifndef HELIX_FEATURE_VIDEO_CODEC_RV20
#define HELIX_FEATURE_VIDEO_CODEC_RV20 1
#endif
#ifndef HELIX_FEATURE_VIDEO_CODEC_RV30
#define HELIX_FEATURE_VIDEO_CODEC_RV30 1
#endif
#ifndef HELIX_FEATURE_VIDEO_CODEC_RV40
#define HELIX_FEATURE_VIDEO_CODEC_RV40 1
#endif
#ifndef HELIX_FEATURE_VIDEO_H263
#define HELIX_FEATURE_VIDEO_H263 1
#endif
#ifndef HELIX_FEATURE_VIDEO_MPEG4
#define HELIX_FEATURE_VIDEO_MPEG4 1
#endif
#ifndef HELIX_FEATURE_VIDEO_REAL
#define HELIX_FEATURE_VIDEO_REAL 1
#endif
#ifndef HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO
#define HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO 1
#endif
#ifndef HELIX_FEATURE_VIDREND_SYNCSMOOTHING
#define HELIX_FEATURE_VIDREND_SYNCSMOOTHING 1
#endif
#ifndef HELIX_FEATURE_VIEWPORT
#define HELIX_FEATURE_VIEWPORT 1
#endif
#ifndef HELIX_FEATURE_VIEWSOURCE
#define HELIX_FEATURE_VIEWSOURCE 1
#endif
#ifndef HELIX_FEATURE_VOLUME
#define HELIX_FEATURE_VOLUME 1
#endif
#ifndef HELIX_FEATURE_XMLPARSER
#define HELIX_FEATURE_XMLPARSER 1
#endif
#ifndef THREADS_SUPPORTED
#define THREADS_SUPPORTED 1
#endif
#ifndef USE_XWINDOWS
#define USE_XWINDOWS 1
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
#ifndef _LINUX
#define _LINUX 1
#endif
#ifndef _LITTLE_ENDIAN
#define _LITTLE_ENDIAN 1
#endif
#ifndef _RED_HAT_5_X_
#define _RED_HAT_5_X_ 1
#endif
#ifndef _REENTRANT
#define _REENTRANT 1
#endif
#ifndef _TIMEDWAITS_RECURSIVE_MUTEXES
#define _TIMEDWAITS_RECURSIVE_MUTEXES 1
#endif
#ifndef _UNIX
#define _UNIX 1
#endif
#ifndef _UNIX_THREADED_NETWORK_IO
#define _UNIX_THREADED_NETWORK_IO 1
#endif
#ifndef _UNIX_THREADS_SUPPORTED
#define _UNIX_THREADS_SUPPORTED 1
#endif


/* =============================================================== data types */


/* ================================================= external data structures */


/* ====================================================== function prototypes */


#endif // HelixDefs_h
