#pragma once

#include "../runtime.h"
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

	auto size() const
	{
		return ::std::min(independent_variable.size(), depend_variable.size());
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
};

struct graph
{
	::std::string title;
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

			for (auto&& [title, width, height, lines] : _graphs)
			{
				if (ImPlot::BeginPlot(title.data(), { width,height })) {
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


NGS_LIB_MODULE_END