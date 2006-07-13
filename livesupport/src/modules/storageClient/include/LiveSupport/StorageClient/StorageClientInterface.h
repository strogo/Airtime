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
#ifndef LiveSupport_Storage_StorageClientInterface_h
#define LiveSupport_Storage_StorageClientInterface_h

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#include <stdexcept>

#include "LiveSupport/Core/UniqueId.h"
#include "LiveSupport/Core/Playlist.h"
#include "LiveSupport/Core/SessionId.h"
#include "LiveSupport/Core/XmlRpcException.h"
#include "LiveSupport/Core/SearchCriteria.h"
#include "LiveSupport/Core/AsyncState.h"


namespace LiveSupport {
namespace StorageClient {

using namespace Core;

/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  An interface for storage clients.
 *
 *  @author  $Author$
 *  @version $Revision$
 */
class StorageClientInterface
{
    public:
        /**
         *  A virtual destructor, as this class has virtual functions.
         */
        virtual
        ~StorageClientInterface(void)           throw ()
        {
        }

        /**
         *  Return the version string from the storage.
         *
         *  @return the version string of the storage.
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call.
         */
        virtual Ptr<const Glib::ustring>::Ref
        getVersion(void)                        throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Create a new, empty, playlist.  Does not automatically open the
         *  playlist for editing; for that, use editPlaylist() and
         *  savePlaylist(). 
         *
         *  @param sessionId the session ID from the authentication client
         *  @return the ID of the newly created playlist.
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call.
         */
        virtual Ptr<UniqueId>::Ref
        createPlaylist(Ptr<SessionId>::Ref sessionId)
                                                throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Tell if a playlist with a given id exists.
         *
         *  @param sessionId the session ID from the authentication client
         *  @param id the id of the playlist to check for.
         *  @return true if a playlist with the specified id exists,
         *          false otherwise.
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call.
         */
        virtual bool
        existsPlaylist(Ptr<SessionId>::Ref          sessionId,
                       Ptr<const UniqueId>::Ref     id) const
                                                throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Return a playlist with the specified id to be displayed.
         *  If the playlist is being edited, and this method is called
         *  by the same user who is editing the playlist,
         *  (i.e., the method is called with the same sessionId and playlistId
         *  that editPlaylist() was), then the working copy of the playlist
         *  is returned.
         *  Any other user gets the old (pre-editPlaylist()) copy from storage.
         *
         *  @param sessionId the session ID from the authentication client
         *  @param id the id of the playlist to return.
         *  @return the requested playlist.
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call or no playlist with the specified
         *                             id exists.
         */
        virtual Ptr<Playlist>::Ref
        getPlaylist(Ptr<SessionId>::Ref       sessionId,
                    Ptr<const UniqueId>::Ref  id) const
                                                throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Return a playlist with the specified id to be edited.
         *  This puts a lock on the playlist, and nobody else can edit it
         *  until we release it using savePlaylist().
         *
         *  This method creates a working copy of the playlist, which will
         *  be returned by getPlaylist() if it is called with the same
         *  sessionId and playlistId, until we call savePlaylist().
         *
         *  @param sessionId the session ID from the authentication client
         *  @param id the id of the playlist to return.
         *  @return the requested playlist.
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call or no playlist with the specified
         *                             id exists.
         */
        virtual Ptr<Playlist>::Ref
        editPlaylist(Ptr<SessionId>::Ref      sessionId,
                     Ptr<const UniqueId>::Ref id)
                                                throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Save the playlist after editing.
         *  Can only be called after we obtained a lock on the playlist using
         *  editPlaylist(); this method releases the lock.
         *
         *  This method destroys the working copy created by editPlaylist().
         *
         *  @param sessionId the session ID from the authentication client
         *  @param playlist the playlist to save.
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call or the playlist has not been 
         *                             previously opened by getPlaylist() 
         */
        virtual void
        savePlaylist(Ptr<SessionId>::Ref sessionId,
                     Ptr<Playlist>::Ref  playlist)
                                                throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Revert a playlist to its pre-editing state.
         *  This is only used for clean-up after crashes.  If the GUI
         *  crashed while editing a playlist, it can release the lock on
         *  the playlist (and lose all changes) at the next login using
         *  this method.
         *
         *  @param playlistToken the token of the edited playlist
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call or no playlist with the specified
         *                             token exists.
         */
        virtual void
        revertPlaylist(Ptr<const std::string>::Ref    playlistToken)
                                                throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Acquire the resources for the playlist.
         *  The last saved copy of the playlist is read, and a local copy
         *  is created in SMIL format.  (A local copy is also created for
         *  each sub-playlist contained in the playlist.)
         *  The address of this local copy is
         *  stored in the <code>uri</code> field of the playlist.  The SMIL
         *  file can be played using the audio player.
         *  For each audio clip contained (directly or indirectly) in the
         *  playlist, acquireAudioClip() is called
         *
         *  @param sessionId the session ID from the authentication client
         *  @param id the id of the playlist to acquire.
         *  @return a new Playlist instance containing a uri field which
         *          points to an executable (playable) SMIL representation of
         *          the playlist (in the local storage).
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call or no playlist with the specified
         *                             specified id exists. 
         */
        virtual Ptr<Playlist>::Ref
        acquirePlaylist(Ptr<SessionId>::Ref         sessionId,
                        Ptr<const UniqueId>::Ref    id) const
                                            throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Release the resources (audio clips, other playlists) used 
         *  in a playlist.
         *  For each audio clip contained (directly or indirectly) in the
         *  playlist, releaseAudioClip() is called, and the local copy of
         *  the playlist (and sub-playlists, if any) is removed.
         *  The <code>uri</code> field of the playlist is erased (set to
         *  a null pointer).
         *
         *  @param playlist the playlist to release.
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call or the playlist has no uri field,
         *                             or the file does not exist, etc.
         */
        virtual void
        releasePlaylist(Ptr<Playlist>::Ref   playlist) const
                                            throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Tell if an audio clip with a given id exists.
         *
         *  @param sessionId the session ID from the authentication client
         *  @param id the id of the audio clip to check for.
         *  @return true if an audio clip with the specified id exists,
         *          false otherwise.
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call.
         */
        virtual bool
        existsAudioClip(Ptr<SessionId>::Ref         sessionId,
                        Ptr<const UniqueId>::Ref    id) const
                                                throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Return an audio clip with the specified id to be displayed.
         *  The audio clip returned contains all the metadata (title, author, 
         *  etc.) available for the audio clip, but no binary sound file.
         *  If you want to play the audio clip, use acquireAudioClip().
         *
         *  @param sessionId the session ID from the authentication client
         *  @param id the id of the audio clip to return.
         *  @return the requested audio clip.
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call or no audio clip with the 
         *                             specified id exists.
         */
        virtual Ptr<AudioClip>::Ref
        getAudioClip(Ptr<SessionId>::Ref      sessionId,
                     Ptr<const UniqueId>::Ref id) const
                                                throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Store an audio clip.
         *  The audio clip is expected to have valid <code>title</code>,
         *  <code>playlength</code> and <code>uri</code> fields, the latter
         *  containing the URI of a binary sound file.
         *  
         *  If the audio clip does not have
         *  an ID field (i.e., <code>audioClip->getId()</code> is a null 
         *  pointer), one will be generated, and <code>audioClip->getId()</code>
         *  will contain a valid UniqueId after the method returns.
         *  If the audio clip had an ID already, then it remains unchanged.
         *
         *  The size of the binary sound file must be less than 2 GB, because 
         *  the storage server can not deal with larger files.
         *
         *  @param sessionId the session ID from the authentication client
         *  @param audioClip the audio clip to store.
         *
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call or we have not logged in yet.
         */
        virtual void
        storeAudioClip(Ptr<SessionId>::Ref sessionId,
                       Ptr<AudioClip>::Ref audioClip)
                                                throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Acquire the resources for the audio clip with the specified id.
         *  The <code>uri</code> field of the audio clip returned by the
         *  method points to a binary sound file playable by the audio player.
         *  This binary sound file can be randomly accessed.
         *
         *  The returned audio clip also contains a <code>token</code> field
         *  which identifies it to the storage server; this is used by
         *  releaseAudioClip().
         *
         *  @param sessionId the session ID from the authentication client
         *  @param id the id of the audio clip to acquire.
         *  @return a new AudioClip instance, containing a uri field which
         *          points to (a way of getting) the sound file.
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call or if no audio clip with the 
         *                             specified id exists. 
         */
        virtual Ptr<AudioClip>::Ref
        acquireAudioClip(Ptr<SessionId>::Ref        sessionId,
                         Ptr<const UniqueId>::Ref   id) const
                                                throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Release the resource (sound file) used by an audio clip.
         *  After the call to this method, the binary sound file is no longer
         *  accessible, and the <code>uri</code> and <code>token</code> fields
         *  of the audioClip are erased (set to null pointers).
         *
         *  @param audioClip the id of the audio clip to release.
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                 call or  the audio clip has no uri field, 
         *                 or the file does not exist, etc. 
         */
        virtual void
        releaseAudioClip(Ptr<AudioClip>::Ref audioClip) const
                                                throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Search for audio clips or playlists.  The results can be read
         *  using getSearchResults().
         *
         *  @param sessionId the session ID from the authentication client
         *  @param searchCriteria an object containing the search criteria
         *  @return the number of items found; this may not be equal to the 
         *          number of items returned: see SearchCriteria::setLimit()
         *          and SearchCriteria::setOffset()
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call.
         */
        virtual int
        search(Ptr<SessionId>::Ref      sessionId,
               Ptr<SearchCriteria>::Ref searchCriteria) 
                                                throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Browse for metadata values.
         *
         *  @param sessionId      the session ID from the authentication client
         *  @param metadataType   the type of metadata to browse for
         *  @param searchCriteria an object containing the search criteria
         *  @return a vector containing the metadata values found
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call.
         */
        virtual Ptr<std::vector<Glib::ustring> >::Ref
        browse(Ptr<SessionId>::Ref                  sessionId,
               Ptr<const Glib::ustring>::Ref        metadata,
               Ptr<SearchCriteria>::Ref             searchCriteria) 
                                                throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Search for audio clips or playlists on a remote network hub.
         *
         *  This starts the asynchronous function call; check the progress
         *  of the search with checkTransport().
         *
         *  Once checkTransport() reports a finishedState, you can call
         *  remoteSearchClose() to get the search results.
         *
         *  @param  sessionId   the session ID from the authentication client.
         *  @param  searchCriteria  an object containing the search criteria.
         *  @return a transport token which identifies this search.
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call.
         */
        virtual Ptr<Glib::ustring>::Ref
        remoteSearchOpen(Ptr<SessionId>::Ref        sessionId,
                         Ptr<SearchCriteria>::Ref   searchCriteria) 
                                                throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Download the search results after the remote search has finished.
         *
         *  If this search is in the finishedState, it will be moved to the
         *  closedState, the transport token will be invalidated, and the 
         *  search results can be read using getSearchResults().
         *
         *  If the search is in any other state, an exception is raised.
         *
         *  You can check the state of the search with checkTransport().
         *
         *  @param token     the transport token from remoteSearchOpen().
         *  @return the number of items found; this may not be equal to the 
         *          number of items returned: see SearchCriteria::setLimit()
         *          and SearchCriteria::setOffset().
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call.
         */
        virtual int
        remoteSearchClose(Ptr<const Glib::ustring>::Ref     token) 
                                                throw (XmlRpcException)
                                                                        = 0;

