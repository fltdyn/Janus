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
//   SI Units: https://en.wikipedia.org/wiki/International_System_of_Units
//   Derived SI Units: https://en.wikipedia.org/wiki/SI_derived_unit
//
//   Based on some of the concepts from:
//     Calum Grant:
//       Home page: http://calumgrant.net/units
//       File location: http://calumgrant.net/units/units.hpp
//
//     Abnegator
//       http://learningcppisfun.blogspot.com.au/2007/01/units-and-dimensions-with-c-templates.html
//

// Ute Includes
#include "aUnits.h"
#include "aMutex.h"
#include "aMessageStream.h"

// C++ Includes
#include <iostream>
#include <cstdlib>
#include <cmath>

using namespace std;

namespace dstoute {

  namespace predefined_aunits {
    //
    // Define the base unit types.
    //
    //                           L  M  T  K  A  M  C
    const char *base_units[] = { "m", "kg", "K", "A", "mol", "cd", "rad", "s"};
    const int base_none[]    = { 0, 0, 0, 0, 0, 0, 0, 0};
    const int base_length[]  = { 1, 0, 0, 0, 0, 0, 0, 0};
    const int base_mass[]    = { 0, 1, 0, 0, 0, 0, 0, 0};
    const int base_temp[]    = { 0, 0, 1, 0, 0, 0, 0, 0};
    const int base_current[] = { 0, 0, 0, 1, 0, 0, 0, 0};
    const int base_mol[]     = { 0, 0, 0, 0, 1, 0, 0, 0};
    const int base_lux[]     = { 0, 0, 0, 0, 0, 1, 0, 0};
    const int base_angle[]   = { 0, 0, 0, 0, 0, 0, 1, 0}; // Technically not a dimension, but this prevents ND being convertible to and from rad, which is almost always a mistake in aUnits
    const int base_time[]    = { 0, 0, 0, 0, 0, 0, 0, 1};

    //
    // Unit prefix factors.
    //
    const size_t Max_Prefix_Length = 2;
    const aUnits prefix_base(   "",  base_none); // A prefix with no base values.;
    const aUnits prefix_deka( "da",  scale( prefix_base,  10));
    const aUnits prefix_hecto( "h",  scale( prefix_base,  100));
    const aUnits prefix_kilo(  "k",  scale( prefix_base,  1000));
    const aUnits prefix_mega(  "M",  scale( prefix_kilo,  1000));
    const aUnits prefix_giga(  "G",  scale( prefix_mega,  1000));
    const aUnits prefix_tera(  "T",  scale( prefix_giga,  1000));
    const aUnits prefix_peta(  "P",  scale( prefix_tera,  1000));
    const aUnits prefix_exa(   "E",  scale( prefix_peta,  1000));
    const aUnits prefix_zetta( "Z",  scale( prefix_exa,   1000));
    const aUnits prefix_yotta( "Y",  scale( prefix_zetta, 1000));

    const aUnits prefix_deci(  "d",  scale( prefix_base,  1, 10));
    const aUnits prefix_centi( "c",  scale( prefix_base,  1, 100));
    const aUnits prefix_milli( "m",  scale( prefix_base,  1, 1000));
    const aUnits prefix_micro( "u",  scale( prefix_milli, 1, 1000));
    const aUnits prefix_nano(  "n",  scale( prefix_micro, 1, 1000));
    const aUnits prefix_pico(  "p",  scale( prefix_nano,  1, 1000));
    const aUnits prefix_femto( "f",  scale( prefix_pico,  1, 1000));
    const aUnits prefix_atto(  "a",  scale( prefix_femto, 1, 1000));
    const aUnits prefix_zepto( "z",  scale( prefix_atto,  1, 1000));
    const aUnits prefix_yocto( "y",  scale( prefix_zepto, 1, 1000));

    //
    // Define some units.
    //
    const aUnits u_none( "",    base_none);    // A unit with no base values.

    // Base Units
    // http://en.wikipedia.org/wiki/International_System_of_Units
    const aUnits u_m(    "m",   base_length);  // Metre (length)
    const aUnits u_kg(   "kg",  base_mass);    // Kilogram (mass)
    const aUnits u_s(    "s",   base_time);    // Second (time)
    const aUnits u_degK( "K",   base_temp);    // Kelvin (temperature)
    const aUnits u_A(    "A",   base_current); // Ampere (electric current)
    const aUnits u_mol(  "mol", base_mol);     // Mole (amount of substance)
    const aUnits u_cd(   "cd",  base_lux);     // Candela (luminous intensity)
    const aUnits u_rad( "rad",  base_angle);   // Radian (angle) // Technically derived, but is treated as a base unit within aUnits

    // Derived Units
    // http://en.wikipedia.org/wiki/International_System_of_Units
    //const aUnits u_rad( "rad",   u_none);                                       // Radian (angle)
    const aUnits u_Hz(  "Hz",    pow( u_s, -1));                                // Hertz (frequency)
    const aUnits u_N(   "N",     compose( u_m, compose( u_kg, pow( u_s, -2)))); // Newton (force, weight)
    const aUnits u_Pa(  "Pa",    compose( u_N, pow( u_m, -2)));                 // Pascal (pressure, stress)
    const aUnits u_J(   "J",     compose( u_N, u_m));                           // Joule (energy, work, heat)
    const aUnits u_W(   "W",     compose( u_J, pow( u_s, -1)));                 // Watt (power, radiant flux)
    const aUnits u_C(   "C",     compose( u_s, u_A));                           // Coulomb (electrical charge) potential conflict with degC.
    const aUnits u_V(   "V",     compose( u_W, pow( u_A, -1)));                 // Volt (voltage)
    const aUnits u_F(   "Farad", compose( u_C, pow( u_V, -1)));                 // Farad (electric capacitance)
    const aUnits u_Ohm( "Ohm",   compose( u_V, pow( u_A, -1)));                 // Ohm (electric resistance)
    const aUnits u_H(   "H",     compose( u_Ohm, u_s));                         // Henry (inductance)
    const aUnits u_S(   "S",     compose( u_A, pow( u_V, -1)));                 // Siemens (electrical conductance)
    const aUnits u_Wb(  "Wb",    compose( u_V, u_s));                           // Weber (magnetic flux)
    const aUnits u_T(   "T",     compose( u_Wb, pow( u_m, -2)));                // Tesla (magnetic field strength)
    const aUnits u_lm(  "lm",    u_cd);                                         // Lumen (luminous flux)
    const aUnits u_lx(  "lx",    compose( u_lm, pow( u_m, -2)));                // Lux (illuminance)
    const aUnits u_Bq(  "Bq",    pow( u_s, -1));                                // Becquerel (radioactivity)
    const aUnits u_Gy(  "Gy",    compose( u_J, pow( u_kg, -1)));                // Gray (absorbed dose)
    const aUnits u_Sv(  "Sv",    u_Gy);                                         // Sievert (equivalent dose)
    const aUnits u_kat( "kat",   compose( pow( u_s, -1), u_mol));               // Katal (catalytic activity)

