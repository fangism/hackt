/**
	\file "util/stacktrace.cc"
	Implementation of stacktrace class.
	$Id: stacktrace.cc,v 1.13 2006/02/13 02:48:07 fang Exp $
 */

// ENABLE_STACKTRACE is forced for this module, regardless of pre-definitions!
#define	ENABLE_STACKTRACE	1
// #define	ENABLE_STATIC_TRACE	1

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include "config.h"

#include "util/pthread.h"
#include <iostream>
#include <iterator>
#include <string>
#include <stack>
#include <list>

#include "util/memory/count_ptr.tcc"
#include "util/stacktrace.h"
#include "util/likely.h"
#include "util/qmap.tcc"

namespace util {
using std::list;
using std::ostream;
using util::qmap;
using std::stack;
using std::ostream_iterator;
#include "util/using_ostream.h"
using util::memory::raw_count_ptr;

// macro to guarantee proper orderly initialization
REQUIRES_STACKTRACE_STATIC_INIT

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
	typedef	stacktrace::stack_text_type	stack_text_type;
	typedef	stacktrace::stack_echo_type	stack_echo_type;
	typedef	stacktrace::stack_streams_type	stack_streams_type;

	static raw_count_ptr<stack_text_type>	get_stack_text(void);
	static raw_count_ptr<stack_text_type>	get_stack_indent(void);
	static raw_count_ptr<stack_echo_type>	get_stack_echo(void);
	static raw_count_ptr<stack_streams_type>	get_stack_streams(void);

private:
	manager() {
		// initialization moved to static initialization below.
	}

	~manager() { }

public:
	static
	ostream&
	print_auto_indent(ostream& o) {
		// static? no, get fresh copy!
		static const stack_text_type&
			si(*manager::get_stack_indent());
		INVARIANT(o.good());
		ostream_iterator<string> osi(o);
		copy(si.begin(), si.end(), osi);
		return o;
	}

};	// end class stacktrace::manager

//-----------------------------------------------------------------------------
// static construction

