#pragma once

#include "./c.h"
#include "./enum.h"
#include "./defined.h"

NGS_LIB_BEGIN
	



struct channel
{
	::std::size_t point_size;

};

inline struct _pcie6920
{
	_pcie6920()
	{
		NGS_ASSERT_VERIFY(!c_api::pcie6920_open(), "the pcie6920 open fail! pcie may be open somewhere else, please close it and try again");
	}
	~_pcie6920()
	{
		c_api::pcie6920_close();
	}

	struct _read_guard
	{
		_read_guard()
		{
			c_api::pcie6920_start();
		}

		~_read_guard()
		{
			c_api::pcie6920_stop();
		}

		::std::size_t point_number_per_channel_in_buffer_query() const
		{
			unsigned int size;
			if (c_api::pcie6920_point_num_per_ch_in_buf_query(&size))
			{
				throw ::std::runtime_error("query the points number fail!");
			}
			return size;
		}

		::std::size_t read(void* destination, ::std::size_t point_size) const
		{
			unsigned int actual_size;
			if (c_api::pcie6920_read_data(static_cast<unsigned>(point_size), reinterpret_cast<short*>(destination), &actual_size))
			{
				throw ::std::runtime_error("read data fail!");
			}
			return actual_size;
		}


	};

	bool config(const config& config)
	{
		_config = config;

		if (c_api::pcie6920_demodulation_ch_quantity_set(static_cast<unsigned>(_config.demodulation_channel_quantity)))
			return false;
		if (c_api::pcie6920_set_points_num_per_scan(_config.points_per_scan))
			return false;
		if (c_api::pcie6920_set_scan_rate(_config.scan_rate))
			return false;
		if (c_api::pcie6920_set_pusle_width(_config.trigger_pulse_width))
			return false;
		if (c_api::pcie6920_set_center_freq(_config.center_frequency))
			return false;
		if (c_api::pcie6920_data_src_sel(static_cast<unsigned>(_config.data_source_sel)))
			return false;
		if (c_api::pcie6920_upload_rate_sel(static_cast<unsigned>(_config.upload_rate_sel)))
			return false;

		return true;
	}

	auto read_guard() const { return _read_guard{}; }

	struct config _config{};
}instance{};

NGS_LIB_END