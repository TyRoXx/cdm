#include <silicium/absolute_path.hpp>
#include <cdm/configure_result.hpp>

namespace CDM_CONFIGURE_NAMESPACE
{
	cdm::configure_result configure(
		Si::absolute_path const &module_temporaries,
		Si::absolute_path const &module_permanent,
		Si::absolute_path const &application_source,
		Si::absolute_path const &application_build_dir,
		Si::optional<Si::absolute_path> const &boost_root,
		Si::Sink<char, Si::success>::interface &output
		)
	{
		boost::ignore_unused_variable_warning(module_temporaries);
		boost::ignore_unused_variable_warning(module_permanent);
		boost::ignore_unused_variable_warning(application_source);
		boost::ignore_unused_variable_warning(application_build_dir);
		boost::ignore_unused_variable_warning(boost_root);
		boost::ignore_unused_variable_warning(output);
		return cdm::configure_result();
	}
}
