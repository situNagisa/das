add_includedirs("./include")
add_includedirs("./include/backend")

add_requires("vcpkg::glfw3")

target("imgui")
	set_kind("object")
	set_group("ui")

	add_packages("vcpkg::glfw3")

	add_files("./src/*.cpp")
	add_files("./src/backend/imgui_impl_opengl3.cpp","./src/backend/imgui_impl_glfw.cpp")
target_end()