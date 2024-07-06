#pragma once

#include "./pcie.h"
#include "./defined.h"

NGS_LIB_BEGIN

struct application
{

	void run()
	{
		while(!_glfw.window().is_should_close())
		{
			{
				auto guard = _gui.render();
				constexpr auto window_flag = 
					ImGuiWindowFlags_NoTitleBar
					| ImGuiWindowFlags_NoMove
					| ImGuiWindowFlags_NoResize
				//	| ImGuiWindowFlags_AlwaysAutoResize
					| ImGuiWindowFlags_NoBackground;
				::ImGui::SetNextWindowPos({ 8,23 }, ImGuiCond_FirstUseEver);
				::ImGui::SetNextWindowSize({ 333,635 }, ImGuiCond_FirstUseEver);
				if (::ImGui::Begin("tool", nullptr, window_flag))
				{
					if (::ImGui::BeginTabBar("control", ImGuiTabBarFlags_None))
					{
						if (::ImGui::BeginTabItem("pcie"))
						{
							::ImGui::SeparatorText("config");
							_instance.render_config();
							::ImGui::SeparatorText("save");
							_instance.render_save();

							::ImGui::EndTabItem();
						}
						if (::ImGui::BeginTabItem("uart"))
						{

							::ImGui::EndTabItem();
						}

						::ImGui::EndTabBar();
					}
					::ImGui::End();
				}
				::ImGui::SetNextWindowPos({ 353,16 }, ImGuiCond_FirstUseEver);
				::ImGui::SetNextWindowSize({ 916,639 }, ImGuiCond_FirstUseEver);
				if (::ImGui::Begin("channel data", nullptr, window_flag))
				{
					_instance.render_plot();
					::ImGui::End();
				}
			}
			_instance.update();
		}
	}

	::quick_ui::guard::glfw _glfw{ "das", 1300, 670 };
	::quick_ui::guard::imgui _gui{ _glfw };
	pcies::instance _instance{};
};

NGS_LIB_END