#pragma once

#include "./defined.h"

#define NGS_LIB_MODULE_NAME enums
#include "NGS/lib/module.h"

NGS_LIB_MODULE_BEGIN

enum class upload_rate : ::std::uint32_t
{
	_250m = 1,
	_125m,
	_83_33m,
	_62_5m,
	_50m,
};

enum class parse_rule : ::std::uint32_t
{
	raw_data = 0,
	iq = 2,
	amplitude_phase = 3,
};

enum class channel_quantity : ::std::uint32_t
{
	_1 = 1,
	_2,
};

NGS_LIB_MODULE_END

#include "NGS/lib/module_end.h"