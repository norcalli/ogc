#include "units.h"
#include "unit-functions.h"

const static Units<unit_value> kDimensionLess (1, 0, 0, 0, 0);

namespace SI {
namespace Prefix {
const static Units<unit_value> kYocto ( 1e-24, 0, 0, 0, 0);
const static Units<unit_value> kZepto ( 1e-21, 0, 0, 0, 0);
const static Units<unit_value> kAtto  ( 1e-18, 0, 0, 0, 0);
const static Units<unit_value> kFemto ( 1e-15, 0, 0, 0, 0);
const static Units<unit_value> kPico  ( 1e-12, 0, 0, 0, 0);
const static Units<unit_value> kNano  ( 1e-9,  0, 0, 0, 0);
const static Units<unit_value> kMicro ( 1e-6,  0, 0, 0, 0);
const static Units<unit_value> kMilli ( 1e-3,  0, 0, 0, 0);
const static Units<unit_value> kCenti ( 1e-2,  0, 0, 0, 0);
const static Units<unit_value> kDeci  ( 1e-1,  0, 0, 0, 0);
const static Units<unit_value> kDeca  ( 1e1,   0, 0, 0, 0);
const static Units<unit_value> kHecto ( 1e2,   0, 0, 0, 0);
const static Units<unit_value> kKilo  ( 1e3,   0, 0, 0, 0);
const static Units<unit_value> kMega  ( 1e6,   0, 0, 0, 0);
const static Units<unit_value> kGiga  ( 1e9,   0, 0, 0, 0);
const static Units<unit_value> kTera  ( 1e12,  0, 0, 0, 0);
const static Units<unit_value> kPeta  ( 1e15,  0, 0, 0, 0);
const static Units<unit_value> kExa   ( 1e18,  0, 0, 0, 0);
const static Units<unit_value> kZetta ( 1e21,  0, 0, 0, 0);
const static Units<unit_value> kYotta ( 1e24,  0, 0, 0, 0);
}
namespace Length {
const static Units<unit_value> kMeter ( 1, 0, 0, 0, 1);
}
namespace Mass {
const static Units<unit_value> kGram ( 1, 0, 1, 0, 0);
}
namespace Time {
const static Units<unit_value> kSecond ( 1, 0, 0, 1, 0);
}
namespace Charge {
const static Units<unit_value> kCoulomb ( 1, 1, 0, 0, 0);
}
namespace Force {
const static Units<unit_value> kNewton = Length::kMeter * Prefix::kKilo * Mass::kGram / pow(Time::kSecond, 2);
}
namespace Pressure {
const static Units<unit_value> kPascal = Force::kNewton / pow(Length::kMeter, 2);
}
}

namespace Imperial {
// All in terms of meters.
namespace Length {
const static Units<unit_value> kThou         = 2.54e-5 * SI::Length::kMeter; // ( 2.54e-5,       0, 0, 0, 1 );
const static Units<unit_value> kInch         = 1000 * kThou;
const static Units<unit_value> kFoot         = 12 * kInch;
const static Units<unit_value> kYard         = 3 * kFoot;
const static Units<unit_value> kChain        = 22 * kYard;
const static Units<unit_value> kFurlong      = 10 * kChain;
const static Units<unit_value> kMile         = 8 * kFurlong;
const static Units<unit_value> kLeague       = 3 * kMile;

const static Units<unit_value> kFathom       = 1.853184 * SI::Length::kMeter; // (1.853184,       0, 0, 0, 1 );
const static Units<unit_value> kCable        = 100 * kFathom;
const static Units<unit_value> kNauticalMile = 10 * kCable;
}
// All in terms of meters^2.
namespace Area {
const static Units<unit_value> kAcre         = Length::kFurlong * Length::kChain;
}
// All in terms of meters^3.
namespace Volume {
const static Units<unit_value> kFluidOunce = 2.95735296e-5 * pow(SI::Length::kMeter, 3); // ( 2.95735296e-5,     0, 0, 0, 3);
const static Units<unit_value> kGill       = 5 * kFluidOunce;
const static Units<unit_value> kPint       = 20 * kFluidOunce;
const static Units<unit_value> kQuart      = 40 * kFluidOunce;
const static Units<unit_value> kGallon     = 160 * kFluidOunce;
}
// All in terms of grams.
namespace Mass {
const static Units<unit_value> kPound         = 4.5359237e2 * SI::Mass::kGram; // ( 4.5359237e2,  0, 1, 0, 0);
const static Units<unit_value> kGrain         = kPound / 7000;
const static Units<unit_value> kDrachm        = kPound / 256;
const static Units<unit_value> kOunce         = kPound / 16;
const static Units<unit_value> kStone         = 14 * kPound;
const static Units<unit_value> kQuarter       = 28 * kPound;
const static Units<unit_value> kHundredWeight = 112 * kPound;
const static Units<unit_value> kTon           = 2240 * kPound;
}
namespace Force {
const static Units<unit_value> kPoundForce    = 9.80665 * SI::Length::kMeter / pow(SI::Time::kSecond, 2) * Mass::kPound;
}
namespace Pressure {
const static Units<unit_value> kAtm           = 101325 * SI::Pressure::kPascal;
const static Units<unit_value> kTorr          = kAtm / 760;
const static Units<unit_value> kmmHg          = kTorr;
const static Units<unit_value> kPsi           = Force::kPoundForce / pow(Length::kInch, 2);
}
}
// All in terms of seconds.
namespace Time {
// const static Units<unit_value> kSecond ( 1,            0, 0, 1, 0);
const static Units<unit_value> kMinute = 60 * SI::Time::kSecond;
const static Units<unit_value> kHour   = 60 * kMinute;
const static Units<unit_value> kDay    = 24 * kHour;
const static Units<unit_value> kYear   = 365 * kDay;
}
