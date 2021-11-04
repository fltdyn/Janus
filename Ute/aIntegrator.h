#ifndef _AINTEGRATOR_H_
#define _AINTEGRATOR_H_

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

// C++ Includes
#include <vector>
#include <string>

// Ute Includes
#include "aMatrix.h"
#include "aRealTimeClock.h"

namespace dstomath {

  /** Integrator is an integration engine class.  It includes a variety of
   * integration method options, both fixed-step and adaptive.  It has primarily
   * been tested on non-stiff DE flight dynamics and performance problems, but
   * the adaptive options should handle stiff DE problems without gross
   * inefficiency.  Integrators for specific flight vehicle models inherit from
   * this class and override the virtual functions #preStepExecutive,
   * #derivativeExecutive and #postStepExecutive with the model's equivalent
   * equations.
   */
  class aIntegrator
  {
   public:
    /** This enum is used to select the integrator method for use by a simulation.
     * The majority of methods provided are single-step predictor-corrector
     * schemes.  The normal default is to use fourth order Runge-Kutta, which
     * provides a relatively high order constant step size solution.  If an
     * alternative method is required, Cellier & Kofman, "Continuous System
     * Simulation", Chapter 3, provides extensive information and some useful
     * decision rationales.  If adaptive step sizing is required, the
     * recommended initial choice is the third order Bogacki-Shampine method.
     */
    enum IntegratorMethod {
      /** Forward Euler's method.  First order, 1 derivative evaluation.
       * Not quality-controlled, not very stable or accurate for typical
       * flight dynamics problems.
       *
       * k1    = f( x_k, t_k ) \n
       * x_k+1 = x_k + k1 * dt
       */
      EULER = 1,

      /** Heun's method.  Second order Runge-Kutta, 2 derivative evaluations.
       * Not quality-controlled, not very accurate for typical flight dynamics
       * problems but applicable to performance work.
       *
       * k1    = f( x_k, t_k ) \n
       * x_p   = x_k + k1 * dt \n
       *
       * k2    = f( x_p, t_k+1 ) \n
       *
       * x_k+1 = x_k + ( k1 + k2 ) * dt / 2
       */
      RUNGE_KUTTA_2 = 2,

      /** Bogacki-Shampine method.  Third order Runge-Kutta, 3 derivative
       * evaluations.  Not quality controlled, may be inaccurate for some
       * flight dynamics problems unless step size is very small.
       *
       * k1    = f( x_k, t_k ) \n
       *
       * x_p1  = x_k + k1 * dt / 2 \n
       * k2    = f( x_p1, t_k+1/2 ) \n
       *
       * x_p2  = x_k + k2 * dt * 3/4 \n
       * k3    = f( x_p2, t_k+3/4 ) \n
       *
       * x_k+1 = x_k + ( 2 * k1 + 3 * k2 + 4 * k3 ) * dt / 9
       */
      RUNGE_KUTTA_3 = 3,

      /** Bogacki-Shampine method.  Third order Runge-Kutta, with second order
       * error estimate, 4 derivative evaluations.  Uses
       * \ref RUNGE_KUTTA_3 plus one more function evaluation to determine the
       * error.  k4 is same as k1 for next step (FSAL property).  This is the
       * recommended adaptive integration method for flight dynamics applications.
       *
       * k4    = f( x_k+1, t_k+1 ) \n
       * error = ( -5 * k1 + 6 * k2 + 8 * k3 - 9 * k4 ) * dt / 72
       */
      RUNGE_KUTTA_34 = 34,

      /** Fourth order Runge-Kutta, the normal default method.
       *
       * k1    = f( x_k, t_k ) \n
       *
       * x_p1  = x_k + k1 * dt / 2 \n
       * k2    = f( x_p1, t_k+1/2 ) \n
       *
       * x_p2  = x_k + k2 * dt /2 \n
       * k3    = f( x_p2, t_k+1/2 ) \n
       *
       * x_p3  = x_k + k3 * dt \n
       * k4    = f( x_p3, t_k+1 ) \n
       *
       * x_k+1 = x_k + ( k1 + 2 * k2 + 2 * k3 + k4 ) * dt / 6
       */
      RUNGE_KUTTA_4 = 4,

