#pragma once

#include "./glfw.h"
#include "./render.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct imgui
{
	imgui(glfw& glfw)
		: _glfw(glfw)
	{
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

		::ImGui_ImplGlfw_InitForOpenGL(_glfw.window().get_context(), true);
		::ImGui_ImplOpenGL3_Init("#version 130");

		// ImFileDialog requires you to set the CreateTexture and DeleteTexture
		ifd::FileDialog::Instance().CreateTexture = [](uint8_t* data, int w, int h, char fmt) -> void* {
			GLuint tex;
			namespace gl = ::ngs::external::opengl;

			glGenTextures(1, &tex);
			glBindTexture(GL_TEXTURE_2D, tex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, (fmt == 0) ? GL_BGRA : GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);

			return (void*)tex;
			};
		ifd::FileDialog::Instance().DeleteTexture = [](void* tex) {
			GLuint texID = (GLuint)((uintptr_t)tex);
			glDeleteTextures(1, &texID);
			};
	}
	imgui(glfw& glfw, ::std::string_view font)
		: imgui(glfw)
	{
		::ImGuiIO& io = ImGui::GetIO();

		if (!::std::filesystem::exists(font))
		{
			return;
		}

		// Load Fonts
		// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
		// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
		// - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
		// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
		// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
		// - Read 'docs/FONTS.md' for more instructions and details.
		// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
		// - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
		//io.Fonts->AddFontDefault();
		//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
		//io.Fonts->AddFontFromFileTTF("../misc/fonts/DroidSans.ttf", 16.0f);
		io.Fonts->AddFontFromFileTTF(font.data(), 18.0f);
		//io.Fonts->AddFontFromFileTTF("../misc/fonts/Cousine-Regular.ttf", 15.0f);
		//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
		//IM_ASSERT(font != nullptr);
	}

	~imgui()
	{
		::ImGui_ImplOpenGL3_Shutdown();
		::ImGui_ImplGlfw_Shutdown();
		::ImPlot::DestroyContext();
		::ImGui::DestroyContext();
	}

	[[nodiscard]] auto render() const { return NGS_LIB_MODULE_NAME::render(_glfw.guard(), _glfw.window()); }

	glfw& _glfw;
};

NGS_LIB_MODULE_END