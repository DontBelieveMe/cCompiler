#pragma once

#include <stdint.h>

#include <vector>
#include <string>
#include <array>
#include <sstream>
#include <memory>
#include <iostream>

namespace cc {
	typedef uint8_t u8;
	typedef int8_t i8;

	typedef uint16_t u16;
	typedef int16_t i16;

	typedef uint32_t u32;
	typedef int32_t i32;

	typedef uint64_t u64;
	typedef int64_t i64;

	typedef std::size_t size_t;
	typedef std::ptrdiff_t ptrdiff_t;

	template <typename T>
	using array = std::vector<T>;

	template <typename T, size_t SIZE>
	using fixed_array = std::array<T, SIZE>;

	// #todo ... maybe implement some form of abstract buffer class
	// e.g. instead of having to use a cc::u8* pointer and hacky code
	// such as `cc::u16 p = *((cc::u16*)(ptr + offset));`
	// instead...
	/*
		cc::u8* ptr = ...;
		cc::byte_buffer buff(ptr); // takes ownership of `ptr`
		cc::u16 p = buff.read<cc::u16>(offset); // offset in bytes from start of buffer
		buff.write<u16>(some_value, offset); // offset in bytes. some_value is a u16.
		cc::fixed_array<u8, 3> id = buff.read_series<u8, 3>(offset); // 3 is the number of u8's to read.
	*/

	// #todo ... need for fixed string class
	// e.g something like
	/*
		cc::fixed_string<10> str;
		str[0] = 'a';
		str = "the string"; // 10 characters.
		cc::u8* buff = str.data();
		cc::size_t length = str.size(); // returns 10.
	*/

	typedef std::string string;
	typedef std::stringstream string_builder;

	template <typename T>
	using shared_ptr = std::shared_ptr<T>;

	template <typename T>
	using unique_ptr = std::unique_ptr<T>;

	template <typename T>
	using scoped_ptr = std::unique_ptr<T> const;
	
	// #todo ... look at maybe having a cc::make_scoped<T>(Args&&... args);  function
	// like make_unique and make_shared.

	template <typename T, typename... Args>
	shared_ptr<T> make_shared(Args&&... args) {
		return std::make_shared<T>(args...);
	}

	template <typename T, typename... Args>
	unique_ptr<T> make_unique(Args&&... args) {
		return std::make_unique<T>(args...);
	}

	template <typename T>
	struct singleton {
		static T* instance() {
			static T t;
			return &t;
		}
	};

	template <typename... Args>
	string format_string(const string& str, const Args&... args);

	struct env {
		static const char newline = '\n';
		static const char tab = '\t';
	};

	struct console : public singleton<console> {
		template <typename... Args>
		static void printlnf(const string& fmt, const Args&... args) {
			std::cout << format_string(fmt, args...) << env::newline;
		}

		static void print(char c) { std::cout << c; }

		template <typename... Args>
		static void printf(const string& fmt, const Args&... args) {
			std::cout << format_string(fmt, args...);
		}

		static void println(size_t num = 1) { 
			for (size_t i = 0; i < num; ++i) {
				std::cout << env::newline;
			}

			flush(); 
		}

		static void flush() {
			std::cout.flush();
		}
	};
}

#include <cc/stringfmt_impl.h>