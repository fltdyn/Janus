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
// Title:      Janus/VariableDef
// Class:      VariableDef
// Module:     VariableDef.cpp
// First Date: 2009-06-05
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file VariableDef.cpp
 *
 * A VariableDef instance holds in its allocated memory alphanumeric data
 * derived from a \em variableDef element of a DOM corresponding to
 * a DAVE-ML compliant XML dataset source file.  It includes descriptive,
 * alphanumeric identification and cross-reference data, and may include
 * a calculation process tree for variables computed through MathML.
 * The variable definition can include statistical information regarding
 * the uncertainty of the values that it might take on, when measured
 * after any calculation is performed. This class sets up a structure
 * which manages the \em variableDef content.
 *
 * The VariableDef class is only used within the janus namespace, and should
 * only be referenced through the Janus class.
 */

/*
 * Author:  D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 *
 */

#include <algorithm>
#ifdef _MSC_VER
#include <functional>
#else
#include <tr1/functional>
#endif

// Ute Includes
#include <Ute/aMessageStream.h>
#include <Ute/aString.h>
#include <Ute/aMath.h>
#include <Ute/aOptional.h>

#include "Janus.h"
#include "DomFunctions.h"
#include "VariableDef.h"

#include "ParseMathML.h"
#include "SolveMathML.h"
#include "ExportMathML.h"

using namespace std;
using namespace dstoute;
using namespace dstomath;
using namespace dstomathml;
using namespace parsemathml;
using namespace solvematrixmathml;
using namespace exportmathml;

namespace janus {

aString mathML_to_ExprTkScript( const DomFunctions::XmlNode& xmlElement, const aString &varID);

aStringList variableTypeStringList = ( aStringList()
  << "INTERNAL"
  << "INPUT"
  << "OUTPUT"
);
aStringList variableMethodStringList = ( aStringList()
  << "PLAIN VARIABLE"
  << "FUNCTION"
  << "MATHML"
  << "SCRIPT"
  << "ARRAY"
  << "MODEL"
);


//------------------------------------------------------------------------//

VariableDef::VariableDef()
  :
  XmlElementDefinition(),
  janus_( 0),
  elementType_( ELEMENT_VARIABLE),
  varIndex_(  aOptionalSizeT::invalidValue()),
  initialValue_( nan()),
  minValue_( -std::numeric_limits<double>::max()),
  maxValue_(  std::numeric_limits<double>::max()),
  isProvenanceRef_( false),
  hasProvenance_( false),
  isInput_( false),
  isControl_( false),
  isDisturbance_( false),
  isOutput_( false),
  isState_( false),
  isStateDeriv_( false),
  isStdAIAA_( false),
  hasUncertainty_( false),
  exportUncertainty_(false),
  variableType_( TYPE_INTERNAL),
  variableMethod_( METHOD_PLAIN_VARIABLE),
  functionRef_( aOptionalSizeT::invalidValue()),
  outputScaleFactor_( 1.0),
  hasOutputScaleFactor_( false),
  isCurrent_( false),
  value_( nan()),
  isCurrentVariance_( false),
  variance_( 0.0),
  isCurrentBound_( false),
  lowerBound_( 0.0),
  upperBound_( 0.0),
  isForced_( false),
  isReferencedExternally_( false),
  isDimensionRef_( false),
  hasDimensionDef_( false),
  hasVarIdEntries_( false),
  isMatrix_( false),
  hasMatrixOps_( false),
  matrix_( 1, 1, 0.0),
  matrixScaleFactor_( 1, 1, 1.0),
  matrixVarId_( 1, 1, aOptionalSizeT::invalidValue()),
  scriptType_( NO_SCRIPT),
  script_(),
  scriptContainsThisVarDef_( false),
  thisVarDef_( 0),
  eFunc_( 0),
  kFunc_( 0),
  isCurrentable_( true),
  inEvaluation_( false),
  hasPerturbation_( false),
  associatedPerturbationVarIndex_( aOptionalSizeT::invalidValue()),
  perturbationTargetVarIndex_( aOptionalSizeT::invalidValue()),
  perturbationEffect_( UNKNOWN_UNCERTAINTY),
  inputWarningDone_( false),
  outputWarningDone_( false)
{
  mathCalculation_.janus_ = janus_;
}

//------------------------------------------------------------------------//

VariableDef::VariableDef(
  Janus* janus,
  const DomFunctions::XmlNode& elementDefinition)
  :
  XmlElementDefinition(),
  janus_( janus),
  elementType_( ELEMENT_VARIABLE),
  varIndex_( aOptionalSizeT::invalidValue()),
  initialValue_( nan()),
  minValue_( -std::numeric_limits<double>::max()),
  maxValue_(  std::numeric_limits<double>::max()),
  isProvenanceRef_( false),
  hasProvenance_( false),
  isInput_( false),
  isControl_( false),
  isDisturbance_( false),
  isOutput_( false),
  isState_( false),
  isStateDeriv_( false),
  isStdAIAA_( false),
  hasUncertainty_( false),
  exportUncertainty_(false),
  variableType_( TYPE_INTERNAL),
  variableMethod_( METHOD_PLAIN_VARIABLE),
  functionRef_( aOptionalSizeT::invalidValue()),
  outputScaleFactor_( 1.0),
  hasOutputScaleFactor_( false),
  isCurrent_( false),
  value_( nan()),
  isCurrentVariance_( false),
  variance_( 0.0),
  isCurrentBound_( false),
  lowerBound_( 0.0),
  upperBound_( 0.0),
  isForced_( false),
  isReferencedExternally_( false),
  isDimensionRef_( false),
  hasDimensionDef_( false),
  hasVarIdEntries_( false),
  isMatrix_( false),
  hasMatrixOps_( false),
  matrix_( 1, 1, 0.0),
  matrixScaleFactor_( 1, 1, 1.0),
  matrixVarId_( 1, 1, aOptionalSizeT::invalidValue()),
  scriptType_( NO_SCRIPT),
  script_(),
  scriptContainsThisVarDef_( false),
  thisVarDef_( 0),
  eFunc_( 0),
  kFunc_( 0),
  isCurrentable_( true),
  inEvaluation_( false),
  hasPerturbation_( false),
  associatedPerturbationVarIndex_( aOptionalSizeT::invalidValue()),
  perturbationTargetVarIndex_( aOptionalSizeT::invalidValue()),
  perturbationEffect_( UNKNOWN_UNCERTAINTY),
  inputWarningDone_( false),
  outputWarningDone_( false)
{
  mathCalculation_.janus_ = janus;

  initialiseDefinition( janus, elementDefinition);
}

VariableDef::~VariableDef()
{
  deleteExprTkFunction();
  deleteLuaFunction();
}

//------------------------------------------------------------------------//

const dstoute::aString& VariableDef::getTypeString( ) const
{
  return variableTypeStringList[ variableType_];
}

//------------------------------------------------------------------------//

const dstoute::aString& VariableDef::getMethodString( ) const
{
  return variableMethodStringList[ variableMethod_];
}

//------------------------------------------------------------------------//

bool VariableDef::isCalculation() const
{
  switch ( variableMethod_) {
  case VariableDef::METHOD_MATHML:
  case VariableDef::METHOD_SCRIPT:
    return true;

  default:
    break;
  }
  return false;
}

bool VariableDef::isInitialValueAllowed() const
{
  switch ( variableMethod_) {
  case METHOD_PLAIN_VARIABLE:
  case METHOD_MATHML:
  case METHOD_SCRIPT:
  case METHOD_ARRAY:
    return true;

  default:
    break;
  }
  return false;
}

//------------------------------------------------------------------------//

void VariableDef::initialiseDefinition(
  Janus* janus,
  const DomFunctions::XmlNode& elementDefinition)
{
  static const aString functionName( "VariableDef::initialiseDefinition()");

  janus_ = janus;
  domElement_ = elementDefinition;

  mathCalculation_.janus_ = janus_;

  /*
   * Dimension definition
   */
  elementType_ = ELEMENT_DIMENSION;
  try {
    DomFunctions::initialiseChildOrRef( 
      this,
      elementDefinition,
      EMPTY_STRING,
      "dimensionDef",
      "dimensionRef",
      "dimID",
      false);
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - "
      << excep.what()
    );
  }

