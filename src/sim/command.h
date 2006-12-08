/**
	\file "sim/command.h"
	TODO: not only modify simulator state but possibly
		control interpreter state as well (modes).
	$Id: command.h,v 1.1.2.1 2006/12/08 07:51:19 fang Exp $
 */

#ifndef	__HAC_SIM_COMMAND_H__
#define	__HAC_SIM_COMMAND_H__

#include <iosfwd>
#include <string>
#include "util/macros.h"
#include "util/tokenize_fwd.h"

namespace HAC {
namespace SIM {
using std::string;
using util::string_list;
using std::ostream;
template <class>
class command_category;

//=============================================================================
/**
	State-independent base class of Command.  
 */
class CommandBase {
public:
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

	typedef	void (usage_type) (ostream&);
	typedef	usage_type*		usage_ptr_type;
protected:
	/// name of the command, the key used
	string				_name;
	/// one-line description
	string				_brief;
	/// verbose decription of command usage
	usage_ptr_type			_usage;
public:
	CommandBase();

	CommandBase(const string& _n, const string& _b, 
		const usage_ptr_type u = NULL);

	~CommandBase();

	const string&
	name(void) const { return _name; }

	const string&
	brief(void) const { return _brief; }

	void
	usage(ostream&) const;

};	// end class CommandBase

//=============================================================================
/**
	Simulator command.  
	This is an entry in the registry tables.  
 */
template <class State>
class Command : public CommandBase {
	typedef	Command<State>		this_type;
public:
	typedef	State			state_type;
	typedef	int (main_type) (state_type&, const string_list&);
	typedef	main_type*		main_ptr_type;
	typedef	command_category<this_type>	command_category_type;
	typedef	command_category_type*	category_ptr_type;
private:
	/// back-reference to category to which this command belongs
	category_ptr_type		_category;
	/// command execution
	main_ptr_type			_main;
public:
	Command();

	Command(const string& _n, const string& _b,
		const category_ptr_type,
		const main_ptr_type m = NULL, const usage_ptr_type u = NULL);

	// default copy-ctor

	~Command() { }

	operator bool () const { return this->_main; }

	int
	main(state_type&, const string_list&) const;

};	// end class command

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
template <class State>
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

#if 0
/**
	Declares a command class.  
	TODO: Make this generic macro...
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
#endif

#if 0
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
#endif

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_COMMAND_H__

