#pragma once

#include "../enum.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct info
{
	enums::parse_rule parse_rule{};
	enums::upload_rate upload_rate{};
	enums::channel_quantity channel_quantity{};
	::std::size_t scan_rate{};
	::std::size_t pulse_width{};
	::std::size_t packet_size{};
	::std::size_t center_frequency{};
};

NGS_LIB_MODULE_END