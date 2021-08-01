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

using std::unordered_map;
using std::string;
using std::tuple;

#ifndef UT61E_DISP_H_
#define UT61E_DISP_H_

// Range setting 
// value_multiplier:  Multiply the displayed value by this factor to get the value in base units.
// dp_digit_position: The digit position of the decimal point in the displayed meter reading value.
// display_unit:      The unit the displayed value is shown in.
// e.g. {1e-9, 3, "nF"}
struct Range_Dict
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
struct Function_Dict
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

typedef struct packet_bytes_t
{
		uint8_t d_range, d_digit4, d_digit3, d_digit2, d_digit1, d_digit0, d_function, d_status, d_option1, d_option2, d_option3, d_option4;
};

union packet_u_t
{
    uint8_t raw_packet[12];
		packet_bytes_t pb;
};

class ut61e_display {
public:
	ut61e_display();
	virtual ~ut61e_display();
	void ut61e_display::parse(packet_u_t, bool);
	string &ut61e_display::get();
private:
};

#endif /* UT61E_DISP_H_ */
