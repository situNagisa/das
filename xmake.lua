set_toolchains("gcc-13")
set_languages("cxxlatest")
set_policy("build.warning", true)
set_optimize("none")
--set_optimize("fast")
set_warnings("all")

add_includedirs("./include")
add_includedirs("./extern/imgui/include")
add_includedirs("./extern/imgui/include/backend")
add_includedirs("./extern/implot/include")
add_includedirs("./extern/pcie6920/include")

option("nagisa_root")
	set_default(os.getenv("NGS_ROOT"))
	set_showmenu(true)
option_end()

if "$(nagisa_root)" then 
	add_includedirs("$(nagisa_root)/include")
end

add_requires("vcpkg::glfw3","opengl")
if is_plat("windows") then
	add_requires("gdi32","shell32","libcmt")
end

if is_mode("release") then
    set_symbols("hidden")
    set_optimize("fastest")
    set_strip("all")
	add_defines("NDEBUG")
end

if is_mode("debug") then
    set_symbols("debug")
    set_optimize("none")
	--add_defines("_DEBUG")
	--add_cxflags("/MTd", {force = true})
end

add_cxflags("cl::/Zc:preprocessor", {force = true})

includes("extern/imgui","extern/implot","extern/pcie6920")


target("das")
	set_kind("binary")

	add_deps("imgui","implot","pcie6920")

	add_packages("vcpkg::glfw3","opengl")
	if is_plat("windows") then
		add_packages("gdi32","shell32","libcmt")
	end
	add_files("./source/*.cpp")
target_end()