  /*
   * Drop through to the original VariableDef class function to populate
   * other entries of the variable.
   */
  try {
    initialiseBaseVariableDef( elementDefinition);
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - "
      << excep.what()
    );
  }

  /*
   * If the variableDef represents a vector or a matrix then check
   * that it has been defined either using an array element, a
   * calculation element, a script definition, or a dynamic system model.
   *
   * If none of the elements have been defined for the variableDef then
   * return an error message.
   *
   * N.B. a vector or matrix variableDef is identified by having a
   * dimensionDef or dimensionRef element.
   */

  /*
   * Check if an array or model element is defined for the variable.
   * If no calculation, array or model element send an error message.
   */
  bool isCalculationAvailable = DomFunctions::isChildInNode( elementDefinition, "calculation");
  bool isArrayAvailable       = DomFunctions::isChildInNode( elementDefinition, "array");
  bool isModelAvailable       = DomFunctions::isChildInNode( elementDefinition, "model");
  if (( isArrayAvailable || isCalculationAvailable || isModelAvailable) &&
     !( isArrayAvailable ^  isCalculationAvailable ^  isModelAvailable)) {
    throw_message( range_error,
      setFunctionName( functionName)
      << "\n - varID \"" << varID_
      << "\" has more than 1 array/calculation/model entries."
    );
  }

  bool isWrongScriptAvailable = DomFunctions::isChildInNode( elementDefinition, "script");
  if ( isWrongScriptAvailable) {
    throw_message( range_error,
      setFunctionName( functionName)
      << "\n - varID \"" << varID_
      << "\" - Need to place script inside <calculation> element."
    );
  }

  if ( dimensionDef_.getDimCount() > 0) {
    size_t dimTotal   = dimensionDef_.getDimTotal();
    size_t index      = dimensionDef_.getDimCount() - 1;
    size_t baseDimRef = dimensionDef_.getDim( index);

    if ( dimensionDef_.getDimCount() == 1) {
      matrix_.resize( baseDimRef, 1);
    }
    else {
      matrix_.resize(( dimTotal / baseDimRef),
                     baseDimRef);
    }
    isMatrix_ = true;

    /*
     * Retrieve Array definition.
     * This requires the dimensions of the vector or matrix to be defined first
     */
    if ( isArrayAvailable) {
      elementType_ = ELEMENT_ARRAY;
      try {
        DomFunctions::initialiseChild( 
          this,
          elementDefinition,
          varID_,
          "array",
          false);
      }
      catch ( exception &excep) {
        throw_message( invalid_argument,
          setFunctionName( functionName)
          << "\n - "
          << excep.what()
        );
      }
    }
  }

  // Scalar/Vector/Matrix Initial Values.
  initialValueString_ = DomFunctions::getAttribute( elementDefinition, "initialValue");
  if ( !initialValueString_.empty() && isInitialValueAllowed())
  {
    // Vector/Matrix value.
    if ( isMatrix_) {
      /*
       * Check if an initial value(s) has been set.
       * Populate based on number of initialValues.
       */
      aStringList initData = initialValueString_.toStringList( " \t\n\r,;");
      if ( initData.size() == 1) {
        matrix_    = initData.front().toDouble();
        isCurrent_ = !isCalculation();
        value_     = nan();
      }
      else if ( initData.size() == matrix_.size()) {
        for ( size_t i = 0; i < matrix_.size(); ++i) {
          matrix_.matrixData()[ i] = initData[ i].toDouble();
        }
        isCurrent_ = !isCalculation();
        value_     = nan();
      }
      else {
        throw_message( range_error,
          setFunctionName( functionName)
          << "\n - varID \"" << varID_
          << "\" - Invalid number of initial values for vector/matrix. Must be 1 or " << matrix_.size()
        );
      }
    }

    // Scalar value.
    else {
      if ( initialValueString_.isNumeric()) {
        initialValue_ = initialValueString_.toDouble();
        value_        = initialValue_;
        isCurrent_    = !isCalculation();
      }
    }
  }
  if ( !isMatrix_) {
    minValueString_ = DomFunctions::getAttribute( elementDefinition, "minValue");
    if ( minValueString_.isNumeric()) {
      minValue_  = minValueString_.toDouble();
      value_     = dstomath::max( minValue_, value_);
      isCurrent_ = true;
    }

    maxValueString_ = DomFunctions::getAttribute( elementDefinition, "maxValue");
    if ( maxValueString_.isNumeric()) {
      maxValue_  = maxValueString_.toDouble();
      value_     = dstomath::min( value_, maxValue_);
      isCurrent_ = true;
    }
  }

  /*
   * Check for an model element if the variableDef represents a dynamic
   * system model: transfer function or state-space.
   *
   * Retrieve Dynamic System Model definition if present
   */
  if ( isModelAvailable) {
    elementType_ = ELEMENT_MODEL;
    try {
      DomFunctions::initialiseChild(
          this,
          elementDefinition,
          varID_,
          "model",
          false);
    }
    catch ( exception &excep) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - "
        << excep.what()
      );
    }
  }

  /*
   * Set up ancestor cross references.
   */
  //setAncestorCrossReferences( independentVarRef_);

  /*
   * Get the current units and set up conversion factors using aUnitConverter.
   *
   * Note: The AIAA/ANSI S-119-2011 standard defines the following
   * dgC, dgF, dgR, dgK for temperature as opposed to the unit definitions
   * used here of C, F, R, K  */
  xmlUnits_.clear();
  try {
    xmlUnits_ = findUnits( units_);
  }
  catch (...) {
  }

  return;
}

//------------------------------------------------------------------------//

VariableDef::VariableFlag VariableDef::getVariableFlag() const
{
  if ( true == isState_ && true == isStdAIAA_) {
    return ISSTATE_STDAIAA;
  }
  else if ( true == isStateDeriv_ && true == isStdAIAA_) {
    return ISSTATEDERIV_STDAIAA;
  }
  else if ( true == isState_) {
    return ISSTATE;
  }
  else if ( true == isStateDeriv_) {
    return ISSTATEDERIV;
  }
  else if ( true == isStdAIAA_) {
    return ISSTDAIAA;
  }

  return ISERRORFLAG;
}

//------------------------------------------------------------------------//

bool VariableDef::isValue()
{
  if ( !isCurrent_) {
    solveValue();
  }

  return !isMatrix_;
}

//------------------------------------------------------------------------//

bool VariableDef::isVector()
{
  if ( !isCurrent_) {
    solveValue();
  }

  return ( isMatrix_ && matrix_.isVector());
}

//------------------------------------------------------------------------//

bool VariableDef::isMatrix()
{
  if ( !isCurrent_) {
    solveValue();
  }

  return isMatrix_;
}

//------------------------------------------------------------------------//

double VariableDef::getValue() const
{
  if ( !isCurrent_) {
    solveValue();
  }

  math_range_check(
    if ( isMatrix_ && !matrix_.isSingleValue()) {
      throw_message( runtime_error,
        setFunctionName( "VariableDef::getValue()")
        << "\n - variable \"" << name_ << " is not of type value."
      );
    }
  );

  return value_;
}

//------------------------------------------------------------------------//

const DVector& VariableDef::getVector()
{
  if ( !isCurrent_) {
    solveValue();
  }

  math_range_check(
    if ( !isMatrix_ || !matrix_.isVector()) {
      throw_message( runtime_error,
        setFunctionName( "VariableDef::getVector()")
        << "\n - variable \"" << name_ << " is not of type vector."
      );
    }
  );

  return matrix_.matrixData();
}

//------------------------------------------------------------------------//

const DMatrix& VariableDef::getMatrix()
{
  if ( !isCurrent_) {
    solveValue();
  }

  math_range_check(
    if ( !isMatrix_ || matrix_.isSingleValue()) {
      throw_message( runtime_error,
        setFunctionName( "VariableDef::getMatrix()")
        << "\n - variable \"" << name_ << " is not of type matrix."
      );
    }
  );

  return matrix_;
}

//------------------------------------------------------------------------//

void VariableDef::solveValue() const
{
  static const aString functionName( "VariableDef::solveValue()");

  /*
   *     First check input variables computed as necessary to ensure that
   *     the final result will reflect the current state of all
   *     contributing input variables.  This can lead to recursion.
   *
   *     may not need to go entirely up the tree if a previous evaluation
   *     has satisfied requirements.
   */

  // Exit if currently in evaluation due to setVarDef
  if ( inEvaluation_) return;

  for ( vector<size_t>::const_iterator indRefIdx = independentVarRef_.begin();
    indRefIdx != independentVarRef_.end(); ++indRefIdx) {
    VariableDef& indepVariableDef = janus_->variableDef_.at( *indRefIdx);

    if ( !indepVariableDef.isCurrent_) {
      indepVariableDef.solveValue();
    }
  }

  switch ( variableMethod_) {
    case METHOD_PLAIN_VARIABLE:
      if ( hasPerturbation_) {
        value_ = initialValue_;
      }
      break;

    case METHOD_FUNCTION:
    {
      /*
       * 1.  Tabular function, either linear or polynomial interpolation.
       */
      Function& thisFunction = janus_->function_[ functionRef_ ];
      if ( thisFunction.getData().size() > 0) {
        if ( ELEMENT_GRIDDEDTABLE == thisFunction.getTableType()) {
          // gridded numeric
          if ( thisFunction.isAllInterpolationLinear()) {
            value_ = janus_->getLinearInterpolation( thisFunction);
          }
          else {
            value_ = janus_->getPolyInterpolation( thisFunction);
          }
        }
        else {
          // ungridded numeric
          value_ = janus_->getUngriddedInterpolation( thisFunction);
        }
      }
      //else string data - leave numeric value alone
    }
    break;

    case METHOD_MATHML:
      /*
       * 3.  MathML function evaluation required.
       */
      solveMath();
      if ( hasOutputScaleFactor_) {
        if ( isMatrix_) {
          matrix_ *= outputScaleFactor_;
        }
        else {
          value_ *= outputScaleFactor_;
        }
      }
      break;

    case METHOD_SCRIPT:
      /*
       * 4.  Lua script function evaluation required.
       */
      switch ( scriptType_) {
      case LUA_SCRIPT:
        solveLuaScript();
        break;

      case EXPRTK_SCRIPT:
      default:
        solveExprTkScript();
        break;
      }
      if ( hasOutputScaleFactor_) {
        if ( isMatrix_) {
          matrix_ *= outputScaleFactor_;
        }
        else {
          value_ *= outputScaleFactor_;
        }
      }
      break;

    case METHOD_ARRAY:
      if ( isMatrix_ && hasVarIdEntries_) {
        evaluateDataTable();
      }
      break;

    case METHOD_MODEL:
      break;

    default:
      throw_message( runtime_error,
        setFunctionName( functionName)
        << "\n - Impossible situation - see VariableDef.cpp."
      );
      break;
  }

  if ( isMatrix_) {
    if ( matrix_.size() == 1) {
      value_ = matrix_.matrixData()[0];
    }
  }
  else {
    if ( !minValueString_.empty() && value_ < minValue_) value_ = minValue_;
    if ( !maxValueString_.empty() && value_ > maxValue_) value_ = maxValue_;
    if ( matrix_.size() != 1) {
      matrix_ = DMatrix( 1, 1, value_);
    }
  }

  if ( hasPerturbation_) {
    applyPerturbation();
  }

  isCurrent_ = isCurrentable_;
}

//------------------------------------------------------------------------//

double VariableDef::getValueSI() const
{
  xmlUnits_.setValue( getValue());
  return xmlUnits_.valueSI();
}

//------------------------------------------------------------------------//

double VariableDef::getValueMetric() const
{
  xmlUnits_.setValue( getValue());
  return xmlUnits_.valueMetric();
}

//------------------------------------------------------------------------//

