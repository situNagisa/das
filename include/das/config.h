#pragma once

#include "./defined.h"

NGS_LIB_BEGIN

#if !defined(DAS_CONFIG_FONT_FILE)
#	define DAS_CONFIG_FONT_FILE "../../misc/fonts/Roboto-Medium.ttf"
#endif

#if !defined(DAS_CONFIG_WINDOW_SIZE_WIDTH)
#	define DAS_CONFIG_WINDOW_SIZE_WIDTH 1300
#endif

#if !defined(DAS_CONFIG_WINDOW_SIZE_HEIGHT)
#	define DAS_CONFIG_WINDOW_SIZE_HEIGHT 670
#endif

#if !defined(DAS_CONFIG_BACKGROUND_COLOR)
#	define DAS_CONFIG_BACKGROUND_COLOR { 21.f / 255.f, 22.f / 255.f, 23.f / 255.f, 1.00f }
#endif

struct das_config
{
	constexpr static ::std::string_view font_file = DAS_CONFIG_FONT_FILE;
	constexpr static ::std::pair<::std::size_t, ::std::size_t> window_size = { DAS_CONFIG_WINDOW_SIZE_WIDTH , DAS_CONFIG_WINDOW_SIZE_HEIGHT };
	constexpr static auto imgui_window_flag =
		ImGuiWindowFlags_NoTitleBar
#if !defined(NGS_BUILD_TYPE_IS_DEBUG)
		//	| ImGuiWindowFlags_NoMove
#endif
		| ImGuiWindowFlags_AlwaysAutoResize
		| ImGuiWindowFlags_NoBackground
		;
	constexpr static struct
	{
		float red;
		float green;
		float blue;
		float alpha;
	} background_color = DAS_CONFIG_BACKGROUND_COLOR;
	constexpr static ::std::size_t limit_point = ::std::numeric_limits<short>::max() / 16;
};

NGS_LIB_END