      /** Runge-Kutta-Merson.  Fourth order Runge-Kutta, 5 derivative evaluations,
       *  with error estimate which is used to provide adaptive step sizing
       * to achieve a specified accuracy.  Note that the Merson error estimate is
       * strictly only valid for linear DEs with constant coefficients.
       *
       * k1    = f( x_k, t_k ) \n
       *
       * x_p1  = x_k + k1 * dt / 3 \n
       * k2    = f( x_p1, t_k+1/3 ) \n
       *
       * x_p2  = x_k + ( k1 + k2 ) * dt / 6 \n
       * k3    = f( x_p2, t_k+1/3 ) \n
       *
       * x_p3  = x_k + ( k1 + 3 * k3 ) * dt / 8 \n
       * k4    = f( x_p3, t_k+1/2 ) \n
       *
       * x_p4  = x_k + ( k1 - 3 * k3 + 4 * k4 ) * dt / 2 \n
       * k5    = f( x_p4, t_k+1 ) \n
       *
       * x_k+1 = x_k + ( k1 + 4 * k4 + k5 ) * dt / 6 \n
       *
       * error = ( 2 * k1 - 9 * k3 + 8 * k4 - k5 ) * dt / 30 \n
       */
      RUNGE_KUTTA_45 = 45,

      /** Cash-Karp (Fehlberg form). Fifth order Runge-Kutta, 6 derivative
       * evaluations, with error estimate (the error of a related fourth order
       * solution) which is used to provide adaptive step sizing to achieve a
       * specified accuracy.  This is normally overkill for a flight
       * dynamic model, since the combination of aircraft dynamic response
       * frequencies, model order, and time step size tends towards
       * mutual inefficiency.
       *
       * k1    = f( x_k, t_k ) \n
       *
       * x_p1  = x_k + k1 * dt / 5 \n
       * k2    = f( x_p1, t_k+1/5 ) \n
       *
       * x_p2  = x_k + ( 3 * k1 + 9 * k2 ) * dt / 40 \n
       * k3    = f( x_p2, t_k+3/10 ) \n
       *
       * x_p3  = x_k + ( 3 * k1 - 9 * k2 + 12 * k3 ) * dt / 10 \n
       * k4    = f( x_p2, t_k+3/5 ) \n
       *
       * x_p4  = x_k + ( -11 * k1 + 135 * k2 - 140 * k3 + 70 * k4 ) * dt / 54 \n
       * k5    = f( x_p4, t_k+1 ) \n
       *
       * x_p5  = x_k + ( 3262 * k1 + 37800 * k2 + 4600 * k3 + 44275 * k4 + 6831 * k5 ) * dt / 110592 \n
       * k6    = f( x_p5, t_k+7/8 ) \n
       *
       * x_k+1 = x_k + ( 37/378 * k1 + 250/621 * k3 + 125/594 * k4 + 512/1771 * k6 ) * dt \n
       *
       * error = ( - 277/64512 * k1 + 6925/370944 * k3 - 6925/202752 * k4 - 277/14366 * k5 + 277/7084 * k6 ) * dt
       */
      RUNGE_KUTTA_56 = 56,

      /** Adams-Bashforth.  Fourth-order multi-step method, 4 derivative
       * evaluations, not quality controlled. First steps are taken using
       * \ref RUNGE_KUTTA_4 to provide starting derivative values.
       *
       * k1 = f( x_k-3, t_k-3 ) \n
       * k2 = f( x_k-2, t_k-2 ) \n
       * k3 = f( x_k-1, t_k-1 ) \n
       * k4 = f( x_k, t_k ) \n
       *
       * x_k+1 = x+k + ( 55 * k4 - 59 * k3 + 37 * k2 - 9 * k1 ) * dt / 24
       */
      ADAMS_BASHFORTH = 6,

      /** Hammud (corrected by Alshina Zaks Kalitkin). Sixth order Runge-Kutta,
       * seven derivative evaluations. Not quality controlled.
       */
      RUNGE_KUTTA_6   = 60,

      /** Dormand-Prince 8(7). Eighth order Runge-Kutta, 13 derivative evaluations,
       * with error estimate relative to the seventh order solution used to provide
       * adaptive step sizing to achieve a specified accuracy. Almost definitely
       * overkill for a flight dynamic model.
       */
      RUNGE_KUTTA_78  = 78
    };

    /** This enum is used to select or interrogate the adaptation algorithm
     * used by the adaptive integrators, controlling how the step size is
     * adjusted to achieve the desired integration accuracy.  The current
     * default is to use the Gustafsson method, and this is recommended for
     * general use.
     */
    enum AdaptMethod {
      /** Press et al.  Relates an error ratio to a step size ratio, based on
       * Press et al 16.2.10.  Uses both relative and absolute tolerance for
       * each state vector element to determine the largest error ratio,
       * which is used as the basis for estimation of the required step size
       * adjustment factor.
       */
      PRESS = 1,
      /** Gustafsson.  Uses a PI controller to minimise the difference
       * between state vector error and tolerance, derived from Cellier &
       * Kofman 3.91.  Uses both relative and absolute tolerance for
       * each state vector element to determine the largest error ratio,
       * which is input to the controller to estimate required step size
       * adjustment factor.
       */
      GUSTAFSSON = 2
    };

    /** This enum is used to select or interrogate the numerical differentiation
     * model to be used during a simulation.  Numerical differentiation is
     * mathematically noisy and computationally undesirable, so most components of
     * \ref Amiel are implemented without requiring this process.  However, should
     * numerical differentiation be unavoidable for any measurement variable,
     * this enum may be required.
     * The default derivative method is \ref FOUR_POINT.  Computation of the
     * derivatives includes the effects of varying step size, if adaptive
     * integration is in use.
     */
    enum DerivativeMethod {
      /** Two point (linear) derivative.
       */
      TWO_POINT   = 2,
      /** Three point (quadratic) derivative.
       */
      THREE_POINT = 3,
      /** Four point (cubic) derivative.
       */
      FOUR_POINT  = 4
    };

    aIntegrator();
    virtual ~aIntegrator() {}

    /** Sets the method for integration computations, and (re)initialises
     * related private member variables.  The integration
     * method may be set before a simulation, or changed at any time
     * during a simulation.  The available methods are described in the
     * \ref IntegratorMethod enum documentation.
     * @param method is one of \ref EULER, \ref RUNGE_KUTTA_2,
     * \ref RUNGE_KUTTA_3, \ref RUNGE_KUTTA_4, \ref RUNGE_KUTTA_34,
     * \ref RUNGE_KUTTA_45, \ref RUNGE_KUTTA_56, \ref ADAMS_BASHFORTH.  The
     * deafult method is \ref RUNGE_KUTTA_4.
     */
    void setIntegratorMethod( IntegratorMethod method);

    /** Provides access to the method currently specified for numerical
     * integration computations.  If none has been explictly specified using
     * \ref setIntegratorMethod, the default value of \ref RUNGE_KUTTA_4 will be
     * returned.
     * @return An \ref IntegratorMethod enum, with possible values of \ref EULER,
     * \ref RUNGE_KUTTA_2, \ref RUNGE_KUTTA_3, \ref RUNGE_KUTTA_4,
     * \ref RUNGE_KUTTA_34, \ref RUNGE_KUTTA_45, \ref RUNGE_KUTTA_56, or
     * \ref ADAMS_BASHFORTH, is returned.
     */
    IntegratorMethod getIntegratorMethod() const { return integratorMethod_;}

    /** Sets the method for numerical derivative computations, and
     * (re)initialises related private member variables.  The numerical
     * differentiation method may be set before a simulation, or changed at any
     * time during a simulation.  The available methods are described in the
     * \ref DerivativeMethod enum documentation.
     * Sets the derivative method and initialises the derivative data.
     * @param method is one of \ref TWO_POINT, \ref THREE_POINT, \ref FOUR_POINT.
     * The default method is \ref FOUR_POINT.
     */
    void setDerivativeMethod( DerivativeMethod method);

