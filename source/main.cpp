#include "./das/das.h"

// Main code
int main(int, char**)
{
	::das::ui::gui gui{};
	::das::ui::plot plot{ "shaded plots"};
	plot.push_graph({
			.width = 900,
			.height = 300,
			.lines = {
				{
					"channel 0",
					::das::ui::line{}
				},
				{
					"channel 1",
					::das::ui::line{}
				}
			}
		});
	plot.push_graph({
			.width = 900,
			.height = 300,
			.lines = {
				{
					"channel 0",
					::das::ui::line{}
				},
				{
					"channel 1",
					::das::ui::line{}
				}
			}
		});

	auto&& main_graph = plot.graph()[0];
	auto&& analyse_graph = plot.graph()[1];

	auto&& channel0_line = main_graph.lines.at("channel 0");
	auto&& channel1_line = main_graph.lines["channel 1"];

	::das::ui::line_storage channel0_line_data{};
	::das::ui::line_storage channel1_line_data{};

	::das::runtime::data.pcie_config = {
		.demodulation_channel_quantity = ::pcie6920::channel_quantity::_1,
		.points_per_scan = 256 * 64,
		.scan_rate = 2000,
		.trigger_pulse_width = 4,
		.center_frequency = 80000000,
		.data_source_sel = ::pcie6920::parse_rule::raw_data,
		.upload_rate_sel = ::pcie6920::upload_rate::_250m
	};

	::pcie6920::instance.config(::das::runtime::data.pcie_config);

	::std::vector<short> read_buffer{};
	::std::size_t time = 0;

	while (!gui.window().is_should_close())
	{
		auto render_guard = gui.render_guard();

		{
			auto read_guard = ::pcie6920::instance.read_guard();

			do
			{
				using namespace ::std::chrono_literals;
				::std::this_thread::sleep_for(1ms);
			} while (read_guard.point_number_per_channel_in_buffer_query() < static_cast<::std::size_t>(::das::runtime::data.total_point_number));

			read_buffer.resize(::das::runtime::data.total_point_number);

			read_buffer.resize(read_guard.read(read_buffer.data(), ::das::runtime::data.total_point_number / 2));
		}

		if(::das::runtime::data.pcie_config.data_source_sel == pcie6920::parse_rule::raw_data)
		{
			struct raw_data
			{
				short channel0;
				short channel1;
			};
			::std::span buffer(reinterpret_cast<raw_data*>(read_buffer.data()), read_buffer.size() / sizeof(raw_data));

			constexpr auto limit_point = ::std::numeric_limits<short>::max() / 8;

			channel0_line_data.independent_variable.reserve(channel0_line_data.independent_variable.size() + buffer.size());
			channel0_line_data.depend_variable.reserve(channel0_line_data.depend_variable.size() + buffer.size());

			channel1_line_data.independent_variable.reserve(channel1_line_data.independent_variable.size() + buffer.size());
			channel1_line_data.depend_variable.reserve(channel1_line_data.depend_variable.size() + buffer.size());

			for(auto&&[channel0, channel1] : buffer)
			{
				channel0_line_data.push(static_cast<short>(time), channel0);
				channel1_line_data.push(static_cast<short>(time), channel1);
				time++;
			}

			if(channel0_line_data.independent_variable.size() > limit_point)
			{
				::std::ranges::shift_left(channel0_line_data.independent_variable, static_cast<long long>(
					                          channel0_line_data.independent_variable.size() - limit_point));
				::std::ranges::shift_left(channel0_line_data.depend_variable, static_cast<long long>(
					                          channel0_line_data.depend_variable.size() - limit_point));
				channel0_line_data.independent_variable.resize(limit_point);
				channel0_line_data.depend_variable.resize(limit_point);
			}
			if (channel1_line_data.independent_variable.size() > limit_point)
			{
				::std::ranges::shift_left(channel1_line_data.independent_variable, static_cast<long long>(
					channel1_line_data.independent_variable.size() - limit_point));
				::std::ranges::shift_left(channel1_line_data.depend_variable, static_cast<long long>(
					channel1_line_data.depend_variable.size() - limit_point));
				channel1_line_data.independent_variable.resize(limit_point);
				channel1_line_data.depend_variable.resize(limit_point);
			}
		}

		channel0_line.bind(channel0_line_data);
		channel1_line.bind(channel1_line_data);

		plot.render();

		::das::ui::render_ui();


	}

	return 0;
}