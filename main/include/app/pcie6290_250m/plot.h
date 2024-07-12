#pragma once

#include "./type.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct plot
{
	void _render_line(::std::string_view title, ::std::span<point_t> dependent)
	{
		::ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_None, ImPlotAxisFlags_AutoFit);
		auto start = _independent.empty() ? 0.0 : static_cast<double>(_independent.front());
		::ImPlot::SetupAxisLimits(ImAxis_X1,
			start,
			start + _independent.size(),
			ImGuiCond_Always);
		::ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);

		::quick_ui::components::plot_line(title, ::std::span(_independent), dependent);
	}

	void render(::std::string_view title, ::std::pair<float, float> size, ::std::span<point_t> dependent)
	{
		if (::ImPlot::BeginPlot(title.data(), { size.first, size.second }))
		{
			_render_line(title.data(), dependent);
			::ImPlot::EndPlot();
		}
	}

	void set_time_span(::std::size_t size)
	{
		_independent.resize(size);
		_independent.shrink_to_fit();
		::std::ranges::copy(::std::views::iota(0) | ::std::views::take(size), ::std::ranges::begin(_independent));
	}

	::std::vector<point_t> _independent{};
};





NGS_LIB_MODULE_END