#pragma once

#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct render
{
	render(::ngs::external::glfw::guards::opengl_guard& guard, ::ngs::external::glfw::window& window)
		: _window(window)
	{
		guard.poll_events();

		// Start the Dear ImGui frame
		::ImGui_ImplOpenGL3_NewFrame();
		::ImGui_ImplGlfw_NewFrame();
		::ImGui::NewFrame();
	}

	~render()
	{
		// Rendering
		::ImGui::Render();

		auto [display_w, display_h] = _window.framebuffer_size();
		::glViewport(0, 0, display_w, display_h);
		::glClearColor(0.1f, 0.1f, 0.1f, 1);
		::glClear(GL_COLOR_BUFFER_BIT);

		::ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (auto& io = ImGui::GetIO(); io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			::ImGui::UpdatePlatformWindows();
			::ImGui::RenderPlatformWindowsDefault();
			_window.active();
		}

		_window.swap_buffer();
	}

	::ngs::external::glfw::window& _window;
};

NGS_LIB_MODULE_END