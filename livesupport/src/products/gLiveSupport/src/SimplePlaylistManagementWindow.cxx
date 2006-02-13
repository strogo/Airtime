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

/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#include <iostream>
#include <stdexcept>

#include "LiveSupport/Core/TimeConversion.h"

#include "SimplePlaylistManagementWindow.h"


using namespace Glib;

using namespace LiveSupport::Core;
using namespace LiveSupport::Widgets;
using namespace LiveSupport::GLiveSupport;

/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/**
 *  The name of the window, used by the keyboard shortcuts (or by the .gtkrc).
 */
static const Glib::ustring  windowName = "simplePlaylistManagementWindow";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Constructor.
 *----------------------------------------------------------------------------*/
SimplePlaylistManagementWindow :: SimplePlaylistManagementWindow (
                                    Ptr<GLiveSupport>::Ref      gLiveSupport,
                                    Ptr<ResourceBundle>::Ref    bundle)
                                                                    throw ()
          : WhiteWindow(WidgetConstants::playlistsWindowTitleImage,
                        Colors::White,
                        WidgetFactory::getInstance()->getWhiteWindowCorners()),
            LocalizedObject(bundle),
            isPlaylistModified(false),
            gLiveSupport(gLiveSupport)
{
    Ptr<WidgetFactory>::Ref     wf = WidgetFactory::getInstance();
    
    try {
        set_title(*getResourceUstring("windowTitle"));
        nameLabel = Gtk::manage(new Gtk::Label(
                                    *getResourceUstring("nameLabel")));
        saveButton = Gtk::manage(wf->createButton(
                                    *getResourceUstring("saveButtonLabel")));
        closeButton = Gtk::manage(wf->createButton(
                                    *getResourceUstring("closeButtonLabel")));
    } catch (std::invalid_argument &e) {
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }

    nameEntry             = Gtk::manage(wf->createEntryBin());
    nameEntry->signal_changed().connect(sigc::mem_fun(
                    *this, &SimplePlaylistManagementWindow::onTitleEdited ));

    entriesScrolledWindow = Gtk::manage(new Gtk::ScrolledWindow());
    entriesModel          = Gtk::ListStore::create(modelColumns);
    entriesView           = Gtk::manage(wf->createTreeView(entriesModel));

    // set up the entry scrolled window, with the entry treeview inside.
    entriesScrolledWindow->add(*entriesView);
    entriesScrolledWindow->set_policy(Gtk::POLICY_AUTOMATIC,
                                      Gtk::POLICY_AUTOMATIC);

    // Add the TreeView's view columns:
    try {
        entriesView->appendColumn(*getResourceUstring("startColumnLabel"),
                                   modelColumns.startColumn,
                                   60);
        entriesView->appendColumn(*getResourceUstring("titleColumnLabel"),
                                   modelColumns.titleColumn,
                                   200);
        entriesView->appendEditableColumn(
                                  *getResourceUstring("fadeInColumnLabel"),
                                   modelColumns.fadeInColumn,
                                   fadeInColumnId,
                                   60);
        entriesView->appendColumn(*getResourceUstring("lengthColumnLabel"),
                                   modelColumns.lengthColumn,
                                   60);
        entriesView->appendEditableColumn(
                                  *getResourceUstring("fadeOutColumnLabel"),
                                   modelColumns.fadeOutColumn,
                                   fadeOutColumnId,
                                   60);

        statusBar = Gtk::manage(new Gtk::Label(""));
    } catch (std::invalid_argument &e) {
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }

    entriesView->signal_button_press_event().connect_notify(sigc::mem_fun(
                    *this, &SimplePlaylistManagementWindow::onEntryClicked ));
    entriesView->signalCellEdited().connect(sigc::mem_fun(
                    *this, &SimplePlaylistManagementWindow::onFadeInfoEdited ));
    entriesView->signal_key_press_event().connect(sigc::mem_fun(
                    *this, &SimplePlaylistManagementWindow::onKeyPressed));

    // create the right-click entry context menu
    rightClickMenu = Gtk::manage(new Gtk::Menu());
    Gtk::Menu::MenuList&    rightClickMenuList = rightClickMenu->items();

    try {
        rightClickMenuList.push_back(Gtk::Menu_Helpers::MenuElem(
                *getResourceUstring("upMenuItem"),
                sigc::mem_fun(*this,
                        &SimplePlaylistManagementWindow::onUpItem)));
        rightClickMenuList.push_back(Gtk::Menu_Helpers::MenuElem(
                *getResourceUstring("downMenuItem"),
                sigc::mem_fun(*this,
                        &SimplePlaylistManagementWindow::onDownItem)));
        rightClickMenuList.push_back(Gtk::Menu_Helpers::MenuElem(
                *getResourceUstring("removeMenuItem"),
                sigc::mem_fun(*this,
                        &SimplePlaylistManagementWindow::onRemoveItem)));
    } catch (std::invalid_argument &e) {
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }
    
    // construct the "lock fades" check button
    Ptr<Glib::ustring>::Ref     lockFadesCheckButtonLabel;
    try {
        lockFadesCheckButtonLabel = getResourceUstring(
                                                "lockFadesCheckButtonLabel");
    } catch (std::invalid_argument &e) {
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }
    Gtk::CheckButton *  lockFadesCheckButton = Gtk::manage(new Gtk::CheckButton(
                                                *lockFadesCheckButtonLabel ));
    lockFadesCheckButton->set_active(true);
    areFadesLocked = true;
    lockFadesCheckButton->signal_toggled().connect(sigc::mem_fun(
            *this, 
            &SimplePlaylistManagementWindow::onLockFadesCheckButtonClicked ));
        
    // set up the layout
    Gtk::VBox *         mainBox = Gtk::manage(new Gtk::VBox);
    
    Gtk::HBox *         nameBox = Gtk::manage(new Gtk::HBox);
    nameBox->pack_start(*nameLabel, Gtk::PACK_SHRINK, 10);
    Gtk::Alignment *    nameEntryAlignment = Gtk::manage(new Gtk::Alignment(
                                        Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER,
                                        0.7));  // take up 70% of available room
    nameEntryAlignment->add(*nameEntry);
    nameBox->pack_start(*nameEntryAlignment, Gtk::PACK_EXPAND_WIDGET, 5);
    mainBox->pack_start(*nameBox, Gtk::PACK_SHRINK, 5);

    mainBox->pack_start(*entriesScrolledWindow, Gtk::PACK_EXPAND_WIDGET, 5);
    
    mainBox->pack_start(*lockFadesCheckButton, Gtk::PACK_SHRINK, 5);

    Gtk::ButtonBox *    buttonBox = Gtk::manage(new Gtk::HButtonBox(
                                                        Gtk::BUTTONBOX_END, 5));
    buttonBox->pack_start(*saveButton);
    buttonBox->pack_start(*closeButton);
    mainBox->pack_start(*buttonBox, Gtk::PACK_SHRINK, 0);
    
    Gtk::Alignment *    statusBarAlignment = Gtk::manage(new Gtk::Alignment(
                                        Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER,
                                        0.0));  // do not expand the label
    statusBarAlignment->add(*statusBar);
    mainBox->pack_start(*statusBarAlignment, Gtk::PACK_SHRINK, 5);

    add(*mainBox);

    // Register the signal handlers for the buttons
    saveButton->signal_clicked().connect(sigc::mem_fun(*this,
                        &SimplePlaylistManagementWindow::onSaveButtonClicked));
    closeButton->signal_clicked().connect(sigc::mem_fun(*this,
                        &SimplePlaylistManagementWindow::onCloseButtonClicked));

    // show
    set_name(windowName);
    set_default_size(480, 350);
    set_modal(false);
    property_window_position().set_value(Gtk::WIN_POS_NONE);

    show_all();

    Ptr<Glib::ustring>::Ref     confirmationMessage;
    try {
        confirmationMessage.reset(new Glib::ustring(
                                *getResourceUstring("savePlaylistDialogMsg") ));
    } catch (std::invalid_argument &e) {
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }

    dialogWindow.reset(new DialogWindow(confirmationMessage,
                                        DialogWindow::cancelButton |
                                        DialogWindow::noButton |
                                        DialogWindow::yesButton,
                                        gLiveSupport->getBundle() ));

    gLiveSupport->signalEditedPlaylistModified().connect(sigc::mem_fun(
            *this, &SimplePlaylistManagementWindow::onPlaylistModified ));
}