    /** Provides access to the method currently specified for numerical
     * differentiation computations.  If none has been explictly specified using
     * \ref setDerivativeMethod, the default value of \ref FOUR_POINT will be
     * returned.
     * @return A \ref DerivativeMethod enum, with possible values of
     * \ref TWO_POINT, \ref THREE_POINT, \ref FOUR_POINT, is returned.
     */
    DerivativeMethod getDerivativeMethod() const { return derivativeMethod_;}
    /** Sets the adaptation algorithm to be used by adaptive integrations.
     * Note that integration method and adaptation algorithm may be set
     * separately, since either adaptation may be used with all adaptive
     * integrators.  This function has no effect on the fixed-step
     * integration methods.
     * \param method is one of \ref PRESS, \ref GUSTAFSSON.  It may be changed
     * between steps during a simulation.  The default value is \ref GUSTAFSSON.
     */
    void setAdaptMethod( const AdaptMethod method ) { adaptMethod_ = method; }

    /** Provides access to the method currently specified for the adaptation
     * algorithm used by adaptive integrations.  If none has been explicitly
     * specified using \ref setAdaptMethod or setAdaptiveStepControls, the
     * default value of \ref GUSTAFSSON will be returned.
     * \return The returned method is one of \ref PRESS, \ref GUSTAFSSON.
     */
    AdaptMethod getAdaptMethod() const { return adaptMethod_;}

    /** Sets the adaptation algorithm and limits used by adaptive integrations.
     * Note that integration method and adaptation algorithm may be set
     * separately, since either adaptation may be used with all adaptive
     * integrators.  These parameters can be critical to the stability and
     * performance of the adaptive integrators.  They should be left at default
     * values unless a specific integration problem, clearly diagnosed by an
     * experienced user, indicates otherwise.  None of these parameters have
     * any effect on the fixed-step integration methods.
     * \param method is one of \ref PRESS, \ref GUSTAFSSON.  It defines the
     * adaptation algorithm to use.  It may be changed between steps during a
     * simulation.  The default value is \ref GUSTAFSSON.
     * \param stepFactorMin limits the factor by which an integration timestep
     * may be reduced after each attempted integration step.  A default value
     * of 0.1 is used.  Increasing this can sometimes help with difficult
     * integrations, particularly for stiff ODEs.
     * \param stepFactorMax limits the factor by which an integration timestep
     * may be increased after each successful integration step.  A default value
     * of 5.0 is used.
     * \param stepFactorWgt reduces the nominal step factor computed using the
     * explicit adaptation method.  It is a factor of ignorance, allowing for
     * low quality error estimates.  The default value is 0.8.
     * \param stepFactorAccept provides the lower cut off of a successful
     * integration step.  The default value is 1.0.  Reducing this value will
     * tend to force smaller step sizes, while increasing it will allow larger
     * steps.  This parameter
     * interacts with stepFactorWgt.  Simultaneously reducing or increasing this
     * factor and stepFactorWgt will negate the effects of both parameters.
     * \param stepFactorExpand limits step expansion to a minimum factor, since
     * factors near 1.0 have negligible effect on the solution.  This factor must
     * be greater than or equal to stepFactorAccept, and less than or equal to
     * stepFactorMax.  Its default value is 1.05.
     */

    void setAdaptiveStepControls( const AdaptMethod method,
                                  const double stepFactorMin,
                                  const double stepFactorMax,
                                  const double stepFactorWgt,
                                  const double stepFactorAccept,
                                  const double stepFactorExpand );

    /** On the first pass of an integration, this function adds the time
     * derivative and current value of a state vector component to the state
     * vector list for this \ref Integrator instance.  On subsequent passes,
     * updates the derivative value.  The inputs to this function should be
     * derived from pass-by-reference function calls, since the state component
     * is included in the state list by pointer to allow it to be updated by the
     * integration process.  This function is normally called from within the
     * \ref derivativeExecutive function of the class which inherited from this
     * \ref Integrator instance.
     * @param xdt is the derivative of the state vector component x.
     * @param x is the state vector component that requires integrating.
     * @param xError is an optional pointer to a variable for returning the
     * integration error when using an adaptive integration method.  The
     * integration error is composed of the numerical integration error estimate
     * applicable to the integration method in use, normalised by the more
     * critical of the variable's absolute error tolerance and its relative
     * error tolerance factored by the variable current value.  At present,
     * no Amiel components make use of the returned error.
     */
    void integrate( const std::string &name, const double &xdt, double &x, double *xError = 0);

