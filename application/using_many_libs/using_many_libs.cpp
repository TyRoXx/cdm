#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <boost/network/protocol/http.hpp>
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include <boost/filesystem/operations.hpp>
#include "sqlite3.h"
#include <iostream>
#include "rapidxml.hpp"
#include "rapidjson/document.h"

unsigned int Factorial( unsigned int number )
{
    return number <= 1 ? number : Factorial(number-1)*number;
}

TEST_CASE( "Factorials are computed", "[factorial]" )
{
    REQUIRE( Factorial(1) == 1 );
    REQUIRE( Factorial(2) == 2 );
    REQUIRE( Factorial(3) == 6 );
    REQUIRE( Factorial(10) == 3628800 );

	websocketpp::server<websocketpp::config::asio> s;

	sqlite3 *db;
	REQUIRE(!sqlite3_open(":memory:", &db));
	sqlite3_close(db);

	rapidxml::xml_document<> doc;
	char source[] = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><root></root>";
	doc.parse<0>(source);

	rapidjson::Document document;

	std::cout << boost::filesystem::current_path() << '\n';

	boost::network::http::client::request request_("http://google.com/");
	request_ << boost::network::header("Connection", "close");
	boost::network::http::client client_;
	boost::network::http::client::response response_ = client_.get(request_);
	std::string body_ = body(response_);
	std::cerr << body_ << '\n';
}