/*------------------------------------------------------------------------------
 *  Destructor.
 *----------------------------------------------------------------------------*/
SimplePlaylistManagementWindow :: ~SimplePlaylistManagementWindow (void)
                                                                    throw ()
{
}


/*------------------------------------------------------------------------------
 *  Save the edited playlist.
 *----------------------------------------------------------------------------*/
bool
SimplePlaylistManagementWindow :: savePlaylist(bool reopen)         throw ()
{
    try {
        Ptr<Playlist>::Ref              playlist
                                        = gLiveSupport->getEditedPlaylist();
        if (!playlist) {
            return false;
        }

        Ptr<const Glib::ustring>::Ref   title(new Glib::ustring(
                                                    nameEntry->get_text()));
        if (*title == "") {
            statusBar->set_text(*getResourceUstring("emptyTitleErrorMsg"));
            return false;
        }
        
        playlist->setTitle(title);
        gLiveSupport->savePlaylist();
        if (reopen) {
            gLiveSupport->openPlaylistForEditing(playlist->getId());
        }
        isPlaylistModified = false;

        Ptr<Glib::ustring>::Ref statusText = formatMessage(
                                                    "playlistSavedMsg",
                                                    *playlist->getTitle());
        statusBar->set_text(*statusText);
        return true;
        
    } catch (XmlRpcException &e) {
        statusBar->set_text(e.what());
        return false;
    }
}


