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
#ifndef UploadFileWindow_h
#define UploadFileWindow_h

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#include <gtkmm/button.h>
#include <gtkmm/table.h>
#include <gtkmm/entry.h>
#include <gtkmm/alignment.h>
#include <gtkmm/box.h>
#include <gtkmm/window.h>
#include <gtkmm/scrolledwindow.h>

#include "LiveSupport/Core/Ptr.h"
#include "LiveSupport/Core/LocalizedObject.h"
#include "LiveSupport/Widgets/Button.h"
#include "LiveSupport/Widgets/EntryBin.h"
#include "LiveSupport/Widgets/ComboBoxText.h"
#include "LiveSupport/Widgets/Notebook.h"
#include "LiveSupport/Widgets/WhiteWindow.h"

#include "GLiveSupport.h"
#include "MasterPanelUserInfoWidget.h"

namespace LiveSupport {
namespace GLiveSupport {

using namespace LiveSupport::Core;
using namespace LiveSupport::Widgets;

/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  The upload file window.
 *
 *  The layout of the window is roughly the following:
 *  <pre><code>
 *  +--- upload file window ----------------+
 *  | choose file:     +-- file browser --+ |
 *  | name:            +-- name input ----+ |
 *  |                  +-- upload button -+ |
 *  |                  +-- close button --+ |
 *  | +-- status bar ---------------------+ |
 *  +---------------------------------------+
 *  </code></pre>
 *
 *  @author $Author$
 *  @version $Revision$
 */
class UploadFileWindow : public WhiteWindow, public LocalizedObject
{
    protected:
        /**
         *  The layout used in the window.
         */
        Gtk::Box                 * layout;

        /**
         *  The choose file label
         */
        Gtk::Label                * chooseFileLabel;

        /**
         *  A container holding the file name entry field.
         */
        EntryBin                  * fileNameEntryBin;

        /**
         *  The text entry for selecting a file name
         */
        Gtk::Entry                * fileNameEntry;

        /**
         *  The file browser button.
         */
        Button                    * chooseFileButton;

        /**
         *  The notepad holding the different sections of metadata.
         */
        Notebook                  * metadataNotebook;

        /**
         *  The layout of the main section.
         */
        Gtk::Table                * mainLayout;

        /**
         *  The layout of the music section.
         */
        Gtk::Table                * musicLayout;

        /**
         *  The layout of the talk section.
         */
        Gtk::Table                * talkLayout;

        /**
         *  A list of the Dublin Core names of the metadata fields.
         */
        std::vector<Ptr<const Glib::ustring>::Ref>  metadataKeys;
        
        /**
         *  A list of the metadata entry fields.
         */
        std::vector<Gtk::Entry *>                   metadataEntries;

        /**
         *  The label containing the "Length: " text.
         */
        Gtk::Label                * lengthLabel;

        /**
         *  The length value label.
         */
        Gtk::Label                * lengthValueLabel;

        /**
         *  The button box.
         */
        Gtk::ButtonBox            * buttonBox;

        /**
         *  The upload button.
         */
        Gtk::Button               * uploadButton;

        /**
         *  The close button.
         */
        Gtk::Button               * closeButton;

        /**
         *  The status bar.
         */
        Gtk::Label                * statusBar;

        /**
         *  The gLiveSupport object, handling the logic of the application.
         */
        Ptr<GLiveSupport>::Ref      gLiveSupport;

        /**
         *  The name of the file to upload.
         */
        Ptr<AudioClip>::Ref         audioClip;

        /**
         *  Signals if the audio clip is valid.
         */
        bool                        isAudioClipValid;

        /**
         *  Function to catch the event of the choose file button being
         *  pressed.
         */
        virtual void
        onChooseFileButtonClicked(void)                     throw ();

        /**
         *  Function to catch the event of the upload button being
         *  pressed.
         */
        virtual void
        onUploadButtonClicked(void)                         throw ();

        /**
         *  Signal handler for the user leaving the filename entry box,
         *  where persumably he may have types in a new filename.
         *
         *  @param event the event recieved.
         *  @return true if the event has been processed, false otherwise.
         */
        bool
        onFileNameEntryLeave(GdkEventFocus    * event)      throw ();

        /**
         *  Function to catch the event of the close button being pressed.
         */
        virtual void
        onCloseButtonClicked(void)                          throw ();

        /**
         *  Update the information for the file to upload, based on the
         *  value of the fileNameEntry text entry field.
         */
        void
        updateFileInfo(void)                                throw ();

        /**
         *  Determine the length of an audio file on disk.
         *
         *  @param fileName     a binary audio file (e.g., /tmp/some_clip.mp3)
         *  @return             the length of the file; a null pointer if the
         *                      length could not be read (see bug #1426)
         *  @exception std::invalid_argument if the file is not found, or its
         *                                   format is not supported by TagLib
         */
        Ptr<time_duration>::Ref
        readPlaylength(const std::string &  fileName)
                                                throw (std::invalid_argument);


    public:
        /**
         *  Constructor.
         *
         *  @param gLiveSupport the gLiveSupport object, handling the
         *         logic of the application
         *  @param bundle the resource bundle holding localized resources
         */
        UploadFileWindow(Ptr<GLiveSupport>::Ref     gLiveSupport,
                         Ptr<ResourceBundle>::Ref   bundle)
                                                                throw ();

        /**
         *  Virtual destructor.
         */
        virtual
        ~UploadFileWindow(void)                                 throw ()
        {
        }

};

/* ================================================= external data structures */


/* ====================================================== function prototypes */


} // namespace GLiveSupport
} // namespace LiveSupport

#endif // UploadFileWindow_h

