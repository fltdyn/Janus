//
// DST Ute Library (Utilities Library)
//
// Defence Science and Technology (DST) Group
// Department of Defence, Australia.
// 506 Lorimer St
// Fishermans Bend, VIC
// AUSTRALIA, 3207
//
// Copyright 2005-2018 Commonwealth of Australia
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

//
// Title:  aIntegrator Class
// Module: aIntegrator.cpp
//
// Modified by: Shane D. Hill (Shane.Hill@dsto.defence.gov.au)
//
// Created from TIntegrator.cpp:
//
//    GEMM  Generic Missile Model
//    Weapons Systems Division, DSTO, 2000
//
//    General purpose, multivariable time integration object for systems of
//    differential equations. Three integration methods are included:
//    Euler, 2nd order Runge-Kutta, 4th order Runge Kutta
//
// Reference for Differentaition:
// Numerical Methods in Engineering and Science by Graham de Vahl Davis, 1986.
//
// First Date: 30/10/2003
//

// C++ Includes
#include <cfloat>
#include <algorithm>

// Local Includes
#include "aMessageStream.h"
#include "aMath.h"
#include "aIntegrator.h"

using namespace dstoute;
using namespace std;

namespace dstomath {

  namespace {

    // Steps to take before adapting step size
    const double initSteps = 4;

    // Integration and adaptive step size parameters
    // Huen
    const double rk2a[] = { 1.0 };
    const double rk2b[] = { 0.5, 0.5 };
    const double rk2c[] = { 1.0 };
    const size_t rk2n   = 2; // length of b (number of derivative evaluations required)

    // Higher-order only Bogacki Shampine
    const double rk3a[] = {     0.5,     0.0,
                                0.0,    0.75 };
    const double rk3b[] = { 2.0/9.0, 1.0/3.0, 4.0/9.0 };
    const double rk3c[] = {     0.5,    0.75 };
    const size_t rk3n   = 3;

    // Classic 4th Order Runge Kutta
    const double rk4a[] = {     0.5,     0.0,     0.0,
                                0.0,     0.5,     0.0,
                                0.0,     0.0,     1.0 };
    const double rk4b[] = { 1.0/6.0, 1.0/3.0, 1.0/3.0, 1.0/6.0 };
    const double rk4c[] = {     0.5,     0.5,     1.0 };
    const size_t rk4n   = 4;

    // Hammud (corrected by Alshina Zaks Kalitkin)
    const double rk6a[] = {                 4.0/7.0,                                          0.0,                                      0.0,                                      0.0,                                  0.0,                                 0.0, 
                                        115.0/112.0,                                    -5.0/16.0,                                      0.0,                                      0.0,                                  0.0,                                 0.0, 
                                        589.0/630.0,                                     5.0/18.0,                                -16.0/45.0,                                     0.0,                                  0.0,                                 0.0,
       229.0/1200.0 - 29.0/6000.0 * std::sqrt( 5.0), 119.0/240.0 - 187.0/1200.0 * std::sqrt( 5.0), -14.0/75.0 + 34.0/375.0 * std::sqrt( 5.0),            -3.0/100.0 * std::sqrt( 5.0),                                  0.0,                                 0.0,
      71.0/2400.0 - 587.0/12000.0 * std::sqrt( 5.0), 187.0/480.0 - 391.0/2400.0 * std::sqrt( 5.0), -38.0/75.0 + 26.0/375.0 * std::sqrt( 5.0), 27.0/80.0 - 3.0/400.0 * std::sqrt( 5.0),  1.0/4.0 + 1.0/4.0 * std::sqrt( 5.0),                                 0.0,
         -49.0/480.0 + 43.0/160.0 * std::sqrt( 5.0),    -425.0/96.0 + 51.0/32.0 * std::sqrt( 5.0),     52.0/15.0 - 4.0/5.0 * std::sqrt( 5.0), -27.0/16.0 + 3.0/16.0 * std::sqrt( 5.0),  5.0/4.0 - 3.0/4.0 * std::sqrt( 5.0), 5.0/2.0 - 1.0/2.0 * std::sqrt( 5.0) };
    const double rk6b[] = {                1.0/12.0,                                          0.0,                                       0.0,                                     0.0,                             5.0/12.0,                            5.0/12.0,    1.0/12.0 };
    const double rk6c[] = {                 4.0/7.0,                                      5.0/7.0,                                   6.0/7.0,    1.0/2.0 - 1.0/10.0 * std::sqrt( 5.0), 1.0/2.0 + 1.0/10.0 * std::sqrt( 5.0),                                 1.0 };
    const size_t rk6n   = 7;

    // Bogacki Shampine
    const double rk34a[]   = {   1.0/2.0,      0.0,     0.0,
                                     0.0,  3.0/4.0,     0.0,
                                 2.0/9.0,  1.0/3.0, 4.0/9.0 };
    const double rk34b[]   = {   2.0/9.0,  1.0/3.0, 4.0/9.0,      0.0 };
    const double rk34bd[]  = { -5.0/72.0, 1.0/12.0, 1.0/9.0, -1.0/8.0 }; // higher order b - lower order b
    const double rk34c[]   = {   1.0/2.0,  3.0/4.0,     1.0 };
    const size_t rk34n     = 4;
    const double rk34Order = 3.0; // double as some embedded schemes use an order estimate that is not an integer value
    const bool   rk34Fsal  = true;

    // Runge Kutta Merson
    const double rk45a[]   = { 1.0/3.0,      0.0,      0.0,       0.0,
                               1.0/6.0,  1.0/6.0,      0.0,       0.0,
                               1.0/8.0,      0.0,  3.0/8.0,       0.0,
                               1.0/2.0,      0.0, -3.0/2.0,       2.0 };
    const double rk45b[]   = { 1.0/6.0,      0.0,       0.0,  4.0/6.0,   1.0/6.0 };
    const double rk45bd[]  = { 1.0/15.0,     0.0, -3.0/10.0, 4.0/15.0, -1.0/30.0 };
    const double rk45c[]   = {  1.0/3.0, 1.0/3.0,       0.5,      1.0 };
    const size_t rk45n     = 5;
    const double rk45Order = 4.0;
    const bool   rk45Fsal  = false;

    // Cash Karp
    const double rk56a[] = {          1.0/5.0,         0.0,             0.0,              0.0,            0.0,
                                     3.0/40.0,    9.0/40.0,             0.0,              0.0,            0.0,
                                     3.0/10.0,   -9.0/10.0,         6.0/5.0,              0.0,            0.0,
                                   -11.0/54.0,     5.0/2.0,      -70.0/27.0,        35.0/27.0,            0.0,
                               1631.0/55296.0, 175.0/512.0,   575.0/13824.0, 44275.0/110592.0,   253.0/4096.0 };
    const double rk56b[]   = {     37.0/378.0,         0.0,     250.0/621.0,      125.0/594.0,            0.0, 512.0/1771.0 };
    const double rk56bd[]  = { -277.0/64512.0,         0.0, 6925.0/370944.0, -6925.0/202752.0, -277.0/14336.0, 277.0/7084.0 };
    const double rk56c[]   = {        1.0/5.0,    3.0/10.0,         3.0/5.0,              1.0,        7.0/8.0 };
    const size_t rk56n     = 6;
    const double rk56Order = 5.0;
    const bool   rk56Fsal  = false;

    // Dormand Prince
    const double rk78a[]   = {               1.0/18.0,      0.0,        0.0,                          0.0,                       0.0,                         0.0,                         0.0,                         0.0,                          0.0,                       0.0,                      0.0,                       0.0,
                                             1.0/48.0, 1.0/16.0,        0.0,                          0.0,                       0.0,                         0.0,                         0.0,                         0.0,                          0.0,                       0.0,                      0.0,                       0.0,
                                             1.0/32.0,      0.0,   3.0/32.0,                          0.0,                       0.0,                         0.0,                         0.0,                         0.0,                          0.0,                       0.0,                      0.0,                       0.0,
                                             5.0/16.0,      0.0, -75.0/64.0,                    75.0/64.0,                       0.0,                         0.0,                         0.0,                         0.0,                          0.0,                       0.0,                      0.0,                       0.0,
                                             3.0/80.0,      0.0,        0.0,                     3.0/16.0,                  3.0/20.0,                         0.0,                         0.0,                         0.0,                          0.0,                       0.0,                      0.0,                       0.0,
                               29443841.0/614563906.0,      0.0,        0.0,       77736538.0/692538347.0,  -28693883.0/1125000000.0,     23124283.0/1800000000.0,                         0.0,                         0.0,                          0.0,                       0.0,                      0.0,                       0.0,
                               16016141.0/946692911.0,      0.0,        0.0,       61564180.0/158732637.0,    22789713.0/633445777.0,    545815736.0/2771057229.0,   -180193667.0/1043307555.0,                         0.0,                          0.0,                       0.0,                      0.0,                       0.0,
                               39632708.0/573591083.0,      0.0,        0.0,     -433636366.0/683701615.0, -421739975.0/2616292301.0,     100302831.0/723423059.0,     790204164.0/839813087.0,    800635310.0/3783071287.0,                          0.0,                       0.0,                      0.0,                       0.0,
                             246121993.0/1340847787.0,      0.0,        0.0, -37695042795.0/15268766246.0, -309121744.0/1061227803.0,     -12992083.0/490766935.0,   6005943493.0/2108947869.0,    393006217.0/1396673457.0,     123872331.0/1001029789.0,                       0.0,                      0.0,                       0.0,
                            -1028468189.0/846180014.0,      0.0,        0.0,     8478235783.0/508512852.0, 1311729495.0/1432422823.0, -10304129995.0/1701304382.0, -48777925059.0/3047939560.0,  15336726248.0/1032824649.0,  -45442868181.0/3398467696.0,  3065993473.0/597172653.0,                      0.0,                       0.0,
                              185892177.0/718116043.0,      0.0,        0.0,    -3185094517.0/667107341.0, -477755414.0/1098053517.0,    -703635378.0/230739211.0,   5731566787.0/1027545527.0,    5232866602.0/850066563.0,    -4093664535.0/808688257.0, 3962137247.0/1805957418.0,   65686358.0/487910083.0,                       0.0,
                              403863854.0/491063109.0,      0.0,        0.0,    -5068492393.0/434740067.0,  -411421997.0/543043805.0,     652783627.0/914296604.0,   11173962825.0/925320556.0, -13158990841.0/6184727034.0,    3936647629.0/1978049680.0,  -160528059.0/685178525.0, 248638103.0/1413531060.0,                       0.0 };
    const double rk78b[]   = { 14005451.0/335480064.0,      0.0,        0.0,                          0.0,                       0.0,    -59238493.0/1068277825.0,     181606767.0/758867731.0,     561292985.0/797845732.0,   -1041891430.0/1371343529.0,  760417239.0/1151165299.0,  118820643.0/751138087.0, -528747749.0/2220607170.0, 1.0/4.0 };
    const double rk78bd[]  = { 206899875720925.0/16966964735038208.0, 0.0, 0.0, 0.0, 0.0, 161224140072326693.0/208527862420056925.0, -308134860501296901.0/4283929245060770651.0, -187090058122256469.0/106070073963259076.0, 3721643503328385829.0/2082408744123259974.0, -290897219666967667.0/371523099811498965.0, 39496005864008611.0/501397231350176553.0, -627441401.0/2220607170.0, 1.0/4.0 };
    const double rk78c[]   = { 1.0/18.0, 1.0/12.0, 1.0/8.0, 5.0/16.0, 3.0/8.0, 59.0/400.0, 93.0/200.0, 5490023248.0/9719169821.0, 13.0/20.0, 1201146811.0/1299019798.0, 1.0, 1.0 };
    const size_t rk78n     = 13;
    const double rk78Order = 8.0;
    const bool   rk78Fsal  = true;
  }

  aIntegrator::aIntegrator()
   : integrate_( false),
     derivative_( false),
     firstPass_( false),
     initializeDerivative_( false),
     isRealTimeCycle_( false),
     isLastCycleStep_( false),
     nStateVars_( 0),
     nDerivVars_( 0),
     nDerivPoint_( FOUR_POINT - 1),
     integratorMethod_( RUNGE_KUTTA_4),
     derivativeMethod_( FOUR_POINT),
     adaptMethod_( GUSTAFSSON),
     integrationTimeStep_( 0.04),
     initialIntegrationTimeStep_( 0.04),
     integrationTimeStepMin_( 1.0e-6),
     integrationTimeStepMax_( 1.0e2),
     toleranceRel_( 1.0e-4),
     toleranceAbs_( 1.0e-4),
     errorNorm_( 0.0),
     errorRatioLast_( -1.0),
     stepFactorMax_( 1.10),
     stepFactorMin_( 0.1),
     stepFactorWgt_( 0.8),
     stepFactorAccept_( 0.95),  // accept must be <= expand
//     stepFactorExpand_( 1.05),
     isFsal_( false),
     count_( 0),
     terminatedByUser_( false)
  {
    timeList_.resize( nDerivPoint_, 0.0);
  }

  void aIntegrator::setDerivativeMethod( DerivativeMethod method)
  {
    derivativeMethod_ = method;
    nDerivPoint_      = size_t( derivativeMethod_) - 1;
    timeList_.resize( nDerivPoint_, 0.0);
  }

  void aIntegrator::setAdaptiveStepControls( const AdaptMethod method,
                                            const double stepFactorMin,
                                            const double stepFactorMax,
                                            const double stepFactorWgt,
                                            const double stepFactorAccept,
                                            const double /* stepFactorExpand */)
  {
    adaptMethod_      = method;
    stepFactorMin_    = stepFactorMin;
    stepFactorMax_    = stepFactorMax;
    stepFactorWgt_    = stepFactorWgt;
    stepFactorAccept_ = stepFactorAccept;
    //stepFactorExpand_ = stepFactorExpand;
  }

  #ifdef _DEBUG_
  void aIntegrator::integrate( const string& name, const double &xdt, double &x, double *xError)
  #else
  void aIntegrator::integrate( const string& /* name */, const double& xdt, double &x, double *xError)
  #endif
  {
    // Only update state variables if called during an integration pass.
    if ( integrate_) {
      if ( nStateVars_ >= stateList_.size()) {
        stateList_.push_back( StateVariable());
      }

      #ifdef _DEBUG_
      // Only save this when debugging.
      stateList_[nStateVars_].name = name;
      #endif
      stateList_[nStateVars_].yerror = xError;
      stateList_[nStateVars_].xptr = &x;
      stateList_[nStateVars_].xdt  = xdt;
      nStateVars_++;
    }
  }

  void aIntegrator::integrate( const string &name, const valarray<double> &xdt, valarray<double> &x, valarray<double> *xError)
  {
    size_t xdtLength = xdt.size();
    size_t xLength   = x.size();
    size_t xErrorLength = 0;

    if ( ( xdtLength > 0) && ( xdtLength == xLength)) {
      if ( xError != 0) {
        xErrorLength = xError->size();
      }

      // xErrorLength is always zero if no error vector is passed in
      if ( xdtLength == xErrorLength) {
        for ( size_t i = 0; i < xdtLength; ++i) {
          integrate( name, xdt[i], x[i], &( (*xError)[i]));
        }
      }
      else {
        for ( size_t i = 0; i < xdtLength; ++i) {
          integrate( name, xdt[i], x[i]);
        }
      }
    }
  }

  void aIntegrator::integrate( const string &name, const vector<double> &xdt, vector<double> &x, vector<double> *xError)
  {
    size_t xdtLength = xdt.size();
    size_t xLength   = x.size();
    size_t xErrorLength = 0;

    if ( ( xdtLength > 0) && ( xdtLength == xLength)) {
      if ( xError != 0) {
        xErrorLength = xError->size();
      }

      // xErrorLength is always zero if no error vector is passed in
      if ( xdtLength == xErrorLength) {
        for ( size_t i = 0; i < xdtLength; ++i) {
          integrate( name, xdt[i], x[i], &( (*xError)[i]));
        }
      }
      else {
        for ( size_t i = 0; i < xdtLength; ++i) {
          integrate( name, xdt[i], x[i]);
        }
      }
    }
  }

  void aIntegrator::integrate( const string &name, const DMatrix &xdt, DMatrix &x, DMatrix *xError)
  {
    if ( xError != 0) {
        integrate( name, xdt.matrixData(), x.matrixData(), &xError->matrixData());
    }
    else {
        integrate( name, xdt.matrixData(), x.matrixData());
    }
  }

  #ifdef _DEBUG_
  void aIntegrator::differentiate( const string &name, double &xdt, const double &x)
  #else
  void aIntegrator::differentiate( const string& /* name */, double &xdt, const double &x)
  #endif
  {
    // Only update derivative variables if called during an integration pass.
    if ( derivative_) {
      if ( nDerivVars_ >= derivList_.size()) {
        derivList_.push_back( DerivVariable());
      }

      #ifdef _DEBUG_
      // Only save this when debugging.
      derivList_[nDerivVars_].name  = name;
      #endif
      derivList_[nDerivVars_].xdtptr = &xdt;
      derivList_[nDerivVars_].x      = x;
      if ( initializeDerivative_) {
        for ( size_t i = 0; i < nDerivPoint_; ++i) {
          derivList_[nDerivVars_].xp[i] = x;
        }
      }
      nDerivVars_++;
    }
  }

//  void Integrator::differentiate( const string &name, Matrix &xdt, Matrix &x)
//  {
//    int xdtLength = xdt.getnCol() * xdt.getnRow();
//    int xLength   = x.getnCol()   * x.getnRow();
//    if ( ( xdtLength > 0) && ( xdtLength == xLength))
//    {
//      double *xdtData = xdt.getMatrixData();
//      double *xData   = x.getMatrixData();
//
//      for ( int i = 0; i < xdtLength; ++i, ++xdtData, ++xData) {
//        differentiate( name, *xdtData, *xData);
//      }
//    }
//  }

  void aIntegrator::differentiate( const string &name, valarray<double> &xdt, const valarray<double> &x)
  {
    size_t xdtLength = xdt.size();
    size_t xLength   = x.size();

    if ( ( xdtLength > 0) && ( xdtLength == xLength)) {
      for ( size_t i = 0; i < xdtLength; ++i) {
        differentiate( name, xdt[i], x[i]);
      }
    }
  }

  DMatrix aIntegrator::getDerivativeHistory()
  {
    DMatrix returnMatrix( nDerivVars_, nDerivPoint_);

    for ( size_t i = 0; i < nDerivVars_; ++i) {
      for ( size_t j = 0; j < nDerivPoint_; ++j) {
        returnMatrix( i, j) = derivList_[i].xp[j];
      }
    }

    return returnMatrix;
  }

  bool aIntegrator::setDerivativeHistory( DMatrix derivativeHistory)
  {
    if ( ( derivativeHistory.rows() <= nDerivVars_) &&
        ( derivativeHistory.cols() <= nDerivPoint_)) {
      for ( size_t i = 0; i < nDerivVars_; ++i) {
        for ( size_t j = 0; j < nDerivPoint_; ++j) {
          derivList_[i].xp[j] = derivativeHistory( i, j);
        }
      }

      return true;
    }
    else {
      return false;
    }
  }

  void aIntegrator::getState( std::vector<double> &xdt, std::vector<double> &x)
  {
    //Onus is on caller to check size of returned vector.
    xdt.resize( stateList_.size());
    x.resize( stateList_.size());

    for ( size_t i = 0; i < x.size(); ++i) {
      xdt[i] = stateList_[i].xdt;
      x[i] = *stateList_[i].xptr;
    }

    return;
  }

  void aIntegrator::setState( std::vector<double> &x)
  {
    if ( x.size() != stateList_.size()) {
      ErrorStream eout;
      eout << setFunctionName( "Integrator::setState( std::vector<double> &x)")
           << "Vector argument x of size " << x.size()
           << " does not match model state list of size " << stateList_.size();
      throw std::range_error( eout.throwStr());
    }

    integrate_ = true; //Allows subsequent calls to integrate() to build stateList_.
    resetStateList();

    for ( size_t i = 0; i < x.size(); ++i) {
      *stateList_[i].xptr = x[i];
    }
  }

  double aIntegrator::step( const double &time, const double &dt)
  {
    if ( isRealTimeCycle_) {
      realTimeClock_.start();
    }
    double completedStep( dt);

#ifdef DO_AINTEGRATOR_STATE_CHECK
    vector<double> prePreStepExecutiveState( nStateVars_, 0.0);
    for ( size_t i = 0; i < nStateVars_; ++i) {
      prePreStepExecutiveState[i] = *stateList_[i].xptr;
    }
#endif

    preStepExecutive( time, dt); //careful using dt here - adaptive method may change it!

#ifdef DO_AINTEGRATOR_STATE_CHECK
    for ( size_t i = 0; i < nStateVars_; ++i) {
      if ( isNotZero( prePreStepExecutiveState[i] - *stateList_[i].xptr)) {
        throw_message( std::runtime_error,
          setFunctionName( "Integrator::step")
          << "preStepExecutive changed the value of a state variable.\n"
        );
      }
    }
#endif

    switch ( integratorMethod_) {
    case RUNGE_KUTTA_6:
      integrateRungeKutta( time, dt, rk6a, rk6b, rk6c, rk6n);
      break;

    case RUNGE_KUTTA_4:
      integrateRungeKutta( time, dt, rk4a, rk4b, rk4c, rk4n);
      break;

    case RUNGE_KUTTA_3:
      integrateRungeKutta( time, dt, rk3a, rk3b, rk3c, rk3n);
      break;
 
    case RUNGE_KUTTA_2:
      integrateRungeKutta( time, dt, rk2a, rk2b, rk2c, rk2n);
      break;

    case RUNGE_KUTTA_78:
      completedStep = integrateEmbeddedRungeKutta( time, dt, rk78a, rk78b, rk78c, rk78bd, rk78n, rk78Order, rk78Fsal);
      break;

    case RUNGE_KUTTA_56:
      completedStep = integrateEmbeddedRungeKutta( time, dt, rk56a, rk56b, rk56c, rk56bd, rk56n, rk56Order, rk56Fsal);
      break;

    case RUNGE_KUTTA_45:
      completedStep = integrateEmbeddedRungeKutta( time, dt, rk45a, rk45b, rk45c, rk45bd, rk45n, rk45Order, rk45Fsal);
      break;

    case RUNGE_KUTTA_34:
      completedStep = integrateEmbeddedRungeKutta( time, dt, rk34a, rk34b, rk34c, rk34bd, rk34n, rk34Order, rk34Fsal);
      break;
 
   case ADAMS_BASHFORTH:
      integrateAdamsBashforth( time, dt);
      break;

    case EULER:
    default:
      integrateEuler( time, dt);
    }

    rotate( timeList_.begin(), timeList_.end()-1, timeList_.end());
    timeList_[0] = completedStep;

#ifdef DO_AINTEGRATOR_STATE_CHECK
    vector<double> prePostStepExecutiveState( nStateVars_, 0.0);
    for ( size_t i = 0; i < nStateVars_; ++i) {
      prePostStepExecutiveState[i] = *stateList_[i].xptr;
    }
#endif

    postStepExecutive( time, completedStep);

#ifdef DO_AINTEGRATOR_STATE_CHECK
    for ( size_t i = 0; i < nStateVars_; ++i) {
      if ( isNotZero( prePostStepExecutiveState[i] - *stateList_[i].xptr)) {
        throw_message( std::runtime_error,
          setFunctionName( "Integrator::step")
          << "postStepExecutive changed the value of a state variable.\n"
        );
      }
    }
#endif

    if ( isRealTimeCycle_) {
      realTimeClock_.stop();
      realTimeClock_.sleep( completedStep - realTimeClock_.getDuration());
    }
    return completedStep;
  }

  void aIntegrator::cycle( const double &time, const double &duration)
  {
    if ( duration <= 1.0e3 * DBL_EPSILON) {
      return;
    }
    double t( time);
    double tend = t + duration;

    integrationTimeStep_ = min( integrationTimeStep_, integrationTimeStepMax_);

#ifdef INTEGRATOR_MIN_TIME_STEP_ENABLED
    integrationTimeStep_ = max( integrationTimeStep_, integrationTimeStepMin_);
#endif

    isLastCycleStep_ = false;
    do {
      double requiredStep( integrationTimeStep_);
      if ( tend - t - integrationTimeStep_ < 1.0e3 * DBL_EPSILON) {
        requiredStep = tend - t;
        isLastCycleStep_ = true; //don't adapt out based on this short step
      }
      t += step( t, requiredStep);
      if ( terminatedByUser_) {
        throw_message( runtime_error,
          "Integrator::cycle - integrator terminated by user."
          "\n - t, dt, rdt, min, max = " << t << ", " << integrationTimeStep_ << ", " << requiredStep
          << ", " << integrationTimeStepMin_ << ", " << integrationTimeStepMax_
        );
      }
    }
    while ( t < tend);
  }

  double aIntegrator::cycleForSingleStep( const double &time, const double &maximumDuration) 
  {
    
    if ( maximumDuration <= 1.0e3 * DBL_EPSILON) {
      return 1.0e3 * DBL_EPSILON;
    }

    integrationTimeStep_ = min( integrationTimeStep_, integrationTimeStepMax_);

#ifdef INTEGRATOR_MIN_TIME_STEP_ENABLED
    integrationTimeStep_ = max( integrationTimeStep_, integrationTimeStepMin_);
#endif

    isLastCycleStep_ = false;
    double requiredStep( integrationTimeStep_);
    if ( maximumDuration - integrationTimeStep_ < 1.0e3 * DBL_EPSILON) {
      requiredStep = maximumDuration;
      isLastCycleStep_ = true; //don't adapt out based on this short step
    }
    const double dt = step( time, requiredStep);
    if ( terminatedByUser_) {
      throw_message( runtime_error,
        "Integrator::cycleForSingleStep - integrator terminated by user."
        "\n - t, dt, rdt, min, max = " << time << ", " << integrationTimeStep_ << ", " << requiredStep
        << ", " << integrationTimeStepMin_ << ", " << integrationTimeStepMax_
      );
    }

    return dt;
  }

  void aIntegrator::resetIntegrator()
  {
    setIntegrationTimeStep( initialIntegrationTimeStep_);
    isFsal_ = false;
  }

  void aIntegrator::setIntegrationTimeStep( const double &integrationTimeStep)
  {
    integrationTimeStep_        = integrationTimeStep;
    initialIntegrationTimeStep_ = integrationTimeStep_;
    errorRatioLast_             = -1.0;
    count_                      = 0;
  }

  void aIntegrator::resetStateList()
  {
    nStateVars_ = 0;
    nDerivVars_ = 0;
  }

