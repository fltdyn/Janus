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

#include "units.h"

#include <algorithm>
#include <iterator>
#include <cassert>
#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

namespace {

  const double GRAVITY = 9.80665;
  const double PI = 3.141592653589793238462643383279502884197169399375105821;

  const size_t N_DIMENSIONS = 8;

  struct unit
  {
    string symbol;
    double scale = 1.0;
    double bias = 0.0;
    int dimensions[N_DIMENSIONS] = { 0, 0, 0, 0, 0, 0, 0, 0};

    unit() = default;
    unit( const string& s, const unit& rhs) : unit( rhs) { symbol = s;}
  };

  unit create( const int* dims)
  {
    unit u;
    for ( size_t i = 0; i < N_DIMENSIONS; ++i) u.dimensions[i] = dims[i];
    u.scale = 1.0;
    u.bias = 0.0;
    return u;
  }

  bool has_dimension( const unit& u)
  {
    for ( size_t i = 0; i < N_DIMENSIONS; ++i) if ( u.dimensions[i] != 0) return true;
    return false;
  }

  unit compose( const unit& a, const unit& b)
  {
    unit u;
    for ( size_t i = 0; i < N_DIMENSIONS; ++i) u.dimensions[i] = a.dimensions[i] + b.dimensions[i];
    u.scale = a.scale * b.scale;
    u.bias = 0.0;
    if ( a.bias != 0.0 && !has_dimension( b)) u.bias = a.bias / b.scale;
    if ( b.bias != 0.0 && !has_dimension( a)) u.bias = b.bias / a.scale;
    return u;
  }

  template<typename... Args>
  unit compose( const unit& a, const unit& b, Args... args)
  {
    return compose( a, compose( b, args...));
  }

  unit scale( const unit& original, const double& numerator, const double& denominator = 1.0)
  {
    unit u = original;
    u.scale = ( u.scale * numerator) / denominator;
    return u;
  }

  unit translate( const unit& original, const double& bias)
  {
    unit u = original;
    u.bias += bias;
    return u;
  }

  unit pow( const unit& original, const int exponent)
  {
    unit u;
    for ( size_t i = 0; i < N_DIMENSIONS; ++i) u.dimensions[i] = original.dimensions[i] * exponent;
    u.scale = 1.0;
    if ( exponent < 0) for ( int i = 0; i != exponent; --i) u.scale /= original.scale;
    if ( exponent > 0) for ( int i = 0; i != exponent; ++i) u.scale *= original.scale;
    u.bias = exponent == 1 ? original.bias : 0.0;
    return u;
  }

  class units_container
  {
  public:
    units_container( const vector<unit>& prefixes, const vector<unit>& units, const vector<unit>& nonprefixunits)
    {
      for ( auto& u : units) {
        assert( units_.find( u.symbol) == units_.end());
        units_[u.symbol] = u;
      }
      for ( auto& u : nonprefixunits) {
        assert( units_.find( u.symbol) == units_.end());
        units_[u.symbol] = u;
      }

      noPrefixUnits_ = units_;

      for ( auto& u :units) {
        for ( auto& p : prefixes) {
          if ( units_.find( p.symbol + u.symbol) == units_.end()) {
            units_[p.symbol + u.symbol] = compose( p, u);
          }
        }
      }
    }

    const unit* get( const string& symbol) const
    {
      auto it = units_.find( symbol);
      if ( it == units_.end()) return nullptr;
      return &it->second;
    }

    set<string> get_compatible( const unit& u) const
    {
      set<string> ret;
      vector<string> currentstr( N_DIMENSIONS);
      vector<int> currentdim( N_DIMENSIONS);
      copy( u.dimensions, u.dimensions + N_DIMENSIONS, currentdim.begin());
      
      /*for ( auto [str, a] : noPrefixUnits_) {
        bool isCompatible = true;
        for ( size_t i = 0; i < N_DIMENSIONS; ++i) {
          isCompatible &= a.dimensions[i] == u.dimensions[i];
        }
        if ( isCompatible) {
          ret.insert( str);
        }
      }*/

      get_compatible_recursive( ret, 0, currentstr, currentdim);

      return ret;
    }

