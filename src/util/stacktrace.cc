/**
	\file "stacktrace.cc"
	Implementation of stacktrace class.
	$Id: stacktrace.cc,v 1.5.4.1.2.2 2005/01/24 20:51:40 fang Exp $
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

namespace util {
USING_LIST
using QMAP_NAMESPACE::qmap;
using std::stack;
using std::ostream_iterator;
#include "using_ostream.h"
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
#if 0
	/// the type of stack used to hold feedback text
	typedef	list<string>	stack_text_type;
	/// the type of stack used to track on/off mode
	typedef	stack<int>	stack_echo_type;
	/// the type of stack used to track stream redirections
	typedef	stack<ostream*>	stack_streams_type;

	static stack_text_type&			get_stack_text(void);
	static stack_text_type&			get_stack_indent(void);
	static stack_echo_type&			get_stack_echo(void);
	static stack_streams_type&		get_stack_streams(void);
#else
	typedef	stacktrace::stack_text_type	stack_text_type;
	typedef	stacktrace::stack_echo_type	stack_echo_type;
	typedef	stacktrace::stack_streams_type	stack_streams_type;

	static raw_count_ptr<stack_text_type>	get_stack_text(void);
	static raw_count_ptr<stack_text_type>	get_stack_indent(void);
	static raw_count_ptr<stack_echo_type>	get_stack_echo(void);
	static raw_count_ptr<stack_streams_type>	get_stack_streams(void);

#endif
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
		static const raw_count_ptr<const stack_text_type>
			si(manager::get_stack_indent());
		NEVER_NULL(si);
		INVARIANT(si.refs() < 10000);
		INVARIANT(o.good());
#if 0
		ostream_iterator<string> osi(o);
		copy(si.begin(), si.end(), osi);
#else
//		cerr << si->size() << endl;
//		cerr << "HELLO" << endl;
		stack_text_type::const_iterator i = si->begin();
		const stack_text_type::const_iterator e = si->end();
		for ( ; i!=e; i++) {
//			cerr << "YO @ " << &*i << endl;
			o << *i;
//			cerr << " HO " << endl;
		}
#endif
		return o;
	}

};	// end class stacktrace::manager

//-----------------------------------------------------------------------------
// static construction

#if 0
stacktrace::manager::stack_text_type&
stacktrace::manager::get_stack_text(void) {
	static stack_text_type stack_text;
	return stack_text;
}
#else
raw_count_ptr<stacktrace::stack_text_type>
stacktrace::manager::get_stack_text(void) {
	typedef	raw_count_ptr<stacktrace::stack_text_type>	return_type;
	static stack_text_type* ptr = new stack_text_type;
	static size_t* count = new size_t(0);
	static const int check = (INVARIANT(ptr->empty()), 0);
#if 1
	static const return_type do_not_return(ptr, count);
	// gotta make sure the consumer doesn't just delete the reference
	// right away!
	NEVER_NULL(ptr);
	NEVER_NULL(count);
	INVARIANT(*count < 10000);
	return_type return_me(ptr, count);
	NEVER_NULL(return_me);
	return return_me;
//	return return_type(ptr, count);
#else
	static const return_type ret(ptr, count);
	NEVER_NULL(ptr);
	NEVER_NULL(count);
	INVARIANT(*count < 10000);
#if 0
	NEVER_NULL(ret);
	return ret;
#else
	// might have been destroyed during termination!
	return (ret) ? ret : return_type(ptr, count);
#endif
#endif
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#if 0
stacktrace::manager::stack_text_type&
stacktrace::manager::get_stack_indent(void) {
	static stack_text_type stack_indent;
	return stack_indent;
}
#else
raw_count_ptr<stacktrace::stack_text_type>
stacktrace::manager::get_stack_indent(void) {
	typedef	raw_count_ptr<stacktrace::stack_text_type>	return_type;
	static stack_text_type* const ptr = new stack_text_type;
	static size_t* const count = new size_t(0);
	static const int check =
		(INVARIANT(ptr->empty()), INVARIANT(!ptr->size()), 0);
	static int visits = 0;
	visits++;
#if 1
	static const return_type do_not_return(ptr, count);
	NEVER_NULL(ptr);
	NEVER_NULL(count);
#if 0
	cerr << "get_stack_indent() visits = " << visits <<
		", ptr = " << ptr <<
		", count = " << count <<
		", *count = " << *count << endl;
#endif
	INVARIANT(*count < 10000);
	// TODO: FANG, FIX ME!!!!!!!!!
	// this invariant failing means: we aren't properly managing the
	// memory for the reference count itself, 
	// it is being released and clobbered!
	// perhaps revisit count_ptr_test and extend it some more...
	const return_type return_me(ptr, count);
	return return_me;
//	return return_type(ptr, count);
#else
	// DILEMMA: could be locally destroyed at any time, but function
	// must always return a valid pointer!
	// local static return_type not guaranteed to work...

	// need to keep local copy, else it will be deleted right away!
	static const return_type ret(ptr, count);
	// could be prematurely destroyed!!!
	NEVER_NULL(ptr);
	NEVER_NULL(count);
	INVARIANT(*count);	// only if there is a local static
	// might have been destroyed during termination!
	if (ret) {
		INVARIANT(ret.refs() < 10000);
		cerr << "stack_indent = " << &*ret <<
			", &count = " << count <<
			", count = " << ret.refs() << endl;
	} else {
		cerr << "ptr = " << ptr <<
			", count = " << count <<
			", *count = " << *count << endl;
	}
	INVARIANT(*count < 10000);
	return (ret) ? ret : return_type(ptr, count);
	// just because it's destroyed here, doesn't invalidate ptr
#endif
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Push 1 to initialize enabled, 0 to disable initially.  
 */
