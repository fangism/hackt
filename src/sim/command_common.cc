/**
	\file "sim/command_common.cc"
	Library of template command implementations, re-usable with
	different state types.  
	$Id: command_common.cc,v 1.1 2009/02/19 02:58:33 fang Exp $
 */

#include <iostream>
#include <iterator>
#include <fstream>
#include <list>
#include <string>
#include <algorithm>
#include <functional>

#include "sim/command_common.h"
#include "sim/command_error_codes.h"
#include "sim/command_base.h"
#include "parser/instref.h"
#include "Object/expr/dlfunction.h"
#include "common/TODO.h"
#include "common/ltdl-wrap.h"
#include "util/compose.h"
#include "util/string.tcc"
#include "util/utypes.h"

namespace HAC {
namespace SIM {
using std::ios_base;
using std::ofstream;
using std::for_each;
using std::ptr_fun;
using std::mem_fun_ref;
// using util::strings::string_to_num;
using entity::module;
#include "util/using_ostream.h"
using parser::parse_name_to_what;
using parser::parse_name_to_members;
using parser::parse_name_to_aliases;
USING_UTIL_COMPOSE

//=============================================================================
// class Echo method definitions

DESCRIBE_COMMON_COMMAND_CLASS(Echo, "echo", 
        "prints arguments back to stdout, space-delimited")

int
Echo::main(const string_list& args) {
	INVARIANT(!args.empty());
	std::ostream_iterator<string> osi(cout, " ");
	copy(++args.begin(), args.end(), osi);
	cout << endl;
	return CommandBase::NORMAL;
}

void
Echo::usage(ostream& o) {
	o << "echo [args]: repeats arguments to stdout, space-delimited"
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

DESCRIBE_COMMON_COMMAND_CLASS(CommentPound,
        "#", "comments are ignored")
DESCRIBE_COMMON_COMMAND_CLASS(CommentComment,
        "comment", "comments are ignored")

int
CommentPound::main(const string_list&) { return CommandBase::NORMAL; }

void
CommentPound::usage(ostream& o) {
	o << "# or \'comment\' ignores the whole line." << endl;
}

int
CommentComment::main(const string_list&) { return CommandBase::NORMAL; }

void
CommentComment::usage(ostream& o) { CommentPound::usage(o); }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS(Exit, "exit", "exits simulator")
DESCRIBE_COMMON_COMMAND_CLASS(Quit, "quit", "exits simulator")

int
Exit::main(const string_list&) {
	return CommandBase::END;
}

void
Exit::usage(ostream& o) {
	o << "exit: exits simulator" << endl;
//	o << "exit: " << brief << endl;
}

int
Quit::main(const string_list& a) {
	return Exit::main(a);
}

void
Quit::usage(ostream& o) {
	Exit::usage(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS(Abort,
        "abort", "exits simulator with fatal status")

int
Abort::main(const string_list&) {
	return CommandBase::FATAL;
}

void
Abort::usage(ostream& o) {
	o << "abort" << endl;
	o << "abort: exits simulator with fatal status" << endl;
}

//=============================================================================
DESCRIBE_COMMON_COMMAND_CLASS(LS, "ls",
	"list subinstances of the referenced instance")

int
LS::main(const module& m, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return command_error_codes::SYNTAX;
} else {
	if (parse_name_to_members(cout, a.back(), m))
		return command_error_codes::BADARG;
	else	return command_error_codes::NORMAL;
}
}

void
LS::usage(ostream& o) {
	o << "ls <name>" << endl;
	o << "prints list of subinstances of the referenced instance" << endl;
	o << "\"ls .\" lists top-level instances" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS(What, "what",
	"print type information of named entity")

int
What::main(const module& m, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return command_error_codes::SYNTAX;
} else {
	if (parse_name_to_what(cout, a.back(), m))
		return command_error_codes::BADARG;
	else	return command_error_codes::NORMAL;
}
}

void
What::usage(ostream& o) {
	o << "what <name>" << endl;
	o << "prints the type/size of the referenced instance(s)" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS(Who, "who",
	"print aliases of node or structure")

int
Who::main(const module& m, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return command_error_codes::SYNTAX;
} else {
	cout << "aliases of \"" << a.back() << "\":" << endl;
	if (parse_name_to_aliases(cout, a.back(), m, " ")) {
		return command_error_codes::BADARG;
	} else {
		cout << endl;
		return command_error_codes::NORMAL;
	}
}
}

void
Who::usage(ostream& o) {
	o << "who <name>" << endl;
	o << "prints all aliases (equivalent names) of the referenced instance"
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS(WhoNewline, "who-newline",
	"print aliases of node or structure, newline separated")

int
WhoNewline::main(const module& m, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return command_error_codes::SYNTAX;
} else {
	cout << "aliases of \"" << a.back() << "\":" << endl;
	if (parse_name_to_aliases(cout, a.back(), m, "\n")) {
		return command_error_codes::BADARG;
	} else {
		cout << endl;
		return command_error_codes::NORMAL;
	}
}
}

void
WhoNewline::usage(ostream& o) {
	o << "who-newline <name>" << endl;
	o << "prints all aliases (equivalent names) of the referenced instance"
		<< endl;
}

//=============================================================================
DESCRIBE_COMMON_COMMAND_CLASS(DLAddPath, "dladdpath", 
	"Append search paths for dlopening modules")

int
DLAddPath::main(const string_list& a) {
if (a.size() < 2) {
	usage(cerr << "usage: ");
	return command_error_codes::SYNTAX;
} else {
	for_each(++a.begin(), a.end(), 
		unary_compose(ptr_fun(&lt_dladdsearchdir),
			mem_fun_ref(&string::c_str)));
	return command_error_codes::NORMAL;
}
}

void
DLAddPath::usage(ostream& o) {
	o << name << " <paths...>" << endl;
	o << "Loads an external library, file extension should be omitted.\n"
"Searches user-defined path (`dlpath\') before system paths.\n"
"Module should automatically register functions during static initization."
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS(DLPaths, "dlpaths", 
	"List search paths for dlopening modules")

int
DLPaths::main(const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return command_error_codes::SYNTAX;
} else {
	const char* const p = lt_dlgetsearchpath();
	cout << "dlopen search paths: ";
	if (p)
		cout << lt_dlgetsearchpath();
	else	cout << "<empty>";
	cout << endl;
	return command_error_codes::NORMAL;
}
}

void
DLPaths::usage(ostream& o) {
	o << name << endl;
	o << "Prints current list of search paths used for dlopening." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS(DLOpen, "dlopen", 
	"load a module (dynamically shared library)")

int
DLOpen::main(const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return command_error_codes::SYNTAX;
} else {
	if (!ltdl_open_append(a.back())) {
		return command_error_codes::BADARG;
	} else	return command_error_codes::NORMAL;
}
}

void
DLOpen::usage(ostream& o) {
	o << name << " <library>" << endl;
	o << "Loads an external library, file extension should be omitted.\n"
"Searches user-defined path (`dlpath') before system paths.\n"
"Module should automatically register functions during static initization."
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS(DLCheckFunc, "dlcheckfunc", 
	"query whether a nonmeta function symbol is bound")

int
DLCheckFunc::main(const string_list& a) {
if (a.size() < 2) {
	usage(cerr << "usage: ");
	return command_error_codes::SYNTAX;
} else {
	// ignore assert's return value
	DLAssertFunc::main(a);
	return command_error_codes::NORMAL;
}
}

void
DLCheckFunc::usage(ostream& o) {
	o << name << " <funcs ...>" << endl;
	o <<
"Reports whether or not each function is already bound to a symbol from a \n"
"loaded dynamic shared library or module.  Does not error out.\n"
"See also `dlassertfunc\'."
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS(DLAssertFunc, "dlassertfunc", 
	"assert-fail if a nonmeta function symbol is unbound")

int
DLAssertFunc::main(const string_list& a) {
if (a.size() < 2) {
	usage(cerr << "usage: ");
	return command_error_codes::SYNTAX;
} else {
	bool good = true;
	string_list::const_iterator i(++a.begin()), e(a.end());
	for ( ; i!=e; ++i) {
		if (entity::lookup_chpsim_function(*i)) {
			cout << "function `" << *i << "\': bound." << endl;
		} else {
			cout << "function `" << *i << "\': unbound." << endl;
			good = false;
		}
	}
	return good ? command_error_codes::NORMAL : command_error_codes::BADARG;
}
}

void
DLAssertFunc::usage(ostream& o) {
	o << name << " <funcs ...>" << endl;
	o <<
"Reports whether or not each function is already bound to a symbol from a \n"
"loaded dynamic shared library or module.  Errors out if any are unbound.\n"
"See also `dlcheckfunc\'."
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS(DLFuncs, "dlfuncs", 
	"List names of all dynamically bound functions")

int
DLFuncs::main(const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return command_error_codes::SYNTAX;
} else {
	entity::list_chpsim_functions(cout);
	return command_error_codes::NORMAL;
}
}

void
DLFuncs::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
}

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC
