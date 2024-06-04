#include "./das/das.h"

using packet_type = ::laser::pcie6920::atomic::packet<laser::pcie6920::enums::parse_rule::raw_data, laser::pcie6920::enums::channel_quantity::_1>;


auto start(::das::ui::line_storage& channel0, ::das::ui::line_storage& channel1,::std::size_t time, ::std::span<packet_type> read_buffer)
{
	if (::das::runtime::data.pcie_config.data_source_sel == laser::pcie6920::enums::parse_rule::raw_data)
	{
		auto buffer = ::laser::pcie6920::atomic::unpack(read_buffer);
		auto push = [time](::das::ui::line_storage& line, decltype(buffer) buffer, ::std::size_t channel_index)
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

		push(channel0, buffer, 0);
		push(channel1, buffer, 1);

		time += ::std::ranges::size(buffer);
	}

	return time;
}

void render_ui()
{
	using namespace ::das;
	using namespace ::das::ui;

	if (ImGui::Begin("config 1", nullptr, das_config::imgui_window_flag))
	{


		/*

		ImGui::SeparatorText("unknown");
		//static int data_source = 0;
		constexpr ::std::array data_sources = { "raw data", "lpf iq", "arctan sqrt" };
		//static int upload_rate_index = 0;
		constexpr ::std::array upload_rate = { "250m", "125m", "83.3m", "62.5m", "50m" };

		runtime_image.data_source.data = data_sources;
		runtime_image.upload_rate.data = upload_rate;

		ImGui::Combo("data source", &runtime_image.data_source.index, runtime_image.data_source.data.data(), static_cast<int>(runtime_image.data_source.data.size()));
		ImGui::Combo("upload rate", &runtime_image.upload_rate.index, runtime_image.upload_rate.data.data(), static_cast<int>(runtime_image.upload_rate.data.size()));

		ImGui::SeparatorText("unknown");
		//static bool sw_dem = false;
		//static bool spectrum_enable = false;
		if (ImGui::BeginTable("split", 2))
		{
			ImGui::TableNextColumn();	ImGui::Checkbox("sw dem", &runtime_image.sw_dem);
			ImGui::TableNextColumn();	ImGui::Checkbox("spectrum enable", &runtime_image.spectrum_enable);
			ImGui::EndTable();
		}
		ImGui::SeparatorText("unknown");

		//static int psd_index = 0;
		constexpr ::std::array psd = { "em", "dis" };
		runtime_image.psd.data = psd;
		ImGui::ListBox("psd", &runtime_image.psd.index, runtime_image.psd.data.data(), static_cast<int>(runtime_image.psd.data.size()));

		if (runtime_image.spectrum_enable)
		{
			//static int spectrum_index = 0;
			constexpr ::std::array spectrum = { "channel 0", "channel 1" };
			runtime_image.spectrum.data = spectrum;
			ImGui::ListBox("spectrum", &runtime_image.spectrum.index, runtime_image.spectrum.data.data(), static_cast<int>(runtime_image.spectrum.data.size()));
		}
		*/
		ImGui::End();
	}
	/*
	if (false && ImGui::Begin("config 2", nullptr, imgui_window_flag))
	{
		ImGui::SeparatorText("unknown");
		//static ::std::uint64_t register_address = 0;
		//static ::std::uint64_t write_register_data_address = 0;
		//static ::std::uint64_t read_register_data_address = 0;
		constexpr auto min = ::std::numeric_limits<::std::uint64_t>::min();
		constexpr auto max = ::std::numeric_limits<::std::uint64_t>::max();

		ImGui::DragScalar("register address", ImGuiDataType_U64, &runtime_image.register_address, 1, &min, &max, "0x%8x");
		ImGui::DragScalar("write register data address", ImGuiDataType_U64, &runtime_image.write_register_data_address, 1, &min, &max, "0x%8x");
		ImGui::DragScalar("read register data address", ImGuiDataType_U64, &runtime_image.read_register_data_address, 1, &min, &max, "0x%8x");

		ImGui::SeparatorText("unknown");
		ImGui::Button("write register");
		ImGui::SameLine();
		ImGui::Button("read register");

		ImGui::End();
	}
	if (false && ImGui::Begin("config 3", nullptr, imgui_window_flag))
	{
		ImGui::SeparatorText("unknown");
		//static ::std::uint64_t write_offset_data_channel_0 = 0;
		//static ::std::uint64_t write_offset_data_channel_1 = 0;
		//static ::std::uint64_t read_offset_data_channel_0 = 0;
		//static ::std::uint64_t read_offset_data_channel_1 = 0;
		constexpr auto min = ::std::numeric_limits<::std::uint64_t>::min();
		constexpr auto max = ::std::numeric_limits<::std::uint64_t>::max();

		ImGui::DragScalar("write offset channel0", ImGuiDataType_U64, &runtime_image.write_offset_data_channel_0, 1, &min, &max, "0x%8x");
		ImGui::DragScalar("write offset channel1", ImGuiDataType_U64, &runtime_image.write_offset_data_channel_1, 1, &min, &max, "0x%8x");
		ImGui::DragScalar("read offset channel0", ImGuiDataType_U64, &runtime_image.read_offset_data_channel_0, 1, &min, &max, "0x%8x");
		ImGui::DragScalar("read offset channel1", ImGuiDataType_U64, &runtime_image.read_offset_data_channel_1, 1, &min, &max, "0x%8x");

		ImGui::SeparatorText("unknown");
		ImGui::Button("write offset");
		ImGui::SameLine();
		ImGui::Button("read offset");

		ImGui::End();
	}
	*/
}

