#ifndef _DOMFUNCTIONS_H_
#define _DOMFUNCTIONS_H_

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

//------------------------------------------------------------------------//
// Title:      Janus/DomFunctions
// Class:      DomFunctions
// Module:     DomFunctions.h
// First Date: 2011-12-15
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//
/**
 * \file DomFunctions.h
 *
 * This class contains common functions for interacting with a
 * Document Object Model (DOM) containing data from a DAVE-ML
 * compliant XML dataset source file.
 *
 * Authors  :  G. J. Brian
 *
 * Modified : S. D. Hill
 *
 */

// C++ Includes
#include <cstring>
#include <stdexcept>
#include <sstream>

// Ute Includes
#include <Ute/aString.h>
#include <Ute/aMessageStream.h>

// Local Includes
#include "JanusConstants.h"
#include "DomTypes.h"
#include "XmlElementDefinition.h"

namespace DomFunctions
{

  using dstoute::aString;
  using dstoute::aStringList;
  using dstoute::aFileString;
  using janus::XmlElementDefinition;

  const bool REQUIRED( true);

  class DomWalker : public pugi::xml_tree_walker
  {
    DomWalker( const aString &elementName = "")
    : 
    xml_tree_walker(),
    elementName_( elementName)
    {}
      
    //virtual bool begin( pugi::xml_node& node) { return pugi::xml_tree_walker.begin( node);}

    virtual bool for_each( pugi::xml_node& node)
    {
      bool allDescendants = elementName_.empty();

      if ( allDescendants || ( elementName_ == node.name())) {
        descendantNodes_.push_back( node);
      }
      return true; // continue traversal
    }

    //virtual bool end( pugi::xml_node& node) { return pugi::xml_tree_walker.end( node);}

    aString elementName_;
    XmlNodeList descendantNodes_;
  };

  XmlResult initialiseDOMForReading(
    XmlDoc& document,
    const aString& dataFileName);

  XmlResult initialiseDOMForReading(
    XmlDoc& document,
    unsigned char* documentBuffer,
    const size_t& size);

  XmlNode getDocumentRootNode(
    const XmlDoc& document,
    const aString& documentType,
    const aString& errmsg);

  aString getCData(
    const XmlNode& parentNode);

  aString getAttribute(
    const XmlNode& parentNode,
    const aString& attributeName,
    const bool &required = false);

  std::wstring getWideAttribute(
    const XmlNode& parentNode,
    const aString& attributeName);

  bool isChildInNode(
    const XmlNode& parentNode,
    const aString& elementName);

  XmlNode getChild(
    const XmlNode& parentNode,
    const aString& elementName);

  XmlNode getChildByAttribute(
    const XmlNode& parentNode,
    const aString& elementName,
    const aString& attributeName,
    const aString& attributeValue);

  aString getChildName(
    const XmlNode& parentNode);

  aString getChildValue(
    const XmlNode& parentNode,
    const aString& elementName,
    const bool &required = false);

  XmlNodeList getChildren(
    const XmlNode& parentNode,
    const aString& elementName,
    const aString& parentNodeID = "",
    const bool &required = true);

  aStringList getChildrenValues(
    const XmlNode& parentNode,
    const aString& elementName,
    const aString& parentNodeID = "",
    const bool &required = true);

  XmlNodeList getSiblings(
    const XmlNode& parentNode,
    const aString& elementName);

  void getNodesByName(
    const XmlNode& parentNode,
    const aString& elementName,
    XmlNodeList& nodeList);

  void getNodesByAttributeName( 
    const XmlNode& parentNode, 
    const aString& attributeName, 
    XmlNodeList& nodeList);

  void initialiseChild(
    XmlElementDefinition* xmlElementDefinition,
    const XmlNode& parentNode,
    const aString& parentNodeId,
    const aString& elementName,
    const bool &required = true);

  void initialiseChildren(
    XmlElementDefinition* xmlElementDefinition,
    const XmlNode& parentNode,
    const aString& parentNodeId,
    const aString& elementName,
    const bool &required = true);

  void initialiseChildOrRef(
    XmlElementDefinition* xmlElementDefinition,
    const XmlNode& parentNode,
    const aString& parentNodeId,
    const aString& elementName,
    const aString& elementReference,
    const aString& elementId,
    const bool &required = true);

