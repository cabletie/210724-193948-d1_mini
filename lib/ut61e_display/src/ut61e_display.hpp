/*
 * ut61e_measure.h
 *
 *  Created on: 2021-07-25
 *      Author: CableTie
 *    Based on: https://github.com/stv0g/dmm_ut61e
 */

#include <cstdlib> // Needed for uint8_t
#include <string>
#include <unordered_map>
#include <tuple>
#include <sstream>

using std::unordered_map;
using std::string;
using std::tuple;
using std::stringstream;

#ifndef UT61E_DISP_H_
#define UT61E_DISP_H_

// Range setting 
// value_multiplier:  Multiply the displayed value by this factor to get the value in base units.
// dp_digit_position: The digit position of the decimal point in the displayed meter reading value.
// display_unit:      The unit the displayed value is shown in.
// e.g. {1e-9, 3, "nF"}
const struct Range_Dict
{
		float value_multiplier;
		int dp_digit_position;
		string display_unit;
};
// Maps a range setting bitmap pattern to a range setting dictionary
typedef unordered_map<uint8_t, Range_Dict> range_dict_map_t;

// function: dial/pushbutton setting
// subfunction: 
// unit: Single character base unit
// e.g. {"voltage", RANGE_VOLTAGE, "V"}
const struct Function_Dict
{
		string function;
		range_dict_map_t subfunction;
		string unit;
};
// Maps a function setting bitmap pattern to a function and range
typedef unordered_map<uint8_t, Function_Dict> function_dict_map_t;
typedef unordered_map<uint8_t, string> status_map_t;
typedef unordered_map<string, uint8_t> bit_map_t;
typedef tuple<string, uint8_t> bit_tuple_t;

struct packet_bytes_t
{
		uint8_t d_range, d_digit4, d_digit3, d_digit2, d_digit1, d_digit0, d_function, d_status, d_option1, d_option2, d_option3, d_option4;
};

union packet_u_t
{
		char char_packet[12];
    uint8_t raw_packet[12];
		packet_bytes_t pb;
};