    // Constants
    const aUnits c_G(    "G",   scale( compose( u_m, pow( u_s, -2)), 9.80665)); // Earth's gravity constant

    // Length
    const aUnits u_cm(  "cm",   scale( u_m, 1, 100));                        // Centimetre
    const aUnits u_km(  "km",   scale( u_m, 1000));                          // Kilometre
    const aUnits u_in(  "in",   scale( u_cm, 254, 100));                     // Inch
    const aUnits u_ft(  "ft",   scale( u_in, 12));                           // Feet
    const aUnits u_yd(  "yd",   scale( u_in, 36));                           // Yard
    const aUnits u_smi( "smi",  scale( u_yd, 1760));                         // Statute mile
    const aUnits u_nmi( "nmi",  scale( u_m,  1852));                         // Nautical mile
    const aUnits u_furlong( "furlong", scale( u_smi, 1, 8)) ;                // Furlong
    const aUnits u_lightyear( "light-year", scale( u_m, 9460730472580800.)); // Light-year
    const aUnits u_rod(   "rod",   scale( u_ft, 16.5));                      // Rod
    const aUnits u_chain( "chain", scale( u_ft, 66));                        // Chain

    // Area
    const aUnits u_m2(   "m2",    pow( u_m, 2));                     // Metre squared
    const aUnits u_ft2(  "ft2",   pow( u_ft, 2));                    // Feet squared
    const aUnits u_ha(   "ha",    scale( u_m2, 10000));              // Hectare
    const aUnits u_are(  "are",   scale( u_m2, 100));                // Are
    const aUnits u_acre( "acres", scale( u_ha, 10000000, 24710538)); // Acre

    // Volume, length.
    const aUnits u_m3(    "m3",    pow( u_m,  3));                      // Cubic metre
    const aUnits u_cm3(   "cm3",   pow( u_cm, 3));                      // Cubic centimetre
    const aUnits u_in3(   "in3",   pow( u_in, 3));                      // Cubic inches

    // Volume, fluid.
    // Note scaleSI() use: Litre is the non-SI accepted SI unit for fluid volume. Yep, non-SI accepted SI unit.
    //                     1(m3) == 1000(l)
    const aUnits u_ml(    "ml",    scaleSI( u_cm3, 1000));              // Millilitre, scale SI m3 to Litre
    const aUnits u_l(     "l",     scale( u_ml, 1000));                 // Litre (non-SI, accepted as SI), u_ml already SI scaled
    const aUnits u_USgal( "USgal", scaleSI( scale( u_in3, 231), 1000)); // US gallon, scale SI m3 to Litre
    const aUnits u_UKgal( "UKgal", scale( u_l, 4546087, 1000000));      // UK gallon, u_l already SI scaled via u_ml

    // Beer Volume
    const aUnits u_shetland( "shetland", scale( u_ml, 115));             // https://en.wikipedia.org/wiki/Beer_in_Australia
    const aUnits u_foursie(  "foursie",  scale( u_ml, 115));
    const aUnits u_pony(     "pony",     scale( u_ml, 140));
    const aUnits u_horse(    "horse",    scale( u_ml, 140));
    const aUnits u_six(      "six",      scale( u_ml, 170));
    const aUnits u_bobbie(   "bobbie",   scale( u_ml, 170));
    const aUnits u_glass(    "glass",    scale( u_ml, 200));
    const aUnits u_seven(    "seven",    scale( u_ml, 200));
    const aUnits u_butcher(  "butcher",  scale( u_ml, 200));
    const aUnits u_middy(    "middy",    scale( u_ml, 285));
    const aUnits u_pot(      "pot",      scale( u_ml, 285));
    const aUnits u_ten(      "ten",      scale( u_ml, 285));
    const aUnits u_schmiddy( "schmiddy", scale( u_ml, 375));
    const aUnits u_schooner( "schooner", scale( u_ml, 425));
    const aUnits u_fifteen(  "fifteen",  scale( u_ml, 425));
    const aUnits u_pint(     "pint",     scale( u_ml, 570));
    const aUnits u_jug(      "jug",      scale( u_pint, 2));

    // Mass
    const aUnits u_g(     "g",     scale( u_kg,   1, 1000));          // Gram
    const aUnits u_tonne( "tonne", scale( u_kg,   1000));             // Tonne
    const aUnits u_lbm(   "lbm",   scale( u_kg,   0.45359237));       // Pound mass (exact conversion) https://en.wikipedia.org/wiki/Pound_(mass)
    const aUnits u_oz(    "oz",    scale( u_lbm,  1, 16));            // Ounce
    const aUnits u_USton( "USton", scale( u_lbm, 2000));              // US ton
    const aUnits u_UKton( "UKton", scale( u_lbm, 2240));              // UK ton

    // Force
    const aUnits u_lbf(  "lbf",    compose( u_lbm, c_G));             // Pound force (lbm * G)

    // Mass - Others based on u_lbf
    const aUnits u_slug(  "slug",  compose( u_lbf, compose( pow( u_s, 2), pow( u_ft, -1)))); // Slug
    const aUnits u_snail( "snail", scale( u_slug, 12));                                      // Snail

