#pragma once

#include "./defined.h"

NGS_LIB_BEGIN

struct buffer
{
	auto size_data() { return _data | ::std::views::take(size); }
	auto size_data() const { return _data | ::std::views::take(size); }

	auto flag()const { return ::laser::mic::protocols::flag(static_cast<::laser::mic::protocols::flag::type>(::std::to_integer<::std::uint16_t>(_data[0]) | (::std::to_integer<::std::uint16_t>(_data[1]) << 8))); }
	auto length()const { return ::laser::mic::protocols::length(::std::to_integer<::std::uint8_t>(_data[2])); }
	::std::size_t data_length() const { return length().value() - 3; }
	auto data_current_length() const { return size_data().size() - 5; }
	auto address()const { return ::laser::mic::protocols::address(static_cast<::laser::mic::protocols::address::type>(::std::to_integer<::std::uint8_t>(_data[3]))); }
	auto response()const { return ::laser::mic::protocols::response(static_cast<::laser::mic::protocols::response::type>(::std::to_integer<::std::uint8_t>(_data[4]))); }

	auto&& data() { return _data; }
	auto&& data() const { return _data; }

	

	::std::deque<::std::byte> _data{};
	mutable ::std::size_t size = 0;
};

struct handler
{
	::std::function<bool(const buffer& buffer, ::std::byte new_byte)> check = [](auto&&...) { return false; };
	::std::function<bool(const buffer& buffer)> complete = [](auto&&...) { return false; };
};

struct decode_data
{
	::std::vector<::std::byte> data{};
};


inline struct _buffer_checker_function
{
	enum class status
	{
		flag,
		length,
		address,
		response,
		data,
		checksum,
		completed,
	};

	_buffer_checker_function()
	{
		//flag
		_handlers.push_back({
			.check = [](const buffer& buffer, ::std::byte new_byte) { return new_byte == static_cast<::std::byte>(static_cast<::std::uint16_t>(::laser::mic::protocols::flag::type::receive) >> (8 * buffer.size_data().size())); },
			.complete = [](const buffer& buffer) { return buffer.size_data().size() >= 2; },
			});
		//length
		_handlers.push_back({
			.check = [](const buffer& buffer, ::std::byte new_byte) { return ::std::to_integer<::std::uint8_t>(new_byte) >= 3; },
			.complete = [](const buffer& buffer) { return buffer.size_data().size() >= 3; },
			});
		//address
		_handlers.push_back({
			.check = [](const buffer& buffer, ::std::byte new_byte) { return true; },
			.complete = [](const buffer& buffer) { return buffer.size_data().size() >= 4; },
			});
		//response
		_handlers.push_back({
			.check = [](const buffer& buffer, ::std::byte new_byte) { return true; },
			.complete = [](const buffer& buffer) { return buffer.size_data().size() >= 5; },
			});
		//data
		_handlers.push_back({
			.check = [](const buffer& buffer, ::std::byte new_byte) { return buffer.data_current_length() < buffer.data_length(); },
			.complete = [](const buffer& buffer) { return buffer.data_current_length() == buffer.data_length(); },
			});
		//checksum
		_handlers.push_back({
			.check = [](const buffer& buffer, ::std::byte new_byte)
			{
				auto checksum = static_cast<::std::uint8_t>(::std::ranges::fold_left(buffer.size_data() | ::std::views::transform([](::std::byte byte)
					{
						return ::std::to_integer<::std::uint8_t>(byte);
					}), static_cast<::std::uint8_t>(0), ::std::plus{}));
				return ::std::to_integer<::std::uint8_t>(new_byte) == checksum;
					
			},
			.complete = [](const buffer& buffer) { return buffer.size_data().size() == buffer.length().value() + 3; },
			});
	}

	auto check(status state, const buffer& buffer, ::std::byte new_byte) const
	{
		if (const auto& [check, complete] = _handlers[static_cast<::std::size_t>(state)]; complete(buffer))
		{
			next_flag(state);
		}
		const auto& [check, complete] = _handlers[static_cast<::std::size_t>(state)];
		return ::std::make_pair(check(buffer, new_byte), state);
	}

	auto operator()(const buffer& buffer) const
	{
		status state = status::flag;
		buffer.size = 0;
		for(auto&& byte : buffer.data())
		{
			auto [success, new_state] = check(state, buffer, byte);
			state = new_state;
			if(!success)
			{
				return ::std::make_pair(false, state);
			}
			buffer.size++;
		}
		if (const auto& [check, complete] = _handlers[static_cast<::std::size_t>(state)]; complete(buffer))
		{
			next_flag(state);
		}
		return ::std::make_pair(true, state);
	}

	void next_flag(status& state) const
	{
		if (state == status::completed)
		{
			state = status::flag;
			return;
		}
		state = static_cast<status>(static_cast<::std::size_t>(state) + 1);
	}

	::std::vector<handler> _handlers{};
}buffer_checker{};

struct stream
{
	void in(::std::byte byte)
	{
		_buffer.in._data.push_back(byte);
		_buffer_checker_function::status status{};
		while (true)
		{
			auto&& [success, state] = buffer_checker(_buffer.in);
			if (success)
			{
				status = state;
				_can_use_buffer = true;
				break;
			}
			_buffer.in.data().pop_front();
			_can_use_buffer = false;
		}
		if (status == _buffer_checker_function::status::completed)
		{
			decode_data data{};
			::std::ranges::copy(_buffer.in.size_data(), ::std::back_inserter(data.data));
			_buffer.out.push(::std::move(data));
			_buffer.in._data.clear();
		}
	}
	void in(::std::ranges::input_range auto&& range)
		requires ::std::convertible_to<::std::ranges::range_value_t<decltype(range)>, ::std::byte>
	{
		for (auto&& byte : range)
		{
			stream::in(static_cast<::std::byte>(byte));
		}
	}

	auto size() const
	{
		return _buffer.out.size();
	}

	auto empty() const
	{
		return _buffer.out.empty();
	}

	auto out()
	{
		auto result = ::std::move(_buffer.out.front());
		_buffer.out.pop();
		return result;
	}

	struct
	{
		buffer in{};
		::std::queue<decode_data> out{};
	}_buffer{};
	bool _can_use_buffer = false;
};

NGS_LIB_END