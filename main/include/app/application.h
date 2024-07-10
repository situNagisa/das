#pragma once

#include "./pcie.h"
#include "./edfa_module.h"
#include "./defined.h"

NGS_LIB_BEGIN

struct application
{

	void run()
	{
		while(!_glfw.window().should_close())
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
							_pcie.render_config();
							::ImGui::SeparatorText("save");
							_pcie.render_save();

							::ImGui::EndTabItem();
						}
						if (::ImGui::BeginTabItem("uart"))
						{
							_edfa.render_control();
							_edfa.render_config();
							_edfa.render_message();
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
					_pcie.render_plot();
					::ImGui::End();
				}
			}
			_pcie.update();
			_edfa.update();
		}
	}

	::quick_ui::guard::glfw _glfw{ "das", 1300, 670 };
	::quick_ui::guard::imgui _gui{ _glfw , "./misc/fonts/Roboto-Medium.ttf"};
	pcies::instance _pcie{};
	edfa_module::instance _edfa{};
};

NGS_LIB_END