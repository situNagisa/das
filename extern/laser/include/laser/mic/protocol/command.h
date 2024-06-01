#pragma once

#include "./defined.h"

NGS_LIB_MODULE_BEGIN

enum class command : ::std::uint8_t
{
	read_all_parameters = 0x00,
	read_serial_number = 0x01,
	read_alarm = 0x02,
	read_temperature = 0x03,
	read_voa = 0xc8,
	set_pump_bias_current = 0x15,
	set_voa = 0xc8,
};

NGS_LIB_MODULE_END