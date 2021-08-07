/*
 * ut61e_display.cpp
 *
 *  Created on: 12.11.2009
 *      Author: steffen vogel
 *         git: https://github.com/stv0g/dmm_ut61e
 */

#include "ut61e_display.h"
#include <exception>
// #include <cstdlib>
// #include <unordered_map>
#include <vector>
// #include <string>
// #include <tuple>
#include <cstring>
// #include <sstream>
#include <math.h>

using std::unordered_map;
using std::vector;
// using std::string;
// using std::tuple;
// using std::stringstream;
UT61E_DISP::UT61E_DISP() {};
UT61E_DISP::UT61E_DISP(HardwareSerial &s):serial(&s) {};
bit_map_t UT61E_DISP::get_bits(uint8_t b, status_map_t bitmap)
{
    // """
    // Extracts 'named bits' from int_type.
    // Naming the bits works by supplying a list of
    // bit names (or fixed bits as 0/1) via template.
    // """

    // bit_map* bits = new bit_map(); // maybe
    bit_map_t bits;
    for (uint8_t i=0;i<7;i++){
        bool bit = b & 1<<i;
        string bit_name = bitmap[6-i];
        // #print(bit, bit_name, i)
        if(((bit_name == "0") & !bit) or ((bit_name == "1") & bit))
            continue;
        else
        {
            if ((bit_name == "0") or (bit_name == "1"))
                std::exception(); // The 0 or 1 bits aren't 0 or 1...
            else 
                bits[bit_name] = bit;
        }
    }
    return(bits);
};

bool UT61E_DISP::parse(char const c[12], bool e){
    strncpy(packet.char_packet,c,12);
    return _parse(e);
};

bool UT61E_DISP::parse(uint8_t const u[12], bool e){
    memcpy(packet.raw_packet,u,12);
    return _parse(e);
};
// ut61e class to map data packet to display value and flags
range_dict_map_t UT61E_DISP::RANGE_VOLTAGE = {
    {0b0110000, {1e0, 4, "V"}},  //2.2000V
    {0b0110001, {1e0, 3, "V"}},  //22.000V
    {0b0110010, {1e0, 2, "V"}},  //220.00V
    {0b0110011, {1e0, 1, "V"}},  //2200.0V
    {0b0110100, {1e-3, 2,"mV"}} //220.00mV
};

// undocumented in datasheet
range_dict_map_t UT61E_DISP::RANGE_CURRENT_AUTO_UA = {
    {0b0110000, {1e-6, 2, "µA"}}, //
    {0b0110001, {1e-6, 1, "µA"}} //2
};

// undocumented in datasheet
range_dict_map_t UT61E_DISP::RANGE_CURRENT_AUTO_MA = {
    {0b0110000, {1e-3, 3, "mA"}}, //
    {0b0110001, {1e-3, 2, "mA"}} //2
};

//2-range auto A *It includes auto μA, mA, 22.000A/220.00A, 220.00A/2200.0A.
unordered_map<uint8_t, const char*> UT61E_DISP::RANGE_CURRENT_AUTO = { 
    {0b0110000, "Lower Range (IVSL)"}, //Current measurement input for 220μA, 22mA.
    {0b0110001, "Higher Range (IVSH)"} //Current measurement input for 2200μA, 220mA and 22A modes.
};

range_dict_map_t UT61E_DISP::RANGE_CURRENT_22A = { 
    {0b0110000, {1e0, 3, "A"}}
}; //22.000 A

range_dict_map_t UT61E_DISP::RANGE_CURRENT_MANUAL = {
    {0b0110000, {1e0, 4, "A"}}, //2.2000A
    {0b0110001, {1e0, 3, "A"}}, //22.000A
    {0b0110010, {1e0, 2, "A"}}, //220.00A
    {0b0110011, {1e0, 1, "A"}}, //2200.0A
    {0b0110100, {1e0, 0, "A"}} //22000A
};

range_dict_map_t UT61E_DISP::RANGE_ADP = {
    {0b0110000, {0,0,"ADP4"}},
    {0b0110001, {0,0,"ADP3"}},
    {0b0110010, {0,0,"ADP2"}},
    {0b0110011, {0,0,"ADP1"}},
    {0b0110100, {0,0,"ADP0"}}
};

range_dict_map_t UT61E_DISP::RANGE_RESISTANCE = {
    {0b0110000, {1e0, 2, "Ω"}}, //220.00Ω
    {0b0110001, {1e3, 4, "kΩ"}}, //2.2000KΩ
    {0b0110010, {1e3, 3, "kΩ"}}, //22.000KΩ
    {0b0110011, {1e3, 2, "kΩ"}}, //220.00KΩ
    {0b0110100, {1e6, 4, "MΩ"}}, //2.2000MΩ
    {0b0110101, {1e6, 3, "MΩ"}}, //22.000MΩ
    {0b0110110, {1e6, 2, "MΩ"}} //220.00MΩ
};

