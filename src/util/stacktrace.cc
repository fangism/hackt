/**
	\file "stacktrace.cc"
	Implementation of stacktrace class.
	$Id: stacktrace.cc,v 1.5 2005/01/15 06:20:46 fang Exp $
 */

// ENABLE_STACKTRACE is forced for this module, regardless of pre-definitions!
#define	ENABLE_STACKTRACE	1

#include <pthread.h>
#include <iostream>
#include <iterator>
#include <string>
#include <stack>

#include "stacktrace.h"
#include "likely.h"
#include "STL/list.tcc"
#include "qmap.tcc"
#include "memory/pointer_classes.h"

namespace util {
USING_LIST
using QMAP_NAMESPACE::qmap;
using std::stack;
using std::ostream_iterator;
#include "using_ostream.h"
using memory::excl_ptr;

//=============================================================================
// eventually for threaded applications, use a map of stacks...

/**
	Private implementation class, not visible to other modules.  
	Only written as a class for convenient static initialization.  
	To be able to trace function calls that occur during static 
	initialization, we must guarantee that the manager's static 
	objects are initialized first!  Global initialization ordering is
	generally non-trivial, so resort to the techinique of 
	interfacing through reference functions which will guarantee 
	a one-time initialization upon first invocation.  
	(This technique is also used in util::persistent_object_manager.)
 */
class stacktrace::manager {
public:
	/// the type of stack used to hold feedback text
	typedef	list<string>	stack_text_type;
	/// the type of stack used to track on/off mode
	typedef	stack<int>	stack_echo_type;
	/// the type of stack used to track stream redirections
	typedef	stack<ostream*>	stack_streams_type;
private:
	static stack_text_type*			stack_text;
	static excl_ptr<stack_text_type>	stack_text_ptr;
public:
	static stack_text_type&			get_stack_text(void);

private:
	// stream-independent indentation
	static stack_text_type*			stack_indent;
	static excl_ptr<stack_text_type>	stack_indent_ptr;
public:
	static stack_text_type&			get_stack_indent(void);

private:
	static stack_echo_type*			stack_echo;
	static excl_ptr<stack_echo_type>	stack_echo_ptr;
public:
	static stack_echo_type&			get_stack_echo(void);

private:
	static stack_streams_type*		stack_streams;
	static excl_ptr<stack_streams_type>	stack_streams_ptr;
public:
	static stack_streams_type&		get_stack_streams(void);

private:
	manager() {
		// initialization moved to static initialization below.
	}

	~manager() { }

public:
	static
	ostream&
	print_auto_indent(ostream& o) {
		const stack_text_type& si(get_stack_indent());
		ostream_iterator<string> osi(o);
		copy(si.begin(), si.end(), osi);
		return o;
	}

};	// end class stacktrace::manager

//-----------------------------------------------------------------------------
// static construction

stacktrace::manager::stack_text_type*
stacktrace::manager::stack_text = NULL;

excl_ptr<stacktrace::manager::stack_text_type>
stacktrace::manager::stack_text_ptr(NULL);

stacktrace::manager::stack_text_type&
stacktrace::manager::get_stack_text(void) {
	if (UNLIKELY(!stack_text)) {
		stack_text = new stack_text_type;
		stack_text_ptr = excl_ptr<stack_text_type>(stack_text);
	}
	NEVER_NULL(stack_text);
	return *stack_text;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

stacktrace::manager::stack_text_type*
stacktrace::manager::stack_indent = NULL;

excl_ptr<stacktrace::manager::stack_text_type>
stacktrace::manager::stack_indent_ptr(NULL);

stacktrace::manager::stack_text_type&
stacktrace::manager::get_stack_indent(void) {
	if (UNLIKELY(!stack_indent)) {
		stack_indent = new stack_text_type;
		stack_indent_ptr = excl_ptr<stack_text_type>(stack_indent);
	}
	NEVER_NULL(stack_indent);
	return *stack_indent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

stacktrace::manager::stack_echo_type*
stacktrace::manager::stack_echo = NULL;

excl_ptr<stacktrace::manager::stack_echo_type>
stacktrace::manager::stack_echo_ptr(NULL);

/**
	Push 1 to initialize enabled, 0 to disable initially.  
 */
stacktrace::manager::stack_echo_type&
stacktrace::manager::get_stack_echo(void) {
	if (UNLIKELY(!stack_echo)) {
		stack_echo = new stack_echo_type;
		stack_echo_ptr = excl_ptr<stack_echo_type>(stack_echo);
		stack_echo->push(1);
	}
	NEVER_NULL(stack_echo);
	return *stack_echo;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

stacktrace::manager::stack_streams_type*
stacktrace::manager::stack_streams = NULL;

excl_ptr<stacktrace::manager::stack_streams_type>
stacktrace::manager::stack_streams_ptr(NULL);

/**
	Defaults to stderr.  
 */
stacktrace::manager::stack_streams_type&
stacktrace::manager::get_stack_streams(void) {
	if (UNLIKELY(!stack_streams)) {
		stack_streams = new stack_streams_type;
		stack_streams_ptr = excl_ptr<stack_streams_type>(stack_streams);
		stack_streams->push(&cerr);
	}
	NEVER_NULL(stack_streams);
	return *stack_streams;
}

//=============================================================================
// class stacktrace method definitions

stacktrace::stacktrace(const string& s) {
	// if this appears outside in global static scope, 
	// it may not be initialized in time!
	static const string default_stack_indent_string("| ");
	stacktrace::manager::get_stack_text().push_back(s);
	if (stacktrace::manager::get_stack_echo().top())
		stacktrace::manager::print_auto_indent(
			*stacktrace::manager::get_stack_streams().top()) <<
			"enter: " <<
			stacktrace::manager::get_stack_text().back() << endl;
	stacktrace::manager::get_stack_indent().push_back(
		default_stack_indent_string);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
stacktrace::~stacktrace() {
	stacktrace::manager::get_stack_indent().pop_back();
	if (stacktrace::manager::get_stack_echo().top())
		stacktrace::manager::print_auto_indent(
			*stacktrace::manager::get_stack_streams().top()) <<
			"leave: " <<
			stacktrace::manager::get_stack_text().back() << endl;
	stacktrace::manager::get_stack_text().pop_back();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns reference to the current stacktrace output stream.
 */
ostream&
stacktrace::stream(void) {
	return *stacktrace::manager::get_stack_streams().top();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
stacktrace::full_dump(void) {
	ostream_iterator<string>
		osi(*stacktrace::manager::get_stack_streams().top(), "\n");
	copy(stacktrace::manager::get_stack_text().begin(),
		stacktrace::manager::get_stack_text().end(), osi);
	(*stacktrace::manager::get_stack_streams().top()) << endl;
}

//=============================================================================
// struct stacktrace::echo method definitions

stacktrace::echo::echo(const int i) {
	stacktrace::manager::get_stack_echo().push(i);
}

stacktrace::echo::~echo() {
	stacktrace::manager::get_stack_echo().pop();
}

//=============================================================================
// struct redirect_stacktrace method definitions

stacktrace::redirect::redirect(ostream& o) {
	stacktrace::manager::get_stack_streams().push(&o);
}

stacktrace::redirect::~redirect() {
	stacktrace::manager::get_stack_streams().pop();
}

//=============================================================================
}	// end namespace util

