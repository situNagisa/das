#pragma once

#include "./defined.h"

NGS_LIB_MODULE_BEGIN

enum class command : ::std::uint8_t
{
	read_all_parameters = 0x00,
	read_serial_number = 0x01,
	read_alarm = 0x02,
	read_temperature = 0x03,

	read_pumps_number = 0x10,
	read_pumps_1_parameters,
	read_pumps_2_parameters,
	set_pump_current = 0x15,
	set_gain = 0x17,
	set_output_power,
	set_and_read_temperature_alarm_threshold,
	arp_function,
	set_and_read_pump_status,
	no_automatic_pump_off = 0x1e,

	read_power = 0x20,
	read_gain,

	read_working = 0x30,

	set_working_mode = 0x40,
	set_input_power_alarm_threshold,
	set_output_power_alarm_threshold,

	set_the_input_dull_pipe_pump_threshold = 0xb2,

	reset = 0xc0,
	information = 0xc3,

	set_pump_current_alarm_threshold = 0xd1,
	read_pump_current_alarm_threshold = 0xe1,
};

NGS_LIB_MODULE_END