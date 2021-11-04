#ifndef _AUNITS_H_
#define _AUNITS_H_

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
// Title:      Runtime Units Conversion & Compatibility Class
// Class:      aUnits
//
// Description:
//   This class allows units to be converted and manipulated at runtime.
//
// Written by: Shane Hill  (Shane.Hill@dsto.defence.gov.au)
//
// First Date: 21/02/2013
//
// References:
//   Based on some of the concepts from:
//     Calum Grant:
//       Home page: http://calumgrant.net/units
//       File location: http://calumgrant.net/units/units.hpp
//
//     Abnegator
//       http://learningcppisfun.blogspot.com.au/2007/01/units-and-dimensions-with-c-templates.html
//

// C++ Includes
#include <string>

// Ute Includes
#include "aString.h"
#include "aMap.h"
#include "aList.h"
#include "aOptional.h"

namespace dstoute {

  enum BaseDimensions {
    BASE_LENGTH,
    BASE_MASS,
    BASE_TEMP,
    BASE_CURRENT,
    BASE_MOLE,
    BASE_LIGHT,
    BASE_ANGLE, // This is a workaround to prevent rad being convertible to and from ND, which, whilst technically correct, is almost always wrong within aUnits
    BASE_TIME,
    BASE_SIZE
  };

  class aUnits {
   public:
    aUnits();
    aUnits( const aUnits &other);
    aUnits( const aUnits &other, const double &valueArg);
    aUnits( const aString &name, const int baseDim[], const double &scaleArg = 1, const double &offsetArg = 0);
    aUnits( const aString &name, const aUnits &other);
    aUnits( const aString &name);

    const aString& units() const         { return units_;}
    const double& value() const         { return value_;}
    const double& scale() const         { return scale_;}
    const double& offset() const        { return offset_;}
    const double& scaleToMetric() const { return scaleMetric_;}
    int     baseDim( int i) const { return baseDim_[ i];}

    double valueSI() const        { return ( value_ - offset_) * scale_;}
    double valueMetric() const    { return ( value_ - offset_) * scale_ * scaleMetric_;}

    aString unitsSI() const;
    aString unitsMetric() const;

    void assign( const aUnits &other) { if ( &other != this) copy( other);}

    void setValue( const double &valueArg)         { value_  = valueArg;}
    //void setUnitString( const aString &unitsArg)   { units_  = unitsArg;}
    void setScale( const double &scaleArg)         { scale_  = scaleArg;}
    void setOffset( const double &offsetArg)       { offset_ = offsetArg;}
    void setScaleToMetric( const double &scaleArg) { scaleMetric_ = scaleArg;}

    void setValueSI( const double &valueArg)     { value_ = ( valueArg / scale_) + offset_;}
    void setValueMetric( const double &valueArg) { value_ = valueArg / ( scale_ * scaleMetric_) + offset_;}

    void setValue( const aUnits &valueArg);
    void setValue( const aUnits &valueArg, const aUnits &convUnit);
    void setValue( const aUnits &valueArg, const aUnits &convUnit1, const aUnits &convUnit2);

    aUnits &setUnits( const aUnits  &unitsArg);
    aUnits &setUnits( const aString &unitsArg);
    aUnits &setUnits( const aUnits  &unitsArg, const aUnits &convUnit);
    aUnits &setUnits( const aUnits  &unitsArg, const aUnits &convUnit1, const aUnits &convUnit2);

    void clear();
    bool isEmpty() const { return isEmpty_;}
    bool isCompatible( const aUnits &other, bool doThrow = true) const;
    bool isScalar() const;

    aUnits& operator=( const aUnits &other);
    aUnits& operator=( const double & valueArg);
    aUnits& operator+=( const aUnits &other);
    aUnits& operator-=( const aUnits &other);
    aUnits& operator*=( const double &valueArg);
    aUnits& operator/=( const double &valueArg);

    aUnits operator+( const aUnits &other) const;
    aUnits operator-( const aUnits &other) const;
    aUnits operator*( const aUnits &other) const;
    aUnits operator*( const double &valueArg) const;
    aUnits operator/( const aUnits &other) const;
    aUnits operator/( const double &valueArg) const;

    friend bool   isValidUnit( const aString& unit);
    friend bool   isCompatible( const aUnits &fromUnits, const aUnits &toUnits);
    friend double convert( const aUnits &fromUnits, const aUnits &toUnits);
    friend double convertFromSI( const aUnits& toUnits, const double& val);
    friend double convert( const aUnits &fromUnits, const aUnits &toUnits, const double& value);
    friend double convertUsing( const aUnits &fromUnits, const aUnits &toUnits, const aUnits &convUnit);
    friend double convertUsing( const aUnits &fromUnits, const aUnits &toUnits, const aUnits &convUnit1, const aUnits &convUnit2);
    friend double deltaConvert( const aUnits &fromUnits, const aUnits &toUnits);
    friend double deltaConvert( const aUnits &fromUnits, const aUnits &toUnits, const double& value);
    friend aUnits scale( const aUnits &units, const double &num, const double &den);
    friend aUnits scaleSI( const aUnits &units, const double &num, const double &den);
    friend aUnits translate( const aUnits &units, const double &num, const double &den);
    friend aUnits pow( const aUnits &units, const int num);
    friend aUnits compose( const aUnits &units1, const aUnits &units2);
    friend const aUnits& findUnits( const aString &units);
    friend std::ostream& operator<< ( std::ostream &os, const aUnits &units);