#if 0
stacktrace::manager::stack_echo_type&
stacktrace::manager::get_stack_echo(void) {
	static stack_echo_type stack_echo;
	static const int init_once = (stack_echo.push(1), 1);
	INVARIANT(init_once && !stack_echo.empty());
	return stack_echo;
}
#else
raw_count_ptr<stacktrace::stack_echo_type>
stacktrace::manager::get_stack_echo(void) {
	typedef	raw_count_ptr<stacktrace::stack_echo_type>	return_type;
	static stack_echo_type* stack_echo = new stack_echo_type;
	static const int init_once = (stack_echo->push(1), 1);
	static size_t* count = new size_t(0);
	static const return_type ret(stack_echo, count);
	INVARIANT(init_once && !stack_echo->empty());
#if 0
	NEVER_NULL(ret);
	return ret;
#else
	NEVER_NULL(stack_echo);
	NEVER_NULL(count);
	INVARIANT(*count < 10000);		// because there is ret
	// might have been destroyed during termination!
	return (ret) ? ret : return_type(stack_echo, count);
#endif
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Defaults to stderr.  
 */
#if 0
stacktrace::manager::stack_streams_type&
stacktrace::manager::get_stack_streams(void) {
	static stack_streams_type stack_streams;
	static const int init_once = (stack_streams.push(&cerr), 1);
	INVARIANT(init_once && !stack_streams.empty());
	return stack_streams;
}
#else
raw_count_ptr<stacktrace::stack_streams_type>
stacktrace::manager::get_stack_streams(void) {
	typedef	raw_count_ptr<stacktrace::stack_streams_type>	return_type;
	static stack_streams_type* stack_streams = new stack_streams_type;
	static const int init_once = (stack_streams->push(&cerr), 1);
	static size_t* count = new size_t(0);
	static const return_type ret(stack_streams, count);
	INVARIANT(init_once && !stack_streams->empty());
#if 0
	NEVER_NULL(ret);
	return ret;
#else
	NEVER_NULL(stack_streams);
	NEVER_NULL(count);
	INVARIANT(*count < 10000);		// because there is ret
	// might have been destroyed during termination!
	return (ret) ? ret : return_type(stack_streams, count);
#endif
}
#endif

//=============================================================================
// class stacktrace method definitions

stacktrace::stacktrace(const string& s) : local_str(s) {
	// if this appears outside in global static scope, 
	// it may not be initialized in time!
#if 0
	static const string default_stack_indent_string("| ");
#else
	const char* const
		default_stack_indent_string = "| ";	// permanent
#endif
#if 0
	static stack_text_type&
		stack_text(*manager::get_stack_text());
	static stack_text_type&
		stack_indent(*manager::get_stack_indent());
	static const stack_echo_type&
		stack_echo(*manager::get_stack_echo());
	static const stack_streams_type&
		stack_streams(*manager::get_stack_streams());
#else
	// must be static or else, new ref_counts will be locally released
	static const raw_count_ptr<stack_text_type>
		stack_indent(manager::get_stack_indent());
	static const raw_count_ptr<const stack_echo_type>
		stack_echo(manager::get_stack_echo());
	static const raw_count_ptr<const stack_streams_type>
		stack_streams(manager::get_stack_streams());
	// fresh copy?
	static const raw_count_ptr<stack_text_type>
		stack_text(manager::get_stack_text());
#if 0
	static int visits = 0;
	visits++;
	cerr << "stacktrace() visits = " << visits << endl;
#endif
	NEVER_NULL(&s);
	NEVER_NULL(stack_text);
#if 0
	if (!stack_indent) {
		// probably because module is destroyed, 
		// along with function-local statics
		stack_indent = manager::get_stack_indent();
	}
#endif
	NEVER_NULL(stack_indent);	// sometimes fails!?!?
	NEVER_NULL(stack_echo);
	NEVER_NULL(stack_streams);
	INVARIANT(stack_indent.refs() < 10000);
#endif

//	cerr << "stacktrace::stacktrace() with " << s << endl;
#if 0
	stack_text.push_back(s);
#else
	stack_text->push_back(local_str);
#endif
#if 0
	cerr << "size = " << stack_text.size() << endl;	// DEATH
	cerr << &stack_text.back() << endl;
	cerr << stack_text.back() << endl;
	cerr << &stack_text.front() << endl;
	cerr << stack_text.front() << endl;
	cerr << "level = " << stack_indent.size() << endl;
#endif
	if (stack_echo->top()) {
		ostream& os(*stack_streams->top());
		if (os.good()) {
#if 0
			cerr << &stack_text.back() << endl;
			cerr << stack_text.back() << endl;
#endif
			manager::print_auto_indent(os) << "\\- " <<
				stack_text->back() << endl;
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
	stack_indent->push_back(default_stack_indent_string);
	INVARIANT(stack_indent.refs() < 10000);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
stacktrace::~stacktrace() {
#if 0
	static stack_text_type&
		stack_text(*manager::get_stack_text());
	static stack_text_type&
		stack_indent(*manager::get_stack_indent());
	static const stack_echo_type&
		stack_echo(*manager::get_stack_echo());
	static const stack_streams_type&
		stack_streams(*manager::get_stack_streams());
#else
	static const raw_count_ptr<stack_text_type>
		stack_text(manager::get_stack_text());
	static const raw_count_ptr<stack_text_type>
		stack_indent(manager::get_stack_indent());
	static const raw_count_ptr<const stack_echo_type>
		stack_echo(manager::get_stack_echo());
	static const raw_count_ptr<const stack_streams_type>
		stack_streams(manager::get_stack_streams());
	NEVER_NULL(stack_text);
	NEVER_NULL(stack_indent);
	NEVER_NULL(stack_echo);
	NEVER_NULL(stack_streams);
	INVARIANT(stack_indent.refs() < 10000);
#endif
//	cerr << stack_text->back() << endl;	// bad text

	stack_indent->pop_back();
	if (stack_echo->top()) {
		ostream& os(*stack_streams->top());
		if (os.good()) {
			manager::print_auto_indent(os) << "/- " <<
				stack_text->back() << endl;
		} else {
#if 0
			INVARIANT(std::cin.good());
			char c;
			std::cin >> c;			// pause
#endif
		}
	}
	stack_text->pop_back();
	INVARIANT(stack_indent.refs() < 10000);
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
#if 0
	STACKTRACE_VERBOSE;
	cerr << __PRETTY_FUNCTION__ << endl;
	cerr << tok.stack_text_ref.refs() << endl;
#endif
	tok.check();
	return tok;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
stacktrace::full_dump(void) {
	static const raw_count_ptr<stack_text_type>
		stack_text(manager::get_stack_text());
	NEVER_NULL(stack_text);
	ostream& current_stream(stream());
	ostream_iterator<string> osi(current_stream, "\n");
	copy(stack_text->begin(), stack_text->end(), osi);
	current_stream << endl;
}

//=============================================================================
// class stacktrace::init_token method definitions

stacktrace::init_token::init_token() :
		stack_text_ref(manager::get_stack_text()), 
		stack_indent_ref(manager::get_stack_text()), 
		stack_echo_ref(manager::get_stack_echo()), 
		stack_streams_ref(manager::get_stack_streams()) {
//	STACKTRACE("init_token()");
	check();
}

stacktrace::init_token::~init_token() {
//	STACKTRACE("~init_token()");
	check();
}

void
stacktrace::init_token::check(void) const {
//	STACKTRACE("init_token::check()");
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

