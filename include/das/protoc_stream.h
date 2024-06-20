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

	inline constexpr struct
	{
		enum class choose_type
		{
			none,
			member,
			convertible,
			adl,
		};

		template<class T>
		consteval static auto choose()
		{
			if constexpr (requires(T t) { NGS_PP_PERFECT_FORWARD(t).has_value(); })
			{
				return choose_type::member;
			}
			else if constexpr (::std::convertible_to<T, bool>)
			{
				return choose_type::convertible;
			}
			else if constexpr (requires(T t) { has_value(NGS_PP_PERFECT_FORWARD(t)); })
			{
				return choose_type::adl;
			}
			else
			{
				return choose_type::none;
			}
		}

		NGS_CONFIG_STATIC_CALL_OPERATOR constexpr decltype(auto) operator()(auto&& t) NGS_CONFIG_STATIC_CALL_OPERATOR_CONST
			requires (choose<decltype(t)>() != choose_type::none)
		{
			constexpr auto chose = choose<decltype(t)>();
			if constexpr(chose == choose_type::member)
			{
				return NGS_PP_PERFECT_FORWARD(t).has_value();
			}
			else if constexpr (chose == choose_type::convertible)
			{
				return static_cast<bool>(NGS_PP_PERFECT_FORWARD(t));
			}
			else if constexpr (chose == choose_type::adl)
			{
				return has_value(NGS_PP_PERFECT_FORWARD(t));
			}
		}
	}has_value{};

	inline constexpr struct
	{
		enum class choose_type
		{
			none,
			member,
			adl,
		};

		template<class T>
		consteval static auto choose()
		{
			if constexpr (requires(T t) { NGS_PP_PERFECT_FORWARD(t).value(); })
			{
				return choose_type::member;
			}
			else if constexpr (requires(T t) { value(NGS_PP_PERFECT_FORWARD(t)); })
			{
				return choose_type::adl;
			}
			else
			{
				return choose_type::none;
			}
		}

		NGS_CONFIG_STATIC_CALL_OPERATOR constexpr decltype(auto) operator()(auto&& t) NGS_CONFIG_STATIC_CALL_OPERATOR_CONST
			requires (choose<decltype(t)>() != choose_type::none)
		{
			constexpr auto chose = choose<decltype(t)>();
			if constexpr (chose == choose_type::member)
			{
				return NGS_PP_PERFECT_FORWARD(t).value();
			}
			else if constexpr (chose == choose_type::adl)
			{
				return value(NGS_PP_PERFECT_FORWARD(t));
			}
		}
	}value{};

	template<class T>
	concept field_parse_result = requires(T t)
	{
		{ has_value(NGS_PP_PERFECT_FORWARD(t)) } -> ::std::convertible_to<bool>;
			requires ::std::copyable<::ngs::type_traits::naked_t<decltype(value(NGS_PP_PERFECT_FORWARD(t)))>>;
			requires ::std::copy_constructible<::ngs::type_traits::naked_t<decltype(value(NGS_PP_PERFECT_FORWARD(t)))>>;
	};

	template<class T>
	struct _parse_functor
	{
		enum class choose_type
		{
			none,
			member,
			callable,
			adl,
		};

		template<class R>
		consteval static auto choose()
		{
			if constexpr (requires(R r) { T::parse(NGS_PP_PERFECT_FORWARD(r)); })
			{
				return choose_type::member;
			}
			else if constexpr (::std::default_initializable<T> && requires(T t, R r) { t(NGS_PP_PERFECT_FORWARD(r)); })
			{
				return choose_type::callable;
			}
			else if constexpr (requires(R r) { parse<T>(NGS_PP_PERFECT_FORWARD(r)); })
			{
				return choose_type::adl;
			}
			else
			{
				return choose_type::none;
			}
		}

		template<::std::ranges::input_range R>
		NGS_CONFIG_STATIC_CALL_OPERATOR constexpr decltype(auto) operator()(R&& r) NGS_CONFIG_STATIC_CALL_OPERATOR_CONST
			requires (choose<R>() != choose_type::none)
		{
			constexpr auto chose = choose<R>();
			if constexpr (chose == choose_type::member)
			{
				return T::parse(NGS_PP_PERFECT_FORWARD(r));
			}
			else if constexpr (chose == choose_type::callable)
			{
				return T{}(NGS_PP_PERFECT_FORWARD(r));
			}
			else if constexpr (chose == choose_type::adl)
			{
				return parse<T>(NGS_PP_PERFECT_FORWARD(r));
			}
		}
	};

	template<class T>
	inline constexpr _parse_functor<T> parse{};

	template<class T, class R>
	concept field = ::std::ranges::range<R> && requires(R r)
	{
		{ parse<T>(NGS_PP_PERFECT_FORWARD(r)) } -> ::std::convertible_to<bool>;
	};

	template<::std::ranges::input_range R, field<R>... Fs>
	using field_sequence = ::std::tuple<Fs...>;

	template<class Fields, ::std::size_t I>
	constexpr auto recurse_check(auto&& range)
	{
		if (!parse<::std::tuple_element_t<I, Fields>>(range))
			return false;
		if constexpr (I == ::std::tuple_size_v<Fields> -1)
		{
			return true;
		}
		else
		{

			return protoc_stream::recurse_check<Fields, I + 1>(NGS_PP_PERFECT_FORWARD(range));
		}
	}



	template<class... Fields>
	struct _checker
	{
		NGS_CONFIG_STATIC_CALL_OPERATOR constexpr decltype(auto) operator()(::std::ranges::input_range auto&& range) NGS_CONFIG_STATIC_CALL_OPERATOR_CONST
			requires (field<Fields, decltype(range)> && ...)
		{
			return protoc_stream::recurse_check<::std::tuple<Fields...>, 0>(NGS_PP_PERFECT_FORWARD(range));
		}
	};

	template<class... Fields>
	inline constexpr _checker<Fields...> check{};
	
}