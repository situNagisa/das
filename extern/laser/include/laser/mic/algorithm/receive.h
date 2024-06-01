#pragma once

#include "../protocol.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

auto receive()
{
	auto flag = protocols::flag(protocols::flag::receive);
	auto length = protocols::length();
	auto address = protocols::address();
	auto response = protocols::response();
	auto data = ::std::vector<::std::uint8_t>();
	auto checksum = protocols::checksum();

	return {address, response, data};
}

auto receive(protocols::address address, protocols::response response, ::std::ranges::input_range auto&& data, ::std::output_iterator<::std::uint8_t> auto&& result)
	requires ::std::convertible_to<::std::ranges::range_value_t<decltype(data)>, uint8_t>&& ::std::indirectly_copyable<::std::ranges::range_value_t<decltype(data)>, decltype(result)>
{
	constexpr auto flag = protocols::flag(protocols::flag::send);
	NGS_ASSERT(
		(::std::ranges::size(data) + sizeof(protocols::address) + sizeof(protocols::command) + sizeof(protocols::checksum)) < protocols::length::max(),
		::std::format("data size is too large: {}", ::std::ranges::size(data))
	);
	auto length = protocols::length(static_cast<::std::uint8_t>(::std::ranges::size(data) + sizeof(protocols::address) + sizeof(protocols::response) + sizeof(protocols::checksum)));
	auto checksum = protocols::checksum(protocols::flag::send, length, address, response);

	auto flag_value = ::std::array<::std::uint8_t, sizeof(decltype(flag.value()))>{flag.value() & 0xff, (flag.value() >> 8) & 0xff};
	result = ::std::ranges::copy(flag_value, result);

	*result = length.value();
	++result;

	*result = address.value();
	++result;

	*result = response.value();
	++result;

	result = ::std::ranges::copy(data, result);

	*result = checksum.value();
	++result;

	return result;
}

NGS_LIB_MODULE_END