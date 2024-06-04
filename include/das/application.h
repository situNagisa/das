#pragma once

#include "./context.h"
#include "./defined.h"

NGS_LIB_BEGIN

struct application
{
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

		if(_context.runtime.uart.is_opened){
			{
				::std::vector<::std::uint8_t> send_buffer(::laser::mic::algorithm::send_size(0));

				auto result = ::laser::mic::algorithm::send(::laser::mic::protocols::command::read_all_parameters, send_buffer.begin());

				auto length = _context.serial_port->write_some(::boost::asio::buffer(send_buffer));
				NGS_EXPECT(::std::ranges::distance(send_buffer.begin(), result) == length);
			}
			{
				
				_context.serial_port->async_read_some(::boost::asio::buffer(_receive_buffer), [this](const ::boost::system::error_code& error, ::std::size_t bytes_transferred)
					{
						if (error)
						{
							NGS_LOGL(error, error.message());
							return;
						}
						auto data = ::laser::mic::algorithm::receive_check(::laser::mic::protocols::command::read_all_parameters, _receive_buffer | ::std::views::take(bytes_transferred));

						NGS_ASSERT(::std::ranges::size(data) == sizeof(::laser::mic::algorithm::all_parameter));
						_context.edfa_parameter = *reinterpret_cast<const ::laser::mic::algorithm::all_parameter*>(::std::ranges::data(data));
					});
			}
		}
		
	}

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
					if (_context.runtime.recording)
					{
						if (::ImGui::Button("stop", { width , 0 }))
							_context.runtime.recording = false;
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
						_context.time = 0;
					}

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
					::ImGui::Text("temperature: %s", _context.edfa_parameter.alarm_temperature ? "true" : "false");
					::ImGui::Text("output power: %s", _context.edfa_parameter.alarm_output_power ? "true" : "false");
					::ImGui::Text("input power: %s", _context.edfa_parameter.alarm_input_power ? "true" : "false");
					::ImGui::Text("pump status: %s", _context.edfa_parameter.alarm_pump_status ? "on" : "off");

					::ImGui::SeparatorText("pump 1");

					::ImGui::Text("cooler: %s", _context.edfa_parameter.alarm_pump_1_cooler ? " true" : "false");
					::ImGui::Text("chip temperature: %s", _context.edfa_parameter.alarm_pump_1_chip_temperature ? " true" : "false");
					::ImGui::Text("lop: %s", _context.edfa_parameter.alarm_pump_1_lop ? " true" : "false");

					::ImGui::SeparatorText("pump 2");

					::ImGui::Text("cooler: %s", _context.edfa_parameter.alarm_pump_2_cooler ? " true" : "false");
					::ImGui::Text("chip temperature: %s", _context.edfa_parameter.alarm_pump_2_chip_temperature ? " true" : "false");
					::ImGui::Text("lop: %s", _context.edfa_parameter.alarm_pump_2_lop ? " true" : "false");

					::ImGui::EndTabItem();
				}
				if (::ImGui::BeginTabItem("pump"))
				{
					::ImGui::SeparatorText("pump 1");
					::ImGui::Text("lop: %d", _context.edfa_parameter.pump_1_lop);
					::ImGui::Text("power: %d", _context.edfa_parameter.pump_1_power);
					::ImGui::Text("chip temperature: %d", _context.edfa_parameter.pump_1_chip_temperature);
					::ImGui::Text("cooler: %d", _context.edfa_parameter.pump_1_cooler);

					::ImGui::SeparatorText("pump 2");
					::ImGui::Text("lop: %d", _context.edfa_parameter.pump_2_lop);
					::ImGui::Text("power: %d", _context.edfa_parameter.pump_2_power);
					::ImGui::Text("chip temperature: %d", _context.edfa_parameter.pump_2_chip_temperature);
					::ImGui::Text("cooler: %d", _context.edfa_parameter.pump_2_cooler);

					::ImGui::EndTabItem();
				}
				if (::ImGui::BeginTabItem("other"))
				{
					::ImGui::Text("working mode: %d, %d", static_cast<int>(_context.edfa_parameter.working_mode), _context.edfa_parameter.working_parameter);

					::ImGui::SeparatorText("optical power");
					::ImGui::Text("input: %d", _context.edfa_parameter.input_power);
					::ImGui::Text("output: %d", _context.edfa_parameter.output_power);
					::ImGui::Text("input threshold: %d", _context.edfa_parameter.input_power_threshold);
					::ImGui::Text("output threshold: %d", _context.edfa_parameter.output_power_threshold);

					::ImGui::EndTabItem();
				}

				::ImGui::EndTabBar();
			}

			::ImGui::PopItemWidth();
			::ImGui::End();
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
};


NGS_LIB_END