#pragma once

#include "./stream.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct instance
{

	void update()
	{
		_context.poll();
	}

	auto open(::std::string_view device)
	{
		::boost::system::error_code error_code;
		_io.open(device.data(), error_code);
		if(error_code)
		{
			NGS_LOGL(error, "open serial port error: ", error_code.message());
			return false;
		}
		_io.set_option(_config.baud_rate);
		_io.set_option(_config.character_size);
		_io.set_option(_config.parity);
		_io.set_option(_config.stop_bits);
		_io.set_option(_config.flow_control);

		::boost::asio::co_spawn(_context, _read_all_parameter_send(), ::boost::asio::detached);
		::boost::asio::co_spawn(_context, _read_all_parameter_receive(), ::boost::asio::detached);

		return true;
	}

	auto close()
	{
		::boost::system::error_code error_code;
		_io.close(error_code);
		if (error_code)
		{
			NGS_LOGL(error, "open serial port error: ", error_code.message());
			return false;
		}

		_context.reset();

		return true;
	}

	::boost::asio::awaitable<void> _read_all_parameter_send()
	{
		constexpr auto buffer = ::edfa::commands::read_all_parameters();
		::std::size_t bytes_transferred = 0;
		while (bytes_transferred < ::std::ranges::size(buffer))
		{
			bytes_transferred += co_await _io.async_write_some(::boost::asio::buffer(buffer), ::boost::asio::use_awaitable);
		}
	}
	::boost::asio::awaitable<void> _read_all_parameter_receive()
	{
		::std::array<::std::byte, 256> buffer{};
		while(true)
		{
			auto n = co_await _io.async_read_some(::boost::asio::buffer(buffer), ::boost::asio::use_awaitable);
			_parser.in(buffer | ::std::views::take(n));
			if (_parser.empty())
				continue;

			auto [data] = _parser.out();
			auto check_result = ::edfa::algorithm::receive_check(::edfa::protocols::command::read_all_parameters, ::std::span{ reinterpret_cast<::std::uint8_t*>(data.data()),data.size() });

			if (::std::ranges::empty(check_result) || ::std::ranges::size(check_result) != sizeof(::edfa::algorithm::all_parameter))
			{
				NGS_LOGL(error, "receive data error: ");
				continue;
			}

			_parameter = *reinterpret_cast<const ::edfa::algorithm::all_parameter*>(::std::ranges::data(check_result));
			_parameter.rectified_endian();
		}
	}

	void render_config()
	{
		::ImGui::BeginDisabled(_io.is_open());

		::ImGui::InputText("device", _config.device.data(), _config.device.size());
		{
			constexpr auto items = ::std::array{ 300, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200 };
			auto current = _config.baud_rate.value();
			::ngs::external::imgui::components::combo("baud rate", current, items);
			_config.baud_rate = ::boost::asio::serial_port_base::baud_rate{ current };
		}
		{
			auto current = _config.character_size.value();
			::ngs::external::imgui::components::drag("character size", current, 1, 5, 8);
			_config.character_size = ::boost::asio::serial_port_base::character_size{ current };
		}
		{
			auto current = _config.parity.value();
			::ngs::external::imgui::components::combo("parity", current);
			_config.parity = ::boost::asio::serial_port_base::parity{ current };
		}
		{
			auto current = _config.stop_bits.value();
			::ngs::external::imgui::components::combo("stop bits", current);
			_config.stop_bits = ::boost::asio::serial_port_base::stop_bits{ current };
		}
		{
			auto current = _config.flow_control.value();
			::ngs::external::imgui::components::combo("flow control", current);
			_config.flow_control = ::boost::asio::serial_port_base::flow_control{ current };
		}

		::ImGui::EndDisabled();
	}

	void render_control()
	{
		if (_io.is_open())
		{
			if (::ImGui::Button("close", {100, 0}))
			{
				close();
			}
		}
		else
		{
			if (::ImGui::Button("open", { 100, 0 }))
			{
				open(_config.device.data());
			}
		}
	}

	void render_message()
	{
		auto alpha = _io.is_open() ? 1.0f : 0.5f;
		::ImGui::PushStyleVar(::ImGuiStyleVar_Alpha, ::ImGui::GetStyle().Alpha * alpha);

		constexpr auto yes = "alarm!";
		constexpr auto no = "normal";

		::ImGui::Text("temperature: %s", _parameter.alarm_temperature ? yes : no);
		//::ImGui::SameLine();
		::ImGui::Text("pump status: %s", _parameter.alarm_pump_status ? "on" : "off");

		::ImGui::Text("output power: %s", _parameter.alarm_output_power ? yes : no);
		//::ImGui::SameLine();
		::ImGui::Text("input power: %s", _parameter.alarm_input_power ? yes : no);

		::ImGui::SeparatorText("pump 1");

		::ImGui::Text("cooler: %s", _parameter.alarm_pump_1_cooler ? yes : no);
		::ImGui::Text("chip temperature: %s", _parameter.alarm_pump_1_chip_temperature ? yes : no);
		::ImGui::Text("lop: %s", _parameter.alarm_pump_1_lop ? yes : no);
		::ImGui::Text("current: %.1f mA", _parameter.pump_1_lop / 10.0f);
		::ImGui::Text("power: %.1f mW", _parameter.pump_1_power / 10.0f);
		::ImGui::Text("temperature: %.1f *C", _parameter.pump_1_chip_temperature / 10.0f);
		::ImGui::Text("tec current: %.1f mA", _parameter.pump_1_cooler / 10.0f - 1420);

		::ImGui::SeparatorText("pump 2");

		::ImGui::Text("cooler: %s", _parameter.alarm_pump_2_cooler ? yes : no);
		::ImGui::Text("chip temperature: %s", _parameter.alarm_pump_2_chip_temperature ? yes : no);
		::ImGui::Text("lop: %s", _parameter.alarm_pump_2_lop ? yes : no);
		::ImGui::Text("current: %.1f mA", _parameter.pump_2_lop / 10.0f);
		::ImGui::Text("power: %.1f mW", _parameter.pump_2_power / 10.0f);
		::ImGui::Text("temperature: %.1f *C", _parameter.pump_2_chip_temperature / 10.0f);
		::ImGui::Text("tec current: %.1f mA", _parameter.pump_2_cooler / 10.0f - 1420);

		::ImGui::SeparatorText("other");
		static const ::std::unordered_map<::edfa::algorithm::all_parameter::working_mode_type, ::std::string_view> working_mode
			{
				{::edfa::algorithm::all_parameter::working_mode_type::acc, "acc"},
				{::edfa::algorithm::all_parameter::working_mode_type::agc, "agc"},
				{::edfa::algorithm::all_parameter::working_mode_type::apc, "apc"},
			};
		::ImGui::Text("working mode: %s, %d", working_mode.at(_parameter.working_mode).data(), _parameter.working_parameter);
		::ImGui::Text("temperature: %.1f *C", _parameter.temperature / 10.0f);

		::ImGui::SeparatorText("optical power");
		::ImGui::Text("input: %.1f dbm", _parameter.input_power / 10.0f - 70);
		::ImGui::Text("output: %.1f dbm", _parameter.output_power / 10.0f - 70);
		::ImGui::Text("input threshold: %.1f dbm", _parameter.input_power_threshold / 10.0f - 70);
		::ImGui::Text("output threshold: %.1f dbm", _parameter.output_power_threshold / 10.0f - 70);

		::ImGui::PopStyleVar();
	}

	::boost::asio::io_context _context{};
	::boost::asio::serial_port _io{ _context };
	::edfa::algorithm::all_parameter _parameter{};
	stream _parser{};

	struct
	{
		::std::array<char, 32> device{ "COM7" };
		::boost::asio::serial_port_base::baud_rate baud_rate{ 9600 };
		::boost::asio::serial_port_base::character_size character_size{ 8 };
		::boost::asio::serial_port_base::parity parity{ ::boost::asio::serial_port_base::parity::none };
		::boost::asio::serial_port_base::stop_bits stop_bits{ ::boost::asio::serial_port_base::stop_bits::one };
		::boost::asio::serial_port_base::flow_control flow_control{ ::boost::asio::serial_port_base::flow_control::none };
	}_config{};
};

NGS_LIB_MODULE_END