    /** This form of the \ref integrate function accepts valarray arguments to
     * allow multiple variables to be added to the integrator's state vector list
     * simultaneously.  In other respects it functions identically to the scalar
     * \ref integrate function.  In particular, the state vector component inputs
     * must be passed by reference.  This function is normally called from within
     * the \ref derivativeExecutive function of the class which inherited from
     * this \ref Integrator instance, or from a private member class within the
     * inheriting class.
     * @param xdt is an array of derivative values of the corresponding state
     * vector components x.
     * @param x is an array of state vector components that require integrating.
     * @param xError is an optional pointer to an array for returning the
     *     integration error when using an adaptive integration method.
     */
    void integrate( const std::string &name, const std::valarray<double> &xdt, std::valarray<double> &x, std::valarray<double> *xError = 0);
    void integrate( const std::string &name, const std::vector<double> &xdt, std::vector<double> &x, std::vector<double> *xError = 0);

    /** This form of the \ref integrate function accepts matrix arguments to
     * allow multiple variables to be added to the integrator's state vector list
     * simultaneously.  In other respects it functions identically to the scalar
     * \ref integrate function.  In particular, the state vector component inputs
     * must be passed by reference.  This function is normally called from within
     * the \ref derivativeExecutive function of the class which inherited from
     * this \ref Integrator instance, or from a private member class within the
     * inheriting class.
     * @param xdt is a matrix of derivative values of the corresponding state
     * vector components x.
     * @param x is a matrix of state vector components that require integrating.
     * @param xError is an optional pointer to a matrix for returning the
     *     integration error when using an adaptive integration method.
     */
    void integrate( const std::string &name, const DMatrix &xdt, DMatrix &x, DMatrix *xError = 0);

    /** Adds derivative and initial state to a list.
     * @param xdt is a derivative variable.
     * @param x is a variable to be differentiated.
     * This function is called from the function derivativeExecutive()
     * to differentiate a state variable.
     */
    void differentiate( const std::string &name, double &xdt, const double &x);

    /** Adds derivative and initial state to a list.
     * @param xdt is an array of derivative variables.
     * @param x is an array of variables to be differentiated.
     * This function is called from the function derivativeExecutive()
     * to differentiate a state variable.
     */
  //  void differentiate( const std::string &name, Matrix &xdt, Matrix &x);
    void differentiate( const std::string &name, valarray<double> &xdt, const valarray<double> &x);

    /** Perform a single step of integration.
     * @param time is current value of time.
     * @param dt is integration time step.
     * \return The integration time step used is returned.  For adaptive
     * integrators, this need not match the requested integration time step.
     */
    double step( const double &time, const double &dt);

    /** Cycle integration for a given duration.  For fixed-step integrators,
     * if the @ref duration is not evenly divisible by the integration time step,
     * then the last time step will be shortened to match the required
     * end time. Only positive durations (forward integration) are supported.
     *
     * @param time is current value of time.
     * @param duration is the time to cycle for.
     *
     */
    virtual void cycle( const double &time, const double &duration);

    /** Cycle integration for a single time step up to a maximum given duration.
     * For fixed-step integrators, if the @ref duration is not evenly divisible
     * by the integration time step, then the last time step will be shortened to
     * match the required end time. Only positive durations (forward integration)
     * are supported.
     *
     * @param time is current value of time.
     * @param maximumDuration is the maximum single time step.
     *
     */
    virtual double cycleForSingleStep( const double &time_, const double &maximumDuration);

    /** Resets the integrator to its initial state.
     */
    void resetIntegrator();

    /** Sets the internal time interval for numerical integration.
     * This represents the calculation interval that is used internally within
     * the model to update the aircraft's state vector. It may differ from the
     * step that an external application uses when calling the \ref cycle
     * function. It must be smaller than or equal to the step used by an external
     * application. If a fixed step integrator is in use, it is recommended that
     * the external step is a whole multiple of the internal time step to ensure
     * optimum runtime performance of the aircraft model.  If an adaptive
     * integrator is in use, the next integration step after a call to this
     * function will be attempted using the supplied time step, but the integration
     * quality control process may vary the value during or after the time step.
     * @param integrationTimeStep is the requested internal model time step in
     * seconds.
     */
    void setIntegrationTimeStep( const double &integrationTimeStep);

