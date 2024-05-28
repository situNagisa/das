#pragma once

#include "../enum.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct info
{
	enums::channel_quantity demodulation_channel_quantity;
	::std::uint32_t points_per_scan;
	::std::uint32_t scan_rate;
	::std::uint32_t trigger_pulse_width;
	::std::uint32_t center_frequency;
	enums::parse_rule data_source_sel;
	enums::upload_rate upload_rate_sel;
};

NGS_LIB_MODULE_END