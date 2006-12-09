/**
	\file "sim/command_builtin.tcc"
	$Id: command_builtin.tcc,v 1.1.2.1 2006/12/09 07:52:07 fang Exp $
 */

#ifndef	__HAC_SIM_COMMAND_BUILTIN_TCC__
#define	__HAC_SIM_COMMAND_BUILTIN_TCC__

#include "sim/command_builtin.h"
#include "sim/command.h"
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
	Defined to use specialization.  
	This includes static initializers for the non-function members.  
	\param _class the name of the class.
	\param _cmd the string-quoted name of the command.
	\param _category the cateory object with which to associate.  
	\param _brief one-line quoted-string description.
 */
#define	INITIALIZE_STATELESS_COMMAND_CLASS(_class, _cmd, _category, _brief) \
template <>								\
const char _class::name[] = _cmd;					\
template <>								\
const char _class::brief[] = _brief;					\
template <>								\
_class::command_category_type& _class::category(_category);		\
template <>								\
const size_t _class::receipt_id = 					\
	_class::command_registry_type::register_command<_class >();

#if 0
/**
	Combined macro.
 */
#define	DECLARE_AND_INITIALIZE_STATELESS_COMMAND_CLASS(_class, _cmd, _category, _brief) \
	DECLARE_STATELESS_COMMAND_CLASS(_class)				\
	INITIALIZE_STATELESS_COMMAND_CLASS(_class, _cmd, _category, _brief)
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapper that drops unused state reference argument.  
 */
COMMAND_WRAPPER_TEMPLATE_SIGNATURE
int
COMMAND_WRAPPER_CLASS::main(state_type&, const string_list& args) {
	return command_base_type::main(args);
}


//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_COMMAND_BUILTIN_TCC__

