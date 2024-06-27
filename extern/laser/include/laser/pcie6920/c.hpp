#pragma once

#include "./c.h"
#include "./atomic.h"
#include "./defined.h"


NGS_LIB_MODULE_BEGIN
namespace c_api
{
	inline atomic::info _config{};

	NGS_HPP_INLINE int pcie6920_open()
	{
		return 0;
	}

	NGS_HPP_INLINE void pcie6920_close()
	{
	}

	NGS_HPP_INLINE int pcie6920_demodulation_ch_quantity_set(unsigned demodulation_ch_quantity)
	{
		_config.demodulation_channel_quantity = static_cast<enums::channel_quantity>(demodulation_ch_quantity);
		return 0;
	}

	NGS_HPP_INLINE int pcie6920_set_points_num_per_scan(unsigned point_num_per_scan)
	{
		_config.packets_per_scan = point_num_per_scan;
		return 0;
	}

	NGS_HPP_INLINE int pcie6920_set_scan_rate(double scan_rate)
	{
		_config.scan_rate = scan_rate;
		return 0;
	}

	NGS_HPP_INLINE int pcie6920_set_pusle_width(unsigned pulse_high_width_ns)
	{
		_config.trigger_pulse_width = pulse_high_width_ns;
		return 0;
	}

	NGS_HPP_INLINE int pcie6920_set_center_freq(unsigned center_freq_hz)
	{
		_config.center_frequency = center_freq_hz;
		return 0;
	}

	NGS_HPP_INLINE int pcie6920_data_src_sel(unsigned data_src_sel)
	{
		_config.data_source_sel = static_cast<enums::parse_rule>(data_src_sel);
		return 0;
	}

	NGS_HPP_INLINE int pcie6920_upload_rate_sel(unsigned upload_rate_sel)
	{
		_config.upload_rate_sel = static_cast<enums::upload_rate>(upload_rate_sel);
		return 0;
	}

	NGS_HPP_INLINE int pcie6920_point_num_per_ch_in_buf_query(unsigned* p_point_num_in_buf_per_ch)
	{
		*p_point_num_in_buf_per_ch = 0x100 * 20;
		return 0;
	}

	NGS_HPP_INLINE int pcie6920_read_data(unsigned point_num_per_ch, short* p_raw_data, unsigned* p_points_per_ch_returned)
	{
		::std::random_device rd{};
		::std::default_random_engine gen(rd());
		::std::uniform_int_distribution<short> dist(0, 100);

		if (_config.data_source_sel == enums::parse_rule::raw_data)
		{
			auto packets = ::std::span{ reinterpret_cast<atomic::packet<enums::parse_rule::raw_data, enums::channel_quantity::_1>*>(p_raw_data), atomic::packet_size_floor(point_num_per_ch) };
			for (auto& unit : atomic::unpack(packets))
			{
				unit[0].data = dist(gen);
				unit[1].data = dist(gen);
			}

			*p_points_per_ch_returned = atomic::unit_size(packets.size());
		}
		return 0;
	}

	NGS_HPP_INLINE int pcie6920_start()
	{
		return 0;
	}

	NGS_HPP_INLINE int pcie6920_stop()
	{
		return 0;
	}
}
NGS_LIB_MODULE_END