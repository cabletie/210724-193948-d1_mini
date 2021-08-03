/*
 * ut61e_display.cpp
 *
 *  Created on: 12.11.2009
 *      Author: steffen vogel
 *         git: https://github.com/stv0g/dmm_ut61e
 */

#include "ut61e_display.hpp"
#include <exception>
#include <cstdlib>
#include <unordered_map>
#include <vector>
#include <string>
#include <tuple>
#include <cstring>
#include <sstream>
#include <math.h>

using std::unordered_map;
using std::vector;
using std::string;
using std::tuple;
using std::stringstream;

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
        display_value += digit_array[i]*(std::pow(10,i));
    if(options["SIGN"])
        display_value = -display_value;
    display_value = display_value / pow(10,m_range.dp_digit_position);
    display_unit = m_range.display_unit;
    value = float(display_value) * m_range.value_multiplier;
    
    if(operation != "normal"){
        display_value = 0;
        value = 0;
        return false;
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