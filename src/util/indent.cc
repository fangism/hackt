/**
	\file "indent.cc"
	Source for indentation manager.  

	$Id: indent.cc,v 1.1 2004/12/01 05:16:17 fang Exp $
 */

#include <iostream>
#include <list>
#include <algorithm>
#include <stack>

#include "hash_map.h"
#include "hash_specializations.h"
#include "indent.h"

namespace util {
using HASH_MAP_NAMESPACE::hash_map;
using std::list;
using std::for_each;
using std::stack;

//=============================================================================
/**
	Private implementation class, visible only to this module.  
 */
class indent_string {
public:
	static const string             default_indent_string;
private:
	string  	indenter;	///< the default string to indent
	list<string>	indent_stack;	///< the entire indentation stack
	/**
		Stack of status flags allows indentation to be
		enabled or disabled in scopes.  
	 */
	stack<int>	status_stack;
public:
	indent_string(const string& s = default_indent_string) :
		indenter(s), indent_stack(), status_stack() {
		status_stack.push(1);	// on by default
	}

	// default destructor suffices

	void
	enable(void) { status_stack.push(1); }

	void
	disable(void) { status_stack.push(1); }

	void
	restore_status(void) { status_stack.pop(); }

	int
	enabled(void) const { return status_stack.top(); }

	inline
	bool
	empty() const { return indent_stack.empty(); }

	/// prefix-only, adds the default indent string
	inline
	indent_string&
	operator ++ () {
		indent_stack.push_back(indenter);
		return *this;
	}

	// prefix-only, removes last added indent string
	inline
	indent_string&
	operator -- () {
		indent_stack.pop_back();
		return *this;
	}

	/// adds custom string, may be used to append directory
	inline
	indent_string&
	operator += (const string& s) {
		indent_stack.push_back(s);
		return *this;
	}

	friend
	ostream&
	operator << (ostream& o, const indent_string& s);

	struct string_dumper {
		ostream& os;
		string_dumper(ostream& o) : os(o) { }

		inline
		void operator () (const string& s) { os << s; }
	};

};      // end struct indent_size_type

typedef hash_map<const ostream*, indent_string>	ostream_indent_map_type;

/// privately maintained global stream indent map
static ostream_indent_map_type			ostream_indent_map;

// initialization
// ostream_indent_map_type                  	ostream_indent_map();

//=============================================================================
// static initializer
const string
indent_string::default_indent_string("  ");

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
operator << (ostream& o, const indent_string& s) {
	if (s.enabled()) {
		for_each(s.indent_stack.begin(), s.indent_stack.end(), 
			indent_string::string_dumper(o));
	}
	return o;
}

//=============================================================================
// class indent method definitions

indent::indent(const ostream& o) : os(o) {
	// will construct a new entry automatically
	++ostream_indent_map[&os];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
indent::indent(const ostream& o, const string& s) : os(o) {
	ostream_indent_map[&os] += s;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
indent::~indent() {
	--ostream_indent_map[&os];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
enable_indent::enable_indent(const ostream& o) : os(o) {
	ostream_indent_map[&os].enable();
}

enable_indent::~enable_indent() {
	ostream_indent_map[&os].restore_status();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
disable_indent::disable_indent(const ostream& o) : os(o) {
	ostream_indent_map[&os].disable();
}

disable_indent::~disable_indent() {
	ostream_indent_map[&os].restore_status();
}

//=============================================================================
// function definitions

ostream&
auto_indent(ostream& o) {
	return o << ostream_indent_map[&o];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
register_indent(const ostream& o, const string& s) {
	indent_string& ind = ostream_indent_map[&o];
	if (ind.empty()) {
		// safe to make new entry
		ind = indent_string(s);
	} else {
		std::cerr << "WARNING: util::register_indent(" << &o <<
			") ignored because existing entry is not empty!"
			<< std::endl;
	}
}

//=============================================================================
}	// end namespace util

