/**
	\file "sim/command_macros.h"
	Command-declaration macros.
	$Id: command_macros.h,v 1.1 2009/02/19 02:58:35 fang Exp $
 */

#ifndef	__HAC_SIM_COMMAND_MACROS_H__
#define	__HAC_SIM_COMMAND_MACROS_H__

#include <iosfwd>
#include "util/size_t.h"
#include "util/tokenize_fwd.h"
#include "sim/command_completion.h"

namespace HAC {
namespace SIM {
using std::ostream;
using util::string_list;
template <class> class Command;
template <class> class command_category;
template <class> class command_registry;

//=============================================================================
/**
	This declares a final class that will be used to construct 
	the Command<State> class used by the command_registry.  
	TODO: factor usage() out into base class function?
 */
#define	DECLARE_COMMON_COMMAND_CLASS(_class)				\
template <class State>							\
class _class {								\
public:									\
	typedef	State				state_type;		\
	typedef	Command<state_type>		command_type;		\
	typedef typename command_type::command_category_type		\
						command_category_type;	\
	typedef	command_registry<command_type>	command_registry_type;	\
	static const char			name[];			\
	static const char			brief[];		\
	static command_category_type&		category;		\
	static int	main(state_type&, const string_list&);		\
	static void	usage(ostream&);				\
	static const command_completer		completer;		\
private:								\
	static const size_t			receipt_id;		\
};	// end class _class

//-----------------------------------------------------------------------------
/**
	Macro for declaring a common command class, independent
	of any simulation state or object.
	Similar to command_builtins.h:DECLARE_STATELESS_COMMAND_CLASS.
	To use this as a final command class, pass this class to 
	the stateless_command_wrapper class template, which provides
	the missing members.  
 */
#define	DECLARE_COMMON_STATELESS_COMMAND_CLASS(_class)			\
class _class {								\
public:									\
	static const char			name[];			\
	static const char			brief[];		\
	static int		main(const string_list&);		\
	static void		usage(ostream&);			\
	static const command_completer		completer;		\
};	// end class _class

#define	COMMAND_WRAPPER_TEMPLATE_SIGNATURE				\
template <class Com, class State>

#define	STATELESS_COMMAND_WRAPPER_CLASS					\
stateless_command_wrapper<Com, State>
/**
	Adapter to take a stateless command class and adapt it
	to a stateful command class.  
	Members _name and _brief are inherited.
 */
COMMAND_WRAPPER_TEMPLATE_SIGNATURE
class stateless_command_wrapper : public Com {
	typedef	STATELESS_COMMAND_WRAPPER_CLASS	this_type;
public:
	typedef	Com				command_base_type;
	typedef	State				state_type;
	typedef	Command<State>			command_type;
	typedef	command_registry<command_type>	command_registry_type;
	typedef	typename command_type::command_category_type
						command_category_type;
/*	static const char			name[];		*/
/*	static const char			brief[];	*/
	static command_category_type&		category;

	static	int		main(state_type&, const string_list&);
/*	using Com::usage;	*/
private:
	static const size_t			receipt_id;
};	// end class stateless_command_wrapper

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Suitable declaration for commands that only depend on
	entity::module, and no other state.  
 */
#define	DECLARE_COMMON_MODULE_COMMAND_CLASS(_class)			\
class _class {								\
public:									\
	static const char			name[];			\
	static const char			brief[];		\
	static int	main(const entity::module&, const string_list&);\
	static void		usage(ostream&);			\
	static const command_completer		completer;		\
};	// end class _class

#define	MODULE_COMMAND_WRAPPER_CLASS					\
module_command_wrapper<Com, State>

/**
	Members name and brief are inherited.
 */
COMMAND_WRAPPER_TEMPLATE_SIGNATURE
class module_command_wrapper : public Com {
	typedef	MODULE_COMMAND_WRAPPER_CLASS	this_type;
public:
	typedef	Com				command_base_type;
	typedef	State				state_type;
	typedef	Command<State>			command_type;
	typedef	command_registry<command_type>	command_registry_type;
	typedef	typename command_type::command_category_type
						command_category_type;
/*	static const char			name[];		*/
/*	static const char			brief[];	*/
	static command_category_type&		category;

	static	int		main(state_type&, const string_list&);
private:
	static const size_t			receipt_id;
};	// end class module_command_wrapper


//-----------------------------------------------------------------------------
/**
	Just initializes the _name and _brief members with strings.
	\param _class should be a non-template class name.
 */
#define	DESCRIBE_COMMON_COMMAND_CLASS(_class, _cmd, _brief)		\
const char _class::name[] = _cmd;					\
const char _class::brief[] = _brief;

/**
	Just initializes the _name and _brief members with strings.
	\param _class should be a template class name.
 */
#define DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(_class, _cmd, _brief)	\
template <class S>							\
const char _class<S>::name[] = _cmd;					\
template <class S>							\
const char _class<S>::brief[] = _brief;					\
template <class S>							\
const size_t _class<S>::receipt_id =					\
_class<S>::command_registry_type::template register_command<_class<S> >();

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_COMMAND_MACROS_H__

