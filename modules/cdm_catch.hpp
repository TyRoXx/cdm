#ifndef CDM_CATCH_HPP
#define CDM_CATCH_HPP

#include <silicium/file_operations.hpp>

namespace cdm
{
	struct catch_paths
	{
		Si::absolute_path include;
	};

	inline catch_paths install_catch(
		Si::absolute_path const &original_source,
		Si::absolute_path const &temporarily_writable,
		Si::absolute_path const &install_root,
		Si::Sink<char, Si::success>::interface &output)
	{
		Si::absolute_path const in_cache = install_root / Si::relative_path("catch");
		if (!Si::file_exists(in_cache, Si::throw_))
		{
			Si::absolute_path const construction = temporarily_writable / Si::relative_path("catch");
			Si::copy_recursively(original_source / Si::relative_path("include"), construction, &output, Si::throw_);
			Si::rename(construction, in_cache, Si::throw_);
		}
		catch_paths result;
		result.include = in_cache;
		return result;
	}
}

#endif
