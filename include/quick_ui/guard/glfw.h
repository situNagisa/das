#pragma once

#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct glfw
{
	auto _create_window(::std::string_view title, ::std::size_t width, ::std::size_t height)
	{
		namespace hints = ::ngs::external::glfw::hints;
		_glfw.set_hint<hints::resizable>(false);
		_glfw.set_hint<hints::maximized>(false);
		return _glfw.create_window(title, width, height);
	}

	glfw(::std::string_view title, ::std::size_t width, ::std::size_t height)
		: _window(_create_window(title, width, height))
	{
		_glfw.swap_interval(0);

		::glfwSetErrorCallback([](int error, const char* description)
			{
				NGS_LOGFL(error, "glfw error %d: %s", error, description);
			});

		bool success = ::gladLoadGLLoader(reinterpret_cast<GLADloadproc>(::glfwGetProcAddress));
		NGS_ASSERT(success);
	}

	auto&& guard() { return _glfw; }
	auto&& guard() const { return _glfw; }

	auto&& window() { return _window; }
	const auto& window() const { return _window; }

	::ngs::external::glfw::guards::opengl_guard _glfw{ 3,0 };
	::ngs::external::glfw::window _window;
};

NGS_LIB_MODULE_END