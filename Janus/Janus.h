#ifndef _JANUS_H_
#define _JANUS_H_

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
// Title:      Janus/Janus
// Class:      Janus
// Module:     Janus.h
// First Date: 2003-07-18
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file Janus.h
 *
 * Janus performs XML initialisation the pugiXML parser loading the
 * supplied XML file or data buffer a DOM structure.  It holds the
 * data structure and accesses it on request, doing interpolation
 * or other computation as required for output.  It cleans up on termination.
 *
 * This header defines all the elements required to use the XML dataset for 
 * flight modelling, and should be included in any source code intended
 * to activate an instance of the Janus class.
 *
 */

/*
 * This header also contains documentation that forms the basis of a
 * doxygen-generated manual.
 *
 * Author: D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 *
 */

// C++ Includes
#include <stdexcept>
#include <vector>

// Ute Includes
#include <Ute/aString.h>
#include <Ute/aMessageStream.h>
#include <Ute/aOptional.h>

// Local Includes
#include "JanusConfig.h"
#include "JanusConstants.h"
#include "XmlElementDefinition.h"
#include "FileHeader.h"
#include "VariableDef.h"
#include "PropertyDef.h"
#include "BreakpointDef.h"
#include "GriddedTableDef.h"
#include "UngriddedTableDef.h"
#include "Function.h"
#include "SignalDef.h"
#include "CheckData.h"
#include "Author.h"
#include "Reference.h"
#include "Provenance.h"
#include "Modification.h"
#include "Uncertainty.h"

/**
 * The Janus class, all functions within it, and various subordinate classes 
 * are included in the \em janus namespace.  Because Janus is a library, it is
 * expected to be used in conjunction with external classes and the namespace 
 * provides a way of avoiding possible naming clashes with those classes.
 */
namespace janus {

  /**
   * Janus is an XML dataset interface class.
   * A Janus instance holds in its allocated memory the DOM corresponding to
   * a DAVE-ML compliant XML dataset source file, and data structures
   * derived from that DOM.  It also provides the functions that allow
   * a calling program to access the DOM and related data structures,
   * including means of evaluating output variable values that are
   * dependent on supplied input variable values.  The DOM is normally only
   * accessed during initialisation, which transfers its contents to
   * more easily accessible structures within the class.
   */
  class Janus : public XmlElementDefinition
  {

   public:
    /**
     * @defgroup janusInstantiationGroup Janus - Class Instantiation
     * The instantiation functions relate to the construction and
     * destruction of a Janus instance.  They perform XML initialisation
     * using the pugiXML parser loading the supplied XML file or data buffer to a
     * DOM structure and create vectors and numeric arrays based on the XML data.
     *
     * The instantiation process will throw standard exceptions if the XML
     * file or the data buffer do not load or parse successfully.
     * If the calling program does not catch these exceptions, the program
     * will abort.  An example of exception handling, applicable to all forms
     * of Janus instantiation, is:
     * \code
     * try {
     *   prop.setXmlFileName( fileName );
     * }
     * catch ( exception &excep ) {
     *   cerr << excep.what() << " \n\n";
     *   return 1;
     * }
     * \endcode
     *
     *  @{
     */

    /**
     * The empty constructor can be used to instance the Janus class without
     * supplying a name for the XML file from which the DOM is to be constructed,
     * but in this state is not useful for any class functions.  It will require
     * an XML file name to be supplied before any further use of the
     * instanced class.  This form of the constructor is principally
     * for use within higher level instances, where memory needs to be allocated
     * before the data to fill it is specified.
     *
     * \sa setXmlFileName
     */
    Janus();


    /**
     * The constructor, when called with the XML document name, does the XML
     * initialisation and then loads the DOM structure.
     * A minimal example is:
     * \code
     * #include <string>
     * #include "Janus.h"
     *
     * using namespace std;
     *
     * int main (int argc, char* args[])
     * {
     *   char fileName[] = "~/pika/pika_prop.xml";
     *   Janus prop( fileName );
     *
     *   return 0;
     * }
     * \endcode
     * When the XML file name is supplied, either at instantiation or
     * afterwards when using #setXmlFileName, the DOM is parsed
     * against \em DAVEfunc.dtd
     * before data structures are set up.  The constructor calls
     * private functions within the class to set up instances representing
     * \em fileHeader, \em variableDef, \em breakpointDef, \em griddedTableDef,
     * \em ungriddedTableDef, \em function and \em checkData
     * DOM Level 1 elements.
     *
     * \param documentName is the XML file name, e.g. "~/pika/pika_prop.xml"
     *
     * \param keyFileName is name of a file containing a RSA public key.
     * It is used for interacting with encrypted DAVE-ML data files.
     */
    Janus( const dstoute::aFileString& documentName,
           const dstoute::aFileString& keyFileName = "");


    Janus( const dstoute::aFileString& documentName,
           const dstoute::aFileString& perturbationDocumentName,
           const dstoute::aFileString& keyFileName);

