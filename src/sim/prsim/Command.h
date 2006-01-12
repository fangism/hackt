/**
	\file "sim/prsim/Command.h"
	TODO: not only modify simulator state but possibly
		control interpreter state as well (modes).
	$Id: Command.h,v 1.1.2.1 2006/01/12 06:13:09 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_COMMAND_H__
#define	__HAC_SIM_PRSIM_COMMAND_H__

#include <iosfwd>
#include "util/macros.h"
#include "util/qmap.h"
#include "util/tokenize_fwd.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
class State;
using util::qmap;
using util::string_list;
using std::string;
using std::ostream;

//=============================================================================
/**
	Simulator command.  
	TODO: make this re-usable.  
 */
class Command {
public:
	typedef	int (main_type) (State&, const string_list&);
	typedef	main_type*		main_ptr_type;
	typedef	void (usage_type) (ostream&);
	typedef	usage_type*		usage_ptr_type;
private:
	main_ptr_type			_main;
	usage_ptr_type			_usage;
public:
	explicit
	Command(const main_ptr_type m = NULL, const usage_ptr_type u = NULL) :
		_main(m), _usage(u) { }

	// default copy-ctor

	~Command() { }

	operator bool () const { return _main; }

	int
	main(State&, const string_list&) const;

	void
	usage(ostream& o) const;

};	// end class command

//=============================================================================
/**
	Static global map of commands.  
 */
class CommandRegistry {
private:
	typedef	qmap<string, Command>	command_map_type;
public:
	typedef	Command::main_ptr_type	main_ptr_type;
	typedef	Command::usage_ptr_type	usage_ptr_type;
	struct command_entry {
		command_entry(const string&, const main_ptr_type = NULL,
			const usage_ptr_type = NULL);
	};	// end struct command_entry
private:
	static command_map_type		command_map;

public:
	static
	void
	list(ostream&);

	static
	int
	interpret(State&, const bool);

	static
	int
	execute(State&, const string_list&);

private:
	static int _help_main(State&, const string_list&);
	static void _help_usage(ostream&);
	static int _echo_main(State&, const string_list&);
	static void _echo_usage(ostream&);
	static int _comment_main(State&, const string_list&);
	static void _comment_usage(ostream&);

	static const command_entry	
		help_command,
		echo_command,
		comment_command, comment_command2;

};	// end class command registry

//=============================================================================
/**
	The user may define custom alias commands at run time.  
	Aliasing just performs string substitution.  
 */
class CommandAlias {
};	// end class command

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_COMMAND_H__