    /** The time step used within the Integrator class may be less than the
     * cycle time step.  This function provides access to the internal time step.
     * \return The internal time step in use for numerical integration and
     * differentiation is returned, expressed in seconds.  For fixed time step
     * integration methods, this will return either the default value of 0.04
     * seconds or the value set by \ref setIntegrationTimeStep.  For adaptive
     * integration methods, it will return the current value of the time step.
     * This is not necessarily the same as the most recently completed time step
     * length.
     */
    const double& getIntegrationTimeStep() const { return integrationTimeStep_; }

    /** Adaptive integrations may result in very small timesteps for discontinuous,
     * stiff or poorly-behaved plants. This function allows a direct constraint
     * on contractive adaptation to be applied. Use of the absolute and relative
     * tolerances is preferred to this function, as a minimum step size may result
     * in unstable behaviour.
     * \param integrationTimeStepMin, expressed in seconds, is the lower limit
     * of adaptive step size which will be allowed during the integration.  It
     * may be changed at any point during a simulation, and the changed value
     * will then apply for the remainder of the simulation unless changed again.
     * The default value is 1.0e3*DBL_EPSILON sec.
     */
    void setIntegrationTimeStepMin( const double& integrationTimeStepMin )
    { integrationTimeStepMin_ = integrationTimeStepMin; }

    /** Adaptive integrations may result in very long timesteps for smooth,
     * non-stiff, well-behaved plants.  This function allows a direct constraint
     * on expansive adaptation to be applied.
     * \param integrationTimeStepMax, expressed in seconds, is the upper limit
     * of adaptive step size which will be allowed during the integration.  It
     * may be changed at any point during a simulation, and the changed value
     * will then apply for the remainder of the simulation unless changed again.
     * The default value is 1.0e206 sec.
     */
    void setIntegrationTimeStepMax( const double& integrationTimeStepMax )
    { integrationTimeStepMax_ = integrationTimeStepMax; }

    /** Provides access to the minimum time step currently specified for
     * numerical integration computations.  If none has been explictly specified
     * using \ref setIntegrationTimeStepMin, or if a non-adaptive integration
     * method is in use, the default value of 1.0e3*DBL_EPSILON will be returned.
     * \return The minimum time step currently in use for numerical
     * integration computations.
     */
    const double& getIntegrationTimeStepMin() const
    { return integrationTimeStepMin_; }

    /** Provides access to the maximum time step currently specified for
     * numerical integration computations.  If none has been explictly specified
     * using \ref setIntegrationTimeStepMax, or if a non-adaptive integration
     * method is in use, the default value of 1.0e206 will be returned.
     * \return The maximum time step currently in use for numerical
     * integration computations.
     */
    const double& getIntegrationTimeStepMax() const
    { return integrationTimeStepMax_; }

    /** Sets adaptive integration convergence criterion.
     * The adaptive integration methods uses a test for convergence
     * based on specified maximum relative and absolute errors.  If the maximum
     * integration error estimate for all elements of the state vector is within
     * the specified tolerance, the integrated values are accepted and the
     * integration time step is unchanged.  If the error estimate of any state
     * vector element is greater than the specified tolerance, the step size
     * is reduced and the step is repeated.  If the worst error estimate of all
     * state vector elements is much smaller than the tolerance, the integrated
     * values are accepted and the step size is increased prior to the next
     * integration step.  The default relative tolerance is 1.0e-4, equivalent
     * to parameter accuracy of three significant figures.  The default absolute
     * tolerance is 1.0e-4.  The relative tolerance is frequently the more
     * important parameter for flight dynamics applications.  This function allows
     * the tolerances to be changed at any time during a simulation.
     * \param toleranceRel is the maximum relative error estimate allowed for
     * all state vector elements when integrating using the adaptive integration
     * schemes.
     * \param toleranceAbs is the maximum absolute error estimate allowed for
     * all state vector elements when integrating using the adaptive integration
     * schemes.
     */
    void setIntegrationTolerance( const double& toleranceRel,
                                  const double& toleranceAbs )
    { toleranceRel_ = toleranceRel; toleranceAbs_ = toleranceAbs; }

