#define _USE_MATH_DEFINES
#include <iostream>
#include <cassert>
#include <cmath>
#include <ctime>
#include <boost/date_time.hpp>
 
using namespace boost;
 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
 
struct Object {
    double a0, ad;
    double e0, ed;
    double I0, Id;
    double L0, Ld;
    double w0, wd;
    double O0, Od;
 
    double a(double T) const { return a0 + ad * T; }
    double e(double T) const { return e0 + ed * T; }
    double I(double T) const { return I0 + Id * T; }
    double L(double T) const { return L0 + Ld * T; }
    double w(double T) const { return w0 + wd * T; }
    double O(double T) const { return O0 + Od * T; }
};
 
Object Earth = {
      1.00000261,   0.00000562,
      0.01671123,  -0.00004392,
     -0.00000026,  -0.00000394,
      1.75343755, 628.30757790,
      1.79660147,   0.00564218,
      0,            0
};
 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
 
double wrap(double v, double min, double max)
{
    assert(max > min);
 
    if (v >= min && v <= max)
        return v;
    else if (v < min)
        return v + std::ceil((min - v) / (max - min)) * (max - min);
    return v - std::ceil((v - max) / (max - min)) * (max - min);
}
 
double calculate_T()
{
    static const posix_time::ptime J2000(gregorian::date(2000, 1, 1), posix_time::hours(12));
    auto now = posix_time::from_time_t(time(NULL));
    return (now - J2000).total_seconds() / 86400.0 / 36525.0;
}
 
double calculate_T(const std::string& iso)
{
    static const posix_time::ptime J2000(gregorian::date(2000, 1, 1), posix_time::hours(12));
    auto t = posix_time::from_iso_string(iso);
    return (t - J2000).total_seconds() / 86400.0 / 36525.0;
}
 
double iterate_E(double M, double e)
{
    double E0 = M + e * sin(M);
 
    double dE;
    do {
        double dM = M - (E0 - e * sin(E0));
        dE = dM / (1 - e * cos(E0));
        E0 = E0 + dE;
    }   while (dE > 0.00001);
 
    return E0;
}
 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
 
// 2011
const char* vernal_equinox   = "20110320T233700";
const char* summer_solstice  = "20110621T182200";
const char* autumnal_equinox = "20110923T100400";
const char* winter_solstice  = "20111222T052800";
const char* periapsis        = "20110103T120000";
const char* apoapsis         = "20110704T120000";
 
int main(int argc, char** argv)
{
    // compute the time in centuries since J2000 epoch
    auto T = calculate_T();
    std::cout.precision(3);
    std::cout << "T = " << (T * 100) << " years\n";
 
    // compute the values of the six Keplerian elements at T
    auto a  = Earth.a(T);
    auto e  = Earth.e(T);
    auto I  = Earth.I(T);
    auto L  = Earth.L(T);
    auto w_ = Earth.w(T);
    auto O  = Earth.O(T);
 
    // compute argument of perihelion (w) and the mean anomaly (M)
    auto w = w_ - O;
    auto M = wrap(L - w_, -180, 180);
 
    // compute the eccentric anomaly
    auto E = wrap(iterate_E(M, e), -M_PI, M_PI);
    std::cout << "\nEarth's current eccentric anomaly: " << (180 * E / M_PI) << " degrees\n";
 
    // compute heliocentric coordinates
    double x_ = a * (cos(E) - e);
    double y_ = a * std::sqrt(1 - e*e) * sin(E);
 
    // compute coordinates in the J2000 ecliptic plane
    double x_ecl = (cos(w)*cos(O) - sin(w)*sin(O)*cos(I)) * x_ + (-sin(w)*cos(O) - cos(w)*sin(O)*cos(I)) * y_;
    double y_ecl = (cos(w)*sin(O) + sin(w)*cos(O)*cos(I)) * x_ + (-sin(w)*sin(O) + cos(w)*cos(O)*cos(I)) * y_;
    double z_ecl = (sin(w)*sin(I)) * x_ + (cos(w)*sin(I)) * y_;
 
    // compute the equatorial coordinates in the J2000 frame
    double axial_tilt = 23.43928 / 180 * M_PI;
    double x_eq = x_ecl;
    double y_eq = cos(axial_tilt) * y_ecl - sin(axial_tilt) * z_ecl;
    double z_eq = sin(axial_tilt) * y_ecl + cos(axial_tilt) * z_ecl;
 
    // compute declination and right ascension (hour angle)
    double dec = acos(z_eq / std::sqrt(x_eq*x_eq + y_eq*y_eq + z_eq*z_eq)) - M_PI / 2;
    double ra  = atan2(-y_eq, -x_eq);
 
    std::cout << "\nSun's current declination " << (180 * dec / M_PI) << " degrees\n";
    std::cout << "Sun's current right ascension " << (180 * ra  / M_PI) << " degrees\n";
}
 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
