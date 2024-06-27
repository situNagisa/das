#pragma once

#include "../config.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

using integral_type = ::std::int32_t;

struct line_storage
{
	::std::vector<integral_type> independent_variable;
	::std::vector<integral_type> depend_variable;

	void push(integral_type x, integral_type y)
	{
		independent_variable.push_back(x);
		depend_variable.push_back(y);
	}

	void resize(::std::size_t size)
	{
		independent_variable.resize(size);
		depend_variable.resize(size);
	}

	auto empty() const { return independent_variable.empty() && depend_variable.empty(); }

	auto size() const
	{
		return ::std::min(independent_variable.size(), depend_variable.size());
	}

	void clear()
	{
		independent_variable.clear();
		depend_variable.clear();
	}
};

struct line
{
	::std::span<const integral_type> independent_variable;
	::std::span<const integral_type> depend_variable;

	void bind(const line_storage& storage)
	{
		independent_variable = storage.independent_variable;
		depend_variable = storage.depend_variable;
	}

	void clear()
	{
		independent_variable = {};
		depend_variable = {};
	}
};

struct graph
{
	::std::string title;
	float width;
	float height;
	::std::unordered_map<::std::string, line> lines;
};

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
		ImGui::SetNextWindowPos({ 376,17 }, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize({ 916,620 }, ImGuiCond_FirstUseEver);
		if (ImGui::Begin(_title.data(), nullptr, das_config::imgui_window_flag | ImGuiWindowFlags_AlwaysAutoResize))
		{
			for (auto&& [title, width, height, lines] : _graphs)
			{
				if (ImPlot::BeginPlot(title.data(), { width,height })) {
					for (auto& [line_name, line] : lines)
					{
						auto&& [independent_variable, depend_variable] = line;
						auto size = ::std::min(::std::initializer_list<::std::size_t>{ ::std::ranges::size(independent_variable), ::std::ranges::size(depend_variable) });

						ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_None, ImPlotAxisFlags_AutoFit);
						if(!independent_variable.empty())
						{
							ImPlot::SetupAxisLimits(ImAxis_X1, independent_variable.front(), independent_variable.front() + das_config::limit_point, ImGuiCond_Always);
						}
						//if(!depend_variable.empty())
						//{
						//	ImPlot::SetupAxisLimits(ImAxis_Y1, depend_variable.front(), depend_variable.back(), ImGuiCond_Appearing);
						//}
						//ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1);
						ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);

						ImPlot::PlotLine(line_name.data(), independent_variable.data(), depend_variable.data(), static_cast<int>(size));
					}
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


NGS_LIB_MODULE_END