    /** When one of the adaptive integrators is in use, the error in each
     * state vector element varies as the integration proceeds.  The combined
     * error at each timestep is represented by its infinity norm, the
     * maximum value of the state error estimate vector elements, with each
     * error normalised by the maximum of the absolute and relative tolerances
     * currently in force on that state vector component.
     * \return The current value of the infinity norm of the adaptive integration
     * error is returned.  Note that this is a relative error.  If a non-adaptive
     * integration is in use, zero is returned.
     * \sa setIntegrationTolerance
     */
    const double& getIntegrationError() const { return errorNorm_; }

    /** Real-Time clock setting.
     * Enable / Disable real-time clock integration cycle.
     */
    void setRealTimeCycle( const bool isRealTimeCycleArg)
    { isRealTimeCycle_ = isRealTimeCycleArg;}

    /**
     * Return the state of the real-time clock.
     */
    const bool& isRealTimeCycle() const { return isRealTimeCycle_;}

    /** Terminate the integrator at an appropriate time using a throw.
     * Useful for graceful termination when running threaded systems.
     * If you use this, make sure to reset simulation with terminate( false).
     */
    void terminate( bool doTerminate = true) { terminatedByUser_ = doTerminate;}

   protected:
    /** Function to call prior to each integration step call.
     * Overload this function to evaluate items outside the integration step.
     */
    virtual void preStepExecutive( const double& /* time */, const double& /* dt */) {}

    /** Function to call after each integration step call.
     * Overload this function to evaluate items outside the integration step.
     */
    virtual void postStepExecutive( const double& /* time */, const double& /* dt */) {}

    /** Function containing all required processes to compute derivatives.
     * This is the derivative function called by the Integrator integrators.
     * Override this virtual function when deriving a model.
     * The function that overrides this one should contain the calculations
     * of the derivatives and calls to the 'Integrate()' function, defined
     * in Integrator.  The parameter list of this function should not be
     * changed, so any inputs and outputs to the model's derivative
     * calculations should be done through private data members in the
     * component's class definition.
     * The purpose of the calls to Integrate() is to provide the integrator
     * with the derivatives to be integrated and the storage position of the
     * integrands.
     */
    virtual void derivativeExecutive( const double &t) = 0;

    virtual void onFailedAdaptiveStep() {}

    /**
     * Returns true during first integration pass, otherwise false.
     */
    bool isFirstPass() { return firstPass_;}

    /** sets the flag to initialise the differentiation variables.
     * @param initialiseizeDerivative is a boolean, either 'true' or 'false'.
     */
    void setInitializeDerivative( const bool initializeDerivative)
    { initializeDerivative_ = initializeDerivative;}

    /**
     * Returns true if initializing derivative procedures, otherwise false.
     */
    bool isInitializeDerivative() { return initializeDerivative_;}

    /**
     */
    DMatrix getDerivativeHistory();

    /**
     */
    bool setDerivativeHistory( DMatrix derivativeHistory);

    /** Get state and state derivative values.
     */
    void getState( std::vector<double> &xdt, std::vector<double> &x);

    /** Set state values.
     */
    void setState( std::vector<double> &x);

   private:
    /**
     * Resets the state list to zero, ready for next integration pass.
     */
    void resetStateList();

    /** Generic Embedded Runge Kutta integrator.
     */
    double integrateEmbeddedRungeKutta( const double& t, const double& dt,
      const double* a, const double* b,
      const double* c, const double* bDiff,
      const size_t  n, const double& order,
      const bool fsal);

    /** Generic Runge Kutta integrator.
     * Performs single step of any integrator in Runge-Kutta predictor-corrector
     * form.  This function is called by all of the specific Runge-Kutta
     * integrator functions, fixed-step or adaptive. It only performs fixed steps,
     * with any required adaptive procedures performed in the calling function.
     * Inputs essentially follow the structure of a Butcher tableau.
     * \param t is the integration starting time.
     * \param dt is the required integration time step.
     * \param a is an array in row-major sequence of the weights for successive
     * predictor steps.  There is always an implicit first row of all zeros and
     * last column of all zeros.  The row-wise sum of the a weights should
     * always equal the corresponding c element.
     * \param b is an array in successive derivative evaluation order of
     * n weights for the final corrector step.
     * \param c is an array in successive derivative evaluation order of time
     * step fractions at which the derivatives of the state vector components
     * are to be evaluated during the predictor steps.  An implicit first time
     * step fraction is always zero.
     * \param n is the number of derivative evaluations required.
     */
    void integrateRungeKutta( const double& t, const double& dt,
                              const double* a, const double* b,
                              const double* c, const size_t n  );