constexpr auto to_string(::std::ranges::range auto&& range, ::std::string_view formatter)
{
	::std::string result{};
	for (auto&& value : range | ::std::views::transform([formatter](auto&& value) { return ::std::vformat(formatter, ::std::make_format_args(NGS_PP_PERFECT_FORWARD(value))); }))
	{
		result += value;
	}
	return result;
}

void mic_test()
{
	namespace asio = ::boost::asio;

	try
	{
		asio::io_service io{};
		asio::serial_port sp(io, "COM7");

		sp.set_option(asio::serial_port::baud_rate(9600));
		sp.set_option(asio::serial_port::character_size(8)); //数据位为8
		sp.set_option(asio::serial_port::parity(asio::serial_port::parity::none)); //无奇偶校验
		sp.set_option(asio::serial_port::stop_bits(asio::serial_port::stop_bits::one)); //一位停止位
		sp.set_option(asio::serial_port::flow_control(asio::serial_port::flow_control::none)); //无流控制

		{
			::std::array<::std::uint8_t, 0x100> send_buffer{};

			::laser::mic::algorithm::send(::laser::mic::protocols::command::read_all_parameters, send_buffer.begin());

			auto length = sp.write_some(asio::buffer(send_buffer));
			NGS_LOGL(info, ::std::format("send length:{}, {}", length, ::to_string(send_buffer, "{:02x} ")));
		}
		
		::std::array<::std::uint8_t, 64> receive_buffer{};

		sp.async_read_some(asio::buffer(receive_buffer), [&receive_buffer](const ::boost::system::error_code& error, ::std::size_t bytes_transferred)
			{
				if(error)
				{
					NGS_LOGL(error, error.message());
					return;
				}
				NGS_LOGL(info, ::std::format("receive length:{}, {}", bytes_transferred, ::to_string(receive_buffer | ::std::views::take(bytes_transferred), "{:02x} ")));
			});

		io.run();
	}
	catch(::std::exception& e)
	{
		NGS_LOGL(error, e.what());
	}

}

