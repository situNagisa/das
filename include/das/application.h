#pragma once

#include "./context.h"
#include "./stream.h"
#include "./defined.h"

NGS_LIB_BEGIN

struct application
{
	NGS_PP_INJECT_BEGIN(application);
public:
	using packet_type = ::laser::pcie6920::atomic::packet<::laser::pcie6920::enums::parse_rule::raw_data, ::laser::pcie6920::enums::channel_quantity::_1>;

	application()
		: _context("shaded plots")
	{
		_context.plot.push_graph({
			.title = "channel 0",
			.width = 900,
			.height = 300,
			.lines = {
				{
					"channel",
					ui::line{}
				},
			}
			});
		_context.plot.push_graph({
				.title = "channel 1",
				.width = 900,
				.height = 300,
				.lines = {
					{
						"channel",
						ui::line{}
					}
				}
			});

		::laser::pcie6920::atomic::config(_context.runtime.pcie_config);
	}
	~application()
	{
		_context.io.stop();
		_done = true;
	}

	auto&& channel0_line_data() { return _context.plot.graph()[0].lines["channel"]; }
	auto&& channel0_line_data() const { return _context.plot.graph()[0].lines.at("channel"); }
	auto&& channel1_line_data() { return _context.plot.graph()[1].lines["channel"]; }
	auto&& channel1_line_data() const { return _context.plot.graph()[1].lines.at("channel"); }

	void run()
	{
		while (!_context.gui.window().is_should_close())
		{
			_update();
			_render();
		}
	}

	void _record(::std::span<packet_type> read_buffer)
	{
		if (_context.runtime.pcie_config.data_source_sel == laser::pcie6920::enums::parse_rule::raw_data)
		{
			auto buffer = ::laser::pcie6920::atomic::unpack(read_buffer);
			auto push = [time = _context.time](::das::ui::line_storage& line, decltype(buffer) buffer, ::std::size_t channel_index)
				{
					auto push_size = ::std::min(::das::das_config::limit_point, ::std::ranges::size(buffer));

					if (line.size() >= ::das::das_config::limit_point)
					{
						::std::ranges::shift_left(line.independent_variable, static_cast<long long>(::std::ranges::size(buffer)));
						::std::ranges::shift_left(line.depend_variable, static_cast<long long>(::std::ranges::size(buffer)));
						line.resize(::das::das_config::limit_point - push_size);
					}
					auto t = time;
					for (auto&& unit : buffer | ::std::views::take(push_size))
					{
						line.push(static_cast<::std::int32_t>(t), unit[static_cast<long long>(channel_index)].data);
						t++;
					}
				};

			push(_channel0_line, buffer, 0);
			push(_channel1_line, buffer, 1);

			_context.time += ::std::ranges::size(buffer);
		}
	}

	void _next_write_uart()
	{
		_context.serial_port->async_write_some(::boost::asio::buffer(::laser::mic::commands::read_all_parameters()), ::boost::bind(&self_type::_write_uart_handler, this, ::boost::asio::placeholders::error, ::boost::asio::placeholders::bytes_transferred));
	}

	void _write_uart_handler(const ::boost::system::error_code& error, ::std::size_t bytes_transferred)
	{
		if (error)
		{
			if (error != ::boost::asio::error::operation_aborted)
				NGS_LOGL(error, error.message());
		}

		if (_done || !_context.serial_port || !_context.serial_port->is_open())
		{
			NGS_LOGL(info, "bye bye~");
			return;
		}

		_next_write_uart();
	}

	void _next_read_uart()
	{
		_context.serial_port->async_read_some(::boost::asio::buffer(_receive_buffer), ::boost::bind(&self_type::_read_uart_handler, this, ::boost::asio::placeholders::error, ::boost::asio::placeholders::bytes_transferred));
	}

	::std::mutex _uart_read_mutex{};

	void _read_uart_handler(const ::boost::system::error_code& error, ::std::size_t bytes_transferred)
	{
		if (error)
		{
			if (error != ::boost::asio::error::operation_aborted)
				NGS_LOGL(error, error.message());
			goto next_read;
		}
		{
			::std::lock_guard lock(_uart_read_mutex);
			_uart_stream.in(_receive_buffer | ::std::views::take(bytes_transferred) | ::std::views::transform([](auto&& i) {return ::std::byte{ i }; }));
		}
		//NGS_LOGL(info, ::std::format("uart received {} bytes", bytes_transferred));
	next_read:
		if (_done || !_context.serial_port || !_context.serial_port->is_open())
		{
			NGS_LOGL(info, "bye bye~");
			return;
		}

		_next_read_uart();
	}