    /**
     * The constructor, when called with the XML document name, does the XML
     * initialisation and then loads the DOM structure.
     * A minimal example is:
     * \code
     * #include <string>
     * #include "Janus.h"
     *
     * using namespace std;
     *
     * int main (int argc, char* args[])
     * {
     *   unsigned char* buffer;
     *   ...
     *   Janus prop( buffer, bufferSize);
     *
     *   return 0;
     * }
     * \endcode
     * When the XML file name is supplied, either at instantiation or
     * afterwards when using #setXmlFileName, the DOM is parsed
     * against \em DAVEfunc.dtd
     * before data structures are set up.  The constructor calls
     * private functions within the class to set up instances representing
     * \em fileHeader, \em variableDef, \em breakpointDef, \em griddedTableDef,
     * \em ungriddedTableDef, \em function and \em checkData
     * DOM Level 1 elements.
     *
     * \param documentBuffer is the XML buffer."
     *
     * \param documentBufferSize is the XML buffer size or length."
     */
    Janus( unsigned char* documentBuffer, size_t documentBufferSize);


    /**
     * The copy constructor may be used to duplicate the data stored
     * within a Janus instance.
     *
     * \param rhs is a reference to the Janus instance being copied.
     *
     * \sa operator=
     */
    Janus( const Janus &rhs);


    /**
     * The assignment operator may be used to duplicate the data stored
     * within a Janus instance.
     *
     * \param rhs is a reference to the Janus instance being copied.
     */
    Janus& operator=( const Janus &rhs);


    /**
     * After deleting memory allocations, the parser instance is released.
     * The destructor is called automatically when the instance goes out of scope.
     */
    virtual ~Janus();


    /**
     * Initialise all Janus member variables as per the empty constructor.
     */
    virtual void clear();


    /**
     * An uninitialised instance of Janus is populated using a nominated
     * XML file through this function.  The DOM for this Janus instance is
     * loaded from the named file, parsed against the \em DAVEfunc.dtd
     * before data arrays are set up.
     *
     * If another XML file name is supplied to an instance that has already
     * been initialised then the check performed at load time throws a standard
     * exception.
     *
     * \param documentName is the XML file name, e.g. "~/pika/pika_prop.xml"
     *
     * \param keyFileName is name of a file containing a RSA public key.
     * It is used for interacting with encrypted DAVE-ML data files.
     */
    virtual void setXmlFileName( const dstoute::aFileString& documentName,
                                 const dstoute::aFileString& keyFileName = "");


    virtual void setXmlFileName( const dstoute::aFileString& documentName,
                                 const dstoute::aFileString& perturbationDocumentName,
                                 const dstoute::aFileString& keyFileName);

    /**
     * An uninitialised instance of Janus is populated using a nominated
     * XML data buffer through this function.  The DOM for this Janus instance is
     * loaded from the buffer, parsed against the \em DAVEfunc.dtd
     * before data arrays are set up.
     *
     * If an instance has previously been initialised then the check
     * performed at load time throws a standard exception.
     *
     * If the instance of Janus is not being inherited by another process
     * then the data buffer is deleted after it has been parsed successfully.
     *
     * \param documentBuffer is the XML data buffer
     * \param documentBufferSize is the size of the buffer in bytes
     */
    virtual void setXmlFileBuffer( unsigned char* documentBuffer, const size_t& documentBufferSize);

    /**
     * If the instance has been fully initialised, the fully-qualified name of
     * the XML dataset file from which it was initialised is returned by this
     * function.
     *
     * \return The XML file name e.g.
     * "~/pika/pika_prop.xml"
     *
     */
    const dstoute::aFileString& getXmlFileName() const
    { return dataFileName_;}

    /**
     * If the instance has been fully initialised, the fully-qualified name of
     * the XML dataset key file from which it was initialised is returned by this
     * function.
     *
     * \return The XML key file name e.g.
     * "~/pika/pika_prop.xml"
     *
     */
    const dstoute::aFileString& getXmlKeyName() const
    { return keyFileName_;}

    /**
     * This enum is used to indicate whether a short, long, or HEX library version
     * description string is required.
     */
    enum VersionType {
      /** a short, purely numeric string, eg "0.97" */
      SHORT,
      /** a longer, alpha-numeric string, eg "Janus V-0.97" */
      LONG,
      /** a hexadecimal, eg 0x000907 */
      HEX};


    /**
     * This function allows the calling program to retrieve the version number of
     * the Janus library that is in use.  It is particularly useful for
     * dynamically linked programs that may use several different
     * library versions.
     *
     * \param versionType determines whether a short, long or hexadecimal
     * string is returned.
     *
     * \return a character pointer to the version description string.
     */
    const char* getJanusVersion( VersionType versionType = HEX) const;


    /**
     * This function permits a calling program to determine if this instance
     * of Janus has been instantiated or is empty.
     *
     * \return a boolean indicating if the Janus instance has been instantiated.
     */
    bool isJanusInitialised() const
    {
      return isJanusInitialised_;
    }


