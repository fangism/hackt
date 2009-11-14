/**
	\file "sim/command_completion.h"
	$Id: command_completion.h,v 1.4 2009/11/14 03:12:11 fang Exp $
 */

#ifndef	__HAC_SIM_COMMAND_COMPLETION_H__
#define	__HAC_SIM_COMMAND_COMPLETION_H__

// #include "util/tokenize_fwd.h"

namespace util {
class directory_stack;
}

namespace HAC {
namespace entity {
class module;
}
namespace SIM {
using util::directory_stack;	// from "util/directory.h"

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

// defined in "sim/command_base.cc"
// global variable needed by the instance_completer
extern
const entity::module* instance_completion_module;

// global variable needed by the instance_completer
extern
const directory_stack*
instance_completion_dirs;

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
	Note: specializations will have the same storage specification
	as their primary templates, [DR 605], and thus need not, 
	and shall not be re-specified.  In this case, 'static'.
	Change is new in g++-4.3.
 */
#define	SPECIALIZE_COMMAND_COMPLETER(_class, _func)			\
template <>								\
char*									\
Completer<_class >(const char* text, const int state) {			\
	return _func(text, state);					\
}

/**
	This macro must be invoked *before* initializing the 
	command class with the INITIALIZE_COMMAND_CLASS macro
	because the overriding specialization must be present 
	before the point of instantiation.
	Specializes in the Completer function's home namespace.
	This temporarily escapes up to the parent namespace
	then re-opens the child namespace.
 */
#define	OVERRIDE_DEFAULT_COMPLETER(_ns, _class, _func)			\
}	/* close namespace _ns */					\
SPECIALIZE_COMMAND_COMPLETER(_ns::_class, _func)			\
namespace _ns {	/* re-open namespace _ns */


//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_COMMAND_COMPLETION_H__

