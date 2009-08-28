/**
	\file "net/netlist_options.cc"
	$Id: netlist_options.cc,v 1.1.2.1 2009/08/28 01:23:05 fang Exp $
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
		lambda(1.0), 
		nested_subcircuits(false),
		emit_top(true)
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// a default copy for reference
// so the help can print default values
// static const netlist_options	default_netlist_options;

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
typedef	bool (*opt_func)(const option_value&, netlist_options&);
typedef	pair<opt_func, string>		opt_entry;
typedef	map<string, opt_entry>		opt_map_type;
static	opt_map_type			netlist_option_map;

// define option functions
#define	DEFINE_OPTION_DEFAULT(member, key, help)			\
static									\
bool									\
__set_ ## member (const option_value& v, netlist_options& o) {		\
	return __set_member_default(v, o, &netlist_options::member);	\
}									\
static const opt_entry& __receipt ## member				\
__ATTRIBUTE_UNUSED_CTOR__((netlist_option_map[key] =			\
	opt_entry(& __set_ ## member, help)));

// could just fold string into here instead of initialization function below...
// TODO: produce usage help for console and texinfo documentation aside
// TODO: make these declarations self-initializing/installing in the map
/***
@texinfo config/lambda.texi
@defopt lambda
Technology-dependent scaling factor for device lengths and widths, 
the multiplier factor applied to lengths and widths specified in PRS.
Default: 1.0
@end defopt
@end texinfo
***/
DEFINE_OPTION_DEFAULT(lambda, "lambda",
	"technology scaling factor for device lengths and widths [1.0]")

/***
@texinfo config/length_unit.texi
@defopt length_unit
Suffix-string to append to emitted length and width parameters.
Can be a unit such as "u" or "n", or exponent such as "e-6" or "e-9".
Default: u (micron)
@end defopt
@end texinfo
***/
DEFINE_OPTION_DEFAULT(length_unit, "length_unit",
	"suffix appended to lengths and widths [u]")

/***
@texinfo config/std_widths.texi
@defopt std_n_width
Default width (in lambda) for NFETs used in logic, where unspecified.
@end defopt

@defopt std_p_width
Default width (in lambda) for PFETs used in logic, where unspecified.
@end defopt

@defopt std_n_length
Default length (in lambda) for NFETs used in logic, where unspecified.
@end defopt

@defopt std_p_length
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
@defopt stat_n_width
Default width (in lambda) for NFETs used in keepers (staticizers), 
where unspecified.
@end defopt

@defopt stat_p_width
Default width (in lambda) for PFETs used in keepers (staticizers), 
where unspecified.
@end defopt

@defopt stat_n_length
Default length (in lambda) for NFETs used in keepers (staticizers), 
where unspecified.
@end defopt

@defopt stat_p_length
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
@defopt nested_subcircuits
If this option is set to 1, then emit local subcircuits as nested
definitions within their used definitions.  
Not sure which variants of SPICE support this.  
Default: 0
@end defopt
@end texinfo
***/
DEFINE_OPTION_DEFAULT(nested_subcircuits, "nested_subcircuits",
	"if true, emit local subcircuits as nested definitions [0]")

/***
@texinfo config/emit_top.texi
@defopt emit_top
If set to 1, include the top-level instances in the netlist output.
Setting this to 0 is useful for producing a library of subcircuit
definitions for every type that was instantiated, 
recursively w.r.t dependencies.  
Default: 1
@end defopt
@end texinfo
***/
DEFINE_OPTION_DEFAULT(emit_top, "emit_top",
	"if true, emit top-level instances in output [1]")

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
			if ((*mf->second.first)(*i, *this)) {
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
/**
	Print brief description of each configuration option.
 */
ostream&
netlist_options::help(ostream& o) {
	o << "netlist generation configuration options:" << endl;
	typedef	opt_map_type::const_iterator	map_iterator;
	map_iterator i(netlist_option_map.begin());
	const map_iterator e(netlist_option_map.end());
	for ( ; i!=e; ++i) {
		const opt_entry& s(i->second);
		o << "  " << i->first << ": " << s.second << endl;
	}
	return o;
}

//=============================================================================
}	// end namespace NET
}	// end namespace HAC

