add_includedirs("./include")
add_includedirs("../imgui/include")

target("pcie6920")
	set_kind("object")
	set_group("pcie")

	add_files("./source/*.cpp")
target_end()