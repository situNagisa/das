#pragma once

#include "./defined.h"

NGS_LIB_MODULE_BEGIN

template<class T>
decltype(auto) plot_line(::std::string_view title, ::std::span<T> independent_variables, ::std::span<T> dependent_variables)
{
	::ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
	::ImPlot::PlotLine(
		title.data(),
		::std::ranges::data(independent_variables), 
		::std::ranges::data(dependent_variables), 
		::std::min(::std::initializer_list<::std::size_t>{ ::std::ranges::size(independent_variables), ::std::ranges::size(dependent_variables) })
	);
}

NGS_LIB_MODULE_END