  double aIntegrator::integrateEmbeddedRungeKutta( const double& t, const double& dtIn,
                                                   const double* a, const double* b,
                                                   const double* c, const double* bDiff,
                                                   const size_t  n, const double& order, 
                                                   const bool /* fsal */)
  {
    /*
     * This function expects arrays representing a Butcher tableau in the form
     *
     *    c | a
     *   -------
     *      | b
     *      | bl 
     *
     *   ( bDiff = b - bl)
     *
     * with the top row and right column of a, and top row of c, (all zeros) 
     * not included. n is the number of derivatives to calculate (length of b). 
     * order is the order of the higher-order method used to propagate the integration
     * (corresponding to the b array). bDiff is used to estimate the local
     * truncation error of the integration scheme and inform the adaptive time step
     * algorithm.
     */

    // Lambda expression to compute the adaptive step size adaptation factor.
    auto getStepFactor = [&]( const double& errorRatio) -> double
    {
      double stepFactor( 1.0);

      if ( errorRatioLast_ < 0.0) { /* set first step previous error ratio */
        errorRatioLast_ = errorRatio;
      }

      if ( PRESS == adaptMethod_) {
        double stepExponent = 1.0 / ( order - 1.0);
        stepFactor = stepFactorWgt_ * std::pow( errorRatio, - stepExponent);
      }
      else { /* GUSTAFSSON */
        stepFactor = 
          std::pow( stepFactorWgt_ / errorRatio, 0.3 / order) * // Soderlind suggests 0.4, 0.2 for these parameters to reduce overshoot.
          std::pow( errorRatioLast_ / errorRatio, 0.4 / order); // Gustafsson 0.3, 0.4
      }

      // Bounding this in place and in this manner to ensure if stepFactor == nan, stepFactorMin_ is returned.
      stepFactor = stepFactor > stepFactorMax_ ? stepFactorMax_ : stepFactor > stepFactorMin_ ? stepFactor : stepFactorMin_;

      return stepFactor;
    };

    // Lambda expression that returns an error ratio that when passed to getStepFactor, will cause getStepFactor to return desiredStep.
    auto getErrorRatio = [&]( const double& desiredStepFactor) -> double
    {
      double errorRatio;

      if ( adaptMethod_ == PRESS) {
        errorRatio = std::pow( desiredStepFactor / stepFactorWgt_, 1.0 - order);
      }
      else { /* GUSTAFSSON */
        if ( errorRatioLast_ < 0.0) { /* due to using previous error ratio for first step */
          errorRatio = stepFactorWgt_ / std::pow( desiredStepFactor, order / 0.3); 
        }
        else {
          const double stepVal = std::pow( stepFactorWgt_, 0.3 / order) 
            * std::pow( errorRatioLast_, 0.4 / order) / desiredStepFactor;
          errorRatio = std::pow( stepVal, order / ( 0.3 + 0.4));
        }
      }

      return errorRatio;
    };

    double dt = dtIn;

    while ( 1) {

      integrateRungeKutta( t, dt, a, b, c, n);

      if ( count_ < initSteps) {
        break;
      }

      integrate_ = true;
    
      errorNorm_ = 0.0;
      double errorRatio( 0.0);
      const double fallbackErrorRatio = getErrorRatio( stepFactorMax_);
      for ( size_t i = 0; i < nStateVars_; ++i) {
        double error( 0.0);
        for ( size_t j = 0; j < n; ++j) {
          error += bDiff[j] * stateList_[i].k[j];
        }
        error = dt * abs( error);

        const double tolerance = 
          max( *stateList_[i].xptr * toleranceRel_, toleranceAbs_);

        if ( error < 1.0e3 * DBL_EPSILON) {
          error = fallbackErrorRatio;
        }
        else {
          error = error / tolerance;
          errorNorm_ = max( errorNorm_, error);
        }

        errorRatio = max( errorRatio, error);

        if ( 0 != stateList_[i].yerror) {
          *stateList_[i].yerror = error;
        }
      }

      const double stepFactor = getStepFactor( errorRatio);

      if ( stepFactor >= stepFactorAccept_) {

        isFsal_ = false; // Setting to false here due to it being set to true upon failed step

        /** Completed an approximately successful step, so increase step size for
         * next step if necessary, then finish.
         */
        if ( isLastCycleStep_) {
          break;
        }

        integrationTimeStep_ = min( integrationTimeStep_ * stepFactor, integrationTimeStepMax_);
        // errorRatioLast_      = getErrorRatio( stepFactor, order); // If the step size has been bounded, this will return the effective error ratio.
        errorRatioLast_ = errorRatio;

        /*
        * use FSAL property to avoid re-calculating first gradient, and
        * flag it so the next step knows about it
        */
        //if ( fsal) {
        //  for ( size_t i = 0; i < nStateVars_; ++i) {
        //    stateList_[i].k[0] = stateList_[i].k[n - 1];
        //  }
        //  isFsal_ = true;
        //} //@TODO: Reconsider isFsal_, as currently prevents changing number of states between steps.

        break;
      }

      /*
      * Step was too big, so reduce the step size, reset the state vector,
      * and repeat the step.
      */

#ifdef INTEGRATOR_MIN_TIME_STEP_ENABLED
      // If last loop resulted in contraction to min step size, accept result and exit.
      if ( dt - integrationTimeStepMin_ < zero()) {
        break;
      }
#endif

      dt = dt * stepFactor;
      integrationTimeStep_ = dt;

      /** We should be able to use the following, however it causes some instability that needs to be sorted out 
       * (derived parameters not updating properly somewhere is the likely cause).
       */
      // isFsal_ = true; // Prevent recalculating the derivative at time t.

      for ( size_t i = 0; i < nStateVars_; ++i) {
        *stateList_[i].xptr = stateList_[i].xn;
      }

#ifndef INTEGRATOR_MIN_TIME_STEP_ENABLED
      if ( isZero( integrationTimeStep_)) {
        throw_message( std::runtime_error,
          setFunctionName( "Integrator::integrateEmbeddedRungeKutta") << "\n"
          << "  Minimum step size limit exceeded.\n"
          << "  time                 = " << t << "\n"
          << "  integrationTimeStep_ = " << integrationTimeStep_;
        );
      }
#endif

      onFailedAdaptiveStep();
    }

    if ( count_ < initSteps) {
      ++count_;
    }
    integrate_ = false;

    return dt;
  }