        /**
         *  The type for the list of playlists returned by the search method
         */
        typedef std::vector<Ptr<Playable>::Ref>
                                    SearchResultsType;

        /**
         *  Return the list of items found by the latest search.
         *
         *  (Or the list of items returned by reset() -- used for testing.)
         *
         *  @return a vector of Playable objects.
         */
        virtual Ptr<SearchResultsType>::Ref
        getSearchResults(void)                  throw ()                = 0;

        /**
         *  Reset the storage to its initial state.  
         *  The contents of the storage can be read using getSearchResults().
         *  Used for testing.
         *
         *  @exception XmlRpcException if the server returns an error.
         */
        virtual void
        reset(void)                             throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Return a list of all playlists in the storage.
         *  It uses the search method to get a list of playlists, passing
         *  the limit and offset parameters on to it.
         *
         *  @param sessionId the session ID from the authentication client
         *  @param limit     the maximum number of playlists to return
         *  @param offset    skip the first <i>offset</i> playlists
         *  @return a vector containing the playlists.
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call.
         */
        virtual Ptr<std::vector<Ptr<Playlist>::Ref> >::Ref
        getAllPlaylists(Ptr<SessionId>::Ref sessionId,
                        int                 limit  = 0,
                        int                 offset = 0)
                                                throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Return a list of all audio clips in the storage.
         *  It uses the search method to get a list of playlists, passing
         *  the limit and offset parameters on to it.
         *
         *  @param sessionId the session ID from the authentication client
         *  @param limit     the maximum number of audio clips to return
         *  @param offset    skip the first <i>offset</i> audio clips
         *  @return a vector containing the playlists.
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call.
         */
        virtual Ptr<std::vector<Ptr<AudioClip>::Ref> >::Ref
        getAllAudioClips(Ptr<SessionId>::Ref  sessionId,
                         int                  limit  = 0,
                         int                  offset = 0)
                                                throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Initiate the creation of a storage backup.
         *
         *  @param sessionId    the session ID from the authentication client.
         *  @param  criteria    specifies which items should go in the backup.
         *  @return a token which identifies this backup task.
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call.
         */
        virtual Ptr<Glib::ustring>::Ref
        createBackupOpen(Ptr<SessionId>::Ref        sessionId,
                         Ptr<SearchCriteria>::Ref   criteria) const
                                                throw (XmlRpcException)
                                                                        = 0;
        
