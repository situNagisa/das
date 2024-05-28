#pragma once

#include "../c.h"
#include "./info.h"
#include "./packet.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

inline bool config(const info& info)
{
	if (c_api::pcie6920_demodulation_ch_quantity_set(static_cast<unsigned>(info.demodulation_channel_quantity)))
		return false;
	if (c_api::pcie6920_set_points_num_per_scan(NGS_LIB_MODULE_NAME::unit_size(info.packets_per_scan)))
		return false;
	if (c_api::pcie6920_set_scan_rate(info.scan_rate))
		return false;
	if (c_api::pcie6920_set_pusle_width(info.trigger_pulse_width))
		return false;
	if (c_api::pcie6920_set_center_freq(info.center_frequency))
		return false;
	if (c_api::pcie6920_data_src_sel(static_cast<unsigned>(info.data_source_sel)))
		return false;
	if (c_api::pcie6920_upload_rate_sel(static_cast<unsigned>(info.upload_rate_sel)))
		return false;

	return true;
}

NGS_LIB_MODULE_END