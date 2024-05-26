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
int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_point_num_per_ch_in_buf_query(unsigned int *p_point_num_in_buf_per_ch);

/// \brief ��ȡ����
///	\param point_num_per_ch ÿͨ����ȡ�ĵ������ò�������Ϊ256������������һ�����ٶ�ȡ512 * point_num_per_scan�����������PCIe���ߵ����ݴ���������
///	\param p_raw_data ָ���ȡ���ݻ�������ָ�룬��ָ�����Ļ��泤�ȱ���Ϊpoint_num_per_ch ������2����4������ȡ���ݵĽ�����ʽ��ο�pcie6920_data_src_sel������
///	\param p_points_per_ch_returned ÿͨ��ʵ�ʶ�ȡ�����ݸ���
/// \return 0: success, -1: fail
int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_read_data(unsigned int point_num_per_ch,short *p_raw_data, unsigned int *p_points_per_ch_returned);

/// \brief ��ʼ�ɼ�
/// \return 0: success, -1: fail
int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_start(void);

/// \brief ֹͣ�ɼ�
/// \return 0: success, -1: fail
int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_stop(void);

int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_test_wr_reg(unsigned int address,unsigned int data);
int NGS_CONFIG_CALLING_CONVENTION_STDCALL pcie6920_test_rd_reg(unsigned int address,unsigned int *p_data);

NGS_EXTERN_C_END

namespace pcie6920
{
	enum class upload_rate : ::std::uint32_t
	{
		_250m = 1,
		_125m,
		_83_33m,
		_62_5m,
		_50m,
	};

	enum class parse_rule : ::std::uint32_t
	{
		raw_data = 0,
		iq = 2,
		amplitude_phase = 3,
	};

	enum class channel_quantity : ::std::uint32_t
	{
		_1 = 1,
		_2,
	};

	struct config
	{
		channel_quantity demodulation_channel_quantity;
		::std::uint32_t points_per_scan;
		::std::uint32_t scan_rate;
		::std::uint32_t trigger_pulse_width;
		::std::uint32_t center_frequency;
		parse_rule data_source_sel;
		upload_rate upload_rate_sel;
	};

	inline struct _pcie6920
	{
		_pcie6920()
		{
			if (::pcie6920_open())
				throw ::std::runtime_error("the pcie6920 open fail!");
		}
		~_pcie6920()
		{
			::pcie6920_close();
		}

		struct _read_guard
		{
			_read_guard()
			{
				::pcie6920_start();
			}

			~_read_guard()
			{
				::pcie6920_stop();
			}

			::std::size_t point_number_per_channel_in_buffer_query() const
			{
				unsigned int size;
				if (::pcie6920_point_num_per_ch_in_buf_query(&size))
				{
					throw ::std::runtime_error("query the points number fail!");
				}
				return size;
			}

			::std::size_t read(void* destination, ::std::size_t size) const
			{
				unsigned int actual_size;
				if (::pcie6920_read_data(static_cast<unsigned int>(size) / sizeof(short), reinterpret_cast<short*>(destination), &actual_size))
				{
					throw ::std::runtime_error("read data fail!");
				}
				return actual_size;
			}
		};

		void config(const config& config) const
		{
			if (::pcie6920_demodulation_ch_quantity_set(static_cast<unsigned>(config.demodulation_channel_quantity)))
				goto error;
			if(::pcie6920_set_points_num_per_scan(config.points_per_scan))
				goto error;
			if(::pcie6920_set_scan_rate(config.scan_rate))
				goto error;
			if(::pcie6920_set_pusle_width(config.trigger_pulse_width))
				goto error;
			if(::pcie6920_set_center_freq(config.center_frequency))
				goto error;
			if(::pcie6920_data_src_sel(static_cast<unsigned>(config.data_source_sel)))
				goto error;
			if(::pcie6920_upload_rate_sel(static_cast<unsigned>(config.upload_rate_sel)))
				goto error;

			return;

			error:
			throw ::std::runtime_error("config fail!");
		}

		auto read_guard() const { return _read_guard{}; }
	}instance{};
}
