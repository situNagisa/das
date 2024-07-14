#pragma once

#include "./type.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct saver
{
	auto _get_directory() const
	{
		return ::std::filesystem::path(_root.data()) / _type.data();
	}
	auto _get_file_name(::std::string_view name) const
	{
		auto time = clock_t::to_time_t(clock_t::now());
		::std::stringstream ss{};
		ss << ::std::put_time(::std::localtime(&time), "%F-%H-%M-%S");
		return ::std::format("{}_{}_{}.bin", _type.data(), ss.str(), name);
	}

	void render(bool enable_save, ::std::size_t collect_times = 0, ::std::size_t record_times = 0)
	{
		if (::ImGui::Button("open root", { 100 , 0 }))
		{
			::ifd::FileDialog::Instance().Open("root_directory", "root directory", "", false, _root.data());
		}
		if (::ifd::FileDialog::Instance().IsDone("root_directory")) {
			if (::ifd::FileDialog::Instance().HasResult()) {
				::std::ranges::copy(::ifd::FileDialog::Instance().GetResult().string(), _root.begin());
			}
			::ifd::FileDialog::Instance().Close();
		}
		if (::std::strlen(_root.data()) && ::std::strlen(_type.data()))
		{
			::ImGui::SameLine();
			if (enable_save)
			{
				_is_change.save = ::ImGui::Button("save", { 100 , 0 });
			}
			else
			{
				_is_change.save = false;
				::ImGui::Text("collect/record :[ %ld, %ld ]", collect_times, record_times);
			}
		}

		_is_change.root = ::ImGui::InputText("root dir", _root.data(), _root.size());
		_is_change.type = ::ImGui::InputText("type", _type.data(), _type.size());
		_is_change.collect_times = ::ngs::external::imgui::components::drag("collect times", _info.collect_times);
		_is_change.record_times = ::ngs::external::imgui::components::drag("record times", _info.record_times);

		_is_change.save_channel0 = ::ImGui::Checkbox("save channel 0", &_info.save_channel0);
		_is_change.save_channel1 = ::ImGui::Checkbox("save channel 1", &_info.save_channel1);
	}

	decltype(auto) get_channel0_file() const { return _info.save_channel0 ? _get_directory() / _get_file_name("0") : ::std::filesystem::path{}; }
	decltype(auto) get_channel1_file() const { return _info.save_channel1 ? _get_directory() / _get_file_name("1") : ::std::filesystem::path{}; }

	auto&& info(this auto&& self) { return self._info; }
	auto&& is_change() const { return _is_change; }

	::std::array<char, 128> _root{};
	::std::array<char, 32> _type{ "unknown" };
	struct
	{
		::std::size_t collect_times = 5;
		::std::size_t record_times = 1;
		bool save_channel0 = true;
		bool save_channel1 = true;
	}_info{};
	struct
	{
		bool save : 1;
		bool root : 1;
		bool type : 1;
		bool collect_times : 1;
		bool record_times : 1;
		bool save_channel0 : 1;
		bool save_channel1 : 1;
	}_is_change{};
};

NGS_LIB_MODULE_END