    /**
     * This function is used to initiate a Document Object Model that stores
     * data for exporting to an XML file.
     *
     * \param documentType this is a string indicating the document type
     * definition of the read or exported XML file. The default document type
     * is \em DAVEfunc indicating that the XML file is encoded using the DAVE-ML
     * syntax as defined in the DAVEfunc.dtd. XML syntax and applications that
     * build upon Janus may define alternative document types, and accompanying
     * document type definitions, such as \em THAMESfunc, which is the
     * Time History for Aircraft Modelling Exchange Syntax.
     */
    void initiateDocumentObjectModel( const dstoute::aString& documentType = "DAVEfunc")
    {
      if ( document_ && document_.first_child()) {
        releaseJanusDomParser();
      }

      if ( !DomFunctions::setDocTypeDeclaration( document_, documentType)) {
        throw_message( std::range_error,
          dstoute::setFunctionName( "Janus::initiateDocumentObjectModel()")
          << "\n - Can't initialise document type for \"" << documentType << "\"."
        );
      }

      if ( !DomFunctions::setChild( document_, documentType)) {
        throw_message( std::range_error,
          dstoute::setFunctionName( "Janus::initiateDocumentObjectModel()")
          << "\n - Can't initialise children for \"" << documentType << "\"."
        );
      }
    }

    /**
     * This function permits a calling routine or application to
     * gain access to the parent document of an instantiated DOM.
     *
     * \return a \em DomFunctions::XmlDoc pointer to the DOM document is returned
     */
    DomFunctions::XmlNode getDomDocument() const { return document_;}

    /**
     * This function permits a calling routine or application to
     * gain access to the parent document level0 node of an instantiated DOM.
     *
     * \return a \em DomFunctions::XmlNode pointer to the document level0 node is returned
     */
    DomFunctions::XmlNode getDomDocumentRootNode() const;

    /**
     * This enum is used to indicate the Export Object options.
     */
    enum ExportObjectType {
      /** a data file */
      FILE,
      /** a data Buffer */
      BUFFER};

    /**
     * This function exports the contents of the Janus instance to
     * an XML text file complying with the DAVE-ML syntax as defined by
     * the DAVE-ML DTD.
     *
     * This function may be overloaded by equivalent functions for projects
     * that inherit Janus.
     *
     * \param dataFileName the name of the file to which the data
     * will be written.
     *
     * \return The value returned will be greater than zero if the data
     * has been successfully exported to the file, else it will be zero.
     */
    virtual size_t exportToFile( const dstoute::aFileString& dataFileName)
    {
      std::ostringstream ostr;
      return exportTo( Janus::FILE, dataFileName, ostr);
    }

    /**
     * This function exports the contents of the Janus instance to
     * an XML data buffer complying with the DAVE-ML syntax as defined by
     * the DAVE-ML DTD.
     *
     * This function may be overloaded by equivalent functions for projects
     * that inherit Janus.
     *
     * \param documentBuffer an address to a ostringstream buffer that
     * data will be written. This data is not '\0' terminated.
     *
     * \return The \em size of the buffer will be returned. If size is
     * zero then the export of the buffer has failed.
     */
    virtual size_t exportToBuffer( std::ostringstream& documentBuffer)
    {
      return exportTo( Janus::BUFFER, EMPTY_STRING, documentBuffer);
    }

    /**
     * This function exports the contents of the Janus instance to
     * an XML data buffer complying with the DAVE-ML syntax as defined by
     * the DAVE-ML DTD.
     *
     * This function may be overloaded by equivalent functions for projects
     * that inherit Janus.
     *
     * \param documentBuffer an address to a unsigned char* buffer that
     * data will be written. The memory allocated to the buffer
     * within this function MUST be managed (freed) by the external
     * application. The buffer will be '\0' terminated.
     *
     * \return The \em size of the buffer will be returned. If size is
     * zero then the export of the buffer has failed.
     */
    virtual size_t exportToBuffer( unsigned char* &documentBuffer)
    {
      std::ostringstream sstr;
      size_t bufferLength = exportTo( Janus::BUFFER, EMPTY_STRING, sstr);

      documentBuffer = ( unsigned char*) malloc( bufferLength + 1);  // +1 for terminating '\0'
      memcpy( documentBuffer, sstr.str().c_str(), bufferLength + 1); // +1 for terminating '\0'
      return bufferLength;
    }

    /** @} */ // end of instantiationGroup

    /** @defgroup janusLevel1Group Janus - Level 1 Elements
     * There are seven DAVE-ML DTD Level 1 elements that attach directly to
     * the \em DAVEfunc root element and provide the basic structure of the
     * DOM.  These functions provide access to each of these
     * elements, so that lower-level functions can access the raw data of
     * the element contents. These functions are generally used within the
     * Janus instance, and some of them are not normally called by
     * external programs.
     *
     * @{
     */