  private:

    void get_compatible_recursive( set<string>& ret, size_t idx, vector<string>& currentstr, vector<int>& currentdim) const
    {
      if ( idx == N_DIMENSIONS) {
        for ( size_t i = 0; i < N_DIMENSIONS; ++i) {
          if ( currentdim[i] != 0) return;
        }
        string s;
        for ( size_t i = 0; i < N_DIMENSIONS; ++i) {
          if ( !currentstr[i].empty()) s += currentstr[i] + " ";
        }
        if ( !s.empty()) {
          s = s.substr( 0, s.size() - 1);
          ret.insert( s);
        }
        return;
      }

      if ( currentdim[idx] == 0) {
        get_compatible_recursive( ret, idx + 1, currentstr, currentdim);
        return;
      }

      for ( auto [str, a] : noPrefixUnits_) {
        bool isCompatible = a.dimensions[idx] != 0 && abs( a.dimensions[idx]) <= abs( currentdim[idx]);
        for ( size_t i = 0; i < idx; ++i) {
          if ( i == idx) continue;
          isCompatible &= a.dimensions[i] == 0;
        }
        if ( isCompatible) {
          const int exp = currentdim[idx] / a.dimensions[idx];
          for ( size_t i = idx; i < N_DIMENSIONS; ++i) {
            currentdim[i] -= exp * a.dimensions[i];
          }
          if ( exp != 1) {
            stringstream ss;
            ss << str << exp;
            currentstr[idx] = ss.str();
          }
          else {
            currentstr[idx] = str;
          }
          get_compatible_recursive( ret, idx + 1, currentstr, currentdim);
          for ( size_t i = idx; i < N_DIMENSIONS; ++i) {
            currentdim[i] += exp * a.dimensions[i];
          }
        }
      }

      currentstr[idx] = "";
    }

    map<string,unit> noPrefixUnits_;
    map<string,unit> units_;
  };

  const string u_symbols[]   = { "m", "kg", "K", "A", "mol", "cd", "rad", "s"};
  const int    u_none_dims[] = { 0, 0, 0, 0, 0, 0, 0, 0};
  const int    u_m_dims[]    = { 1, 0, 0, 0, 0, 0, 0, 0};
  const int    u_kg_dims[]   = { 0, 1, 0, 0, 0, 0, 0, 0};
  const int    u_K_dims[]    = { 0, 0, 1, 0, 0, 0, 0, 0};
  const int    u_A_dims[]    = { 0, 0, 0, 1, 0, 0, 0, 0};
  const int    u_mol_dims[]  = { 0, 0, 0, 0, 1, 0, 0, 0};
  const int    u_cd_dims[]   = { 0, 0, 0, 0, 0, 1, 0, 0};
  const int    u_rad_dims[]  = { 0, 0, 0, 0, 0, 0, 1, 0};
  const int    u_s_dims[]    = { 0, 0, 0, 0, 0, 0, 0, 1};

  //
  // Base Units
  //
  const unit u_none = { "",  create( u_none_dims)};
  const unit u_m    = { "m",   create( u_m_dims)};
  const unit u_kg   = { "kg",  create( u_kg_dims)};
  const unit u_K    = { "K",   create( u_K_dims)};
  const unit u_A    = { "A",   create( u_A_dims)};
  const unit u_mol  = { "mol", create( u_mol_dims)};
  const unit u_cd   = { "cd",  create( u_cd_dims)};
  const unit u_rad  = { "rad", create( u_rad_dims)};
  const unit u_s    = { "s",   create( u_s_dims)};

