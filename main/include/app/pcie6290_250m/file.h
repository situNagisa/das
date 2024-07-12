#pragma once

#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct file
{



	::std::filesystem::path _root;
	::std::filesystem::path _type;
	::std::filesystem::path _name;
};

NGS_LIB_MODULE_END