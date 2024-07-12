#pragma once

#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct configurator
{
	auto&& info(this auto&& self) { return self._info; }
	auto&& is_change() const { return _is_change; }

	void render()
	{
		{
			using ::pcie6920_250m::enums::parse_rule;
			constexpr auto items = ::std::array{
				::std::make_pair(parse_rule::raw_data, "RawData")
			};

			_is_change.parse_rule = ::ngs::external::imgui::components::combo<items>("DataSource", _info.parse_rule);
		}
		{
			using ::pcie6920_250m::enums::upload_rate;
			constexpr auto items = ::std::array{
				::std::make_pair(upload_rate::_250m, "250M"),
				::std::make_pair(upload_rate::_125m, "125M"),
				::std::make_pair(upload_rate::_83_33m, "88.33M"),
				::std::make_pair(upload_rate::_62_5m, "62.5M"),
				::std::make_pair(upload_rate::_50m, "50M"),
			};

			_is_change.upload_rate = ::ngs::external::imgui::components::combo<items>("UploadRate", _info.upload_rate);
		}
		{
			using ::pcie6920_250m::enums::channel_quantity;
			constexpr auto items = ::std::array{
				::std::make_pair(channel_quantity::_1, "1"),
				::std::make_pair(channel_quantity::_2, "2"),
			};
			_is_change.channel_quantity = ::ngs::external::imgui::components::combo<items>("DemChQuantity", _info.channel_quantity);
		}
		_is_change.scan_rate = ::ngs::external::imgui::components::drag("ScanRate", _info.scan_rate);
		_is_change.pulse_width = ::ngs::external::imgui::components::drag("PulseWidth", _info.pulse_width);

		::std::size_t total_point_number = ::pcie6920_250m::atomic::unit_size(_info.packet_size);
		_is_change.packet_size = false;
		::ngs::external::imgui::components::drag("TotalPointNum", total_point_number, ::pcie6920_250m::atomic::unit_size_per_packet());
		if (_info.packet_size != ::pcie6920_250m::atomic::packet_size_floor(total_point_number))
		{
			_info.packet_size = ::pcie6920_250m::atomic::packet_size_floor(total_point_number);
			_is_change.packet_size = true;
		}

		_is_change.center_frequency = ::ngs::external::imgui::components::drag("CenterFreq", _info.center_frequency, 1, {}, {}, "%d m");

		::ImGui::Text("FiberLen %.2f km",
			::pcie6920_250m::atomic::unit_size(_info.packet_size) *
			::pcie6920_250m::enums::point_precision(_info.upload_rate) / 1000);
	}

	::pcie6920_250m::atomic::info _info
	{
		.parse_rule = pcie6920_250m::enums::parse_rule::raw_data,
		.upload_rate = pcie6920_250m::enums::upload_rate::_250m,
		.channel_quantity = pcie6920_250m::enums::channel_quantity::_1,
		.scan_rate = 1000,
		.pulse_width = 100,
		.packet_size = 50,
		.center_frequency = 80,
	};
	struct
	{
		bool parse_rule : 1;
		bool upload_rate : 1;
		bool channel_quantity : 1;
		bool scan_rate : 1;
		bool pulse_width : 1;
		bool packet_size : 1;
		bool center_frequency : 1;
	}_is_change{};
};

NGS_LIB_MODULE_END