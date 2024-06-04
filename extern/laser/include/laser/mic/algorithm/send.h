#pragma once

#include "../protocol.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

constexpr auto send_size(::std::size_t data_size = 0)
{
	return sizeof(protocols::flag) + sizeof(protocols::length) + sizeof(protocols::address) + sizeof(protocols::command) + data_size + sizeof(protocols::checksum);
}

auto send(protocols::address address, protocols::command command, ::std::output_iterator<::std::uint8_t> auto&& result, ::std::ranges::input_range auto&& data)
	requires ::std::convertible_to<::std::ranges::range_value_t<decltype(data)>, uint8_t>&& ::std::indirectly_copyable<::std::ranges::iterator_t<decltype(data)>,decltype(result)>
{
	constexpr auto flag = protocols::flag(protocols::flag::send);
	NGS_ASSERT(
		(::std::ranges::size(data) + sizeof(protocols::address) + sizeof(protocols::command) + sizeof(protocols::checksum)) < protocols::length::max(),
		::std::format("data size is too large: {}", ::std::ranges::size(data))
	);
	auto length = protocols::length(static_cast<::std::uint8_t>(::std::ranges::size(data) + sizeof(protocols::address) + sizeof(protocols::command) + sizeof(protocols::checksum)));
	auto checksum = protocols::checksum(protocols::flag::send, length, address, command, ::std::ranges::fold_left(data, static_cast<::std::uint8_t>(0), ::std::plus{}));

	auto flag_value = ::std::array<::std::uint8_t, sizeof(decltype(flag.value()))>{flag.value() & 0xff, (flag.value() >> 8) & 0xff};
	result = ::std::ranges::copy(flag_value, result).out;

	*result = length.value();
	++result;

	*result = address.value();
	++result;

	*result = static_cast<::std::uint8_t>(command);
	++result;

	result = ::std::ranges::copy(data, result).out;

	*result = checksum.value();
	++result;

	return result;
}

auto send(protocols::address address, protocols::command command, ::std::output_iterator<::std::uint8_t> auto&& result)
{
	return NGS_LIB_MODULE_NAME::send(address, command, NGS_PP_PERFECT_FORWARD(result), ::std::array<::std::uint8_t, 0>{});
}

auto send(protocols::command command, ::std::output_iterator<::std::uint8_t> auto&& result, ::std::ranges::input_range auto&& data)
	requires ::std::convertible_to<::std::ranges::range_value_t<decltype(data)>, uint8_t>&& ::std::indirectly_copyable<::std::ranges::range_value_t<decltype(data)>, decltype(result)>
{
	return NGS_LIB_MODULE_NAME::send(protocols::address::any, command, NGS_PP_PERFECT_FORWARD(result), NGS_PP_PERFECT_FORWARD(data));
}

auto send(protocols::command command, ::std::output_iterator<::std::uint8_t> auto&& result)
{
	return NGS_LIB_MODULE_NAME::send(protocols::address::any, command, NGS_PP_PERFECT_FORWARD(result), ::std::array<::std::uint8_t, 0>{});
}

NGS_LIB_MODULE_END
