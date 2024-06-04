#pragma once

#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct data
{
	NGS_PP_INJECT_BEGIN(data);
public:
	bool recording = false;
	::laser::pcie6920::atomic::info pcie_config{
		.demodulation_channel_quantity = ::laser::pcie6920::enums::channel_quantity::_1,
		.packets_per_scan = 64,
		.scan_rate = 2000,
		.trigger_pulse_width = 4,
		.center_frequency = 80000000,
		.data_source_sel = ::laser::pcie6920::enums::parse_rule::raw_data,
		.upload_rate_sel = ::laser::pcie6920::enums::upload_rate::_250m
	};

	struct
	{
		::std::string device = "COM7";
		::boost::asio::serial_port_base::baud_rate baud_rate{ 9600 };
		::boost::asio::serial_port_base::character_size character_size{ 8 };
		::boost::asio::serial_port_base::parity parity{::boost::asio::serial_port_base::parity::none};
		::boost::asio::serial_port_base::stop_bits stop_bits{::boost::asio::serial_port_base::stop_bits::one};
		::boost::asio::serial_port_base::flow_control flow_control{::boost::asio::serial_port_base::flow_control::none};
		bool is_opened = false;
	}uart{};

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



	void save(::std::ofstream& of) const
	{
		::std::span<const ::std::byte, sizeof(self_type)> memory{ reinterpret_cast<const ::std::byte*>(this), sizeof(self_type) };
		of.write(reinterpret_cast<const char*>(memory.data()), memory.size());
	}

	void read(::std::ifstream& ifs)
	{
		::std::span<::std::byte, sizeof(self_type)> memory{ reinterpret_cast<::std::byte*>(this), sizeof(self_type) };
		ifs.read(reinterpret_cast<char*>(memory.data()), memory.size());
	}
};

NGS_LIB_MODULE_END