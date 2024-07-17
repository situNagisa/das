#pragma once

#include "./type.h"
#include "./buffer.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct reader
{
	NGS_PP_INJECT_BEGIN(reader);
public:
	using packet_type = ::pcie6920_250m::atomic::packet<::pcie6920_250m::enums::parse_rule::raw_data, ::pcie6920_250m::enums::channel_quantity::_1>;
	using buffer_type = buffer<packet_type, point_t>;

	struct read_pcie_promise;
	struct read_pcie_task : ::std::coroutine_handle<read_pcie_promise> { using promise_type = read_pcie_promise; };
	struct read_pcie_promise
	{
		NGS_PP_INJECT_BEGIN(read_pcie_promise);
	public:
		static decltype(auto) initial_suspend() noexcept { return ::std::suspend_always{}; }
		static decltype(auto) final_suspend() noexcept { return ::std::suspend_always{}; }
		static void return_void() noexcept {}
		read_pcie_task get_return_object() noexcept { return { read_pcie_task::from_promise(*this) }; }
		static void unhandled_exception() { NGS_LOGL(error, "woc? "); } // ±£¥Ê“Ï≥£

		decltype(auto) yield_value(this self_type& self, bool success) { self.success = success; return ::std::suspend_always{};}
		bool success = false;
	};
	bool _try_read(::pcie6920_250m::guard::io& io)
	{
		if (io.buffer_packet_size() < _buffer.size())
			return false;
		auto buffer_size = io.buffer_packet_size();
		auto read_size = io.read(_buffer.packet_buffer());

		//NGS_LOGFL(debug, "buffer size: %ld, read size: %ld", buffer_size, read_size);

		_buffer.transfer();
		return true;
	}
	read_pcie_task _read(::pcie6920_250m::guard::instance& instance, ::std::predicate<> auto&& predicate)
	{
		using namespace ::std::chrono_literals;
		auto io = instance.open_io();
		while (!predicate())
		{
			co_yield _try_read(io);
		}
	}
	decltype(auto) start(::pcie6920_250m::guard::instance& instance)
	{
		return start_while(instance, [] { return false; });
	}
	void start_while(::pcie6920_250m::guard::instance& instance, ::std::predicate<> auto&& predicate)
	{
		NGS_ASSERT(done());
		_handle = self_type::_read(instance, NGS_PP_PERFECT_FORWARD(predicate));
	}
	bool done() const
	{
		return !_handle || _handle.done();
	}
	bool read()
	{
		NGS_ASSERT(!done());
		_handle.resume();
		return _handle.promise().success;
	}
	void stop()
	{
		_handle.destroy();
		_handle = {};
	}

	auto&& buffer(this auto&& self) { return self._buffer; }

	buffer_type _buffer{};
	read_pcie_task _handle{};
};

NGS_LIB_MODULE_END