  //
  // Derived
  //
  const unit u_ND   = { "ND",    u_none};
  const unit u_Hz   = { "Hz",    compose( scale( u_rad, 2. * PI), pow( u_s, -1))};
  const unit u_N    = { "N",     compose( u_m, u_kg, pow( u_s, -2))}; // Newton (force, weight)
  const unit u_Pa   = { "Pa",    compose( u_N, pow( u_m, -2))};       // Pascal (pressure, stress)
  const unit u_J    = { "J",     compose( u_N, u_m)};                 // Joule (energy, work, heat)
  const unit u_W    = { "W",     compose( u_J, pow( u_s, -1))};       // Watt (power, radiant flux)
  const unit u_C    = { "Coulomb", compose( u_s, u_A)};               // Coulomb (electrical charge) potential conflict with degC, hence the name change
  const unit u_V    = { "V",     compose( u_W, pow( u_A, -1))};       // Volt (voltage)
  const unit u_F    = { "Farad", compose( u_C, pow( u_V, -1))};       // Farad (electric capacitance)
  const unit u_Ohm  = { "Ohm",   compose( u_V, pow( u_A, -1))};       // Ohm (electric resistance)
  const unit u_H    = { "H",     compose( u_Ohm, u_s)};               // Henry (inductance)
  const unit u_S    = { "S",     compose( u_A, pow( u_V, -1))};       // Siemens (electrical conductance)
  const unit u_Wb   = { "Wb",    compose( u_V, u_s)};                 // Weber (magnetic flux)
  const unit u_T    = { "T",     compose( u_Wb, pow( u_m, -2))};      // Tesla (magnetic field strength)
  const unit u_lm   = { "lm",    u_cd};                               // Lumen (luminous flux)
  const unit u_lx   = { "lx",    compose( u_lm, pow( u_m, -2))};      // Lux (illuminance)
  const unit u_Bq   = { "Bq",    pow( u_s, -1)};                      // Becquerel (radioactivity)
  const unit u_Gy   = { "Gy",    compose( u_J, pow( u_kg, -1))};      // Gray (absorbed dose)
  const unit u_Sv   = { "Sv",    u_Gy};                               // Sievert (equivalent dose)
  const unit u_kat  = { "kat",   compose( pow( u_s, -1), u_mol)};     // Katal (catalytic activity)

  //
  // Length
  //
  const unit u_mm        = { "mm",        scale( u_m, 1, 1000)};
  const unit u_cm        = { "cm",        scale( u_m, 1, 100)};
  const unit u_km        = { "km",        scale( u_m, 1000)};
  const unit u_in        = { "in",        scale( scale( u_m, 1, 100), 254, 100)};      // Inch
  const unit u_ft        = { "ft",        scale( u_in, 12)};                           // Feet
  const unit u_yd        = { "yd",        scale( u_in, 36)};                           // Yard
  const unit u_smi       = { "smi",       scale( u_yd, 1760)};                         // Statute mile
  const unit u_nmi       = { "nmi",       scale( u_m,  1852)};                         // Nautical mile
  const unit u_furlong   = { "furlong",   scale( u_smi, 1, 8)};                        // Furlong
  const unit u_lightyear = { "lightyear", scale( u_m, 9460730472580800.)};             // Light-year
  const unit u_rod       = { "rod",       scale( u_ft, 16.5)};                         // Rod
  const unit u_chain     = { "chain",     scale( u_ft, 66)};                           // Chain

  //
  // Area
  //
  const unit u_ha   = { "ha",   scale( pow( u_m, 2), 10000)};
  const unit u_are  = { "are",  scale( pow( u_m, 2), 100)};
  const unit u_acre = { "acre", scale( u_ha, 10000000000000, 24710538146717)};

  //
  // Volume, fluid.
  //
  const unit u_l     = { "l",     scale( pow( u_m, 3), 1, 1000)};      // Litre (non-SI, accepted as SI), u_ml already SI scaled
  const unit u_USgal = { "USgal", scale( pow( u_in, 3), 231000)};      // US gallon, scale SI m3 to Litre
  const unit u_UKgal = { "UKgal", scale( u_l, 4546087, 1000000)};      // UK gallon, u_l already SI scaled via u_ml

  //
  // Mass
  //
  const unit u_g      = { "g",     scale( u_kg,  1, 1000)};          // Gram
  const unit u_tonne  = { "tonne", scale( u_kg,  1000)};             // Tonne
  const unit u_lbm    = { "lbm",   scale( u_kg,  0.45359237)};       // Pound mass (exact conversion) https://en.wikipedia.org/wiki/Pound_(mass)
  const unit u_oz     = { "oz",    scale( u_lbm, 1, 16)};            // Ounce
  const unit u_USton  = { "USton", scale( u_lbm, 2000)};             // US ton
  const unit u_UKton  = { "UKton", scale( u_lbm, 2240)};             // UK ton

