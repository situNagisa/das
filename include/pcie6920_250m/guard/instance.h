#pragma once

#include "../c.h"
#include "../atomic.h"
#include "./io.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct instance
{
	instance()
	{
		if(c_api::pcie6920_open())
		{
			using namespace ::std::chrono_literals;
			c_api::pcie6920_close();
			::std::this_thread::sleep_for(1s);
			NGS_ASSERT_VERIFY(!c_api::pcie6920_open(), "the pcie6920 open fail! pcie may be open somewhere else, please close it and try again");
		}
	}
	~instance()
	{
		c_api::pcie6920_close();
	}

	void config(const atomic::info& info)
	{
		atomic::config(info);
	}

	decltype(auto) set_demodulation_channel_quantity(enums::channel_quantity q)
	{
		return atomic::set_demodulation_channel_quantity(q);
	}
	decltype(auto) set_scan_packet(::std::size_t packet_size)
	{
		return atomic::set_scan_packet(packet_size);
	}
	decltype(auto) set_scan_rate(::std::size_t rate)
	{
		return atomic::set_scan_rate(rate);
	}
	decltype(auto) set_pulse_width(::std::size_t width)
	{
		return atomic::set_pulse_width(width);
	}
	decltype(auto) set_center_frequency(::std::size_t freq)
	{
		return atomic::set_center_frequency(freq);
	}
	decltype(auto) set_data_source(enums::parse_rule rule)
	{
		return atomic::set_data_source(rule);
	}
	decltype(auto) set_upload_rate(enums::upload_rate rate)
	{
		return atomic::set_upload_rate(rate);
	}

	auto open_io() { return io{}; }
};

NGS_LIB_MODULE_END