range_dict_map_t UT61E_DISP::RANGE_FREQUENCY = {
    {0b0110000, {1e0, 2, "Hz"}}, //22.00Hz
    {0b0110001, {1e0, 1, "Hz"}}, //220.0Hz
    //0b0110010
    {0b0110011, {1e3, 3, "kHz"}}, //22.000KHz
    {0b0110100, {1e3, 2, "kHz"}}, //220.00KHz
    {0b0110101, {1e6, 4, "MHz"}}, //2.2000MHz
    {0b0110110, {1e6, 3, "MHz"}}, //22.000MHz
    {0b0110111, {1e6, 2, "MHz"}} //220.00MHz
};

range_dict_map_t UT61E_DISP::RANGE_CAPACITANCE = {
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
range_dict_map_t UT61E_DISP::RANGE_DIODE = {
    {0b0110000, {1e0, 4, "V"}}  //2.2000V
};

range_dict_map_t UT61E_DISP::RANGE_CONTINUITY = {
    {0b0110000, {1e0, 2, "Ω"}} //220.00Ω
};

range_dict_map_t UT61E_DISP::RANGE_NULL = {
    {0b0110000, {1e0, 2, "Ω"}} //220.00Ω
};

function_dict_map_t  UT61E_DISP::DIAL_FUNCTION = {
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

unordered_map<uint8_t, int> UT61E_DISP::LCD_DIGITS = {
    {0b0110000, 0},
    {0b0110001, 1},
    {0b0110010, 2},
    {0b0110011, 3},
    {0b0110100, 4},
    {0b0110101, 5},
    {0b0110110, 6},
    {0b0110111, 7},
    {0b0111000, 8},
    {0b0111001, 9}
};

// typedef unordered_map<uint8_t, string> status_map_t;

status_map_t UT61E_DISP::STATUS = 
{
    {0b1000000, "0"},
    {0b0100000, "1"},
    {0b0010000, "1"},
    {0b0001000, "JUDGE"},// 1-°C, 0-°F.
    {0b0000100, "SIGN"}, // 1-minus sign, 0-no sign
    {0b0000010, "BATT"}, // 1-battery low
    {0b0000001, "OL"}   // input overflow
};
status_map_t UT61E_DISP::OPTION1 =
{
    {0b1000000, "0"}, // 0
    {0b0100000, "1"}, // 1
    {0b0010000, "1"}, // 1
    {0b0001000, "JUDGE"},// 1-°C, 0-°F.
    {0b0000100, "SIGN"}, // 1-minus sign, 0-no sign
    {0b0000010, "BATT"}, // 1-battery low
    {0b0000001, "OL"}   // input overflow
};
status_map_t UT61E_DISP::OPTION2 = {
    {0b1000000, "0"},   // 0
    {0b0100000, "1"},   // 1
    {0b0010000, "1"},   // 1
    {0b0001000, "UL"},   // 1 -at 22.00Hz <2.00Hz., at 220.0Hz <20.0Hz,duty cycle <10.0%.
    {0b0000100, "PMAX"}, // maximum peak value
    {0b0000010, "PMIN"}, // minimum peak value
    {0b0000001, "0"}   // 0
};

status_map_t UT61E_DISP::OPTION3 = 
{
    {0b1000000, "0"},   // 0
    {0b0100000, "1"},   // 1
    {0b0010000, "1"},   // 1
    {0b0001000, "DC"},   // DC measurement mode, either voltage or current.
    {0b0000100, "AC"},   // AC measurement mode, either voltage or current.
    {0b0000010, "AUTO"}, // 1-automatic mode, 0-manual
    {0b0000001, "VAHZ"}
};
status_map_t UT61E_DISP::OPTION4 = 
{
    {0b1000000, "0"},   // 0
    {0b0100000, "1"},   // 1
    {0b0010000, "1"},   // 1
    {0b0001000, "0"},   // 0
    {0b0000100, "VBAR"}, // 1-VBAR pin is connected to V-.
    {0b0000010, "HOLD"}, // hold mode
    {0b0000001, "LPF"}  // low-pass-filter feature is activated.
};

// The most important function of this module:
// Parses 12-byte-long packets from the UT61E DMM and returns
// a dictionary with all information extracted from the packet.
bool UT61E_DISP::_parse(bool extended_format = false){
    // an unordered map of bit names and their values
    bit_map_t options;
    options.merge(get_bits(packet.pb.d_status,STATUS));
    options.merge(get_bits(packet.pb.d_option1,OPTION1));
    options.merge(get_bits(packet.pb.d_option2,OPTION2));
    options.merge(get_bits(packet.pb.d_option3,OPTION3));
    options.merge(get_bits(packet.pb.d_option4,OPTION4));
    
    for (auto const &pair: options) {
        serial->printf("{%s: %i}\n",pair.first.c_str(), pair.second);
    }

    Function_Dict dial_function = DIAL_FUNCTION[packet.pb.d_function];
     
    // # When the rotary switch is set to 'voltage' or 'ampere' mode and then you 
    // # press the frequency button, the meter shows 'Hz' (or '%') but the
    // # function byte is still the same as before so we have to correct for that:
    if(options["VAHZ"])
        dial_function = DIAL_FUNCTION[0b0110010];
    mode = dial_function.function;
    Range_Dict m_range = dial_function.subfunction[packet.pb.d_range];
    unit = dial_function.unit;
    if(mode == "frequency" and options["JUDGE"])
    {
        mode = "duty_cycle";
        unit = "%";
        m_range = {1e0, 1, "%"}; // 2200.0°C
    };
    if(options["AC"] and options["DC"])
        std::exception(); // ValueError
    else if (options["DC"])
        currentType = "DC";
    else if (options["AC"])
        currentType = "AC";
    
    operation = "normal";
    // sometimes there a glitch where both UL and OL are enabled in normal operation
    // so no error is raised when it occurs
    if (options["UL"])
        operation = "underload";
    else if (options["OL"])
        operation = "overload";
    
    if (options["AUTO"])
        mrange = "auto";
    else
        mrange = "manual";

    if (options["BATT"])
        battery_low = true;
    else
        battery_low = false;
    
    // relative measurement mode, received value is actual!
    if (options["REL"])
        relative = true;
    else
        relative = false;

    // data hold mode, received value is actual!
    if (options["HOLD"])
        hold = true;
    else
        hold = false;
   
    if (options["MAX"])
        peak = "max";
    else if (options["MIN"])
        peak = "min";
    
    if (mode == "current" and options["VBAR"])
        ;
        // """Auto μA Current
        // Auto mA Current"""
    else if (mode == "current" and not options["VBAR"])
        ;
        // """Auto 220.00A/2200.0A
        // Auto 22.000A/220.00A"""
    
    if (mode == "temperature" and options["VBAR"])
        m_range = {1e0, 1, "deg"}; // 2200.0°C
    else if (mode == "temperature" and not options["VBAR"])
        m_range = {1e0, 2, "deg"}; // 220.00°C and °F

    int d4,d3,d2,d1,d0;
    d4 = LCD_DIGITS[packet.pb.d_digit4];
    d3 = LCD_DIGITS[packet.pb.d_digit3];
    d2 = LCD_DIGITS[packet.pb.d_digit2];
    d1 = LCD_DIGITS[packet.pb.d_digit1];
    d0 = LCD_DIGITS[packet.pb.d_digit0];

    vector<int> digit_array = {d0,d1,d2,d3,d4};

    stringstream display_string;

    display_string << d4 << d3 << d2 << d1 << d0;
    display_string.str().insert(m_range.dp_digit_position,1,'.');
    display_value = 0;
    for (int i = 0; i<5;i++)
        display_value += digit_array[i]*pow(10,i);
    if(options["SIGN"])
        display_value = -display_value;
    display_value = display_value / pow(10,m_range.dp_digit_position);
    display_unit = m_range.display_unit;
    value = float(display_value) * m_range.value_multiplier;
    
    if(operation != "normal"){
        display_value = 0;
        value = 0;
        serial->println(operation.c_str());
        return true;
    }

    // detailed_results = {
    //     'packet_details' : {
    //         'raw_data_binary' :  packet,
    //         'raw_data_hex'    :  ' '.join('0x{:02X}'.format(x) for x in packet),
    //         'data_bytes' : {
    //             'd_range'    :  d_range,
    //             'd_digit4'   :  d_digit4,
    //             'd_digit3'   :  d_digit3,
    //             'd_digit2'   :  d_digit2,
    //             'd_digit1'   :  d_digit1,
    //             'd_digit0'   :  d_digit0,
    //             'd_function' :  d_function,
    //             'd_status'   :  d_status,
    //             'd_option1'  :  d_option1,
    //             'd_option2'  :  d_option2,
    //             'd_option3'  :  d_option3,
    //             'd_option4'  :  d_option4
    //         },
    //         'options' :  options,
    //         'range'   :  {
    //             'value_multiplier' : m_range[0],
    //             'dp_digit_position' : m_range[1],
    //             'display_unit' : m_range[2]
    //         }
    //     },
    //     'display_value' : str(display_value)
    // }
    return true;
};

const char *UT61E_DISP::get(){
    results.clear();
    results << 
        "value:" << value <<
        ",unit:" << unit << 
        ",display_value:" << display_value <<
        ",display_unit:" << display_unit <<
        ",mode:" << mode <<
        ",currentType:" << currentType <<
        ",peak:" << peak <<
        ",relative:" << relative <<
        ",hold:" << hold <<
        ",range:" << mrange <<
        ",operation:" << operation <<
        ",battery_low:" << battery_low;
 //   results_string = results.str();
    return results.str().c_str();
}