  //
  // Force
  //
  const unit u_lbf = { "lbf", compose( u_lbm, scale( compose( u_m, pow( u_s, -2)), GRAVITY))}; // Pound force (lbm * G)

  //
  // Mass - Others based on u_lbf
  //
  const unit u_slug  = { "slug",  compose( u_lbf, pow( u_s, 2), pow( u_ft, -1))}; // Slug
  const unit u_snail = { "snail", scale( u_slug, 12)};                            // Snail

  //
  // Time
  //
  const unit u_min        = { "min",        scale( u_s,     60)};         // Minute
  const unit u_hours      = { "h",          scale( u_min,   60)};         // Hour
  const unit u_days       = { "days",       scale( u_hours, 24)};         // Day
  const unit u_weeks      = { "weeks",      scale( u_days,  7)};          // Week
  const unit u_fortnights = { "fortnights", scale( u_days,  14)};         // Fortnight
  const unit u_years      = { "years",      scale( u_days,  365.25)};     // Year
  const unit u_months     = { "months",     scale( u_years, 1, 12)};      // Month

  //
  // Speed
  //
  const unit u_kn = { "kn", compose( u_nmi, pow( u_hours, -1))}; // Knots (nautical miles per hour)

  // Acceleration
  const unit u_gee = { "gee", scale( compose( u_m, pow( u_s, -2)), GRAVITY)};

  //
  // Temperature
  //
  const unit u_degC = { "C", translate( u_K, -273.15)};               // Celcius
  const unit u_degF = { "F", translate( scale( u_K, 5, 9), -459.67)}; // Fahrenheit
  const unit u_degR = { "R", scale( u_K, 5, 9)};                      // Rankine

  //
  // Angles
  //
  const unit u_deg  = { "deg",  scale( u_rad, PI / 180.)};      // Degree
  const unit u_grad = { "grad", scale( u_rad, PI, 200.0)};      // Gradian
  const unit u_rev  = { "rev",  scale( u_deg, 360)};                // Revolutions

  //
  // Angular Velocity
  //
  const unit u_rpm = { "rpm", compose( u_rev, pow( u_min, -1))};      // RPM

  //
  // Pressure
  //
  const unit u_psi    = { "psi",      scale( scale( u_Pa, 1000),  10000000, 1450377)};  // Pounds per square inch
  const unit u_psf    = { "psf",      scale( u_psi,  1, 144)};                          // Pounds per square foot
  const unit u_bar    = { "bar",      scale( u_Pa,   100000)};                          // Bar
  const unit u_mbar   = { "millibar", scale( u_bar,  1, 1000)};                         // Millibar
  const unit u_mmHg   = { "mmHg",     scale( scale( u_Pa, 1000), 101.3250144354, 760)}; // Millimetres of mercury
  const unit u_inHg   = { "inHg",     scale( u_mmHg, 254, 10)};                         // Inches of mercury
  const unit u_atm    = { "atm",      scale( u_Pa,   101325)};                          // Atmosphere
  const unit u_inH2O  = { "inH2O",    scale( u_Pa,   248.64536925)};                    // Inches of water at a reference temperature of 20C

  //
  // Power
  //
  const unit u_hp = { "hp", scale( compose( u_ft, u_lbf, pow( u_s, -1)), 550)}; // Horse power

  //
  // Energy
  //
  const unit u_cal = { "cal", scale( u_J, 4.1867456)}; // Calorie
  const unit u_btu = { "btu", scale( u_J, 1054.3507)}; // British Thermal Units
  const unit u_erg = { "erg", scale( u_J, 1.0E-07)};   // Erg

