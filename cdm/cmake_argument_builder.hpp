#ifndef CDM_CMAKE_ARGUMENT_BUILDER_HPP
#define CDM_CMAKE_ARGUMENT_BUILDER_HPP

#include <silicium/sink/append.hpp>
#include <silicium/noexcept_string.hpp>

namespace cdm
{
	template <class StringSink>
	struct cmake_argument_builder
	{
		StringSink m_arguments;

		explicit cmake_argument_builder(StringSink arguments)
			: m_arguments(std::forward<StringSink>(arguments))
		{
		}

		template <class Key, class Value>
		void define(Key const &key, Value const &value)
		{
			Si::noexcept_string argument = "-D";
			argument += key;
			argument += "=";
			argument += value;
			Si::append(m_arguments, std::move(argument));
		}
	};
}

#endif
