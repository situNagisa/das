#pragma once

#include "./defined.h"

NGS_LIB_MODULE_BEGIN

inline struct _runtime_image
{
	NGS_PP_INJECT_BEGIN(_runtime_image);
public:
	bool recording = false;
	::laser::pcie6920::atomic::info pcie_config;

	//float fiber_length;
	//combo_list data_source;
	//combo_list upload_rate;
	//bool sw_dem;
	//bool spectrum_enable;
	//combo_list psd;
	//combo_list spectrum;

	//::std::uint64_t register_address;
	//::std::uint64_t write_register_data_address;
	//::std::uint64_t read_register_data_address;

	//::std::uint64_t write_offset_data_channel_0;
	//::std::uint64_t write_offset_data_channel_1;
	//::std::uint64_t read_offset_data_channel_0;
	//::std::uint64_t read_offset_data_channel_1;



	void save(const ::std::filesystem::path& file) const
	{
		::std::ofstream of(file, ::std::ios::out | ::std::ios::binary);
	}
}data{};

NGS_LIB_MODULE_END