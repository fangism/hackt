/**
	\file "net/netlist_options.cc"
	$Id: netlist_options.cc,v 1.14 2010/01/22 02:01:56 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <functional>
#include <fstream>
#include <map>
#include <list>
#include "net/netlist_options.h"
#include "util/string.tcc"		// for strgsub, string_to_num
#include "util/stacktrace.h"
#include "util/attributes.h"
#include "util/iterator_more.h"
#include "util/cppcat.h"

namespace HAC {
namespace NET {
#include "util/using_ostream.h"
using std::map;
using std::pair;
using std::list;
using util::option_value;
using util::option_value_list;
using util::strings::string_to_num;
using util::strings::strgsub;
using util::file_status;
using util::named_ifstream;
using util::set_inserter;

//=============================================================================
// class netlist_options method definitions

netlist_options::netlist_options() :
		file_manager(), 
		__dump_flags(dump_flags::no_definition_owner), 
		misc_options_map(), 
		unknown_option_policy(OPTION_WARN), 
		internal_node_supply_mismatch_policy(OPTION_WARN),
		undriven_node_policy(OPTION_WARN),
		case_collision_policy(OPTION_WARN),
		non_CMOS_precharge_policy(OPTION_WARN),
		std_n_width(5.0),
		std_p_width(5.0),
		std_n_length(2.0),
		std_p_length(2.0),
		stat_n_width(3.0),
		stat_p_width(3.0),
		stat_n_length(20.0),
		stat_p_length(10.0),
		length_unit("u"),
		area_unit("p"),
	// mangle options
		mangle_process_member_separator("."),
		mangle_struct_member_separator("."),
		mangle_underscore("_"),
		mangle_array_index_open("["),
		mangle_array_index_close("]"),
		mangle_template_empty("<>"),
		mangle_template_open("<"),
		mangle_template_close(">"),
		// Hmmmm, we don't have a way to escape the comma, do we?
		mangle_parameter_separator(","),
		mangle_parameter_group_open("{"),
		mangle_parameter_group_close("}"),
		mangle_scope("::"),
		mangle_colon(":"),
		mangle_internal_at("@"),
		mangle_auxiliary_pound("#"),
		mangle_implicit_bang("!"),
	// format options
		pre_line_continue(),
		post_line_continue("+"),	// spice-style
		comment_prefix("* "),

		lambda(1.0), 
		min_width(4.0),			// in lambda
		min_length(2.0),		// in lambda
		max_p_width(0.0),		// in lambda
		max_n_width(0.0),		// in lambda
		fet_diff_overhang(6.0),
		fet_spacing_diffonly(4.0),
		reserved_names(), 
		emit_parasitics(false),
		nested_subcircuits(false),
		empty_subcircuits(false),
		unused_ports(false),
		prefer_port_aliases(false),
		top_type_ports(false), 
		emit_top(true), 
		emit_node_aliases(false),
		emit_mangle_map(false)
		{
	// delayed mangling
	// to prevent double-mangling, we have to postpone
	// all mangling to the ::mangle_* functions
	// This distinguishes process from struct members
	__dump_flags.struct_member_separator = "$";
	// default device names
	misc_options_map["nfet_svt"].push_back("nch");
	misc_options_map["nfet_lvt"].push_back("nch_lvt");
	misc_options_map["nfet_hvt"].push_back("nch_hvt");
	misc_options_map["pfet_svt"].push_back("pch");
	misc_options_map["pfet_lvt"].push_back("pch_lvt");
	misc_options_map["pfet_hvt"].push_back("pch_hvt");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// a default copy for reference
// so the help can print default values
const netlist_options	netlist_options::default_value;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Useful for quickly un-mangling, and disabling mangling configurations
	to restored the default (for human readability).  
 */
