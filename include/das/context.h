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
	::laser::pcie6920::guard::open pcie{};
	::std::size_t time;
	::boost::asio::io_service io{};
	::std::unique_ptr<::boost::asio::serial_port> serial_port = nullptr;

	context(::std::string_view plot_title)
		: plot(plot_title)
	{}
};

NGS_LIB_END