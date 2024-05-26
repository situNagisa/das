//==============================================================================
//
// Title:		pcie6920_api
// Purpose:		A short description of the interface.
//
// Created on:	2020-09-21 at 21:20:01 by localadmin.
// Copyright:	. All Rights Reserved.
//
//==============================================================================
#pragma once

#include "./defined.h"

namespace pcie6920
{
namespace c_api
{

	NGS_EXTERN_C_BEGIN

	/// \return 0: success, -1: fail
	int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_open();

	void NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_close();

	/// \brief 设置IQ解调通道数量
	/// \param demodulation_ch_quantity ：如果需要使用板卡内置的IQ解调功能，可将参数设置为1或2；
	/// \return 0: success, -1: fail
	int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_demodulation_ch_quantity_set(unsigned int demodulation_ch_quantity);

	/// \brief 设置每次触发脉冲每通道采集的点数
	///	\param point_num_per_scan ：每通道采集的点数，该参数必须是256的整数倍
	///	\return 0: success, -1: fail
	int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_set_points_num_per_scan(unsigned int point_num_per_scan);

	/// \brief 设置扫描率，即激光脉冲的频率
	///	\param scan_rate 扫描率，单位为Hz；
	///	\return 0: success, -1: fail
	int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_set_scan_rate(double scan_rate);

	/// \breif 设置触发脉冲的高电平宽度
	/// \param pulse_high_width_ns 单位为ns，该参数必须是4ns的整数倍，最小值为4ns
	/// \return 0: success, -1: fail
	int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_set_pusle_width(unsigned int pulse_high_width_ns);

	/// \brief 如果使用IQ解调功能，需要设置信号的中心频率；对于声光调制器移频80M的系统，该参数设置为80000000即可。
	///	\param center_freq_hz 采集信号的中心频率，单位为Hz
	/// \return 0: success, -1: fail
	int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_set_center_freq(unsigned int center_freq_hz);

	/// \brief 板卡上已经做了数字下变频的功能，可以对采集的数据进行数字I/Q解调，data_src_sel参数 和demodulation_ch_quantity共同用于选择读取解调算法中哪一位置处的数据。
	/// \return 0: success, -1: fail
	int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_data_src_sel(unsigned int data_src_sel);

	/// \brief 设置IQ或者相位/幅值解调数据的上传速率，在相同光纤长度的情况下降低上传的数据量，但是会降低单个采样点的空间分辨率。
	///	\param upload_rate_sel
	///			1：上传速率250M，对应单点空间分辨率0.4m；
	///			2：上传速率125M，对应单点空间分辨率0.8m；
	///			3：上传速率83.33M，对应单点空间分辨率1.2m；
	///			4：上传速率62.5M，对应单点空间分辨率1.6m；
	///			5：上传速率50M，对应单点空间分辨率2m；
	/// \return 0: success, -1: fail
	int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_upload_rate_sel(unsigned int upload_rate_sel);

	/// \brief 查询缓存中每通道的数据点数
	///	\param p_point_num_in_buf_per_ch 缓存中每通道的数据点数
	/// \return 0: success, -1: fail
	int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_point_num_per_ch_in_buf_query(unsigned int* p_point_num_in_buf_per_ch);

	/// \brief 读取数据
	///	\param point_num_per_ch 每通道读取的点数，该参数必须为256整数倍，建议一次至少读取512 * point_num_per_scan个点数，提高PCIe总线的数据传输利用率
	///	\param p_raw_data 指向读取数据缓冲区的指针，该指针代表的缓存长度必须为point_num_per_ch 参数的2倍或4倍，读取数据的解析方式请参考pcie6920_data_src_sel函数；
	///	\param p_points_per_ch_returned 每通道实际读取的数据个数
	/// \return 0: success, -1: fail
	int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_read_data(unsigned int point_num_per_ch, short* p_raw_data, unsigned int* p_points_per_ch_returned);

	/// \brief 开始采集
	/// \return 0: success, -1: fail
	int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_start(void);

	/// \brief 停止采集
	/// \return 0: success, -1: fail
	int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_stop(void);

	int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_test_wr_reg(unsigned int address, unsigned int data);
	int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_test_rd_reg(unsigned int address, unsigned int* p_data);

	NGS_EXTERN_C_END
}
}