/*------------------------------------------------------------------------------
 *  Signal handler for the save button getting clicked.
 *----------------------------------------------------------------------------*/
void
SimplePlaylistManagementWindow :: onSaveButtonClicked(void)         throw ()
{
    savePlaylist(true);
}


/*------------------------------------------------------------------------------
 *  Signal handler for the close button getting clicked.
 *----------------------------------------------------------------------------*/
void
SimplePlaylistManagementWindow :: onCloseButtonClicked(void)        throw ()
{
    if (cancelPlaylist()) {
        closeWindow();
    }
}


/*------------------------------------------------------------------------------
 *  Cancel the edited playlist, after asking for confirmation.
 *----------------------------------------------------------------------------*/
bool
SimplePlaylistManagementWindow :: cancelPlaylist(void)        throw ()
{
    if (gLiveSupport->getEditedPlaylist()) {
        if (!isPlaylistModified) {
            gLiveSupport->cancelEditedPlaylist();
        } else {
            DialogWindow::ButtonType    result = dialogWindow->run();
            switch (result) {
                case DialogWindow::noButton:
                                gLiveSupport->cancelEditedPlaylist();
                                break;

                case DialogWindow::yesButton:
                                if (!savePlaylist(false)) {
                                    return false;
                                }
                                break;

                case DialogWindow::cancelButton:
                                return false;

                default :       return false;
                                        // can happen if window is closed
            }                           //   with Alt-F4 -- treated as cancel
        }
    }
    
    return true;
}


/*------------------------------------------------------------------------------
 *  Clean and close the window.
 *----------------------------------------------------------------------------*/
void
SimplePlaylistManagementWindow :: closeWindow(void)                 throw ()
{
    statusBar->set_text("");
    nameEntry->set_text("");
    entriesModel->clear();
    isPlaylistModified = false;
    gLiveSupport->putWindowPosition(shared_from_this());
    hide();
}


/*------------------------------------------------------------------------------
 *  Signal handler for the "lock fades" check button toggled.
 *----------------------------------------------------------------------------*/
