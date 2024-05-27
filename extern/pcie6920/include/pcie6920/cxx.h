#pragma once

#include "./c.h"
#include "./defined.h"


namespace pcie6920
{
	enum class upload_rate : ::std::uint32_t
	{
		_250m = 1,
		_125m,
		_83_33m,
		_62_5m,
		_50m,
	};

	enum class parse_rule : ::std::uint32_t
	{
		raw_data = 0,
		iq = 2,
		amplitude_phase = 3,
	};

	enum class channel_quantity : ::std::uint32_t
	{
		_1 = 1,
		_2,
	};

	struct config
	{
		channel_quantity demodulation_channel_quantity;
		::std::uint32_t points_per_scan;
		::std::uint32_t scan_rate;
		::std::uint32_t trigger_pulse_width;
		::std::uint32_t center_frequency;
		parse_rule data_source_sel;
		upload_rate upload_rate_sel;
	};

	inline struct _pcie6920
	{
		_pcie6920()
		{
			if (c_api::pcie6920_open())
				throw ::std::runtime_error("the pcie6920 open fail!");
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

		bool config(const config& config) const
		{
			if (c_api::pcie6920_demodulation_ch_quantity_set(static_cast<unsigned>(config.demodulation_channel_quantity)))
				return false;
			if (c_api::pcie6920_set_points_num_per_scan(config.points_per_scan))
				return false;
			if (c_api::pcie6920_set_scan_rate(config.scan_rate))
				return false;
			if (c_api::pcie6920_set_pusle_width(config.trigger_pulse_width))
				return false;
			if (c_api::pcie6920_set_center_freq(config.center_frequency))
				return false;
			if (c_api::pcie6920_data_src_sel(static_cast<unsigned>(config.data_source_sel)))
				return false;
			if (c_api::pcie6920_upload_rate_sel(static_cast<unsigned>(config.upload_rate_sel)))
				return false;

			return true;
		}

		auto read_guard() const { return _read_guard{}; }
	}instance{};
}