const double& VariableDef::getVariance()
{
  /*
   * Check whether the variance for the variableDef is current, or
   * the uncertainty type is a normal pdf.
   */
  if ( !isCurrentVariance_) {
    const Uncertainty::UncertaintyPdf& pdf = uncertainty_.getPdf();
    switch ( pdf) {
      case Uncertainty::UNIFORM_PDF:
      case Uncertainty::ERROR_PDF:
        // wrong PDF type
        variance_ = nan();
        break;

      case Uncertainty::UNKNOWN_PDF:
        variance_ = 0.0; // no uncertainty specified
        isCurrentVariance_ = true;
        break;

      case Uncertainty::NORMAL_PDF:
        /*
         * Compute the variance of the Normal PDF for the variableDef.
         */
        variance_ = computeVarianceForNormalPdf();
        isCurrentVariance_ = true;
        break;

      default:
        break;
    }
  }

  return variance_;
}

//------------------------------------------------------------------------//

double VariableDef::computeVarianceForNormalPdf()
{
  static const aString functionName( "VariableDef::computeVarianceForNormalPdf()");

  /*
   * Compute the variance of Normal PDF for the variableDef.
   */
  bool   appliedAtOutput = false;
  double variance = 0.0;
  double sigmaFactor = 0.0;
  double sdBound = 0.0;
  double stdDev;
  UncertaintyEffect effect = UNKNOWN_UNCERTAINTY;

  /*
   * check if applied directly to variable at output
   */
  if ( !uncertainty_.isSet()) {
    switch ( variableMethod_) {
      case METHOD_FUNCTION:
      {
        const Function& thisFunction = janus_->getFunction( functionRef_);
        aOptionalSizeT tableRef = thisFunction.getTableRef();

        if ( tableRef.isValid()) {
          Uncertainty& functionUncertainty =
              janus_->griddedTableDef_[ tableRef ].getUncertainty();

          if ( ELEMENT_UNGRIDDEDTABLE == thisFunction.getTableType()) {
            functionUncertainty = janus_->ungriddedTableDef_[ tableRef ].getUncertainty();
          }

          if ( functionUncertainty.isSet()) {
            effect  = functionUncertainty.getEffect();
            sdBound = functionUncertainty.getBounds()[0].getBound( functionRef_);
            sigmaFactor = 1.0 / (double)functionUncertainty.getNumSigmas();
            appliedAtOutput = true;
          }
        }
      }
      /* FALLTHRU */

      case METHOD_MATHML:
        if ( !appliedAtOutput) {
          size_t n = independentVarRef_.size();
          vector<double> jacVec( n, 0.0);
          vector<double> tempVec( n, 0.0);
          vector<double> covar( n * n, 0.0);

          /*
           * no direct contribution from function or from MathML - call
           * recursively up independent variable tree
           */
          for ( size_t i = 0; i < n; ++i) {
            size_t indxi = independentVarRef_[ i ];
            jacVec[ i ] = getJacobianComponent( indxi);
            covar[ i + n * i ] = janus_->variableDef_[ indxi ].getVariance();
          }

          for ( size_t i = 0; i < n; ++i) {
            for ( size_t j = i + 1; j < n; ++j) {
              if ( j != i) {                      // variances already set up
                size_t i1 = independentVarRef_[ i ];
                size_t j1 = independentVarRef_[ j ];
                size_t ij = j + n * i;
                size_t ji = i + n * j;
                // covariances
                double corrCoef = janus_->variableDef_[ i1 ].getCorrelationCoefficient( j1);
                covar[ ij ] = corrCoef *
                    sqrt( covar[ i + n * i ] * covar[ j + n * j ]);
                covar[ ji ] = covar[ ij ];
              }
            }
          }

          for ( size_t i = 0; i < n; ++i) {
            tempVec[ i ] = 0.0;
            for ( size_t j = 0; j < n; ++j) {
              size_t ij = j + n * i;
              tempVec[ i ] = tempVec[ i ] + covar[ ij ] * jacVec[ j ];
            }
          }

          variance = 0.0;
          for ( size_t i = 0; i < n; ++i) {
            variance = variance + jacVec[ i ] * tempVec[ i ];
          }
        }
        break;

      case METHOD_ARRAY:
        // @TODO What to do here
        break;

      default:
        throw_message( runtime_error,
          setFunctionName( functionName)
          << "\n - Impossible variance - see VariableDef.cpp."
        );
        break;
    }
  }
  else {
    effect  = uncertainty_.getEffect();
    sdBound = uncertainty_.getBounds()[0].getBound();
    sigmaFactor = 1.0 / (double)uncertainty_.getNumSigmas();
    appliedAtOutput = true;
  }

  /*
   *
   */
  if ( appliedAtOutput) {
    switch( effect) {
    case ADDITIVE_UNCERTAINTY:
      stdDev = sdBound * sigmaFactor;
      break;
    case MULTIPLICATIVE_UNCERTAINTY:
      stdDev = sdBound * getValue() * sigmaFactor;
      break;
    case PERCENTAGE_UNCERTAINTY:
      stdDev = sdBound / 100.0 * getValue() * sigmaFactor;
      break;
    case ABSOLUTE_UNCERTAINTY:
      stdDev = fabs( getValue() - sdBound) * sigmaFactor;
      break;
    default:
      stdDev = nan();
      break;
    }
    variance = stdDev * stdDev;
  }

  return variance;
}

//------------------------------------------------------------------------//

const double& VariableDef::getAdditiveBounds(
  const bool& isUpper)
{
  /*
   * Check whether the bounds for the variableDef are current, or
   * the uncertainty type is a uniform pdf.
   */
  if ( !isCurrentBound_) {
    const Uncertainty::UncertaintyPdf& pdf = uncertainty_.getPdf();
    switch ( pdf) {
      case Uncertainty::NORMAL_PDF:
      case Uncertainty::ERROR_PDF:
        // wrong PDF type
        lowerBound_ = nan();
        upperBound_ = nan();
        break;

      case Uncertainty::UNKNOWN_PDF:
        // no uncertainty specified
        lowerBound_ = 0.0;
        upperBound_ = 0.0;
        isCurrentBound_ = true;
        break;

      case Uncertainty::UNIFORM_PDF:
        /*
         * Compute the additive bound of the Uniform PDF for the variableDef.
         */
        computeAdditiveBoundsForUniformPdf();
        isCurrentBound_ = true;
        break;

      default:
        break;
    }
  }

  if ( true == isUpper) {
    return upperBound_;
  }
  return lowerBound_;
}

//------------------------------------------------------------------------//

void VariableDef::computeAdditiveBoundsForUniformPdf()
{
  static const aString functionName( "VariableDef::computeAdditiveBoundsForUniformPdf()");

  /*
   * Compute the additive bounds of the Uniform Pdf for the variableDef.
   */
  bool   appliedAtOutput = false;
  size_t nBound;
  double luBound[ 2 ] = { 0.0, 0.0};
  UncertaintyEffect effect = UNKNOWN_UNCERTAINTY;

  /*
   * check if applied directly to variable at output
   */
  if ( !uncertainty_.isSet()) {
    switch ( variableMethod_) {
      case METHOD_FUNCTION:
      {
        /*
         * first check for contribution from the function table itself
         */
        const Function& thisFunction = janus_->getFunction( functionRef_);
        aOptionalSizeT tableRef = thisFunction.getTableRef();

        if ( tableRef.isValid()) {
          Uncertainty& functionUncertainty =
              janus_->griddedTableDef_[ tableRef ].getUncertainty();
          if ( ELEMENT_UNGRIDDEDTABLE == thisFunction.getTableType()) {
            functionUncertainty = janus_->ungriddedTableDef_[ tableRef ].getUncertainty();
          }

          if ( functionUncertainty.isSet()) {
            const vector<Bounds>& bounds = functionUncertainty.getBounds();
            nBound = bounds.size();
            effect = functionUncertainty.getEffect();
            for ( size_t i = 0; i < nBound; ++i) { // 1 or 2 bounds
              luBound[ i ] = bounds[ i ].getBound();
            }
            appliedAtOutput = true;
          }
        }
      }
      /* FALLTHRU */

      case METHOD_MATHML:
        if ( !appliedAtOutput) {
          size_t n = independentVarRef_.size();

          double savedOutput = getValue();
          double lowerBound = 0.0;
          double upperBound = 0.0;
          double output;

          /*
           * no direct contribution from function or from MathML - call
           * recursively up independent variable tree - for n input variables,
           * needs 2^n function evaluations
           */
          unsigned long int nEvals = 2 << ( n - 1);
          vector<double> savedInput(n, 0.0);
          vector<double> inputBound(2*n, 0.0);
          vector<int> ulbit(n, 0);
          vector<size_t> ivar(n, 0);
          size_t ibits;

          for ( size_t i = 0; i < n; ++i) {
            size_t k = independentVarRef_[i];
            ivar[i] = k;
            janus_->variableDef_[k].getAdditiveBounds( true); // set both bounds
            savedInput[i] = janus_->variableDef_[k].getValue();
            inputBound[ 2 * i ] = janus_->variableDef_[k].getValue() +
                janus_->variableDef_[k].upperBound_;
            inputBound[ 2 * i + 1 ] = janus_->variableDef_[k].getValue() +
                janus_->variableDef_[k].lowerBound_;
          }

          for ( size_t i = 0; i < nEvals; ++i) {
            ibits = i;
            for ( int j = static_cast<int>( n) - 1; j >= 0; j--) {
              ulbit[j] = ( ibits & 1);
              ibits = ibits >> 1;
            }

            for ( size_t j = 0; j < n; ++j) {
              if ( !janus_->variableDef_[ ivar[j]].isMatrix()) { // @TODO: Temporary till vector/matrix support worked out
                janus_->variableDef_[ ivar[j]].setValueForUncertainty( inputBound[ 2 * j + ulbit[j]]);
              }
            }

            output = getValue() - savedOutput;
            lowerBound = std::min( lowerBound, output);
            upperBound = std::max( upperBound, output);
          }

          for ( size_t i = 0; i < n; ++i) {
            if ( !janus_->variableDef_[ ivar[i]].isMatrix()) { // @TODO: Temporary till vector/matrix support worked out
              janus_->variableDef_[ ivar[i]].setValueForUncertainty( savedInput[i]);
            }
          }

          getValue();
          lowerBound_ = lowerBound;
          upperBound_ = upperBound;
        }
        break;

      case METHOD_ARRAY:
        // @TODO What to do here
        break;

      default:
        throw_message( runtime_error,
          setFunctionName( functionName)
          << "\n - Impossible bounds - see VariableDef.cpp."
        );
        break;
    }
  }
  else {
    const vector<Bounds>& bounds = uncertainty_.getBounds();
    nBound = bounds.size();
    effect = uncertainty_.getEffect();
    for ( size_t i = 0; i < nBound; ++i) { // 1 or 2 bounds
      luBound[ i ] = bounds[i].getBound();
    }
    appliedAtOutput = true;
  }

  if ( appliedAtOutput) {
    double value = getValue();
    /*
     * compute ADDITIVE uncertainties, -ve for lower bound and +ve for
     * upper bound
     */
    lowerBound_ = - luBound[ 0 ];
    if ( 1 == nBound) {
      upperBound_ = luBound[ 0 ];
    }
    else {
      upperBound_ = luBound[ 1 ];
    }

    switch( effect) {
      case ADDITIVE_UNCERTAINTY:
        break;

      case MULTIPLICATIVE_UNCERTAINTY:
        lowerBound_ *= value;
        upperBound_ *= value;
        break;

      case PERCENTAGE_UNCERTAINTY:
        lowerBound_ *= ( value * 0.01);
        upperBound_ *= ( value * 0.01);
        break;

      case ABSOLUTE_UNCERTAINTY: // must have 2 values
        lowerBound_ = -lowerBound_ - value;
        upperBound_ -= value;
        break;

      default:
        lowerBound_ = nan();
        upperBound_ = nan();
        break;
    }
  }
}

