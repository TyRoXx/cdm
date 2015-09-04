#include <boost/test/unit_test.hpp>
#include <silicium/sink/iterator_sink.hpp>
#include "log.hpp"

BOOST_AUTO_TEST_CASE(make_program_output_printer_empty_input)
{
	std::string screen;
	auto printer = cdm::make_program_output_printer(Si::make_container_sink(screen));
	BOOST_CHECK_EQUAL("", screen);
	Si::append(printer, "");
	BOOST_CHECK_EQUAL("", screen);
}

BOOST_AUTO_TEST_CASE(make_program_output_printer_multiple_lines)
{
	std::string screen;
	auto printer = cdm::make_program_output_printer(Si::make_container_sink(screen));
	BOOST_CHECK_EQUAL("", screen);
	Si::append(printer, "abc\ndef\n123");
	BOOST_CHECK_EQUAL("\tabc\n\tdef\n\t123", screen);
}

BOOST_AUTO_TEST_CASE(make_program_output_printer_empty_lines)
{
	std::string screen;
	auto printer = cdm::make_program_output_printer(Si::make_container_sink(screen));
	BOOST_CHECK_EQUAL("", screen);
	Si::append(printer, "\ndef\n\n123\n\n\n");
	BOOST_CHECK_EQUAL("\t\n\tdef\n\t\n\t123\n\t\n\t\n", screen);
}

BOOST_AUTO_TEST_CASE(make_program_output_printer_crlf)
{
	std::string screen;
	auto printer = cdm::make_program_output_printer(Si::make_container_sink(screen));
	BOOST_CHECK_EQUAL("", screen);
	Si::append(printer, "abc\r\ndef\r\n123");
	BOOST_CHECK_EQUAL("\tabc\r\n\tdef\r\n\t123", screen);
}
