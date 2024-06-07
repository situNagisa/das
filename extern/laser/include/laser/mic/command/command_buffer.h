#pragma once

#include "../protocol.h"
#include "../algorithm.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

template<protocols::command Command>
constexpr auto send_buffer(const ::ngs::external::stl::ranges::algorithm::static_sized_range auto& data)
{
	constexpr auto data_size = ::ngs::external::stl::ranges::algorithm::static_size<decltype(data)>();
	constexpr auto buffer_size = algorithm::send_size(data_size);
	auto buffer = ::std::array<::std::uint8_t, buffer_size>{};

	auto result = ::std::ranges::begin(buffer);
	result = algorithm::send(protocols::address::any, Command, result, data);

	return buffer;
}

constexpr auto read_all_parameters()
{
	return NGS_LIB_MODULE_NAME::send_buffer<protocols::command::read_all_parameters>(::std::array<::std::uint8_t, 0>{});
}

NGS_LIB_MODULE_END