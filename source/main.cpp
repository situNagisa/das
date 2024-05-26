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
		.points_per_scan = 100,
		.scan_rate = 1,
		.trigger_pulse_width = 2,
		.center_frequency = 3,
		.data_source_sel = ::pcie6920::parse_rule::raw_data,
		.upload_rate_sel = ::pcie6920::upload_rate::_250m
	};

	::pcie6920::instance.config(::das::runtime::data.pcie_config);

	::std::vector<::std::byte> read_buffer{};

	while (!gui.window().is_should_close())
	{
		auto render_guard = gui.render_guard();

		{
			auto read_guard = ::pcie6920::instance.read_guard();

			//do
			//{
			//	using namespace ::std::chrono_literals;
			//	::std::this_thread::sleep_for(1ms);
			//} while (read_guard.point_number_per_channel_in_buffer_query() < static_cast<::std::size_t>(runtime_image.total_point_number));

			read_buffer.resize(::das::runtime::data.total_point_number * 2 * sizeof(short));

			read_buffer.resize(read_guard.read(read_buffer.data(), ::das::runtime::data.total_point_number * 2 * sizeof(short)));
		}

		if(::das::runtime::data.pcie_config.data_source_sel == pcie6920::parse_rule::raw_data)
		{
			struct raw_data
			{
				short channel0;
				short channel1;
			};
			::std::span buffer(reinterpret_cast<raw_data*>(read_buffer.data()), read_buffer.size() / sizeof(raw_data));

			static ::std::size_t i = 0;

			for(auto&&[channel0, channel1] : buffer)
			{
				channel0_line_data.push(::std::make_tuple(static_cast<float>(i) * 0.0001f, static_cast<float>(channel0), static_cast<float>(channel1)));
				channel1_line_data.push(::std::make_tuple(static_cast<float>(i) * 0.0001f, static_cast<float>(channel1), -1.0f));
			}
			i++;
		}

		channel0_line.bind(channel0_line_data);
		channel1_line.bind(channel1_line_data);

		plot.render();

		::das::ui::render_ui();


	}

	return 0;
}