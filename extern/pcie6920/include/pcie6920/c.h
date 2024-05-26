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

	/// \brief ����IQ���ͨ������
	/// \param demodulation_ch_quantity �������Ҫʹ�ð忨���õ�IQ������ܣ��ɽ���������Ϊ1��2��
	/// \return 0: success, -1: fail
	int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_demodulation_ch_quantity_set(unsigned int demodulation_ch_quantity);

	/// \brief ����ÿ�δ�������ÿͨ���ɼ��ĵ���
	///	\param point_num_per_scan ��ÿͨ���ɼ��ĵ������ò���������256��������
	///	\return 0: success, -1: fail
	int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_set_points_num_per_scan(unsigned int point_num_per_scan);

	/// \brief ����ɨ���ʣ������������Ƶ��
	///	\param scan_rate ɨ���ʣ���λΪHz��
	///	\return 0: success, -1: fail
	int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_set_scan_rate(double scan_rate);

	/// \breif ���ô�������ĸߵ�ƽ���
	/// \param pulse_high_width_ns ��λΪns���ò���������4ns������������СֵΪ4ns
	/// \return 0: success, -1: fail
	int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_set_pusle_width(unsigned int pulse_high_width_ns);

	/// \brief ���ʹ��IQ������ܣ���Ҫ�����źŵ�����Ƶ�ʣ����������������Ƶ80M��ϵͳ���ò�������Ϊ80000000���ɡ�
	///	\param center_freq_hz �ɼ��źŵ�����Ƶ�ʣ���λΪHz
	/// \return 0: success, -1: fail
	int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_set_center_freq(unsigned int center_freq_hz);

	/// \brief �忨���Ѿ����������±�Ƶ�Ĺ��ܣ����ԶԲɼ������ݽ�������I/Q�����data_src_sel���� ��demodulation_ch_quantity��ͬ����ѡ���ȡ����㷨����һλ�ô������ݡ�
	/// \return 0: success, -1: fail
	int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_data_src_sel(unsigned int data_src_sel);

	/// \brief ����IQ������λ/��ֵ������ݵ��ϴ����ʣ�����ͬ���˳��ȵ�����½����ϴ��������������ǻή�͵���������Ŀռ�ֱ��ʡ�
	///	\param upload_rate_sel
	///			1���ϴ�����250M����Ӧ����ռ�ֱ���0.4m��
	///			2���ϴ�����125M����Ӧ����ռ�ֱ���0.8m��
	///			3���ϴ�����83.33M����Ӧ����ռ�ֱ���1.2m��
	///			4���ϴ�����62.5M����Ӧ����ռ�ֱ���1.6m��
	///			5���ϴ�����50M����Ӧ����ռ�ֱ���2m��
	/// \return 0: success, -1: fail
	int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_upload_rate_sel(unsigned int upload_rate_sel);

	/// \brief ��ѯ������ÿͨ�������ݵ���
	///	\param p_point_num_in_buf_per_ch ������ÿͨ�������ݵ���
	/// \return 0: success, -1: fail
	int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_point_num_per_ch_in_buf_query(unsigned int* p_point_num_in_buf_per_ch);

	/// \brief ��ȡ����
	///	\param point_num_per_ch ÿͨ����ȡ�ĵ������ò�������Ϊ256������������һ�����ٶ�ȡ512 * point_num_per_scan�����������PCIe���ߵ����ݴ���������
	///	\param p_raw_data ָ���ȡ���ݻ�������ָ�룬��ָ�����Ļ��泤�ȱ���Ϊpoint_num_per_ch ������2����4������ȡ���ݵĽ�����ʽ��ο�pcie6920_data_src_sel������
	///	\param p_points_per_ch_returned ÿͨ��ʵ�ʶ�ȡ�����ݸ���
	/// \return 0: success, -1: fail
	int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_read_data(unsigned int point_num_per_ch, short* p_raw_data, unsigned int* p_points_per_ch_returned);

	/// \brief ��ʼ�ɼ�
	/// \return 0: success, -1: fail
	int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_start(void);

	/// \brief ֹͣ�ɼ�
	/// \return 0: success, -1: fail
	int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_stop(void);

	int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_test_wr_reg(unsigned int address, unsigned int data);
	int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_test_rd_reg(unsigned int address, unsigned int* p_data);

	NGS_EXTERN_C_END
}
}