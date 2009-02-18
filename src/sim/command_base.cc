/**
	\file "sim/command_base.cc"
	$Id: command_base.cc,v 1.3 2009/02/18 00:22:37 fang Exp $
 */

#include <iostream>
#include <iterator>
#include <string>
#include "sim/command_base.h"
#include "util/NULL.h"

// stuff needed for instance completion
#include "parser/instref.h"

namespace HAC {
namespace SIM {

//=============================================================================
/**
	Converts simulator's error code signal into an error code
	for the interpreter.  
 */
CommandStatus
error_policy_to_status(const error_policy_enum e) {
switch (e) {
case ERROR_IGNORE:
case ERROR_WARN:
case ERROR_BREAK:
	return command_error_codes::NORMAL;
case ERROR_INTERACTIVE:
	return command_error_codes::INTERACT;
case ERROR_FATAL:
default:
	return command_error_codes::FATAL;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char*
error_policy_string(const error_policy_enum e) {
	switch (e) {
	case ERROR_IGNORE:	return "ignore";
	case ERROR_WARN:	return "warn";
	case ERROR_BREAK:	return "break";
	case ERROR_INTERACTIVE:	return "interactive";
	case ERROR_FATAL:	return "fatal";
	default:		DIE;
	}	// end switch
	 return NULL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Too lazy to write a map...
 */
error_policy_enum
string_to_error_policy(const string& s) {
	static const string _ignore("ignore");
	static const string _warn("warn");
	static const string _notify("notify");
	static const string _break("break");
	static const string _interactive("interactive");
	static const string _fatal("fatal");
	if (s == _ignore) {
		return ERROR_IGNORE;
	} else if (s == _warn || s == _notify) {
		return ERROR_WARN;
	} else if (s == _break) {
		return ERROR_BREAK;
	} else if (s == _interactive) {
		return ERROR_INTERACTIVE;
	} else if (s == _fatal) {
		return ERROR_FATAL;
	}
	// else
	return ERROR_INVALID;
}

//=============================================================================
// class CommandBase method definitions

CommandBase::CommandBase() : _name(), _brief(), _usage(NULL), 
		_completer(NULL) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CommandBase::CommandBase(const string& _n, const string& _b,
		const usage_ptr_type _u, 
		const command_completer _c) :
		_name(_n), _brief(_b), _usage(_u), _completer(_c) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CommandBase::~CommandBase() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
CommandBase::usage(ostream& o) const {
	if (_usage) {
		(*_usage)(o);
	} else {
		o << "help/usage unavailable." << std::endl;
	}
}

//=============================================================================
// completion functions

using std::cout;
using std::endl;
using std::vector;

char*
null_completer(const char*, const int) {
//	cout << "null_completer()";
	return NULL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Set this module before calling any completion functions
	that require a module.
	Recommand using util::value_saver for automatic restoration.
 */
const entity::module* instance_completion_module = NULL;
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Function is not re-entrant.
	\return newly allocated copy of matching strings, or NULL to signal end.
 */
char*
instance_completer(const char* _text, const int state) {
	typedef vector<string>::const_iterator	const_iterator;
//	cout << "instance_completer(" << _text << ',' << state << ")";
	NEVER_NULL(_text);
	static const_iterator i, e;
	if (!state) {
		static vector<string> matches;
		matches.clear();
		NEVER_NULL(instance_completion_module);
		parser::complete_instance_names(_text, 
			*instance_completion_module, matches);
		i = matches.begin();
		e = matches.end();
#if 0
		copy(i, e, std::ostream_iterator<string>(cout, ", "));
			cout << endl;
#endif
	}
	if (i != e) {
		const char* n = i->c_str();
		NEVER_NULL(n);
#if 0
		cout << "strdup(" << n << ")...";
#endif
		++i;
		char* ret = strdup(n);
#if 0
		NEVER_NULL(ret);
		cout << " success." << endl;
#endif
		return ret;
	}
	return NULL;
}

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

