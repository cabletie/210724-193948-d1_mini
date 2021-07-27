/*
 * ut61e_display.cpp
 *
 *  Created on: 12.11.2009
 *      Author: steffen vogel
 *         git: https://github.com/stv0g/dmm_ut61e
 */

#include "ut61e_display.h"
#include <exception>
#include <cstdlib>
#include <unordered_map>
using std::unordered_map;

// const char* UT61E_DISP::modelbl[] = { "V", "A", "Ω", "▶︎", "Hz", "F", "H", "℧" };
// const char* UT61E_DISP::fmodelbl[] = { "Duty" , "Frequency" };
// const char* UT61E_DISP::powerlbl[] = {"AC", "DC" };
// const char* UT61E_DISP::rangelbl[] = { "Auto", "Manual" };
// const char* UT61E_DISP::loadlbl[] = { "Overload", "Normal", "Underload" };
// const char* UT61E_DISP::peaklbl[] = { "Maximum", "Minimum" };
struct Range_Dict
{
	float value_multiplier;
	int dp_digit_position;
	const char* display_unit;
};

// value_multiplier:  Multiply the displayed value by this factor to get the value in base units.
// dp_digit_position: The digit position of the decimal point in the displayed meter reading value.
// display_unit:      The unit the displayed value is shown in.
typedef unordered_map<char, Range_Dict> range_dict_map;

const range_dict_map RANGE_VOLTAGE = {
    {0b0110000, {1e0, 4, "V"}},  //2.2000V
    {0b0110001, {1e0, 3, "V"}},  //22.000V
    {0b0110010, {1e0, 2, "V"}},  //220.00V
    {0b0110011, {1e0, 1, "V"}},  //2200.0V
    {0b0110100, {1e-3, 2,"mV"}} //220.00mV
};

/* -------- Start imported python ------------------*/
// undocumented in datasheet
const range_dict_map RANGE_CURRENT_AUTO_UA = {
    {0b0110000, {1e-6, 2, "µA"}}, //
    {0b0110001, {1e-6, 1, "µA"}} //2
};
// undocumented in datasheet
const range_dict_map RANGE_CURRENT_AUTO_MA = {
    {0b0110000, {1e-3, 3, "mA"}}, //
    {0b0110001, {1e-3, 2, "mA"}} //2
};

const unordered_map<char, const char*> RANGE_CURRENT_AUTO = { //2-range auto A *It includes auto μA, mA, 22.000A/220.00A, 220.00A/2200.0A.
    {0b0110000, "Lower Range (IVSL)"}, //Current measurement input for 220μA, 22mA.
    {0b0110001, "Higher Range (IVSH)"} //Current measurement input for 2200μA, 220mA and 22A modes.
};

const range_dict_map RANGE_CURRENT_22A = { 
	{0b0110000, {1e0, 3, "A"}}
}; //22.000 A

const range_dict_map RANGE_CURRENT_MANUAL = {
    {0b0110000, {1e0, 4, "A"}}, //2.2000A
    {0b0110001, {1e0, 3, "A"}}, //22.000A
    {0b0110010, {1e0, 2, "A"}}, //220.00A
    {0b0110011, {1e0, 1, "A"}}, //2200.0A
    {0b0110100, {1e0, 0, "A"}} //22000A
};

const unordered_map<char, const char*> RANGE_ADP = {
    {0b0110000, "ADP4"},
    {0b0110001, "ADP3"},
    {0b0110010, "ADP2"},
    {0b0110011, "ADP1"},
    {0b0110100, "ADP0"}
};

const range_dict_map RANGE_RESISTANCE = {
    {0b0110000, {1e0, 2, "Ω"}}, //220.00Ω
    {0b0110001, {1e3, 4, "kΩ"}}, //2.2000KΩ
    {0b0110010, {1e3, 3, "kΩ"}}, //22.000KΩ
    {0b0110011, {1e3, 2, "kΩ"}}, //220.00KΩ
    {0b0110100, {1e6, 4, "MΩ"}}, //2.2000MΩ
    {0b0110101, {1e6, 3, "MΩ"}}, //22.000MΩ
    {0b0110110, {1e6, 2, "MΩ"}} //220.00MΩ
};