    /**
     * This function permits a calling program to retrieve the header information
     * for a DAVE-ML compliant XML dataset. Descriptive material is contained
     * in the file header.  This includes file authorship, modification
     * records, and cross-references to source material.
     * FileHeader and lower level class functions may be used
     * through the returned reference to access the \em fileHeader contents.
     * Provided it has been instantiated without error, each Janus instance
     * contains one FileHeader instance.
     *
     * \return A reference to the FileHeader instance is returned.
     */
    const FileHeader& getFileHeader() const { return fileHeader_;}

    /**
     * The \em variableDef elements contained in a DOM that complies with
     * the DAVE-ML DTD identify the input and output signals used by function
     * blocks.  They also provide MathML content markup to indicate that a
     * calculation is required to arrive at the value of the variable,
     * using other variables as inputs.  The variable definition can
     * include statistical information regarding the uncertainty of the
     * values that it might take on, when measured after any
     * calculation is performed.  In general, calling programs should access
     * variable-based data through the VariableDef instance and its lower-level
     * procedures. Provided it has been instantiated and initialised without
     * error, each Janus instance contains one VariableDef vector of instances,
     * of length not less than one.
     *
     * \return  A reference to the list of VariableDef instances is returned.
     */
    VariableDefList& getVariableDef() { return variableDef_;}
    const VariableDefList& getVariableDef() const { return variableDef_;}

    /**
     * As well as accessing the complete vector of VariableDefs within a Janus
     * instance, an individual VariableDef may be accessed by index.
     *
     * \param index is the offset of a required VariableDef within the vector
     * of VariableDef instances contained in a Janus instance.  It has a range
     * from 0 to (#getVariableDef().size() - 1)
     *
     * \return The selected VariableDef is returned by reference.
     */
    VariableDef& getVariableDef( dstoute::aOptionalSizeT index)
    { 
      if ( index.isValid()) return variableDef_[ index];
      throw_message( 
        std::runtime_error, 
        dstoute::setFunctionName( "Janus::getVariableDef( dstoute::aOptionalSizeT index)")
        << "\n - invalid index"
      );
    }

    const VariableDef& getVariableDef( dstoute::aOptionalSizeT index) const
    {
      if ( index.isValid()) return variableDef_[ index];
      throw_message( 
        std::runtime_error, 
        dstoute::setFunctionName( "Janus::getVariableDef( dstoute::aOptionalSizeT index)")
        << "\n - invalid index"
      );
    }

    /**
     * As well as accessing the complete vector of VariableDefs within a Janus
     * instance, an individual VariableDef may be accessed by varID.
     *
     * \param varID is a C++ string containing the \em varID of a required
     * VariableDef within the vector of VariableDef instances contained in a
     * Janus instance.  Where no VariableDef contains a \em varID matching the
     * input, a standard exception will be thrown.
     *
     * \return The selected VariableDef is returned by reference.
     */
    VariableDef& getVariableDef( const dstoute::aString& varID );
    const VariableDef& getVariableDef( const dstoute::aString& varID) const;
       
    /**
     * This function searches the list of VariableDefs within a Janus instance
     * for an entry corresponding to the \em varID variable identifier specified.
     *
     * \param varID is a C++ string containing the \em varID of the
     * variableDef within the list of VariableDef instances to be returned from the
     * Janus instance.  A null pointer is returned if there is no corresponding variableDef
     * for the \em varID specified.
     *
     * \return The pointer to the selected VariableDef is returned, otherwise 0.
     */
    VariableDef* findVariableDef( const dstoute::aString& varID );

    /**
     * A variable's \em varID attribute is uniquely related to the
     * \em variableDef and may be used as an index.
     * This function is used by a calling program to establish the numeric index
     * associated with a variable definition. If the \em varID is known for a
     * variable definition then knowing the index provides a more efficient means
     * of interfacing with the \em variableDef as it eliminates the need to
     * perform string comparisons. The returned integer value may be used to address all
     * \em variable'-related attributes, child nodes or data elements.
     *
     * \param varID is a short string without whitespace, such as "MACH02",
     * that uniquely defines the variable of which it is an attribute.
     *
     * \return An \em index in the range from 0 to
     * (#getNumberOfVariables() - 1), corresponding to the variable
     * whose \em varID matches the input \e varID.  If the
     * input does not match any dependent variable ID within the DOM, the
     * returned value is invalid.
     */
    dstoute::aOptionalSizeT getVariableIndex( const dstoute::aString& varID ) const
    {
      return crossReferenceId( ELEMENT_VARIABLE, varID);
    }

    /**
     * The \em propertyDef elements contained in a DOM that complies with
     * the DAVE-ML DTD, defines a descriptive parameter that may be used
     * describe a property or object associated with an model. It can
     * be used to encode non-numeric parameters. In general, calling programs
     * should access property-based data through the PropertyDef instance and
     * its lower-level procedures. Provided it has been instantiated and
     * initialised without error, each Janus instance contains a ParameterDef
     * vector of instances with zero or more entries.
     *
     * \return  A reference to the list of PropertyDef instances is returned.
     */
    PropertyDefList& getPropertyDef() { return propertyDef_;}

