#pragma once

#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct glfw
{
	static auto _create_window(::std::string_view title, ::std::size_t width, ::std::size_t height)
	{
		namespace ngs_glfw = ::ngs::external::glfw;

		::glfwSetErrorCallback([](int error, const char* description)
			{
				NGS_LOGFL(error, "glfw error %d: %s", error, description);
			});
		ngs_glfw::GLFW::instance().init_opengl(3, 0);

		return ngs_glfw::window(title, width, height);
	}

	glfw(::std::string_view title, ::std::size_t width, ::std::size_t height)
		: _window(_create_window(title, width, height))
	{
		::glfwSwapInterval(0); // Enable vsync

		bool success = ::gladLoadGLLoader(reinterpret_cast<GLADloadproc>(::glfwGetProcAddress));
		NGS_ASSERT(success);
	}

	auto&& window() { return _window; }
	auto&& window() const { return _window; }

	::ngs::external::glfw::window _window;
};

NGS_LIB_MODULE_END