//------------------------------------------------------------------------//

double VariableDef::getJacobianComponent(
  const aOptionalSizeT& indxi)
{
  static const aString functionName( "VariableDef::getJacobianComponent()");

  if ( !janus_->variableDef_[ indxi ].isMatrix())
  { // Temporary check to avoid computing if variable is a vector or matrix
    double savedInput = janus_->variableDef_[ indxi ].value_;
    janus_->variableDef_[ indxi].setValueForUncertainty( savedInput - 1000.0 * EPS);
    double low = getValue();

    janus_->variableDef_[ indxi].setValueForUncertainty( savedInput + 1000.0 * EPS);
    double high = getValue();
    double result = ( high - low) / ( 2000.0 * EPS);

    janus_->variableDef_[ indxi].setValueForUncertainty( savedInput);
    getValue();  // reset any internal variables

    return result;
  }
  return 0.0;
}


//------------------------------------------------------------------------//

double VariableDef::getCorrelationCoefficient(
  const size_t& indx2)
{
  /*
   * correlation coefficient may be associated with either or both of the
   * variables - need to check both
   */
  double result = 0.0;
  const Uncertainty& uncertainty1 = uncertainty_;
  const Uncertainty& uncertainty2 = janus_->variableDef_[ indx2 ].getUncertainty();

  const vector<correlationPair>& correlationPair1 = uncertainty1.getCorrelation();
  const vector<correlationPair>& correlationPair2 = uncertainty2.getCorrelation();

  size_t correlationPair1size = correlationPair1.size();

  for ( size_t i = 0; i < correlationPair1size; ++i) {
    if ( indx2 == correlationPair1[i].first) {
      result = correlationPair1[i].second;

      if ( 0.0 == result) {
        size_t correlationPair2size = correlationPair2.size();

        for ( size_t j = 0; j < correlationPair2size; ++j) {
          if ((size_t)varIndex_ == correlationPair1[j].first) {
            result = correlationPair2[j].second;
            return result;
          }
        }
      }
      return result;
    }
  }
  return result; // should only reach here if there is no correlation
}

//------------------------------------------------------------------------//
// @TODO :: Temporary internal function used by uncertainty calculations only
void VariableDef::setValueForUncertainty( const double &x)
{
  static const aString functionName( "VariableDef::setValueForUncertainty()");

  value_  = x;
  if (isMatrix_) {
    matrix_ = x;
  }
  else {
    isMatrix_ = false;
  }
  isCurrent_ = isCurrentable_;

  for ( size_t j = 0; j < descendantsRef_.size(); ++j){
    janus_->getVariableDef( descendantsRef_[ j ]).setNotCurrent();
  }
}

//------------------------------------------------------------------------//

void VariableDef::setValue( const double &x, bool isForced)
{
  static const aString functionName( "VariableDef::setValue( double)");

  if ( !inputWarningDone_ && variableType_ != TYPE_INPUT && !isForced) {
    inputWarningDone_ = true;
    warning_message(
      setFunctionName( functionName) << verbose
      << "\n - In DML file \"" << janus_->getXmlFileName() << "\""
      << "\n - Attempting to set internal or output variable \"" << varID_ << "\"."
      << "\n - Consider using the <isInput/> element for this variable."
    );
  }

  math_range_check(
    isForced_ = isForced;
    if ( !isForced_ && isMatrix_) {
      throw_message( range_error,
        setFunctionName( functionName)
        << "\n - Attempt to set a vector/matrix to a single value with variable \""
        << varID_
        << "\"."
      );
    }
  );

  value_ = x;
  if ( !minValueString_.empty() && value_ < minValue_) value_ = minValue_;
  if ( !maxValueString_.empty() && value_ > maxValue_) value_ = maxValue_;
  isMatrix_  = false;
  isCurrent_ = isCurrentable_;

  if ( hasPerturbation_) {
    applyPerturbation();
  }

  for ( size_t j = 0; j < descendantsRef_.size(); ++j){
    janus_->getVariableDef( descendantsRef_[ j ]).setNotCurrent();
    math_range_check( janus_->getVariableDef( descendantsRef_[ j ]).setForced( isForced_));
  }
}

//------------------------------------------------------------------------//

void VariableDef::setValue( const dstomath::DVector &x, bool isForced)
{
  static const aString functionName( "VariableDef::setValue( vector)");

  if ( !inputWarningDone_ && variableType_ != TYPE_INPUT && !isForced) {
    inputWarningDone_ = true;
    warning_message(
      setFunctionName( functionName) << verbose
      << "\n - In DML file \"" << janus_->getXmlFileName() << "\""
      << "\n - Attempting to set internal or output vector variable \"" << varID_ << "\"."
      << "\n - Consider using the <isInput/> element for this variable."
    );
  }

  math_range_check(
    isForced_ = isForced;
    if ( x.size() != matrix_.size() || !matrix_.isVector()) { // Size must match and one of rows or cols must be 1, but not both.
      if ( isForced_) {
        matrix_.resize( x.size(), 1);
      }
      else {
        throw_message( range_error,
          setFunctionName( functionName)
          << "\n - Input vector dimensions are not compatible with variable \""
          << varID_
          << "\"."
        );
      }
    }
  );
  else_no_math_range_check(
    matrix_.resize( x.size(), 1);
  );

  matrix_    = x;
  isMatrix_  = true;
  isCurrent_ = isCurrentable_;

  for ( size_t j = 0; j < descendantsRef_.size(); ++j){
    janus_->getVariableDef( descendantsRef_[ j ]).setNotCurrent();
    math_range_check( janus_->getVariableDef( descendantsRef_[ j ]).setForced( isForced_));
  }

  return;
}

//------------------------------------------------------------------------//

void VariableDef::setValue( const dstomath::DMatrix &x, bool isForced)
{
  static const aString functionName( "VariableDef::setValue( matrix)");

  if ( !inputWarningDone_ && variableType_ != TYPE_INPUT && !isForced) {
    inputWarningDone_ = true;
    warning_message(
      setFunctionName( functionName) << verbose
      << "\n - In DML file \"" << janus_->getXmlFileName() << "\""
      << "\n - Attempting to set internal or output matrix variable \"" << varID_ << "\"."
      << "\n - Consider using the <isInput/> element for this variable."
    );
  }

  math_range_check(
    isForced_ = isForced;
    if ( !isForced_ && !x.isSameDimension( matrix_)) {
      throw_message( range_error,
        setFunctionName( functionName)
        << "\n - Input matrix dimensions are not compatible with variable \"" << varID_ << "\""
        << "\n - given " << x.rows() << "x" << x.cols()
        << " expected " << matrix_.rows() << "x" << matrix_.cols() << "."
      );
    }
  );

  matrix_    = x;
  isMatrix_  = true;
  isCurrent_ = isCurrentable_;

  for ( size_t j = 0; j < descendantsRef_.size(); ++j){
    janus_->getVariableDef( descendantsRef_[ j ]).setNotCurrent();
    math_range_check( janus_->getVariableDef( descendantsRef_[ j ]).setForced( isForced_));
  }
}

//------------------------------------------------------------------------//

void VariableDef::setValueSI(
  const double &xSI)
{
  xmlUnits_.setValueSI( xSI);
  setValue( xmlUnits_.value());
}

//------------------------------------------------------------------------//

void VariableDef::setValueMetric(
  const double &xMetric)
{
  xmlUnits_.setValueMetric( xMetric);
  setValue( xmlUnits_.value());
}

//------------------------------------------------------------------------//

void VariableDef::setFunctionRef(
  const aOptionalSizeT& functionRef)
{
  functionRef_ = functionRef;

  /*
   * Append function table independent variables ancestors to the
   * ancestor list.
   */
  size_t functionIndependentVarDefSize =
      janus_->getFunction()[functionRef].getInDependentVarDef().size();

//  vector<size_t> independentVariables( functionIndependentVarDefSize, 0);

  for ( size_t i = 0; i < functionIndependentVarDefSize; ++i) {
    aOptionalSizeT indVarIndex = janus_->getFunction()[functionRef].getInDependentVarDef()[i].getVariableReference();
    if ( indVarIndex > janus_->getVariableDef().size()) {
      throw_message( std::out_of_range, 
          "Independent variable " << janus_->getFunction()[functionRef].getInDependentVarDef()[i].getVarID() << 
          " referenced from function " << janus_->getFunction()[functionRef].getName() << " does not exist.");
    }
    independentVarRef_.push_back( indVarIndex);
//    independentVariables[i) =
//        janus_->getFunction()[ functionRef).getInDependentVarDef()[i).getVariableReference();
  }

  //setAncestorCrossReferences( independentVarRef_);
}

//------------------------------------------------------------------------//

