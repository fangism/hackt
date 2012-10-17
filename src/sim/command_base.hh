/**
	\file "sim/command_base.hh"
	TODO: not only modify simulator state but possibly
		control interpreter state as well (modes).
	$Id: command_base.hh,v 1.6 2009/02/19 02:58:29 fang Exp $
 */

#ifndef	__HAC_SIM_COMMAND_BASE_H__
#define	__HAC_SIM_COMMAND_BASE_H__

#include <iosfwd>
#include <string>
#include "util/macros.h"
#include "util/tokenize_fwd.hh"
#include "sim/command_error_codes.hh"
#include "sim/command_completion.hh"

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
class CommandBase : public command_error_codes {
public:
	typedef	CommandStatus		Status;
	typedef	void (usage_type) (ostream&);
	typedef	usage_type*		usage_ptr_type;
protected:
	/// name of the command, the key used
	string				_name;
	/// one-line description
	string				_brief;
	/// verbose decription of command usage
	usage_ptr_type			_usage;
	/// command-line argument completer
	command_completer		_completer;
public:
	CommandBase();

	CommandBase(const string& _n, const string& _b, 
		const usage_ptr_type u = NULL, 
		const command_completer c = NULL);

	~CommandBase();

	const string&
	name(void) const { return _name; }

	const string&
	brief(void) const { return _brief; }

	void
	usage(ostream&) const;
	
	command_completer
	completer(void) const { return _completer; }

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
		const main_ptr_type m = NULL, const usage_ptr_type u = NULL, 
		const command_completer c = NULL);

	// default copy-ctor

	~Command() { }

	operator bool () const { return this->_main; }

	int
	main(state_type&, const string_list&) const;

};	// end class command

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_COMMAND_BASE_H__