  //
  // Prefixes
  //
  const unit prefix_deka  = { "da", scale( u_none,       10)};
  const unit prefix_hecto = { "h",  scale( u_none,       100)};
  const unit prefix_kilo  = { "k",  scale( u_none,       1000)};
  const unit prefix_mega  = { "M",  scale( prefix_kilo,  1000)};
  const unit prefix_giga  = { "G",  scale( prefix_mega,  1000)};
  const unit prefix_tera  = { "T",  scale( prefix_giga,  1000)};
  const unit prefix_peta  = { "P",  scale( prefix_tera,  1000)};
  const unit prefix_exa   = { "E",  scale( prefix_peta,  1000)};
  const unit prefix_zetta = { "Z",  scale( prefix_exa,   1000)};
  const unit prefix_yotta = { "Y",  scale( prefix_zetta, 1000)};

  const unit prefix_deci  = { "d", scale( u_none,       1, 10)};
  const unit prefix_centi = { "c", scale( u_none,       1, 100)};
  const unit prefix_milli = { "m", scale( u_none,       1, 1000)};
  const unit prefix_micro = { "u", scale( prefix_milli, 1, 1000)};
  const unit prefix_nano  = { "n", scale( prefix_micro, 1, 1000)};
  const unit prefix_pico  = { "p", scale( prefix_nano,  1, 1000)};
  const unit prefix_femto = { "f", scale( prefix_pico,  1, 1000)};
  const unit prefix_atto  = { "a", scale( prefix_femto, 1, 1000)};
  const unit prefix_zepto = { "z", scale( prefix_atto,  1, 1000)};
  const unit prefix_yocto = { "y", scale( prefix_zepto, 1, 1000)};

  const units_container Units(
    {
      prefix_deka  ,
      prefix_hecto ,
      prefix_kilo  ,
      prefix_mega  ,
      prefix_giga  ,
      prefix_tera  ,
      prefix_peta  ,
      prefix_exa   ,
      prefix_zetta ,
      prefix_yotta ,
      prefix_deci  ,
      prefix_centi ,
      prefix_milli ,
      prefix_micro ,
      prefix_nano  ,
      prefix_pico  ,
      prefix_femto ,
      prefix_atto  ,
      prefix_zepto ,
      prefix_yocto 
    },
    {
      u_m         ,
      u_K         ,
      u_A         ,
      u_mol       ,
      u_cd        ,
      u_rad       ,
      u_s         ,
      u_Hz        ,
      u_N         ,
      u_Pa        ,
      u_J         ,
      u_W         ,
      u_C         ,
      u_V         ,
      u_F         ,
      u_Ohm       ,
      u_H         ,
      u_S         ,
      u_Wb        ,
      u_T         ,
      u_lm        ,
      u_lx        ,
      u_Bq        ,
      u_Gy        ,
      u_Sv        ,
      u_kat       ,
      u_l         ,
      u_g         ,
      u_tonne     ,
      u_degC      ,
      u_degF      ,
      u_degR      ,
      u_deg       ,
      u_grad      ,
      u_rev       ,
      u_atm       ,
      u_inH2O     ,
      u_hp        ,
      u_cal       ,
      u_btu       ,
      u_erg
    },
    {
      u_none      ,
      u_ND        ,
      u_kg        ,
      u_mm        ,
      u_cm        ,
      u_km        ,
      u_in        ,
      u_ft        ,
      u_yd        ,
      u_smi       ,
      u_nmi       ,
      u_furlong   ,
      u_lightyear ,
      u_rod       ,
      u_chain     ,
      u_ha        ,
      u_are       ,
      u_acre      ,
      u_USgal     ,
      u_UKgal     ,
      u_lbm       ,
      u_oz        ,
      u_USton     ,
      u_UKton     ,
      u_lbf       ,
      u_slug      ,
      u_snail     ,
      u_min       ,
      u_hours     ,
      u_days      ,
      u_weeks     ,
      u_fortnights,
      u_years     ,
      u_months    ,
      u_kn        ,
      u_gee       ,
      u_rpm       ,
      u_psi       ,
      u_psf       ,
      u_bar       ,
      u_mbar      ,
      u_mmHg      ,
      u_inHg      ,
    }
  );

