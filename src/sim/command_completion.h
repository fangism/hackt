/**
	\file "sim/command_completion.h"
	$Id: command_completion.h,v 1.1 2009/02/18 00:22:43 fang Exp $
 */

#ifndef	__HAC_SIM_COMMAND_COMPLETION_H__
#define	__HAC_SIM_COMMAND_COMPLETION_H__

// #include "util/tokenize_fwd.h"

namespace HAC {
namespace entity {
class module;
}
namespace SIM {

//=============================================================================
// for custom command/argument completion
typedef	char* (*command_completer)(const char*, int);

// functions defined in "sim/command_base.cc"
extern
char*
null_completer(const char*, const int);

extern
char*
instance_completer(const char*, const int);

// global variable needed by the instance_completer
extern
const entity::module* instance_completion_module;

/**
	Overrideable template defaults to some completion function.
	Recommend explicit specialization for each class.
 */
template <class C>
static
inline
char*
Completer(const char* t, const int s) {
	return null_completer(t, s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This macro must be invoked *before* initializing the 
	command class with the INITIALIZE_COMMAND_CLASS macro
	because the overriding specialization must be present 
	before the point of instantiation.
	Specializes in the Completer function's home namespace.
 */
#define	OVERRIDE_DEFAULT_COMPLETER(_ns, _class, _func)			\
class _class;								\
}	/* close namespace _ns */					\
template <>								\
static									\
char*									\
Completer<_ns::_class >(const char* text, const int state) {		\
	return _func(text, state);					\
}									\
namespace _ns {	/* re-open namespace _ns */


//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_COMMAND_COMPLETION_H__

