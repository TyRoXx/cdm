#include <git2.h>
#include <iostream>

int main()
{
	int major, minor, rev;
	git_libgit2_version(&major, &minor, &rev);
	std::cerr << major << "." << minor << "." << rev << '\n';
}
