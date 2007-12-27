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
#ifndef LiveSupport_Core_SearchCriteria_h
#define LiveSupport_Core_SearchCriteria_h

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#include <stdexcept>
#include <cctype>
#include <XmlRpcValue.h>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "LiveSupport/Core/Ptr.h"

// forward declaration of friend class
namespace LiveSupport {
namespace StorageClient {
    class TestStorageClient;
} }

namespace LiveSupport {
namespace Core {

using namespace boost::posix_time;

/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  An object which contains a collection of search conditions.
 * 
 *  Its fields are:
 *  <ul>
 *    <li>type     - values in (audioClip | playlist | all); the default is
 *                          <i>all</i></li>
 *    <li>operator - values in (and | or); the default is <i>and</i></li>
 *    <li>condition1 : { key : string, comparison: string, value : string }
 *                          - a search condition, where <i>key</i> is one of the
 *                            fields in the metadata, and <i>comparison</i> is
 *                            one of ("=" | "partial" | "prefix" 
 *                                        | "<" | "<=" | ">" | ">=")</li>
 *    <li>...</li>
 *    <li>conditionN</li>
 *    <li>limit  : int - the maximum number of results to be returned;
 *                       the default is 0, which means unlimited</li>
 *    <li>offset : int - ignore the first <i>offset</i> matches;
 *                       the default is 0.</li>
 *  </ul>
 *
 *  Usage: construct a SearchCriteria object either directly using the 
 *  constructor with 4 string arguments, or in several steps using 
 *  addCondition() and the setter methods; 
 *  then pass this object to StorageClientInterface::search() 
 *  to search the local storage.
 *
 *  The <i>key</i> and <i>value</i> fields are case-sensitive, all the other
 *  strings (type, operator names) are case-insensitive.
 */
class SearchCriteria
{
    public:
    
        /**
         *  A type to hold a single search condition.
         */
        struct SearchConditionType
        {
            std::string key;
            std::string comparisonOperator;
            std::string value;
            
            SearchConditionType(const std::string &     key,
                                const std::string &     comparisonOperator,
                                const std::string &     value)
                : key(key), comparisonOperator(comparisonOperator), value(value)
            {
            }
        };
        
    private:

        /**
         *  The kind of object we are searching for.
         */
        std::string                         type;

        /**
         *  The logical operator joining the conditions: "and" or "or".
         */
        std::string                         logicalOperator;

        /**
         *  A type to hold the list of search conditions.
         */
        typedef std::vector<SearchConditionType>
                                            SearchConditionListType;

        /**
         *  The vector of search conditions.
         */
        SearchConditionListType             searchConditions;

        /**
         *  The mtime to use for "ls:mtime".
         */
        Ptr<const ptime>::Ref               mtimeValue;

        /**
         *  The comparison operator to use for "ls:mtime".
         */
        std::string                         mtimeComparisonOperator;

        /**
         *  The maximum number of conditions to be returned.
         */
        int                                 limit;

        /**
         *  The index of the first matching condition to be returned.
         */
        int                                 offset;

        /**
         *  Lowercase a string.
         */
        std::string
        lowerCase(const std::string & s)        throw()
        {
            std::string                     returnValue;
            std::string::const_iterator     it = s.begin();
            while (it != s.end()) {
                returnValue += std::tolower(*it);
                ++it;
            }
            return returnValue;
        }

        /**
         *  Give access of private members to the TestStorageClient.
         */
        friend class LiveSupport::StorageClient::TestStorageClient;

        /**
         *  Add a search condition.
         *
         *  @param xmlRpcValue  the condition to add.
         *  @exception  std::invalid_argument   on syntax errors.
         */
        void
        addCondition(const XmlRpc::XmlRpcValue &    xmlRpcValue)
                                                throw(std::invalid_argument);


    public:

        /**
         *  Construct an empty SearchCriteria object.
         *  This also works as a default constructor.
         *
         *  @param type one of "audioClip" (default), "playlist" or "all"
         *  @param logicalOperator either "and" (default) or "or"
         *  @exception  std::invalid_argument   on syntax errors.
         */
        SearchCriteria(const std::string & type = "all", 
                       const std::string & logicalOperator = "and")
                                                throw(std::invalid_argument)
                       : limit(0), offset(0)
        {
            setType(type);
            setLogicalOperator(logicalOperator);
        }

        /**
         *  Construct a SearchCriteria object with a single condition.
         *
         *  @param type one of "audioClip", "playlist" or "all"
         *  @param key  the metadata field to search in
         *  @param comparisonOperator one of "=", "partial", "prefix",
         *                            "<", "<=", ">" or ">="
         *  @param value the value to compare to
         *  @exception  std::invalid_argument   on syntax errors.
         */
        SearchCriteria(const std::string & type, 
                       const std::string & key,
                       const std::string & comparisonOperator,
                       const std::string & value)
                                                throw(std::invalid_argument);

