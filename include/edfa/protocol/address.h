#pragma once

#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct address
{
	enum type : ::std::uint8_t
	{
		any = 0xff,
	};

	constexpr explicit(false) address(type value) : _value(value) {}

	constexpr auto&& value() const { return _value; }

	type _value;
};

NGS_LIB_MODULE_END