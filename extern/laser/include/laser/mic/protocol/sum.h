#pragma once

#include "./flag.h"
#include "./address.h"
#include "./command.h"
#include "./response.h"
#include "./length.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct checksum
{
	constexpr explicit(false) checksum(flag flag, length length, address address, command command, ::std::uint8_t data = {})
		: _value(static_cast<::std::uint8_t>(flag.value() >> 8) + static_cast<::std::uint8_t>(flag.value()) + length.value() + address.value() + static_cast<::std::uint8_t>(command))
	{}

	constexpr explicit(false) checksum(flag flag, length length, address address, response response, ::std::uint8_t data = {})
		: _value(static_cast<::std::uint8_t>(flag.value() >> 8) + static_cast<::std::uint8_t>(flag.value()) + length.value() + address.value() + response.value())
	{}

	constexpr auto&& value() const { return _value; }

	::std::uint8_t _value;
};

NGS_LIB_MODULE_END
