#pragma once

#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct flag
{
	enum type : ::std::uint16_t
	{
		send = 0xefef,
		receive = 0xfaed,
	};

	constexpr explicit(false) flag(type value) : _value(value) {}

	constexpr auto&& value() const { return _value; }

	type _value;
};

NGS_LIB_MODULE_END