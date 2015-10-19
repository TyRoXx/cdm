#include <ventura/absolute_path.hpp>
#include <cdm/configure_result.hpp>

namespace CDM_CONFIGURE_NAMESPACE
{
	cdm::configure_result configure(
		ventura::absolute_path const &module_temporaries,
		ventura::absolute_path const &module_permanent,
		ventura::absolute_path const &application_source,
		ventura::absolute_path const &application_build_dir,
		unsigned cpu_parallelism,
		Si::Sink<char, Si::success>::interface &output
		)
	{
		boost::ignore_unused_variable_warning(module_temporaries);
		boost::ignore_unused_variable_warning(module_permanent);
		boost::ignore_unused_variable_warning(application_source);
		boost::ignore_unused_variable_warning(application_build_dir);
		boost::ignore_unused_variable_warning(cpu_parallelism);
		boost::ignore_unused_variable_warning(output);
		return cdm::configure_result();
	}
}