   private:
    void copy( const aUnits &other);
    double convert( const aUnits &other) const;

    aString units_;
    double value_;
    double scale_;
    double offset_;
    double scaleMetric_;
    int    baseDim_[ BASE_SIZE];
    bool   isEmpty_;
  };

  typedef aList<aUnits> aUnitsList;

  bool   isValidUnit( const aString& unit);
  bool   isCompatible( const aUnits &fromUnits, const aUnits &toUnits);
  double convert( const aUnits &fromUnits, const aUnits &toUnits);
  double convert( const aUnits &fromUnits, const aUnits &toUnits, const double& value);
  double convertFromSI( const aUnits& toUnits, const double& val);
  double convertUsing( const aUnits &fromUnits, const aUnits &toUnits, const aUnits &convUnit);
  double convertUsing( const aUnits &fromUnits, const aUnits &toUnits, const aUnits &convUnit1, const aUnits &convUnit2);
  double deltaConvert( const aUnits &fromUnits, const aUnits &toUnits);
  double deltaConvert( const aUnits &fromUnits, const aUnits &toUnits, const double& value);
  aUnits scale( const aUnits &units, const double &num, const double &den = 1);
  aUnits scaleSI( const aUnits &units, const double &num, const double &den = 1);
  aUnits translate( const aUnits &units, const double &num, const double &den = 1);
  aUnits pow( const aUnits &units, const int num);
  aUnits compose( const aUnits &units1, const aUnits &units2);
  const aUnits& findUnits( const aString &units);
  const aUnits& addUnits( const aString &name, const aUnits &units);
  aUnitsList findCompatibleUnits( const aString &units);
  aUnitsList findCompatibleUnits( const aUnits &units);
  std::ostream& operator<< ( std::ostream &os, const aUnits &units);
  aUnits& operator<< ( aUnits &os, const aUnits &units);

  namespace predefined_aunits {
    //
    // Unit prefix factors.
    //
    extern const size_t Max_Prefix_Length;
    extern const aUnits prefix_base;
    extern const aUnits prefix_deka;
    extern const aUnits prefix_hecto;
    extern const aUnits prefix_kilo;
    extern const aUnits prefix_mega;
    extern const aUnits prefix_giga;
    extern const aUnits prefix_tera;
    extern const aUnits prefix_peta;
    extern const aUnits prefix_exa;
    extern const aUnits prefix_zetta;
    extern const aUnits prefix_yotta;

    extern const aUnits prefix_deci;
    extern const aUnits prefix_centi;
    extern const aUnits prefix_milli;
    extern const aUnits prefix_micro;
    extern const aUnits prefix_nano;
    extern const aUnits prefix_pico;
    extern const aUnits prefix_femto;
    extern const aUnits prefix_atto;
    extern const aUnits prefix_zepto;
    extern const aUnits prefix_yocto;

    //
    // Define the base unit types.
    //
    //                           L  M  T  K  A  M  C
    extern const char *base_units[];
    extern const int base_none[];
    extern const int base_length[];
    extern const int base_mass[];
    extern const int base_temp[];
    extern const int base_current[];
    extern const int base_mol[];
    extern const int base_lux[];
    extern const int base_angle[]; // Technically not a dimension, but this prevents ND being convertible to and from rad, which is almost always a mistake in aUnits
    extern const int base_time[];

    //
    // Define some units.
    //
    extern const aUnits u_none;

    // Base Units
    extern const aUnits u_m;
    extern const aUnits u_kg;
    extern const aUnits u_s;
    extern const aUnits u_degK;
    extern const aUnits u_A;
    extern const aUnits u_mol;
    extern const aUnits u_cd;
    extern const aUnits u_rad; // Technically derived, but is treated as a base unit within aUnits

    // Derived Units
    // extern const aUnits u_rad;
    extern const aUnits u_Hz;
    extern const aUnits u_N;
    extern const aUnits u_Pa;
    extern const aUnits u_J;
    extern const aUnits u_W;
    extern const aUnits u_C;
    extern const aUnits u_V;
    extern const aUnits u_F;
    extern const aUnits u_Ohm;
    extern const aUnits u_H;
    extern const aUnits u_S;
    extern const aUnits u_Wb;
    extern const aUnits u_T;
    extern const aUnits u_lm;
    extern const aUnits u_lx;
    extern const aUnits u_Bq;
    extern const aUnits u_Gy;
    extern const aUnits u_Sv;
    extern const aUnits u_kat;

