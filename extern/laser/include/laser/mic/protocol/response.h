#pragma once

#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct response
{
	enum type : ::std::uint8_t
	{
		error = 0xff,
	};

	constexpr explicit(false) response(type value) : _value(value) {}

	constexpr auto&& value() const { return _value; }

	type _value;
};

NGS_LIB_MODULE_END