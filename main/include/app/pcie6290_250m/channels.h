#pragma once

#include "./defined.h"

NGS_LIB_MODULE_BEGIN

template<class T, ::std::size_t N>
struct channels
{
	NGS_PP_INJECT_BEGIN(channels);
public:
	using value_type = T;
	constexpr static auto channel_count() { return N; }

	constexpr auto size() const { return ::std::ranges::size(_buffer) / self_type::channel_count(); }
	constexpr void resize(::std::size_t new_size)
	{
		_buffer.resize(new_size * self_type::channel_count());
	}

	template<::std::size_t Channel>
		requires (Channel < self_type::channel_count())
	constexpr auto channel(this auto&& self)
	{
		auto chunks = (self._buffer | ::std::views::chunk(self.size()));
		return ::std::span(*::std::ranges::next(::std::ranges::begin(chunks), Channel));
	}

	constexpr void shrink_to_fit()
	{
		_buffer.shrink_to_fit();
	}

	::std::vector<value_type> _buffer{};
};

NGS_LIB_MODULE_END