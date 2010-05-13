/**
	\file "util/optparse.h"
	Simple option parsing library.
	Simply tokenizes into string sets, which is then handed off
	elsewhere for processing.
	Options can come from file or command-line argv.
	$Id: optparse.h,v 1.5 2010/05/13 00:32:03 fang Exp $
 */

#ifndef	__UTIL_OPTPARSE_H__
#define	__UTIL_OPTPARSE_H__

#include <iosfwd>
#include <string>
#include <list>
#include <map>

namespace util {
using std::string;
using std::list;
using std::ostream;
using std::istream;
using std::map;

//=============================================================================
/**
	Storage structure for options.
 */
struct option_value {
	string		key;
	list<string>	values;

	bool
	empty(void) const { return key.empty(); }
};	// end struct option_value

typedef	list<option_value>		option_value_list;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
operator << (ostream&, const option_value&);

ostream&
operator << (ostream&, const option_value_list&);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Parses a single option of the form:
	key=value,value,...
 */
extern
option_value
optparse(const string&, const char e, const char s);

extern
option_value
optparse(const string&, const char s);

extern
option_value
optparse(const string&);

extern
option_value
optparse_compat(const string&);

/**
	This variant does not attempt to tokenize values into a list.
 */
extern
option_value
optparse_no_sep(const string&);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Parse a space-separated sequence of option values.  
	e.g. key=value key=value key=value
 */
#if 0
extern
option_value_list
optparse_list(const string&, const char s);
#endif

extern
option_value_list
optparse_list(const string&);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern
option_value_list
optparse_file(istream&);
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// for backward compatibility, yuck
extern
option_value_list
optparse_file_compat(istream&);

//=============================================================================
// option maps structures

/**
	\param T the option structure type.
 */
template <class T>
struct options_map_impl {
	typedef	T					options_struct_type;
	typedef	bool (*opt_func)(const option_value&, options_struct_type&);
	typedef	ostream& (*print_func)(ostream&, const options_struct_type&);
	struct opt_entry {
		opt_func				func;
		print_func				printer;
		const string*				type;
		string					help;

		opt_entry() : type(NULL) { }
		opt_entry(const opt_func f, const print_func p, 
			const string* const t, const string& h) :
			func(f), printer(p), type(t), help(h) { }

	};
	typedef	map<string, opt_entry>			opt_map_type;

	/**
		Keep one copy, really only needed for help printing.
	 */
	static const options_struct_type		default_value;
	/**
		The actual underlying options map.
	 */
	opt_map_type					options_map;

	options_map_impl() : options_map() { }

// helper functions
// you don't have to use these, but they serve as a starting point
	template <class V>
	static
	bool
	set_member_single_numeric_value(const option_value&, 
		options_struct_type&, V options_struct_type::*);

	template <class V, V options_struct_type::*mem>
	static
	bool
	set_member_single_numeric_value(const option_value& opt, 
			options_struct_type& t) {
		return set_member_single_numeric_value(opt, t, mem);
	}

	static
	bool
	set_member_single_string(const option_value&, 
		options_struct_type&, string options_struct_type::*);


	template <string options_struct_type::*mem>
	static
	bool
	set_member_single_string(const option_value& opt, 
			options_struct_type& t) {
		return set_member_single_string(opt, t, mem);
	}

	template <class V, V K>
	static
	bool
	set_member_constant(const option_value&,
		options_struct_type&, V options_struct_type::*);

	template <class V, V options_struct_type::*mem, V K>
	static
	bool
	set_member_constant(const option_value& opt, options_struct_type& t) {
		return set_member_constant<V,K>(opt, t, mem);
	}

	template <class V1, class V2, V2 K>
	static
	bool
	set_member_member_constant(const option_value&,
		options_struct_type&, V1 options_struct_type::*, 
		V2 V1::*);

	template <class V1, class V2,
		V1 options_struct_type::*mem1,
		V2 V1::*mem2, V2 K>
	static
	bool
	set_member_member_constant(const option_value& opt,
			options_struct_type& t) {
		return set_member_member_constant<V1,V2,K>(opt, t, mem1, mem2);
	}


// TODO: set member bitfield, and clear member bitfield

	template <class V>
	static
	ostream&
	print_member_default(ostream&, const options_struct_type&, 
		V options_struct_type::*);

	template <class V, V options_struct_type::*mem>
	static
	ostream&
	print_member_default(ostream& o, const options_struct_type& t) {
		return print_member_default(o, t, mem);
	}

	template <class VC>
	static
	ostream&
	print_member_sequence(ostream&, const options_struct_type&, 
		VC options_struct_type::*);

	template <class VC, VC options_struct_type::*mem>
	static
	ostream&
	print_member_sequence(ostream& o, const options_struct_type& t) {
		return print_member_sequence(o, t, mem);
	}

	ostream&
	dump(ostream&, const options_struct_type&) const;

	ostream&
	help(ostream&, const bool t = true, const bool d = true) const;

};	// end class options_map

//=============================================================================
}	// end namespace util

#endif	// __UTIL_OPTPARSE_H__

