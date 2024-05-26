#include "./pcie6920/c.h"

namespace pcie6920
{
	namespace c_api
	{

		int pcie6920_open()
		{
			return 0;
		}

		void pcie6920_close()
		{
		}

		int pcie6920_demodulation_ch_quantity_set(unsigned demodulation_ch_quantity)
		{
			return 0;
		}

		int pcie6920_set_points_num_per_scan(unsigned point_num_per_scan)
		{
			return 0;
		}

		int pcie6920_set_scan_rate(double scan_rate)
		{
			return 0;
		}

		int pcie6920_set_pusle_width(unsigned pulse_high_width_ns)
		{
			return 0;
		}

		int pcie6920_set_center_freq(unsigned center_freq_hz)
		{
			return 0;
		}

		int pcie6920_data_src_sel(unsigned data_src_sel)
		{
			return 0;
		}

		int pcie6920_upload_rate_sel(unsigned upload_rate_sel)
		{
			return 0;
		}

		int pcie6920_point_num_per_ch_in_buf_query(unsigned* p_point_num_in_buf_per_ch)
		{
			*p_point_num_in_buf_per_ch = 100;
			return 0;
		}

		int pcie6920_read_data(unsigned point_num_per_ch, short* p_raw_data, unsigned* p_points_per_ch_returned)
		{

			::std::random_device rd{};
			::std::default_random_engine gen(rd());
			::std::uniform_int_distribution<short> dist(0, 100);

			for (::std::size_t i = 0; i < point_num_per_ch; i++)
			{
				p_raw_data[i] = dist(gen);
				i++;
				p_raw_data[i] = dist(gen);
			}

			*p_points_per_ch_returned = point_num_per_ch;
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