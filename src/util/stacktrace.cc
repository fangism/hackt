/**
	\file "stacktrace.cc"
	Implementation of stacktrace class.
	$Id: stacktrace.cc,v 1.1 2004/12/06 07:13:02 fang Exp $
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
class stacktrace_manager {
friend class stacktrace;
friend class enable_stacktrace;
friend class redirect_stacktrace;
public:
	typedef	list<string>	stack_text_type;
private:

	static
	stack_text_type		stack_text;

	// stream-independent indentation
	static
	stack_text_type		stack_indent;

	static
	stack<int>		stack_enable;

	static
	stack<ostream*>		stack_streams;

public:
	stacktrace_manager() {
		// must guarantee that they aren't empty
		stack_enable.push(0);
		stack_streams.push(&cerr);
	}

	~stacktrace_manager() { }

	static
	ostream&
	print_auto_indent(ostream& o) {
		ostream_iterator<string> osi(o);
		copy(stack_indent.begin(), stack_indent.end(), osi);
		return o;
	}

};	// end class stacktrace_manager

//-----------------------------------------------------------------------------
// static construction
stacktrace_manager::stack_text_type
stacktrace_manager::stack_text;

stacktrace_manager::stack_text_type
stacktrace_manager::stack_indent;

stack<int>
stacktrace_manager::stack_enable;

stack<ostream*>
stacktrace_manager::stack_streams;

static
stacktrace_manager		the_stacktrace_manager;

static
const string
default_stack_indent_string("| ");

//=============================================================================
// class stacktrace method definitions

stacktrace::stacktrace(const char* s) {
	stacktrace_manager::stack_text.push_back(s);
	if (stacktrace_manager::stack_enable.top())
		stacktrace_manager::print_auto_indent(
			*stacktrace_manager::stack_streams.top())
			<< "enter: " << stacktrace_manager::stack_text.back()
			<< endl;
	stacktrace_manager::stack_indent.push_back(default_stack_indent_string);
}

stacktrace::stacktrace(const string& s) {
	stacktrace_manager::stack_text.push_back(s);
	if (stacktrace_manager::stack_enable.top())
		stacktrace_manager::print_auto_indent(
			*stacktrace_manager::stack_streams.top())
			<< "enter: " << stacktrace_manager::stack_text.back()
			<< endl;
	stacktrace_manager::stack_indent.push_back(default_stack_indent_string);
}

stacktrace::~stacktrace() {
	stacktrace_manager::stack_indent.pop_back();
	if (stacktrace_manager::stack_enable.top())
		stacktrace_manager::print_auto_indent(
			*stacktrace_manager::stack_streams.top())
			<< "leave: " << stacktrace_manager::stack_text.back()
			<< endl;
	stacktrace_manager::stack_text.pop_back();
}

void
stacktrace::full_dump(void) {
	ostream_iterator<string>
		osi(*stacktrace_manager::stack_streams.top(), "\n");
	copy(stacktrace_manager::stack_text.begin(),
		stacktrace_manager::stack_text.end(), osi);
	(*stacktrace_manager::stack_streams.top()) << endl;
}

//=============================================================================
// struct enable_stacktrace method definitions

enable_stacktrace::enable_stacktrace(const int i) {
	stacktrace_manager::stack_enable.push(i);
}

enable_stacktrace::~enable_stacktrace() {
	stacktrace_manager::stack_enable.pop();
}

//=============================================================================
// struct redirect_stacktrace method definitions

redirect_stacktrace::redirect_stacktrace(ostream& o) {
	stacktrace_manager::stack_streams.push(&o);
}

redirect_stacktrace::~redirect_stacktrace() {
	stacktrace_manager::stack_streams.pop();
}

//=============================================================================
}	// end namespace util

