/**
	\file "sim/prsim/Command.h"
	TODO: not only modify simulator state but possibly
		control interpreter state as well (modes).
	$Id: Command.h,v 1.6.42.3 2006/12/11 00:40:28 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_COMMAND_H__
#define	__HAC_SIM_PRSIM_COMMAND_H__

#include <iosfwd>
#include <string>
#include "util/macros.h"
#include "sim/command.h"
#include "sim/command_category.h"
#include "sim/command_registry.h"

namespace HAC {
namespace SIM {
template <class> class Help;

namespace PRSIM {
class State;
using std::string;
using util::default_qmap;
using util::string_list;
using std::string;
using std::ostream;

//=============================================================================
typedef	SIM::Command<State>		Command;
typedef	command_category<Command>	CommandCategory;
typedef	command_registry<Command>	CommandRegistry;

//=============================================================================
/**
	Not a real class but a template of what a typical command class 
	should look like, interface-wise.
	The command-registration template function expects 
	members in this class.  
	The CommandTemplate is used to construct a Command object.
	TODO: perhaps a helper functor, leveraging template argument deduction?
 */
struct CommandTemplate {
	static const char		name[];
	static const char		brief[];
	static const CommandCategory&	category;

	static
	int
	main(State&, const string_list&);

	static
	void
	usage(ostream& o);
private:
	static const size_t		receipt_id;
};	// end class CommandTemplate

//=============================================================================
#if 0
/**
	The user may define custom alias commands at run time.  
	Aliasing just performs string substitution.  
	Need to define scope of duration for alias commands.  
	Don't want to affect successive invocations.  
 */
class CommandAlias {
};	// end class command
#endif

//=============================================================================

/**
	Declares a command class.  
 */
#define	DECLARE_PRSIM_COMMAND_CLASS(class_name)				\
struct class_name {                                                     \
public:                                                                 \
	static const char		name[];				\
	static const char		brief[];			\
	static CommandCategory&		category;			\
	static int	main(State&, const string_list&);		\
	static void	usage(ostream&);				\
private:								\
	static const size_t		receipt_id;			\
};

/**
	public help class.  
 */
typedef	Help<State>			Help;

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_COMMAND_H__