    // Time
    const aUnits u_min(   "min",     scale( u_s,     60));         // Minute
    const aUnits u_hours( "h",       scale( u_min,   60));         // Hour
    const aUnits u_days(  "days",    scale( u_hours, 24));         // Day
    const aUnits u_weeks( "weeks",   scale( u_days,  7));          // Week
    const aUnits u_fortnights( "fortnights", scale( u_days, 14));  // Fortnight
    const aUnits u_years(  "years",  scale( u_days, 365.25));      // Year
    const aUnits u_months( "months", scale( u_years, 1, 12));      // Month

    // Speed
    const aUnits u_mps(  "m s-1",   compose( u_m,   pow( u_s, -1)));     // Metres per second
    const aUnits u_kph(  "km h-1",  compose( u_km,  pow( u_hours, -1))); // Kilometres per hour
    const aUnits u_ftps( "ft s-1",  compose( u_ft,  pow( u_s, -1)));     // Feet per second
    const aUnits u_mph(  "smi h-1", compose( u_km,  pow( u_hours, -1))); // Statue miles per hour
    const aUnits u_kn(   "kn",      compose( u_nmi, pow( u_hours, -1))); // Knots (nautical miles per hour)

    // Temperature
    // Ref: http://en.wikipedia.org/wiki/Kelvin
    const aUnits u_degC( "C", translate( u_degK, -273.15));               // Celcius
    const aUnits u_degF( "F", translate( scale( u_degK, 5, 9), -459.67)); // Fahrenheit
    const aUnits u_degR( "R", scale( u_degK, 5, 9));                      // Rankine

    // Angles
    const aUnits u_deg(  "deg",  scale( u_rad, 0.017453292519943295769236907684886));      // Degree
    const aUnits u_grad( "grad", scale( u_rad, 3.1415926535897932384626433832795, 200.0)); // Gradian
    const aUnits u_rev(  "rev",  scale( u_deg, 360));                                      // Revolutions

    // Pressure
    const aUnits u_kPa(   "kPa",   scale( u_Pa,   1000));                // Kilopascal
    const aUnits u_psi(   "psi",   scale( u_kPa,  10000000, 1450377));   // Pounds per square inch
    const aUnits u_psf(   "psf",   scale( u_psi,  1, 144));              // Pounds per square foot
    const aUnits u_bar(   "bar",   scale( u_Pa,   100000));              // Bar
    const aUnits u_mbar(  "millibar", scale( u_bar, 1, 1000));           // Millibar
    const aUnits u_mmHg(  "mmHg",  scale( u_kPa,  101.3250144354, 760)); // Millimetres of mercury
    const aUnits u_inHg(  "inHg",  scale( u_mmHg, 254, 10));             // Inches of mercury
    const aUnits u_atm(   "atm",   scale( u_Pa,   101325));              // Atmosphere
    const aUnits u_inH2O( "inH2O", scale( u_Pa,   248.64536925));        // Inches of water at a reference temperature of 20C

    // Power
    const aUnits u_hp( "hp", scale( compose( u_ft, compose( u_lbf, pow( u_s, -1))), 550)); // Horse power

    // Energy
    const aUnits u_cal( "cal", scale( u_J, 4.1867456)); // Calorie
    const aUnits u_btu( "btu", scale( u_J, 1054.3507)); // British Thermal Units
    const aUnits u_erg( "erg", scale( u_J, 1.0E-07));   // Erg

    // Acceleration
    const aUnits u_mps2(  "m s-2",   compose( u_m,  pow( u_s, -2)));  // Metres per second squared
    const aUnits u_ftps2( "ft s-2",  compose( u_ft, pow( u_s, -2)));  // Feet per second squared
    const aUnits u_knps(  "kn s-1",  compose( u_kn, pow( u_s, -1)));  // Knots (nautical miles per hour) per second

    // Angular velocity
    const aUnits u_rpm(   "rpm",     compose( u_rev, pow( u_min, -1))); // Revolutions per minute
    const aUnits u_radps( "rad s-1", compose( u_rad, pow( u_s, -1)));   // Radians per second
    const aUnits u_degps( "deg s-1", compose( u_deg, pow( u_s, -1)));   // Degrees per second


  } // End namespace predefine_aunits.

  namespace aunits_cache {

    using namespace predefined_aunits;

    const UnitsMap unitsPrefixMapCache = ( UnitsMap()
      << UnitsMap::pair( prefix_deka.units(), prefix_deka)
      << UnitsMap::pair( prefix_hecto.units(), prefix_hecto)
      << UnitsMap::pair( prefix_kilo.units(), prefix_kilo)
      << UnitsMap::pair( prefix_mega.units(), prefix_mega)
      << UnitsMap::pair( prefix_giga.units(), prefix_giga)
      << UnitsMap::pair( prefix_tera.units(), prefix_tera)
      << UnitsMap::pair( prefix_peta.units(), prefix_peta)
      << UnitsMap::pair( prefix_exa.units(), prefix_exa)
      << UnitsMap::pair( prefix_zetta.units(), prefix_zetta)
      << UnitsMap::pair( prefix_yotta.units(), prefix_yotta)
      << UnitsMap::pair( prefix_deci.units(), prefix_deci)
      << UnitsMap::pair( prefix_centi.units(), prefix_centi)
      << UnitsMap::pair( prefix_milli.units(), prefix_milli)
      << UnitsMap::pair( prefix_micro.units(), prefix_micro)
      << UnitsMap::pair( prefix_nano.units(), prefix_nano)
      << UnitsMap::pair( prefix_pico.units(), prefix_pico)
      << UnitsMap::pair( prefix_femto.units(), prefix_femto)
      << UnitsMap::pair( prefix_atto.units(), prefix_atto)
      << UnitsMap::pair( prefix_zepto.units(), prefix_zepto)
      << UnitsMap::pair( prefix_yocto.units(), prefix_yocto)
    );