    /**
     * As well as accessing the complete vector of PropertyDefs within a Janus
     * instance, an individual PropertyDef may be accessed by index.
     *
     * \param index is the offset of a required PropertyDef within the vector
     * of PropertyDef instances contained in a Janus instance.  It has a range
     * from 0 to (#getPropertyDef().size() - 1)
     *
     * \return The selected PropertyDef is returned by reference.
     */
    PropertyDef& getPropertyDef( dstoute::aOptionalSizeT index)
    {
      if ( index.isValid()) return propertyDef_[ index ];
      throw_message( 
        std::runtime_error, 
        dstoute::setFunctionName( "Janus::getPropertyDef( dstoute::aOptionalSizeT index)")
        << "\n - invalid index"
      );
    }

    const PropertyDef& getPropertyDef( dstoute::aOptionalSizeT index) const
    {
      if ( index.isValid()) return propertyDef_[ index ];
      throw_message( 
        std::runtime_error, 
        dstoute::setFunctionName( "Janus::getPropertyDef( dstoute::aOptionalSizeT index)")
        << "\n - invalid index"
      );
    }

    /**
     * As well as accessing the complete vector of PropertyDefs within a Janus
     * instance, an individual PropertyDef may be accessed by ptyID.
     *
     * \param ptyID is a C++ string containing the \em ptyID of a required
     * PropertyDef within the vector of PropertyDef instances contained in a
     * Janus instance.  Where no PropertyDef contains a \em ptyID matching the
     * input, a standard exception will be thrown.
     *
     * \return The selected PropertyDef is returned by reference.
     */
    PropertyDef& getPropertyDef( const dstoute::aString& ptyID );
    const PropertyDef& getPropertyDef( const dstoute::aString& ptyID) const;

    /**
     * As well as accessing the complete vector of PropertyDefs within a Janus
     * instance, an individual PropertyDef may be accessed by ptyID.
     *
     * \param ptyID is a C++ string containing the \em ptyID of a required
     * PropertyDef within the vector of PropertyDef instances contained in a
     * Janus instance.
     *
     * \return The selected PropertyDef is returned by pointer, otherwise 0.
     */
    PropertyDef* findPropertyDef( const dstoute::aString& ptyID );

    /**
     * As well as accessing the complete vector of PropertyDefs within a Janus
     * instance, an individual PropertyDef property may be accessed by ptyID.
     *
     * \param ptyID is a C++ string containing the \em ptyID of a required
     * PropertyDef within the vector of PropertyDef instances contained in a
     * Janus instance.
     *
     * \param defProperty is the default value to return if the \em ptyID is not found.
     *
     * \return the property string.
     */
    dstoute::aString getProperty( const dstoute::aString& ptyID,
                                  const dstoute::aString& defProperty = dstoute::aString());

    dstoute::aStringList getPropertyList( const dstoute::aString& ptyID,
                                          const dstoute::aStringList& defPropertyList = dstoute::aStringList());

    bool propertyExists( const dstoute::aString& ptyID);

    /**
     * This function returns the list of the breakpoint definitions,
     * \em breakpointDef, storing gridded table break point data.
     * A \em breakpointDef contains identification and cross-reference data,
     * as well as a set of independent variable values associated with one
     * of the dimensions of a gridded table of data. A breakpoint definition
     * may be used by more than one gridded table function.
     * Provided it has been instantiated without error, each Janus instance
     * contains one \em breakpointDef vector of instances. However, the vector
     * may have zero length in a dataset that does not include gridded data.
     *
     * \return A reference to the list of BreakpointDef instances is returned.
     */
    BreakpointDefList& getBreakpointDef() { return breakpointDef_;}

    /**
     * Within the DOM, a \em griddedTableDef contains points arranged in an
     * orthogonal (possibly multi-dimensional) array, where the independent
     * variables are defined by separate breakpoint vectors.  This table
     * definition may be specified within a function, or separately so
     * that it may be used by multiple functions.  Janus handles both forms
     * similarly.  The table data points are specified as comma-separated
     * values in floating-point notation (0.93638E-06) in a single long sequence
     * as if the table had been unravelled with the last-specified dimension
     * changing most rapidly.  Line breaks and comments are ignored by Janus.
     * Provided it has been instantiated without error, each Janus instance
     * contains one GriddedTableDef vector of instances; however, the vector
     * may have zero length.
     *
     * \return  A reference to the list of GriddedTableDef instances is
     * returned.
     */
    GriddedTableDefList& getGriddedTableDef()
    { return griddedTableDef_;}

