#pragma once

#include "./buffer.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct instance
{
	using packet_type = ::pcie6920_250m::atomic::packet<::pcie6920_250m::enums::parse_rule::raw_data, ::pcie6920_250m::enums::channel_quantity::_1>;
	using point_type = ::std::int16_t;

	instance()
	{
		resize(_info.packet_size, _info.scan_rate);
	}

	bool read()
	{
		auto io = _instance.open_io();

		if (io.buffer_packet_size() < _buffer.size())
			return false;

		io.read(_buffer.packet_buffer());
		_buffer.transfer();
		return true;
	}

	void resize(::std::size_t packet_size, ::std::size_t total_frame)
	{
		_times.resize(::pcie6920_250m::atomic::unit_size(packet_size));
		::std::ranges::copy(::std::views::iota(0) | ::std::views::take(::pcie6920_250m::atomic::unit_size(packet_size)), ::std::ranges::begin(_times));
		_buffer.resize(packet_size * total_frame);
	}

	void shrink_to_fit()
	{
		_times.shrink_to_fit();
		_buffer.shrink_to_fit();
	}

	void update()
	{
		if(_state.hold_time)
		{
			if(read())
			{
				auto bytes_channel0 = ::std::as_bytes(_buffer.channel<0>());
				_state.channel0.write(reinterpret_cast<const char*>(::std::ranges::data(bytes_channel0)), ::std::ranges::size(bytes_channel0));
				auto bytes_channel1 = ::std::as_bytes(_buffer.channel<1>());
				_state.channel1.write(reinterpret_cast<const char*>(::std::ranges::data(bytes_channel1)), ::std::ranges::size(bytes_channel1));
				--_state.hold_time;
			}

			if(!_state.hold_time)
			{
				_state.channel0.close();
				_state.channel1.close();
			}
		}
	}

	void _open_file(::std::ofstream& file, ::std::string_view name) const
	{
		::std::filesystem::path dir = ::std::string(_config.root_directory.data());
		dir /= _config.type.data();
		
		if (!::std::filesystem::exists(dir))
		{
			::std::filesystem::create_directories(dir);
		}
		dir /= ::std::string(_config.type.data()) + '_' + ::std::string(name) + '_' + ::ngs::to_strings::to_string(_config.hold_time) + ".bin";
		file.open(dir, ::std::ios::binary | ::std::ios::out);
	}

	void render_config()
	{
		{
			using ::pcie6920_250m::enums::parse_rule;
			constexpr auto items = ::std::array{
				::std::make_pair(parse_rule::raw_data, "RawData")
			};

			if (::quick_ui::components::combo<items>("DataSource", _info.parse_rule))
			{
				if(_instance.set_data_source(_info.parse_rule))
				{
					NGS_LOGL(error, "set data source fail: ", ::std::to_underlying(_info.parse_rule));
				}
			}
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
			if (::quick_ui::components::combo<items>("UploadRate", _info.upload_rate))
			{
				if (_instance.set_upload_rate(_info.upload_rate))
				{
					NGS_LOGL(error, "set upload rate fail: ", ::std::to_underlying(_info.upload_rate));
				}
			}
		}
		{
			using ::pcie6920_250m::enums::channel_quantity;
			constexpr auto items = ::std::array{
				::std::make_pair(channel_quantity::_1, "1"),
				::std::make_pair(channel_quantity::_2, "2"),
			};
			if (::quick_ui::components::combo<items>("DemChQuantity", _info.channel_quantity))
			{
				if(_instance.set_demodulation_channel_quantity(_info.channel_quantity))
				{
					NGS_LOGL(error, "set demodulation channel quantity fail: ", ::std::to_underlying(_info.channel_quantity));
				}
			}
		}
		if (::quick_ui::components::drag("ScanRate", _info.scan_rate))
		{
			if(_instance.set_scan_rate(_info.scan_rate))
			{
				NGS_LOGL(error, "set scan rate fail: ", _info.scan_rate);
			}
			resize(_info.packet_size, _info.scan_rate);
			shrink_to_fit();
		}
		if (::quick_ui::components::drag("PulseWidth", _info.pulse_width))
		{
			if(_instance.set_pulse_width(_info.pulse_width))
			{
				NGS_LOGL(error, "set pulse width fail: ", _info.pulse_width);
			}
		}
		::std::size_t total_point_number = ::pcie6920_250m::atomic::unit_size(_info.packet_size);
		if (::quick_ui::components::drag("TotalPointNum", total_point_number, ::pcie6920_250m::atomic::unit_size_per_packet()))
		{
			if(_info.packet_size != ::pcie6920_250m::atomic::packet_size_floor(total_point_number))
			{
				_info.packet_size = ::pcie6920_250m::atomic::packet_size_floor(total_point_number);
				if (_instance.set_scan_packet(_info.packet_size))
				{
					NGS_LOGL(error, "set packet size fail: ", _info.packet_size);
				}
				resize(_info.packet_size, _info.scan_rate);
				shrink_to_fit();
			}
		}
		if (::quick_ui::components::drag("CenterFreq", _info.center_frequency))
		{
			if (_instance.set_center_frequency(_info.center_frequency))
			{
				NGS_LOGL(error, "set center frequency fail: ", _info.center_frequency);
			}
		}
		if (::quick_ui::components::drag("FiberLen", _info.fiber_length))
		{
			
		}
	}

	void _render_line(::std::string_view title, ::std::span<point_type> independent, ::std::span<point_type> dependent)
	{
		::ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_None, ImPlotAxisFlags_AutoFit);
		if (!independent.empty())
		{
			::ImPlot::SetupAxisLimits(ImAxis_X1, independent.front(), independent.front() + ::pcie6920_250m::atomic::unit_size(_info.packet_size), ImGuiCond_Always);
		}
		::ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);

		::quick_ui::components::plot_line(title, independent, dependent);
	}


	void _render_plot(::std::string_view title, ::std::pair<float, float> size, ::std::span<point_type> dependent)
	{
		if (::ImPlot::BeginPlot(title.data(), { size.first, size.second }))
		{
			_render_line(title.data(), _times, dependent);
			::ImPlot::EndPlot();
		}
	}
	void render_plot()
	{
		_render_plot("channel 0", { 900,300 },_buffer.channel<0>());
		_render_plot("channel 1", { 900,300 },_buffer.channel<1>());
	}

	void render_save()
	{
		if (::ImGui::Button("open root", { 100 , 0 }))
		{
			::ifd::FileDialog::Instance().Open("root_directory", "root directory", "", false, _config.root_directory.data());
		}
		if (::ifd::FileDialog::Instance().IsDone("root_directory")) {
			if (::ifd::FileDialog::Instance().HasResult()) {
				::std::ranges::copy(::ifd::FileDialog::Instance().GetResult().string(), _config.root_directory.begin());
			}
			::ifd::FileDialog::Instance().Close();
		}
		if (::std::strlen(_config.root_directory.data()) && ::std::strlen(_config.type.data()))
		{
			::ImGui::SameLine();
			if(!_state.hold_time)
			{
				if (::ImGui::Button("save", { 100 , 0 }))
				{
					_open_file(_state.channel0, "channel0");
					while(!_state.channel0.is_open());

					_open_file(_state.channel1, "channel1");
					while(!_state.channel1.is_open());

					_state.hold_time = _config.hold_time;
				}
			}
			else
			{
				::ImGui::Text("hold time: %ld", _state.hold_time);
			}
		}

		::ImGui::InputText("root dir", _config.root_directory.data(), _config.root_directory.size());
		::ImGui::InputText("type", _config.type.data(), _config.type.size());
		::quick_ui::components::drag("hold time", _config.hold_time);
	}

	::pcie6920_250m::guard::instance _instance{};
	::pcie6920_250m::atomic::info _info
	{
		.parse_rule = pcie6920_250m::enums::parse_rule::raw_data,
		.upload_rate = pcie6920_250m::enums::upload_rate::_250m,
		.channel_quantity = pcie6920_250m::enums::channel_quantity::_1,
		.scan_rate = 1,
		.pulse_width = 100,
		.packet_size = 32,
		.center_frequency = 100,
		.fiber_length = 200
	};
	struct
	{
		::std::array<char, 128> root_directory{};
		::std::array<char, 32> type{"unknown"};
		::std::size_t hold_time = 1;
	}_config{};
	struct
	{
		::std::ofstream channel0{};
		::std::ofstream channel1{};
		::std::size_t hold_time = 0;
	}_state{};

	::std::vector<point_type> _times{};
	buffer< packet_type, point_type> _buffer{};
};

NGS_LIB_MODULE_END