void VariableDef::setMathMLDependencies()
{
  MathMLData::crossReference_ci( mathCalculation_, janus_);
  if ( variableMethod_ == METHOD_MATHML) {
    hasMatrixOps_ = hasMatrixOps( mathCalculation_);
  }
}

//------------------------------------------------------------------------//

const aString& VariableDef::getStringValue()
{
  static const aString functionName( "VariableDef::getStringValue()");
  /*
   * Check validity of table
   */
  if ( !functionRef_.isValid()) {
    throw_message( range_error,
      setFunctionName( functionName)
      << "\n - varID \"" << varID_
      << "\" is not a string table function."
    );
  }

  Function& thisFunction = janus_->getFunction( functionRef_);
  aOptionalSizeT tableRef = thisFunction.getTableRef();

  if (( !tableRef.isValid()) ||
      ( ELEMENT_UNGRIDDEDTABLE == thisFunction.getTableType())) {
    throw_message( range_error,
      setFunctionName( functionName)
      << "\n - varID \"" << varID_
      << "\" is not a string table function."
    );
  }

  if ( janus_->griddedTableDef_[ tableRef].isStringDataTableEmpty()) {
    throw_message( range_error,
      setFunctionName( functionName)
      << "\n - varID \"" << varID_
      << "\" is an empty string table function."
    );
  }

  /*
   * this assumes string table is n-dimensional, and that the input index
   * matches one of the integer breakpoints in each dimension
   */
  double x;
  size_t j, indxi = 0, indxj, indxiv, ibp;
  size_t nIndVar = independentVarRef_.size();
  vector<size_t> nbp( nIndVar);

  for ( size_t i = 0; i < nIndVar; ++i) {
    ibp = janus_->griddedTableDef_[ tableRef].getBreakpointRef()[ i];
    nbp[ i ] = janus_->breakpointDef_[ ibp].getBpVals().size();
  }

  for ( int i = static_cast<int>( nIndVar) - 1; i >= 0; i--) {
    indxiv = thisFunction.getIndependentVarRef( aOptionalSizeT( i));
    x = dstomath::nearbyint( janus_->getVariableDef()[ indxiv].getValue());
    /*
     * find the breakpoint to which this corresponds
     */
    indxj = 0;
    ibp = janus_->griddedTableDef_[ tableRef].getBreakpointRef()[ size_t( i)];
    for ( j = 0; j < nbp[ i ]; j++) {
      if ( x == janus_->getBreakpointDef()[ ibp].getBpVals()[ j]) {
        indxj = j;
        break;
      }
    }

    if (( nIndVar - 1) == size_t( i)) {
      indxi = indxj;
    }
    else {
      indxi = indxi + indxj * nbp[ i + 1  ];
    }
  }

  /*
   * Bound the index: 0 -- size()-1
   */
  indxi = dstomath::bound( indxi, size_t( 0),
                           (janus_->griddedTableDef_[ tableRef].getStringData().size() - 1));

  return janus_->griddedTableDef_[ tableRef].getStringData()[ indxi];
}

//------------------------------------------------------------------------//

void VariableDef::exportDefinition(
  DomFunctions::XmlNode& documentElement)
{
  /*
   * Create a child node in the DOM for the VariableDef element
   */
  DomFunctions::XmlNode childElement =
    DomFunctions::setChild( documentElement, "variableDef");

  /*
   * Add attributes to the VariableDef child
   */
  DomFunctions::setAttribute( childElement, "name",  name_);
  DomFunctions::setAttribute( childElement, "varID", varID_);
  DomFunctions::setAttribute( childElement, "units", units_);

  if ( !axisSystem_.empty()) {
    DomFunctions::setAttribute( childElement, "axisSystem", axisSystem_);
  }

  if ( !sign_.empty()) {
    DomFunctions::setAttribute( childElement, "sign", sign_);
  }

  if ( !alias_.empty()) {
    DomFunctions::setAttribute( childElement, "alias", alias_);
  }

  if ( !symbol_.empty()) {
  DomFunctions::setAttribute( childElement, "symbol", symbol_);
  }

  if ( !initialValueString_.empty()) {
    DomFunctions::setAttribute( childElement, "initialValue", initialValueString_);
  }

  if ( !minValueString_.empty()) {
    DomFunctions::setAttribute( childElement, "minValue",
                                aString("%").arg( minValue_));
  }

  if ( !maxValueString_.empty()) {
    DomFunctions::setAttribute( childElement, "maxValue",
                                aString("%").arg( maxValue_));
  }

  /*
   * Add description element
   */
  if ( !description_.empty()) {
    DomFunctions::setChild( childElement, "description", description_);
  }

  /*
   * Add the optional provenance entry to the VariableDef child
   */
  if ( hasProvenance_) {
    provenance_.exportDefinition( childElement, isProvenanceRef_);
  }

  /*
   * Add the vector/matrix dimensions to the VariableDef child
   */
  if ( dimensionDef_.getDimCount() > 0) {
    dimensionDef_.exportDefinition( childElement, isDimensionRef_);
  }

  /*
   * Add the calculation element if either MathML or a script is used
   * for this VariableDef child. This includes MathML scripts that have
   * been converted to ExprTk scripts.
   */
  if ( mathCalculation_.mathChildren_.size() > 0 ||
      !script_.empty()) {
    DomFunctions::XmlNode calculationElement =
      DomFunctions::setChild( childElement, "calculation");

    if ( !script_.empty()) {
      exportScript( calculationElement);
    }
    else {
      exportMath( calculationElement);
    }
  }

  /*
   * Add the array element if the VariableDef child represents a non-computed
   * vector or matrix
   */
  if ( array_.getArraySize() > 0) {
    array_.exportDefinition( childElement);
  }

  /*
   * Add the model element if the VariableDef child represents a dynamic system
   * model
   */
  if ( !model_.getModelID().empty()) {
    model_.exportDefinition( childElement);
  }

  /*
   * Add the type tag elements to the VariableDef child
   */
  if ( isInput_) {
    DomFunctions::setChild( childElement, "isInput");
  }
  if ( isControl_) {
    DomFunctions::setChild( childElement, "isControl");
  }
  if ( isDisturbance_) {
    DomFunctions::setChild( childElement, "isDisturbance");
  }

  if ( isState_) {
    DomFunctions::setChild( childElement, "isState");
  }
  if ( isStateDeriv_) {
    DomFunctions::setChild( childElement, "isStateDeriv");
  }
  if ( isOutput_) {
    DomFunctions::setChild( childElement, "isOutput");
  }
  if ( isStdAIAA_) {
    DomFunctions::setChild( childElement, "isStdAIAA");
  }

  /*
   * Add the optional uncertainty entry to the VariableDef child
   * only if it was defined when initially parsing a file or it was
   * explicitly set
   */
  if ( hasUncertainty_ && exportUncertainty_) {
    uncertainty_.exportDefinition( childElement);
  }

//  if ( script_.empty() && mathCalculation_.mathChildren_.size() == 0
//      && hasUncertainty_) {
//    uncertainty_.exportDefinition( childElement);
//  }
}

//------------------------------------------------------------------------//

void VariableDef::readDefinitionFromDom(
  const DomFunctions::XmlNode& xmlElement)
{
  static const aString functionName( "VariableDef::readDefinitionFromDom()");

  try {
    switch ( elementType_) {
      case ELEMENT_ARRAY:
        initialiseArray( xmlElement);
        break;
		
      case ELEMENT_CALCULATION:
        initialiseCalculation( xmlElement);
        break;

      case ELEMENT_SCRIPT:
        initialiseScript( xmlElement);
        break;

      case ELEMENT_DIMENSION:
        dimensionDef_.initialiseDefinition( xmlElement);
        hasDimensionDef_ = true;
        break;
		
      case ELEMENT_MATH:
        initialiseMath( xmlElement);

        /*
         * Try converting MathMl to an equivalent script representation.
         * This is done for simple MathML expressions to increase the
         * speed of runtime execution.
         */
        if ( janus_->getMathML_to_ExprTk()) {
          try {
            script_ = mathML_to_ExprTkScript( xmlElement, varID_);
#ifdef _DEBUG_
            cout << "---------------------------------------" << endl;
            cout << "VARID = " << varID_ << " =\n" << endl;
            cout << script_ << endl << endl;
#endif
          }
#ifndef _DEBUG_
          catch( exception& ) {
            script_.clear();
#else
          catch( exception &excep) {
            script_.clear();
            cout << "VARID: MathML script conversion to ExprTk failed...\n"
                 << excep.what() << endl;
#endif
          }
          catch(...) {
            script_.clear();
#ifdef _DEBUG_
            cout << "VARID: MathML script conversion to ExprTk failed." << endl;
#endif
          }
        }
        break;
		
      case ELEMENT_MODEL:
        initialiseModel( xmlElement);
      break;

      case ELEMENT_PROVENANCE:
        provenance_.initialiseDefinition( xmlElement);
        hasProvenance_ = true;
        break;

      case ELEMENT_PERTURBATION:
        initialisePerturbation( xmlElement);
        break;
		
      default:
        break;
	  }
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - "
      << excep.what()
    );
  }
}

//------------------------------------------------------------------------//

bool VariableDef::compareElementID(
  const DomFunctions::XmlNode& xmlElement,
  const aString& elementID,
	const size_t& /*documentElementReferenceIndex*/)
{
  switch ( elementType_) {
    case ELEMENT_PROVENANCE:
      if ( DomFunctions::getAttribute( xmlElement, "provID") != elementID) {
        return false;
      }
      isProvenanceRef_ = true;
      break;

    case ELEMENT_DIMENSION:
      if ( DomFunctions::getAttribute( xmlElement, "dimID") != elementID) {
        return false;
      }
      isDimensionRef_ = true;
      break;

    default:
      return false;
      break;
  }

  readDefinitionFromDom( xmlElement);

  return true;
}

//------------------------------------------------------------------------//

void VariableDef::resetJanus( Janus *janus)
{
  janus_ = janus;

  /*
   * Reset the Janus pointer in the Uncertainty class
   */
  uncertainty_.resetJanus( janus);

  /*
   * Reset the Janus pointer in the MathMLDataClass class
   */
  MathMLData::crossReference_ci( mathCalculation_, janus);

  /*
   * Reset the Kaguya/Lua, ExprTk script function pointer.
   */
  deleteLuaFunction();
  deleteExprTkFunction();
}