bool
netlist_options::no_mangling(const option_value&) {
	const this_type& d(default_value);
	mangle_process_member_separator = d.mangle_process_member_separator;
	mangle_struct_member_separator = d.mangle_struct_member_separator;
	mangle_underscore = d.mangle_underscore;
	mangle_array_index_open = d.mangle_array_index_open;
	mangle_array_index_close = d.mangle_array_index_close;
	mangle_template_empty = d.mangle_template_empty;
	mangle_template_open = d.mangle_template_open;
	mangle_template_close = d.mangle_template_close;
	mangle_parameter_separator = d.mangle_parameter_separator;
	mangle_parameter_group_open = d.mangle_parameter_group_open;
	mangle_parameter_group_close = d.mangle_parameter_group_close;
	mangle_scope = d.mangle_scope;
	mangle_colon = d.mangle_colon;
	mangle_internal_at = d.mangle_internal_at;
	mangle_auxiliary_pound = d.mangle_auxiliary_pound;
	mangle_implicit_bang = d.mangle_implicit_bang;
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Mangle instance and node names, and transistor names. 
	For efficiency, apply manglings that are possible.
	Considered C++-style name mangling?
	Deleting character sequence for mangling is not acceptable, 
	thus we use the length of the substitution string to 
	determine whether or not to attempt substitution.  
 */
string&
netlist_options::mangle_instance(string& n) const {
	// must mangle underscore first, because other strings are likely
	// to use underscores in substitutions.
	strgsub(n, "_", mangle_underscore);
	strgsub(n, ".", mangle_process_member_separator);
	strgsub(n, "$", mangle_struct_member_separator);
	strgsub(n, "[", mangle_array_index_open);
	strgsub(n, "]", mangle_array_index_close);
//	strgsub(n, "::", mangle_scope);
	// colon must be mangled *after* scope "::"
	strgsub(n, ":", mangle_colon);	// appears in transistor names
	// @,# mangling happens in node::emit instead
	strgsub(n, "!", mangle_implicit_bang);
	return n;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For efficiency, apply manglings that are possible.
 */
string&
netlist_options::mangle_type(string& n) const {
	// must mangle underscore first, because other strings are likely
	// to use underscores in substitutions.
	strgsub(n, "_", mangle_underscore);
	strgsub(n, "<>", mangle_template_empty);
	strgsub(n, "<", mangle_template_open);
	strgsub(n, ">", mangle_template_close);
	strgsub(n, ",", mangle_parameter_separator);
	strgsub(n, "{", mangle_parameter_group_open);
	strgsub(n, "}", mangle_parameter_group_close);
	strgsub(n, "::", mangle_scope);
	// colon must be mangled *after* scope "::"
//	strgsub(n, ":", mangle_colon);
	return n;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
netlist_options::__open_config_file(const option_value& v, 
		option_value_list (*parse)(istream&)) {
	list<string>::const_iterator
		i(v.values.begin()), e(v.values.end());
for ( ; i!=e; ++i) {
	ifstream_manager::placeholder
		fh(file_manager, *i);
	switch (fh.status().second) {
	case file_status::NOT_FOUND:
		cerr << "Error: unable to find/open/read config file \""
			<< *i << "\"." << endl;
		return true;
	case file_status::CYCLE:
		cerr << "Error: config inclusion cycle detected on "
			"file \"" << fh.status().first->name()
			<< "\"." << endl;
		return true;
	case file_status::SEEN_FILE:
		cerr << "Warning: already seen configuration file \""
			<< fh.status().first->name()
			<< "\", but processing again anyway." << endl;
		// fall-through
	case file_status::NEW_FILE: {
		const option_value_list
			opts((*parse)(fh.get_stream()));
		if (set_options(opts)) {
		cerr << "Error processing options in file \"" <<
			fh.status().first->name() << "\"." << endl;
			return true;
		}
		break;
	}
	default: DIE;
	}
}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
netlist_options::open_config_file(const option_value& v) {
	return __open_config_file(v, &util::optparse_file);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
netlist_options::open_config_file_compat(const option_value& v) {
	return __open_config_file(v, &util::optparse_file_compat);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
netlist_options::add_config_path(const option_value& v) {
#if 0
	for_each(v.values.begin(), v.values.end(), 
		bind1st(mem_fun_ref(&ifstream_manager::add_path), 
			file_manager));	// damn it, reference-to-reference...
#else
	list<string>::const_iterator
		i(v.values.begin()), e(v.values.end());
	for ( ; i!=e; ++i) {
		file_manager.add_path(*i);
	}
#endif
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Post-process some options, finalize.
	This is necessary because some data structure members are
	linked/aliased, and this keeps them coherent.  
 */
void
netlist_options::commit(void) {
	// doesn't need to do anything right now, jsut a reminder
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string&
netlist_options::emit_scope(void) const {
	return mangle_scope;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string&
netlist_options::emit_colon(void) const {
	return mangle_colon;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string&
netlist_options::emit_internal_at(void) const {
	return mangle_internal_at;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string&
netlist_options::emit_auxiliary_pound(void) const {
	return mangle_auxiliary_pound;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Use this function when printing a line-continuation to respect
	the requested formatting.  
 */
ostream&
netlist_options::line_continue(ostream& o) const {
	return o << pre_line_continue << '\n' << post_line_continue;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sets a value of a structure member according to first value
	associated with option.  
	Such functions should be re-usable in util library.  
	\param T is value type, can be deduced from arguments!  
	\param opt key=values option value association.  
	\param mem is a pointer-to-member of type T.
 */
template <typename T>
static
bool
__set_member_single_numeric_value(const option_value& opt, 
		netlist_options& n_opt, 
		T netlist_options::*mem) {
	const size_t s = opt.values.size();
	if (s >= 1) {
		if (s > 1) {
		cerr << "Warning: extra arguments passed to \'" << opt.key
			<< "\' option ignored." << endl;
		}
		const string& arg(opt.values.front());
		const bool ret = string_to_num(arg, n_opt.*mem);
		if (ret) {
			cerr << "Error: processing argument of \'" << opt.key
				<< "\' option, expecting numeric value, "
				"but got: " << arg << endl;
		}
		return ret;
	} else {
		cerr << "Warning: blank argument passed to \'" << opt.key
			<< "\' option where one is expected.  Ignoring."
			<< endl;
		return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
bool
__set_member_single_string(const option_value& opt, 
		netlist_options& n_opt, 
		string netlist_options::*mem) {
	const size_t s = opt.values.size();
	if (s >= 1) {
		if (s > 1) {
		cerr << "Warning: extra arguments passed to \'" << opt.key
			<< "\' option ignored." << endl;
		}
		n_opt.*mem = opt.values.front();
	} else {
		// if blank value, erase the string
		(n_opt.*mem).clear();
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: this conditionally applies lower-casing transformation
	depending on case_collision.
	Blank values are filtered out.
	If remaining set is empty, then clear the set member variable.
	TODO: apply name mangling with n_opt.mangle_instance()?
 */
static
bool
__set_member_string_set(const option_value& opt, 
		netlist_options& n_opt, 
		set<string> netlist_options::*mem) {
	// remove blanks
	std::list<string> s;
	std::remove_copy(opt.values.begin(), opt.values.end(), 
		back_inserter(s), string());
if (s.size()) {
	if (n_opt.case_collision_policy == OPTION_IGNORE) {
		copy(s.begin(), s.end(), set_inserter(n_opt.*mem));
	} else {
		transform(s.begin(), s.end(), set_inserter(n_opt.*mem), 
			&util::strings::string_tolower);
	}
} else {
	// if argument is blank, then clear the set
	(n_opt.*mem).clear();
}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// translate a policy string into an enumeration
static
bool
__set_policy_member(const option_value& opt, 
		netlist_options& n_opt, 
		option_error_policy netlist_options::*mem) {
	const size_t s = opt.values.size();
	if (s >= 1) {
		if (s > 1) {
		cerr << "Warning: extra arguments passed to \'" << opt.key
			<< "\' option ignored." << endl;
		}
		const string& p(opt.values.front());
		if (p == "ignore") {
			n_opt.*mem = OPTION_IGNORE;
		} else if (p == "warn") {
			n_opt.*mem = OPTION_WARN;
		} else if (p == "error") {
			n_opt.*mem = OPTION_ERROR;
		} else {
			cerr << "Error: error policy values for option " <<
				opt.key << " are [ignore|warn|error]." << endl;
			return true;
		}
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Leave undefined, specialize the rest.
	Use single overloaded function to automatically dispatch.
 */
template <typename T>
bool
__set_member_default(const option_value& opt, 
		netlist_options& n_opt, T netlist_options::*mem);

// specialize for bool
bool
__set_member_default(const option_value& opt, 
		netlist_options& n_opt, bool netlist_options::*mem) {
	return __set_member_single_numeric_value(opt, n_opt, mem);
}

// specialize for real_type
bool
__set_member_default(const option_value& opt, 
		netlist_options& n_opt, real_type netlist_options::*mem) {
	return __set_member_single_numeric_value(opt, n_opt, mem);
}

// specialize for string
bool
__set_member_default(const option_value& opt, 
		netlist_options& n_opt, string netlist_options::*mem) {
	return __set_member_single_string(opt, n_opt, mem);
}

bool
__set_member_default(const option_value& opt, 
		netlist_options& n_opt,
		option_error_policy netlist_options::*mem) {
	return __set_policy_member(opt, n_opt, mem);
}

bool
__set_member_default(const option_value& opt, 
		netlist_options& n_opt,
		set<string> netlist_options::*mem) {
	return __set_member_string_set(opt, n_opt, mem);
}

/**
	Take option list, convert to option set.  
 */
bool
__set_misc_option(const option_value& opt, 
		netlist_options& n_opt) {
	n_opt.misc_options_map[opt.key] = opt.values;	// override
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
const
string
__bool_type__("bool"),
__int_type__("int"),
__real_type__("real"),
__str_type__("string"),
__strs_type__("strings");

template <typename T>
const string&
__string_type_of(T netlist_options::*);

const string&
__string_type_of(bool netlist_options::*) { return __bool_type__; }
const string&
__string_type_of(size_t netlist_options::*) { return __int_type__; }
const string&
__string_type_of(real_type netlist_options::*) { return __real_type__; }
const string&
__string_type_of(string netlist_options::*) { return __str_type__; }
const string&
__string_type_of(option_error_policy netlist_options::*) { return __str_type__; }
const string&
__string_type_of(set<string> netlist_options::*) { return __strs_type__; }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <typename T>
ostream&
__print_member_default(ostream& o, const netlist_options& n_opt,
		T netlist_options::*mem) {
	return o << n_opt.*mem;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <typename T>
ostream&
__print_member_sequence(ostream& o, const netlist_options& n_opt,
		T netlist_options::*mem) {
	const T& s(n_opt.*mem);
	if (!s.empty()) {
		typedef	typename T::const_iterator	const_iterator;
		typedef	typename T::value_type	value_type;
		const_iterator i(s.begin()), l(--s.end());
		copy(i, l, std::ostream_iterator<value_type>(o, ","));
		o << *l;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
__print_misc_option(ostream& o, const netlist_options& n_opt, 
		const string& k) {
	const netlist_options::misc_options_map_type::const_iterator
		f(n_opt.misc_options_map.find(k));
	if (f != n_opt.misc_options_map.end()) {
		list<string>::const_iterator
			i(f->second.begin()), l(--f->second.end());
		copy(i, l, std::ostream_iterator<string>(o, ","));
		o << *l;
	}
	// else print nothing
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
ostream&
__print_member_default(ostream& o, const netlist_options& n_opt,
		option_error_policy netlist_options::*mem) {
switch (n_opt.*mem) {
	case OPTION_IGNORE:	o << "ignore"; break;
	case OPTION_WARN:	o << "warn"; break;
	case OPTION_ERROR:	o << "error"; break;
	default:		o << "???";
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	bool (*opt_func)(const option_value&, netlist_options&);
typedef	ostream& (*print_func)(ostream&, const netlist_options&);
// typedef	const string& (*type_func)(void);
struct opt_entry {
	opt_func			func;
	print_func			printer;
	const string*			type;
	string				help;
	opt_entry() : type(NULL) { }
	opt_entry(const opt_func f, const print_func p, 
		const string* const t, const string& h) :
		func(f), printer(p), type(t), help(h) { }
};
typedef	map<string, opt_entry>		opt_map_type;
static	opt_map_type			netlist_option_map;


#define	DEFINE_SET_MEMBER(member)					\
static									\
bool									\
__set_ ## member (const option_value& v, netlist_options& o) {		\
	return __set_member_default(v, o, &netlist_options::member);	\
}

#define	DEFINE_CALL_MEMBER_FUNCTION(memfun)				\
static									\
bool									\
__set_ ## memfun (const option_value& v, netlist_options& o) {		\
	o.memfun(v);							\
	return false;							\
}

#define	DEFINE_SET_POLICY_MEMBER(member)				\
	DEFINE_SET_MEMBER(member ## _policy)

#define	DEFINE_PRINT_MEMBER(member)					\
static									\
ostream&								\
__print_ ## member (ostream& o, const netlist_options& n) {		\
	return __print_member_default(o, n, &netlist_options::member);	\
}

#define	DEFINE_PRINT_MEMBER_SEQUENCE(member)				\
static									\
ostream&								\
__print_ ## member (ostream& o, const netlist_options& n) {		\
	return __print_member_sequence(o, n, &netlist_options::member);	\
}

#define	DEFINE_PRINT_POLICY_MEMBER(member)				\
	DEFINE_PRINT_MEMBER(member ## _policy)

#define	DEFINE_PRINT_MISC_OPTION(key)					\
static									\
ostream&								\
__print_ ## key (ostream& o, const netlist_options& n) {		\
	static const string k(STRINGIFY(key));				\
	return __print_misc_option(o, n, k);				\
}

#define	DEFINE_TYPE_MEMBER(member)					\
static									\
const string&								\
__type_ ## member (void) {						\
	return __string_type_of(&netlist_options::member);		\
}

#define	DEFINE_TYPE_POLICY_MEMBER(member)				\
	DEFINE_TYPE_MEMBER(member ## _policy)

#define	REGISTER_OPTION_DEFAULT(member, key, help)			\
static const opt_entry& __receipt ## member				\
__ATTRIBUTE_UNUSED_CTOR__((netlist_option_map[key] =			\
	opt_entry(& __set_ ## member, &__print_ ## member, 		\
	&__type_ ## member(), help)));

#define	REGISTER_OPTION_POLICY(member, key, help)			\
	REGISTER_OPTION_DEFAULT(member ## _policy, key, help)

#define	REGISTER_PSEUDO_OPTION(memfun, key, help)			\
static const opt_entry& __receipt ## memfun				\
__ATTRIBUTE_UNUSED_CTOR__((netlist_option_map[key] =			\
	opt_entry(& __set_ ## memfun, NULL, NULL, help)));

#define	REGISTER_MISC_OPTION(key, help)					\
static const opt_entry& __receipt ## key				\
__ATTRIBUTE_UNUSED_CTOR__((netlist_option_map[STRINGIFY(key)] =		\
	opt_entry(&__set_misc_option, &__print_ ## key,	 		\
	&__str_type__, help)));


// define option functions
#define	DEFINE_OPTION_DEFAULT(member, key, help)			\
	DEFINE_SET_MEMBER(member)					\
	DEFINE_PRINT_MEMBER(member)					\
	DEFINE_TYPE_MEMBER(member)					\
	REGISTER_OPTION_DEFAULT(member, key, help)

#define	DEFINE_OPTION_SEQUENCE(member, key, help)			\
	DEFINE_SET_MEMBER(member)					\
	DEFINE_PRINT_MEMBER_SEQUENCE(member)				\
	DEFINE_TYPE_MEMBER(member)					\
	REGISTER_OPTION_DEFAULT(member, key, help)

// for member function calls

// for member function calls
#define	DEFINE_OPTION_MEMFUN(memfun, key, help)				\
	DEFINE_CALL_MEMBER_FUNCTION(memfun)				\
	REGISTER_PSEUDO_OPTION(memfun, key, help)

// for policy members
#define	DEFINE_OPTION_POLICY(member, key, help)				\
	DEFINE_SET_POLICY_MEMBER(member)				\
	DEFINE_PRINT_POLICY_MEMBER(member)				\
	DEFINE_TYPE_POLICY_MEMBER(member)				\
	REGISTER_OPTION_POLICY(member, key, help)

#define	DEFINE_MISC_OPTION(key, help)					\
	DEFINE_PRINT_MISC_OPTION(key)					\
	REGISTER_MISC_OPTION(key, help)

/***
@texinfo config/diagnostic_policies.texi

Option policies control the behavior of certain diagnostics.
Policy options accept the values "ignore", "warn", or "error".

@defopt unknown_option (string)
Set error-handling policy when encountering unknown configuration options.
Default: warn
@end defopt

@defopt internal_node_supply_mismatch (string)
Set error-handling policy when an internal node is used in a different
supply domain than the one it was defined in.  
Default: warn
@end defopt

@defopt undriven_node (string)
Set error handling policy for subcircuits that find nodes that are
neither used (connected to source, gate terminals) but not driven
(connected to drain terminal) nor coming from subcircuit port.
This check is always skipped for top-level circuits.
Default: warn
@end defopt

@defopt case_collision (string)
Set error handling policy for names that collide when case
is ignored (insensitive).  
For this policy warnings are promoted to errors; 
use ignore to completely disable.  
If the value is anything but @t{ignore}, then the set of reserved
names will also be transformed when detecting collisions.  
Default: warn
@end defopt

@defopt non_CMOS_precharge (string)
Set the error handling policy for precharge expressions that 
are written non-CMOS, i.e. not fully-restoring.
(for example, using an NMOS expression to pull-up an internal node
in an NMOS stack)
Such nets will only charge or precharge to 1 Vt from the supply rail.  
Be very careful with this option, it allows you to write nonsense like
@samp{a &@{+x & ~y@} b & c -> o-}.
Default: warn
@end defopt
@end texinfo
***/
DEFINE_OPTION_POLICY(unknown_option, "unknown_option",
	"EH for unknown options")
DEFINE_OPTION_POLICY(internal_node_supply_mismatch,
	"internal_node_supply_mismatch", 
	"EH for internal node supply mismatch btw. def/use")
DEFINE_OPTION_POLICY(undriven_node, "undriven_node",
	"EH for used but undriven nodes")
DEFINE_OPTION_POLICY(case_collision, "case_collision",
	"EH for case-insensitive name collisions")
DEFINE_OPTION_POLICY(non_CMOS_precharge, "non_CMOS_precharge",
	"EH for not fully-restoring precharges")

// could just fold string into here instead of initialization function below...
// TODO: produce usage help for console and texinfo documentation aside
// TODO: make these declarations self-initializing/installing in the map
/***
@texinfo config/lambda.texi
@cindex lambda
@defopt lambda (real)
Technology-dependent scaling factor for device lengths and widths, 
the multiplier factor applied to lengths and widths specified in PRS.
Default: 1.0
@end defopt
@end texinfo
***/
DEFINE_OPTION_DEFAULT(lambda, "lambda",
	"technology scaling factor for device lengths and widths")

/***
@texinfo config/length_unit.texi
@defopt length_unit (string)
Suffix-string to append to emitted length and width parameters.
Can be a unit such as "u" or "n", or exponent such as "e-6" or "e-9".
Default: u (micron)
@end defopt
@end texinfo
***/
DEFINE_OPTION_DEFAULT(length_unit, "length_unit",
	"suffix appended to lengths and widths")

/***
@texinfo config/area_unit.texi
@defopt area_unit (string)
Suffix-string to append to emitted area valiues.  
Can be a unit such as "p" (for pico), or exponent such as "e-6" or "e-12".
@strong{Alert:} this must be set consistently with respect to @var{length_unit}.
Default: p (pico, micron-squared)
@end defopt
@end texinfo
***/
DEFINE_OPTION_DEFAULT(area_unit, "area_unit",
	"suffix appended to areas values")

/***
@texinfo config/mangle.texi
@defopt mangle_underscore (string)
Substitute the `@t{_}' (underscore) character with another string, 
which may contain more underscores.  
@strong{Alert:} It is essential to set this appropriately if underscores 
are to be used in other mangling replacement strings.  
@end defopt

@defopt mangle_process_member_separator (char)
String used to separate members of process instances.
e.g., the `@t{.}' in `a.b' usually denotes that b is a member of typeof(a).
Default: .
@end defopt
@defopt mangle_struct_member_separator (char)
String used to separate members of datatype and channel instances.
Default: .
@end defopt

@defopt mangle_array_index_open (string)
Mangle the `@t{[}' character with a replacement string.
@end defopt
@defopt mangle_array_index_close (string)
Mangle the `@t{]}' character with a replacement string.
@end defopt

@defopt mangle_template_open (string)
Mangle the `@t{<}' character with a replacement string.
@end defopt
@defopt mangle_template_close (string)
Mangle the `@t{>}' character with a replacement string.
@end defopt
@defopt mangle_template_empty (string)
Mangle the `@t{<>}' sequence with a replacement string.
This is applied @emph{before} @t{<} and @t{>} are mangled.
@end defopt

@defopt mangle_parameter_separator (string)
Mangle the `@t{,}' character with a replacement string.
@end defopt

@defopt mangle_parameter_group_open (string)
Mangle the `@t{@{}' character with a replacement string.
@end defopt
@defopt mangle_parameter_group_close (string)
Mangle the `@t{@}}' character with a replacement string.
@end defopt

@defopt mangle_scope (string)
Mangle the `@t{::}' sequence with a replacement string.
This is applied @emph{before} @t{:} is mangled.
@end defopt
@defopt mangle_colon (string)
Mangle the `@t{:}' character with a replacement string.
@end defopt
@defopt mangle_internal_at (string)
Mangle the `@t{@@}' character (designating named internal node)
with a replacement string.
@end defopt
@defopt mangle_auxiliary_pound (string)
Mangle the `@t{#}' character (designating auxiliary node) 
with a replacement string.
@end defopt
@defopt mangle_implicit_bang (string)
Mangle the `@t{!}' character (designating implicit supply node) 
with a replacement string.
@end defopt
@end texinfo

@defopt no_mangling
Pseudo-option (no argument).
If this option is present, regardless of argument value, 
then suppress all name mangling by reverting their substitution
strings to empty.
This is useful for quickly disabling name mangling for
human-readability and diagnostics.  
@end defopt
***/
DEFINE_OPTION_DEFAULT(mangle_underscore,
	"mangle_underscore", "mangle: _ replacement")
DEFINE_OPTION_DEFAULT(mangle_process_member_separator,
	"mangle_process_member_separator", "mangle: . replacement (process)")
DEFINE_OPTION_DEFAULT(mangle_struct_member_separator,
	"mangle_struct_member_separator", "mangle: . replacement (structs)")
DEFINE_OPTION_DEFAULT(mangle_array_index_open,
	"mangle_array_index_open", "mangle: [ replacement")
DEFINE_OPTION_DEFAULT(mangle_array_index_close,
	"mangle_array_index_close", "mangle: ] replacement")
DEFINE_OPTION_DEFAULT(mangle_template_empty,
	"mangle_template_empty", "mangle: <> replacement")
DEFINE_OPTION_DEFAULT(mangle_template_open,
	"mangle_template_open", "mangle: < replacement")
DEFINE_OPTION_DEFAULT(mangle_template_close,
	"mangle_template_close", "mangle: > replacement")
DEFINE_OPTION_DEFAULT(mangle_parameter_separator,
	"mangle_parameter_separator", "mangle: , replacement")
DEFINE_OPTION_DEFAULT(mangle_parameter_group_open,
	"mangle_parameter_group_open", "mangle: { replacement")
DEFINE_OPTION_DEFAULT(mangle_parameter_group_close,
	"mangle_parameter_group_close", "mangle: } replacement")
DEFINE_OPTION_DEFAULT(mangle_colon,
	"mangle_colon", "mangle: : replacement")
DEFINE_OPTION_DEFAULT(mangle_scope,
	"mangle_scope", "mangle: :: replacement")
DEFINE_OPTION_DEFAULT(mangle_internal_at,
	"mangle_internal_at", "mangle: @ replacement")
DEFINE_OPTION_DEFAULT(mangle_auxiliary_pound,
	"mangle_auxiliary_pound", "mangle: # replacement")
DEFINE_OPTION_DEFAULT(mangle_implicit_bang,
	"mangle_implicit_bang", "mangle: ! replacement")
DEFINE_OPTION_MEMFUN(no_mangling,
	"no_mangling", "disable name-mangling")

/***
@texinfo config/pre_line_continue.texi
@defopt pre_line_continue (string)
String to print before emitting a continued line.
Default: (none)
@end defopt
@end texinfo
***/
DEFINE_OPTION_DEFAULT(pre_line_continue, "pre_line_continue",
	"line-continuation prefix")

/***
@texinfo config/post_line_continue.texi
@defopt post_line_continue (string)
String to print after emitting a continued line.
Default: +
@end defopt
@end texinfo
***/
DEFINE_OPTION_DEFAULT(post_line_continue, "post_line_continue",
	"line-continuation suffix")

/***
@texinfo config/comment_prefix.texi
@defopt comment_prefix (string)
String to print before whole-line comments.
Default: "* "
@end defopt
@end texinfo
***/
DEFINE_OPTION_DEFAULT(comment_prefix, "comment_prefix",
	"whole-line comment string")

/***
@texinfo config/min_width.texi
@defopt min_width (real)
Minimum transistor width in lambda.
@end defopt
@end texinfo
***/
DEFINE_OPTION_DEFAULT(min_width, "min_width", "minimum transistor width")
/***
@texinfo config/min_length.texi
@defopt min_length (real)
Minimum transistor length in lambda.
@end defopt
@end texinfo
***/
DEFINE_OPTION_DEFAULT(min_length, "min_length", "minimum transistor length")
/***
@texinfo config/max_p_width.texi
@defopt max_p_width (real)
Maximum PFET width.
@end defopt
@end texinfo
***/
DEFINE_OPTION_DEFAULT(max_p_width, "max_p_width", "maximum PFET width")
/***
@texinfo config/max_n_width.texi
@defopt max_n_width (real)
Maximum NFET width.
@end defopt
@end texinfo
***/
DEFINE_OPTION_DEFAULT(max_n_width, "max_n_width", "maximum NFET width")

/***
@texinfo config/std_widths.texi
@defopt std_n_width (real)
Default width (in lambda) for NFETs used in logic, where unspecified.
@end defopt

@defopt std_p_width (real)
Default width (in lambda) for PFETs used in logic, where unspecified.
@end defopt

@defopt std_n_length (real)
Default length (in lambda) for NFETs used in logic, where unspecified.
@end defopt

@defopt std_p_length (real)
Default length (in lambda) for PFETs used in logic, where unspecified.
@end defopt
@end texinfo
***/
DEFINE_OPTION_DEFAULT(std_n_width, "std_n_width",
	"default width of standard NFET (in lambda)")
DEFINE_OPTION_DEFAULT(std_p_width, "std_p_width",
	"default width of standard PFET (in lambda)")
DEFINE_OPTION_DEFAULT(std_n_length, "std_n_length",
	"default length of standard NFET (in lambda)")
DEFINE_OPTION_DEFAULT(std_p_length, "std_p_length",
	"default length of standard PFET (in lambda)")

/***
@texinfo config/stat_widths.texi
@defopt stat_n_width (real)
Default width (in lambda) for NFETs used in keepers (staticizers), 
where unspecified.
@end defopt

@defopt stat_p_width (real)
Default width (in lambda) for PFETs used in keepers (staticizers), 
where unspecified.
@end defopt

@defopt stat_n_length (real)
Default length (in lambda) for NFETs used in keepers (staticizers), 
where unspecified.
@end defopt

@defopt stat_p_length (real)
Default length (in lambda) for PFETs used in keepers (staticizers), 
where unspecified.
@end defopt
@end texinfo
***/
DEFINE_OPTION_DEFAULT(stat_n_width, "stat_n_width",
	"default width of keeper/staticizer NFET (in lambda)")
DEFINE_OPTION_DEFAULT(stat_p_width, "stat_p_width",
	"default width of keeper/staticizer PFET (in lambda)")
DEFINE_OPTION_DEFAULT(stat_n_length, "stat_n_length",
	"default length of keeper/staticizer NFET (in lambda)")
DEFINE_OPTION_DEFAULT(stat_p_length, "stat_p_length",
	"default length of keeper/staticizer PFET (in lambda)")


/***
@texinfo config/fet_type_overrides.texi
@defopt nfet_svt (string)
@defoptx nfet_lvt (string)
@defoptx nfet_hvt (string)
@defoptx pfet_svt (string)
@defoptx pfet_lvt (string)
@defoptx pfet_hvt (string)
Override the default device type names for NFETs and PFETS
along with their non-standard threshold voltage variants.  
@end defopt
@end texinfo
***/
DEFINE_MISC_OPTION(nfet_svt, "overriding model name for nfet_svt")
DEFINE_MISC_OPTION(nfet_lvt, "overriding model name for nfet_lvt")
DEFINE_MISC_OPTION(nfet_hvt, "overriding model name for nfet_hvt")
DEFINE_MISC_OPTION(pfet_svt, "overriding model name for pfet_svt")
DEFINE_MISC_OPTION(pfet_lvt, "overriding model name for pfet_lvt")
DEFINE_MISC_OPTION(pfet_hvt, "overriding model name for pfet_hvt")

/***
@texinfo config/nested_subcircuits.texi
@defopt nested_subcircuits (bool)
If this option is set to 1, then emit local subcircuits as nested
definitions within their used definitions.  
Not sure which variants of SPICE support this.  
Default: 0
@end defopt
@end texinfo
***/
DEFINE_OPTION_DEFAULT(nested_subcircuits, "nested_subcircuits",
	"emit local subcircuits as nested definitions")

/***
@texinfo config/empty_subcircuits.texi
@defopt empty_subcircuits (bool)
If this option is set to 1, then emit empty subcircuits, 
i.e. subcircuits with no devices.
Probably want to force unused port nodes to be emitted in
empty subcircuit definitions, option @option{unused_ports}.
Default: 0
@end defopt
@end texinfo
***/
DEFINE_OPTION_DEFAULT(empty_subcircuits, "empty_subcircuits",
	"emit empty subcircuit definitions and instances")

/***
@texinfo config/unused_ports.texi
@defopt unused_ports (bool)
If this option is set to 1, then consider all ports used
even if they are unconnected, for the purposes of emitting port lists.
This is useful @option{empty_subcircuits}, which would result in
subcircuits with no ports.  
Default: 0
@end defopt
@end texinfo
***/
DEFINE_OPTION_DEFAULT(unused_ports, "unused_ports",
	"include unused ports in subcircuit definitions")

/***
@texinfo config/prefer_port_aliases.texi
@defopt prefer_port_aliases (bool)
Instead of using default heuristic for choosing shallowest or shortest
canonical name, prefer any equivalent port name, if applicable.  
This can make netlists and simulation results easier to grok.
Default: 0
@end defopt
@end texinfo
***/
DEFINE_OPTION_DEFAULT(prefer_port_aliases, "prefer_port_aliases",
	"use port name for aliases (if available) as canonical name")

/***
@texinfo config/emit_top.texi
@defopt emit_top (bool)
If set to 1, include the top-level instances in the netlist output.
Setting this to 0 is useful for producing a library of subcircuit
definitions for every type that was instantiated, 
recursively w.r.t dependencies.  
Default: 1
@end defopt
@end texinfo
***/
DEFINE_OPTION_DEFAULT(emit_top, "emit_top",
	"if true, emit top-level instances in output")

/***
@texinfo config/emit_node_aliases.texi
@defopt emit_node_aliases (bool)
If set to 1, print a sets of node aliases (equivalent names).
Default: 0
@end defopt
@end texinfo
***/
DEFINE_OPTION_DEFAULT(emit_node_aliases, "emit_node_aliases",
	"if true, emit node alias sets in subcircuit comments")

/***
@texinfo config/emit_mangle_map.texi
@defopt emit_mangle_map (bool)
If set to 1, print a list of manglings in comments.  
Default: 0
@end defopt
@end texinfo
***/
DEFINE_OPTION_DEFAULT(emit_mangle_map, "emit_mangle_map",
	"if true, emit mangled node name map in subcircuit comments")

#if 0
/***
@texinfo config/top_type_ports.texi
@defopt top_type_ports (bool)
@strong{This option is not yet implemented.}
If set to 1, wrap the top-level instances inside a subcircuit.
This option is useful when a type was chosen as the top level, 
using the @option{-t} option.
Default: 1
@end defopt
@end texinfo
***/
DEFINE_OPTION_DEFAULT(top_type_ports, "top_type_ports",
	"if true, emit top-level instances in subcircuit")
#endif

/***
@texinfo config/emit_parasitics.texi
@defopt emit_parasitics (bool)
If set to 1, include source and drain area and perimeter parameters
for every transistor
Default: 0
@end defopt
@end texinfo
***/
DEFINE_OPTION_DEFAULT(emit_parasitics, "emit_parasitics",
	"if true, emit parasitic parameters for transistors")

/***
@texinfo config/fet_diff_overhang.texi
@defopt fet_diff_overhang (real)
When computing parasitics, this is the length of diffusion overhang 
past the end of the drawn transistor, in lambda.
Default: 6.0
@end defopt
@end texinfo
***/
DEFINE_OPTION_DEFAULT(fet_diff_overhang, "fet_diff_overhang",
	"length of diffusion overhang in lambda")

/***
@texinfo config/fet_spacing_diffonly.texi
@defopt fet_spacing_diffonly (real)
When computing parasitics, this is the length of diffusion between
adjoining transistors, in lambda.  
Default: 4.0
@end defopt
@end texinfo
***/
DEFINE_OPTION_DEFAULT(fet_spacing_diffonly, "fet_spacing_diffonly",
	"diffusion spacing between gates in lambda")

/***
@texinfo config/config_file.texi
@defopt config_file files
@defoptx config_file_compat files
Import other configuration file(s), exactly like the @option{-c} 
and @option{-C} command-line options.
File are searched using the configuration search path.
The @option{_compat} variation processes old-style configuration files.
@end defopt
@end texinfo
***/
DEFINE_OPTION_MEMFUN(open_config_file,
	"config_file", "import referenced configuration file")
DEFINE_OPTION_MEMFUN(open_config_file_compat,
	"config_file_compat", "import referenced configuration file")
/***
@texinfo config/config_path.texi
@defopt config_path paths
Append to list of paths for searching for configuration files, 
exactly like the @option{-I} command-line option.
Reminder: paths are comma-separated.
@end defopt
@end texinfo
***/
DEFINE_OPTION_MEMFUN(add_config_path,
	"config_path", "append search path for config files (cumulative)")

/***
@texinfo config/reserved_names.texi
@defopt reserved_names (strings)
Amend the set of names that should be rejected from normal use
in the netlist because they have special meaning to other back-ends.  
Issuing a blank value will clear out all previous values.  
Comparison is done using the @emph{post-mangled} names.  
Default: (blank)
@end defopt
@end texinfo
***/
DEFINE_OPTION_SEQUENCE(reserved_names, "reserved_names",
	"set of reserved names to avoid")

#undef	DEFINE_OPTION_DEFAULT
#undef	DEFINE_OPTION_MEMFUN
#undef	DEFINE_OPTION_POLICY
#undef	DEFINE_OPTION_SEQUENCE

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Handle a single keyed option.
 */
bool
netlist_options::set_option(const option_value& opt) {
if (opt.key.length()) {
	typedef	opt_map_type::const_iterator	map_iterator;
	const map_iterator me(netlist_option_map.end());
	const map_iterator mf(netlist_option_map.find(opt.key));
	if (mf != me) {
		if ((*mf->second.func)(opt, *this)) {
			return true;
		}
	} else {
	switch (unknown_option_policy) {
	case OPTION_ERROR:
		cerr << "Error: unknown netlist option \'"
			<< opt.key << "\'." << endl;
		return true;
	case OPTION_WARN:
		cerr << "Warning: ignoring unknown netlist option \'"
			<< opt.key << "\'." << endl;
	default: break;
	}
	}
}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: this should be reusable boilerplate code...
	\return true on error.
 */
bool
netlist_options::set_options(const option_value_list& opts) {
	STACKTRACE_VERBOSE;
	size_t errs = 0;
	option_value_list::const_iterator i(opts.begin()), e(opts.end());
	for ( ; i!=e; ++i) {
		if (set_option(*i))
			++errs;
	}
	if (errs) {
		cerr << "Error: " << errs <<
			" error(s) found in option processing." << endl;
	}
	return errs;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
netlist_options::dump(ostream& o) const {
	o << "# netlist generation configuration values:"
		<< endl;
	typedef	opt_map_type::const_iterator	map_iterator;
	map_iterator i(netlist_option_map.begin());
	const map_iterator e(netlist_option_map.end());
	for ( ; i!=e; ++i) {
		const opt_entry& s(i->second);
	if (s.printer) {
		o << i->first << '=';
		(*s.printer)(o, *this) << endl;
	}	// else is a meta option
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print brief description of each configuration option.
 */
ostream&
netlist_options::help(ostream& o) {
	o << "hacknet configuration options [default values]:"
		<< endl;
	typedef	opt_map_type::const_iterator	map_iterator;
	map_iterator i(netlist_option_map.begin());
	const map_iterator e(netlist_option_map.end());
	for ( ; i!=e; ++i) {
		const opt_entry& s(i->second);
		o << i->first << " (";
		if (s.type)
			o << *s.type;
		o << "): "
		<< s.help;
		if (s.printer) {
			o << " [";
			(*s.printer)(o, default_value);
			o << ']';
		} else o << " <pseudo-option>";
		o << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param d is true if pull-up/PFET, else false for pull-dn/NFET.
	\param k is true if is a standard keeper (not combinational).
	\return the default width.
 */
real_type
netlist_options::get_default_width(const bool d, const bool k) const {
	return (d ? 
		(k ? stat_p_width : std_p_width) :
		(k ? stat_n_width : std_n_width));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
real_type
netlist_options::get_default_length(const bool d, const bool k) const {
	return (d ? 
		(k ? stat_p_length : std_p_length) :
		(k ? stat_n_length : std_n_length));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
netlist_options::collides_reserved_name(const string& n) const {
	const std::set<string>::const_iterator f(reserved_names.find(n));
	return f != reserved_names.end();
}

//=============================================================================
}	// end namespace NET
}	// end namespace HAC