        /**
         *  Check the state of a storage backup.
         *
         *  @param  token   the identifier of this backup task.
         *  @param  url     return parameter;
         *                      if a finishedState is returned, it contains the
         *                      URL of the created backup file.
         *  @param  path    return parameter;
         *                      if a finishedState is returned, it contains the
         *                      local access path of the created backup file.
         *  @param  errorMessage    return parameter;
         *                      if a failedState is returned, it contains the
         *                      fault string.
         *  @return the state of the backup process: one of pendingState,
         *                      finishedState, or failedState.
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call.
         */
        virtual AsyncState
        createBackupCheck(const Glib::ustring &             token,
                          Ptr<const Glib::ustring>::Ref &   url,
                          Ptr<const Glib::ustring>::Ref &   path,
                          Ptr<const Glib::ustring>::Ref &   errorMessage) const
                                                throw (XmlRpcException)
                                                                        = 0;
        
        /**
         *  Close the storage backup process.
         *
         *  @param  token           the identifier of this backup task.
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call.
         */
        virtual void
        createBackupClose(const Glib::ustring &     token) const
                                                throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Initiate the uploading of a storage backup to the local storage.
         *
         *  @param  sessionId   the session ID from the authentication client.
         *  @param  path        the location of the archive file to upload.
         *  @return a token which identifies this task.
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call.
         */
        virtual Ptr<Glib::ustring>::Ref
        restoreBackupOpen(Ptr<SessionId>::Ref               sessionId,
                          Ptr<const Glib::ustring>::Ref     path) const
                                                throw (XmlRpcException)
                                                                        = 0;
        
