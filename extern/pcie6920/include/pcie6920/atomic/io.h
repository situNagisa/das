#pragma once

#include "../enum.h"
#include "../c.h"
#include "./packet.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

::std::size_t read(::std::ranges::contiguous_range auto&& result)
	requires ::std::ranges::output_range<decltype(result), ::std::ranges::range_value_t<decltype(result)>>
{
	unsigned read_size;
	c_api::pcie6920_read_data(
		NGS_LIB_MODULE_NAME::unit_size(::std::ranges::size(result)),
		reinterpret_cast<short*>(::std::ranges::data(result)),
		&read_size
	);

	NGS_EXPECT(read_size % NGS_LIB_MODULE_NAME::unit_size_per_packet() == 0);

	return NGS_LIB_MODULE_NAME::packet_size_ceil(read_size);
}

NGS_LIB_MODULE_END