    // Define cache map of searchable units types.
    //   - Decomposed units are added to this cache at runtime.
    aMutex unitsMapCacheLocker;
    UnitsMap unitsMapCache = ( UnitsMap()
      // Base Units
      << UnitsMap::pair( "m",      u_m)   // Metres
      << UnitsMap::pair( "metre",  u_m)
      << UnitsMap::pair( "metres", u_m)
      << UnitsMap::pair( "kg",     u_kg)
      << UnitsMap::pair( "s",      u_s)
      << UnitsMap::pair( "K",      u_degK)
      << UnitsMap::pair( "A",      u_A)
      << UnitsMap::pair( "mol",    u_mol)
      << UnitsMap::pair( "cd",     u_cd)

      // Derived Units
      << UnitsMap::pair( "rad",    u_rad)
      << UnitsMap::pair( "Hz",     u_Hz)
      << UnitsMap::pair( "N",      u_N)
      << UnitsMap::pair( "Pa",     u_Pa)
      << UnitsMap::pair( "J",      u_J)
      << UnitsMap::pair( "W",      u_W)
      << UnitsMap::pair( "Coulomb",u_C)
      << UnitsMap::pair( "V",      u_V)
      // @TODO: Farad should be "F". Need to fix degF.
      << UnitsMap::pair( "Farad",  u_F)
      << UnitsMap::pair( "Ohm",    u_Ohm)
      << UnitsMap::pair( "H",      u_H)
      << UnitsMap::pair( "S",      u_S)
      << UnitsMap::pair( "Wb",     u_Wb)
      << UnitsMap::pair( "T",      u_T)
      << UnitsMap::pair( "lm",     u_lm)
      << UnitsMap::pair( "Bq",     u_Bq)
      << UnitsMap::pair( "Gy",     u_Gy)
      << UnitsMap::pair( "Sv",     u_Sv)
      << UnitsMap::pair( "kat",    u_kat)

      // Length
      << UnitsMap::pair( "cm",     u_cm)
      << UnitsMap::pair( "km",     u_km)
      << UnitsMap::pair( "ft",     u_ft)  // Feet
      << UnitsMap::pair( "foot",   u_ft)
      << UnitsMap::pair( "feet",   u_ft)
      << UnitsMap::pair( "in",     u_in)  // Inches
      << UnitsMap::pair( "inch",   u_in)
      << UnitsMap::pair( "inches", u_in)
      << UnitsMap::pair( "yd",     u_yd)  // Yards
      << UnitsMap::pair( "yard",   u_yd)
      << UnitsMap::pair( "yards",  u_yd)
      << UnitsMap::pair( "nmi",    u_nmi) // Nautical Miles
      << UnitsMap::pair( "nm",     u_nmi)
      << UnitsMap::pair( "smi",    u_smi) // Statute Miles
      << UnitsMap::pair( "mile",   u_smi)
      << UnitsMap::pair( "miles",  u_smi)
      << UnitsMap::pair( "furlong",u_furlong)
      << UnitsMap::pair( "light-year",u_lightyear)
      << UnitsMap::pair( "rod",    u_rod)
      << UnitsMap::pair( "chain",  u_chain)

      // Area
      << UnitsMap::pair( "m2",     u_m2)
      << UnitsMap::pair( "ft2",    u_ft2)
      << UnitsMap::pair( "ha",     u_ha)
      << UnitsMap::pair( "are",    u_are)
      << UnitsMap::pair( "acres",  u_acre)

      // Volume
      << UnitsMap::pair( "cm3",    u_cm3)
      << UnitsMap::pair( "in3",    u_in3)
      << UnitsMap::pair( "ml",     u_ml)
      << UnitsMap::pair( "l",      u_l)
      << UnitsMap::pair( "USgal",  u_USgal)
      << UnitsMap::pair( "UKgal",  u_UKgal)

      // Beer Volume
      << UnitsMap::pair( "shetland", u_shetland)
      << UnitsMap::pair( "foursie",  u_foursie)
      << UnitsMap::pair( "pony",     u_pony)
      << UnitsMap::pair( "horse",    u_horse)
      << UnitsMap::pair( "six",      u_six)
      << UnitsMap::pair( "bobbie",   u_bobbie)
      << UnitsMap::pair( "glass",    u_glass)
      << UnitsMap::pair( "seven",    u_seven)
      << UnitsMap::pair( "butcher",  u_butcher)
      << UnitsMap::pair( "middy",    u_middy)
      << UnitsMap::pair( "pot",      u_pot)
      << UnitsMap::pair( "ten",      u_ten)
      << UnitsMap::pair( "schmiddy", u_schmiddy)
      << UnitsMap::pair( "schooner", u_schooner)
      << UnitsMap::pair( "fifteen",  u_fifteen)
      << UnitsMap::pair( "pint",     u_pint)
      << UnitsMap::pair( "jug",      u_jug)

      // Mass
      << UnitsMap::pair( "g",      u_g)
      << UnitsMap::pair( "tonne",  u_tonne)
      << UnitsMap::pair( "slug",   u_slug)
      << UnitsMap::pair( "lbm",    u_lbm)
      << UnitsMap::pair( "lb",     u_lbm)
      << UnitsMap::pair( "oz",     u_oz)
      << UnitsMap::pair( "snail",  u_snail)
      << UnitsMap::pair( "blob",   u_snail)
      << UnitsMap::pair( "USton",  u_USton)
      << UnitsMap::pair( "UKton",  u_UKton)

      // Time
      << UnitsMap::pair( "\"",     u_s)
      << UnitsMap::pair( "min",    u_min)
      << UnitsMap::pair( "'",      u_min)
      << UnitsMap::pair( "h",      u_hours)
      << UnitsMap::pair( "hours",  u_hours)
      << UnitsMap::pair( "day",    u_days)
      << UnitsMap::pair( "days",   u_days)
      << UnitsMap::pair( "week",   u_weeks)
      << UnitsMap::pair( "weeks",  u_weeks)
      << UnitsMap::pair( "fortnight",  u_fortnights)
      << UnitsMap::pair( "fortnights", u_fortnights)
      << UnitsMap::pair( "year",   u_years)
      << UnitsMap::pair( "years",  u_years)
      << UnitsMap::pair( "month",  u_months)
      << UnitsMap::pair( "months", u_months)

      //Speed
      << UnitsMap::pair( "m s-1",   u_mps)
      << UnitsMap::pair( "m/s",     u_mps)
      << UnitsMap::pair( "km h-1",  u_kph)
      << UnitsMap::pair( "km/h",    u_kph)
      << UnitsMap::pair( "ft s-1",  u_ftps)
      << UnitsMap::pair( "ft/s",    u_ftps)
      << UnitsMap::pair( "smi h-1", u_mph)
      << UnitsMap::pair( "mph",     u_mph)
      << UnitsMap::pair( "kn",      u_kn)
      << UnitsMap::pair( "knots",   u_kn)

      // Temperature
      << UnitsMap::pair( "dgK",    u_degK)
      << UnitsMap::pair( "'K",     u_degK)
      << UnitsMap::pair( "°K",     u_degK)
      << UnitsMap::pair( "C",      u_degC)
      << UnitsMap::pair( "dgC",    u_degC)
      << UnitsMap::pair( "'C",     u_degC)
      << UnitsMap::pair( "°C",     u_degC)
      << UnitsMap::pair( "F",      u_degF)
      << UnitsMap::pair( "dgF",    u_degF)
      << UnitsMap::pair( "'F",     u_degF)
      << UnitsMap::pair( "°F",     u_degF)
      << UnitsMap::pair( "R",      u_degR)
      << UnitsMap::pair( "dgR",    u_degR)

      // Angles
      << UnitsMap::pair( "deg",    u_deg)
      << UnitsMap::pair( "grad",   u_grad)
      << UnitsMap::pair( "gra",    u_grad)
      << UnitsMap::pair( "rev",    u_rev)

      // Pressure
      << UnitsMap::pair( "kPa",    u_kPa)
      << UnitsMap::pair( "psi",    u_psi)
      << UnitsMap::pair( "psf",    u_psf)
      << UnitsMap::pair( "bar",    u_bar)
      << UnitsMap::pair( "millibar", u_mbar)
      << UnitsMap::pair( "mmHg",   u_mmHg)
      << UnitsMap::pair( "inHg",   u_inHg)
      << UnitsMap::pair( "atm",    u_atm)
      << UnitsMap::pair( "inH2O",  u_inH2O)

      // Force
      << UnitsMap::pair( "lbf",    u_lbf)

      // Power
      << UnitsMap::pair( "hp",     u_hp)

      // Energy
      << UnitsMap::pair( "cal",     u_cal)
      << UnitsMap::pair( "btu",     u_btu)
      << UnitsMap::pair( "erg",     u_erg)

      // Acceleration
      << UnitsMap::pair( "m s-2",   u_mps2)
      << UnitsMap::pair( "ft s-2",  u_ftps2)
      << UnitsMap::pair( "kn s-1",  u_knps)
      << UnitsMap::pair( "G",       c_G)

      // Angular velocity
      << UnitsMap::pair( "rpm",     u_rpm)
      << UnitsMap::pair( "rad s-1", u_radps)
      << UnitsMap::pair( "deg s-1", u_degps)

      // Other
      << UnitsMap::pair( "",        u_none)
      << UnitsMap::pair( "nd",      u_none)
      << UnitsMap::pair( "ND",      u_none)
    );

  } // End namespace aunits_cache