class UT61E_DISP {
	private:
		packet_u_t packet;
		bool _parse(bool);
		bit_map_t get_bits(uint8_t b, status_map_t bitmap);
		stringstream results;
	public:
// ut61e class to map data packet to display value and flags
// extern const range_dict_map_t  RANGE_VOLTAGE;
static const range_dict_map_t RANGE_VOLTAGE; // = {
//     {0b0110000, {1e0, 4, "V"}},  //2.2000V
//     {0b0110001, {1e0, 3, "V"}},  //22.000V
//     {0b0110010, {1e0, 2, "V"}},  //220.00V
//     {0b0110011, {1e0, 1, "V"}},  //2200.0V
//     {0b0110100, {1e-3, 2,"mV"}} //220.00mV
// };

// // undocumented in datasheet
static const range_dict_map_t RANGE_CURRENT_AUTO_UA;
// static inline const range_dict_map_t RANGE_CURRENT_AUTO_UA = {
//     {0b0110000, {1e-6, 2, "µA"}}, //
//     {0b0110001, {1e-6, 1, "µA"}} //2
// };

// // undocumented in datasheet
static const range_dict_map_t RANGE_CURRENT_AUTO_MA;
// const range_dict_map_t RANGE_CURRENT_AUTO_MA = {
//     {0b0110000, {1e-3, 3, "mA"}}, //
//     {0b0110001, {1e-3, 2, "mA"}} //2
// };

// //2-range auto A *It includes auto μA, mA, 22.000A/220.00A, 220.00A/2200.0A.
static const unordered_map<uint8_t, const char*> RANGE_CURRENT_AUTO;
// const unordered_map<uint8_t, const char*> RANGE_CURRENT_AUTO = { 
//     {0b0110000, "Lower Range (IVSL)"}, //Current measurement input for 220μA, 22mA.
//     {0b0110001, "Higher Range (IVSH)"} //Current measurement input for 2200μA, 220mA and 22A modes.
// };

static const range_dict_map_t RANGE_CURRENT_22A;
// const range_dict_map_t RANGE_CURRENT_22A = { 
//     {0b0110000, {1e0, 3, "A"}}
// }; //22.000 A

static const range_dict_map_t RANGE_CURRENT_MANUAL;
// const range_dict_map_t RANGE_CURRENT_MANUAL = {
//     {0b0110000, {1e0, 4, "A"}}, //2.2000A
//     {0b0110001, {1e0, 3, "A"}}, //22.000A
//     {0b0110010, {1e0, 2, "A"}}, //220.00A
//     {0b0110011, {1e0, 1, "A"}}, //2200.0A
//     {0b0110100, {1e0, 0, "A"}} //22000A
// };

static const range_dict_map_t RANGE_ADP;
// const range_dict_map_t RANGE_ADP = {
//     {0b0110000, {0,0,"ADP4"}},
//     {0b0110001, {0,0,"ADP3"}},
//     {0b0110010, {0,0,"ADP2"}},
//     {0b0110011, {0,0,"ADP1"}},
//     {0b0110100, {0,0,"ADP0"}}
// };

static const range_dict_map_t RANGE_RESISTANCE;
// const range_dict_map_t RANGE_RESISTANCE = {
//     {0b0110000, {1e0, 2, "Ω"}}, //220.00Ω
//     {0b0110001, {1e3, 4, "kΩ"}}, //2.2000KΩ
//     {0b0110010, {1e3, 3, "kΩ"}}, //22.000KΩ
//     {0b0110011, {1e3, 2, "kΩ"}}, //220.00KΩ
//     {0b0110100, {1e6, 4, "MΩ"}}, //2.2000MΩ
//     {0b0110101, {1e6, 3, "MΩ"}}, //22.000MΩ
//     {0b0110110, {1e6, 2, "MΩ"}} //220.00MΩ
// };

static const range_dict_map_t RANGE_FREQUENCY;
// const range_dict_map_t RANGE_FREQUENCY = {
//     {0b0110000, {1e0, 2, "Hz"}}, //22.00Hz
//     {0b0110001, {1e0, 1, "Hz"}}, //220.0Hz
//     //0b0110010
//     {0b0110011, {1e3, 3, "kHz"}}, //22.000KHz
//     {0b0110100, {1e3, 2, "kHz"}}, //220.00KHz
//     {0b0110101, {1e6, 4, "MHz"}}, //2.2000MHz
//     {0b0110110, {1e6, 3, "MHz"}}, //22.000MHz
//     {0b0110111, {1e6, 2, "MHz"}} //220.00MHz
// };

static const range_dict_map_t RANGE_CAPACITANCE;
// const range_dict_map_t RANGE_CAPACITANCE = {
//     {0b0110000, {1e-9, 3, "nF"}}, //22.000nF
//     {0b0110001, {1e-9, 2, "nF"}}, //220.00nF
//     {0b0110010, {1e-6, 4, "µF"}}, //2.2000μF
//     {0b0110011, {1e-6, 3, "µF"}}, //22.000μF
//     {0b0110100, {1e-6, 2, "µF"}}, //220.00μF
//     {0b0110101, {1e-3, 4, "mF"}}, //2.2000mF
//     {0b0110110, {1e-3, 3, "mF"}}, //22.000mF
//     {0b0110111, {1e-3, 2, "mF"}} //220.00mF
// };

// // When the meter operates in continuity mode or diode mode, this packet is always
// // 0110000 since the full-scale ranges in these modes are fixed.
// const range_dict_map_t RANGE_DIODE = {
static const range_dict_map_t RANGE_DIODE;
//     {0b0110000, {1e0, 4, "V"}}  //2.2000V
// };

static const range_dict_map_t RANGE_CONTINUITY;
// const range_dict_map_t RANGE_CONTINUITY = {
//     {0b0110000, {1e0, 2, "Ω"}} //220.00Ω
// };

static const range_dict_map_t RANGE_NULL;
// const range_dict_map_t RANGE_NULL = {
//     {0b0110000, {1e0, 2, "Ω"}} //220.00Ω
// };

static const function_dict_map_t DIAL_FUNCTION;
// function_dict_map_t DIAL_FUNCTION = {
//     // (function, subfunction, unit)
//     {0b0111011, {"voltage", RANGE_VOLTAGE, "V"}},
//     {0b0111101, {"current", RANGE_CURRENT_AUTO_UA, "A"}}, //Auto μA Current / Auto μA Current / Auto 220.00A/2200.0A
//     {0b0111111, {"current", RANGE_CURRENT_AUTO_MA, "A"}}, //Auto mA Current   Auto mA Current   Auto 22.000A/220.00A
//     {0b0110000, {"current", RANGE_CURRENT_22A, "A"}}, //22 A current
//     {0b0111001, {"current", RANGE_CURRENT_MANUAL, "A"}}, //Manual A Current
//     {0b0110011, {"resistance", RANGE_RESISTANCE, "Ω"}},
//     {0b0110101, {"continuity", RANGE_CONTINUITY, "Ω"}},
//     {0b0110001, {"diode", RANGE_DIODE, "V"}},
//     {0b0110010, {"frequency", RANGE_FREQUENCY, "Hz"}},
//     {0b0110110, {"capacitance", RANGE_CAPACITANCE, "F"}},
//     {0b0110100, {"temperature", RANGE_NULL, "deg"}},
//     {0b0111110, {"ADP", RANGE_ADP, ""}}
// }; 

static const unordered_map<uint8_t, int> LCD_DIGITS;
// static const unordered_map<uint8_t, int> LCD_DIGITS = {
//     {0b0110000, 0},
//     {0b0110001, 1},
//     {0b0110010, 2},
//     {0b0110011, 3},
//     {0b0110100, 4},
//     {0b0110101, 5},
//     {0b0110110, 6},
//     {0b0110111, 7},
//     {0b0111000, 8},
//     {0b0111001, 9}
// };

// // typedef unordered_map<uint8_t, string> status_map_t;

static const status_map_t STATUS;
// const status_map_t STATUS = 
// {
//     {0b1000000, "0"},
//     {0b0100000, "1"},
//     {0b0010000, "1"},
//     {0b0001000, "JUDGE"},// 1-°C, 0-°F.
//     {0b0000100, "SIGN"}, // 1-minus sign, 0-no sign
//     {0b0000010, "BATT"}, // 1-battery low
//     {0b0000001, "OL"}   // input overflow
// };

static const status_map_t OPTION1;
// const status_map_t OPTION1 =
// {
//     {0b1000000, "0"}, // 0
//     {0b0100000, "1"}, // 1
//     {0b0010000, "1"}, // 1
//     {0b0001000, "JUDGE"},// 1-°C, 0-°F.
//     {0b0000100, "SIGN"}, // 1-minus sign, 0-no sign
//     {0b0000010, "BATT"}, // 1-battery low
//     {0b0000001, "OL"}   // input overflow
// };

static const status_map_t OPTION2;
// const status_map_t OPTION2 = {
//     {0b1000000, "0"},   // 0
//     {0b0100000, "1"},   // 1
//     {0b0010000, "1"},   // 1
//     {0b0001000, "UL"},   // 1 -at 22.00Hz <2.00Hz., at 220.0Hz <20.0Hz,duty cycle <10.0%.
//     {0b0000100, "PMAX"}, // maximum peak value
//     {0b0000010, "PMIN"}, // minimum peak value
//     {0b0000001, "0"}   // 0
// };

static const status_map_t OPTION3;
// const status_map_t OPTION3 = 
// {
//     {0b1000000, "0"},   // 0
//     {0b0100000, "1"},   // 1
//     {0b0010000, "1"},   // 1
//     {0b0001000, "DC"},   // DC measurement mode, either voltage or current.
//     {0b0000100, "AC"},   // AC measurement mode, either voltage or current.
//     {0b0000010, "AUTO"}, // 1-automatic mode, 0-manual
//     {0b0000001, "VAHZ"}
// };
static const status_map_t OPTION4; 
// const status_map_t OPTION4 = 
// {
//     {0b1000000, "0"},   // 0
//     {0b0100000, "1"},   // 1
//     {0b0010000, "1"},   // 1
//     {0b0001000, "0"},   // 0
//     {0b0000100, "VBAR"}, // 1-VBAR pin is connected to V-.
//     {0b0000010, "HOLD"}, // hold mode
//     {0b0000001, "LPF"}  // low-pass-filter feature is activated.
// };

	// public:
    float value; //float
    string unit; //string
    float display_value; //float
    string display_unit; // string
    string mode; // string
    string currentType; //string
    string peak; //string
    bool relative; //bool
    bool hold; //bool
    string mrange; //string
    string operation; //string
    bool battery_low; //bool

		UT61E_DISP(/* args */) { }
		~UT61E_DISP() { }

		bool parse(char const *, bool);
		bool parse(uint8_t const *, bool);
		const char *get(); // Format results into a string stream then return its .str

};

#endif /* UT61E_DISP_H_ */
