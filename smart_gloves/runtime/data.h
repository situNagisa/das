#pragma once

#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct data
{
	NGS_PP_INJECT_BEGIN(data);
public:
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