  void aIntegrator::integrateRungeKutta( const double& t, const double& dt,
                                         const double* a, const double* b,
                                         const double* c, const size_t n)
  {
    /*
     * This function expects arrays representing a Butcher tableau in the form
     *
     *    c | a
     *   -------
     *      | b
     *
     * with the top row and right column of a, and top row of c, (all zeros) 
     * not included.
     */
    size_t origNumStateVars = 0;
    integrate_  = true;
    derivative_ = true;
    firstPass_  = true;

    /*
     * predictor steps
     */

    // first step unrolled from loop as it's handled differently
    if ( !isFsal_) {
      resetStateList();
      derivativeExecutive( t);
    }
    firstPass_ = false;
    differentiation();
    derivative_ = false;
    origNumStateVars = nStateVars_;
    for ( size_t i = 0; i < nStateVars_; ++i) {
      if ( !isFsal_) {
        stateList_[i].k[0] = stateList_[i].xdt;
      }
      stateList_[i].xn = *stateList_[i].xptr;
    }

    // remainder of loop
    for ( size_t j = 1; j < n; ++j) {
      for ( size_t i = 0; i < nStateVars_; ++i) {
        double dx( 0.0);
        for ( size_t k = 0; k < j; ++k) {
          dx += a[ k + ( j - 1) * ( n - 1)] * stateList_[i].k[k];
        }
        *stateList_[i].xptr = stateList_[i].xn + dt * dx;
      }
      resetStateList();
      derivativeExecutive( t + c[ j - 1 ] * dt);
      if ( origNumStateVars != nStateVars_) {
        throw_message( runtime_error,
          setFunctionName( "Integrator::integrateRungeKutta") << "\n"
          << "  Number of integrators changed on minor step\n"
          << "  therefore illegal equations in model."
        );
      }
      for ( size_t i = 0; i < nStateVars_; ++i) {
        stateList_[i].k[j] = stateList_[i].xdt;
      }
    }

    /*
     * corrector step
     */
    for ( size_t i = 0; i < nStateVars_; ++i) {
      double dx( 0.0);
      for ( size_t j = 0; j < n; ++j) {
        dx += b[j] * stateList_[i].k[j];
      }
      *stateList_[i].xptr = stateList_[i].xn + dt * dx;
    }

    integrate_ = false;
  }

  void aIntegrator::integrateAdamsBashforth(const double &t, const double &dt)
  {
    size_t origNumStateVars;

    if ( count_ < 3) {
      // Take RK4 step to populate state derivative history.
      integrateRungeKutta( t, dt, rk4a, rk4b, rk4c, rk4n);
      for ( size_t i = 0; i < nStateVars_; ++i) {
        stateList_[i].xdtlll = stateList_[i].xdtll;
        stateList_[i].xdtll  = stateList_[i].xdtl;
        stateList_[i].xdtl   = stateList_[i].xdt;
      }
      count_++;
    }
    else {
      // Commence Adams-Bashforth algorithm.
      integrate_ = true;
      derivative_ = true;
      firstPass_ = true;

      origNumStateVars = nStateVars_;
      resetStateList();
      // Pass 1.
      // Calculates xdt at time t.
      derivativeExecutive( t);
      if ( origNumStateVars != nStateVars_) {
        ErrorStream eout;
        eout << "Integrator::integrateAdamsBashforth" << endl;
        eout << "  - Number of states changed between steps, so algorithm broken." << endmsg;
      }
      firstPass_ = false;

      differentiation(); // process any numerical differentiation of parameters
      derivative_ = false;

      for ( size_t i = 0; i < nStateVars_; ++i) {
        stateList_[i].xn   = *stateList_[i].xptr;
        stateList_[i].k[0] = dt * stateList_[i].xdt;
        stateList_[i].k[1] = dt * stateList_[i].xdtl;
        stateList_[i].k[2] = dt * stateList_[i].xdtll;
        stateList_[i].k[3] = dt * stateList_[i].xdtlll;
        //predictor:
        *stateList_[i].xptr = stateList_[i].xn 
          + ( 55.0*stateList_[i].k[0] - 59.0*stateList_[i].k[1] +
           + 37.0*stateList_[i].k[2] - 9.0*stateList_[i].k[3]) / 24.0;

        stateList_[i].xdtlll = stateList_[i].xdtll;
        stateList_[i].xdtll  = stateList_[i].xdtl;
        stateList_[i].xdtl   = stateList_[i].xdt;
      }

      origNumStateVars = nStateVars_;
      resetStateList();
      // Pass 2.
      // Calculates xdt at t + dt.
      derivativeExecutive( t + dt);
      if ( origNumStateVars != nStateVars_) {
        ErrorStream eout;
        eout << "Integrator::integrateAdamsBashforth" << endl;
        eout << "  - Number of states changed on minor step, so algorithm broken." << endmsg;
      }

      for ( size_t i = 0; i < nStateVars_; ++i) {
        stateList_[i].k[3] = dt * stateList_[i].xdt;
        //correction:
        *stateList_[i].xptr = stateList_[i].xn + 
          ( 9.0*stateList_[i].k[3] + 19.0*stateList_[i].k[0] -
           5.0*stateList_[i].k[1] + stateList_[i].k[2]) / 24.0;
      }

      integrate_ = false;
    }
  }

