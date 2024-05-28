#define NGS_USE_HPP

#include "NGS/basic/basic.h"
#include "NGS/external/glfw.h"

#include "imgui.h"
#include "backend/imgui_impl_glfw.h"
#include "backend/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include "implot.h"

int main()
{

	namespace glfw = ::ngs::external::glfw;

	::glfwSetErrorCallback([](int error, const char* description)
		{
			NGS_LOGFL(error, "glfw error %d: %s", error, description);
		});
	glfw::GLFW::instance().init_opengl(3, 0);

	auto window = glfw::window("pcie 250 sps data reader", 1600, 900);
	::glfwSwapInterval(0); // Enable vsync

	IMGUI_CHECKVERSION();
	::ImGui::CreateContext();
	::ImPlot::CreateContext();

	::ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	::ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	::ImGui_ImplGlfw_InitForOpenGL(window.get_context(), true);
	::ImGui_ImplOpenGL3_Init("#version 130");

	io.Fonts->AddFontFromFileTTF("C:/workspace/project/xmake/das/misc/fonts/Roboto-Medium.ttf", 16.0f);

	while (!window.is_should_close())
	{
		glfw::GLFW::instance().poll_events();

		// Start the Dear ImGui frame
		::ImGui_ImplOpenGL3_NewFrame();
		::ImGui_ImplGlfw_NewFrame();
		::ImGui::NewFrame();

		ImPlot::ShowDemoWindow();

		::ImGui::Render();

		auto [display_w, display_h] = window.get_framebuffer_size();
		::glViewport(0, 0, display_w, display_h);
		::glClearColor(0.1, 0.1, 0.1, 1);
		::glClear(GL_COLOR_BUFFER_BIT);

		::ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (auto& io = ImGui::GetIO(); io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			::ImGui::UpdatePlatformWindows();
			::ImGui::RenderPlatformWindowsDefault();
			window.active();
		}

		window.swap_buffer();
	}

	::ImGui_ImplOpenGL3_Shutdown();
	::ImGui_ImplGlfw_Shutdown();
	::ImPlot::DestroyContext();
	::ImGui::DestroyContext();

	return 0;
}