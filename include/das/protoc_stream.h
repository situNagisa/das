#pragma once

#include "./defined.h"

namespace protoc_stream
{
	template<class T, class R>
	concept transform_view = ::std::invocable<T, R> && requires
	{
		requires ::std::ranges::viewable_range<::std::invoke_result_t<T, R>>;
	};

	inline constexpr struct
	{
		template<::std::ranges::viewable_range R>
		NGS_CONFIG_STATIC_CALL_OPERATOR constexpr ::std::ranges::subrange<::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>> operator()(
			R&& range,
			::std::predicate<const R&> auto predicate,
			transform_view<R&&> auto transform_view
			) NGS_CONFIG_STATIC_CALL_OPERATOR_CONST
		{
			if (::std::invoke(predicate, range))
			{
				return operator()(transform_view(NGS_PP_PERFECT_FORWARD(range)), predicate, transform_view);
			}
			return NGS_PP_PERFECT_FORWARD(range);
		}
	}recurse_while{};
}