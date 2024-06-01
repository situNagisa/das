#pragma once

#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct length
{
	enum type : ::std::uint8_t {};

	constexpr static auto max() { return sizeof(type); }

	constexpr explicit(false) length(type value) : _value(value) {}

	constexpr auto&& value() const { return _value; }

	type _value;
};

NGS_LIB_MODULE_END