void
SimplePlaylistManagementWindow :: onLockFadesCheckButtonClicked(void)
                                                                    throw ()
{
    areFadesLocked = !areFadesLocked;
}


/*------------------------------------------------------------------------------
 *  Show the contents of the currently edited playlist.
 *----------------------------------------------------------------------------*/
void
SimplePlaylistManagementWindow :: showContents(void)                throw ()
{
    Ptr<Playlist>::Ref          playlist;
    Playlist::const_iterator    it;
    Playlist::const_iterator    end;
    int                         rowNumber;

    playlist = gLiveSupport->getEditedPlaylist();
    
    if (playlist) {
        nameEntry->set_text(*playlist->getTitle());
        entriesModel->clear();
        rowNumber = 0;
        for (it = playlist->begin(); it != playlist->end(); ++it) {
            Ptr<PlaylistElement>::Ref playlistElement
                                          = it->second;
            Ptr<Playable>::Ref   playable = playlistElement->getPlayable();
            Gtk::TreeModel::Row  row      = *(entriesModel->append());
    
            row[modelColumns.playlistElementColumn]
                        = playlistElement;
            row[modelColumns.rowNumberColumn]
                        = rowNumber++;
            row[modelColumns.startColumn]
                        = *TimeConversion::timeDurationToHhMmSsString(
                                        playlistElement->getRelativeOffset());
            row[modelColumns.titleColumn]
                        = Glib::Markup::escape_text(*playable->getTitle());
            row[modelColumns.lengthColumn]
                        = *TimeConversion::timeDurationToHhMmSsString(
                                        playable->getPlaylength());

            Ptr<FadeInfo>::Ref      fadeInfo = playlistElement->getFadeInfo();
            Ptr<time_duration>::Ref fadeIn, fadeOut;
            if (fadeInfo) {
                fadeIn  = fadeInfo->getFadeIn();
                fadeOut = fadeInfo->getFadeOut();
            }
            row[modelColumns.fadeInColumn]
                        = (fadeIn && fadeIn->total_microseconds() != 0)
                          ? *TimeConversion::timeDurationToShortString(fadeIn)
                          : "-   ";
            row[modelColumns.fadeOutColumn]
                        = (fadeOut && fadeOut->total_microseconds() != 0)
                          ? *TimeConversion::timeDurationToShortString(fadeOut)
                          : "-   ";
        }
    }
}


/*------------------------------------------------------------------------------
 *  Signal handler for the fade info being edited.
 *----------------------------------------------------------------------------*/
void
SimplePlaylistManagementWindow :: onTitleEdited(void)               throw()
{
    Ptr<Playlist>::Ref          playlist = gLiveSupport->getEditedPlaylist();
    if (!playlist) {
        playlist = gLiveSupport->openPlaylistForEditing();
    }
    Ptr<Glib::ustring>::Ref     title(new Glib::ustring(
                                                    nameEntry->get_text()));
    if (*title != *playlist->getTitle()) {
        playlist->setTitle(title);
        isPlaylistModified = true;
    }
    
    showContents();
}


/*------------------------------------------------------------------------------
 *  Signal handler for the fade info being edited.
 *----------------------------------------------------------------------------*/
