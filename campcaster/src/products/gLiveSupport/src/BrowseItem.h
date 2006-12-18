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
#ifndef LiveSupport_GLiveSupport_BrowseItem_h
#define LiveSupport_GLiveSupport_BrowseItem_h

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#include <vector>
#include <utility>

#include <gtkmm.h>

#include "LiveSupport/Core/Ptr.h"
#include "LiveSupport/Core/LocalizedObject.h"
#include "LiveSupport/Core/SearchCriteria.h"
#include "LiveSupport/Widgets/ComboBoxText.h"
#include "LiveSupport/Widgets/ZebraTreeView.h"
#include "LiveSupport/Widgets/ZebraTreeModelColumnRecord.h"

#include "GLiveSupport.h"


namespace LiveSupport {
namespace GLiveSupport {

using namespace LiveSupport::Core;
using namespace LiveSupport::Widgets;
    
/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  A single browse input field.
 *
 *  It consists of a Widgets::ComboBoxText and a Widgets::ZebraTreeView
 *  (without header).  It stores a "parent search criteria", and shows all
 *  possible metadata values of the type selected in the ComboBoxText which
 *  match this condition.  The parent search criteria should be conjunction
 *  of all search conditions selected in BrowseItem objects to the left of
 *  this one.
 *
 *  @author  $Author$
 *  @version $Revision$
 */
class BrowseItem : public Gtk::VBox,
                   public LocalizedObject
{
    private:
    
        /**
         *  The type for storing both the metadata and the comparison operator
         *  localizations.
         */
        typedef std::vector<std::pair<Glib::ustring, Glib::ustring> >
                                MapVector;

        /**
         *  The metadata field.
         */
        MetadataComboBoxText *  metadataEntry;
        
        /**
         *  The selection field.
         */
        ZebraTreeView *         metadataValues;
        
        /**
         *  The columns model needed by Gtk::TreeView.
         *  Lists one clip per row.
         *
         *  @author $Author$
         *  @version $Revision$
         */
        class ModelColumns : public ZebraTreeModelColumnRecord
        {
            public:
                /**
                 *  The original (unescaped) value of metadata.
                 */
                Gtk::TreeModelColumn<Glib::ustring>     originalColumn;
                
                /**
                 *  The single displayed column.
                 */
                Gtk::TreeModelColumn<Glib::ustring>     displayedColumn;
                
                /**
                 *  Constructor.
                 */
                ModelColumns(void)                              throw ()
                {
                    add(originalColumn);
                    add(displayedColumn);
                }
        };
        
        /**
         *  The column model.
         */
        ModelColumns                    modelColumns;

        /**
         *  The tree model, as a GTK reference.
         */
        Glib::RefPtr<Gtk::ListStore>    treeModel;

        /**
         *  The localized version of the "--- all ---" string.
         */
        Glib::ustring                   allString;

        /**
         *  The GLiveSupport object, holding the state of the application.
         */
        Ptr<LiveSupport::GLiveSupport::GLiveSupport>::Ref   gLiveSupport;
         
        /**
         *  The criteria from the browse items to the left of this one.
         */
        Ptr<SearchCriteria>::Ref        parentCriteria;
         
        /**
         *  Default constructor.
         */
        BrowseItem(void)                               throw ();

        /**
         *  Emit the "selection changed" signal.
         */
        void
        emitSignalSelectionChanged(void)        throw ()
        {
            signalSelectionChanged().emit();
        }


    protected:
    
        /**
         *  A signal object to notify people that the selection has changed.
         */
        sigc::signal<void>              signalSelectionChangedObject;


    public:
    
        /**
         *  Constructor with parent and localization parameter.
         *
         *  @param gLiveSupport the main program object
         *  @param bundle       the resource bundle for localization
         *  @param defaultIndex the index of the metadata entry to display
         *                      initially
         */
        BrowseItem(
            Ptr<LiveSupport::GLiveSupport::GLiveSupport>::Ref   gLiveSupport,
            Ptr<ResourceBundle>::Ref                            bundle,
            int                                                 defaultIndex)
                                                       throw ();

        /**
         *  A virtual destructor.
         */
        virtual
        ~BrowseItem(void)                              throw ()
        {
        }

        /**
         *  Return the search criteria selected by the user.
         *  This is the parent criteria (assumed to have operator "and")
         *  with the search condition showing the current selection added
         *  (if any).
         *
         *  @return a new LiveSupport::StorageClient::SearchCriteria instance,
         *          which contains the data entered by the user
         */
        Ptr<SearchCriteria>::Ref
        getSearchCriteria(void)                 throw (std::invalid_argument);
        
        /**
         *  Fill in the column with the possible values (limited by the
         *  parent criteria), and set the selection to "all".
         */
        void
        onShow(void)                                    throw ();

        /**
         *  The signal handler for refreshing the treeview of metadata values,
         *  if we also need to change the parent criteria.  Same as onShow(),
         *  plus changing the parent criteria.
         *
         *  @param criteria     the new parent search criteria
         */
        void
        onParentChangedShow(BrowseItem *    leftNeighbor)
                                                        throw ()
        {
            parentCriteria = leftNeighbor->getSearchCriteria();
            onShow();
        }
        
        /**
         *  The signal raised when either the combo box or the tree view
         *  selection has changed.
         *
         *  @return the signal object (a protected member of this class)
         */
        sigc::signal<void>
        signalSelectionChanged(void)                        throw ()
        {
            return signalSelectionChangedObject;
        }
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */


} // namespace GLiveSupport
} // namespace LiveSupport

#endif // LiveSupport_GLiveSupport_BrowseItem_h

