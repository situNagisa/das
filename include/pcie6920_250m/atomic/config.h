#pragma once

#include "../c.h"
#include "./info.h"
#include "./packet.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

inline bool set_demodulation_channel_quantity(enums::channel_quantity q)
{
	return c_api::pcie6920_demodulation_ch_quantity_set(static_cast<unsigned>(q));
}

inline bool set_scan_packet(::std::size_t packet_size)
{
	return c_api::pcie6920_set_points_num_per_scan(NGS_LIB_MODULE_NAME::unit_size(packet_size));
}

inline bool set_scan_rate(::std::size_t rate)
{
	return c_api::pcie6920_set_scan_rate(static_cast<double>(rate));
}

inline bool set_pulse_width(::std::size_t width)
{
	return c_api::pcie6920_set_pusle_width(static_cast<unsigned int>(width));
}

inline bool set_center_frequency(::std::size_t freq)
{
	return c_api::pcie6920_set_center_freq(static_cast<unsigned int>(freq));
}

inline bool set_data_source(enums::parse_rule rule)
{
	return c_api::pcie6920_data_src_sel(static_cast<unsigned>(rule));
}

inline bool set_upload_rate(enums::upload_rate rate)
{
	return c_api::pcie6920_upload_rate_sel(static_cast<unsigned>(rate));
}

inline bool config(const info& info)
{
	if (c_api::pcie6920_demodulation_ch_quantity_set(static_cast<unsigned>(info.channel_quantity)))
		return false;
	if (c_api::pcie6920_set_points_num_per_scan(NGS_LIB_MODULE_NAME::unit_size(info.packet_size)))
		return false;
	if (c_api::pcie6920_set_scan_rate(info.scan_rate))
		return false;
	if (c_api::pcie6920_set_pusle_width(info.pulse_width))
		return false;
	if (c_api::pcie6920_set_center_freq(info.center_frequency))
		return false;
	if (c_api::pcie6920_data_src_sel(static_cast<unsigned>(info.parse_rule)))
		return false;
	if (c_api::pcie6920_upload_rate_sel(static_cast<unsigned>(info.upload_rate)))
		return false;

	return true;
}

NGS_LIB_MODULE_END