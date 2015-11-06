#include "temporary.hpp"
#include <ventura/file_operations.hpp>

namespace cdm
{
	ventura::absolute_path get_temporary_root_for_testing()
	{
		return ventura::temporary_directory(Si::throw_) / "cdm"
#ifdef _MSC_VER
		       BOOST_STRINGIZE(_MSC_VER)
#endif
		    ;
	}
}
