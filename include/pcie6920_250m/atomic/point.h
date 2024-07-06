#pragma once

#include "../enum.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN


template<enums::parse_rule ParseRule>
struct point;

template<>
struct point<enums::parse_rule::raw_data>
{
	::std::int16_t data;

	constexpr operator auto&&() const { return data; }
};

template<>
struct point<enums::parse_rule::iq>
{
	::std::int16_t i;
	::std::int16_t q;
};

template<>
struct point<enums::parse_rule::amplitude_phase>
{
	::std::int16_t amplitude;
	::std::uint16_t phase;
};

NGS_LIB_MODULE_END