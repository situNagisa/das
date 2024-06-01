#pragma once

#include "../c.h"
#include "../atomic.h"
#include "./io.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct open
{
	open()
	{
		if(c_api::pcie6920_open())
		{
			using namespace ::std::chrono_literals;
			c_api::pcie6920_close();
			::std::this_thread::sleep_for(1s);
			NGS_ASSERT_VERIFY(!c_api::pcie6920_open(), "the pcie6920 open fail! pcie may be open somewhere else, please close it and try again");
		}
	}
	~open()
	{
		c_api::pcie6920_close();
	}

	io io() const { return {}; }
};

NGS_LIB_MODULE_END