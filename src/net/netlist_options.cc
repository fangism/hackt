/**
	\file "net/netlist_options.cc"
	$Id: netlist_options.cc,v 1.2.2.3 2009/09/08 22:28:56 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <map>
#include "net/netlist_options.h"
#include "util/string.tcc"		// for strgsub, string_to_num
#include "util/stacktrace.h"
#include "util/attributes.h"

namespace HAC {
namespace NET {
#include "util/using_ostream.h"
using std::map;
using std::pair;
using util::option_value;
using util::option_value_list;
using util::strings::string_to_num;
using util::strings::strgsub;

//=============================================================================
// class netlist_options method definitions

netlist_options::netlist_options() :
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
		mangle_instance_member_separator(),
		mangle_underscore(),
		mangle_array_index_open(),
		mangle_array_index_close(),
		mangle_template_open(),
		mangle_template_close(),
		mangle_parameter_separator(),
		mangle_parameter_group_open(),
		mangle_parameter_group_close(),
		mangle_scope(),
		mangle_colon(),
		mangle_internal_at(),
		mangle_auxiliary_pound(),
	// format options
		pre_line_continue(),
		post_line_continue("+"),	// spice-style

		lambda(1.0), 
		min_width(4.0),			// in lambda
		min_length(2.0),		// in lambda
		max_p_width(0.0),		// in lambda
		max_n_width(0.0),		// in lambda
		fet_diff_overhang(6.0),
		fet_spacing_diffonly(4.0),
		emit_parasitics(false),
		nested_subcircuits(false),
		emit_top(true)
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// a default copy for reference
// so the help can print default values
const netlist_options	netlist_options::default_value;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Considered C++-style name mangling?
	Deleting character sequence for mangling is not acceptable, 
	thus we use the length of the substitution string to 
	determine whether or not to attempt substitution.  
 */
string&
netlist_options::mangle_name(string& n) const {
	// must mangle underscore first, because other strings are likely
	// to use underscores in substitutions.
	if (mangle_underscore.length())
		strgsub(n, "_", mangle_underscore);
	if (mangle_instance_member_separator.length())
		strgsub(n, ".", mangle_instance_member_separator);
	if (mangle_array_index_open.length())
		strgsub(n, "[", mangle_array_index_open);
	if (mangle_array_index_close.length())
		strgsub(n, "]", mangle_array_index_close);
	if (mangle_template_open.length())
		strgsub(n, "<", mangle_template_open);
	if (mangle_template_close.length())
		strgsub(n, ">", mangle_template_close);
	if (mangle_parameter_separator.length())
		strgsub(n, ",", mangle_parameter_separator);
	if (mangle_parameter_group_open.length())
		strgsub(n, "{", mangle_parameter_group_open);
	if (mangle_parameter_group_close.length())
		strgsub(n, "}", mangle_parameter_group_close);
	if (mangle_scope.length())
		strgsub(n, "::", mangle_scope);
	// colon must be mangled *after* scope "::"
	if (mangle_colon.length())
		strgsub(n, ":", mangle_colon);
	if (mangle_internal_at.length())
		strgsub(n, "@", mangle_internal_at);
	if (mangle_auxiliary_pound.length())
		strgsub(n, "#", mangle_auxiliary_pound);
	return n;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string&
netlist_options::emit_scope(void) const {
	static const string scope("::");
	if (mangle_scope.length())
		return mangle_scope;
	else	return scope;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string&
netlist_options::emit_colon(void) const {
	static const string colon(":");
	if (mangle_colon.length())
		return mangle_colon;
	else	return colon;
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
const
string
__bool_type__("bool"),
__int_type__("int"),
__real_type__("real"),
__str_type__("string");

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

#if 0
const string&
__string_type_of(bool) { return __bool__; }
const string&
__string_type_of(size_t) { return __int__; }
const string&
__string_type_of(real_type) { return __real__; }
const string&
__string_type_of(string) { return __str__; }
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <typename T>
ostream&
__print_member_default(ostream& o, const netlist_options& n_opt,
		T netlist_options::*mem) {
	return o << n_opt.*mem;
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
		const string& t, const string& h) :
		func(f), printer(p), type(&t), help(h) { }
};
typedef	map<string, opt_entry>		opt_map_type;
static	opt_map_type			netlist_option_map;

// define option functions
#define	DEFINE_OPTION_DEFAULT(member, key, help)			\
static									\
bool									\
__set_ ## member (const option_value& v, netlist_options& o) {		\
	return __set_member_default(v, o, &netlist_options::member);	\
}									\
static									\
ostream&								\
__print_ ## member (ostream& o, const netlist_options& n) {		\
	return __print_member_default(o, n, &netlist_options::member);	\
}									\
static									\
const string&								\
__type_ ## member (void) {						\
	return __string_type_of(&netlist_options::member);		\
}									\
static const opt_entry& __receipt ## member				\
__ATTRIBUTE_UNUSED_CTOR__((netlist_option_map[key] =			\
	opt_entry(& __set_ ## member, &__print_ ## member, 		\
	__type_ ## member(), help)));

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

@defopt mangle_instance_member_separator (string)
Text used to separate members of instance hierarchy.
e.g., the `@t{.}' in `a.b' usually denotes that b is a member of typeof(a).
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
@end defopt
@defopt mangle_colon (string)
Mangle the `@t{:}' character with a replacement string.
@end defopt
@defopt mangle_internal_at (string)
Mangle the `@t{@@}' character with a replacement string.
@end defopt
@defopt mangle_auxiliary_pound (string)
Mangle the `@t{#}' character with a replacement string.
@end defopt
@end texinfo
***/
DEFINE_OPTION_DEFAULT(mangle_underscore,
	"mangle_underscore", "mangle: _ replacement")
DEFINE_OPTION_DEFAULT(mangle_instance_member_separator,
	"mangle_instance_member_separator", "mangle: . replacement")
DEFINE_OPTION_DEFAULT(mangle_array_index_open,
	"mangle_array_index_open", "mangle: [ replacement")
DEFINE_OPTION_DEFAULT(mangle_array_index_close,
	"mangle_array_index_close", "mangle: ] replacement")
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

#undef	DEFINE_OPTION_DEFAULT
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: this should be reusable boilerplate code...
	\return true on error.
 */
bool
netlist_options::set(const option_value_list& opts) {
	STACKTRACE_VERBOSE;
	size_t errs = 0;
	typedef	opt_map_type::const_iterator	map_iterator;
	const map_iterator me(netlist_option_map.end());
	option_value_list::const_iterator i(opts.begin()), e(opts.end());
	for ( ; i!=e; ++i) {
	if (i->key.length()) {
		const map_iterator mf(netlist_option_map.find(i->key));
		if (mf != me) {
			if ((*mf->second.func)(*i, *this)) {
				++errs;
			}
		} else {
			cerr << "Warning: ignoring unknown netlist option \'"
				<< i->key << "\'." << endl;
		}
	}
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
		o << i->first << '=';
		(*s.printer)(o, *this) << endl;
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
		o << i->first << " (" << *s.type << "): "
		<< s.help << " [";
		(*s.printer)(o, default_value) << ']' << endl;
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

//=============================================================================
}	// end namespace NET
}	// end namespace HAC

