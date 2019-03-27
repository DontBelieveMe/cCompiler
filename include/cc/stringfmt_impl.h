#pragma once

namespace cc {
	namespace fmt_impl {
		template <int ArrSize>
		void to_string_array(FixedArray<String, ArrSize>& str_array, int& index)
		{
			(void)str_array; (void)index;
		}

		template <int ArrSize, typename T, typename... Args>
		void to_string_array(FixedArray<String, ArrSize>& str_array, int& index, const T& item, const Args&... args)
		{
			StringBuilder ss;
			ss << item;

			str_array[index++] = ss.str();
			to_string_array(str_array, index, args...);
		}

		template <int ArrSize, typename... Args>
		void to_string_array(FixedArray<String, ArrSize>& arr, const Args&... args)
		{
			int i = 0;
			to_string_array(arr, i, args...);
		}
	}

	// This is to allow passing empty variadic template argument lists and variadic macro parameters
	// to this method, and still compile.
	// Initially implemented for assertion macros - no need to specify additional message, but you can
	// and it will format correctly as a format string.
	inline String format_string() { return ""; }

	// #todo: implement -> complete format syntax checking
	template <typename... Args>
	String format_string(const String& fmt, const Args&... args)
	{
		FixedArray<String, sizeof...(args)> args_array;
		fmt_impl::to_string_array(args_array, args...);

		StringBuilder buff;
		for (size_t i = 0; i < fmt.length(); ++i)
		{
			char currentchar = fmt[i];

			if (currentchar == '{')
			{
				if (i + 1 < fmt.length())
				{
					if (fmt[i + 1] == '{')
					{
						i++;
						buff << '{';
						continue;
					}
				}
				else {
					continue;
				}

				String index_string;
				while (currentchar != '}')
				{
					currentchar = fmt[++i];
					index_string += currentchar;
				}

				const int index = std::stoi(index_string);

				if (index < static_cast<int>(args_array.size()) && index >= 0)
				{
					buff << args_array[index];
				}
			}
			else {
				buff << currentchar;
			}
		}

		return buff.str();
	}
}