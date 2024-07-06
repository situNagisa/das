#pragma once

#include "../enum.h"
#include "./unit.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

constexpr auto unit_size_per_packet() {  return 0x100; }

constexpr auto unit_size(::std::size_t packet_size) { return packet_size * NGS_LIB_MODULE_NAME::unit_size_per_packet(); }
constexpr auto packet_size_floor(::std::size_t unit_size) { return unit_size / NGS_LIB_MODULE_NAME::unit_size_per_packet(); }
constexpr auto packet_size_ceil(::std::size_t unit_size) { return NGS_LIB_MODULE_NAME::packet_size_floor(unit_size) + static_cast<bool>(unit_size % NGS_LIB_MODULE_NAME::unit_size_per_packet()); }

template<enums::parse_rule ParseRule, enums::channel_quantity Quantity>
using packet = ::std::array<unit<ParseRule, Quantity>, NGS_LIB_MODULE_NAME::unit_size_per_packet()>;

constexpr auto unpack(::std::ranges::contiguous_range auto&& packet_range)
{
	return ::std::span{ ::std::ranges::data(*::std::ranges::begin(packet_range)), NGS_LIB_MODULE_NAME::unit_size(::std::ranges::size(packet_range)) };
}

NGS_LIB_MODULE_END