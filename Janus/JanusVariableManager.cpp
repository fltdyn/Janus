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
 * Module:     JanusVariableManager.cpp
 * Reference:
 *
 * Description:
 *  This file contains the class prototypes for data elements that
 *  interact with Janus XML files.
 *  The associated header file for the class is: JanusManager.h
 *
 * Written by: Shane Hill  (Shane.Hill@dsto.defence.gov.au)
 *
 * Derived from: JanusVariable class.
 *
 * First Date: 20/11/2013
 *
 */

// C++ Includes
#include <stdexcept>

// Local Includes
#include "JanusVariableManager.h"

// Ute Includes
#include <Ute/aMessageStream.h>

using namespace std;
using namespace dstoute;

JanusIndex JanusVariableManager::push_back( const JanusVariable &jv)
{
  // See if we have the variable already.
  for ( size_t i = 0; i < jvList_.size(); ++i) {
    if ( jvList_[ i].variableName_ == jv.variableName_) {
      return JanusIndex( i);
    }
  }

  // Create new managed JanusVariable.
  jvList_.push_back( jv);
  jvList_.back().setJanusFile( this);
  jvList_.back().initialiseVariable();
  return JanusIndex( jvList_.size() - 1);
}

std::vector<JanusIndex> JanusVariableManager::push_back( const std::vector<JanusVariable>& jv)
{
  std::vector<JanusIndex> cache;
  for ( size_t i = 0; i < jv.size(); ++i) {
    cache.push_back( push_back( jv[i]));
  }
  return cache;
}

bool JanusVariableManager::setIndexer( JanusIndex& ji, int i)
{
  if ( operator[]( ji).isAvailable()) {
    if ( !ji.indexerBase_.isValid()) {
      findIndexerBase( ji);
    }
    return operator[]( ji).setValue( double( i + ji.indexerBase_));
  }

  return false;
}

int JanusVariableManager::getIndexer( JanusIndex& ji)
{
  if ( operator[]( ji).isAvailable()) {
    if ( !ji.indexerBase_.isValid()) {
      findIndexerBase( ji);
    }
    return int( operator[]( ji).value()) - int( ji.indexerBase_);
  }

  return 0;
}

void JanusVariableManager::findIndexerBase( JanusIndex &ji)
{
  if ( !ji.indexerBase_.isValid()) {
    ji.indexerBase_ = 0; // Input variables base value (0 or 1).
    bool foundFunctionDef = false;
    janus::FunctionList funcList = getFunction();
    for ( size_t i = 0; !foundFunctionDef && i < funcList.size(); ++i) {
      janus::InDependentVarDefList indepVars = funcList[ i].getInDependentVarDef();
      for ( size_t j = 0; !foundFunctionDef && j < indepVars.size(); ++j) {
        if ( indepVars[ j].getVarID() == operator[]( ji).getVarID()) {
          int indexerBase = static_cast<int>( funcList[ i].getIndependentVarMin( j)); // This is the base index.
          if ( indexerBase == -1) indexerBase = 0; // Special case to allow extrapolation of index below 0.
          ji.indexerBase_ = indexerBase;
          foundFunctionDef = true;
        }
      }
    }

    if ( ji.indexerBase_ != 0 && ji.indexerBase_ != 1) {
      throw_message( runtime_error,
        setFunctionName( "JanusVariableManager::findIndexerBase()")
        << "\n - Base index for indexer variable \"" << operator[]( ji).getVarID() << "\" must be 0 or 1, not " << ji.indexerBase_ << "."
        << "\n - See DML file \"" << getXmlFileName() << "\"."
      );
    }
    else if ( !foundFunctionDef) {
      // Check initial value of indexer.
      if ( int( operator[]( ji).getInitialValue()) == 0) {
        ji.indexerBase_ = 0;
      }
      else if ( int( operator[]( ji).getInitialValue()) == 1) {
        ji.indexerBase_ = 1;
      }
      // Else use the input variables name to determine the index base value.
      // Leap of faith here if not found above.
      else {
        ji.indexerBase_ = ( operator[]( ji).getVarID().toLowerCase().endsWith( "number_") ? 1 : 0);
      }
    }
  }
}

//
// Helper Functions (Forward / Backwards compatibility)
//

aStringList JanusVariableManager::helpGetPropertyList(
  const aString& ptyID,
  const aString& jvSizeID,
  const aString& jvInputID,
  bool isMandatory)
{
  aStringList propertyList;

  // Try new propertDef method.
  aString ptyIDList = ptyID;
  while ( !propertyList.size() && ptyIDList.hasStringTokens()) {
    propertyList = getPropertyList( ptyIDList.getStringToken( "|"));
  }

  // Try old variableDef method.
  if ( !propertyList.size()) {
    JanusIndex jvOutput = push_back( JanusVariable( ptyID, janusOutputVariable, !janusMandatory));
    if ( operator[]( jvOutput).isAvailable()) {
      // No table definitions.
      if ( jvSizeID.empty() && jvInputID.empty()) {
        // Is this a string table - crude but effective test.
        if ( dstomath::isnan( operator[]( jvOutput).value())) {
          propertyList << operator[]( jvOutput).getStringValue();
        }
        // Is this a numerical table.
        else {
          propertyList << aString( "%").arg( operator[]( jvOutput).value());
        }
      }

      // We have a table definition.
      else {
        size_t nProperties = 1;
        JanusIndex jvSize  = push_back( JanusVariable( jvSizeID,  janusOutputVariable, janusMandatory));
        JanusIndex jvInput = push_back( JanusVariable( jvInputID, janusInputVariable, !janusMandatory));
        if ( operator[]( jvSize).isAvailable()) {
          nProperties = size_t( operator[]( jvSize).value());
        }
        for ( size_t i = 0; i < nProperties; ++i) {
          setIndexer( jvInput, static_cast<int>( i));
          // Is this a string table - crude but effective test.
          if ( dstomath::isnan( operator[]( jvOutput).value())) {
            propertyList << operator[]( jvOutput).getStringValue();
          }
          // Is this a numerical table.
          else {
            propertyList << aString( "%").arg( operator[]( jvOutput).value());
          }
        }
      }
    }
  }

  if ( isMandatory && !propertyList.size()) {
    throw_message( runtime_error,
      setFunctionName( "JanusVariableManager::helpGetPropertyList()")
      << "\n - Mandatory property \"" << ptyID << "\" not found."
    );
  }

  return propertyList;
}
