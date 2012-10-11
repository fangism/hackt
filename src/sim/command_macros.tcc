/**
	\file "sim/command_macros.tcc"
	$Id: command_macros.tcc,v 1.1 2009/02/19 02:58:35 fang Exp $
 */

#ifndef	__HAC_SIM_COMMAND_MACROS_TCC__
#define	__HAC_SIM_COMMAND_MACROS_TCC__

#include "sim/command_macros.h"
#include "sim/command_base.h"
#include "sim/command_category.h"
#include "sim/command_registry.h"
#include <list>
#include <string>

namespace HAC {
namespace SIM {
//=============================================================================
// class stateless_command_wrapper method definitions


/**
	Use this macro to declare ordinary commands.
	Defined to use specialization for category selection.  
	TODO: use explicit instantiation, not specialization.
	This includes static initializers for the non-function members.  
	\param _class the name of the class.
	\param _cmd the string-quoted name of the command.
	\param _category the cateory object with which to associate.  
	\param _brief one-line quoted-string description.
 */

#define	CATEGORIZE_COMMON_COMMAND_CLASS(_ns, _class, _category)		\
template <>								\
_ns::_class::command_category_type&					\
_ns::_class::category(_category);

/**
	\param _wrap is the wrapper class template
	\param _cat is the category object
 */
#define INSTANTIATE_COMMON_COMMAND_CLASS(_ns, _wrap, _class, _cat)	\
}	/* end namespace _ns */						\
CATEGORIZE_COMMON_COMMAND_CLASS(_ns, _class, _ns::_cat)			\
template class _wrap<_class, _ns::State>;				\
namespace _ns {	/* re-open namespace */

/**
	Variant that is used for namespace that is sibling to SIM.
 */
#define INSTANTIATE_COMMON_COMMAND_CLASS_SIM(_ns, _wrap, _class, _cat)	\
}	/* end namespace _ns */						\
namespace SIM {								\
CATEGORIZE_COMMON_COMMAND_CLASS(_ns, _class, _ns::_cat)			\
template class _wrap<_class, _ns::State>;				\
}	/* end namespace SIM */						\
namespace _ns {	/* re-open namespace */

/**
	Macro for classes based on SIM::Class<State>.
	\param _wrap is the wrapper class template
	Requires the following typedef in the simulator namespace:
	typedef	_class<State>				_class;
 */
#define INSTANTIATE_TRIVIAL_COMMAND_CLASS(_ns, _class, _cat)		\
}	/* end namespace _ns */						\
CATEGORIZE_COMMON_COMMAND_CLASS(_ns, _class, _ns::_cat)			\
template class _class<_ns::State>;					\
namespace _ns {	/* re-open namespace */

/**
	Variant that is used for namespace that is sibling to SIM.
 */
#define INSTANTIATE_TRIVIAL_COMMAND_CLASS_SIM(_ns, _class, _cat)	\
}	/* end namespace _ns */						\
namespace SIM {								\
CATEGORIZE_COMMON_COMMAND_CLASS(_ns, _class, _ns::_cat)			\
template class _class<_ns::State>;					\
}	/* end namespace SIM */						\
namespace _ns {	/* re-open namespace */


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapper that drops unused state reference argument.  
 */
COMMAND_WRAPPER_TEMPLATE_SIGNATURE
int
STATELESS_COMMAND_WRAPPER_CLASS::main(state_type&, const string_list& args) {
	return command_base_type::main(args);
}

/**
	Standard definition for command class registration.
	Still needs to be explicitly instantiated.  
 */
COMMAND_WRAPPER_TEMPLATE_SIGNATURE
const size_t
STATELESS_COMMAND_WRAPPER_CLASS::receipt_id =
STATELESS_COMMAND_WRAPPER_CLASS::
command_registry_type::template register_command<this_type>();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapper that automatically forwards a reference to the 
	module member of the state object.  
 */
COMMAND_WRAPPER_TEMPLATE_SIGNATURE
int
MODULE_COMMAND_WRAPPER_CLASS::main(state_type& s, const string_list& args) {
	return command_base_type::main(s.get_module(), args);
}

COMMAND_WRAPPER_TEMPLATE_SIGNATURE
const size_t
MODULE_COMMAND_WRAPPER_CLASS::receipt_id =
MODULE_COMMAND_WRAPPER_CLASS::
command_registry_type::template register_command<this_type>();

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_COMMAND_MACROS_TCC__

