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
// Title:      Janus/Signal
// Class:      Signal
// Module:     Signal.cpp
// First Date: 2009-05-07
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file Sgnl.cpp
 *
 * A Signal instance holds in its allocated memory alphanumeric data
 * derived from a \em signal element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  The instance may describe
 * inputs, internal values of a computation, or outputs.  The class also
 * provides the functions that allow a calling StaticShot instance to access
 * these data elements. It is used to document the name, ID, value, tolerance,
 * and units of measure for checkcases.
 *
 * A \em signal must have \em signalName and \em signalUnits if it is
 * a child of \em checkInputs or \em checkOutputs.  Alternatively, if it is a
 * child of \em internalValues, it must have a \em varID (\em signalID is
 * deprecated).  When used in a \em checkOutputs vector, the \em tol element
 * must be present. Tolerance is specified as a maximum absolute difference
 * between the expected and actual value. This class accepts whichever of these
 * children it finds in the XML dataset, and leaves applicability to its parents
 * to sort out.
 *
 * The Signal class is only used within the janus namespace, and should
 * only be referenced indirectly through the StaticShot, CheckInputs,
 * InternalValues and CheckOutputs classes.
 */

/*
 * Author:  D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 *
 */

// Ute Includes
#include <Ute/aMessageStream.h>
#include <Ute/aMath.h>

#include "DomFunctions.h"
#include "Sgnl.h"

using namespace std;
using namespace dstoute;

namespace janus {

//------------------------------------------------------------------------//

Signal::Signal()
  :
  XmlElementDefinition(),
  value_( dstomath::nan()),
  tol_( 1.0e-10),
  actualValue_( dstomath::nan())
{
}

Signal::Signal(
  const DomFunctions::XmlNode& elementDefinition,
  const SignalTypeEnum &signalType )
  :
  XmlElementDefinition(),
  value_( dstomath::nan()),
  tol_( 1.0e-10),
  actualValue_( dstomath::nan())
{
  initialiseDefinition( elementDefinition, signalType );
}

//------------------------------------------------------------------------//

void Signal::initialiseDefinition(
  const DomFunctions::XmlNode& elementDefinition,
  const SignalTypeEnum &signalType )
{
  static const aString functionName( "Signal::initialiseDefinition()");

  switch ( signalType) {
    case SIGNAL_CHECKOUTPUTS:
      if ( !DomFunctions::isChildInNode( elementDefinition, "tol" )) {
        throw_message( range_error,
          setFunctionName( functionName)
          << "\n - CheckOutput Signal does not have a tol element."
        );
      }
      /* FALLTHRU */

    case SIGNAL_CHECKINPUTS:
      units_ = DomFunctions::getChildValue( elementDefinition, "signalUnits" );
      name_  = DomFunctions::getChildValue( elementDefinition, "signalName" );
      if ( name_.size() == 0) {
        throw_message( range_error,
          setFunctionName( functionName)
          << "\n - CheckInput/CheckOutput Signal does not have a signalName element."
        );
      }
      break;

    case SIGNAL_INTERNALVALUES:
      varID_ = DomFunctions::getChildValue( elementDefinition, "varID" );
      if ( varID_.size() == 0) {
        varID_ = DomFunctions::getChildValue( elementDefinition, "signalID" );
      }
      break;

    default:
      break;
  }

  value_ = DomFunctions::getChildValue( elementDefinition, "signalValue" ).toDouble();

  if ( DomFunctions::isChildInNode( elementDefinition, "tol")) {
    tol_ = DomFunctions::getChildValue( elementDefinition, "tol" ).toDouble();
    if ( dstomath::isZero( tol_)) {
      tol_ = dstomath::zero();
    }
  }
}

//------------------------------------------------------------------------//

void Signal::exportDefinition(
  DomFunctions::XmlNode& documentElement)
{
  /*
   * Create a child node in the DOM for the Signal element
   */
  DomFunctions::XmlNode childElement =
    DomFunctions::setChild( documentElement, "signal");

  if ( varID_.empty()) {
    DomFunctions::setChild( childElement, "signalName", name_);
    DomFunctions::setChild( childElement, "signalUnits", units_);
  }
  else {
    DomFunctions::setChild( childElement, "varID", varID_);
  }

  DomFunctions::setChild( childElement, "signalValue",
                          aString("%").arg( value_, 16));

  if ( !dstomath::isnan( tol_) && !dstomath::isZero( tol_)) {
    DomFunctions::setChild( childElement, "tol",
                            aString("%").arg( tol_, 16));
  }
}

//------------------------------------------------------------------------//

ostream& operator<< (
  ostream &os,
  const Signal &signal)
{
  /*
   * General properties of the Class
   */
  os << endl << endl
     << "Display Signal contents:" << endl
     << "-----------------------------------" << endl;

  os << "  name               : " << signal.getName() << endl
     << "  units              : " << signal.getUnits() << endl
     << "  varID              : " << signal.getVarID() << endl
     << "  value              : " << signal.getValue() << endl
     << "  actual value       : " << signal.getActualValue() << endl
     << "  tolerance          : " << signal.getTolerance() << endl
     << endl;

  return os;
}

//------------------------------------------------------------------------//

} // namespace janus

