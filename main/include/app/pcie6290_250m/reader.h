#pragma once

#include "./type.h"
#include "./buffer.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct reader
{
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

		decltype(auto) yield_value(this self_type& self, ::std::size_t current_frame) { self.current_frame = current_frame; return ::std::suspend_always{};}
		::std::size_t current_frame = 0;
	};
	bool _try_read(::pcie6920_250m::guard::io& io)
	{
		if (io.buffer_packet_size() < _buffer.size())
			return false;
		auto buffer_size = io.buffer_packet_size();
		auto read_size = io.read(_buffer.packet_buffer());

		NGS_LOGFL(debug, "buffer size: %ld, read size: %ld", buffer_size, read_size);

		_buffer.transfer();
		return true;
	}
	read_pcie_task _read(::pcie6920_250m::guard::instance& instance, ::std::filesystem::path channel0, ::std::filesystem::path channel1, ::std::size_t hold_frame)
	{
		using namespace ::std::chrono_literals;

		if (!channel0.empty() && !::std::filesystem::exists(channel0.parent_path()))
		{
			::std::filesystem::create_directories(channel0.parent_path());
		}
		if (!channel1.empty() && !::std::filesystem::exists(channel1.parent_path()))
		{
			::std::filesystem::create_directories(channel1.parent_path());
		}

		::std::ofstream file0(channel0, ::std::ios::binary | ::std::ios::out);
		::std::ofstream file1(channel1, ::std::ios::binary | ::std::ios::out);
		auto io = instance.open_io();
		while (hold_frame--)
		{
			while (!_try_read(io))
			{
				co_yield hold_frame;
			}
			if (file0)
			{
				auto bytes_channel0 = ::std::as_bytes(_buffer.channel<0>());
				file0.write(reinterpret_cast<const char*>(::std::ranges::data(bytes_channel0)), ::std::ranges::size(bytes_channel0));
			}
			if (file1)
			{
				auto bytes_channel1 = ::std::as_bytes(_buffer.channel<1>());
				file1.write(reinterpret_cast<const char*>(::std::ranges::data(bytes_channel1)), ::std::ranges::size(bytes_channel1));
			}
			co_yield hold_frame;
		}
	}
	void start(::pcie6920_250m::guard::instance& instance, const ::std::filesystem::path& channel0, const ::std::filesystem::path& channel1, ::std::size_t hold_frame)
	{
		NGS_ASSERT(done());
		_handle = _read(instance, channel0, channel1, hold_frame);
	}
	bool done() const
	{
		return !_handle || _handle.done();
	}
	void read()
	{
		NGS_ASSERT(!done());
		_handle.resume();
	}
	void stop()
	{
		_handle.destroy();
		_handle = {};
	}
	decltype(auto) current_frame() const { return done() ? 0 : _handle.promise().current_frame; }

	auto&& buffer(this auto&& self) { return self._buffer; }

	buffer_type _buffer{};
	read_pcie_task _handle{};
};

NGS_LIB_MODULE_END