	void _update()
	{
		if (_context.runtime.recording)
		{
			auto io = _context.pcie.io();

			if (_read_buffer_index < _read_buffer.size())
			{
				_record(::std::span{ _read_buffer }.subspan(_read_buffer_index, 1));
				++_read_buffer_index;
			}

			if (io.buffer_packet_size())
			{
				if (_read_buffer_index < _read_buffer.size())
				{
					_record(::std::span{ _read_buffer }.subspan(_read_buffer_index));
				}
				_read_buffer_index = 0;

				_read_buffer.resize(io.buffer_packet_size());
				_read_buffer.resize(io.read(_read_buffer));
			}

			channel0_line_data().bind(_channel0_line);
			channel1_line_data().bind(_channel1_line);
		}
		try
		{
			_context.io.poll();
		}
		catch (const ::boost::system::system_error& e)
		{
			NGS_LOGL(error, e.what());
		}
		{
			::std::lock_guard lock(_uart_read_mutex);

			constexpr auto to_string = [](const auto& data)
				{
					::std::string result{};
					for (auto&& unit : data)
					{
						result += ::std::format("{:02x} ", static_cast<::std::uint8_t>(unit));
					}
					return result;
				};

			if (!_uart_stream.empty())
			{
				auto [data] = _uart_stream.out();

				auto check_result = ::laser::mic::algorithm::receive_check(::laser::mic::protocols::command::read_all_parameters, ::std::span{ reinterpret_cast<::std::uint8_t*>(data.data()),data.size() });
				if (::std::ranges::empty(check_result) || ::std::ranges::size(check_result) != sizeof(::laser::mic::algorithm::all_parameter))
				{

					NGS_LOGL(error, "receive data error: ", to_string(data));
				}
				_context.edfa_parameter = *reinterpret_cast<const ::laser::mic::algorithm::all_parameter*>(::std::ranges::data(check_result));
				_context.edfa_parameter.rectified_endian();
				//NGS_LOGL(info, "receive data: ", to_string(check_result));
			}
		}
	}

	void _save() const
	{
		::std::filesystem::path path = ::std::filesystem::path(_root_directory) / ::std::string(_type.data());
		::std::filesystem::path file = ::std::string(_type.data()) + "_" + ::std::to_string(_hold_time) + ".dat";

		if(!::std::filesystem::exists(path))
		{
			::std::filesystem::create_directory(path);
		}

		try
		{
			::std::ofstream output(path / file, ::std::ios::out | ::std::ios::binary);
			output.write(reinterpret_cast<const char*>(_channel0_line.depend_variable.data()), _channel0_line.depend_variable.size() * sizeof(::std::ranges::range_value_t<decltype(_channel0_line.depend_variable)>));
			output.write(reinterpret_cast<const char*>(_channel1_line.depend_variable.data()), _channel1_line.depend_variable.size() * sizeof(::std::ranges::range_value_t<decltype(_channel1_line.depend_variable)>));
			output.close();
		}
		catch (::std::exception& e)
		{
			NGS_LOGL(error, "save fail! ", e.what());
		}
	}