// Main code
int main(int, char**)
{
	using namespace ::std::chrono_literals;

	::das::ui::gui gui{};
	::das::ui::plot plot{ "shaded plots"};
	plot.push_graph({
			.title = "channel 0",
			.width = 900,
			.height = 300,
			.lines = {
				{
					"channel",
					::das::ui::line{}
				},
			}
		});
	plot.push_graph({
			.title = "channel 1",
			.width = 900,
			.height = 300,
			.lines = {
				{
					"channel",
					::das::ui::line{}
				}
			}
		});

	auto&& graph_channel0 = plot.graph()[0];
	auto&& graph_channel1 = plot.graph()[1];

	auto&& channel0_line = graph_channel0.lines["channel"];
	auto&& channel1_line = graph_channel1.lines["channel"];

	::das::ui::line_storage channel0_line_data{};
	::das::ui::line_storage channel1_line_data{};

	::das::runtime::data.pcie_config = {
		.demodulation_channel_quantity = ::laser::pcie6920::enums::channel_quantity::_1,
		.packets_per_scan = 64,
		.scan_rate = 2000,
		.trigger_pulse_width = 4,
		.center_frequency = 80000000,
		.data_source_sel = ::laser::pcie6920::enums::parse_rule::raw_data,
		.upload_rate_sel = ::laser::pcie6920::enums::upload_rate::_250m
	};
	
	::laser::pcie6920::guard::open pcie{};
	::laser::pcie6920::atomic::config(::das::runtime::data.pcie_config);

	::std::vector<packet_type> read_buffer{};
	::std::size_t read_buffer_index{};
	::std::size_t time = 0;


	mic_test();
	while (!gui.window().is_should_close())
	{
		auto render_guard = gui.render_guard();
		{
			{
				using namespace ::das;
				using namespace ::das::ui;

				ImGui::SetNextWindowPos({ 8,23 }, ImGuiCond_FirstUseEver);
				ImGui::SetNextWindowSize({ 348,272 }, ImGuiCond_FirstUseEver);
				if (ImGui::Begin("config 1", nullptr, das_config::imgui_window_flag))
				{
					constexpr auto width = 100;
					ImGui::PushItemWidth(150);

					if (runtime::data.recording)
					{
						if (ImGui::Button("stop",{ width , 0}))
						{
							runtime::data.recording = false;
						}
					}
					else
					{
						if (ImGui::Button("start", { width , 0 }))
						{
							runtime::data.recording = true;
						}
					}
					if(ImGui::Button("clear", { width , 0 }))
					{
						channel0_line_data.clear();
						channel1_line_data.clear();
						channel0_line.bind(channel0_line_data);
						channel1_line.bind(channel1_line_data);
						time = 0;
					}

					ImGui::SeparatorText("config");
					{
						if (ImGui::Button("config", { width , 0 }))
						{
							::laser::pcie6920::atomic::config(::das::runtime::data.pcie_config);
						}
						{
							int packets_per_scan = static_cast<int>(runtime::data.pcie_config.packets_per_scan);
							int scan_rate = static_cast<int>(runtime::data.pcie_config.scan_rate);
							int pulse_width = static_cast<int>(runtime::data.pcie_config.trigger_pulse_width);
							int center_frequency = static_cast<int>(runtime::data.pcie_config.center_frequency);
							//static float fiber_length = 0.0f;

							ImGui::DragInt("packets/scan", &packets_per_scan, 1, 0, 128, "%d");
							ImGui::DragInt("scan rate", &scan_rate, 100, 0, 10000, "%dhz");
							ImGui::DragInt("pusle width", &pulse_width, 1, 0, ::std::numeric_limits<int>::max(), "%dms");
							ImGui::DragInt("center frequency", &center_frequency, 1, 0, ::std::numeric_limits<int>::max(), "%dmhz");
							//ImGui::DragFloat("fiber length", &runtime_image.fiber_length, 1, 0, ::std::numeric_limits<float>::max(), "%.2fkm");

							runtime::data.pcie_config.packets_per_scan = static_cast<::std::uint32_t>(packets_per_scan);
							runtime::data.pcie_config.scan_rate = static_cast<::std::uint32_t>(scan_rate);
							runtime::data.pcie_config.trigger_pulse_width = static_cast<::std::uint32_t>(pulse_width);
							runtime::data.pcie_config.center_frequency = static_cast<::std::uint32_t>(center_frequency);
						}
					}
					ImGui::PopItemWidth();
					ImGui::End();
				}
			}

			if(::das::runtime::data.recording)
			{
				auto io = pcie.io();

				if (read_buffer_index < read_buffer.size())
				{
					time = start(channel0_line_data, channel1_line_data, time, ::std::span{ read_buffer }.subspan(read_buffer_index, 1));
					++read_buffer_index;
				}

				if ( io.buffer_packet_size())
				{
					if(read_buffer_index < read_buffer.size())
					{
						time = start(channel0_line_data, channel1_line_data, time, ::std::span{ read_buffer }.subspan(read_buffer_index) );
					}
					read_buffer_index = 0;

					read_buffer.resize(io.buffer_packet_size());
					read_buffer.resize(io.read(read_buffer));
				}

				channel0_line.bind(channel0_line_data);
				channel1_line.bind(channel1_line_data);
			}
			plot.render();
		}
	}

	return 0;
}