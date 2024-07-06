#pragma once

#include "../enum.h"
#include "./point.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN


template<enums::parse_rule ParseRule, enums::channel_quantity Quantity>
using unit = ::std::array<point<ParseRule>, (ParseRule == enums::parse_rule::raw_data) ? 2 : static_cast<::std::size_t>(Quantity)>;

NGS_LIB_MODULE_END