#include <boost/filesystem/operations.hpp>
#include <iostream>

int main()
{
	std::cout << boost::filesystem::current_path() << '\n';
}