  aUnits::aUnits()
  : value_( 1),
    scale_( 1),
    offset_( 0),
    scaleMetric_( 1),
    isEmpty_( true)
  {
    for ( size_t i = 0; i < BASE_SIZE; ++i) {
      baseDim_[ i] = 0;
    }
  }

  aUnits::aUnits( const aUnits &other)
  {
    copy( other);
  }

  aUnits::aUnits( const aUnits &other, const double &valueArg)
  {
    copy( other);
    value_ = valueArg;
  }

  aUnits::aUnits( const aString &name, const int baseDim[], const double &scaleArg, const double &offsetArg)
  : units_( name),
    value_( 1),
    scale_( scaleArg),
    offset_( offsetArg),
    scaleMetric_( 1),
    isEmpty_( false)
  {
    for ( size_t i = 0; i < BASE_SIZE; ++i) {
      baseDim_[ i] = baseDim[ i];
    }
  }

  aUnits::aUnits( const aString &name, const aUnits &other)
  : units_( name),
    value_( other.value_),
    scale_( other.scale_),
    offset_( other.offset_),
    scaleMetric_( other.scaleMetric_),
    isEmpty_( other.isEmpty_)
  {
    for ( size_t i = 0; i < BASE_SIZE; ++i) {
      baseDim_[ i] = other.baseDim_[ i];
    }
  }

  aUnits::aUnits( const aString &name)
  {
    copy( findUnits( name));
  }

  void aUnits::setValue( const aUnits &valueArg)
  {
    if ( &valueArg != this) {
      if ( isEmpty_) {
        copy( valueArg);
      }
      else {
        isCompatible( valueArg);
        value_ = convert( valueArg);
      }
    }
  }

  void aUnits::setValue( const aUnits &valueArg, const aUnits &convUnit)
  {
    if ( &valueArg != this) {
      if ( isEmpty_) {
        copy( valueArg);
      }
      else {
        isCompatible( valueArg);
        value_ = convertUsing( valueArg, *this, convUnit);
      }
    }
  }

  void aUnits::setValue( const aUnits &valueArg, const aUnits &convUnit1, const aUnits &convUnit2)
  {
    if ( &valueArg != this) {
      if ( isEmpty_) {
        copy( valueArg);
      }
      else {
        isCompatible( valueArg);
        value_ = convertUsing( valueArg, *this, convUnit1, convUnit2);
      }
    }
  }

  aUnits &aUnits::setUnits( const aUnits &unitsArg)
  {
    double newValue = unitsArg.convert( *this);
    copy( unitsArg);
    setValue( newValue);
    return *this;
  }

  aUnits &aUnits::setUnits( const aString &unitsArg)
  {
    return setUnits( findUnits( unitsArg));
  }

  aUnits &aUnits::setUnits( const aUnits &unitsArg, const aUnits &convUnit)
  {
    double newValue = convertUsing( *this, unitsArg, convUnit);
    copy( unitsArg);
    setValue( newValue);
    return *this;
  }

