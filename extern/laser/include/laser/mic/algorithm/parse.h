#pragma once

#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct all_parameter
{
	::std::uint8_t sn1;
	::std::uint8_t sn2;
	::std::uint8_t sn3;

	::std::uint8_t alarm_pump_1_cooler : 1;
	::std::uint8_t alarm_pump_1_chip_temperature : 1;
	::std::uint8_t : 1;
	::std::uint8_t alarm_pump_1_lop : 1;
	::std::uint8_t  : 1;
	::std::uint8_t alarm_temperature : 1;
	::std::uint8_t alarm_output_power : 1;
	::std::uint8_t alarm_input_power : 1;

	::std::uint8_t : 1;
	::std::uint8_t alarm_pump_status : 1;
	::std::uint8_t : 1;
	::std::uint8_t : 1;
	::std::uint8_t alarm_pump_2_cooler : 1;
	::std::uint8_t alarm_pump_2_chip_temperature : 1;
	::std::uint8_t : 1;
	::std::uint8_t alarm_pump_2_lop : 1;

	::std::uint8_t : 8;

	::std::int16_t temperature;

	enum class working_mode_type : ::std::uint8_t
	{
		acc,
		agc,
		apc,
	}working_mode;
	::std::uint8_t working_parameter;

	::std::uint16_t input_power;
	::std::uint16_t output_power;
	::std::uint16_t input_power_threshold;
	::std::uint16_t output_power_threshold;

	::std::uint16_t pump_1_lop;
	::std::uint16_t pump_1_power;
	::std::uint16_t pump_1_chip_temperature;
	::std::uint16_t pump_1_cooler;
	::std::uint16_t pump_2_lop;
	::std::uint16_t pump_2_power;
	::std::uint16_t pump_2_chip_temperature;
	::std::uint16_t pump_2_cooler;

	::std::uint16_t : 16;
	::std::uint16_t : 16;
	::std::uint16_t : 16;
	::std::uint16_t : 16;

	::std::uint16_t : 16;
	::std::uint16_t : 16;
	::std::uint16_t : 16;
	::std::uint16_t : 16;
	::std::uint16_t : 16;
	::std::uint16_t : 16;
};

NGS_LIB_MODULE_END