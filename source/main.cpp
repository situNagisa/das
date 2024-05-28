#include "./das/das.h"

using packet_type = ::pcie6920::atomic::packet<pcie6920::enums::parse_rule::raw_data, pcie6920::enums::channel_quantity::_1>;


auto start(::pcie6920::guard::open& pcie, ::pcie6920::guard::io& io,::das::ui::line_storage& channel0, ::das::ui::line_storage& channel1,::std::size_t time)
{
	::std::vector<packet_type> read_buffer(::das::runtime::data.packet_size_per_scan);

	read_buffer.resize(io.read(read_buffer));

	if (::das::runtime::data.pcie_config.data_source_sel == pcie6920::enums::parse_rule::raw_data)
	{
		constexpr ::std::size_t limit_point = ::std::numeric_limits<short>::max() / 16;

		auto buffer = ::pcie6920::atomic::unpack(read_buffer);
		auto push = [time](::das::ui::line_storage& line, decltype(buffer) buffer, ::std::size_t channel_index)
			{
				auto push_size = ::std::min(limit_point, ::std::ranges::size(buffer));

				if (line.size() >= limit_point)
				{
					::std::ranges::shift_left(line.independent_variable, static_cast<long long>(::std::ranges::size(buffer)));
					::std::ranges::shift_left(line.depend_variable, static_cast<long long>(::std::ranges::size(buffer)));
					line.resize(limit_point - push_size);
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

	if (ImGui::Begin("config 1", nullptr, imgui_window_flag))
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

// Main code
int main(int, char**)
{
	using namespace ::std::chrono_literals;

	::das::ui::gui gui{};
	::das::ui::plot plot{ "shaded plots"};
	plot.push_graph({
			.title = "main",
			.width = 900,
			.height = 300,
			.lines = {
				{
					"channel 0",
					::das::ui::line{}
				},
				{
					"channel 1",
					::das::ui::line{}
				}
			}
		});
	plot.push_graph({
			.title = "analyse",
			.width = 900,
			.height = 300,
			.lines = {
				{
					"channel 0",
					::das::ui::line{}
				},
				{
					"channel 1",
					::das::ui::line{}
				}
			}
		});

	auto&& main_graph = plot.graph()[0];
	auto&& analyse_graph = plot.graph()[1];

	auto&& channel0_line = main_graph.lines.at("channel 0");
	auto&& channel1_line = main_graph.lines["channel 1"];

	::das::ui::line_storage channel0_line_data{};
	::das::ui::line_storage channel1_line_data{};

	::das::runtime::data.pcie_config = {
		.demodulation_channel_quantity = ::pcie6920::enums::channel_quantity::_1,
		.points_per_scan = 256 * 64,
		.scan_rate = 2000,
		.trigger_pulse_width = 4,
		.center_frequency = 80000000,
		.data_source_sel = ::pcie6920::enums::parse_rule::raw_data,
		.upload_rate_sel = ::pcie6920::enums::upload_rate::_250m
	};
	
	::pcie6920::guard::open pcie{};
	::pcie6920::atomic::config(::das::runtime::data.pcie_config);

	::std::vector<packet_type> read_buffer{};
	::std::size_t time = 0;

	auto now = ::std::chrono::system_clock::now();

	while (!gui.window().is_should_close())
	{
		auto render_guard = gui.render_guard();
		{
			{
				using namespace ::das;
				using namespace ::das::ui;

				if (ImGui::Begin("config 1", nullptr, imgui_window_flag))
				{
					if (runtime::data.recording)
					{
						if (ImGui::Button("stop"))
						{
							runtime::data.recording = false;
						}
					}
					else
					{
						if (ImGui::Button("start"))
						{
							runtime::data.recording = true;
						}
					}
					{
						int packet_size_per_scan = static_cast<int>(runtime::data.packet_size_per_scan);
						ImGui::DragInt("packet size per scan", &packet_size_per_scan, 1, 1, 32);
						runtime::data.packet_size_per_scan = static_cast<::std::uint32_t>(packet_size_per_scan);
					}

					ImGui::SeparatorText("config");
					{
						if (ImGui::Button("config"))
						{
							::pcie6920::atomic::config(::das::runtime::data.pcie_config);
						}
						{
							int scan_rate = static_cast<int>(runtime::data.pcie_config.scan_rate);
							int pulse_width = static_cast<int>(runtime::data.pcie_config.trigger_pulse_width);
							int center_frequency = static_cast<int>(runtime::data.pcie_config.center_frequency);
							//static float fiber_length = 0.0f;

							ImGui::DragInt("scan rate", &scan_rate, 100, 0, 10000, "%dhz");
							ImGui::DragInt("pusle width", &pulse_width, 1, 0, ::std::numeric_limits<int>::max(), "%dms");
							ImGui::DragInt("center frequency", &center_frequency, 1, 0, ::std::numeric_limits<int>::max(), "%dmhz");
							//ImGui::DragFloat("fiber length", &runtime_image.fiber_length, 1, 0, ::std::numeric_limits<float>::max(), "%.2fkm");

							runtime::data.pcie_config.scan_rate = static_cast<::std::uint32_t>(scan_rate);
							runtime::data.pcie_config.trigger_pulse_width = static_cast<::std::uint32_t>(pulse_width);
							runtime::data.pcie_config.center_frequency = static_cast<::std::uint32_t>(center_frequency);
						}
					}

					ImGui::End();
				}
			}
			if(::das::runtime::data.recording)
			{
				if (auto io = pcie.io(); io.buffer_packet_size() >= ::das::runtime::data.packet_size_per_scan)
				{
					if (::std::chrono::system_clock::now() - now > 1s)
					{
						now = ::std::chrono::system_clock::now();

						time = start(pcie, io, channel0_line_data, channel1_line_data, time);

						channel0_line.bind(channel0_line_data);
						channel1_line.bind(channel1_line_data);
					}
				}
			}
			plot.render();
		}
	}

	return 0;
}