	bool _save_success = false;
	void _render()
	{
		auto render_guard = _context.gui.render_guard();

		using namespace ::das;
		using namespace ::das::ui;

		::ImGui::SetNextWindowPos({ 8,23 }, ImGuiCond_FirstUseEver);
		::ImGui::SetNextWindowSize({ 326,272 }, ImGuiCond_FirstUseEver);
		if (::ImGui::Begin("config 1", nullptr, das_config::imgui_window_flag))
		{
			constexpr auto width = 100;
			::ImGui::PushItemWidth(150);

			if (::ImGui::BeginTabBar("MyTabBar", ImGuiTabBarFlags_None))
			{
				if (::ImGui::BeginTabItem("pcie"))
				{
					/*
					if (_context.runtime.recording)
					{
						if (::ImGui::Button("stop", { width , 0 }))
						{
							_context.runtime.recording = false;
							_save();
						}
					}
					else
					{
						if (::ImGui::Button("start", { width , 0 }))
							_context.runtime.recording = true;
					}
					if (::ImGui::Button("clear", { width , 0 }))
					{
						channel0_line_data().clear();
						channel1_line_data().clear();
						_channel0_line.clear();
						_channel1_line.clear();
						_context.time = 0;
					}
					*/
					::ImGui::SeparatorText("config");
					{
						if (::ImGui::Button("config", { width , 0 }))
							::laser::pcie6920::atomic::config(_context.runtime.pcie_config);
						{
							int packets_per_scan = static_cast<int>(_context.runtime.pcie_config.packets_per_scan);
							int scan_rate = static_cast<int>(_context.runtime.pcie_config.scan_rate);
							int pulse_width = static_cast<int>(_context.runtime.pcie_config.trigger_pulse_width);
							int center_frequency = static_cast<int>(_context.runtime.pcie_config.center_frequency);
							//static float fiber_length = 0.0f;

							::ImGui::DragInt("packets/scan", &packets_per_scan, 1, 0, 128, "%d");
							::ImGui::DragInt("scan rate", &scan_rate, 100, 0, 10000, "%dhz");
							::ImGui::DragInt("pusle width", &pulse_width, 1, 0, ::std::numeric_limits<int>::max(), "%dms");
							::ImGui::DragInt("center frequency", &center_frequency, 1, 0, ::std::numeric_limits<int>::max(), "%dmhz");
							//ImGui::DragFloat("fiber length", &runtime_image.fiber_length, 1, 0, ::std::numeric_limits<float>::max(), "%.2fkm");

							_context.runtime.pcie_config.packets_per_scan = static_cast<::std::uint32_t>(packets_per_scan);
							_context.runtime.pcie_config.scan_rate = static_cast<::std::uint32_t>(scan_rate);
							_context.runtime.pcie_config.trigger_pulse_width = static_cast<::std::uint32_t>(pulse_width);
							_context.runtime.pcie_config.center_frequency = static_cast<::std::uint32_t>(center_frequency);
						}
					}
					::ImGui::SeparatorText("save");
					if (!_context.runtime.recording)
					{
						if (::ImGui::Button("open root", { width , 0 }))
						{
							::ifd::FileDialog::Instance().Open("root_directory", "save root directory", "");
						}
						if (::ifd::FileDialog::Instance().IsDone("root_directory")) {
							if (::ifd::FileDialog::Instance().HasResult()) {
								_root_directory = ::ifd::FileDialog::Instance().GetResult().string();
								NGS_LOGL(info, "root file: ", _root_directory);
							}
							::ifd::FileDialog::Instance().Close();
						}
						::ImGui::InputText("root dir", _root_directory.data(), _root_directory.size());
						::ImGui::InputText("type", _type.data(), _type.size());
						::ImGui::DragScalar("hold time", ImGuiDataType_U32, &_hold_time, 1, nullptr, nullptr, "%d s");

						if (!_root_directory.empty() && (::std::strlen(_type.data()) != 0))
						{
							if (::ImGui::Button("start", { width , 0 }))
							{
								_context.runtime.recording = true;
								das_config::limit_point = _hold_time * 1024;
								::std::thread([this]
									{
										using namespace ::std::chrono_literals;
										auto start = ::std::chrono::system_clock::now();
										while (::std::chrono::system_clock::now() - start < ::std::chrono::seconds(_hold_time))
										{

										}
										_context.runtime.recording = false;
										_save();
										_save_success = true;
										
									}).detach();
							}
						}
					}
					else
					{
						::ImGui::Text("recoding data...");
					}

					::ImGui::EndTabItem();
				}
				if (::ImGui::BeginTabItem("uart"))
				{
					::ImGui::SeparatorText("uart");
					{
						constexpr auto baud_rates = ::std::array{ 300, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200 };
						constexpr auto baud_rates_item = ::std::array{ "300", "1200", "2400", "4800", "9600", "14400", "19200", "38400", "57600", "115200" };
						int baud_rate = ::std::ranges::distance(baud_rates.begin(), ::std::ranges::find(baud_rates, static_cast<int>(_context.runtime.uart.baud_rate.value())));

						int character_size = static_cast<int>(_context.runtime.uart.character_size.value());

						constexpr auto parities = ::std::array{ ::boost::asio::serial_port_base::parity::none, ::boost::asio::serial_port_base::parity::odd, ::boost::asio::serial_port_base::parity::even };
						constexpr auto parities_item = ::std::array{ "none", "odd", "even" };
						int parity = ::std::ranges::distance(parities.begin(), ::std::ranges::find(parities, _context.runtime.uart.parity.value()));

						constexpr auto stop_bits = ::std::array{ ::boost::asio::serial_port_base::stop_bits::one, ::boost::asio::serial_port_base::stop_bits::onepointfive, ::boost::asio::serial_port_base::stop_bits::two };
						constexpr auto stop_bits_item = ::std::array{ "one", "onepointfive", "two" };
						int stop_bits_index = ::std::ranges::distance(stop_bits.begin(), ::std::ranges::find(stop_bits, _context.runtime.uart.stop_bits.value()));

						constexpr auto flow_controls = ::std::array{ ::boost::asio::serial_port_base::flow_control::none, ::boost::asio::serial_port_base::flow_control::software, ::boost::asio::serial_port_base::flow_control::hardware };
						constexpr auto flow_controls_item = ::std::array{ "none", "software", "hardware" };
						int flow_control = ::std::ranges::distance(flow_controls.begin(), ::std::ranges::find(flow_controls, _context.runtime.uart.flow_control.value()));

						::ImGui::InputText("device", _context.runtime.uart.device.data(), _context.runtime.uart.device.size());
						::ImGui::Combo("baud rate", &baud_rate, baud_rates_item.data(), baud_rates.size());
						::ImGui::DragInt("character size", &character_size, 1, 5, 8, "%d");
						::ImGui::Combo("parity", &parity, parities_item.data(), parities.size());
						::ImGui::Combo("stop bits", &stop_bits_index, stop_bits_item.data(), stop_bits.size());
						::ImGui::Combo("flow control", &flow_control, flow_controls_item.data(), flow_controls.size());

						_context.runtime.uart.baud_rate = ::boost::asio::serial_port_base::baud_rate(baud_rates[baud_rate]);
						_context.runtime.uart.character_size = static_cast<::boost::asio::serial_port_base::character_size>(character_size);
						_context.runtime.uart.parity = ::boost::asio::serial_port_base::parity(parities[parity]);
						_context.runtime.uart.stop_bits = ::boost::asio::serial_port_base::stop_bits(stop_bits[stop_bits_index]);
						_context.runtime.uart.flow_control = ::boost::asio::serial_port_base::flow_control(flow_controls[flow_control]);

						if (_context.runtime.uart.is_opened)
						{
							if (::ImGui::Button("close", { width , 0 }))
							{
								_context.runtime.uart.is_opened = false;
								_context.serial_port.reset();
							}
						}
						else
						{
							if (::ImGui::Button("open", { width , 0 }))
							{

								try
								{
									_context.serial_port = ::std::make_unique<::boost::asio::serial_port>(_context.io, _context.runtime.uart.device);

									_context.serial_port->set_option(_context.runtime.uart.baud_rate);
									_context.serial_port->set_option(_context.runtime.uart.character_size); //数据位为8
									_context.serial_port->set_option(_context.runtime.uart.parity); //无奇偶校验
									_context.serial_port->set_option(_context.runtime.uart.stop_bits); //一位停止位
									_context.serial_port->set_option(_context.runtime.uart.flow_control); //无流控制

									_next_write_uart();
									_next_read_uart();

									_context.runtime.uart.is_opened = true;
									_open_message = "success";
								}
								catch (::std::exception& e)
								{
									NGS_LOGL(error, e.what());
									_open_message = "fail!";
								}
							}
						}
						::ImGui::SameLine();
						::ImGui::Text(_open_message.data());
					}
					::ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
			::ImGui::Separator();

			::ImGui::PopItemWidth();
			::ImGui::End();
		}

		::ImGui::SetNextWindowPos({ 18, 295 }, ImGuiCond_FirstUseEver);
		::ImGui::SetNextWindowSize({ 326,326 }, ImGuiCond_FirstUseEver);
		if (::ImGui::Begin("edfa config", nullptr, das_config::imgui_window_flag))
		{
			::ImGui::PushItemWidth(150);

			if (::ImGui::BeginTabBar("message", ImGuiTabBarFlags_None))
			{
				if (::ImGui::BeginTabItem("alarm"))
				{
					constexpr auto yes = "alarm!";
					constexpr auto no = "normal";

					::ImGui::Text("temperature: %s", _context.edfa_parameter.alarm_temperature ? yes : no);
					::ImGui::Text("output power: %s", _context.edfa_parameter.alarm_output_power ? yes : no);
					::ImGui::Text("input power: %s", _context.edfa_parameter.alarm_input_power ? yes : no);
					::ImGui::Text("pump status: %s", _context.edfa_parameter.alarm_pump_status ? "on" : "off");

					::ImGui::SeparatorText("pump 1");

					::ImGui::Text("cooler: %s", _context.edfa_parameter.alarm_pump_1_cooler ? yes : no);
					::ImGui::Text("chip temperature: %s", _context.edfa_parameter.alarm_pump_1_chip_temperature ? yes : no);
					::ImGui::Text("lop: %s", _context.edfa_parameter.alarm_pump_1_lop ? yes : no);

					::ImGui::SeparatorText("pump 2");

					::ImGui::Text("cooler: %s", _context.edfa_parameter.alarm_pump_2_cooler ? yes : no);
					::ImGui::Text("chip temperature: %s", _context.edfa_parameter.alarm_pump_2_chip_temperature ? yes : no);
					::ImGui::Text("lop: %s", _context.edfa_parameter.alarm_pump_2_lop ? yes : no);

					::ImGui::EndTabItem();
				}
				if (::ImGui::BeginTabItem("pump"))
				{
					::ImGui::SeparatorText("pump 1");
					::ImGui::Text("current: %.1f mA", _context.edfa_parameter.pump_1_lop / 10.0f);
					::ImGui::Text("power: %.1f mW", _context.edfa_parameter.pump_1_power / 10.0f);
					::ImGui::Text("temperature: %.1f *C", _context.edfa_parameter.pump_1_chip_temperature / 10.0f);
					::ImGui::Text("tec current: %.1f mA", _context.edfa_parameter.pump_1_cooler / 10.0f - 1420);

					::ImGui::SeparatorText("pump 2");
					::ImGui::Text("current: %.1f mA", _context.edfa_parameter.pump_2_lop / 10.0f);
					::ImGui::Text("power: %.1f mW", _context.edfa_parameter.pump_2_power / 10.0f);
					::ImGui::Text("temperature: %.1f *C", _context.edfa_parameter.pump_2_chip_temperature / 10.0f);
					::ImGui::Text("tec current: %.1f mA", _context.edfa_parameter.pump_2_cooler / 10.0f - 1420);

					::ImGui::EndTabItem();
				}
				if (::ImGui::BeginTabItem("other"))
				{
					static const ::std::unordered_map<::laser::mic::algorithm::all_parameter::working_mode_type, ::std::string_view> working_mode
					{
						{::laser::mic::algorithm::all_parameter::working_mode_type::acc, "acc"},
						{::laser::mic::algorithm::all_parameter::working_mode_type::agc, "agc"},
						{::laser::mic::algorithm::all_parameter::working_mode_type::apc, "apc"},
					};

					::ImGui::Text("working mode: %s, %d", working_mode.at(_context.edfa_parameter.working_mode).data(), _context.edfa_parameter.working_parameter);
					::ImGui::Text("temperature: %.1f *C", _context.edfa_parameter.temperature / 10.0f);

					::ImGui::SeparatorText("optical power");
					::ImGui::Text("input: %.1f dbm", _context.edfa_parameter.input_power / 10.0f - 70);
					::ImGui::Text("output: %.1f dbm", _context.edfa_parameter.output_power / 10.0f - 70);
					::ImGui::Text("input threshold: %.1f dbm", _context.edfa_parameter.input_power_threshold / 10.0f - 70);
					::ImGui::Text("output threshold: %.1f dbm", _context.edfa_parameter.output_power_threshold / 10.0f - 70);

					::ImGui::EndTabItem();
				}
				::ImGui::EndTabBar();
			}

			::ImGui::PopItemWidth();
			::ImGui::End();
		}
		if (_save_success)
			::ImGui::OpenPopup("__");
		if (::ImGui::BeginPopupModal("__"))
		{
			ImGui::Text("save success!");

			if (ImGui::Button("ok", ImVec2(120, 0)))
			{
				channel0_line_data().clear();
				channel1_line_data().clear();
				_channel0_line.clear();
				_channel1_line.clear();
				_context.time = 0;
				_save_success = false;
				ImGui::CloseCurrentPopup(); // 关闭当前的弹窗
			}
			// 必须调用 End 来结束弹窗的创建
			ImGui::EndPopup();
		}

		_context.plot.render();
	}

	context _context;
	::std::vector<packet_type> _read_buffer{};
	::std::size_t _read_buffer_index{};
	::das::ui::line_storage _channel0_line{};
	::das::ui::line_storage _channel1_line{};
	::std::string_view _open_message = "";
	::std::array<::std::uint8_t, 0x100> _receive_buffer{};
	::std::atomic_bool _done = false;

	stream _uart_stream{};

	::std::string _root_directory{};
	::std::array<char, 32> _type{ "unknown" };
	::std::uint32_t _hold_time = 2;
};


NGS_LIB_END