    /**
     * Within the DOM, an \em ungriddedTableDef contains points that are
     * not in an orthogonal grid pattern; thus, the independent variable
     * coordinates are specified for each dependent variable value.  The table
     * data point values are specified as comma-separated values in
     * floating-point notation.
     * Provided it has been instantiated without error, each Janus instance
     * contains one UngriddedTableDef vector of instances; however, the vector
     * may have zero length.
     *
     * \return  A reference to the list of UngriddedTableDef instances is
     * returned.
     */
    UngriddedTableDefList& getUngriddedTableDef()
    { return ungriddedTableDef_;}

    /**
     * The \em function elements contained in a DOM that complies with
     * the DAVE-ML DTD, indicate how an output value is to be computed from
     * independent inputs and tabulated data, either gridded or ungridded.
     * Each \em function has an optional description, optional provenance data, and
     * either a simple table of input/output values or references to more
     * complete (possibly multiple) input, output, and function data
     * elements.  In general, calling programs should access function-based
     * data through \em variableDef procedures rather than directly
     * through the Function instance and its lower-level procedures.
     * Provided it has been instantiated without error, each Janus instance
     * contains a vector of Function instances; however, the vector may
     * have zero length.
     *
     * \return  A reference to the list of Function instances is returned.
     */
    const FunctionList& getFunction() const { return function_;}

    /**
     * As well as accessing the complete vector of Function instances within a Janus
     * instance, an individual Function may be accessed by index.
     *
     * \param index is the index of a required Function within the vector
     * of Function instances contained in a Janus instance.  It has a range
     * from 0 to (#getFunction().size() - 1)
     *
     * \return The selected Function is returned by reference.
     */
    Function& getFunction( dstoute::aOptionalSizeT index )
    { 
      if ( index.isValid()) return function_[ index];
      throw_message( 
        std::runtime_error, 
        dstoute::setFunctionName( "Janus::getFunction( dstoute::aOptionalSizeT index)")
        << "\n - invalid index"
      );
    }

    /**
     *
     * @param evaluate
     * @return
     */
    SignalDefList& getSignalDef() { return signalDef_;}

    /**
     *
     * @param evaluate
     * @return
     */
    SignalDef& getSignalDef( const dstoute::aString& sigID );

    /**
     *
     * @param evaluate
     * @return
     */
    SignalDef* findSignalDef( const dstoute::aString& sigID );

    /**
     * The DAVE-ML DTD permits an XML dataset to contain sets of input signal
     * values and the corresponding output signal values (and, optionally, the
     * corresponding internal values).  These sets of data may be used to
     * validate the functional processes represented by the remainder of the
     * XML dataset.  A dataset that includes one or more \em checkData elements
     * can therefore be self-validating.  This function provides a means for
     * calling programs to access all check data contained within a Janus
     * instance.  Not all datasets contain a \em checkData element.
     *
     * \return A reference to the CheckData instance within a Janus instance is
     * returned.
     */
    const CheckData& getCheckData( const bool &evaluate = true);

    /**
     * The DAVE-ML DTD permits an XML dataset to contain sets of input signal
     * values and the corresponding output signal values (and, optionally, the
     * corresponding internal values).  These sets of data may be used to
     * validate the functional processes represented by the remainder of the
     * XML dataset.  A dataset that includes one or more \em checkData elements
     * can therefore be self-validating.  This function provides a means for
     * calling programs to display a summary of the check data contained within
     * a Janus instance.  Not all datasets contain a \em checkData element.
     *
     * \param checkData A reference to the CheckData instance within a
     * Janus instance.
     */
    void displayCheckDataSummary( const CheckData& checkData);

    /** @} */ // end of level1Group

    /*
     * Include Janus deprecated function definitions
     */
    #ifdef JANUS_USE_DEPRECATED
    #include "JanusDeprecated.h"
    #endif


    // ---- Internally reference functions. ----
    dstoute::aOptionalSizeT crossReferenceId( ElementDefinitionEnum elementType,
                                              const dstoute::aString& checkID) const;
    dstoute::aOptionalSizeT crossReferenceName( ElementDefinitionEnum elementType,
                                                const dstoute::aString& checkName) const;
    size_t isUniqueID( ElementDefinitionEnum& elementType,
                       const dstoute::aString& checkID) const;

    // Virtual functions overloading functions in the DomFunctions class.
    void readDefinitionFromDom( const DomFunctions::XmlNode& elementDefinition );

    void setMathML_to_ExprTk( bool doMathML_to_ExprTk) { doMathML_to_ExprTk_ = doMathML_to_ExprTk;}
    bool getMathML_to_ExprTk() const                   { return doMathML_to_ExprTk_;}

   protected:
    /** @defgroup janusProtectedGroup Janus - Internal Functions
     *
     *  @{
     */


    /**
     * This is an internal function that performs multi-dimensional linear
     * interpolation when all the degrees of freedom of a tabulated function are
     * specified as or default to linear or discrete interpolation.  This
     * separation
     * from other interpolation schemes is designed to maximise speed for
     * this case in particular, which is expected to be generally used for
     * real time or faster computations.
     *
     * \param function is a reference to the Function instance in use.
     * \param dataTable is a reference to the \em griddedDataTable_ element
     * of the Function instance in use.  This argument is only necessary for
     * computation of uncertainty bounds.
     *
     * \return the result of the multi-dimensional linear interpolation
     * is returned in double precision.
     */
    double getLinearInterpolation( Function& function,
                                   const std::vector<double>& dataTable);
    double getLinearInterpolation( Function& function )
    { return getLinearInterpolation( function, function.getData());}


