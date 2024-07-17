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
		_configurator.set_process_frame(1);
		resize(_configurator.info().packet_size, _configurator.process_frame());
		_instance.config(_configurator.info());
		_reader.start(_instance);
	}
	~instance()
	{
		_reader.stop();
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
	bool recording() const
	{
		return _current_frame;
	}
	void record_start()
	{
		_configurator.set_process_frame(128);
		resize(_configurator.info().packet_size, _configurator.process_frame());
		_current_frame = _saver.info().record_times * _saver.info().collect_times * _configurator.info().scan_rate / _configurator.process_frame();

		NGS_LOGL(info, ::std::format("record start, total frame: {}, process frame: {}, packet size: {}", _current_frame, _configurator.process_frame(), _configurator.info().packet_size));

		if(_saver.info().save_channel[0])
		{
			if (!_saver.open_channel_output_file(0))
			{
				NGS_LOGL(error, "open channel 0 output file fail");
			}
		}
		if (_saver.info().save_channel[1])
		{
			if (!_saver.open_channel_output_file(1))
			{
				NGS_LOGL(error, "open channel 1 output file fail");
			}
		}
	}
	void record()
	{
		if (_saver.channel_output_is_open(0))
		{
			auto buffer = ::std::as_bytes(_reader.buffer().channel<0>());
			_saver.write_channel_output_file(0, buffer.data(), buffer.size());
		}
		if (_saver.channel_output_is_open(1))
		{
			auto buffer = ::std::as_bytes(_reader.buffer().channel<1>());
			_saver.write_channel_output_file(1, buffer.data(), buffer.size());
		}
	}
	void record_end()
	{
		NGS_LOGL(debug, "record done");
		if (_saver.channel_output_is_open(0))
			_saver.close_channel_output_file(0);
		if (_saver.channel_output_is_open(1))
			_saver.close_channel_output_file(1);
	}

	void update()
	{
		update_config();
		update_saver();
		update_reader();
	}
	//=================config=================
	void update_config()
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
	void render_config()
	{
		::ImGui::BeginDisabled(recording());
		_configurator.render();
		::ImGui::EndDisabled();
	}
	//=================saver=================
	void update_saver()
	{
		if (_saver.is_change().save)
		{
			record_start();
		}
	}
	void render_saver()
	{
		::ImGui::BeginDisabled(recording());
		_saver.render(!recording());
		::ImGui::EndDisabled();
	}
	//=================reader=================
	void update_reader()
	{
		if(_reader.read())
		{
			if(recording())
			{
				_current_frame--;

				record();

				if (!recording())
				{
					record_end();
				}
			}
		}
	}
	//=================plot=================
	void render_plot()
	{
		_plot.render("channel 0", { 900,300 }, _reader.buffer().channel<0>());
		_plot.render("channel 1", { 900,300 }, _reader.buffer().channel<1>());
	}


	::pcie6920_250m::guard::instance _instance{};
	configurator _configurator{};
	saver _saver{};
	plot _plot{};
	reader _reader{};

	::std::size_t _current_frame = 0;
};

NGS_LIB_MODULE_END