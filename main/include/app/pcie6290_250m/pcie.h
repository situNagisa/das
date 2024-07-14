#pragma once

#include "./configurator.h"
#include "./saver.h"
#include "./reader.h"
#include "./plot.h"
#include "./defined.h"

NGS_LIB_MODULE_BEGIN

struct instance
{
	instance()
	{
		resize(_configurator.info().packet_size, _configurator.process_frame());
		_instance.config(_configurator.info());
	}

	void resize(::std::size_t packet_size, ::std::size_t process_frame)
	{
		_plot.set_time_span(::pcie6920_250m::atomic::unit_size(packet_size));
		_reader.buffer().resize(packet_size * process_frame);
		NGS_LOGL(debug, ::std::format("packet size: {}, frame per scan: {}", packet_size, process_frame));
	}

	void shrink_to_fit()
	{
		_reader.buffer().shrink_to_fit();
	}

	void update()
	{
		update_configure();
		update_saver();
		update_reader();
	}

	void update_configure()
	{
		if (_configurator.is_change().parse_rule)
		{
			NGS_LOGL(debug, "parse rule change: ", ::std::to_underlying(_configurator.info().parse_rule));
			if (_instance.set_data_source(_configurator.info().parse_rule))
				NGS_LOGL(error, "set data source fail: ", ::std::to_underlying(_configurator.info().parse_rule));
		}
		if (_configurator.is_change().upload_rate)
		{
			NGS_LOGL(debug, "upload rate change: ", ::std::to_underlying(_configurator.info().upload_rate));
			if (_instance.set_upload_rate(_configurator.info().upload_rate))
				NGS_LOGL(error, "set upload rate fail: ", ::std::to_underlying(_configurator.info().upload_rate));
		}
		if (_configurator.is_change().channel_quantity)
		{
			NGS_LOGL(debug, "channel quantity change: ", ::std::to_underlying(_configurator.info().channel_quantity));
			if (_instance.set_demodulation_channel_quantity(_configurator.info().channel_quantity))
				NGS_LOGL(error, "set demodulation channel quantity fail: ", ::std::to_underlying(_configurator.info().channel_quantity));
		}
		if (_configurator.is_change().scan_rate)
		{
			NGS_LOGL(debug, "scan rate change: ", _configurator.info().scan_rate);
			if (_instance.set_scan_rate(_configurator.info().scan_rate))
				NGS_LOGL(error, "set scan rate fail: ", _configurator.info().scan_rate);
		}
		if (_configurator.is_change().pulse_width)
		{
			NGS_LOGL(debug, "pulse width change: ", _configurator.info().pulse_width);
			if (_instance.set_pulse_width(_configurator.info().pulse_width))
				NGS_LOGL(error, "set pulse width fail: ", _configurator.info().pulse_width);
		}
		if (_configurator.is_change().center_frequency)
		{
			NGS_LOGL(debug, "center frequency change: ", _configurator.info().center_frequency);
			if (_instance.set_center_frequency(_configurator.info().center_frequency * 1'000'000))
				NGS_LOGL(error, "set center frequency fail: ", _configurator.info().center_frequency);
		}
		if (_configurator.is_change().packet_size)
		{
			NGS_LOGL(debug, "packet size change: ", _configurator.info().packet_size);
			if (_instance.set_scan_packet(_configurator.info().packet_size))
				NGS_LOGL(error, "set packet size fail: ", _configurator.info().packet_size);
			resize(_configurator.info().packet_size, _configurator.process_frame());
			shrink_to_fit();
		}
	}
	decltype(auto) _times_to_frame(::std::size_t collect_times) const
	{
		return collect_times * _configurator.info().scan_rate / _configurator.process_frame();
	}
	decltype(auto) _frame_to_times(::std::size_t hold_frame) const
	{
		return (hold_frame * _configurator.process_frame() / _configurator.info().scan_rate);
	}
	void update_saver()
	{
		if (_saver.is_change().save)
		{
			_current_record_times = _saver.info().record_times;
		}
	}
	void update_reader()
	{
		if(_current_record_times)
		{
			if(_reader.done())
			{
				_reader.start(_instance, _saver.get_channel0_file(), _saver.get_channel1_file(), _times_to_frame(_saver.info().collect_times));
			}
			if (!_reader.done())
			{
				_reader.read();

				if (_reader.done())
				{
					_reader.stop();
					_current_record_times--;
				}
			}
		}
	}

	bool record_done() const
	{
		return _reader.done() && !_current_record_times;
	}

	void render_config()
	{
		::ImGui::BeginDisabled(!record_done());
		_configurator.render();
		::ImGui::EndDisabled();
	}

	void render_plot()
	{
		_plot.render("channel 0", { 900,300 }, _reader.buffer().channel<0>());
		_plot.render("channel 1", { 900,300 }, _reader.buffer().channel<1>());
	}

	void render_save()
	{
		::ImGui::BeginDisabled(!record_done());
		_saver.render(record_done(), _frame_to_times(_reader.current_frame()), _current_record_times);
		::ImGui::EndDisabled();
	}

	::pcie6920_250m::guard::instance _instance{};
	configurator _configurator{};
	saver _saver{};
	plot _plot{};
	reader _reader{};

	::std::size_t _current_record_times = 0;
};

NGS_LIB_MODULE_END