//------------------------------------------------------------------------//

void VariableDef::initialiseBaseVariableDef(
  const DomFunctions::XmlNode& variableDefElement)
{
  static const aString functionName( "VariableDef::initialiseBaseVariableDef()");

  /*
   * Retrieve attributes for the Variable Definition
   */
  name_   = DomFunctions::getAttribute( variableDefElement, "name");
  varID_  = DomFunctions::getAttribute( variableDefElement, "varID");
  units_  = DomFunctions::getAttribute( variableDefElement, "units");
  sign_   = DomFunctions::getAttribute( variableDefElement, "sign");
  alias_  = DomFunctions::getAttribute( variableDefElement, "alias");
  symbol_ = DomFunctions::getAttribute( variableDefElement, "symbol");
  axisSystem_ = DomFunctions::getAttribute( variableDefElement, "axisSystem");

  /*
   * Retrieve the description associated with the variable
   */
  description_ = DomFunctions::getChildValue( variableDefElement, "description");

  /*
   * Provenance definition
   */
  elementType_ = ELEMENT_PROVENANCE;
  try {
    DomFunctions::initialiseChildOrRef( 
      this,
      variableDefElement,
      varID_,
      "provenance",
      "provenanceRef",
      "provID",
      false);
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - "
      << excep.what()
    );
  }
  
  /*
   * Variable Type Definitions
   */
  isInput_ = DomFunctions::isChildInNode( variableDefElement, "isInput");
  if ( isInput_) {
    variableType_ = TYPE_INPUT;
  }
  
  if ( !isInput_) {
    isControl_ = DomFunctions::isChildInNode( variableDefElement, "isControl");
    if ( isControl_) {
      variableType_ = TYPE_INPUT;
    }
  }
  
  if ( !isInput_ && !isControl_) {
    isDisturbance_ = DomFunctions::isChildInNode( variableDefElement, "isDisturbance");
    if ( isDisturbance_) {
      variableType_ = TYPE_INPUT;
    }
  }
  
  isOutput_ = DomFunctions::isChildInNode( variableDefElement, "isOutput");
  if ( isOutput_) {
    variableType_ = TYPE_OUTPUT;
    /*
     * this element has not yet been appended to the variableDef_ vector,
     * so it will go in position "size()"
     */
    janus_->outputIndex_.push_back( janus_->variableDef_.size());
  }
  
  /*
   * Variable Flag Definitions
   */
  isState_      = DomFunctions::isChildInNode( variableDefElement, "isState");
  isStateDeriv_ = DomFunctions::isChildInNode( variableDefElement, "isStateDeriv");
  isStdAIAA_    = DomFunctions::isChildInNode( variableDefElement, "isStdAIAA");

  /*
   * Is this a perturbation
   */
  elementType_ = ELEMENT_PERTURBATION;
  try {
    DomFunctions::initialiseChild(
      this,
      variableDefElement,
      varID_,
      "isPerturbation",
      false);
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - "
      << excep.what()
    );
  }

  /*
   * Uncertainty is processed in Janus, since it requires cross-referencing.
   * Just set the uncertainty flag if it is present for this variableDef.
   */
  hasUncertainty_ = DomFunctions::isChildInNode( variableDefElement, "uncertainty");
  exportUncertainty_ = hasUncertainty_;

  /*
   * Retrieve Calculation definition
   */
  elementType_ = ELEMENT_CALCULATION;
  try {
    DomFunctions::initialiseChild(
      this,
      variableDefElement,
      varID_,
      "calculation",
      false);
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - "
      << excep.what()
    );
  }

  return;
}

//------------------------------------------------------------------------//

void VariableDef::initialiseCalculation(
  const DomFunctions::XmlNode& xmlElement)
{
  static const aString functionName( "VariableDef::initialiseCalculation()");

  isCurrent_ = false;

  /*
   * Check the entries for the calculation element to check that it has
   * valid content. If neither of a "math" or "script" sub-element have
   * been specified then provide a error message and exit the function
   */
  bool isMathAvailable = DomFunctions::isChildInNode( xmlElement, "math");
  bool isScriptAvailable = DomFunctions::isChildInNode( xmlElement, "script");

  if ( !( isMathAvailable ^ isScriptAvailable)) {
    throw_message( range_error,
      setFunctionName( functionName)
      << "\n - varID \"" << varID_
      << "\" has neither or both the \"math\" and \"script\" elements specified."
    );
  }

  /*
   * Parse either the "math" or "script" element
   */
  
  if ( isMathAvailable) {
    /*
     * Get the highest level <apply> child node within the <math>
     * child node.  NB this assumes the child element is tagged as
     * <mathml2:math>.  Note there can be multiple <apply> nodes
     * below <math>, but only the highest is the pointer to the
     * complete function.  If there is no calculation involved, but just
     * straight equality to another variable value or to a constant, this
     * may also be <ci> or <cn>.  There are also numerous other top-level
     * components allowed by the MathML spec.  These may be added as
     * required.  Math tags may include alternative namespace form
     */

    variableMethod_ = METHOD_MATHML;
    elementType_    = ELEMENT_MATH;
    if ( DomFunctions::isChildInNode( xmlElement, "math")) {
      try {
        DomFunctions::initialiseChild(
          this,
          xmlElement,
          varID_,
          "math",
          false);
      }
      catch ( exception &excep) {
        throw_message( invalid_argument,
          setFunctionName( functionName)
          << "\n - "
          << excep.what()
        );
      }
    }
    else {
      try {
        DomFunctions::initialiseChild(
          this,
          xmlElement,
          varID_,
          "mathml2:math",
          false);
      }
      catch ( exception &excep) {
        throw_message( invalid_argument,
          setFunctionName( functionName)
          << "\n - "
          << excep.what()
        );
      }
    }
  }
  else if ( isScriptAvailable) {
    /*
     * Retrieve Script definition
     */
    variableMethod_ = METHOD_SCRIPT;
    elementType_    = ELEMENT_SCRIPT;
    try {
      DomFunctions::initialiseChild(
        this,
        xmlElement,
        varID_,
        "script",
        false);
    }
    catch ( exception &excep) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - "
        << excep.what()
      );
    }
  }
}

//------------------------------------------------------------------------//

void VariableDef::initialiseScript(
  const DomFunctions::XmlNode& xmlElement)
{
  static const aString functionName( "VariableDef::initialiseScript()");
  script_            = DomFunctions::getCData( xmlElement);
  aString scriptType = DomFunctions::getAttribute( xmlElement, "type", true);

  if ( scriptType.toLowerCase() == "exprtk") {
    scriptType_ = EXPRTK_SCRIPT;
    initialiseExprTkScript();
  }
  else if ( scriptType.toLowerCase() == "lua") {
    scriptType_ = LUA_SCRIPT;
    initialiseLuaScript();
  }
  else {
    aString scriptTypes = "ExprTk";
    aString elseNote;
#ifdef HAVE_KAGUYA
    scriptTypes += " and Lua";
#else
    elseNote = "\nNote: Lua scripts have not been compiled into this version of Janus.";
#endif
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - varID \"" << varID_
      << "\" - Only " << scriptTypes << " scripts currently supported."
      << elseNote
    );
  }

  isCurrent_      = false;
  variableMethod_ = METHOD_SCRIPT;
}

//------------------------------------------------------------------------//

void VariableDef::initialiseArray( const DomFunctions::XmlNode& xmlElement)
{
  static const aString functionName( "VariableDef::initialiseArray()");

  isCurrent_      = false;
  variableMethod_ = METHOD_ARRAY;

  try {
    array_.initialiseDefinition( xmlElement);
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - "
      << excep.what()
    );
  }

  /*
   * Once the array data has been read need to check the
   * dimension size against number of points.
   * If not consistent then trigger an error command.
   * If consistent need to convert the data table to
   * numeric representations, set up ancestry and
   * descendant linkages.
   */
  instantiateDataTable();
}

//------------------------------------------------------------------------//

void VariableDef::initialiseModel( const DomFunctions::XmlNode& xmlElement)
{
  static const aString functionName( "VariableDef::initialiseModel()");

  isCurrent_      = false;
  variableMethod_ = METHOD_MODEL;

  try {
    model_.initialiseDefinition( xmlElement);
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - "
      << excep.what()
    );
  }
}

//------------------------------------------------------------------------//

