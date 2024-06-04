﻿#include "./das/das.h"

constexpr auto to_string(::std::ranges::range auto&& range, ::std::string_view formatter)
{
	::std::string result{};
	for (auto&& value : range | ::std::views::transform([formatter](auto&& value) { return ::std::vformat(formatter, ::std::make_format_args(NGS_PP_PERFECT_FORWARD(value))); }))
	{
		result += value;
	}
	return result;
}

void mic_test()
{
	namespace asio = ::boost::asio;

	try
	{
		asio::io_service io{};
		asio::serial_port sp(io, "COM7");

		sp.set_option(asio::serial_port::baud_rate(9600));
		sp.set_option(asio::serial_port::character_size(8)); //数据位为8
		sp.set_option(asio::serial_port::parity(asio::serial_port::parity::none)); //无奇偶校验
		sp.set_option(asio::serial_port::stop_bits(asio::serial_port::stop_bits::one)); //一位停止位
		sp.set_option(asio::serial_port::flow_control(asio::serial_port::flow_control::none)); //无流控制

		{
			::std::array<::std::uint8_t, 0x100> send_buffer{};

			::laser::mic::algorithm::send(::laser::mic::protocols::command::read_all_parameters, send_buffer.begin());

			auto length = sp.write_some(asio::buffer(send_buffer));
			NGS_LOGL(info, ::std::format("send length:{}, {}", length, ::to_string(send_buffer, "{:02x} ")));
		}
		
		::std::array<::std::uint8_t, 64> receive_buffer{};

		sp.async_read_some(asio::buffer(receive_buffer), [&receive_buffer](const ::boost::system::error_code& error, ::std::size_t bytes_transferred)
			{
				if(error)
				{
					NGS_LOGL(error, error.message());
					return;
				}
				NGS_LOGL(info, ::std::format("receive length:{}, {}", bytes_transferred, ::to_string(receive_buffer | ::std::views::take(bytes_transferred), "{:02x} ")));
				NGS_LOGL(info, ::std::format("data {}", ::to_string(::laser::mic::algorithm::receive_check(::laser::mic::protocols::command::read_all_parameters, receive_buffer | ::std::views::take(bytes_transferred)), "{:02x} ")));
			});

		io.run();
	}
	catch(::std::exception& e)
	{
		NGS_LOGL(error, e.what());
	}

}

// Main code
int main(int, char**)
{
	using namespace ::std::chrono_literals;

	::das::application app{};

	mic_test();
	app.run();

	return 0;
}