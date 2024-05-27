#pragma once

#include "../runtime.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct line_storage
{
	::std::vector<short> independent_variable;
	::std::vector<short> depend_variable;

	void push(short x, short y)
	{
		independent_variable.push_back(x);
		depend_variable.push_back(y);
	}
};

struct line
{
	::std::span<const short> independent_variable;
	::std::span<const short> depend_variable;

	void bind(const line_storage& storage)
	{
		independent_variable = storage.independent_variable;
		depend_variable = storage.depend_variable;
	}
};

struct graph
{
	float width;
	float height;
	::std::unordered_map<::std::string, line> lines;
};

inline constexpr auto imgui_window_flag = 
	ImGuiWindowFlags_NoTitleBar
#if !defined(NGS_BUILD_TYPE_IS_DEBUG)
//	| ImGuiWindowFlags_NoMove
#endif
	| ImGuiWindowFlags_AlwaysAutoResize
	| ImGuiWindowFlags_NoBackground
;

struct plot
{
	::std::string _title;
	::std::vector<graph> _graphs{};


	plot(::std::string_view title)
		: _title(title)
	{

	}

	void render()
	{
		if (ImGui::Begin(_title.data(), nullptr, imgui_window_flag))
		{
			static float alpha = 0.25f;
			ImGui::DragFloat("alpha", &alpha, 0.01f, 0, 1);

			for (auto&& [width, height, lines] : _graphs)
			{
				if (ImPlot::BeginPlot(_title.data(), { width,height })) {
					ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, alpha);

					for (auto& [line_name, line] : lines)
					{
						auto&& [independent_variable, depend_variable] = line;
						auto size = ::std::min(::std::initializer_list<::std::size_t>{ ::std::ranges::size(independent_variable), ::std::ranges::size(depend_variable) });

						ImPlot::PlotLine(line_name.data(), independent_variable.data(), depend_variable.data(), static_cast<int>(size));
					}

					ImPlot::PopStyleVar();
					ImPlot::EndPlot();
				}
			}
			ImGui::End();
		}
	}

	void push_graph(const graph& graph) { _graphs.emplace_back(graph); }
	void push_graph(graph&& graph) { _graphs.emplace_back(::std::move(graph)); }

	auto&& graph() { return _graphs; }
	auto&& graph() const { return _graphs; }
};


void render_ui()
{
	if (ImGui::Begin("config 1", nullptr, imgui_window_flag))
	{
		ImGui::Button("start");
		ImGui::SameLine();
		ImGui::Button("quit");

		ImGui::SeparatorText("unknown");
		{
			int scan_rate = static_cast<int>(runtime::data.pcie_config.scan_rate);
			int pulse_width = static_cast<int>(runtime::data.pcie_config.trigger_pulse_width);
			int total_point_number = static_cast<int>(runtime::data.total_point_number);
			int center_frequency = static_cast<int>(runtime::data.pcie_config.center_frequency);
			//static float fiber_length = 0.0f;

			ImGui::DragInt("scan rate", &scan_rate, 100, 0, 10000, "%dhz");
			ImGui::DragInt("pusle width", &pulse_width, 1, 0, ::std::numeric_limits<int>::max(), "%dms");
			ImGui::DragInt("total point number", &total_point_number, 256, 0, ::std::numeric_limits<int>::max());
			ImGui::DragInt("center frequency", &center_frequency, 1, 0, ::std::numeric_limits<int>::max(), "%dmhz");
			//ImGui::DragFloat("fiber length", &runtime_image.fiber_length, 1, 0, ::std::numeric_limits<float>::max(), "%.2fkm");

			runtime::data.pcie_config.scan_rate = static_cast<::std::uint32_t>(scan_rate);
			runtime::data.pcie_config.trigger_pulse_width = static_cast<::std::uint32_t>(pulse_width);
			runtime::data.total_point_number = static_cast<::std::uint32_t>(total_point_number);
			runtime::data.pcie_config.center_frequency = static_cast<::std::uint32_t>(center_frequency);
		}
		

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

NGS_LIB_MODULE_END