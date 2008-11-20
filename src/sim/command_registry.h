/**
	\file "sim/command_registry.h"
	$Id: command_registry.h,v 1.4.4.1 2008/11/20 23:18:46 fang Exp $
 */

#ifndef	__HAC_SIM_COMMAND_REGISTRY_H__
#define	__HAC_SIM_COMMAND_REGISTRY_H__

#include <iosfwd>
#include <string>
#include "util/macros.h"
#include "util/qmap.h"	// TODO: use std::map instead
#include "util/tokenize_fwd.h"
#include "util/attributes.h"

namespace HAC {
namespace SIM {
using std::string;
using util::default_qmap;
using util::string_list;
using std::string;
using std::ostream;
template <class> class command_category;

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
	typedef	default_qmap<string, string_list>::type	aliases_map_type;
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
	typedef	typename default_qmap<string, command_type>::type
							command_map_type;
	// consider using never_ptr to guarantee NULL initialization
	typedef	typename default_qmap<string, command_category_type*>::type
							category_map_type;
	typedef	typename command_map_type::const_iterator
							command_iterator;
	typedef	typename category_map_type::const_iterator
							category_iterator;
	using command_aliases_base::auto_file_echo;
public:
	typedef	typename command_type::main_ptr_type	main_ptr_type;
	typedef	typename command_type::usage_ptr_type	usage_ptr_type;
private:
	static command_map_type		command_map;
	static category_map_type	category_map;
	/**
		Aliases specific to this interpreter.  
	 */
	static aliases_map_type		aliases;
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
		Switch: whether or not each command is echoed as it is
		interpreted.  Default off (false).
		TODO: use local switch instead?
	 */
	static bool			echo_commands;
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
	interpret(state_type&, const bool);

	static
	int
	interpret_line(state_type&, const string&);

	static
	int
	source(state_type&, const string&);

	static
	bool
	continue_interpreter(const int _status, const bool);

	static
	int
	execute(state_type&, const string_list&);

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

private:
	class interactive_mode;

	static
	int
	__source(std::istream&, state_type&);

};	// end class command_registry

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_COMMAND_REGISTRY_H__