raw_count_ptr<stacktrace::stack_text_type>
stacktrace::manager::get_stack_text(void) {
	typedef	raw_count_ptr<stacktrace::stack_text_type>	return_type;
	static stack_text_type* ptr = new stack_text_type;
	STATIC_RC_POOL_REF_INIT;
	static size_t* const count = NEW_SIZE_T;
	static const size_t zero __ATTRIBUTE_UNUSED__ = (*count = 0);
	static const int check __ATTRIBUTE_UNUSED__ =
		(INVARIANT(ptr->empty()), 0);
	return return_type(ptr, count);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

raw_count_ptr<stacktrace::stack_text_type>
stacktrace::manager::get_stack_indent(void) {
	typedef	raw_count_ptr<stacktrace::stack_text_type>	return_type;
	static stack_text_type* const ptr = new stack_text_type;
	STATIC_RC_POOL_REF_INIT;
	static size_t* const count = NEW_SIZE_T;
	static const size_t zero __ATTRIBUTE_UNUSED__ = (*count = 0);
	static const int check __ATTRIBUTE_UNUSED__ =
		(INVARIANT(ptr->empty()), INVARIANT(!ptr->size()), 0);
	return return_type(ptr, count);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Push 1 to initialize enabled, 0 to disable initially.  
 */
raw_count_ptr<stacktrace::stack_echo_type>
stacktrace::manager::get_stack_echo(void) {
	typedef	raw_count_ptr<stacktrace::stack_echo_type>	return_type;
	static stack_echo_type* stack_echo = new stack_echo_type;
	static const int init_once __ATTRIBUTE_UNUSED__
		= (stack_echo->push(1), 1);
	STATIC_RC_POOL_REF_INIT;
	static size_t* const count = NEW_SIZE_T;
	static const size_t zero __ATTRIBUTE_UNUSED__ = (*count = 0);
	static const return_type ret(stack_echo, count);
	INVARIANT(init_once && !stack_echo->empty());
	return return_type(stack_echo, count);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Defaults to stderr.  
 */
raw_count_ptr<stacktrace::stack_streams_type>
stacktrace::manager::get_stack_streams(void) {
	typedef	raw_count_ptr<stacktrace::stack_streams_type>	return_type;
	static stack_streams_type* stack_streams = new stack_streams_type;
	static const int init_once __ATTRIBUTE_UNUSED__
		= (stack_streams->push(&cerr), 1);
	STATIC_RC_POOL_REF_INIT;
	static size_t* const count = NEW_SIZE_T;
	static const size_t zero __ATTRIBUTE_UNUSED__ = (*count = 0);
	static const return_type ret(stack_streams, count);
	INVARIANT(init_once && !stack_streams->empty());
	return return_type(stack_streams, count);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Stacktrace stream manipulator.  
 */
const stacktrace::indent	stacktrace_auto_indent = stacktrace::indent();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Uses the stacktrace's position to automatically indent.  
 */
ostream&
operator << (ostream& o, const stacktrace::indent&) {
	// need static initializers?
	return stacktrace::manager::print_auto_indent(o) << ":  ";
}

//=============================================================================
// class stacktrace method definitions

stacktrace::stacktrace(const string& s) {
	// cannot use string (without ref-count) because it may be destroyed
	// prematurely during static destruction, char* is robust and permanent.
	static const char* const
		default_stack_indent_string = "| ";	// permanent
	// must be static or else, new ref_counts will be locally released
	static stack_text_type& stack_text(*manager::get_stack_text());
	static stack_text_type& stack_indent(*manager::get_stack_indent());
	static const stack_echo_type& stack_echo(*manager::get_stack_echo());
	static const stack_streams_type&
		stack_streams(*manager::get_stack_streams());

	stack_text.push_back(s);
	if (stack_echo.top()) {
		ostream& os(*stack_streams.top());
		if (LIKELY(os.good())) {
			manager::print_auto_indent(os) << "\\- " <<
				stack_text.back() << endl;
		} else {
#if 0
			// probably near end of program termination
			// can't print any more diagnostic messages!
			INVARIANT(std::cin.good());
			char c;
			std::cin >> c;			// pause
#endif
		}
	}
	stack_indent.push_back(default_stack_indent_string);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
stacktrace::~stacktrace() {
	static stack_text_type& stack_text(*manager::get_stack_text());
	static stack_text_type& stack_indent(*manager::get_stack_indent());
	static const stack_echo_type& stack_echo(*manager::get_stack_echo());
	static const stack_streams_type&
		stack_streams(*manager::get_stack_streams());

	stack_indent.pop_back();
	if (stack_echo.top()) {
		ostream& os(*stack_streams.top());
		if (LIKELY(os.good())) {
			manager::print_auto_indent(os) << "/- " <<
				stack_text.back() << endl;
		} else {
#if 0
			INVARIANT(std::cin.good());
			char c;
			std::cin >> c;			// pause
#endif
		}
	}
	stack_text.pop_back();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns reference to the current stacktrace output stream.
 */
ostream&
stacktrace::stream(void) {
	static const stack_streams_type&
		stack_streams(*manager::get_stack_streams());
	return *stack_streams.top();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
stacktrace::init_token&
stacktrace::require_static_init(void) {
	static init_token tok;
	tok.check();
	return tok;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
stacktrace::full_dump(void) {
	static const stack_text_type&
		stack_text(*manager::get_stack_text());
	ostream& current_stream(stream());
	ostream_iterator<string> osi(current_stream, "\n");
	copy(stack_text.begin(), stack_text.end(), osi);
	current_stream << endl;
}

//=============================================================================
// class stacktrace::init_token method definitions

/**
	This will hold onto a reference count, intended to be kept
	per-module that requires static initialization of stacktrace.  
 */
stacktrace::init_token::init_token() :
		stack_text_ref(manager::get_stack_text()), 
		stack_indent_ref(manager::get_stack_text()), 
		stack_echo_ref(manager::get_stack_echo()), 
		stack_streams_ref(manager::get_stack_streams()) {
	check();
}

/**
	When the last init_token is destroyed (among all modules)
 */
stacktrace::init_token::~init_token() {
	check();
}

void
stacktrace::init_token::check(void) const {
	NEVER_NULL(stack_text_ref);
	NEVER_NULL(stack_indent_ref);
	NEVER_NULL(stack_echo_ref);
	NEVER_NULL(stack_streams_ref);
}

//=============================================================================
// struct stacktrace::echo method definitions

stacktrace::echo::echo(const int i) {
	static stack_echo_type&
		stack_echo(*manager::get_stack_echo());
	stack_echo.push(i);
}

stacktrace::echo::~echo() {
	static stack_echo_type&
		stack_echo(*manager::get_stack_echo());
	stack_echo.pop();
}

//=============================================================================
// struct redirect_stacktrace method definitions

stacktrace::redirect::redirect(ostream& o) {
	static stack_streams_type&
		stack_streams(*manager::get_stack_streams());
	stack_streams.push(&o);
}

stacktrace::redirect::~redirect() {
	static stack_streams_type&
		stack_streams(*manager::get_stack_streams());
	stack_streams.pop();
}

//=============================================================================
}	// end namespace util

DEFAULT_STATIC_TRACE_END

