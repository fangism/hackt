/**
	\file "sim/command_builtin.h"
	Converts/imports state-independent commands into
	a state-dependent command registry.  
	$Id: command_builtin.h,v 1.1.2.1 2006/12/09 07:52:04 fang Exp $
 */

#ifndef	__HAC_SIM_COMMAND_BUILTIN_H__
#define	__HAC_SIM_COMMAND_BUILTIN_H__

#include <iosfwd>
#include "util/size_t.h"
#include "util/tokenize_fwd.h"

namespace HAC {
namespace SIM {
using std::ostream;
using util::string_list;
template <class> class Command;
template <class> class command_category;
template <class> class command_registry;

//=============================================================================
/**
	Bogus command class, used as an example.  
	NOTE: we deliberately push the name and brief strings
	to child classes to avoid dependence on global static
	object initialization ordering (had they been included
	here in the base class)
 */
class SampleBaseCommand {
public:
	static int				main(const string_list&);
	static void				usage(ostream&);
};	// end class SampleBaseCommand

//=============================================================================
#define	COMMAND_WRAPPER_TEMPLATE_SIGNATURE				\
template <class Com, class State>

#define	COMMAND_WRAPPER_CLASS						\
stateless_command_wrapper<Com,State>

/**
	Adapter to take a stateless command class and adapt it
	to a stateful command class.  
 */
COMMAND_WRAPPER_TEMPLATE_SIGNATURE
class stateless_command_wrapper : public Com {
public:
	typedef	Com				command_base_type;
	typedef	State				state_type;
	typedef	Command<State>			command_type;
	typedef	command_registry<command_type>	command_registry_type;
	typedef	typename command_type::command_category_type
						command_category_type;
	static const char			name[];
	static const char			brief[];
	static command_category_type&		category;

	static	int		main(state_type&, const string_list&);
private:
	static const size_t			receipt_id;
};	// end class stateless_command_wrapper

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define	DECLARE_STATELESS_COMMAND_CLASS(class_name)			\
struct class_name {                                                     \
public:                                                                 \
	static int	main(const string_list&);			\
	static void	usage(ostream&);				\
};

// here: we declare a bunch of state-independent base command classes
DECLARE_STATELESS_COMMAND_CLASS(Echo)
DECLARE_STATELESS_COMMAND_CLASS(CommentPound)
DECLARE_STATELESS_COMMAND_CLASS(CommentComment)
DECLARE_STATELESS_COMMAND_CLASS(Exit)
DECLARE_STATELESS_COMMAND_CLASS(Quit)

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_COMMAND_BUILTIN_H__