const range_dict_map RANGE_FREQUENCY = {
    {0b0110000, {1e0, 2, "Hz"}}, //22.00Hz
    {0b0110001, {1e0, 1, "Hz"}}, //220.0Hz
    //0b0110010
    {0b0110011, {1e3, 3, "kHz"}}, //22.000KHz
    {0b0110100, {1e3, 2, "kHz"}}, //220.00KHz
    {0b0110101, {1e6, 4, "MHz"}}, //2.2000MHz
    {0b0110110, {1e6, 3, "MHz"}}, //22.000MHz
    {0b0110111, {1e6, 2, "MHz"}} //220.00MHz
};

const range_dict_map RANGE_CAPACITANCE = {
    {0b0110000, {1e-9, 3, "nF"}}, //22.000nF
    {0b0110001, {1e-9, 2, "nF"}}, //220.00nF
    {0b0110010, {1e-6, 4, "µF"}}, //2.2000μF
    {0b0110011, {1e-6, 3, "µF"}}, //22.000μF
    {0b0110100, {1e-6, 2, "µF"}}, //220.00μF
    {0b0110101, {1e-3, 4, "mF"}}, //2.2000mF
    {0b0110110, {1e-3, 3, "mF"}}, //22.000mF
    {0b0110111, {1e-3, 2, "mF"}} //220.00mF
};

// When the meter operates in continuity mode or diode mode, this packet is always
// 0110000 since the full-scale ranges in these modes are fixed.
const range_dict_map RANGE_DIODE = {
    {0b0110000, {1e0, 4, "V"}}  //2.2000V
};

const range_dict_map RANGE_CONTINUITY = {
    {0b0110000, {1e0, 2, "Ω"}} //220.00Ω
};

const range_dict_map RANGE_NULL = {
    {0b0110000, {1e0, 2, "Ω"}} //220.00Ω
};

struct Function_Dict
{
	char * function;
	range_dict_map * subfunction;
	char * unit;
};

const unordered_map<char, Function_Dict> FUNCTION = {
    // (function, subfunction, unit)
    {0b0111011, {"voltage", RANGE_VOLTAGE, "V"}},
    {0b0111101, {"current", RANGE_CURRENT_AUTO_UA, "A"}}, //Auto μA Current / Auto μA Current / Auto 220.00A/2200.0A
    {0b0111111, {"current", RANGE_CURRENT_AUTO_MA, "A"}}, //Auto mA Current   Auto mA Current   Auto 22.000A/220.00A
    {0b0110000, {"current", RANGE_CURRENT_22A, "A"}}, //22 A current
    {0b0111001, {"current", RANGE_CURRENT_MANUAL, "A"}}, //Manual A Current
    {0b0110011, {"resistance", RANGE_RESISTANCE, "Ω"}},
    {0b0110101, {"continuity", RANGE_CONTINUITY, "Ω"}},
    {0b0110001, {"diode", RANGE_DIODE, "V"}},
    {0b0110010, {"frequency", RANGE_FREQUENCY, "Hz"}},
    {0b0110110, {"capacitance", RANGE_CAPACITANCE, "F"}},
    {0b0110100, {"temperature", RANGE_NULL, "deg"}},
    {0b0111110, {"ADP", RANGE_ADP, ""}}
};

/* --------- end imported python -------------------*/ 
UT61E_DISP::UT61E_DISP() {
	mode = M_VOLTAGE;
}

UT61E_DISP::~UT61E_DISP() {}

// Returns current DMM mode from packet
// "V", "A", "Ω", "▶︎", "Hz", "F", "H", "℧"

//  map<int, string> map2;
//     map2[1] = "Banana";
//     map2[2] = "Mango";
//     map2[3] = "Cocoa";
//     map2[4] = "Raspberry";