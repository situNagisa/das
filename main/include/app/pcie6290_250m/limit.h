#pragma once

#include "./defined.h"

NGS_LIB_MODULE_BEGIN

template<::std::ranges::forward_range R >
	requires ::std::ranges::sized_range<R> && ::std::permutable<::std::ranges::iterator_t<R>>
constexpr decltype(auto) range_limit(::std::ranges::input_range auto&& range, R&& limit_range, ::std::ranges::range_difference_t<R> active_size)
	requires ::std::indirectly_copyable<::std::ranges::iterator_t<decltype(range)>, ::std::ranges::iterator_t<R>>
{
	auto limit_size = ::std::ranges::size(limit_range);
	auto input_size = ::std::ranges::size(range);

	if(limit_size - active_size > input_size)
	{
		auto active_range = NGS_PP_PERFECT_FORWARD(limit_range) | ::std::views::take(active_size);
		return ::std::ranges::distance(::std::ranges::begin(active_range), ::std::ranges::copy(NGS_PP_PERFECT_FORWARD(range), ::std::ranges::end(active_range)).out);
	}
	auto shift_size = input_size - (limit_size - active_size);
	auto active_range = ::std::ranges::shift_left(limit_range, shift_size);

	return ::std::ranges::distance(::std::ranges::begin(active_range), ::std::ranges::copy(NGS_PP_PERFECT_FORWARD(range), ::std::ranges::end(active_range)).out);
}

NGS_LIB_MODULE_END