  aUnits &aUnits::setUnits( const aUnits &unitsArg, const aUnits &convUnit1, const aUnits &convUnit2)
  {
    double newValue = convertUsing( *this, unitsArg, convUnit1, convUnit2);
    copy( unitsArg);
    setValue( newValue);
    return *this;
  }

  void aUnits::clear()
  {
    units_.clear();
    value_       = 1;
    scale_       = 1;
    offset_      = 0;
    scaleMetric_ = 1;
    isEmpty_     = true;
  }

  bool aUnits::isCompatible( const aUnits &other, bool doThrow) const
  {
    for ( size_t i = 0; i < BASE_SIZE; ++i) {
      if ( baseDim_[i] != other.baseDim_[i]) {
        if ( doThrow) {
          throw_message( runtime_error,
            setFunctionName( "aUnits::isCompatible()")
            << "\n - Units \"" << units_
            << "\" are not compatible with \""
            << other.units_ << "\".");
       }
        return false;
      }
    }
    return true;
  }

  bool aUnits::isScalar() const
  {
    for ( size_t i = 0; i < BASE_SIZE; ++i) {
      if ( baseDim_[i] != 0) {
        return false;
      }
    }
    return true;
  }

  aString aUnits::unitsSI() const
  {
    aString siUnits;
    for ( int i = 0; i < BASE_SIZE; ++i) {
      if ( baseDim_[ i]) {
        if ( !siUnits.empty()) siUnits += ' ';
        siUnits += predefined_aunits::base_units[ i];
        if ( baseDim_[ i] != 1) {
          siUnits += aString( "%").arg( baseDim_[ i]);
        }
      }
    }
    return siUnits;
  }

  aString aUnits::unitsMetric() const
  {
    return aString( "% %").arg( 1/scaleMetric_).arg( unitsSI());
  }

  double aUnits::convert( const aUnits &other) const
  {
    isCompatible( other);
    // Convert apply sequence:
    // 1. Offset, 2. Scale, 3. Scale, 4. Offset.
    return ( other.value_ - other.offset_) * other.scale_ / scale_ + offset_;
  }

  aUnits& aUnits::operator=( const aUnits &other)
  {
    if ( &other != this) {
      copy( other);
    }
    return *this;
  }

  aUnits& aUnits::operator=( const double & valueArg)
  {
    value_ = valueArg;
    return *this;
  }

  aUnits& aUnits::operator+=( const aUnits &other)
  {
    isCompatible( other);
    value_ += convert( other);
    return *this;
  }

  aUnits& aUnits::operator-=( const aUnits &other)
  {
    isCompatible( other);
    value_ -= convert( other);
    return *this;
  }

  aUnits& aUnits::operator*=( const double &valueArg)
  {
    value_ *= valueArg;
    return *this;
  }

  aUnits& aUnits::operator/=( const double &valueArg)
  {
    value_ *= valueArg;
    return *this;
  }

  aUnits aUnits::operator+( const aUnits &other) const
  {
    isCompatible( other);
    aUnits uCopy( *this);
    uCopy.value_ += uCopy.convert( other);
    return uCopy;
  }

  aUnits aUnits::operator-( const aUnits &other) const
  {
    isCompatible( other);
    aUnits uCopy( *this);
    uCopy.value_ -= uCopy.convert( other);
    return uCopy;
  }

  aUnits aUnits::operator*( const aUnits &other) const
  {
    return compose( *this, other);
  }

  aUnits aUnits::operator*( const double &valueArg) const
  {
    aUnits u( *this);
    u.value_ *= valueArg;
    return u;
  }

  aUnits aUnits::operator/( const aUnits &other) const
  {
    return compose( *this, pow( other, -1));
  }

  aUnits aUnits::operator/( const double &valueArg) const
  {
    aUnits u( *this);
    u.value_ /= valueArg;
    return u;
  }

  void aUnits::copy( const aUnits &other)
  {
    units_       = other.units_;
    value_       = other.value_;
    scale_       = other.scale_;
    offset_      = other.offset_;
    scaleMetric_ = other.scaleMetric_;
    isEmpty_     = other.isEmpty_;

    for ( size_t i = 0; i < BASE_SIZE; ++i) {
      baseDim_[ i] = other.baseDim_[ i];
    }
  }

  bool isValidUnit( const aString& unit)
  {
    try {
      findUnits( unit);
      return true;
    }
    catch ( ...) {}
    return false;
  }

  bool isCompatible( const aUnits &fromUnits, const aUnits &toUnits)
  {
    for ( size_t i = 0; i < BASE_SIZE; ++i) {
      if ( fromUnits.baseDim_[i] != toUnits.baseDim_[i]) {
        return false;
      }
    }
    return true;
  }

  double convert( const aUnits &fromUnits, const aUnits &toUnits)
  {
    fromUnits.isCompatible( toUnits);
    // Convert apply sequence:
    // 1. Offset, 2. Scale, 3. Scale, 4. Offset.
    return ( fromUnits.value_ - fromUnits.offset_) * fromUnits.scale_ / toUnits.scale_ + toUnits.offset_;
  }

  double convert( const aUnits &fromUnits, const aUnits &toUnits, const double &value)
  {
    fromUnits.isCompatible( toUnits);
    // Convert apply sequence:
    // 1. Offset, 2. Scale, 3. Scale, 4. Offset.
    return ( value - fromUnits.offset_) * fromUnits.scale_ / toUnits.scale_ + toUnits.offset_;
  }

  double convertFromSI( const aUnits& toUnits, const double& val)
  {
    return val / ( toUnits.scale_ * toUnits.scaleMetric_) + toUnits.offset_;
  }

  double convertUsing( const aUnits &fromUnits, const aUnits &toUnits, const aUnits &convUnit)
  {
    // Units are directly compatible.
    if ( isCompatible( fromUnits, toUnits)) {
      return convert( fromUnits, toUnits);
    }

    // Units are compatible after scaling by gravity convUnit.
    else if ( isCompatible( fromUnits * convUnit, toUnits)) {
      return convert( fromUnits * convUnit, toUnits);
    }
    else if ( isCompatible( fromUnits / convUnit, toUnits)) {
      return convert( fromUnits / convUnit, toUnits);
    }

    // Otherwise, not compatible.
    throw_message( runtime_error,
      setFunctionName( "aUnits::convertUsing1()")
      << "\n - Units \"" << fromUnits.units_
      << "\" are not compatible with \""
      << toUnits.units_ << "\".");

//    cerr << "aUnits::convertUsing1() - Units incompatible - from, to: \"" << fromUnits.units_ << "\", \"" << toUnits.units_ << "\"" << endl;
    return fromUnits.value_;
  }