    /**
     * This is an internal function that performs multi-dimensional
     * polynomial interpolation on a tabulated function.  It includes the
     * situation where different degrees of freedom are of different order,
     * including linear.  It
     * is at present limited to a maximum order of 3, and is significantly
     * slower than #getLinearInterpolation.
     *
     * \param function is a reference to the Function instance in use.
     * \param dataTable is a reference to the griddedDataTable_ element
     * of the Function instance in use.  This argument is only necessary for
     * computation of uncertainty bounds.
     *
     * \return the result of the multi-dimensional polynomial interpolation
     * is returned in double precision.
     */
    double getPolyInterpolation( Function& function,
                                 const std::vector<double>& dataTable);
    double getPolyInterpolation( Function& function )
    { return getPolyInterpolation( function, function.getData());}


    /**
     * This is an internal function that performs multi-dimensional
     * interpolation on ungridded data.
     *
     * \param function is a reference to the Function instance in use.
     * \param dataColumn is a reference to the ungriddedDataColumn_ element
     * of the Function instance in use.  This argument is only necessary for
     * computation of uncertainty bounds.
     *
     * \return the result of the multi-dimensional linear interpolation
     * is returned in double precision.
     */

    double getUngriddedInterpolation( Function& function,
                                      const std::vector<double>& dataColumn);
    double getUngriddedInterpolation( Function& function)
    { return getUngriddedInterpolation( function, function.getData());}

    /************************************************************************
     * The following protected elements provide the structure for MathML low
     * level operations.  During initialisation, a linked list of mathematical
     * operations and their associated variables is set up for each
     * variableDef involving an MathML calculation.  The list's components
     * are shown below.
     */

    /**
     * Called during initialisation, this function finds all variables that
     * depend, however indirectly, on the \em variableDef specified by the index.
     *
     * \return A vector of indices to all descendent variables.
     */
    std::vector<size_t> getAllDescendents( size_t index, bool checkRecursively = false);


    /**
     * Called during initialisation, this function finds all variables
     * that the \em variableDef specified by the index depends.
     *
     * \return A vector of indices to all ancestor variables.
     */
    std::vector<size_t> getAllAncestors( size_t ix);


    /**
     * Called during initialisation, this function finds all TYPE_INPUT or
     * TYPE_OUTPUT variables that \em variableDef specified by the index depends.
     *
     * \return A vector of indices to all ultimate ancestor variables.
     */
    std::vector<size_t> getIndependentAncestors( size_t ix);


    /**
     * Called during initialisation, this function determines the PDF of
     * the uncertainty associated with a variable, based on the tables and
     * independent variables contributing to the variable.
     */
    Uncertainty::UncertaintyPdf getPdfFromAntecedents( size_t index);

    /**
     * This function is used to set the variable that indicates that the
     * Janus instance has been initialised.
     *
     * \param isInitialised a boolean value of True or False indicating the
     * status of the Janus instance initialisation.
     */
    void isJanusInitialised( bool isInitialised)
    {
      isJanusInitialised_ = isInitialised;
    }

    /**
     * Called during initialisation, this function returns a reference to a provenance
     * record.
     * \param parentID is the identifier of the provenance's parent element.
     * \param provIndex is the index of the provenance element
     * \return The function returns the a reference to the requested provenance class.
     */
    const Provenance& retrieveProvenanceReference( const dstoute::aString& parentID,
                                                   size_t provIndex);


    /**
     * This function is used to export the contents of the Janus constructs to
     * a DOM document.
     *
     * \param documentType this is a string indicating the document type
     * definition of exported XML data. The default document type
     * is \em DAVEfunc indicating that the DOM is encoded using the DAVE-ML
     * syntax as defined in the DAVEfunc.dtd. XML syntax and applications that
     * build upon Janus may define alternative document types, and accompanying
     * document type definitions, such as \em THAMESfunc, which is the
     * Time History for Aircraft Modelling Exchange Syntax.
     */
    virtual void exportToDocumentObjectModel( const dstoute::aString& documentType = "DAVEfunc");
  
    /**
     * This function allows the calling program to duplicate the contents
     * of the DOM, as currently loaded, into an XML text file. Before this
     * function is called a DOM will need to be created using the function
     * \em initiateDocumentObjectModel(), and then populated with the
     * desired data. The method of populating the DOM will depend on
     * the source of the data, be it stored internally within the Janus
     * constructs, or via an external source such as Matlab/Octave
     * structures. Failures occurring while writing the contents of the
     * DOM to an file may throw XML exceptions or DOM exceptions.
     *
     * This function may be overloaded by equivalent functions for projects
     * that inherit Janus.
     *
     * \param dataFileName is a legal, fully qualified file name to which
     * the current contents of the DOM will be written in XML format.
     *
     * \return A value of \em true is returned if the XML file is written
     * successfully.
     */
    bool writeDocumentObjectModel( const dstoute::aFileString& dataFileName) const;

