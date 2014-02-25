/**
	\file "sim/command_registry.hh"
	$Id: command_registry.hh,v 1.16 2011/05/23 01:10:51 fang Exp $
 */

#ifndef	__HAC_SIM_COMMAND_REGISTRY_HH__
#define	__HAC_SIM_COMMAND_REGISTRY_HH__

#include <iosfwd>
#include <vector>
#include <string>
#include <utility>
#include <map>
#include "sim/type_scope.hh"
#include "util/macros.h"
#include "util/tokenize_fwd.hh"
#include "util/attributes.h"
#include "util/value_saver.hh"
#include "util/directory.hh"

namespace HAC {
namespace entity {
class module;
}
namespace SIM {
using std::string;
using std::map;
using util::string_list;
using util::value_saver;
using util::directory_stack;
using std::string;
using std::ostream;
using std::istream;
using entity::module;
template <class> class command_category;

// TODO: typedef CommandStatus command_return_type; (instead of int)

//=============================================================================
/**
	State-independent base class for alias support.  
 */
class command_aliases_base {
protected:
	struct auto_file_echo {
		ostream&		os;
		const bool		echo;
		const string		name;
		auto_file_echo(ostream&, const bool, const string&);
		~auto_file_echo();
	} __ATTRIBUTE_UNUSED__ ;	// end struct auto_file_echo
public:
	/**
		An alias is just a string_list, the key string
		will expand into the value strings.  
	 */
	typedef	map<string, string_list>		aliases_map_type;
	typedef	aliases_map_type::const_iterator	alias_iterator;

	static
	int
	expand_aliases(const aliases_map_type&, string_list&);

	static
	int
	add_alias(aliases_map_type&, const string&, const string_list&);

	static
	int
	unalias(aliases_map_type&, const string&);

	static
	int
	unalias_all(aliases_map_type&);

	static
	void
	list_aliases(const aliases_map_type&, ostream&);

};	// end class command_aliases_base

//=============================================================================
/**
	Static global map of commands.  
	Any compelling reason why this should be a purely static class?
	Consider Singleton class?
	What if need multiple instances?
 */
template <class Command>
class command_registry : public command_aliases_base {
	typedef	command_registry<Command>		this_type;
public:
	typedef	Command					command_type;
	typedef	typename command_type::state_type	state_type;
	typedef	command_category<command_type>		command_category_type;
private:
	typedef	map<string, command_type>		command_map_type;
	// consider using never_ptr to guarantee NULL initialization
	typedef	map<string, command_category_type*>	category_map_type;
	typedef	typename command_map_type::const_iterator
							command_iterator;
	typedef	typename category_map_type::const_iterator
							category_iterator;
	using command_aliases_base::auto_file_echo;
public:
	typedef	typename command_type::main_ptr_type	main_ptr_type;
	typedef	typename command_type::usage_ptr_type	usage_ptr_type;
	/**
		Class for initializing readline and cleaning up when done.
	 */
	class readline_init {
		value_saver<char** (*)(const char*, int, int)>
							_compl;
		value_saver<const module*>		_mod;
		value_saver<const directory_stack*>	_dirs;
	public:
	
	readline_init();

	explicit
	readline_init(const module&);

	~readline_init();
	} __ATTRIBUTE_UNUSED__ ;	// end struct readline_init
private:
	class dummy_type;
	static dummy_type		dummy;
	static command_map_type		command_map;
	static category_map_type	category_map;
	/**
		Aliases specific to this interpreter.  
	 */
	static aliases_map_type		aliases;
	/**
		Directory stack state (to emulate cd, pushd, popd, ...).
	 */
	static directory_stack		dir_stack;
	/**
		Current type-scope context.
	 */
	static type_scope_manager	type_stack;
	/**
		Keep command history.
	 */
	typedef	std::vector<string>	history_type;
	static history_type		history;
	/**
		For nested comment blocks, pseudo C-style.
	 */
	static int			comment_level;
	/**
		For error reporting, where the outer-most comment began.
	 */
	static int			begin_outermost_comment;
public:
	/**
		User-modifiable prompt string. 
	 */
	static	string			prompt;
	/**
		Switch: whether or not each command is echoed as it is
		interpreted.  Default off (false).
		TODO: use local switch instead?
	 */
	static bool			echo_commands;
	/**
		Whether or not to record history of sourced, 
		non-interactive commands.  
	 */
	static bool			keep_noninteractive_history;
	/**
		Set to true if co-simulating, and thus, should forbid
		user from manually advancing time with simulation commands.
	 */
	static bool			external_cosimulation;
private:
	// not constructible
	command_registry();

	// not copy-able
	command_registry(const this_type&);
public:
	template <class C>
	static
	size_t
	register_command(void);

	static
	size_t
	register_category(command_category_type&);

	static
	void
	list_categories(ostream&);

	static
	void
	list_commands(ostream&);

	static
	int
	interpret(state_type&, istream&, const bool);

	static
	int
	interpret_stdin(state_type&);

	static
	int
	interpret_line(state_type&, const string&, const bool = false);

	static
	int
	source(state_type&, const string&);

	static
	bool
	continue_interpreter(const int _status, const bool);

	static
	void
	forbid_cosimulation(const string_list&);	// throw std::exception

	static
	int
	execute(state_type&, const string_list&);

	static
	int
	repeat_execute(state_type&, const string_list&, const size_t);

	static
	bool
	help_command(ostream&, const string&);

	static
	bool
	help_category(ostream&, const string&);

	static
	int
	expand_aliases(string_list&);

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

// history commands
	static
	std::pair<int, int>
	history_range(const int, const int);

	static
	ostream&
	dump_history(ostream&, const int, const int);

	static
	int
	rerun(state_type&, const int, const int);

	// save-to-file
	static
	int
	write_history(const string&);

// directory commands
	static
	void
	reset_dirs(void);

	static
	bool
	change_dir(const string&);

	static
	bool
	change_dir_abs(const string&);

	static
	bool
	push_dir(const string&);

	static
	bool
	pop_dir(void);

	static
	const string&
	working_dir(void);

	static
	string
	prepend_working_dir(const string&);

	static
	ostream&
	show_working_dir(ostream&);

	static
	ostream&
	show_dirs(ostream&);

	// for readline tab-completion
	static
	char**
	completion(const char*, int, int);

// type-scope commands
	static
	bool
	in_local_type(void) {
		return type_stack.in_local_type();
	}

	static
	const entity::footprint&
	current_type(void) {
		return type_stack.current_type();
	}

	static
	void
	push_type(const entity::footprint* f) {
		type_stack.push(f);
	}

	static
	void
	pop_type(void) {
		type_stack.pop();
	}

private:
	class interactive_mode;

	static
	int
	__source(istream&, state_type&);

	// for readline tab-completion
	static
	char*
	command_generator(const char*, int);

};	// end class command_registry

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Display hook to print matching candidates without
	the common prefix, basically by omitting everything
	up to and including the last '.' in a hierarchical name.
	Defined in command_registry.cc.
 */
extern
void
display_hierarchical_matches_hook(char**, int, int);

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_COMMAND_REGISTRY_HH__

