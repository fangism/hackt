/**
	\file "PR/pr-command.h"
	$Id: pr-command.h,v 1.1.2.1 2011/04/11 18:38:40 fang Exp $
 */

#ifndef	__HAC_PR_PR_COMMAND_H__
#define	__HAC_PR_PR_COMMAND_H__

#include <iosfwd>
#include <string>
#include "util/macros.h"
#include "util/tokenize_fwd.h"
#include "sim/command_base.h"
#include "sim/command_category.h"
#include "sim/command_registry.h"
#include "sim/command_macros.h"

namespace PR {
class placement_engine;
typedef	placement_engine			State;
using std::string;
using util::string_list;
using std::string;
using std::ostream;

//=============================================================================
typedef	HAC::SIM::Command<State>		Command;
typedef	HAC::SIM::command_category<Command>	CommandCategory;
typedef	HAC::SIM::command_registry<Command>	CommandRegistry;

//=============================================================================
#if 0
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
#endif

//=============================================================================

/**
	Declares a command class.  
 */
#define	DECLARE_PR_COMMAND_CLASS(class_name)				\
struct class_name {                                                     \
public:                                                                 \
	static const char		name[];				\
	static const char		brief[];			\
	static CommandCategory&		category;			\
	static int	main(State&, const string_list&);		\
	static void	usage(ostream&);				\
	static const command_completer	completer;			\
private:								\
	static const size_t		receipt_id;			\
};

#if 0
/**
	public help class.  
 */
typedef	HAC::SIM::Help<State>			Help;
#endif

//=============================================================================
}	// end namespace PR

#endif	// __HAC_PR_PR_COMMAND_H__

