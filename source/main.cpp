﻿#include "./das/das.h"
#include "das/protoc_stream.h"

constexpr auto to_string(::std::ranges::range auto&& range, ::std::string_view formatter)
{
	::std::string result{};
	for (auto&& value : range)
	{
		result += ::std::vformat(formatter, ::std::make_format_args(value));
	}
	return result;
}

inline constexpr auto flag_check = [](::std::ranges::random_access_range auto&& buffer)
	{
		return buffer[0] == 0xed && buffer[1] == 0xfa;
	};

// Main code
int main(int, char**)
{
	using namespace ::std::chrono_literals;
	
	//::das::stream stream{};

	constexpr auto data = ::std::array{ 0xed, 0xfa, 0x03, 0xed, 0xfa, 0x04, 0xff, 0xff,0x00, 0xe9, 0xed, 0xfa, 0x04, 0xff, 0xff,0x00, 0xe9 };
	/*stream.in(data | ::std::views::transform([](auto&& value) { return static_cast<::std::byte>(value); }));
	NGS_LOGL(info, ::std::format("stream out size {}", stream.size()));
	while (!stream.empty())
	{
		auto&& output_range = stream.out();
		NGS_LOGL(info, ::std::format("stream out({}): {}", output_range.data.size(), to_string(output_range.data | ::std::views::transform([](::std::byte value) { return ::std::to_integer<::std::uint8_t>(value); }), "{:02x} ")));
	}*/

	auto drop_size = 0;
	while (!protoc_stream::check<decltype(flag_check)>(data | ::std::views::drop(drop_size)))
	{
		drop_size++;
	}
	auto result = data | ::std::views::drop(drop_size);
	NGS_LOGL(info, ::std::format("stream out({}): {}", result.size(), to_string(result, "{:02x} ")));



	//return 0;
	
	::das::application app{};

	app.run();

	return 0;
}