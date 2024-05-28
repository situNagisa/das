#pragma once

#include "../enum.h"
#include "./unit.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

template<enums::parse_rule ParseRule, enums::channel_quantity Quantity>
struct packet : ::ngs::external::stl::ranges::range_interface<unit<ParseRule, Quantity>>
{
	using unit_type = unit<ParseRule,Quantity>;

	constexpr auto begin() { return _data.begin(); }
	constexpr auto begin() const { return _data.begin(); }
	constexpr auto end() { return _data.end(); }
	constexpr auto end() const { return _data.end(); }

	::std::array<unit_type, 0x100> _data;
};

NGS_LIB_MODULE_END