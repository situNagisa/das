add_includedirs("./include")
add_includedirs("../imgui/include")

target("implot")
	set_kind("object")
	set_group("ui")

	add_deps("imgui")

	add_files("./include/*.cpp")
target_end()