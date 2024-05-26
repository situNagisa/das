set_toolchains("gcc-13")
set_languages("cxxlatest")
set_policy("build.warning", true)
set_optimize("none")
--set_optimize("fast")
set_warnings("all")

--include

add_includedirs("./include")
add_includedirs("./extern/imgui/include")
add_includedirs("./extern/imgui/include/backend")
add_includedirs("./extern/implot/include")
add_includedirs("./extern/pcie6920/include")

option("nagisa_root")
	set_default(os.getenv("NGS_ROOT"))
	set_showmenu(true)
option_end()

option("visual_pcie")
	set_default(false)
	set_showmenu(true)
option_end()

--depend

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

includes("extern/imgui","extern/implot")

if "$(visual_pcie)" then
	includes("extern/pcie6920")
end


target("das")
	set_kind("binary")

	add_deps("imgui","implot")

	if "$(visual_pcie)" then
		add_deps("pcie6920")
	else
		add_linkdirs("./lib")
		if is_arch("x64") then
			add_links("pcie6920_250m_64bits_api")
		end
		if is_arch("x86") then
			add_links("pcie6920_250m_32bits_api")
		end
	end

	add_packages("vcpkg::glfw3","opengl")
	if is_plat("windows") then
		add_packages("gdi32","shell32","libcmt")
	end
	add_files("./source/*.cpp")
target_end()