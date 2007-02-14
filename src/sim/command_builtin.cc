/**
	\file "sim/command_builtin.cc"
	NOTE: this file should NOT contain any global static initializations
	because their initialization ordering is not defined between TUs.  
	$Id: command_builtin.cc,v 1.3 2007/02/14 04:57:24 fang Exp $
 */

#include "sim/command_builtin.h"
#include "sim/command_base.h"
#include <list>
#include <string>
#include <iostream>
#include <iterator>
#include <algorithm>
#include "util/macros.h"

namespace HAC {
namespace SIM {
using std::copy;
using std::ostream_iterator;
#include "util/using_ostream.h"

//=============================================================================
// class Echo method definitions


int
Echo::main(const string_list& args) {
	INVARIANT(!args.empty());
	ostream_iterator<string> osi(cout, " ");
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

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