void
SimplePlaylistManagementWindow :: onFadeInfoEdited(
                                        const Glib::ustring &  pathString,
                                        int                    columnId,
                                        const Glib::ustring &  newText)
                                                                    throw()
{
    Gtk::TreeModel::Path        path(pathString);
    std::vector<int>            rowNumberVector = path.get_indices();
    int                         rowNumber = rowNumberVector.at(0);
    
    Ptr<time_duration>::Ref     newTime;
    try {
        Ptr<std::string>::Ref   newTextPtr(new std::string(newText));
        newTime = TimeConversion::parseTimeDuration(newTextPtr);
    } catch (boost::bad_lexical_cast &e) {
        showContents();         // bad time format; restore previous state
        return;
    }
    
    Ptr<Playlist>::Ref          playlist = gLiveSupport->getEditedPlaylist();
    Playlist::const_iterator    iter = playlist->begin();
    for (int i=0; i<rowNumber; ++i) {
        ++iter;
    }
    // TODO: add an at(n) access function to Playlist
    Ptr<PlaylistElement>::Ref   playlistElement = iter->second;
    
    switch (columnId) {
        case fadeInColumnId :
            setFadeIn(playlistElement, newTime);
            if (areFadesLocked && iter-- != playlist->begin()) {
                Ptr<PlaylistElement>::Ref   prevPlaylistElement = iter->second;
                setFadeOut(prevPlaylistElement, newTime);
            }
            break;
        case fadeOutColumnId :
            setFadeOut(playlistElement, newTime);
            if (areFadesLocked && ++iter != playlist->end()) {
                Ptr<PlaylistElement>::Ref   nextPlaylistElement = iter->second;
                setFadeIn(nextPlaylistElement, newTime);
            }
            break;
        default :
            return;         // should never happen
    }
    
    showContents();
}


/*------------------------------------------------------------------------------
 *  Auxilliary function: set the fade in of a playlist element.
 *----------------------------------------------------------------------------*/
void
SimplePlaylistManagementWindow :: setFadeIn(
                          Ptr<PlaylistElement>::Ref     playlistElement,
                          Ptr<time_duration>::Ref       newFadeIn)
                                                                    throw()
{
    Ptr<FadeInfo>::Ref          oldFadeInfo = playlistElement->getFadeInfo();
    Ptr<time_duration>::Ref     oldFadeOut;
    if (oldFadeInfo) {
        if (*oldFadeInfo->getFadeIn() == *newFadeIn) {
            return;
        }
        oldFadeOut  = oldFadeInfo->getFadeOut();
    } else {
        oldFadeOut.reset(new time_duration(0,0,0,0));
    }
    Ptr<FadeInfo>::Ref          newFadeInfo(new FadeInfo(
                                                newFadeIn, oldFadeOut ));
    if (isLengthOkay(playlistElement, newFadeInfo)) {
        playlistElement->setFadeInfo(newFadeInfo);
        isPlaylistModified = true;
    }
}


/*------------------------------------------------------------------------------
 *  Auxilliary function: set the fade out of a playlist element.
 *----------------------------------------------------------------------------*/
void
SimplePlaylistManagementWindow :: setFadeOut(
                            Ptr<PlaylistElement>::Ref     playlistElement,
                            Ptr<time_duration>::Ref       newFadeOut)
                                                                    throw()
{
    Ptr<FadeInfo>::Ref          oldFadeInfo = playlistElement->getFadeInfo();
    Ptr<time_duration>::Ref     oldFadeIn;
    if (oldFadeInfo) {
        if (*oldFadeInfo->getFadeOut() == *newFadeOut) {
            return;
        }
        oldFadeIn = oldFadeInfo->getFadeIn();
    } else {
        oldFadeIn.reset(new time_duration(0,0,0,0));
    }
    Ptr<FadeInfo>::Ref          newFadeInfo(new FadeInfo(
                                                    oldFadeIn, newFadeOut ));
    if (isLengthOkay(playlistElement, newFadeInfo)) {
        playlistElement->setFadeInfo(newFadeInfo);
        isPlaylistModified = true;
    }
}


/*------------------------------------------------------------------------------
 *  Auxilliary function: check that fades are not longer than the whole clip.
 *----------------------------------------------------------------------------*/
inline bool
SimplePlaylistManagementWindow :: isLengthOkay(
                            Ptr<PlaylistElement>::Ref     playlistElement,
                            Ptr<FadeInfo>::Ref            newFadeInfo)
                                                                    throw()
{
    time_duration   totalFades = *newFadeInfo->getFadeIn()
                               + *newFadeInfo->getFadeOut();
    return (totalFades <= *playlistElement->getPlayable()->getPlaylength());
}


/*------------------------------------------------------------------------------
 *  Signal handler for the playlist being modified outside the window.
 *----------------------------------------------------------------------------*/
