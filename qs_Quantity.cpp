/*
 * qs_Quantity.cpp
 *
 *  Created on: 31/05/2015
 *      Author: jim
 */


#include "qs_Quantity.h"
#include "utils.h"

#include "math.h"
#include "stdint.h"

#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

using std::bind;
using std::placeholders::_1;
using std::placeholders::_2;
using std::shared_ptr;
using std::string;
using std::to_string;
using std::vector;

namespace qs {

struct UnitInfo
{
    string name;
    string quantity;
    string symbol;
    UnitInfo(const string& name, const string& quantity, const string& symbol) : name(name), quantity(quantity), symbol(symbol) {}
};

static const UnitInfo* getUnitsInfo(int &len)
{
    /*!
     * Table generated from:
     *   grep \<unit\  ~/src/ingenitech/apps/units.xml | ~/bin/units-xml-2-c-array.awk
     *  And the XML file generated from
     *   ~/bin/si-table-2-xml.awk
     *  pasting in the tables from http://physics.nist.gov/cuu/Units/units.html
     */
    static const UnitInfo UNITS_INFO[] = {
        UnitInfo("meter", "length", "m"),
        UnitInfo("kilogram", "mass", "kg"),
        UnitInfo("second", "time", "s"),
        UnitInfo("ampere", "electric current", "A"),
        UnitInfo("kelvin", "thermodynamic temperature", "K"),
        UnitInfo("mole", "amount of substance", "mol"),
        UnitInfo("candela", "luminous intensity", "cd"),
        UnitInfo("square meter", "area", "m2"),
        UnitInfo("cubic meter", "volume", "m3"),
        UnitInfo("meter per second", "speed, velocity", "m/s"),
        UnitInfo("meter per second squared  ", "acceleration", "m/s2"),
        UnitInfo("reciprocal meter", "wave number", "m-1"),
        UnitInfo("kilogram per cubic meter", "mass density", "kg/m3"),
        UnitInfo("cubic meter per kilogram", "specific volume", "m3/kg"),
        UnitInfo("ampere per square meter", "current density", "A/m2"),
        UnitInfo("ampere per meter", "magnetic field strength  ", "A/m"),
        UnitInfo("mole per cubic meter", "amount-of-substance concentration", "mol/m3"),
        UnitInfo("candela per square meter", "luminance", "cd/m2"),
        UnitInfo("kilogram per kilogram, which may be represented by the number 1", "mass fraction", "kg/kg = 1"),
        UnitInfo("radian (a)", "plane angle", "rad"),
        UnitInfo("steradian (a)", "solid angle", "sr (c)"),
        UnitInfo("hertz", "frequency", "Hz"),
        UnitInfo("newton", "force", "N"),
        UnitInfo("pascal", "pressure, stress", "Pa"),
        UnitInfo("joule", "energy, work, quantity of heat  ", "J"),
        UnitInfo("watt", "power, radiant flux", "W"),
        UnitInfo("coulomb", "electric charge, quantity of electricity", "C"),
        UnitInfo("", "electric potential difference,", ""),
        UnitInfo("volt", "electromotive force", "V"),
        UnitInfo("farad", "capacitance", "F"),
        UnitInfo("ohm", "electric resistance", "Omega"),
        UnitInfo("siemens", "electric conductance", "S"),
        UnitInfo("weber", "magnetic flux", "Wb"),
        UnitInfo("tesla", "magnetic flux density", "T"),
        UnitInfo("henry", "inductance", "H"),
        UnitInfo("degree Celsius", "Celsius temperature", "°C"),
        UnitInfo("lumen", "luminous flux", "lm"),
        UnitInfo("lux", "illuminance", "lx"),
        UnitInfo("becquerel", "activity (of a radionuclide)", "Bq"),
        UnitInfo("gray", "absorbed dose, specific energy (imparted), kerma", "Gy"),
        UnitInfo("sievert", "dose equivalent (d)", "Sv"),
        UnitInfo("katal", "catalytic activity", "kat"),
        UnitInfo("degree Farenheit", "Farenheit temperature", "°F"),
        UnitInfo("pounds per square inch", "psi pressure", "psi"),
        UnitInfo("percentage", "percentage", "%"),
        UnitInfo("boolean", "boolean", " "),
        UnitInfo("enumerated", "enumerated", ""),
        UnitInfo("count", "count,id", " "),
    };
    len = sizeof(UNITS_INFO)/sizeof(UNITS_INFO[0]);
    return UNITS_INFO;
}

static int getUnitIdx(const std::string& name)
{
    int len;
    const UnitInfo* unitsInfo = getUnitsInfo(len);
    int ret = -1;
    for (int n = 0; n < len; ++n) {
        string iname(unitsInfo[n].name.c_str());
        if (iname.find(name.c_str()) != string::npos) {
            ret = n;
            break;
        }
    }
    return ret;
}

static const std::string unitIdxToName(uint8_t idx)
{
    int len;
    const UnitInfo* unitsInfo = getUnitsInfo(len);

    if (idx >= len) {
    	std::ostringstream oss;
    	oss<<"idx="<<idx<<" is greater than unit table length="<<len;
        throw std::logic_error(oss.str());
    }
    return unitsInfo[idx].name;
}

const char** getStdQuantities()
{
    static const char* STD_QUANTITIES[] = {
            "acceleration.x",
            "acceleration.y",
            "acceleration.z",
            "distance",
            "gps_speed",
			"wheel_based_speed"
            "ignition",
            "rpm",
            "fuel",
            "fuel_rate",
            "temperature",
            "voltage",
            "altitude",
            "latitude",
            "longitude",
            "track",
            "gyro.x",
            "gyro.y",
            "gyro.z",
            "magnetic.x",
            "magnetic.y",
            "magnetic.z",
            "id",
			"enginehours",
			"idlinghours",
			"movinghours",
            "unixtime",
            NULL
    };
    return STD_QUANTITIES;
}

const uint8_t STD_QUANTITY_NOT_PRESENT=255;
uint8_t getStdChannelIdx(const std::string& name)
{
    const char** channelZip = getStdQuantities();
    uint8_t idx = STD_QUANTITY_NOT_PRESENT;
    int n = 0;
    while (channelZip[n] != NULL) {
        if (name == channelZip[n]) {
            idx = n;
            break;
        }
        n++;
    }
    return idx;
}

/*
 * ToDo.
 *   1. Adapt default QStep's to standard quantities
 *   2. qs utilities
 *   3. Physicist
 */

QStep getDefaultQStep(uint8_t defIdx)
{
    static const QStep defaultQSteps[] = {
            QStep(0,5), //"acceleration.x"
            QStep(0,5), //"acceleration.y"
            QStep(0,5), //"acceleration.z"
            QStep(0,0), //"distance.master",
            QStep(0,0), //"edr.distance",
            QStep(0,0), //"edr.gpsDistance",
            QStep(0,0), //"edr.speed",
            QStep(0,0), //"edr.vssDistance",
            QStep(0,-4), //"fuel.used",
            QStep(0,0), //"gps.altitude",
            QStep(0,0), //"gps.hdop",
            QStep(0,-13), //"gps.latitude",
            QStep(0,-13), //"gps.longitude",
            QStep(0,-0), //"gps.mode",
            QStep(0,0), //"gps.pdop",
            QStep(0,0), //"gps.quality",
            QStep(0,0), //"gps.satellites",
            QStep(0,0), //"gps.satellitesused",
            QStep(0,-4), //"gps.speed",
            QStep(0,0), //"gps.track",
            QStep(0,0), //"gps.vdop",
            QStep(0,0), //"gyro.x",
            QStep(0,0), //"gyro.y",
            QStep(0,0), //"gyro.z",
            QStep(0,0), //"ignitionInput",
            QStep(0,-4), //"j1587.pid84.roadspeed",
            QStep(0,-13), //"j1587.pid183.fuelrate",
            QStep(0,-4), //"j1587.pid190.enginespeed",
            QStep(0,0), //"j1587.pid245.totalvehicledistance",
            QStep(0,-4), //"j1587.pid250.totalfuelused",
            QStep(0,0), //"j1939.pgn61444.enginespeed",
            QStep(0,0), //"j1939.pgn65217.highresolutiontotalvehicledistance",
            QStep(0,0), //"j1939.pgn65248.totalvehicledistance",
            QStep(0,-4), //"j1939.pgn65257.enginetotalfuelused",
            QStep(0,-4), //"j1939.pgn65265.wheelbasedvehiclespeed",
            QStep(0,-13), //"j1939.pgn65266.enginefuelrate",
            QStep(0,0), //"magnetic.x",
            QStep(0,0), //"magnetic.y",
            QStep(0,0), //"magnetic.z",
            QStep(0,0), //"obds.enginespeed",
            QStep(0,-4), //"obds.speed",
            QStep(0,0), //"obds.tripodometer",
            QStep(0,-4), //"obds.tripfuelconsumption",
            QStep(0,0), //"plci.id",
            QStep(0,0), //"rpm",
            QStep(0,-4), //"speed",
            QStep(0,-4), //"speed.master",
            QStep(0,-4), //"system.temperature",
            QStep(0,-4), //"system.voltage",
            QStep(0,0), //"unixtime", 1000 = quantize to whole seconds
    };
    if (defIdx >= sizeof(defaultQSteps)/sizeof(defaultQSteps[0]))
        throw std::logic_error("Default QStep does not exist for index="+to_string(defIdx));
    return defaultQSteps[defIdx];
}

static QStep getUnixTimeQStep(const std::string& baseChannel)
{
    if (baseChannel.find("acceleration") != string::npos)
        return QStep(0,-1);
    return QStep(0, -1);
}

static double qStepToDouble(const QStep& qStep)
{
    return (double)(1 + qStep.sig/256.0) * pow(2.0,qStep.exp);
}

/*
static QStep doubleToQStep(double val)
{
    QStep qStep;
    if (val < 1/256) {
        // Find exponent
        int exp = 8;
        while (val * (1 << exp) < 1)
            exp++;
        exp--;
        uint8_t significand = 0;
        int prec = 7;
        while (significand < (1<<exp) * val) {
            significand += 1 << prec;
            prec++;
        }
        qStep = QStep(significand, -exp);
    }
    return qStep;
}
*/
using std::ostream;
ostream& operator<<(ostream& os, const QStep& qStep)
{
    os<<"{sig="<<hex(qStep.sig)<<", exp="<<(int)qStep.exp<<"}="<<qStepToDouble(qStep);
    return os;
}

ostream& operator<<(ostream& os, const QuantityInfo& ci)
{
    os<<"{name="<<ci.name<<", unit="<<ci.unit<<", qStep="<<ci.qStep<<"}";
    return os;
}
extern std::ostream& operator<<(std::ostream& os, const std::vector<QuantityInfo>& quantityInfos)
{
    os<<"[";
	for (unsigned m = 0; m < quantityInfos.size(); ++m) {
		os<<quantityInfos[m];
		if (m != quantityInfos.size() - 1)
			os<<", ";
	}
	os<<"]";
    return os;
}

} // namespace qs


