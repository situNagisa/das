#pragma once

#include "./type.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct saver
{
	constexpr static auto channel_size = 2;

	auto _get_directory() const
	{
		return ::std::filesystem::path(_root.data()) / _type.data();
	}
	auto _get_file_name(::std::size_t index) const
	{
		auto time = clock_t::to_time_t(clock_t::now());
		::std::stringstream ss{};
		ss << ::std::put_time(::std::localtime(&time), "%F-%H-%M-%S");
		return ::std::format("{}_{}_{}.bin", _type.data(), ss.str(), index);
	}

	void render(bool enable_save, ::std::size_t record_times)
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
				::ImGui::Text("remaining record times: %ld", record_times);
			}
		}

		_is_change.root = ::ImGui::InputText("root dir", _root.data(), _root.size());
		_is_change.type = ::ImGui::InputText("type", _type.data(), _type.size());
		_is_change.collect_times = ::ngs::external::imgui::components::drag("collect times", _info.collect_times, 1, 1, 4096);
		_is_change.record_times = ::ngs::external::imgui::components::drag("record times", _info.record_times, 1, 1, 4096);

		for (auto i = 0ul; i < channel_size; ++i)
		{
			_is_change.save_channel[i] = ::ImGui::Checkbox(::std::format("save channel {}", i).c_str(), &_info.save_channel[i]);
		}
	}

	decltype(auto) open_channel_output_file(::std::size_t index)
	{
		auto file = _get_directory() / _get_file_name(index);
		if (!file.empty() && !::std::filesystem::exists(file.parent_path()))
		{
			::std::filesystem::create_directories(file.parent_path());
		}
		_files[index].open(file, ::std::ios::binary | ::std::ios::out);
		NGS_EXPECT(_files[index].is_open());
		return _files[index].is_open();
	}
	decltype(auto) channel_output_is_open(::std::size_t index) const
	{
		return _files[index].is_open();
	}
	decltype(auto) write_channel_output_file(::std::size_t index, const void* data, ::std::size_t size)
	{
		_files[index].write(reinterpret_cast<const char*>(data), size);
	}
	decltype(auto) close_channel_output_file(::std::size_t index)
	{
		_files[index].close();
	}

	auto&& info(this auto&& self) { return self._info; }
	auto&& is_change() const { return _is_change; }

	::std::array<::std::ofstream, channel_size> _files{};

	::std::array<char, 128> _root{};
	::std::array<char, 32> _type{ "unknown" };
	struct
	{
		::std::size_t collect_times = 5;
		::std::size_t record_times = 1;
		::std::array<bool, channel_size> save_channel{true, true};
	}_info{};
	struct
	{
		bool save : 1;
		bool root : 1;
		bool type : 1;
		bool collect_times : 1;
		bool record_times : 1;
		::std::array<bool, channel_size> save_channel{};
	}_is_change{};
};

NGS_LIB_MODULE_END