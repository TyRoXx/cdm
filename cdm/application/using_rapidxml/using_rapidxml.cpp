#include "rapidxml.hpp"
#include <iostream>

int main()
{
	rapidxml::xml_document<> doc;
	char source[] = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><root></root>";
	try
	{
		doc.parse<0>(source);
	}
	catch (std::exception const &e)
	{
		std::cerr << e.what() << '\n';
		return 1;
	}
}
