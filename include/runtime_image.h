#pragma once

#include "./pcie6920_api.h"
#include "./defined.h"

inline struct _runtime_image
{
	NGS_PP_INJECT_BEGIN(_runtime_image);
public:
	struct combo_list
	{
		int index = 0;
		::std::span<const char* const> data{};
	};
	float plot_alpha;

	int total_point_number = 2;
	float fiber_length;
	combo_list data_source;
	combo_list upload_rate;
	bool sw_dem;
	bool spectrum_enable;
	combo_list psd;
	combo_list spectrum;

	::std::uint64_t register_address;
	::std::uint64_t write_register_data_address;
	::std::uint64_t read_register_data_address;

	::std::uint64_t write_offset_data_channel_0;
	::std::uint64_t write_offset_data_channel_1;
	::std::uint64_t read_offset_data_channel_0;
	::std::uint64_t read_offset_data_channel_1;

	::pcie6920::config pcie_config;


	void save(const ::std::filesystem::path& file) const
	{
		::std::ofstream of(file, ::std::ios::out | ::std::ios::binary);
	}
}runtime_image{};