  bool get_unit( unit& u, string s)
  {
    s += " ";
    u = u_none;

    size_t off = 0;
    while ( s.find_first_of( " ", off) != string::npos) {
      const size_t end = s.find_first_of( " ", off);
      const string piece = s.substr( off, end - off);
      off = end + 1;

      int exp = 1;
      const size_t startexp = piece.find_first_of( "+-0123456789");
      if ( startexp != string::npos) {
        exp = atoi( piece.substr( startexp).c_str());
      }

      const string symbol = piece.substr( 0, startexp);
      const unit* punits = Units.get( symbol);

      if ( !punits) return false;

      u = compose( u, pow( *punits, exp));
    }

    return true;
  }

  const set<string> AeroUnits = {
    // Length
    "mm",
    "m",
    "km",
    "in",
    "ft",
    "smi",
    "nmi",

    // Area
    "mm2",
    "m2",
    "ft2",

    // Volume
    "l",
    "USGal",
    "UKGal",

    // Mass
    "g",
    "kg",
    "lbm",

    // Force
    "N",
    "lbf",
    "slug",

    // Time
    "s",

    // Speed
    "m s-1",
    "km h-1",
    "ft s-1",
    "kn",

    // Acceleration
    "m s-2",
    "ft s-2",
    "kn s-1",
    "gee",

    // Temperature
    "C",
    "K",
    "F",

    // Angles
    "deg",
    "rad",

    // Angular Rate
    "Hz",
    "deg s-1",
    "rad s-1",

    // Angular Acceleration
    "deg s-2",
    "rad s-2",

    // Pressure
    "Pa",
    "psi",
    "bar",
    "atm",

    // Power
    "W",
    "hp",

    // Misc
    "m2 kg",
    "ft2 slug"
  };

  void trim_whitespace_lr( string& str)
  {
    auto ltrim = [](string& str)
    {
      str.erase( str.begin(), find_if( str.begin(), str.end(), [](auto ch){ return !isspace( ch); }));
    };
    auto rtrim = [](string& str)
    {
      str.erase( find_if( str.rbegin(), str.rend(), [](auto ch){ return !isspace( ch); }).base(), str.end());
    };
    ltrim( str);
    rtrim( str);
  }

  vector<string> split_by( const string& str, const char delim)
  {
    vector<string> ret;
    stringstream ss( str);
    string tmp;
    while ( getline( ss, tmp, delim)) {
      ret.push_back( tmp);
    }
    return ret;
  }

} // namespace 

namespace units {

  string metric( const string& str)
  {
    /*if ( str == "Hz") return "Hz";*/
    if ( str == "dB") return "ND";

    unit u;
    if ( !get_unit( u, str)) return str;
    stringstream ss;
    for ( size_t i = 0; i < N_DIMENSIONS; ++i) {
      if ( u.dimensions[i] != 0) {
        ss << u_symbols[i];
        if ( u.dimensions[i] != 1) {
          ss << u.dimensions[i];
        }
        ss << " ";
      }
    }
    string ret = ss.str();
    if ( !ret.empty()) ret = ret.substr( 0, ret.size() - 1);
    else ret = "ND";
    return ret;
  }

  bool is_compatible( const string& astr, const string& bstr)
  {
    /*if ( astr == "Hz" && bstr == "rad s-1") return true;
    if ( bstr == "Hz" && astr == "rad s-1") return true;*/
    if ( astr == "ND" && bstr == "dB") return true;
    if ( bstr == "ND" && astr == "dB") return true;

    unit a, b;
    if ( !( get_unit( a, astr) && get_unit( b, bstr))) return false;
    for ( size_t i = 0; i < N_DIMENSIONS; ++i) {
      if ( a.dimensions[i] != b.dimensions[i]) return false;
    }
    return true;
  }

  function<double( const double&)> get_converter( const string& fromstr, const string& tostr)
  {
    if ( fromstr == tostr) return []( const double& x) { return x;};
    if ( !is_compatible( fromstr, tostr)) return nullptr;

    if ( fromstr == "dB" && tostr == "ND") {
      return []( const double& x) { return ::pow( 10., x / 20.);};
    }
    if ( fromstr == "ND" && tostr == "dB") {
      return []( const double& x) { return 20. * log10( x);};
    }

    unit from; (void) get_unit( from, fromstr);
    unit to; (void) get_unit( to, tostr);
    return [from,to]( const double& x) { return ( x - from.bias) * from.scale / to.scale + to.bias;};
  }

}