  void initialiseChildrenOrRefs(
    XmlElementDefinition* xmlElementDefinition,
    const XmlNode& parentNode,
    const aString& parentNodeId,
    const aString& elementName,
    const aString& elementReferenceList,
    const aString& elementReference,
    const aString& elementId,
    const bool &required = true);

  XmlNode setDocTypeDeclaration(
    XmlNode& parentNode,
    const aString& dcoumentType);

  void setComment(
    XmlNode& parentNode,
    const aString& comment);

  void setAttribute(
    XmlNode& parentNode,
    const aString& attributeName,
    const aString& attributeData);

  XmlNode setChild(
    XmlNode& parentNode,
    const aString& childName);

  XmlNode setChild(
    XmlNode& parentNode,
    const aString& childName,
    const aString& childContent);

  XmlNode setChildCData(
    XmlNode& parentNode,
    const aString& childName,
    const aString& childContent);

  //------------------------------------------------------------------------//

  XmlResult initialiseDOMForReading( XmlDoc& document, const aString& dataFileName, const aString& keyFileName);
  XmlResult initialiseDOMForReading( XmlDoc& document, unsigned char* documentBuffer, const size_t& size);

  //------------------------------------------------------------------------//

  inline XmlNode getDocumentRootNode(
    const XmlDoc& document,
    const aString& documentType,
    const aString& errmsg)
  {
    XmlNode docTypeElement = document.child( documentType.c_str());
    if ( docTypeElement == 0) {
      throw_message( std::invalid_argument, errmsg);
    }

    return docTypeElement;
  }

  // Retrieve CDATA from the a Node
  inline aString getCData(
    const XmlNode& parentNode)
  {
    static const aString functionName( "DomFunctions::getCDataFromNode()");

    aString elementData;

    const char* cdataStr = parentNode.child_value();
    if ( std::strlen( cdataStr) > elementData.max_size()) {
      throw_message( std::invalid_argument,
        dstoute::setFunctionName( functionName)
        << "\n - ID \"" << parentNode << "\""
        << "\n Insufficient memory to store the CData for the element."
      );
    }
    else {
      elementData.assign( cdataStr);
    }

    return elementData;
  }

  // Retrieve an Attribute of an element from the Node
  inline aString getAttribute(
    const XmlNode& parentNode,
    const aString& attributeName,
    const bool &required)
  {
    static const aString functionName( "DomFunctions::getAttribute()");

    aString attributeData;
    pugi::xml_attribute attributeNode = parentNode.attribute( attributeName.c_str());

    if ( attributeNode != 0) {
      attributeData.assign( attributeNode.value());
    }
    else if ( required) {
      throw_message( std::invalid_argument,
        dstoute::setFunctionName( functionName)
        << "\n - Element does not contain the required attribute \"" << attributeName << "\"."
      );
    }

    return attributeData;
  }

  // Retrieve a Unicode Attribute of an element from the Node
  inline std::wstring getWideAttribute(
    const XmlNode& parentNode,
    const aString& attributeName)
  {
    std::wstring attributeData;
    aString attributeValue;
    pugi::xml_attribute attributeNode = parentNode.attribute( attributeName.c_str());

    if ( attributeNode != 0) {
      attributeValue.assign( attributeNode.value());
      size_t iLen = attributeValue.length();
      for ( size_t i = 0; i < iLen; ++i) {
        attributeData.push_back( attributeValue[ i]);
      }
    }

    return attributeData;
  }

  // Check whether a child element is available in the Node
  inline bool isChildInNode(
    const XmlNode& parentNode,
    const aString& elementName)
  {
    static const aString functionName( "DomFunctions::isChildInNode()");

    aString attributeData;
    XmlNode childNode = parentNode.child( elementName.c_str());

    if ( childNode != 0) {
      return true;
    }
    return false;
  }

  // Retrieve a child element from the Dom - this gets the first matching child
  // element only
  inline XmlNode getChild(
    const XmlNode& parentNode,
    const aString& elementName)
  {
    if ( elementName.empty()) {
      return parentNode.first_child();
    }
    return parentNode.child( elementName.c_str());
  }

