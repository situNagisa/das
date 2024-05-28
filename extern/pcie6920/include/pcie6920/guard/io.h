#pragma once

#include "../c.h"
#include "../enum.h"
#include "../atomic.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct io
{
	NGS_PP_INJECT_BEGIN(io);
	friend struct open;
private:
	io()
	{
		c_api::pcie6920_start();
	}
public:
	~io()
	{
		c_api::pcie6920_stop();
	}

	::std::size_t buffer_unit_size() const
	{
		unsigned int size;
		NGS_ASSERT_VERIFY(!c_api::pcie6920_point_num_per_ch_in_buf_query(&size), "query the points number fail!");
		return size;
	}

	::std::size_t buffer_packet_size() const
	{
		auto size = self_type::buffer_unit_size();
		NGS_EXPECT(size % atomic::unit_size_per_packet() == 0);
		return atomic::packet_size_floor(size);
	}

	::std::size_t read(::std::ranges::contiguous_range auto&& result) const
		requires ::std::ranges::output_range<decltype(result), ::std::ranges::range_value_t<decltype(result)>>
	{
		return atomic::read(NGS_PP_PERFECT_FORWARD(result));
	}

	template<enums::parse_rule ParseRule, enums::channel_quantity Quantity>
	::std::size_t read(::std::span<atomic::packet<ParseRule,Quantity>> result) const
	{
		return self_type::read(result);
	}

	template<enums::parse_rule ParseRule, enums::channel_quantity Quantity>
	auto _convert_to_packet_range(::std::ranges::contiguous_range auto&& any_range) const
	{
		using packet_type = atomic::packet<ParseRule, Quantity>;
		using result_range_type = ::std::span<packet_type>;
		auto size_of_range = ::std::ranges::size(any_range) * sizeof(::std::ranges::range_value_t<decltype(any_range)>);
		auto size_of_packet = sizeof(packet_type);
		auto size = size_of_range / size_of_packet;
		NGS_ASSERT_VERIFY(size * size_of_packet == size_of_range, "the size of range is not a multiple of the size of packet");
		return result_range_type{ reinterpret_cast<packet_type*>(::std::ranges::data(any_range)), size };
	}

	template<enums::parse_rule ParseRule>
	::std::size_t _read_partial(enums::channel_quantity quantity, ::std::ranges::contiguous_range auto&& result) const
	{
		switch (quantity)
		{
		case enums::channel_quantity::_1:
			return self_type::read<ParseRule, enums::channel_quantity::_1>(self_type::_convert_to_packet_range<ParseRule, enums::channel_quantity::_1>(NGS_PP_PERFECT_FORWARD(result)));
		case enums::channel_quantity::_2:
			return self_type::read<ParseRule, enums::channel_quantity::_2>(self_type::_convert_to_packet_range<ParseRule, enums::channel_quantity::_1>(NGS_PP_PERFECT_FORWARD(result)));
		}
		return 0;
	}

	::std::size_t read(const atomic::info& info, ::std::ranges::contiguous_range auto&& result) const
	{
		switch (info.data_source_sel)
		{
		case enums::parse_rule::raw_data:
			return self_type::_read_partial<enums::parse_rule::raw_data>(info.demodulation_channel_quantity, NGS_PP_PERFECT_FORWARD(result));
		case enums::parse_rule::iq:
			return self_type::_read_partial<enums::parse_rule::iq>(info.demodulation_channel_quantity, NGS_PP_PERFECT_FORWARD(result));
		case enums::parse_rule::amplitude_phase:
			return self_type::_read_partial<enums::parse_rule::amplitude_phase>(info.demodulation_channel_quantity, NGS_PP_PERFECT_FORWARD(result));
		}
		return 0;
	}
};

NGS_LIB_MODULE_END