  void aIntegrator::integrateEuler(const double &t, const double &dt)
  {
    integrate_ = true;
    derivative_ = true;

    resetStateList();
    // Pass 1.
    derivativeExecutive( t);

    differentiation(); // process any derivatives to be computed
    derivative_ = false;

    for ( size_t i = 0; i < nStateVars_; ++i) {
      *stateList_[i].xptr += dt * stateList_[i].xdt;
    }

    integrate_ = false;
  }

  void aIntegrator::differentiation()
  {
    switch ( derivativeMethod_) {
      case TWO_POINT:
        derivativeTwo();
        break;

      case THREE_POINT:
        derivativeThree();
        break;

      case FOUR_POINT:
      default:
        derivativeFour();
        break;
    }
  }

  void aIntegrator::derivativeTwo()
  {
    for ( size_t i = 0; i < nDerivVars_; ++i) {
      if ( timeList_[0] > dstomath::zero()) {
        *derivList_[i].xdtptr = 
          ( derivList_[i].x - derivList_[i].xp[0]) / timeList_[0];
        
        setDerivativeHistory( i);
      }
      else {
        *derivList_[i].xdtptr = 0.0;
      }
    }
  }

  void aIntegrator::derivativeThree()
  {
    double dt12 = timeList_[0] * timeList_[0];
    double dt22 = timeList_[1] * timeList_[1];
    double dtdiv =  timeList_[0] * dt22 + dt12 * timeList_[1];
    for ( size_t i = 0; i < nDerivVars_; ++i) {
      if ( timeList_[0] > dstomath::zero() &&
           timeList_[1] > dstomath::zero()) {
        *derivList_[i].xdtptr = 
          ( - dt12 * derivList_[i].xp[1] + 
            ( dt22 + 2.0 * timeList_[0] * timeList_[1] + dt12) * 
            derivList_[i].xp[0] + 
            ( - dt22 - 2.0 * timeList_[0] * timeList_[1]) * 
            derivList_[i].x) / dtdiv;

        setDerivativeHistory( i);
      }
      else {
        *derivList_[i].xdtptr = 0.0;
      }
    }
  }

  void aIntegrator::derivativeFour()
  {
    double dt12 = timeList_[0] * timeList_[0];
    double dt22 = timeList_[1] * timeList_[1];
    double dt32 = timeList_[2] * timeList_[2];
    double dt13 = dt12 * timeList_[0];
    double dt23 = dt22 * timeList_[1];
    double dt33 = dt32 * timeList_[2];
    double dtdiv = timeList_[0] * ( dt22 * dt33 + 2.0 * dt23 * dt32 + 
                                      dt22 * dt22 * timeList_[2]) + 
      dt12 * ( timeList_[1] * dt33 + 3.0 * dt22 * dt32 + 
               2.0 * dt23 * timeList_[2]) + 
      dt13 * ( timeList_[1] * dt32 + dt22 * timeList_[2]);

    for ( size_t i = 0; i < nDerivVars_; ++i) {
      if ( timeList_[0] > dstomath::zero() &&
           timeList_[1] > dstomath::zero() &&
           timeList_[2] > dstomath::zero()) {
        *derivList_[i].xdtptr = 
          - ( dt12 * ( dt23 * derivList_[i].xp[2] + 
                       ( - dt33 - 3.0 * timeList_[1] * dt32 - 
                         3.0 * dt22 * timeList_[2] - dt23) * 
                       derivList_[i].xp[1])
              + dt13 * ( 2.0 * dt22 * derivList_[i].xp[2] + 
                         ( - 2.0 * dt32 - 4.0 * timeList_[1] * timeList_[2] -
                           2.0 * dt22) * derivList_[i].xp[1]) + 
              dt12 * dt12 * ( timeList_[1] * derivList_[i].xp[2] + 
                              ( - timeList_[2] - timeList_[1]) * 
                              derivList_[i].xp[1]) + 
              ( timeList_[0] * ( 2.0 * timeList_[1] * dt33 + 
                                   6.0 * dt22 * dt32 + 4.0 * dt23 * 
                                   timeList_[2]) + 
                dt12 * ( dt33 + 6.0 * timeList_[1] * dt32 + 6.0 * dt22 *
                         timeList_[2]) + dt22 * dt33 + dt13 *
                ( 2.0 * dt32 + 4.0 * timeList_[1] * timeList_[2]) +
                2.0 * dt23 * dt32 + dt22 * dt22 * timeList_[2] + 
                dt12 * dt12 * timeList_[2]) * derivList_[i].xp[0] +
              ( timeList_[0] * ( - 2.0 * timeList_[1] * dt33 - 
                                   6.0 * dt22 * dt32 - 4.0 * dt23 * 
                                   timeList_[2]) - 
                dt22 * dt33 + dt12 * ( - 3.0 * timeList_[1] * dt32 - 
                                       3.0 * dt22 * timeList_[2]) - 
                2.0 * dt23 * dt32 - dt22 * dt22 * timeList_[2]) *
              derivList_[i].x) / dtdiv;

        setDerivativeHistory( i);
      }
      else {
        *derivList_[i].xdtptr = 0.0;
      }
    }
  }

  void aIntegrator::setDerivativeHistory(const size_t i)
  {
    for ( size_t j = ( nDerivPoint_ - 1); j > 0; j--)
    {
      derivList_[i].xp[j] = derivList_[i].xp[(j - 1)];
    }
    derivList_[i].xp[0] = derivList_[i].x;
  }

  void aIntegrator::setIntegratorMethod( IntegratorMethod method)
  {
    integratorMethod_ = method;
    resetIntegrator();
  }

} // End namespace dstomath
