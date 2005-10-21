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
#ifndef GLiveSupport_h
#define GLiveSupport_h

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#include <string>
#include <list>
#include <map>
#include <boost/enable_shared_from_this.hpp>
#include <unicode/resbund.h>

#include "LiveSupport/Core/Ptr.h"
#include "LiveSupport/Core/LocalizedConfigurable.h"
#include "LiveSupport/Core/MetadataTypeContainer.h"
#include "LiveSupport/Authentication/AuthenticationClientInterface.h"
#include "LiveSupport/Storage/StorageClientInterface.h"
#include "LiveSupport/SchedulerClient/SchedulerClientInterface.h"
#include "LiveSupport/PlaylistExecutor/AudioPlayerInterface.h"
#include "LiveSupport/Widgets/WidgetFactory.h"
#include "KeyboardShortcutContainer.h"


namespace LiveSupport {
namespace GLiveSupport {

using namespace LiveSupport::Core;
using namespace LiveSupport::SchedulerClient;
using namespace LiveSupport::Authentication;
using namespace LiveSupport::Storage;
using namespace LiveSupport::PlaylistExecutor;
using namespace LiveSupport::Widgets;


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */


class MasterPanelWindow;


/**
 *  The main application object for the gLiveSupport GUI.
 *
 *  The configuration file for this object is based on configuration
 *  elements used by the object. The DTD for the root element of the
 *  configuration file is:
 *
 *  <pre><code>
 *  <!ELEMENT gLiveSupport  (resourceBundle,
 *                           supportedLanguages,
 *                           widgetFactory,
 *                           authenticationClientFactory,
 *                           storageClientFactory,
 *                           schedulerClientFactory,
 *                           outputPlayer,
 *                           cuePlayer,
 *                           stationLogo,
 *                           metadataTypeContainer,
 *                           keyboardShortcutContainer*) >
 *  </code></pre>
 *
 *  These elements configure objects of the same name, except for
 *  outputPlayer and cuePlayer, which configure two instances of
 *  AudioPlayerFactory; and stationLogo, which is just a path to the
 *  station logo image file.
 *
 *  @author $Author$
 *  @version $Revision$
 *  @see LocalizedObject#getBundle(const xmlpp::Element &)
 *  @see AuthenticationClientFactory
 *  @see StorageClientFactory
 *  @see SchedulerClientFactory
 */
class GLiveSupport : public LocalizedConfigurable,
                     public boost::enable_shared_from_this<GLiveSupport>,
                     public AudioPlayerEventListener
{
    public:
        /**
         *  A type for the map of supported languages.
         *  This is an STL map, containing const Glib::ustring as keys, which
         *  are the language name of supported langauges. The values are
         *  const std::string, the names of the locales for the languages.
         */
        typedef std::map<const Glib::ustring,
                         const std::string>         LanguageMap;

        /**
         *  The type of the list for storing the Scratchpad contents.
         *  This is a list holding Ptr<Playable>::Ref references.
         */
        typedef std::list<Ptr<Playable>::Ref>       PlayableList;

        /**
         *  A type for having a map of AudioClip objects, with using
         *  the ids of the objects as keys.
         */
        typedef std::map<UniqueId::IdType,
                         Ptr<AudioClip>::Ref>       AudioClipMap;

        /**
         *  A type for having a map of Playlist objects, with using
         *  the ids of the objects as keys.
         */
        typedef std::map<UniqueId::IdType,
                         Ptr<Playlist>::Ref>        PlaylistMap;


    private:
        /**
         *  The name of the configuration XML elmenent used by Playlist.
         */
        static const std::string                    configElementNameStr;

        /**
         *  The authentication client used by the application.
         */
        Ptr<AuthenticationClientInterface>::Ref     authentication;

        /**
         *  The storage client used by the application.
         */
        Ptr<StorageClientInterface>::Ref            storage;

        /**
         *  The widget factory, containing our own widgets.
         */
        Ptr<WidgetFactory>::Ref                     widgetFactory;

        /**
         *  The scheduler client, used to access the scheduler daemon.
         */
        Ptr<SchedulerClientInterface>::Ref          scheduler;

        /**
         *  The output audio player.
         */
        Ptr<AudioPlayerInterface>::Ref              outputPlayer;

        /**
         *  The cue audio player.
         */
        Ptr<AudioPlayerInterface>::Ref              cuePlayer;

        /**
         *  The session id for the user.
         */
        Ptr<SessionId>::Ref             sessionId;

        /**
         *  The map of supported languages.
         */
        Ptr<LanguageMap>::Ref           supportedLanguages;

        /**
         *  The container for all the possible metadata types.
         */
        Ptr<MetadataTypeContainer>::Ref metadataTypeContainer;

        /**
         *  The master panel window.
         */
        Ptr<MasterPanelWindow>::Ref     masterPanel;

        /**
         *  A map, holding references to all AudioClip objects that are
         *  openned.
         */
        Ptr<AudioClipMap>::Ref          opennedAudioClips;

        /**
         *  A map, holding references to all Playlist objects that are
         *  openned.
         */
        Ptr<PlaylistMap>::Ref           opennedPlaylists;

        /**
         *  The contents of a Scratchpad, stored as a list.
         */
        Ptr<PlayableList>::Ref          scratchpadContents;

        /**
         *  The one and only playlist that may be edited at any one time.
         */
        Ptr<Playlist>::Ref              editedPlaylist;

        /**
         *  The playlist or audio clip that is being played on the
         *  live mode output sound card (may be null).
         */
        Ptr<Playable>::Ref              outputItemPlayingNow;

        /**
         *  The playlist or audio clip that is being played on the
         *  cue (preview) sound card (may be null).
         */
        Ptr<Playable>::Ref              cueItemPlayingNow;

        /**
         *  True if the output audio player has been paused.
         */
        bool                            outputPlayerIsPaused;

        /**
         *  True if the cue audio player has been paused.
         */
        bool                            cuePlayerIsPaused;

        /**
         *  The raw image containing the station logo.
         */
        Glib::RefPtr<Gdk::Pixbuf>       stationLogoPixbuf;

        /**
         *  Read a supportedLanguages configuration element,
         *  and fill the supportedLanguages map with its contents.
         *
         *  @param element a supportedLanguages element
         *  @exception std::invalid_argument if the supplied XML element
         *             is wrong
         */
        void
        configSupportedLanguages(const xmlpp::Element    & element)
                                                throw (std::invalid_argument);

        /**
         *  Store the contents of the Scratchpad as a user preference.
         */
        void
        storeScratchpadContents(void)                           throw ();

        /**
         *  Load the contents of the Scratchpad as a user preference.
         */
        void
        loadScratchpadContents(void)                            throw ();

        /**
         *  Emit the "edited playlist has been modified" signal.
         */
        void
        emitSignalEditedPlaylistModified(void)                  throw ()
        {
            signalEditedPlaylistModified().emit();
        }

        /**
         *  Remove a playlist from the playlist cache.
         *  The playlist will be released, if it has been acquired earlier.
         *  If the playlist wasn't in the cache, nothing happens.
         *
         *  @param id the id of the playlist to remove.
         */
        void
        uncachePlaylist(Ptr<UniqueId>::Ref  id)                 throw ();
        
        /**
         *  The type for storing the keyboard shortcuts.
         */
        typedef std::map<const Glib::ustring, 
                         Ptr<KeyboardShortcutContainer>::Ref> 
                                                    KeyboardShortcutListType;

        /**
         *  The list of keyboard shortcuts for the various windows.
         */
        KeyboardShortcutListType    keyboardShortcutList;


    protected:
        /**
         *  A signal object to notify people that the edited playlist changed.
         */
        sigc::signal<void>              signalEditedPlaylistModifiedObject;


    public:
        /**
         *  Constructor.
         */
        GLiveSupport(void)                                      throw ()
                : outputPlayerIsPaused(false),
                  cuePlayerIsPaused(false)
        {
            scratchpadContents.reset(new PlayableList());
            opennedAudioClips.reset(new AudioClipMap());
            opennedPlaylists.reset(new PlaylistMap());
        }

        /**
         *  Virtual destructor.
         */
        virtual
        ~GLiveSupport(void)                                     throw ()
        {
            if (outputPlayer.get()) {
                outputPlayer->deInitialize();
            }
            if (cuePlayer.get()) {
                cuePlayer->deInitialize();
            }
            releaseOpennedAudioClips();
            releaseOpennedPlaylists();
        }

        /**
         *  Return the name of the XML element this object expects
         *  to be sent to a call to configure().
         *  
         *  @return the name of the expected XML configuration element.
         */
        static const std::string
        getConfigElementName(void)                              throw ()
        {
            return configElementNameStr;
        }

        /**
         *  Configure the scheduler daemon based on the XML element
         *  supplied.
         *
         *  @param element the XML element to configure the scheduler
         *                 daemon from.
         *  @exception std::invalid_argument if the supplied XML element
         *             contains bad configuraiton information
         *  @exception std::logic_error if the object has already
         *             been configured.
         */
        void
        configure(const xmlpp::Element    & element)
                                                throw (std::invalid_argument,
                                                       std::logic_error);

        /**
         *  A function to check the configuration of this object.
         *  Checks all resources configured by configure()
         *
         *  @retun true if all resources are accessible and working,
         *         false otherwise
         */
        bool
        checkConfiguration(void)                                throw ();

        /**
         *  Display a message window.
         *  This function only returns after the message window is closed.
         *
         *  @param message the message to display
         */
        void
        displayMessageWindow(Ptr<Glib::ustring>::Ref    message)
                                                                throw ();

        /**
         *  Show the main window, and run the application.
         *  This call will only return after the main window has been closed.
         */
        void
        show(void)                                              throw ();

        /**
         *  Change the language of the application.
         *
         *  @param locale the new locale of the appliction.
         *  @exception std::invalid_argument if the specified locale is not
         *             available
         */
        void
        changeLanguage(Ptr<const std::string>::Ref  locale)
                                                throw (std::invalid_argument);

        /**
         *  Perform authentication for the user of the application.
         *  As a result, the user will be authenticated for later
         *  actions, that need authentication.
         *  The user has to be logged out later.
         *
         *  @param login the login ID of the user
         *  @param password the password of the user
         *  @return true if the authentication was successful,
         *          false otherwise
         *  @see #logout
         */
        bool
        login(const std::string & login,
              const std::string & password)                     throw ();

        /**
         *  Return the session id for the user.
         *
         *  @return the session id for the user, if he has been already
         *          successfully authenticated with a call to login(),
         *          and not yet logged out.
         *          otherwise, a reference to 0.
         *  @see #login
         */
        Ptr<SessionId>::Ref
        getSessionId(void) const                                throw ()
        {
            return sessionId;
        }

        /**
         *  Log out the user, if he has been authenticated previously
         *  with a successful call to login().
         *
         *  @see #logout
         */
        void
        logout(void)                                            throw ();

        /**
         *  Accessor function to the scheduler client held by this object.
         *
         *  @return the scheduler client held by this object.
         */
        Ptr<SchedulerClientInterface>::Ref
        getScheduler(void)                                      throw ()
        {
            return scheduler;
        }

        /**
         *  Get the map of supported languages.
         *
         *  @return the map of supported languages.
         */
        Ptr<const LanguageMap>::Ref
        getSupportedLanguages(void) const                       throw ()
        {
            return supportedLanguages;
        }

        /**
         *  Return a container with all supported metadata types.
         *
         *  @return the metadata type container
         */
        Ptr<MetadataTypeContainer>::Ref
        getMetadataTypeContainer(void) const                    throw ()
        {
            return metadataTypeContainer;
        }

        /**
         *  Show the UI components that are visible when no one is logged in.
         */
        void
        showAnonymousUI(void)                                   throw ();

        /**
         *  Show the UI components that are visible when someone is logged in.
         */
        void
        showLoggedInUI(void)                                    throw ();

        /**
         *  Upload a file to the storage.
         *
         *  @param audioClip the file to upload
         *  @exception XmlRpcException on upload failures.
         */
        void
        uploadFile(Ptr<AudioClip>::Ref      audioClip)
                                                    throw (XmlRpcException);

        /**
         *  Add an item to the Scratchpad, and update it.
         *  If the item is already in the scratchpad, it gets pushed to the top.
         *
         *  @param playable the audio clip or playlist to be added
         */
        void
        addToScratchpad(Ptr<Playable>::Ref  playable)           throw ();

        /**
         *  Return the Scratchpad contents.
         *
         *  @return the list holding the Scratchpad contents.
         */
        Ptr<PlayableList>::Ref
        getScratchpadContents(void)                             throw ()
        {
            return scratchpadContents;
        }

        /**
         *  Reset the storage behind GLiveSupport.
         *  Used for testing only.
         *
         *  @exception XmlRpcException on communication problems.
         */
        void
        resetStorage(void)                              throw (XmlRpcException)
        {
            storage->reset();
        }

        /**
         *  Return the full range of audio clip ids in the storage.
         *  Mostly used for testing.
         *
         *  @return a vector of UniqueId object references.
         */
         Ptr<std::vector<Ptr<UniqueId>::Ref> >::Ref
         getAudioClipIds(void)                                  throw ()
         {
             return storage->getAudioClipIds();
         }

        /**
         *  Tell if an audio clip specified by an id exists.
         *
         *  @param id the id of the audio clip to check for.
         *  @return true if the audio clip by the specified id exists,
         *          false otherwise.
         *  @exception XmlRpcException on communication problems.
         */
        bool
        existsAudioClip(Ptr<UniqueId>::Ref   id)     throw (XmlRpcException)
        {
            return storage->existsAudioClip(sessionId, id);
        }

        /**
         *  Open an audio clip, for reading only.
         *  The audio clip will be put into the internal cache of the
         *  GLiveSupport object.
         *
         *  @param id the audio clip id.
         *  @return the audio clip openned.
         *  @exception XmlRpcException if no audio clip with the specified
         *             id exists, or there was a communication problem.
         */
        Ptr<AudioClip>::Ref
        getAudioClip(Ptr<UniqueId>::Ref  id)
                                                    throw (XmlRpcException);

        /**
         *  Acquire an audio clip, for random file access.
         *  The audio clip will be put into the internal cache of the
         *  GLiveSupport object.
         *
         *  @param id the audio clip id.
         *  @return the AudioClip acquired.
         *          note that the returned AudioClip does not have to be
         *          released, this will be done by the caching system
         *          automatically.
         *  @exception XmlRpcException if no audio clip with the specified
         *             id exists, or there was a communication problem.
         */
        Ptr<AudioClip>::Ref
        acquireAudioClip(Ptr<UniqueId>::Ref  id)
                                                    throw (XmlRpcException);

        /**
         *  Return the full range of playlist ids in the storage.
         *  Mostly used for testing.
         *
         *  @return a vector of UniqueId object references.
         */
         Ptr<std::vector<Ptr<UniqueId>::Ref> >::Ref
         getPlaylistIds(void)                                   throw ()
         {
             return storage->getPlaylistIds();
         }

        /**
         *  Tell if a playlist specified by an id exists.
         *
         *  @param id the id of the playlist to check for.
         *  @return true if the playlist by the specified id exists,
         *          false otherwise.
         *  @exception XmlRpcException on communication problems.
         */
        bool
        existsPlaylist(Ptr<UniqueId>::Ref   id)     throw (XmlRpcException)
        {
            return storage->existsPlaylist(sessionId, id);
        }

        /**
         *  Open a playlist, for reading only.
         *  The playlist will be put into the internal cache of the
         *  GLiveSupport object.
         *
         *  @param id the playlist id.
         *  @return the playlist openned.
         *  @exception XmlRpcException if no playlist with the specified
         *             id exists, or there was a communication problem.
         */
        Ptr<Playlist>::Ref
        getPlaylist(Ptr<UniqueId>::Ref  id)
                                                    throw (XmlRpcException);

        /**
         *  Acquire a playlist, for random file access.
         *  The playlist will be put into the internal cache of the
         *  GLiveSupport object.
         *
         *  @param id the playlist id.
         *  @return the playlist acquired.
         *          note that the returned Playlist does not have to be
         *          released, this will be done by the caching system
         *          automatically.
         *  @exception XmlRpcException if no playlist with the specified
         *             id exists, or there was a communication problem.
         */
        Ptr<Playlist>::Ref
        acquirePlaylist(Ptr<UniqueId>::Ref  id)
                                                    throw (XmlRpcException);

        /**
         *  Release all openned audio clips.
         */
        void
        releaseOpennedAudioClips(void)                          throw ();

        /**
         *  Release all openned playlists.
         */
        void
        releaseOpennedPlaylists(void)                           throw ();

        /**
         *  Add a file to the Live Mode, and update it.
         *
         *  @param playable the audio clip or playlist to be added
         */
        void
        addToLiveMode(Ptr<Playable>::Ref  playable)             throw ();

        /**
         *  Return the currently edited playlist.
         *
         *  @return the currenlty edited playlist, or a reference to 0
         *          if no playlist is edited
         */
        Ptr<Playlist>::Ref
        getEditedPlaylist(void)                                 throw ()
        {
            return editedPlaylist;
        }

        /**
         *  Create a new playlist or Open a playlist for editing.
         *  The opened playlist can be later accessed by getEditedPlaylist().
         *  Always release the opened playlist by calling
         *  releaseEditedPlaylist()
         *
         *  @param playlistId the id of the playlist to open for editing.
         *         if a reference to 0, create a new playlist.
         *  @return the new playlist object, which is opened for editing.
         *  @exception XmlRpcException on XMl-RPC errors.
         *  @see #getEditedPlaylist
         *  @see #releaseEditedPlaylist
         */
        Ptr<Playlist>::Ref
        openPlaylistForEditing(
                    Ptr<UniqueId>::Ref playlistId = Ptr<UniqueId>::Ref())
                                                      throw (XmlRpcException);

        /**
         *  Add a playable item to the currently open playlist.
         *  If there is no currently open playlist, open the simple playlist
         *  management window with a new playlist, holding only this one
         *  entry.
         *  Always release the opened playlist by calling
         *  releaseEditedPlaylist()
         *
         *  @param id the id of the playable object to add to the playlist.
         *  @exception XmlRpcException on XMl-RPC errors.
         *  @see #releaseEditedPlaylist
         */
        void
        addToPlaylist(Ptr<const UniqueId>::Ref  id)
                                                      throw (XmlRpcException);
        /**
         *  Save the currently edited playlist in storage.
         *  This call has to be preceeded by a call to openPlaylistForEditing()
         *  or addToPlaylist().
         *  After this call, the playlist is no longer being edited.  If you 
         *  want to continue editing, open the playlist for editing again.
         *
         *  @exception XmlRpcException on upload failures.
         *  @see #openPlaylistForEditing
         *  @see #addToPlaylist
         */
        void
        savePlaylist(void)                          throw (XmlRpcException);

        /**
         *  Cancel the edited playlist: undo changes and release the lock.
         *
         *  @exception XmlRpcException on XML-RPC errors.
         *  @see #openPlaylistForEditing
         */
        void
        cancelEditedPlaylist(void)                  throw (XmlRpcException);

        /**
         *  Return the scheduled entries for a specified time interval.
         *
         *  @param from the start of the interval, inclusive
         *  @param to the end of the interval, exclusive
         *  @return a vector of the schedule entries for the time period.
         *  @exception XmlRpcException in case of XML-RPC errors.
         */
        virtual Ptr<std::vector<Ptr<ScheduleEntry>::Ref> >::Ref
        displaySchedule(Ptr<boost::posix_time::ptime>::Ref  from,
                        Ptr<boost::posix_time::ptime>::Ref  to)
                                                    throw (XmlRpcException)
        {
            return scheduler->displaySchedule(sessionId, from, to);
        }

        /**
         *  Schedule a playlist.
         *  This will schedule the plalyist, and show the scheduler window
         *  at the time of the scheduled playlist.
         *
         *  @param playlist the playlist to schedule.
         *  @param playtime the time for when to schedule.
         *  @exception XmlRpcException in case of XML-RPC errors.
         */
        virtual void
        schedulePlaylist(Ptr<Playlist>::Ref                 playlist,
                         Ptr<boost::posix_time::ptime>::Ref playtime)
                                                    throw (XmlRpcException);
        

        /**
         *  Remove a scheduled item.
         *
         *  @param sessionId a valid, authenticated session id.
         *  @param scheduledEntryId the id of the scheduled entry to remove.
         *  @exception XmlRpcException in case of XML-RPC errors.
         */
        virtual void
        removeFromSchedule(Ptr<UniqueId>::Ref   scheduleEntryId)
                                                    throw (XmlRpcException);

        /**
         *  Play a Playable object using the output audio player.
         *
         *  @param playable the Playable object to play.
         *  @exception std::logic_error in case of audio player errors.
         */
        virtual void
        playOutputAudio(Ptr<Playable>::Ref   playable)
                                                throw (std::logic_error);

        /**
         *  Stop the output audio player.
         *
         *  @exception std::logic_error in case of audio player errors.
         */
        virtual void
        stopOutputAudio(void)
                                                throw (std::logic_error);

        /**
         *  Pause the output audio player.
         *
         *  @exception std::logic_error in case of audio player errors.
         */
        virtual void
        pauseOutputAudio(void)
                                                throw (std::logic_error);

        /**
         *  Determine the time elapsed in the current Playable object
         *  played by the output audio player.
         *
         *  @return     the current time position in the currently open
         *                  Playable object.
         *  @exception  std::logic_error if there is no Playable object open.
         */
        virtual Ptr<time_duration>::Ref
        getOutputAudioPosition(void)
                                                throw (std::logic_error)
        {
            return outputPlayer->getPosition();
        }

        /**
         *  Play a Playable object using the cue audio player.
         *
         *  @param playable the Playable object to play.
         *  @exception XmlRpcException in case of storage server errors.
         *  @exception std::invalid_argument in case of audio player errors.
         *  @exception std::logic_error in case of audio player errors.
         *  @exception std::runtime_error in case of audio player errors.
         */
        virtual void
        playCueAudio(Ptr<Playable>::Ref   playable)
                                                throw (std::logic_error);

        /**
         *  Stop the cue audio player.
         *
         *  @exception XmlRpcException in case of storage server errors.
         *  @exception std::logic_error in case of audio player errors.
         */
        virtual void
        stopCueAudio(void)
                                                throw (std::logic_error);

        /**
         *  Pause the cue audio player.
         *
         *  @exception std::logic_error in case of audio player errors.
         */
        virtual void
        pauseCueAudio(void)
                                                throw (std::logic_error);

        /**
         *  Attach a listener for the cue audio player (the listener
         *  will be notified when the cue audio player has stopped playing).
         *
         *  @param listener the event listener to register.
         */
        void
        attachCueAudioListener(AudioPlayerEventListener *   listener)
                                                throw ();

        /**
         *  Detach the listener for the cue audio player.
         *
         *  @param listener the event listener to unregister.
         *  @exception std::invalid_argument if the supplied event listener 
         *             has not been previously registered.
         */
        void
        detachCueAudioListener(AudioPlayerEventListener *   listener)
                                                throw (std::invalid_argument);

        /**
         *  Search in the local storage.
         *
         *  @param criteria the search conditions to use.
         *  @return the list of audio clips and playlists found.
         *  @exception XmlRpcException passed on from Storage::search()
         */
        Ptr<PlayableList>::Ref
        search(Ptr<SearchCriteria>::Ref     criteria)
                                                throw (XmlRpcException);

        /**
         *  Browse in the local storage.
         *
         *  @param metadata the type of metadata to list (e.g., "dc:title").
         *  @param criteria the search conditions to use.
         *  @return the list of metadata values found.
         *  @exception XmlRpcException passed on from Storage::browse()
         */
        Ptr<std::vector<Glib::ustring> >::Ref
        browse(Ptr<const Glib::ustring>::Ref        metadata,
               Ptr<SearchCriteria>::Ref             criteria)
                                                throw (XmlRpcException);

        /**
         *  Event handler for the "output audio player has stopped" event.
         */
        virtual void
        onStop(void)                            throw ();

        /**
         *  Display the playable item on the master panel as "now playing".
         */
        void
        setNowPlaying(Ptr<Playable>::Ref    playable)
                                                throw ();

        /**
         *  Return an image containing the radio station logo.
         *  It is the responsibility of the caller to dispose of this object.
         *
         *  @return the station logo image.
         */
        Gtk::Image*
        getStationLogoImage()                   throw ();

        /**
         *  The signal raised when the edited playlist is modified.
         *
         *  @return the signal object (a protected member of this class)
         */
        sigc::signal<void>
        signalEditedPlaylistModified(void)      throw ()
        {
            return signalEditedPlaylistModifiedObject;
        }
        
        /**
         *  Find the action triggered by the given key in the given window.
         *
         *  @param  windowName  a string identifying the window (not localized).
         *  @param  modifiers   the gdktypes code for the Shift, Ctrl etc.
         *                          modifier keys which are pressed.
         *  @param  key         the gdkkeysyms code for the key pressed.
         *  @return the associated action; or noAction, if none is found.
         */
        KeyboardShortcut::Action
        findAction(const Glib::ustring &    windowName,
                   unsigned int             modifiers,
                   unsigned int             key) const              throw ();
};

/* ================================================= external data structures */


/* ====================================================== function prototypes */


} // namespace GLiveSupport
} // namespace LiveSupport

#endif // GLiveSupport_h

