#include "./pcie6920/pcie6920.h"

namespace pcie6920
{
	namespace c_api
	{
		namespace
		{
			atomic::info config{};
		}

		int pcie6920_open()
		{
			return 0;
		}

		void pcie6920_close()
		{
		}

		int pcie6920_demodulation_ch_quantity_set(unsigned demodulation_ch_quantity)
		{
			config.demodulation_channel_quantity = static_cast<enums::channel_quantity>(demodulation_ch_quantity);
			return 0;
		}

		int pcie6920_set_points_num_per_scan(unsigned point_num_per_scan)
		{
			config.points_per_scan = point_num_per_scan;
			return 0;
		}

		int pcie6920_set_scan_rate(double scan_rate)
		{
			config.scan_rate = scan_rate;
			return 0;
		}

		int pcie6920_set_pusle_width(unsigned pulse_high_width_ns)
		{
			config.trigger_pulse_width = pulse_high_width_ns;
			return 0;
		}

		int pcie6920_set_center_freq(unsigned center_freq_hz)
		{
			config.center_frequency = center_freq_hz;
			return 0;
		}

		int pcie6920_data_src_sel(unsigned data_src_sel)
		{
			config.data_source_sel = static_cast<enums::parse_rule>(data_src_sel);
			return 0;
		}

		int pcie6920_upload_rate_sel(unsigned upload_rate_sel)
		{
			config.upload_rate_sel = static_cast<enums::upload_rate>(upload_rate_sel);
			return 0;
		}

		int pcie6920_point_num_per_ch_in_buf_query(unsigned* p_point_num_in_buf_per_ch)
		{
			*p_point_num_in_buf_per_ch = 0x100;
			return 0;
		}

		int pcie6920_read_data(unsigned point_num_per_ch, short* p_raw_data, unsigned* p_points_per_ch_returned)
		{
			::std::random_device rd{};
			::std::default_random_engine gen(rd());
			::std::uniform_int_distribution<short> dist(0, 100);

			if (config.data_source_sel == enums::parse_rule::raw_data)
			{
				auto packets = ::std::span{ reinterpret_cast<atomic::packet<enums::parse_rule::raw_data, enums::channel_quantity::_1>*>(p_raw_data), atomic::packet_size_floor(point_num_per_ch) };
				for(auto& unit : atomic::unpack(packets))
				{
					unit[0].data = dist(gen);
					unit[1].data = dist(gen);
				}

				*p_points_per_ch_returned = atomic::unit_size(packets.size());
			}
			return 0;
		}

		int pcie6920_start()
		{
			return 0;
		}

		int pcie6920_stop()
		{
			return 0;
		}

	}
}