    /**
     * This function allows the calling program to duplicate the contents
     * of the DOM, as currently loaded, into an XML data buffer. Before this
     * function is called a DOM will need to be created using the function
     * \em initiateDocumentObjectModel(), and then populated with the
     * desired data. The method of populating the DOM will depend on
     * the source of the data, be it stored internally within the Janus
     * constructs, or via an external source such as Matlab/Octave
     * structures. Failures occurring while writing the contents of the
     * DOM to an file may throw XML exceptions or DOM exceptions.
     *
     * This function may be overloaded by equivalent functions for projects
     * that inherit Janus.
     *
     * \param sstr is a buffer stream to which the current contents of
     * the DOM will be written in XML format.
     *
     * \return A value of \em true is returned if the XML file is written
     * successfully.
     */
    bool writeDocumentObjectModel( std::ostringstream& sstr) const;

    /**
     * This function is called at the end of the initialisation sequence of a file
     * to release the memory that is being used to store data in a DOM.
     */
    void releaseJanusDomParser()
    {
      document_.reset();
    }

    /**
     * This function is used to parse the document object model instantiated
     * from either an XML file or a buffer containing the contents of an XML
     * file. The function is called from the setXMLFileName() and the
     * setXMLFileBuffer() functions. XML syntax and applications that
     * build upon Janus may define alternative versions of this function
     * to extend its capability.
     */
    virtual void parseDOM();

    /**
     * This function is used to set dependencies and cross-references between
     * variable definitions and other elements of a dataset. This function is
     * executed after the file has been parsed from the DOM and all the dataset
     * elements have been instantiated. XML syntax and applications that
     * build upon Janus may define alternative versions of this function
     * to extend its capability.
     */
    virtual void initialiseDependencies();

    /** @} */ // end of protectedGroup

    void deleteLuaState();

    /**********************************************************************
     * These are the top level elements of the DOM, and their related
     * flags and counters.
     */
    DomFunctions::XmlDoc  document_;
    bool                  documentCreated_;

    dstoute::aFileString  dataFileName_;
    dstoute::aFileString  keyFileName_;
    dstoute::aString      documentType_;

    bool                  isJanusInitialised_;

    FileHeader            fileHeader_;
    VariableDefList       variableDef_;
    PropertyDefList       propertyDef_;
    BreakpointDefList     breakpointDef_;
    GriddedTableDefList   griddedTableDef_;
    UngriddedTableDefList ungriddedTableDef_;
    FunctionList          function_;
    SignalDefList         signalDef_;
    CheckData             checkData_;
    bool                  hasCheckData_;
    bool                  isCheckDataCurrent_;

    /**********************************************************************
     * Experimental seperate perturbation dataset
     */
    dstoute::aFileString perturbationsFileName_;

    /************************************************************************
     * Indices to the output variables, which can be of three
     * types.  They are not a single definition within the DTD, but are
     * aggregated from functions, MathML evaluations, and explicitly
     * defined output values.
     */

    std::vector<dstoute::aOptionalSizeT> outputIndex_;

    /************************************************************************
     * These are statically defined indices and the fractions for linear or
     * polynomial interpolation (see LinearInterpolation.cpp,
     * PolyInterpolation.cpp), workspace for ungridded interpolation (see
     * UngriddedInterpolation.cpp), save overhead in allocating then
     * freeing memory for each interpolation.
     */

    std::valarray<int>    nbp_;
    std::valarray<double> frac_;
    std::valarray<int>    bpa_;
    std::valarray<int>    bpi_;
    std::valarray<int>    nOrd_;
    dstomath::DMatrix     fracBp_;

    /*
     * Script elements
     */
    bool doMathML_to_ExprTk_;
    void *kState_;

    /************************************************************************
     * Workspace size limits
     */

  #if !defined(MAXDOF)
  #define MAXDOF            32
  #endif

  #if !defined(MAXPOLYORDER)
  #define MAXPOLYORDER       3
  #endif

  #if !defined(EPS_S)
  #define EPS_S             1.4901162e-08 // sqrt( eps ) from octave
  #endif

  #if !defined(EPS)
  #define EPS               2.2204461e-16 // eps from octave
  #endif

  #if !defined(EPSILON)
  #define EPSILON           1.0e-5
  #endif
  
  private:
    friend class VariableDef;
    friend class Function;
    friend class Bounds;

    void resetJanus();
    size_t exportTo( ExportObjectType exportObjectType,
                     const dstoute::aFileString& dataFileName,
                     std::ostringstream& dataFileBuffer);
  };

}  /* janus namespace */

#endif  /* _JANUS_H_ */
