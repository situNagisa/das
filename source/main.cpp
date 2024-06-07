#include "./das/das.h"

constexpr auto to_string(::std::ranges::range auto&& range, ::std::string_view formatter)
{
	::std::string result{};
	for (auto&& value : range | ::std::views::transform([formatter](auto&& value) { return ::std::vformat(formatter, ::std::make_format_args(NGS_PP_PERFECT_FORWARD(value))); }))
	{
		result += value;
	}
	return result;
}

// Main code
int main(int, char**)
{
	using namespace ::std::chrono_literals;

	::das::application app{};

	app.run();

	return 0;
}