void
SimplePlaylistManagementWindow :: onPlaylistModified(void)          throw()
{
    isPlaylistModified = true;
}


/*------------------------------------------------------------------------------
 *  Event handler for an entry being clicked in the list
 *----------------------------------------------------------------------------*/
void
SimplePlaylistManagementWindow :: onEntryClicked(GdkEventButton * event)
                                                                    throw()
{
    if (event->type == GDK_BUTTON_PRESS && event->button == 3) {
        Gtk::TreePath           currentPath;
        Gtk::TreeViewColumn *   column;
        int     cell_x,
                cell_y;
        bool foundValidRow = entriesView->get_path_at_pos(
                                            int(event->x), int(event->y),
                                            currentPath, column,
                                            cell_x, cell_y);

        if (foundValidRow) {
            currentItem = entriesModel->get_iter(currentPath);
            if (currentItem) {
                rightClickMenu->popup(event->button, event->time);
            }
        }
    }
}


/*------------------------------------------------------------------------------
 *  Event handler for the Up menu item selected from the context menu.
 *----------------------------------------------------------------------------*/
void
SimplePlaylistManagementWindow :: onUpItem(void)                    throw()
{
    if (currentItem && currentItem != entriesModel->children().begin()) {
        int             rowNumber    = (*currentItem)
                                            [modelColumns.rowNumberColumn];
        Gtk::TreeIter   previousItem = currentItem;
        --previousItem;
        swapPlaylistElements(previousItem, currentItem);
        isPlaylistModified = true;
        showContents();
        selectRow(--rowNumber);
    }
}


/*------------------------------------------------------------------------------
 *  Event handler for the Down menu item selected from the context menu.
 *----------------------------------------------------------------------------*/
void
SimplePlaylistManagementWindow :: onDownItem(void)                  throw()
{
    if (currentItem) {
        Gtk::TreeIter   nextItem  = currentItem;
        ++nextItem;
        if (nextItem) {
            int         rowNumber = (*currentItem)
                                            [modelColumns.rowNumberColumn];
            swapPlaylistElements(currentItem, nextItem);
            isPlaylistModified = true;
            showContents();
            selectRow(++rowNumber);
        }
    }
}


/*------------------------------------------------------------------------------
 *  Swap two playlist elements in the edited playlist.
 *----------------------------------------------------------------------------*/
void
SimplePlaylistManagementWindow :: swapPlaylistElements(
                                        Gtk::TreeIter   firstIter,
                                        Gtk::TreeIter   secondIter)
                                                                    throw()
{
    Ptr<PlaylistElement>::Ref
            firstElement  = (*firstIter) [modelColumns.playlistElementColumn];
    Ptr<PlaylistElement>::Ref
            secondElement = (*secondIter)[modelColumns.playlistElementColumn];

    // remove the two playlist elements
    Ptr<Playlist>::Ref  playlist = gLiveSupport->getEditedPlaylist();
    playlist->removePlaylistElement(firstElement->getId());
    playlist->removePlaylistElement(secondElement->getId());

    // swap the relative offsets so that elt2.begin <-- elt1.begin
    //                               and elt1.end   <-- elt2.end   
    Ptr<time_duration>::Ref     firstStart = firstElement->getRelativeOffset();
    Ptr<time_duration>::Ref     secondStart(new time_duration(
                                        *secondElement->getRelativeOffset()
                                        + *secondElement->getPlayable()
                                                        ->getPlaylength()
                                        - *firstElement->getPlayable()
                                                       ->getPlaylength() ));
    firstElement->setRelativeOffset(secondStart);
    secondElement->setRelativeOffset(firstStart);

    // read the fade infos
    bool hasFadeInfo = false;
    Ptr<FadeInfo>::Ref          firstFadeInfo  = firstElement->getFadeInfo();
    Ptr<FadeInfo>::Ref          secondFadeInfo = secondElement->getFadeInfo();
    Ptr<time_duration>::Ref     beginFade,
                                midFade1,
                                midFade2,
                                endFade;

    if (firstFadeInfo) {
        hasFadeInfo = true;
        beginFade = firstFadeInfo->getFadeIn();
        midFade1  = firstFadeInfo->getFadeOut();
    } else {
        beginFade.reset(new time_duration(0,0,0,0));
        midFade1 .reset(new time_duration(0,0,0,0));
    }
    
    if (secondFadeInfo) {
        hasFadeInfo = true;
        midFade2  = secondFadeInfo->getFadeIn();
        endFade   = secondFadeInfo->getFadeOut();
    } else if (hasFadeInfo) {
        midFade2.reset(new time_duration(0,0,0,0));
        endFade .reset(new time_duration(0,0,0,0));
    }    

    // move fades around if they seem to be simple crossfades
    // otherwise, just leave them as they are
    if (hasFadeInfo && *midFade1 == *midFade2) {
        Ptr<FadeInfo>::Ref  firstFadeInfo (new FadeInfo(beginFade, midFade1));
        Ptr<FadeInfo>::Ref  secondFadeInfo(new FadeInfo(midFade1,  endFade ));

        firstElement->setFadeInfo(secondFadeInfo);
        secondElement->setFadeInfo(firstFadeInfo);
    }

    // add the playlist elements back in
    playlist->addPlaylistElement(firstElement);
    playlist->addPlaylistElement(secondElement);
    
    // Note:
    // removing and then adding is necessary to make sure that the playlist
    // elements are correctly indexed by their relative offset in the playlist.
}