    // Constants
    extern const aUnits c_G;

    // Length
    extern const aUnits u_cm;
    extern const aUnits u_km;
    extern const aUnits u_in;
    extern const aUnits u_ft;
    extern const aUnits u_yd;
    extern const aUnits u_smi;
    extern const aUnits u_nmi;
    extern const aUnits u_furlong;

    // Area
    extern const aUnits u_m2;
    extern const aUnits u_ft2;
    extern const aUnits u_ha;
    extern const aUnits u_are;
    extern const aUnits u_acre;

    // Volume
    extern const aUnits u_cm3;
    extern const aUnits u_in3;
    extern const aUnits u_ml;
    extern const aUnits u_l;
    extern const aUnits u_USgal;
    extern const aUnits u_UKgal;

    // Mass
    extern const aUnits u_g;
    extern const aUnits u_tonne;
    extern const aUnits u_slug;
    extern const aUnits u_lbm;
    extern const aUnits u_oz;
    extern const aUnits u_snail;
    extern const aUnits u_USton;
    extern const aUnits u_UKton;

    // Time
    extern const aUnits u_min;
    extern const aUnits u_hours;
    extern const aUnits u_days;
    extern const aUnits u_weeks;
    extern const aUnits u_fortnights;
    extern const aUnits u_years;
    extern const aUnits u_months;

    // Speed
    extern const aUnits u_mps;
    extern const aUnits u_kph;
    extern const aUnits u_ftps;
    extern const aUnits u_mph;
    extern const aUnits u_kn;

    // Temperature
    extern const aUnits u_degC;
    extern const aUnits u_degF;
    extern const aUnits u_degR;

    // Angles
    extern const aUnits u_deg;
    extern const aUnits u_grad;
    extern const aUnits u_rev;

    // Pressure
    extern const aUnits u_kPa;
    extern const aUnits u_psi;
    extern const aUnits u_psf;
    extern const aUnits u_bar;
    extern const aUnits u_mbar;
    extern const aUnits u_mmHg;
    extern const aUnits u_inHg;
    extern const aUnits u_atm;
    extern const aUnits u_inH2O; // inH2O @ a reference temperature of 20C

    // Force
    extern const aUnits u_lbf;

    // Power
    extern const aUnits u_hp;

    // Energy
    extern const aUnits u_cal;
    extern const aUnits u_btu; // British Thermal Units
    extern const aUnits u_erg;

    // Acceleration
    extern const aUnits u_mps2;
    extern const aUnits u_ftps2;
    extern const aUnits u_knps;

    // Angular velocity
    extern const aUnits u_rpm;
    extern const aUnits u_radps;
    extern const aUnits u_degps;

    //
    // Predefined unit conversion functions
    //
    // Angles
    inline double deg2rad( const double &value = 1) { return convert( u_deg, u_rad, value);}
    inline double rad2deg( const double &value = 1) { return convert( u_rad, u_deg, value);}
    inline double degps2radps( const double &value = 1) { return convert( u_degps, u_radps, value);}
    inline double radps2degps( const double &value = 1) { return convert( u_radps, u_degps, value);}

    // Length
    inline double m2ft( const double &value = 1) { return convert( u_m, u_ft, value);}
    inline double ft2m( const double &value = 1) { return convert( u_ft, u_m, value);}
    inline double m2nmi( const double &value = 1) { return convert( u_m, u_nmi, value);}
    inline double nmi2m( const double &value = 1) { return convert( u_nmi, u_m, value);}

    // Velocity
    inline double mps2kn( const double &value = 1) { return convert( u_mps, u_kn, value);}
    inline double kn2mps( const double &value = 1) { return convert( u_kn, u_mps, value);}
    inline double mps2ftps( const double &value = 1) { return convert( u_mps, u_ftps, value);}
    inline double ftps2mps( const double &value = 1) { return convert( u_ftps, u_mps, value);}

    // Force
    inline double N2lbf( const double &value = 1) { return convert( u_N, u_lbf, value);}
    inline double lbf2N( const double &value = 1) { return convert( u_lbf, u_N, value);}

  } // End namespace predefined_aunits.

  namespace aunits_cache {

    typedef dstoute::aMap< aString, aUnits> UnitsMap;

    extern const UnitsMap unitsPrefixMapCache;
    extern UnitsMap unitsMapCache;

  } // End namespace aunits_cache.

  // aOptional support for aUnits
  template<>
  struct aOptionalValidator<aUnits>
  {
    static bool isValid( const aUnits& v)
    {
      return v.value() != std::numeric_limits<double>::max();
    }
    static aUnits invalidValue()
    {
      aUnits cache;
      cache.setValue( std::numeric_limits<double>::max());
      return cache;
    }
    static void makeInvalid( aUnits& v)
    {
      v.setValue( std::numeric_limits<double>::max());
    }
  };

  typedef aOptional<aUnits> aOptionalUnits;

} // End napespace dstoute.

#endif // _AUNITS_H_