  double convertUsing( const aUnits &fromUnits, const aUnits &toUnits, const aUnits &convUnit1, const aUnits &convUnit2)
  {
    // Units are directly compatible.
    if ( isCompatible( fromUnits, toUnits)) {
      return convert( fromUnits, toUnits);
    }

    // Units are compatible after scaling by convUnit1.
    else if ( isCompatible( fromUnits * convUnit1, toUnits)) {
      return convert( fromUnits * convUnit1, toUnits);
    }
    else if ( isCompatible( fromUnits / convUnit1, toUnits)) {
      return convert( fromUnits / convUnit1, toUnits);
    }

    // Units are compatible after scaling by convUnit2.
    else if ( isCompatible( fromUnits * convUnit2, toUnits)) {
      return convert( fromUnits * convUnit2, toUnits);
    }
    else if ( isCompatible( fromUnits / convUnit2, toUnits)) {
      return convert( fromUnits / convUnit2, toUnits);
    }

    // Units are compatible after scaling by ( convUnit1 * convUnit2).
    else if ( isCompatible( fromUnits * ( convUnit1 * convUnit2), toUnits)) {
      return convert( fromUnits * ( convUnit1 * convUnit2), toUnits);
    }
    else if ( isCompatible( fromUnits / ( convUnit1 * convUnit2), toUnits)) {
      return convert( fromUnits / ( convUnit1 * convUnit2), toUnits);
    }

    // Units are compatible after scaling by ( convUnit1 / convUnit2).
    else if ( isCompatible( fromUnits * ( convUnit1 / convUnit2), toUnits)) {
      return convert( fromUnits * ( convUnit1 / convUnit2), toUnits);
    }
    else if ( isCompatible( fromUnits / ( convUnit1 / convUnit2), toUnits)) {
      return convert( fromUnits / ( convUnit1 / convUnit2), toUnits);
    }

    // Otherwise, not compatible.
    throw_message( runtime_error,
      setFunctionName( "aUnits::convertUsing2()")
      << "\n - Units \"" << fromUnits.units_
      << "\" are not compatible with \""
      << toUnits.units_ << "\".");

//    cerr << "aUnits::convertUsing2() - Units incompatible - from, to: \"" << fromUnits.units_ << "\", \"" << toUnits.units_ << "\"" << endl;
    return fromUnits.value_;
  }

  double deltaConvert( const aUnits &fromUnits, const aUnits &toUnits)
  {
    return fromUnits.value_ * fromUnits.scale_ / toUnits.scale_;
  }

  double deltaConvert( const aUnits &fromUnits, const aUnits &toUnits, const double &value)
  {
    return value * fromUnits.scale_ / toUnits.scale_;
  }

  aUnits scale( const aUnits &units, const double &num, const double &den)
  {
    aUnits newUnits( units);
    newUnits.scale_ *= num / den;
    return newUnits;
  }

  aUnits scaleSI( const aUnits &units, const double &num, const double &den)
  {
    aUnits newUnits( units);
    newUnits.scaleMetric_ *= num / den;
    return newUnits;
  }

  aUnits translate( const aUnits &units, const double &num, const double &den)
  {
    aUnits newUnits( units);
    newUnits.offset_ += num / den;
    return newUnits;
  }

  aUnits pow( const aUnits &units, const int num)
  {
    aUnits newUnits( units);
    for ( int i = 0; i < BASE_SIZE; ++i) {
      newUnits.baseDim_[ i] *= num;
    }
    // This switch is just for code speed; limits the use of using std::pow().
    switch ( num) {
    case -3:
      newUnits.scale_ = 1.0 / ( newUnits.scale_   * newUnits.scale_   * newUnits.scale_);
      newUnits.value_ = 1.0 / ( newUnits.value_   * newUnits.value_   * newUnits.value_);
      newUnits.units_ = aString( "(%)%").arg( newUnits.units_).arg( num);
      newUnits.scaleMetric_ = 1.0 / ( newUnits.scaleMetric_ * newUnits.scaleMetric_ * newUnits.scaleMetric_);
      break;

    case -2:
      newUnits.scale_ = 1.0 / ( newUnits.scale_   * newUnits.scale_);
      newUnits.value_ = 1.0 / ( newUnits.value_   * newUnits.value_);
      newUnits.units_ = aString( "(%)%").arg( newUnits.units_).arg( num);
      newUnits.scaleMetric_ = 1.0 / ( newUnits.scaleMetric_ * newUnits.scaleMetric_);
      break;

    case -1:
      newUnits.scale_ = 1.0 / newUnits.scale_;
      newUnits.value_ = 1.0 / newUnits.value_;
      newUnits.units_ = aString( "(%)%").arg( newUnits.units_).arg( num);
      newUnits.scaleMetric_ = 1.0 / newUnits.scaleMetric_;
      break;

    case 0:
      newUnits.scale_ = 1.0;
      newUnits.value_ = 1.0;
      newUnits.units_ = "";
      newUnits.scaleMetric_ = 1.0;
      break;

    case 1:
      break;

    case 2:
      newUnits.scale_ *= newUnits.scale_;
      newUnits.value_ *= newUnits.value_;
      newUnits.units_  = aString( "(%)%").arg( newUnits.units_).arg( num);
      newUnits.scaleMetric_ *= newUnits.scaleMetric_;
      break;

    case 3:
      newUnits.scale_ *= newUnits.scale_   * newUnits.scale_;
      newUnits.value_ *= newUnits.value_   * newUnits.value_;
      newUnits.units_  = aString( "(%)%").arg( newUnits.units_).arg( num);
      newUnits.scaleMetric_ *= newUnits.scaleMetric_ * newUnits.scaleMetric_;
      break;

    default:
      newUnits.scale_ = std::pow( newUnits.scale_, double( num));
      newUnits.value_ = std::pow( newUnits.value_, double( num));
      newUnits.units_ = aString( "(%)%").arg( newUnits.units_).arg( num);
      newUnits.scaleMetric_ = std::pow( newUnits.scaleMetric_, double( num));
      break;
    }

    return newUnits;
  }

