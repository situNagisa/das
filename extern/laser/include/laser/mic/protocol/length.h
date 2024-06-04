#pragma once

#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct length
{
	constexpr static auto max() { return ::std::numeric_limits<::std::uint8_t>::max(); }

	constexpr explicit(false) length(::std::uint8_t value) : _value(value) {}

	constexpr auto&& value() const { return _value; }

	::std::uint8_t _value;
};

NGS_LIB_MODULE_END