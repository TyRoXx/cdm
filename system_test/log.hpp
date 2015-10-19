#ifndef CDM_SYSTEM_TEST_LOG_HPP
#define CDM_SYSTEM_TEST_LOG_HPP

#include <silicium/sink/append.hpp>
#include <ventura/absolute_path.hpp>
#include <fstream>

namespace cdm
{
	namespace detail
	{
		template <class NextSink>
		struct program_output_printer
		{
			typedef char element_type;
			typedef Si::success error_type;

			explicit program_output_printer(NextSink next)
			    : m_next(std::move(next))
			    , m_needs_indentation(true)
			{
			}

			error_type append(Si::iterator_range<element_type const *> data)
			{
				auto written = data.begin();
				for (;;)
				{
					if (written == data.end())
					{
						return error_type();
					}
					if (m_needs_indentation)
					{
						Si::append(m_next, '\t');
						m_needs_indentation = false;
					}
					element_type const *const end_of_line = std::find(written, data.end(), '\n');
					if (end_of_line == data.end())
					{
						return m_next.append(Si::make_iterator_range(written, data.end()));
					}
					auto write_until = end_of_line + 1;
					error_type error = m_next.append(Si::make_iterator_range(written, write_until));
					if (error)
					{
						return error;
					}
					written = write_until;
					m_needs_indentation = true;
				}
			}

		  private:
			NextSink m_next;
			bool m_needs_indentation;
		};
	}

	template <class NextSink>
	auto make_program_output_printer(NextSink &&next)
#if !SILICIUM_COMPILER_HAS_AUTO_RETURN_TYPE
	    -> Si::Sink<char, Si::success>::eraser<Si::Sink<char, Si::success>::box>
#endif
	{
		return Si::Sink<char, Si::success>::erase(
		    Si::Sink<char, Si::success>::make_box(detail::program_output_printer<typename std::decay<NextSink>::type>(std::forward<NextSink>(next))));
	}

	inline std::unique_ptr<std::ofstream> open_log(ventura::absolute_path const &name)
	{
		auto utf8_name = to_utf8_string(name);
		std::unique_ptr<std::ofstream> file = Si::make_unique<std::ofstream>(utf8_name.c_str());
		if (!*file)
		{
			throw std::runtime_error("Could not open file for writing: " + utf8_name);
		}
		return file;
	}
}

#endif