void VariableDef::initialiseMath( const DomFunctions::XmlNode& xmlElement)
{
  static const aString functionName( "VariableDef::initialiseMath()");

 /*
  * NOTE: attributes of the <math> element are ignored as they are primarily
  * for presentation of the MathML content, and therefore, do not contribute
  * to defining how a mathematical expression should be evaluated.
  */

  DomFunctions::XmlNodeList childList;
  
  /*
   * Retrieve a list of the children for the 'math' element
   */
  try {
    childList = DomFunctions::getChildren( 
      xmlElement,
      EMPTY_STRING,
      varID_);
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - for ID \"" << varID_ << "\"\n - "
      << excep.what()
    );
  }
  
  /*
   * find one and only one of the allowable children of this element
   */
  size_t childListLength = childList.size();
  size_t numberOfValidChildren = 0;
  size_t offset = 0;
  aString childName;
  bool isApply = true;
  
  for ( size_t i = 0; i < childListLength; ++i) {
    childName = DomFunctions::getChildName( childList[ i]).trim();
  
    if ( childName == "apply") {
      isApply = true;
      offset  = i;
      numberOfValidChildren++;
     }
    else if (( childName == "ci") ||
              ( childName == "cn") ||
              ( childName == "piecewise") ||
              ( childName == "pi") ||
              ( childName == "exponentiale") ||
              ( childName == "notanumber") ||
              ( childName == "eulergamma") ||
              ( childName == "infinity")) {
      isApply = false;
      offset  = i;
      numberOfValidChildren++;
    }
    else if ( childName.size()) {
      throw_message( range_error,
        setFunctionName( functionName)
        << "\n - for ID \"" << varID_
        << "\", support for tag \""
        << childName << "\" is not provided."
      );
    }

    if ( numberOfValidChildren > 1) {
      throw_message( range_error,
        setFunctionName( functionName)
        << "\n - ID \"" << varID_
        << "\" has more than 1 valid top level tag in <math> definition."
      );
    }
  }

  if ( 0 == numberOfValidChildren) {
    throw_message( range_error,
      setFunctionName( functionName)
      << "\n - ID \"" << varID_ << "\" has no valid top level tags in <math> definition."
    );
  }
  
  /*
   * set up the MathML equation and cross-references for this variable,
   * only looking for cross-references to previously-defined variables
   */
  try {
    dstomathml::parsemathml::parse( childList[ offset], mathCalculation_);
  }
  catch ( exception &excep) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - ID \"" << varID_ << "\"\n - "
      << excep.what()
    );
  }

  /*
   * set up the cross-references for this variable. Only looking for
   * cross-references to previously-defined variables
   */
  DomFunctions::XmlNodeList ciList;
  if ( isApply) {
    DomFunctions::getNodesByName( childList[ offset], "ci", ciList);
  }
  else {
    DomFunctions::getNodesByName( xmlElement, "ci", ciList);
  }

  size_t ciListSize = ciList.size();

  for ( size_t j = 0; j < ciListSize; ++j) {
	  aString ciVarId = DomFunctions::getCData( ciList[j]).trim();
	  aOptionalSizeT ciVarIndex = janus_->crossReferenceId( ELEMENT_VARIABLE, ciVarId);
	  if ( ciVarIndex.isValid()) {
	    independentVarRef_.push_back( ciVarIndex);
  	}
	  else {
	    throw_message( range_error,
        setFunctionName( functionName)
        << "\n - ID \"" << ciVarId
        << "\" is not in VariableDef list."
      );
	  }
  }
}

//------------------------------------------------------------------------//

void VariableDef::solveMath() const
{
  if ( !hasMatrixOps_) {
    value_ = mathCalculation_.mathMLFunctionPtr_( mathCalculation_);
  }
  else {
    mathCalculation_.mathMLMatrixFunctionPtr_( mathCalculation_);

    math_range_check(
      if ( !isForced_) {
        if ( mathCalculation_.isMatrix_ != isMatrix_) {
          if ( isMatrix_) {
            throw_message( range_error,
              setFunctionName( "VariableDef::solveMath()")
              << "\n - ID \"" << varID_
              << "\" expected a matrix not a single value."
            );
          }
          else {
            throw_message( range_error,
              setFunctionName( "VariableDef::solveMath()")
              << "\n - ID \"" << varID_
              << "\" expected a single value not a matrix."
            );
          }
        }
        if ( isMatrix_ && !mathCalculation_.matrix_.isSameDimension( matrix_)) {
          throw_message( range_error,
            setFunctionName( "VariableDef::solveMath()")
            << "\n - ID \"" << varID_
            << "\" expected a " << matrix_.rows() << "x" << matrix_.cols()
            << " matrix not a " << mathCalculation_.matrix_.rows() << "x" << mathCalculation_.matrix_.cols()
            << " matrix."
          );
        }
      }
    );

    if ( mathCalculation_.isMatrix_) {
      isMatrix_ = true;
      matrix_ = mathCalculation_.matrix_;
    }
    else {
      isMatrix_ = false;
      value_ = mathCalculation_.value_;
    }
  }
}

//------------------------------------------------------------------------//

bool VariableDef::hasMatrixOps() const
{


  if ( isMatrix_) {
    return true;
  }
  if ( variableMethod_ == METHOD_MATHML) {
    hasMatrixOps_ = hasMatrixOps( mathCalculation_);
    return hasMatrixOps_;
  }

  return false;
}

//------------------------------------------------------------------------//

bool VariableDef::hasMatrixOps( const dstomathml::MathMLData &t) const
{
  if ( t.variableDef_) {
    if ( t.variableDef_->isMatrix_) { // Variable is a matrix.
      return true;
    }
    if ( t.variableDef_->hasMatrixOps()) { // See if this variable has dependent matrix ops.
      return true;
    }
  }
  else if ( !t.mathMLFunctionPtr_ && t.mathMLMatrixFunctionPtr_) {
    return true; // Function not found in single double value table. Use matrix table instead.
  }

  for ( MathMLDataVector::const_iterator child = t.mathChildren_.begin();
        child != t.mathChildren_.end(); ++child) {
    if ( hasMatrixOps( *child)) {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------//

void VariableDef::exportScript( DomFunctions::XmlNode& documentElement)
{
  /*
   * Create a child node in the DOM for the math element
   *
   * NOTE: attributes of the <math> element are ignored as they are primarily
   * for presentation of the MathML content, and therefore, do not contribute
   * to defining how a mathematical expression should be evaluated.
   */
  if ( mathCalculation_.mathChildren_.size() > 0) {
    DomFunctions::setComment( documentElement, " MathML script converted to ExprTk script ");
  }
  DomFunctions::XmlNode childElement = DomFunctions::setChildCData( documentElement, "script", script_);
  DomFunctions::setAttribute( childElement, "type", ( scriptType_ == LUA_SCRIPT) ? "lua" : "exprtk");
}

//------------------------------------------------------------------------//

void VariableDef::exportMath( DomFunctions::XmlNode& documentElement)
{
  /*
   * Create a child node in the DOM for the math element
   *
   * NOTE: attributes of the <math> element are ignored as they are primarily
   * for presentation of the MathML content, and therefore, do not contribute
   * to defining how a mathematical expression should be evaluated.
   */
  DomFunctions::XmlNode childElement =
    DomFunctions::setChild( documentElement, "math");

  /*
   *  Traverse the mathCalculation_ construct to create the mathML tree
   */
  dstomathml::exportmathml::exportMathMl( childElement, mathCalculation_);

}

//------------------------------------------------------------------------//

void VariableDef::instantiateDataTable()
{
  static const aString functionName( "VariableDef::instantiateDataTable()");

  /*
   * Once the array data has been read need to check the
   * dimension size against number of points.
   * If not consistent then trigger an error command.
   * If consistent need to convert the data table to
   * numeric representations, set up ancestry and
   * descendant linkages.
   */
  size_t nrows = 0;
  size_t ncols = 0;

  if ( dimensionDef_.getDimTotal() == array_.getArraySize()) {
    nrows = matrix_.rows();
    ncols = matrix_.cols();
    matrixVarId_.resize( nrows, ncols, aOptionalSizeT());
    matrixScaleFactor_.resize( nrows, ncols);

    if ( dimensionDef_.getDimCount() > 1) {
      nrows = dimensionDef_.getDim( dimensionDef_.getDimCount() - 2);
    }

    /*
     * Evaluate the data table from the array class and
     * determine the ancestry relationships.
     */
    size_t j = 0;
    size_t k = 0;
    size_t offset = 1;
    size_t nStart = 0;
    aString dataEntry;
    int minusFactor = 1;

    hasVarIdEntries_ = false;

    const aStringList& dataTable = array_.getStringDataTable();
    size_t arrayLength = array_.getArraySize();
    for ( size_t i = 0; i < arrayLength; ++i) {
      minusFactor = 1;
      dataEntry   = dataTable[ i];

      if ( dataEntry.isNumeric()) {
        matrix_(( j * offset), k)      = dataEntry.toDouble();
        matrixVarId_(( j * offset), k) = aOptionalSizeT();
        matrixScaleFactor_(( j * offset), k) = 1.0;
      }
      else {
        // Need to convert this to a numeric value and populate ancestry data.
        // If first character is a '-' need to set the minusFactor to -1.
        nStart = dataEntry.find_first_not_of( "-");
        if ( nStart != 0) {
          minusFactor = -1;
          dataEntry   = dataEntry.substr( nStart);
        }

        // Note: Can't populate the matrix_ element at this point, especially if it relies on MathML
        aOptionalSizeT matrixVarIDIndex = janus_->crossReferenceId( ELEMENT_VARIABLE, dataEntry);
        if ( matrixVarIDIndex.isValid()) {
          matrixVarId_(( j * offset), k) = matrixVarIDIndex;
          matrixScaleFactor_(( j * offset), k) = minusFactor;
          independentVarRef_.push_back( matrixVarId_(( j * offset), k));
          hasVarIdEntries_ = true;
        }
        else {
          throw_message( range_error,
            setFunctionName( functionName)
            << "\n - ID \"" << matrixVarIDIndex << " : " << dataEntry
            << "\" is not in VariableDef list."
          );
        }
      }

      ++k;
      k %= ncols;
      if ( k == 0) {
        ++j;
        j %= nrows;
      }
      if ( j == nrows) {
        j = 0;
        ++offset;
      }
    }

    if ( !hasVarIdEntries_) {
     isCurrent_ = true;
    }
  }
  else {
    throw_message( range_error,
      setFunctionName( functionName)
      << "\n - varID \"" << varID_
      << "\" array size incompatible with defined dimensions."
    );
  }
}

//------------------------------------------------------------------------//

void VariableDef::evaluateDataTable() const
{
  for ( size_t i = 0; i < matrix_.rows(); ++i) {
    for ( size_t j = 0; j < matrix_.cols(); ++j) {
      if ( matrixVarId_( i, j).isValid()) {
        matrix_( i, j) = janus_->getVariableDef( matrixVarId_( i, j)).getValue() *
                         matrixScaleFactor_( i, j);
      }
    }
  }
}

//------------------------------------------------------------------------//

double VariableDef::evaluateDataEntry( const aString &dataEntry)
{
  static const aString functionName( "VariableDef::evaluateDataEntry()");

  VariableDef* variableDef = janus_->findVariableDef( dataEntry);
  if ( variableDef == 0) {
    throw_message( range_error,
      setFunctionName( functionName)
      << "\n - varID \"" << varID_
      << "\" alpha-numeric varID array element not defined before use."
    );
  }
  return variableDef->getValue();
}

//------------------------------------------------------------------------//

void VariableDef::initialisePerturbation( const DomFunctions::XmlNode& xmlElement)
{
  static const aString functionName( "VariableDef::initialisePerturbation()");

  // Get target variableRef
  const aString perturbationTargetVarId = DomFunctions::getAttribute( xmlElement, "variableRef", true);
  perturbationTargetVarIndex_ = janus_->getVariableIndex( perturbationTargetVarId);
  if ( !perturbationTargetVarIndex_.isValid()) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - varID \"" << varID_
      << "\" - Invalid perturbation variableRef \"" << perturbationTargetVarId << "\"."
    );
  }

  // Check effect is valid
  const aString effectString = DomFunctions::getAttribute( xmlElement, "effect", true);
  aOptionalInt effect        = Uncertainty::uncertaintyAttributesMap.get( effectString);
  if ( effect.isValid() 
    && ( effect.value() == ADDITIVE_UNCERTAINTY 
      || effect.value() == MULTIPLICATIVE_UNCERTAINTY))
  {
    perturbationEffect_ = UncertaintyEffect( effect.value());
  }
  else {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - varID \"" << varID_
      << "\" - Invalid perturbation effect \"" << effectString << "\"."
    );
  }
}

