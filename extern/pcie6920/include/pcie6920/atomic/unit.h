#pragma once

#include "../enum.h"
#include "./point.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN


template<enums::parse_rule ParseRule, enums::channel_quantity Quantity>
struct unit : ::ngs::external::stl::ranges::range_interface<unit<ParseRule,Quantity>>
{
	using point_type = point<ParseRule>;

	constexpr auto begin() { return _data.begin(); }
	constexpr auto begin() const { return _data.begin(); }
	constexpr auto end() { return _data.end(); }
	constexpr auto end() const { return _data.end(); }

	::std::array<point_type, (ParseRule == enums::parse_rule::raw_data) ? 2 : static_cast<::std::size_t>(Quantity)> _data;
};

NGS_LIB_MODULE_END