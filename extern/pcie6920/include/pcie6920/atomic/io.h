#pragma once

#include "../enum.h"
#include "./packet.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

template<enums::parse_rule ParseRule, enums::channel_quantity Quantity>
::std::size_t read()

NGS_LIB_MODULE_END