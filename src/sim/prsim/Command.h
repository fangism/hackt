/**
	\file "sim/prsim/Command.h"
	TODO: not only modify simulator state but possibly
		control interpreter state as well (modes).
	$Id: Command.h,v 1.6 2006/04/18 18:42:42 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_COMMAND_H__
#define	__HAC_SIM_PRSIM_COMMAND_H__

#include <iosfwd>
#include <string>
#include "util/macros.h"
#include "util/qmap.h"
#include "util/tokenize_fwd.h"
#include "util/member_saver_fwd.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
class State;
using std::string;
using util::default_qmap;
using util::string_list;
using std::string;
using std::ostream;
class CommandCategory;

//=============================================================================
/**
	Simulator command.  
	This is an entry in the registry tables.  
	TODO: make this re-usable.  
 */
class Command {
public:
	typedef	int (main_type) (State&, const string_list&);
	typedef	main_type*		main_ptr_type;
	typedef	void (usage_type) (ostream&);
	typedef	usage_type*		usage_ptr_type;
	typedef	CommandCategory*	category_ptr_type;
	/**
		A few reserved exit codes for main functions.  
		TODO: validate the range of these values w.r.t. 
			return type size or standard enums.
	 */
	enum Status {
		FATAL = -4,	///< terminate immediately (e.g. assert fail)
		BADFILE = -3,	///< source file not found
		SYNTAX = -2,	///< bad syntax
		UNKNOWN = -1,	///< unknown command
		NORMAL = 0,	///< command executed fine
		BADARG = 1,	///< other error with input
		END = 0xFF	///< normal exit, such as EOF
	};
private:
	/// name of the command, the key used
	string				_name;
	/// one-line description
	string				_brief;
	/// back-reference to category to which this command belongs
	category_ptr_type		_category;
	/// command execution
	main_ptr_type			_main;
	/// verbose decription of command usage
	usage_ptr_type			_usage;
public:
	Command();

	Command(const string& _n, const string& _b,
		const category_ptr_type,
		const main_ptr_type m = NULL, const usage_ptr_type u = NULL);

	// default copy-ctor

	~Command() { }

	operator bool () const { return _main; }

	const string&
	name(void) const { return _name; }

	const string&
	brief(void) const { return _brief; }

	int
	main(State&, const string_list&) const;

	void
	usage(ostream& o) const;

};	// end class command

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
/**
	Static global map of commands.  
	Any compelling reason why this should be a purely static class?
	Consider Singleton class?
 */
class CommandRegistry {
private:
	typedef	default_qmap<string, Command>::type	command_map_type;
	// consider using never_ptr to guarantee NULL initialization
	typedef	default_qmap<string, CommandCategory*>::type
							category_map_type;
	typedef	command_map_type::const_iterator	command_iterator;
	typedef	category_map_type::const_iterator	category_iterator;
public:
	typedef	Command::main_ptr_type	main_ptr_type;
	typedef	Command::usage_ptr_type	usage_ptr_type;
	/**
		An alias is just a string_list, the key string
		will expand into the value strings.  
	 */
	typedef	default_qmap<string, string_list>::type	aliases_map_type;
	typedef	aliases_map_type::const_iterator	alias_iterator;
private:
	static command_map_type		command_map;
	static category_map_type	category_map;
	static aliases_map_type		aliases;
public:
	template <class C>
	static
	size_t
	register_command(void);

	static
	size_t
	register_category(CommandCategory&);

	static
	void
	list_categories(ostream&);

	static
	void
	list_commands(ostream&);

	static
	int
	interpret(State&, const bool);

	static
	int
	interpret_line(State&, const string&);

	static
	int
	expand_aliases(string_list&);

	static
	int
	source(State&, const string&);

	static
	bool
	continue_interpreter(const int _status, const bool);

	static
	int
	execute(State&, const string_list&);

	static
	bool
	help_command(ostream&, const string&);

	static
	bool
	help_category(ostream&, const string&);

	static
	int
	add_alias(const string&, const string_list&);

	static
	int
	unalias(const string&);

	static
	int
	unalias_all(void);

	static
	void
	list_aliases(ostream&);

private:
	class interactive_mode;

	static
	int
	__source(std::istream&, State&);

};	// end class CommandRegistry

//=============================================================================
/**
	We organize all prsim commands into categories.  
 */
class CommandCategory {
private:
	typedef	default_qmap<string, Command>::type	command_map_type;
	typedef	Command::main_ptr_type		main_ptr_type;
	typedef	Command::usage_ptr_type		usage_ptr_type;
	typedef	command_map_type::const_iterator	const_iterator;
private:
	string					_name;
	string					_brief;
	command_map_type			command_map;
public:
	CommandCategory();
	CommandCategory(const string& _n, const string& _b);
private:
	// private, undefined copy-constructor
	CommandCategory(const CommandCategory&);
public:
	~CommandCategory();

	// this should not be called directly, but rather through
	// CommandRegistry::register_command().
	size_t
	register_command(const Command&);

	const string&
	name(void) const { return _name; }

	const string&
	brief(void) const { return _brief; }

	void
	list(ostream&) const;

	const_iterator
	lookup_command(const string&) const;

	bool
	is_valid(const const_iterator i) const {
		return i != command_map.end();
	}

};	// end class CommandCategory

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
#define	DECLARE_COMMAND_CLASS(class_name)				\
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

// declare some generally useful commands
/**
	The 'help' command class.  
	Not using the macro to define because we extend the interface somewhat.
 */
struct Help {
public:
	static const char		name[];
	static const char		brief[];
	static CommandCategory&		category;
	static int	main(const string_list&);	// no state needed
	static int	main(State&, const string_list&);
	static void	usage(ostream&);
private:
	static const size_t		receipt_id;
};	// end class Help

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_COMMAND_H__

