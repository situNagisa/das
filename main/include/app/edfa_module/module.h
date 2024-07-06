#pragma once

#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct module
{
	struct promise_type
	{
		using handle_type = ::std::coroutine_handle<promise_type>;

		static constexpr auto initial_suspend() noexcept { return ::std::suspend_always{}; }
		static constexpr auto final_suspend() noexcept { return ::std::suspend_always{}; }
		static constexpr auto return_void() noexcept {}
		static auto unhandled_exception() { NGS_LOGL(error, "woc? "); } // ±£¥Ê“Ï≥£

		auto get_return_object() noexcept { return handle_type::from_promise(*this); }
		auto yield_value(bool success) noexcept
		{
			read_success = success;
			return ::std::suspend_always{};
		}
		bool read_success = false;
	};

	::std::span<::std::byte> read_all_parameters(::boost::asio::io_context& io,::boost::asio::serial_port& serial_port)
	{
		serial_port.async_write_some()
	}
};

NGS_LIB_MODULE_END