/**
	\file "stacktrace.cc"
	Implementation of stacktrace class.
	$Id: stacktrace.cc,v 1.2 2005/01/08 08:30:53 fang Exp $
 */

#include <pthread.h>
#include <iostream>
#include <iterator>
#include <string>

#include "stacktrace.h"
#include "STL/list.tcc"
#include "qmap.tcc"

namespace util {
USING_LIST
using QMAP_NAMESPACE::qmap;
using std::stack;
using std::string;
using std::ostream_iterator;
#include "using_ostream.h"

//=============================================================================
// eventually for threaded applications, use a map of stacks...

/**
	Private implementation class, not visible to other modules.  
	Only written as a class for convenient static initialization.  
 */
class stacktrace::manager {
friend class stacktrace;
friend class stacktrace::echo;
friend class stacktrace::redirect;
public:
	typedef	list<string>	stack_text_type;
private:

	static
	stack_text_type		stack_text;

	// stream-independent indentation
	static
	stack_text_type		stack_indent;

	static
	stack<int>		stack_echo;

	static
	stack<ostream*>		stack_streams;

private:
	manager() {
		// must guarantee that they aren't empty
		stack_echo.push(0);
		stack_streams.push(&cerr);
	}

	~manager() { }

public:
	static
	ostream&
	print_auto_indent(ostream& o) {
		ostream_iterator<string> osi(o);
		copy(stack_indent.begin(), stack_indent.end(), osi);
		return o;
	}

};	// end class stacktrace::manager

//-----------------------------------------------------------------------------
// static construction
stacktrace::manager::stack_text_type
stacktrace::manager::stack_text;

stacktrace::manager::stack_text_type
stacktrace::manager::stack_indent;

stack<int>
stacktrace::manager::stack_echo;

stack<ostream*>
stacktrace::manager::stack_streams;

stacktrace::manager
stacktrace::the_manager;

static
const string
default_stack_indent_string("| ");

//=============================================================================
// class stacktrace method definitions

stacktrace::stacktrace(const char* s) {
	stacktrace::manager::stack_text.push_back(s);
	if (stacktrace::manager::stack_echo.top())
		stacktrace::manager::print_auto_indent(
			*stacktrace::manager::stack_streams.top())
			<< "enter: " << stacktrace::manager::stack_text.back()
			<< endl;
	stacktrace::manager::stack_indent.push_back(default_stack_indent_string);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
stacktrace::stacktrace(const string& s) {
	stacktrace::manager::stack_text.push_back(s);
	if (stacktrace::manager::stack_echo.top())
		stacktrace::manager::print_auto_indent(
			*stacktrace::manager::stack_streams.top())
			<< "enter: " << stacktrace::manager::stack_text.back()
			<< endl;
	stacktrace::manager::stack_indent.push_back(default_stack_indent_string);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
stacktrace::~stacktrace() {
	stacktrace::manager::stack_indent.pop_back();
	if (stacktrace::manager::stack_echo.top())
		stacktrace::manager::print_auto_indent(
			*stacktrace::manager::stack_streams.top())
			<< "leave: " << stacktrace::manager::stack_text.back()
			<< endl;
	stacktrace::manager::stack_text.pop_back();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
stacktrace::full_dump(void) {
	ostream_iterator<string>
		osi(*stacktrace::manager::stack_streams.top(), "\n");
	copy(stacktrace::manager::stack_text.begin(),
		stacktrace::manager::stack_text.end(), osi);
	(*stacktrace::manager::stack_streams.top()) << endl;
}

//=============================================================================
// struct stacktrace::echo method definitions

stacktrace::echo::echo(const int i) {
	stacktrace::manager::stack_echo.push(i);
}

stacktrace::echo::~echo() {
	stacktrace::manager::stack_echo.pop();
}

//=============================================================================
// struct redirect_stacktrace method definitions

stacktrace::redirect::redirect(ostream& o) {
	stacktrace::manager::stack_streams.push(&o);
}

stacktrace::redirect::~redirect() {
	stacktrace::manager::stack_streams.pop();
}

//=============================================================================
}	// end namespace util