  // Retrieve a child element from the Dom with specified attribute name and
  // value - this gets the first matching child element only.
  inline XmlNode getChildByAttribute(
    const XmlNode& parentNode,
    const aString& elementName,
    const aString& attributeName,
    const aString& attributeValue)
  {
    return parentNode.find_child_by_attribute( 
      elementName.c_str(), attributeName.c_str(), attributeValue.c_str());
  }

  // Retrieve the Name of a child element from the Dom
  inline aString getChildName(
    const XmlNode& elementNode)
  {
    return ( elementNode.name());
  }

  // Retrieve the Value of a child element from the Dom
  inline aString getChildValue(
    const XmlNode& parentNode,
    const aString& elementName,
    const bool &required)
  {
    static const aString functionName( "DomFunctions::getChildValue()");

    aString elementData;
    XmlNode childNode = parentNode.child( elementName.c_str());

    if ( childNode != 0) {
      elementData.assign( childNode.child_value());
    }
    else if ( required) {
      throw_message( std::invalid_argument,
        dstoute::setFunctionName( functionName)
        << "\n - ID \"" << parentNode << "\" "
        << "does not have a child \"" << elementName << "\" element."
      );
    }

    return elementData;
  }

  // Retrieve a list of child elements from a Dom Node
  //  This function gets all child elements matching 'elementName'.
  //  If 'elementName' is an empty string then all children are returned.
  inline XmlNodeList getChildren(
    const XmlNode& parentNode,
    const aString& elementName,
    const aString& parentNodeID,
    const bool &required)
  {
    static const aString functionName( "DomFunctions::getChildren()");

    XmlNodeList childList;
    bool allChildren = elementName.empty();

    for ( XmlNode childNode = parentNode.first_child(); childNode; childNode = childNode.next_sibling()) {
      if ( allChildren || ( elementName == childNode.name())) {
        childList.push_back( childNode);
      }
    }

    if ( childList.size() == 0 && required) {
      throw_message( std::invalid_argument,
        dstoute::setFunctionName( functionName)
        << "\n - ID \"" << parentNodeID << "\" "
        << "must have at least 1 \"" << elementName << "\" record."
      );
    }

    return childList;
  }

  // Retrieve the Value of a child element from the Dom
  inline aStringList getChildrenValues(
    const XmlNode& parentNode,
    const aString& elementName,
    const aString& parentNodeID,
    const bool &required)
  {
    XmlNodeList childList = getChildren( parentNode, elementName, parentNodeID, required);
    aStringList childValues;
    for ( size_t i = 0; i < childList.size(); ++i) {
      childValues << childList[ i].child_value();
    }

    return childValues;
  }

  // Retrieve a list of sibling elements from Dom Node
  //  This function gets all sibling elements matching 'elementName'.
  //  If 'elementName' is an empty string then all siblings are returned.
  inline XmlNodeList getSiblings(
    const XmlNode& parentNode,
    const aString& elementName)
  {
    static const aString functionName( "DomFunctions::getSiblings()");

    XmlNodeList siblingVector;
    bool allSiblings = elementName.empty();

    for ( XmlNode siblingNode = parentNode.next_sibling(); siblingNode; siblingNode = siblingNode.next_sibling()) {
      if ( allSiblings || ( elementName == siblingNode.name())) {
        siblingVector.push_back( siblingNode);
      }
    }

    return siblingVector;
  }

  // Retrieve a list of Nodes matching an 'elementName' for an
  // entire branch of a tree starting from the 'parentNode'
  inline void getNodesByName(
    const XmlNode& parentNode,
    const aString& elementName,
    XmlNodeList& nodeList)
  {
    if ( elementName == parentNode.name()) {
      nodeList.push_back( parentNode);
    }

    for ( XmlNode childNode = parentNode.first_child(); childNode; childNode = childNode.next_sibling()) {
      getNodesByName( childNode, elementName, nodeList);
    }
  }

  // Retrieve a list of Nodes having a matching 'attributeName' for an
  // entire branch of a tree starting from the 'parentNode'
  inline void getNodesByAttributeName( 
    const XmlNode& parentNode, 
    const aString& attributeName, 
    XmlNodeList& nodeList)
  {
    bool hasAttribute = false;
    for ( pugi::xml_attribute attr = parentNode.first_attribute(); attr; attr = attr.next_attribute()) {
      if ( 0 == attributeName.compare( attr.value())) {
        hasAttribute = true;
        break;
      }
    }
    if ( hasAttribute) {
      nodeList.push_back( parentNode);
    }
    
    for ( XmlNode childNode = parentNode.first_child(); childNode; childNode = childNode.next_sibling()) {
      getNodesByAttributeName( childNode, attributeName, nodeList);
    }
  }