  aUnits compose( const aUnits &units1, const aUnits &units2)
  {
    aUnits newUnits( units1);
    for ( int i = 0; i < BASE_SIZE; ++i) {
      newUnits.baseDim_[ i] += units2.baseDim_[i];
    }
    newUnits.scale_ *= units2.scale_;
    newUnits.value_ *= units2.value_;
    newUnits.scaleMetric_ *= units2.scaleMetric_;
    if ( newUnits.units_.empty()) {
      newUnits.units_ = units2.units_;
    }
    else {
      newUnits.units_ += " " + units2.units_;
    }
    return newUnits;
  }

  ostream& operator<< ( ostream &os, const aUnits &units)
  {
    os << units.value_ << " (" << units.units_ << ')';
    return os;
  }

  aUnits& operator<< ( aUnits &os, const aUnits &units)
  {
    os.setValue( units);
    return os;
  }

  const aUnits& findUnits( const aString &units)
  {
    using namespace predefined_aunits;
    using namespace aunits_cache;

    // Do we have it cached already?
    UnitsMap::const_iterator umIter = unitsMapCache.find( units);
    if ( umIter != unitsMapCache.end()) {
      return umIter->second;
    }

    aUnits foundUnits;
    aString theseUnits = units;
    bool firstPass = true;
    while ( theseUnits.hasStringTokens()) {
      bool hasPower = false;
      int  powValue = 0;
      aString thisUnit = theseUnits.getStringToken( " ._/"); // " ._" for separator, "/" for divisor support.
      aString::size_type ipos = thisUnit.find_first_of( "-+");
      if ( ipos == aString::npos) {
        ipos = thisUnit.find_first_of( "0123456789");
      }
      if ( ipos != aString::npos) {
        hasPower = true;
        powValue = atoi( thisUnit.substr( ipos).c_str());
        if ( theseUnits.delimeterTriggered() == '/') {
          powValue *= -1;
        }
        thisUnit = thisUnit.substr( 0, ipos); // Remove power part from thisUnit string.
      }
      umIter = unitsMapCache.find( thisUnit);
      if ( umIter == unitsMapCache.end()) {
        // Do we have a prefix scale factor?
        for ( size_t i = 1; i < thisUnit.size() && i <= Max_Prefix_Length; ++i) {
          umIter = unitsMapCache.find( thisUnit.substr( i));
          if ( umIter != unitsMapCache.end()) {
            // Found a valid unit; now check scale prefix.
            UnitsMap::const_iterator prefixIter =
              unitsPrefixMapCache.find( thisUnit.substr( 0, i));
            if ( prefixIter == unitsPrefixMapCache.end()) {
              throw_message( runtime_error,
                setFunctionName( "aUnits::findUnits()")
                << " - for units \"" << units << "\"\n"
                << "   - can't find valid scale prefix for unit \"" << umIter->first << "\".");
            }
            if ( firstPass) {
              firstPass = false;
              if ( hasPower) {
                foundUnits.copy( pow( prefixIter->second, powValue));
              }
              else {
                foundUnits.copy( prefixIter->second);
              }
            }
            else {
              if (hasPower) {
                foundUnits.copy( compose( foundUnits, pow( prefixIter->second, powValue)));
              }
              else {
                foundUnits.copy( compose( foundUnits, prefixIter->second));
              }
            }
            break;
          }
        }
        if ( umIter == unitsMapCache.end()) {
          throw_message( runtime_error,
            setFunctionName( "aUnits::findUnits()")
            << " - for units \"" << units << "\"\n"
            << "   - can't find valid unit of \"" << thisUnit << "\".");
        }
      }
      if ( firstPass) {
        firstPass = false;
        if ( hasPower) {
          foundUnits.copy( pow( umIter->second, powValue));
        }
        else {
          foundUnits.copy( umIter->second);
        }
      }
      else {
        if ( hasPower) {
          foundUnits.copy( compose( foundUnits, pow( umIter->second, powValue)));
        }
        else {
          foundUnits.copy( compose( foundUnits, umIter->second));
        }
      }
    }

    foundUnits.units_   = units;
    foundUnits.isEmpty_ = false; // Just to be sure this carried through.

    // Add to cache; thread safe.
    unitsMapCacheLocker.lock();
    unitsMapCache[ units] = foundUnits;
    unitsMapCacheLocker.unlock();

    return unitsMapCache[ units];
  }

  const aUnits& addUnits( const aString &name, const aUnits &units)
  {
    using namespace predefined_aunits;
    using namespace aunits_cache;

    // Do we have it cached already?
    UnitsMap::const_iterator umIter = unitsMapCache.find( name);
    if ( umIter != unitsMapCache.end()) {
      return umIter->second;
    }

    // Add to cache; thread safe.
    unitsMapCacheLocker.lock();
    unitsMapCache[ name] = units;
    unitsMapCacheLocker.unlock();

    return unitsMapCache[ name];
  }

  aUnitsList findCompatibleUnits( const aString &units)
  {
    return findCompatibleUnits( findUnits( units));
  }

  aUnitsList findCompatibleUnits( const aUnits &units)
  {
    using namespace predefined_aunits;
    using namespace aunits_cache;

    aUnitsList unitsList;
    for ( UnitsMap::const_iterator umIter = unitsMapCache.begin(); umIter != unitsMapCache.end(); ++umIter) {
      if ( isCompatible( units, umIter->second)) {
        bool isUnique = true;
        for ( size_t i = 0; i < unitsList.size(); ++i) {
          if ( umIter->second.units() == unitsList[ i].units()) {
            isUnique = false;
            break;
          }
        }
        if ( isUnique) {
          unitsList << umIter->second;
        }
      }
    }

    return unitsList;
  }
}
