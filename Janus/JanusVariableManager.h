#ifndef _JANUSVARIABLEMANAGER_H_
#define _JANUSVARIABLEMANAGER_H_

//
// DST Janus Library (Janus DAVE-ML Interpreter Library)
//
// Defence Science and Technology (DST) Group
// Department of Defence, Australia.
// 506 Lorimer St
// Fishermans Bend, VIC
// AUSTRALIA, 3207
//
// Copyright 2005-2021 Commonwealth of Australia
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this
// software and associated documentation files (the "Software"), to deal in the Software
// without restriction, including without limitation the rights to use, copy, modify,
// merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be included in all copies
// or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
// OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//

/*
 * Title:      Janus Variable Manager Class
 * Class:      JanusVariableManager
 * Module:     JanusVariableManager.h
 * Reference:
 *
 * Description:
 *  This file contains the class prototypes for data elements that
 *  interact with Janus XML files.
 *  The associated code file for the class is: JanusVariableManager.cpp
 *
 * Written by: Shane Hill  (Shane.Hill@dsto.defence.gov.au)
 *
 * Derived from: JanusVariable class.
 *
 * First Date: 20/11/2013
 *
 */

// Local Includes
#include "JanusVariable.h"
#include "Janus.h"

// C++ Includes
#include <vector>
#include <fstream>
#include <limits>

// Ute Includes
#include <Ute/aOptional.h>

// ----------------
// Class Definition
// ----------------

struct JanusIndex
{
  JanusIndex() : idx_( dstoute::aOptionalSizeT::invalidValue()) {}
  JanusIndex( size_t i) : idx_( i), indexerBase_( dstoute::aOptionalSizeT::invalidValue()) {}
  template <typename T> bool operator==( T rhs) const { return ( idx_ == dstoute::aOptionalSizeT( rhs));}
  operator size_t() const { return idx_;}
  bool isValid() const { return idx_.isValid();}
  friend std::ostream& operator<< ( std::ostream &os, const JanusIndex &ji) {
    os << ji.idx_;
    return os;
  }
  dstoute::aOptionalSizeT idx_;
  dstoute::aOptionalSizeT indexerBase_;
};

class JanusVariableManager : public janus::Janus
{
 public:
  virtual ~JanusVariableManager() {}

  JanusIndex push_back( const JanusVariable& jv);
  std::vector<JanusIndex> push_back( const std::vector<JanusVariable>& jv);
  JanusVariable& operator[]( const JanusIndex& ji)
  {
    if ( !ji.idx_.isValid()) return EMPTY_JANUSVARIABLE;
    jvList_[ ji.idx_].setJanusFile( this);
    return jvList_[ ji.idx_];
  }
  JanusVariable& at( const JanusIndex& ji)
  {
    if ( !ji.idx_.isValid()) return EMPTY_JANUSVARIABLE;
    jvList_.at( ji.idx_).setJanusFile( this);
    return jvList_.at( ji.idx_);
  }
  /**
   * Set special Amiel indexer variables (e.g. engineInxed_ or engineNumber_).
   * These variables may either have a base value of 0 or 1. This set function
   * will determine the correct base to use based on the DML definition within the DML file.
   * All input indices to this function must be zero (0) based.
   * @param ji Indexer variable JanusIndex
   * @param i Base zero (0) index variable.
   */
  bool setIndexer( JanusIndex& ji, int i);
  int  getIndexer( JanusIndex& ji);
  void clear()
  {
    jvList_.clear();
    janus::Janus::clear();
  }
  void clearManagedVariables()
  {
    jvList_.clear();
  }
  const std::vector<JanusVariable>& jvList()
  {
    return jvList_;
  }

  //
  // Helper Functions (Forward / Backwards compatibility)
  //

  /**
   * Get a property list from new PropertyDef list or from old numerical or string VariableDef.
   *
   * @param ptyID: The ptyID or old varID containing the list, (may be '|' separated).
   * @param jvSizeID: The old output varID that provides the length of the list.
   * @param jvInputID: The old input varID that sets the index
   * @return The propertyList.
   */
  dstoute::aStringList helpGetPropertyList(
    const dstoute::aString& ptyID,
    const dstoute::aString& jvSizeID,
    const dstoute::aString& jvInputID,
    bool isMandatory = false);

 private:
  void findIndexerBase( JanusIndex &ji);
  std::vector<JanusVariable> jvList_;
};

#endif /* _JANUSVARIABLEMANAGER_H_ */