  // Initialise a child element by retrieving data from the node.
  // The elements are referenced as a definition.
  inline void initialiseChild(
    XmlElementDefinition* xmlElementDefinition,
    const XmlNode& parentNode,
    const aString& parentNodeId,
    const aString& elementName,
    const bool &required)
  {
    static const aString functionName( "DomFunctions::initialiseChild()");

    XmlNode childNode = parentNode.child( elementName.c_str());
    if ( childNode != 0) {
      xmlElementDefinition->readDefinitionFromDom( childNode);
    }
    else if ( required) {
      throw_message( std::invalid_argument,
        dstoute::setFunctionName( functionName)
        << "\n - ID \"" << parentNodeId << "\" does not have a\""
        << elementName << "\" Def element."
      );
    }
  }

  // Initialise children by retrieving data from the Dom.
  // The elements are referenced as definitions
  inline void initialiseChildren(
    XmlElementDefinition* xmlElementDefinition,
    const XmlNode& parentNode,
    const aString& parentNodeId,
    const aString& elementName,
    const bool &required)
  {
    static const aString functionName( "DomFunctions::initialiseChildren()");

    try {
      XmlNodeList children = getChildren( parentNode, elementName, parentNodeId, required);
      size_t childrenListLength = children.size();

      for ( size_t j = 0; j < childrenListLength; ++j) {
        xmlElementDefinition->readDefinitionFromDom( children.at( j));
      }
    }
    catch ( std::exception &excep) {
      throw_message( std::invalid_argument,
        dstoute::setFunctionName( functionName) << "\n - "
        << excep.what()
      );
    }
  }

  // Initialise a Child element by retrieving data from the Dom.
  // The element is included as either a definition or a reference.
  inline void initialiseChildOrRef(
    XmlElementDefinition* xmlElementDefinition,
    const XmlNode& parentNode,
    const aString& parentNodeId,
    const aString& elementName,
    const aString& elementReference,
    const aString& elementId,
    const bool &required)
  {
    static const aString functionName( "DomFunctions::initialiseChildOrRef()");

    XmlNode childNode = parentNode.child( elementName.c_str());
    if ( childNode) {
      xmlElementDefinition->readDefinitionFromDom( childNode);
    }
    else { // This is a reference to a child element
      // Check for reference elements - '*Ref'
      XmlNode childRef = parentNode.child( elementReference.c_str());
      if ( !childRef) {
        if ( required) {
          throw_message( std::invalid_argument,
            dstoute::setFunctionName( functionName)
            << "\n - ID \"" << parentNodeId << "\" "
            << "requires at least 1 \"" << elementReference << "\" Ref element."
          );
        }
        return;
      }

      aString elementIdChar = getAttribute( childRef, elementId);

      //XmlNode node = parentNode.
      //  find_child_by_attribute( elementName.c_str(), elementId.c_str(), elementIdChar.c_str());
      //xmlElementDefinition->readDefinitionFromDom( node);
      //xmlElementDefinition->compareElementID( node, elementIdChar, j);

      XmlNodeList nodeList;
      getNodesByName( parentNode.root().first_child(), elementName, nodeList);

      size_t nodeListLength = nodeList.size();
      for ( size_t j = 0; j < nodeListLength; ++j) {
        if ( xmlElementDefinition->compareElementID( nodeList.at( j), elementIdChar, j)) {
          break;
        }
      }
    }

    return;
  }