//------------------------------------------------------------------------//

void VariableDef::setPerturbation( UncertaintyEffect uncertaintyEffect, const double& value)
{
  static const aString functionName( "VariableDef::setPerturbation()");

  if ( uncertaintyEffect != MULTIPLICATIVE_UNCERTAINTY && uncertaintyEffect != ADDITIVE_UNCERTAINTY) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - varID \"" << varID_
      << "\" - Invalid perturbation effect set \"" 
      << Uncertainty::uncertaintyAttributesMap.get( uncertaintyEffect, "Unknown Uncertainty") << "\"."
    );
  }

  auto setupPerturbation = [&]( VariableDef& ptb) -> void {
    ptb.variableMethod_     = METHOD_PLAIN_VARIABLE;
    ptb.value_              = value;
    ptb.perturbationEffect_ = uncertaintyEffect;
    ptb.xmlUnits_           = aUnits( ptb.perturbationEffect_ == MULTIPLICATIVE_UNCERTAINTY ? "nd" : xmlUnits_.unitsMetric(), value);
  };

  if ( hasPerturbation_) {
    setupPerturbation( janus_->getVariableDef( associatedPerturbationVarIndex_));
    isCurrent_ = false;
    return;
  }

  const size_t thisIdx = janus_->getVariableIndex( varID_);
  const size_t newIdx  = janus_->variableDef_.size();

  VariableDef newVariableDef;
  setupPerturbation( newVariableDef);

  newVariableDef.resetJanus( janus_);
  newVariableDef.perturbationTargetVarIndex_ = thisIdx;

  vector<size_t> descendants = getDescendantsRef();
  descendants.push_back( thisIdx);
  newVariableDef.setDescendantsRef( descendants);

  ancestorsRef_.push_back( newIdx);

  janus_->variableDef_.push_back( newVariableDef);

  setPerturbationVarIndex( newIdx);
}

//------------------------------------------------------------------------//

void VariableDef::setPerturbationVarIndex( const size_t index)
{
  static const aString functionName( "VariableDef::setPerturbationVarIndex()");

  if ( hasPerturbation_) {
    throw_message( 
      runtime_error, 
      setFunctionName( "Janus::setPerturbationVarIndex( size_t index)")
      << "\n - perturbation is already associated with this variable."
      << "\n - varID: \"" << varID_ << "\""
    );
  }

  const VariableDef& perturbation = janus_->getVariableDef( index);

  // Check unit compatibility.
  switch ( perturbation.perturbationEffect_) {
  case ADDITIVE_UNCERTAINTY:
    if ( !xmlUnits_.isCompatible( perturbation.xmlUnits_, false)) {
      throw_message( invalid_argument,
        setFunctionName( functionName)
        << "\n - varID \"" << perturbation.getVarID()
        << "\" - additive perturbation must have units compatible with the perturbation target."
      );
    }
    break;

  case MULTIPLICATIVE_UNCERTAINTY:
    { 
      const aString units = perturbation.xmlUnits_.units();
      if ( !( units == "ND" || units == "nd" || units == "")) {
        throw_message( invalid_argument,
          setFunctionName( functionName)
          << "\n - varID \"" << perturbation.getVarID()
          << "\" - multiplicative perturbation must be non-dimensional."
        );
      }
    }
    break;
  
  default:
    break;
  }

  // Check axis system compatibility.
  if ( perturbation.getAxisSystem().trim().toLowerCase() != getAxisSystem().trim().toLowerCase()) {
    throw_message( invalid_argument,
      setFunctionName( functionName)
      << "\n - varID \"" << perturbation.getVarID()
      << "\" - the axisSystem does not match that of the perturbation target."
    );
  }

  // TODO: Matrix compatibility check
  if ( isMatrix_) {
    throw_message( runtime_error,
      "\n - matrix perturbations are not yet supported."
    );
  }

  associatedPerturbationVarIndex_ = index;
  hasPerturbation_ = true;
  independentVarRef_.push_back( index);
  isCurrent_ = false;
}

//------------------------------------------------------------------------//

void VariableDef::applyPerturbation() const
{
  const VariableDef& perturbation = janus_->getVariableDef( associatedPerturbationVarIndex_);

  if ( isMatrix_) {
  
    // TODO: Matrix handling
    throw_message( runtime_error,
      "\n - matrix perturbations are not yet supported"
    );

  }
  else {

    switch ( perturbation.perturbationEffect_) {
    case ADDITIVE_UNCERTAINTY:
      { 
        xmlUnits_.setValue( value_);
        const double valueSI = xmlUnits_.valueSI();

        xmlUnits_.setValueSI( valueSI + perturbation.getValueSI());
        value_ = xmlUnits_.value();
      }
      break;

    case MULTIPLICATIVE_UNCERTAINTY:
      value_ *= perturbation.getValue();
      break;

    default:
      break;
    }
  }
}

//------------------------------------------------------------------------//

ostream& operator<< (
  ostream& os,
  const VariableDef& variableDef)
{
  /*
   * General properties of the Class
   */
  os << endl << endl
     << "Display VariableDef contents:" << endl
     << "-----------------------------------" << endl;

  os << "  name               : " << variableDef.getName() << endl
     << "  varID              : " << variableDef.getVarID() << endl
     << "  units              : " << variableDef.getUnits() << endl
     << "  axisSystem         : " << variableDef.getAxisSystem() << endl
     << "  sign               : " << variableDef.getSign() << endl
     << "  alias              : " << variableDef.getAlias() << endl
     << "  symbol             : " << variableDef.getSymbol() << endl
     << "  initialValue       : " << variableDef.getInitialValue() << endl
     << "  maxValue           : " << variableDef.getMinValue() << endl
     << "  minValue           : " << variableDef.getMaxValue() << endl
     << endl;

  os << "  isInput            : " << variableDef.isInput() << endl
     << "  isControl          : " << variableDef.isControl() << endl
     << "  isDisturbance      : " << variableDef.isDisturbance() << endl
     << "  isOutput           : " << variableDef.isOutput() << endl
     << "  isState            : " << variableDef.isState() << endl
     << "  isStateDeriv       : " << variableDef.isStateDeriv() << endl
     << "  isStdAIAA          : " << variableDef.isStdAIAA() << endl
     << endl;

  os << "  description        : " << variableDef.getDescription() << endl
     << "  hasProvenance      : " << variableDef.hasProvenance() << endl
     << "  hasUncertainty     : " << variableDef.hasUncertainty() << endl
     << "  hasDimension       : " << variableDef.hasDimension() << endl
     << endl;

  /*
   * Provenance data for the Class
   */
  if ( variableDef.hasProvenance()) {
    os << variableDef.getProvenance() << endl;
  }

  /*
   * Uncertainty data for the Class
   */
  if ( variableDef.hasUncertainty()) {
    os << variableDef.uncertainty_ << endl;
  }

  /*
   * Dimension data for the Class
   */
  if ( variableDef.hasDimension()) {
    os << variableDef.getDimension() << endl;
  }

  /*
   * function table reference
   */
  aOptionalSizeT functionRef = variableDef.getFunctionRef();
  os << "  Function table Ref : " << functionRef << endl;
  if ( functionRef.isValid()) {
    os << variableDef.janus_->getFunction()[ functionRef] << endl;
  }

  /*
   * Independent variable data
   */
  size_t independentVarCount = variableDef.getIndependentVarCount();
  os << "  # independent vars : " << independentVarCount << endl;
  if ( 0 < independentVarCount) {
    vector<size_t> independentVarRefs = variableDef.getIndependentVarRef();
    for ( size_t i = 0; i < independentVarCount; ++i) {
      os << "  independent var #  : "<< i << " : Reference :"
         << independentVarRefs[ i] << endl;
    }
  }

  /*
   * Ancestor variable data
   */
  size_t ancestorCount = variableDef.getAncestorCount();
  os << "  # ancestor vars    : " << ancestorCount << endl;
  if ( 0 < ancestorCount) {
    vector<size_t> ancestorRefs = variableDef.getAncestorsRef();
    for ( size_t i = 0; i < ancestorCount; ++i) {
      os << "  ancestor variable #: "<< i << " : Reference :"
         << ancestorRefs[ i] << endl;
    }
  }

  /*
   * Descendent variable data
   */
  size_t descendantCount = variableDef.getDescendantsRef().size();
  os << "  # descendant vars    : " << descendantCount << endl;
  if ( 0 < descendantCount) {
    vector<size_t> descendantRefs = variableDef.getDescendantsRef();
    for ( size_t i = 0; i < descendantCount; ++i) {
      os << "  descendant variable #: " << i << " : Reference :"
         << descendantRefs[ i] << endl;
    }
  }

  // @TODO More to add
//  os << "  cnValue      : " << variableDef.mathCalculation_.cnValue_ << endl;
//
//  if ( variableDef.mathCalculation_.mathRetType_ == dstomathml::BOOL) {
//    os << "  mathRetType  : BOOL" << endl;
//  }
//  else {
//    os << "  mathRetType  : REAL" << endl;
//  }
//  os << "  # children   : " << variableDef.mathCalculation_ << endl;

  os << "-----------------" << endl;

  return os;
}

//------------------------------------------------------------------------//

} // namespace janus
