#pragma once

#include "./runtime.h"
#include "./ui.h"
#include "./defined.h"

NGS_LIB_BEGIN

struct context
{
	ui::gui gui{};
	ui::plot plot;
	runtime::data runtime{};
	::std::size_t time;
	::boost::asio::io_service io{};
	::boost::asio::ip::udp::socket udp_socket;

	context(::std::string_view plot_title)
		: plot(plot_title)
		, udp_socket(io)
	{
		try
		{
			udp_socket.open(::boost::asio::ip::udp::v4());
			::boost::asio::ip::udp::resolver resolver(io);
			udp_socket.bind(*resolver.resolve({ ::boost::asio::ip::udp::v4(), "127.0.0.1", "6665" }));

			auto endpoint = udp_socket.local_endpoint();
			NGS_LOGL(info, ::std::format("udp socket open {}: {}", endpoint.address().to_string(), endpoint.port()));
		}
		catch(const ::boost::system::system_error& error)
		{
			NGS_LOGL(error, error.what());
		}
	}
};

NGS_LIB_END