  // Initialise children by retrieving data from the Dom.
  // The children are included as either definitions or references.
  inline void initialiseChildrenOrRefs(
    XmlElementDefinition* xmlElementDefinition,
    const XmlNode& parentNode,
    const aString& parentNodeId,
    const aString& elementName,
    const aString& elementReferenceList,
    const aString& elementReference,
    const aString& elementId,
    const bool &required)
  {
    static const aString functionName( "DomFunctions::initialiseChildrenOrRefs()");

    // Check for definition elements - '*Def'
    XmlNodeList children = getChildren( parentNode, elementName, parentNodeId, false);
    size_t childrenListLength = children.size();

    // Check for reference elements - '*Ref'
    XmlNode refElement = parentNode;

    if ( !elementReferenceList.empty()) { // check for reference container elements - '*Refs'
      XmlNode childCntRef = parentNode.child( elementReferenceList.c_str());

//      XmlNodeList childrenCntRefs = getChildren( parentNode, elementReferenceList, parentNodeId, false);

      if ( childCntRef == 0) {
        if ( required) {
          throw_message( std::invalid_argument,
            dstoute::setFunctionName( functionName)
            << "\n - ID \"" << parentNodeId << "\" "
            << "requires at least 1 \"" << elementReferenceList << "\" Refs element."
          );
        }
        return;
      }

      refElement = childCntRef;
    }

    // Check for reference elements - '*Ref'
    XmlNodeList childrenRefs = getChildren( refElement, elementReference, janus::EMPTY_STRING, false);
    size_t childrenRefsListLength = childrenRefs.size();

    if (( childrenListLength == 0) && ( childrenRefsListLength == 0)) {
      if ( required) {
        throw_message( std::invalid_argument,
          dstoute::setFunctionName( functionName)
          << "\n - ID \"" << parentNodeId << "\" "
          << "requires at least 1 \""
          << elementName << "\" element or"
          << elementReference << "\" element."
        );
      }
      return;
    }

    // Read definition elements from DOM - '*Def'
    if ( childrenListLength != 0) {
      for ( size_t j = 0; j < childrenListLength; ++j) {
        xmlElementDefinition->readDefinitionFromDom( children.at( j));
      }
    }

    // Read reference elements from DOM - '*Def'
    if ( childrenRefsListLength != 0) {
      XmlNodeList nodeList;
      getNodesByName( parentNode.root().first_child(), elementName, nodeList);
      size_t nodeListLength = nodeList.size();

      aString elementIdChar;
      size_t childrenRefsLength = childrenRefs.size();
      for ( size_t j = 0; j < childrenRefsLength; ++j) {
        elementIdChar = getAttribute( childrenRefs.at( j), elementId);

        for ( size_t k = 0; k < nodeListLength; ++k) {
          if ( xmlElementDefinition->compareElementID( nodeList.at( k), elementIdChar, k)) {
            break;
          }
        }
      }
    }
  }

  inline XmlNode setDocTypeDeclaration(
    XmlNode& parentNode,
    const aString& documentType)
  {
    aString docTypeDec = documentType + " SYSTEM \"../../dtd/" + documentType + ".dtd\"";
    XmlNode docTypeElement = parentNode.append_child( pugi::node_doctype);
    docTypeElement.set_value( docTypeDec.c_str());
    return docTypeElement;
  }

  inline void setComment(
    XmlNode& parentNode,
    const aString& comment)
  {
    XmlNode commentElement = parentNode.append_child( pugi::node_comment);
    commentElement.set_value( comment.c_str());
  }

  inline void setAttribute(
    XmlNode& parentNode,
    const aString& attributeName,
    const aString& attributeData)
  {
    pugi::xml_attribute attributeElement = parentNode.append_attribute( attributeName.c_str());
    attributeElement.set_value( attributeData.c_str());
  }

  inline XmlNode setChild(
    XmlNode& parentNode,
    const aString& childName)
  {
    return ( parentNode.append_child( childName.c_str()));
  }

  inline XmlNode setChild(
    XmlNode& parentNode,
    const aString& childName,
    const aString& childContent)
  {
    XmlNode childElement = parentNode.append_child( childName.c_str());
    childElement.append_child( pugi::node_pcdata).set_value( childContent.c_str());
    return childElement;
  }

  inline XmlNode setChildCData(
    XmlNode& parentNode,
    const aString& childName,
    const aString& childContent)
  {
    XmlNode childElement = parentNode.append_child( childName.c_str());
    childElement.append_child( pugi::node_cdata).set_value( childContent.c_str());
    return childElement;
  }

  void copyAllSiblings( XmlNode existingParent, XmlNode otherParent);
}

#endif /* _DOMFUNCTIONS_H_ */
