#include "./das/das.h"


// Main code
int main(int, char**)
{
	using namespace ::std::chrono_literals;

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
		.demodulation_channel_quantity = ::pcie6920::enums::channel_quantity::_1,
		.points_per_scan = 256 * 64,
		.scan_rate = 2000,
		.trigger_pulse_width = 4,
		.center_frequency = 80000000,
		.data_source_sel = ::pcie6920::enums::parse_rule::raw_data,
		.upload_rate_sel = ::pcie6920::enums::upload_rate::_250m
	};
	using packet_type = ::pcie6920::atomic::packet<pcie6920::enums::parse_rule::raw_data, pcie6920::enums::channel_quantity::_1>;

	::pcie6920::guard::open pcie{};
	::pcie6920::atomic::config(::das::runtime::data.pcie_config);

	::std::vector<packet_type> read_buffer{};
	::std::size_t time = 0;

	auto now = ::std::chrono::system_clock::now();

	while (!gui.window().is_should_close())
	{
		auto render_guard = gui.render_guard();
		{
			if(auto io = pcie.io(); io.buffer_packet_size() >= ::das::runtime::data.packet_size_per_scan)
			{
				if (::std::chrono::system_clock::now() - now > 1s)
				{
					now = ::std::chrono::system_clock::now();
					{
						read_buffer.resize(::das::runtime::data.packet_size_per_scan);

						read_buffer.resize(io.read(read_buffer));
					}

					if (::das::runtime::data.pcie_config.data_source_sel == pcie6920::enums::parse_rule::raw_data)
					{
						constexpr ::std::size_t limit_point = ::std::numeric_limits<short>::max() / 16;

						auto buffer = ::pcie6920::atomic::unpack(read_buffer);
						auto push = [time](::das::ui::line_storage& line, decltype(buffer) buffer, ::std::size_t channel_index)
							{
								auto push_size = ::std::min(limit_point, ::std::ranges::size(buffer));

								if (line.size() >= limit_point)
								{
									::std::ranges::shift_left(line.independent_variable, static_cast<long long>(::std::ranges::size(buffer)));
									::std::ranges::shift_left(line.depend_variable, static_cast<long long>(::std::ranges::size(buffer)));
									line.resize(limit_point - push_size);
								}
								auto t = time;
								for (auto&& unit : buffer | ::std::views::take(push_size))
								{
									line.push(static_cast<::std::int32_t>(t), unit[static_cast<long long>(channel_index)].data);
									t++;
								}
							};

						push(channel0_line_data, buffer, 0);
						push(channel1_line_data, buffer, 1);

						time += ::std::ranges::size(buffer);

						channel0_line.bind(channel0_line_data);
						channel1_line.bind(channel1_line_data);
					}
				}
			}

			plot.render();
			::das::ui::render_ui();
		}
	}

	return 0;
}