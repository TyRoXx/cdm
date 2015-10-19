#ifndef CDM_CATCH_HPP
#define CDM_CATCH_HPP

#include <ventura/file_operations.hpp>

namespace cdm
{
	struct catch_paths
	{
		ventura::absolute_path include;
	};

	inline catch_paths install_catch(ventura::absolute_path const &original_source, ventura::absolute_path const &temporarily_writable,
	                                 ventura::absolute_path const &install_root, Si::Sink<char, Si::success>::interface &output)
	{
		ventura::absolute_path const in_cache = install_root / ventura::relative_path("catch");
		if (!ventura::file_exists(in_cache, Si::throw_))
		{
			ventura::absolute_path const construction = temporarily_writable / ventura::relative_path("catch");
			ventura::copy_recursively(original_source / ventura::relative_path("include"), construction, &output, Si::throw_);
			ventura::rename(construction, in_cache, Si::throw_);
		}
		catch_paths result;
		result.include = in_cache;
		return result;
	}
}

#endif