        /**
         *  Check the status of a backup restore.
         *
         *  @param  token       the identifier of this backup task.
         *  @param  errorMessage    return parameter;
         *                      if a failedState is returned, it contains the
         *                      fault string.
         *  @return the state of the restore process: one of pendingState,
         *                      finishedState, or failedState.
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call.
         */
        virtual AsyncState
        restoreBackupCheck(const Glib::ustring &            token,
                           Ptr<const Glib::ustring>::Ref &  errorMessage) const
                                                throw (XmlRpcException)
                                                                        = 0;
        
        /**
         *  Close the backup restore process.
         *
         *  @param  token       the identifier of this backup task.
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call.
         */
        virtual void
        restoreBackupClose(const Glib::ustring &            token) const
                                                throw (XmlRpcException)
                                                                        = 0;
        
        /**
         *  The possible formats of exported playlists.
         */
        typedef enum {  internalFormat,
                        smilFormat }        ExportFormatType;

        /**
         *  Initiate the exporting of a playlist.
         *
         *  @param  sessionId   the session ID from the authentication client.
         *  @param  playlistId  the ID of the playlist to be exported.
         *  @param  format      the format of the exported playlist.
         *  @param  url         return parameter: readable URL pointing to the
         *                      exported playlist.
         *  @return a token which identifies this export task.
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call.
         */
        virtual Ptr<Glib::ustring>::Ref
        exportPlaylistOpen(Ptr<SessionId>::Ref        sessionId,
                           Ptr<UniqueId>::Ref         playlistId,
                           ExportFormatType           format,
                           Ptr<Glib::ustring>::Ref    url) const
                                                throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Close the playlist export process.
         *
         *  @param  token           the identifier of this export task.
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call.
         */
        virtual void
        exportPlaylistClose(Ptr<const Glib::ustring>::Ref   token) const
                                                throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Import a playlist archive to the local storage.
         *  This must be a tar file, in the LS Archive format, as produced
         *  by exportPlaylistOpen/Close() when called with the internalFormat
         *  parameter.
         *
         *  The size of the tar file must be less than 2 GB, because 
         *  the storage server can not deal with larger files.
         *
         *  @param sessionId the session ID from the authentication client
         *  @param path      the path for the playlist archive file.
         *  @return          on success, the unique ID of the imported playlist.
         *
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call or we have not logged in yet.
         */
        virtual Ptr<UniqueId>::Ref
        importPlaylist(Ptr<SessionId>::Ref              sessionId,
                       Ptr<const Glib::ustring>::Ref    path)       const
                                                throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Check the status of the asynchronous network transport operation.
         *
         *  If the return value is
         *  <ul><li>initState or pendingState, then the operation
         *  is in progress, and you need to call this function again until
         *  a different value is returned;</li>
         *      <li>finishedState, then the asynchronous XML-RPC call has
         *  completed normally;</li>
         *      <li>closedState, then the transport has been
         *  closed or canceled, and the token is no longer valid;</li>
         *      <li>failedState, then an error has occured (and the token is
         *  no longer valid); the error message is returned in the (optional)
         *  errorMessage return parameter.
         *  </ul>
         *
         *  @param token        the transport token of an asynchronous method.
         *  @param errorMessage return parameter: if the transport has failed,
         *                      this will contain the error message (optional).
         *  @return the state of the transport.
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call.
         */
        virtual AsyncState
        checkTransport(Ptr<const Glib::ustring>::Ref    token,
                       Ptr<Glib::ustring>::Ref      errorMessage
                                                    = Ptr<Glib::ustring>::Ref())
                                                throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Cancel an asynchronous network transport operation.
         *
         *  @param  sessionId   the session ID from the authentication client.
         *  @param token        the transport token of an asynchronous method.
         *  @return the state of the transport.
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call.
         */
        virtual void
        cancelTransport(Ptr<SessionId>::Ref             sessionId,
                        Ptr<const Glib::ustring>::Ref   token)
                                                throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Upload an audio clip or playlist to the network hub.
         *  The progress of the upload process can be monitored with
         *  checkTransport().
         *
         *  @param  sessionId   the session ID from the authentication client.
         *  @param  id          the ID of the Playable object to be uploaded.
         *  @return a token which identifies this task.
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call.
         */
        virtual Ptr<Glib::ustring>::Ref
        uploadToHub(Ptr<const SessionId>::Ref       sessionId,
                    Ptr<const UniqueId>::Ref        id)
                                                throw (XmlRpcException)
                                                                        = 0;

        /**
         *  Download an audio clip or playlist from the network hub.
         *  The progress of the upload process can be monitored with
         *  checkTransport().
         *
         *  @param  sessionId   the session ID from the authentication client.
         *  @param  id          the ID of the Playable object to be downloaded.
         *  @return a token which identifies this task.
         *  @exception XmlRpcException if there is a problem with the XML-RPC
         *                             call.
         */
        virtual Ptr<Glib::ustring>::Ref
        downloadFromHub(Ptr<const SessionId>::Ref       sessionId,
                        Ptr<const UniqueId>::Ref        id)
                                                throw (XmlRpcException)
                                                                        = 0;
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */


} // namespace StorageClient
} // namespace LiveSupport

#endif // LiveSupport_Storage_StorageClientInterface_h

