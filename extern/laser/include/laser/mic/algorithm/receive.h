#pragma once

#include "../protocol.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

constexpr ::std::size_t receive_size(::std::size_t data_size)
{
	return sizeof(protocols::flag) + sizeof(protocols::length) + sizeof(protocols::address) + sizeof(protocols::response) + data_size + sizeof(protocols::checksum);
}


::std::span<const ::std::uint8_t> receive_check(protocols::command command, ::std::ranges::contiguous_range auto&& data)
	requires ::ngs::cpt::object_same_as<::std::ranges::range_value_t<decltype(data)>, uint8_t>
{
	NGS_ASSERT(::std::ranges::size(data) >= NGS_LIB_MODULE_NAME::receive_size(0), ::std::format("data size is too small: {}", ::std::ranges::size(data)));
	auto result = ::std::ranges::begin(data);
	::std::array<::std::uint8_t, sizeof(protocols::flag)> flag_bytes{};
	{
		result = ::std::ranges::copy_n(::std::ranges::begin(data), sizeof(protocols::flag), flag_bytes.begin()).in;
		if(static_cast<protocols::flag::type>(static_cast<::std::uint16_t>(flag_bytes[1] << 8) & flag_bytes[0]) != protocols::flag::receive)
		{
			return {};
		}
	}
	::std::uint8_t length{};
	{
		result = ::std::ranges::copy_n(result, sizeof(protocols::length), &length).in;
		if(length != ::std::ranges::distance(result, ::std::ranges::end(data)))
		{
			return {};
		}
	}
	::std::uint8_t address{};
	{
		result = ::std::ranges::copy_n(result, sizeof(protocols::address), &address).in;
	}
	::std::uint8_t response{};
	{
		result = ::std::ranges::copy_n(result, sizeof(protocols::response), &response).in;
		if (protocols::response(response) == protocols::response::error || response != static_cast<::std::uint8_t>(command))
		{
			return {};
		}
	}
	::std::span<const ::std::uint8_t> result_data{};
	{
		auto data_size = length - sizeof(protocols::address) - sizeof(protocols::response) - sizeof(protocols::checksum);
		result_data = { ::std::ranges::data(data) + ::std::ranges::distance(::std::ranges::begin(data), result), data_size};
		result = ::std::ranges::next(result, data_size);
	}
	{
		::std::uint8_t checksum{};
		result = ::std::ranges::copy_n(result, sizeof(protocols::checksum), &checksum).in;
		if (checksum != (flag_bytes[0] + flag_bytes[1] + length + address + response + ::std::ranges::fold_left(result_data, static_cast<::std::uint8_t>(0), ::std::plus{})))
		{
			return {};
		}
	}
	return result_data;
}

NGS_LIB_MODULE_END