/*------------------------------------------------------------------------------
 *  Event handler for the Remove menu item selected from the context menu.
 *----------------------------------------------------------------------------*/
void
SimplePlaylistManagementWindow :: onRemoveItem(void)                throw()
{
    if (currentItem) {
        Ptr<Playlist>::Ref 
                playlist        = gLiveSupport->getEditedPlaylist();
        Ptr<PlaylistElement>::Ref 
                playlistElement = (*currentItem)
                                        [modelColumns.playlistElementColumn];
    
        playlist->removePlaylistElement(playlistElement->getId());
        playlist->eliminateGaps();

        isPlaylistModified = true;
        showContents();
    }
}


/*------------------------------------------------------------------------------
 *  Event handler for a key pressed.
 *----------------------------------------------------------------------------*/
bool
SimplePlaylistManagementWindow :: onKeyPressed(GdkEventKey *    event)
                                                                    throw ()
{
    if (event->type == GDK_KEY_PRESS) {
        KeyboardShortcut::Action    action = gLiveSupport->findAction(
                                                    windowName,
                                                    event->state,
                                                    event->keyval);
        switch (action) {
            case KeyboardShortcut::moveItemUp :
                                    findCurrentItem();
                                    onUpItem();
                                    return true;

            case KeyboardShortcut::moveItemDown :
                                    findCurrentItem();
                                    onDownItem();
                                    return true;
            
            case KeyboardShortcut::removeItem :
                                    findCurrentItem();
                                    onRemoveItem();
                                    return true;
            
            default :               break;
        }
    }

    return false;
}


/*------------------------------------------------------------------------------
 *  Find (an iterator pointing to) the currently selected row.
 *----------------------------------------------------------------------------*/
void
SimplePlaylistManagementWindow :: findCurrentItem(void)             throw ()
{
    Glib::RefPtr<Gtk::TreeView::Selection>  selection
                                            = entriesView->get_selection();
    currentItem = selection->get_selected();
}


/*------------------------------------------------------------------------------
 *  Select (highlight) the nth row.
 *----------------------------------------------------------------------------*/
void
SimplePlaylistManagementWindow :: selectRow(int rowNumber)          throw ()
{
    Gtk::TreeModel::iterator    iter = entriesModel->children().begin();
    for (; rowNumber > 0; --rowNumber) {
        ++iter;
    }
    if (iter) {
        Glib::RefPtr<Gtk::TreeView::Selection>  selection
                                                = entriesView->get_selection();
        selection->select(iter);
    }
}

