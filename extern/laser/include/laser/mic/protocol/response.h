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
	constexpr explicit(true) response(::std::uint8_t value) : _value(static_cast<type>(value)) {}

	constexpr auto&& value() const { return _value; }

	constexpr bool operator==(const response&) const = default;

	type _value;
};

NGS_LIB_MODULE_END