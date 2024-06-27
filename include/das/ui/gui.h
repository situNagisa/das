#pragma once

#include "../config.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct gui
{
	gui()
	{
		namespace glfw = ::ngs::external::glfw;

		::glfwSetErrorCallback([](int error, const char* description)
			{
				NGS_LOGFL(error, "glfw error %d: %s", error, description);
			});
		glfw::GLFW::instance().init_opengl(3, 0);

		glfw::window& window = *(_window = ::std::make_unique<glfw::window>("pcie 250 sps data reader", das_config::window_size.first, das_config::window_size.second));
		::glfwSwapInterval(0); // Enable vsync

		bool success = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
		NGS_ASSERT(success);

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
		io.Fonts->AddFontFromFileTTF(das_config::font_file.data(), 18.0f);
		//io.Fonts->AddFontFromFileTTF("../misc/fonts/Cousine-Regular.ttf", 15.0f);
		//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
		//IM_ASSERT(font != nullptr);

		// ImFileDialog requires you to set the CreateTexture and DeleteTexture
		ifd::FileDialog::Instance().CreateTexture = [](uint8_t* data, int w, int h, char fmt) -> void* {
			GLuint tex;

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

	~gui()
	{
		::ImGui_ImplOpenGL3_Shutdown();
		::ImGui_ImplGlfw_Shutdown();
		::ImPlot::DestroyContext();
		::ImGui::DestroyContext();
	}

	auto&& window() { return *_window; }
	auto&& window() const { return *_window; }

	struct _render_guard
	{
		_render_guard(::ngs::external::glfw::window& window) : _window(window)
		{
			namespace glfw = ::ngs::external::glfw;

			glfw::GLFW::instance().poll_events();

			// Start the Dear ImGui frame
			::ImGui_ImplOpenGL3_NewFrame();
			::ImGui_ImplGlfw_NewFrame();
			::ImGui::NewFrame();
		}

		~_render_guard()
		{
			// Rendering
			::ImGui::Render();

			auto [display_w, display_h] = _window.get_framebuffer_size();
			::glViewport(0, 0, display_w, display_h);
			::glClearColor(das_config::background_color.red, das_config::background_color.green, das_config::background_color.blue, das_config::background_color.alpha);
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

	auto render_guard() const { return _render_guard(*_window); }

	::std::unique_ptr<::ngs::external::glfw::window> _window;
};

NGS_LIB_MODULE_END