        /**
         *  Construct a SearchCriteria object from an XmlRpcValue.
         *
         *  @param xmlRpcValue  an XmlRpcValue struct, containing
         *                      the search criteria.
         *  @exception  std::invalid_argument   on syntax errors.
         */
        SearchCriteria(const XmlRpc::XmlRpcValue &      xmlRpcValue)
                                                throw(std::invalid_argument);

        /**
         *  Set the type field.
         *
         *  @param type one of "audioClip", "playlist" or "all"
         *  @exception  std::invalid_argument   on syntax errors.
         */
        void
        setType(const std::string & type)
                                                throw(std::invalid_argument)
        {
            std::string  lowerCaseType = lowerCase(type);
            if (lowerCaseType == "audioclip" 
                    || lowerCaseType == "playlist"
                    || lowerCaseType == "all") {
                this->type = lowerCaseType;
            } else {
                throw std::invalid_argument("bad type argument");
            }

        }

        /**
         *  Set the logical operator field.
         *
         *  @param logicalOperator either "and" or "or"
         *  @exception  std::invalid_argument   on syntax errors.
         */
        void
        setLogicalOperator(const std::string & logicalOperator)
                                                throw(std::invalid_argument)
        {
            std::string  lowerCaseOp = lowerCase(logicalOperator);
            if (lowerCaseOp == "and" || lowerCaseOp == "or") {
                this->logicalOperator = lowerCaseOp;
            } else {
                throw std::invalid_argument("bad logical operator argument");
            }
        }

        /**
         *  Add a search condition.
         *
         *  @param key  the metadata field to search in
         *  @param comparisonOperator one of "=", "partial", "prefix",
         *                            "<", "<=", ">" or ">="
         *  @param value the value to compare to
         *  @exception  std::invalid_argument   on syntax errors.
         */
        void
        addCondition(const std::string & key,
                     const std::string & comparisonOperator,
                     const std::string & value)
                                                throw(std::invalid_argument);

        /**
         *  Add a search condition specifying the mtime (modified-at time).
         *
         *  @param comparisonOperator one of "=", "partial", "prefix",
         *                            "<", "<=", ">" or ">="
         *  @param value the value of the mtime to compare to
         *  @exception  std::invalid_argument   on syntax errors.
         */
        void
        addMtimeCondition(const std::string &       comparisonOperator,
                          Ptr<const ptime>::Ref     value)
                                                throw(std::invalid_argument);

        /**
         *  Add a search condition.
         *
         *  @param condition the search condition to add
         *  @exception  std::invalid_argument   on syntax errors.
         */
        void
        addCondition(const Ptr<SearchConditionType>::Ref  condition)
                                                throw(std::invalid_argument)
        {
            addCondition(condition->key,
                         condition->comparisonOperator,
                         condition->value);
        }

        /**
         *  Get the limit field.
         *
         *  @return the maximum number of search results to be returned
         */
        int
        getLimit()                                                  throw()
        {
            return limit;
        }

        /**
         *  Set the limit field.
         *
         *  @param limit the maximum number of search results to be returned
         *  @exception  std::invalid_argument   on syntax errors.
         */
        void
        setLimit(const int limit)
                                                throw(std::invalid_argument)
        {
            if (limit >= 0) {
                this->limit = limit;
            } else {
                throw std::invalid_argument("bad argument: less than zero");
            }
        }

        /**
         *  Get the offset field.
         *
         *  @return the index of the first matching condition 
         *          to be returned (first = 0)
         */
        int
        getOffset()                                                 throw()
        {
            return offset;
        }

        /**
         *  Set the offset field.
         *
         *  @param offset   the index of the first matching condition 
         *                  to be returned (first = 0)
         *  @exception  std::invalid_argument   on syntax errors.
         */
        void
        setOffset(const int offset)
                                                throw(std::invalid_argument)
        {
            if (offset >= 0) {
                this->offset = offset;
            } else {
                throw std::invalid_argument("bad argument: less than zero");
            }
        }

        /**
         *  Convert to an XmlRpc::XmlRpcValue.
         */
        operator XmlRpc::XmlRpcValue() const                        throw();
        
        /**
         *  Check two SearchCriteria objects for equality.
         */
        bool
        operator ==(const SearchCriteria &  other) const            throw();
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */


} // namespace Core
} // namespace LiveSupport

#endif // LiveSupport_Core_SearchCriteria_h

