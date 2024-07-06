#pragma once

#include "./limit.h"
#include "./channels.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

template<class Packet,class Point>
struct buffer
{
	using packet_type = Packet;
	using point_type = Point;

	void transfer(::std::size_t packet_size)
	{
		auto units = ::pcie6920_250m::atomic::unpack(_buffer | ::std::views::take(packet_size));
		NGS_LIB_MODULE_NAME::range_limit(units | ::std::views::elements<0>, channel<0>(), _channels.size());
		NGS_LIB_MODULE_NAME::range_limit(units | ::std::views::elements<1>, channel<1>(), _channels.size());
		::std::ranges::shift_left(_buffer, static_cast<::std::ptrdiff_t>(packet_size));
	}

	void transfer()
	{
		transfer(_size);
	}

	constexpr void resize(::std::size_t size)
	{
		_size = size;
		_buffer.resize(_size);
		_channels.resize(point_size());
	}

	constexpr void shrink_to_fit()
	{
		_buffer.shrink_to_fit();
		_channels.shrink_to_fit();
	}

	constexpr auto size() const { return _size; }
	constexpr auto point_size() const { return ::pcie6920_250m::atomic::unit_size(_size); }

	constexpr auto packet_buffer(this auto&& self) { return ::std::span(self._buffer); }
	template<::std::size_t Index>
	constexpr auto channel(this auto&& self) { return self._channels.template channel<Index>(); }

	::std::size_t _size{};
	::std::vector<packet_type> _buffer{};
	channels<point_type, 2> _channels{};
};

NGS_LIB_MODULE_END