    /** Adams-Bashforth integrator.
     * Performs single step of Adams-Bashforth fifth order integration using two
     * function evaluations. Three previous values of the state derivatives are
     * required. Runge Kutta 4 will be called to start the algorithm, if required.
     * @param t is the current time.
     * @param dt is the integration time step.
     */
    void integrateAdamsBashforth( const double &t, const double &dt);

    /** Euler integrator.
     * Performs single step of Euler's Method integration.
     * @param t is the current time.
     * @param dt is the integration time step.
     */
    void integrateEuler( const double &t, const double &dt);

    /** Directs computation of the derivative using selected method.
     */
    void differentiation();

    /** Two point Differentiation - Backwards Difference.
     * Performs a two-point backwards differentiation
     */
    void derivativeTwo();

    /** Three point Differentiation - Backwards Difference with varying
     * time step size.
     */
    void derivativeThree();

    /** Four point Differentiation - Backwards Difference with varying time
     * step size.
     */
    void derivativeFour();

    /** Sets the historical derivative data.
     * Stores the state values of current and past evaluations in a vector
     * for future computations.
     * @param i is the index to the derivative variable in derivList_.
     */
    void setDerivativeHistory( const size_t i);

    /**
     * TStateVariable is the element of the state variable
     * array which contains data for each state variable.
     */
    struct StateVariable
    {
      std::string name;             // Variable name.
      double *xptr;                 // Pointer to model state variable.
      double  xdt;                  // Current derivative value.
      double  xdtl,xdtll,xdtlll;    // Previous three derivative values.
      double  xn;                   // Runge-Kutta initial value.
      double  k[ 13 ];               // Runge-Kutta intermediate values.
      double  *yerror;              // estimate of error - RK45.
    };

    struct DerivVariable
    {
      DerivVariable() : xp(FOUR_POINT - 1) {}
      std::string name;             // Variable name.
      double *xdtptr;               // Pointer to the model derivative variable.
      double  x;                    // Current state value.
      std::vector<double> xp;       // Previous state values.
    };
    std::vector<double> timeList_;  // List of past integration times

    bool integrate_;                // True during integration passes of derivativeExecutive().
    bool derivative_;               // True during differentiation passes of derivativeExecutive().
    bool firstPass_;                // True during first integration pass.
    bool initializeDerivative_;     // True to initialise the derivative procedures.
    bool isRealTimeCycle_;          // True to integrate cycle in real-time.
    bool isLastCycleStep_;          // True when current integration step completes the cycle.

    size_t   nStateVars_;           // Current number of state variables defined in derivativeExecutive().
    size_t   nDerivVars_;           // Current number of derivative variables defined in derivativeExecutive().
    size_t   nDerivPoint_;          // Number of previous values used in derivative calculations.
    std::vector<StateVariable> stateList_; // List of state variable data.
    std::vector<DerivVariable> derivList_; // List of derivative variable data.

    IntegratorMethod integratorMethod_;
    DerivativeMethod derivativeMethod_;
    AdaptMethod      adaptMethod_;
    double integrationTimeStep_;
    double initialIntegrationTimeStep_; // Used for resetting the integrator
    double integrationTimeStepMin_;
    double integrationTimeStepMax_;
    double toleranceRel_;           // adaptive relative error tolerance
    double toleranceAbs_;           // adaptive absolute error tolerance
    double errorNorm_;              // adaptive relative error norm
    double errorRatioLast_;
    double stepFactorMax_;
    double stepFactorMin_;
    double stepFactorWgt_;
    double stepFactorAccept_;
    // double stepFactorExpand_;
    bool isFsal_;                   // first same as last (FSAL) flag

    size_t count_;

    dstoute::aRealTimeClock realTimeClock_;

    bool terminatedByUser_;

  };

} // end namespace dstomath

#endif /* _AEALTIMECLOCK_H_ */
