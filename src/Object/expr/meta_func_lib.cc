/**
	\file "Object/expr/meta_func_lib.cc"
	stolen from "Object/expr/dlfunction.cc"
	$Id: meta_func_lib.cc,v 1.3 2011/01/14 01:32:57 fang Exp $
 */

#define	ENABLE_STATIC_TRACE				0
#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#define	ENABLE_STACKTRACE				0

#include <iostream>
#include <map>
#include <typeinfo>		// for std::bad_cast

#include <sstream>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include "util/cppcat.h"	// for UNIQUIFY
#include "util/macros.h"
#include "util/memory/count_ptr.h"
// #include "util/memory/count_ptr.tcc"
#include "util/memory/excl_ptr.h"
#include "util/stacktrace.h"

#include "util/function_traits.h"	// for result_of
#include "util/string.tcc"

#include "Object/expr/meta_func_lib.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/pbool_const.h"
#include "Object/expr/preal_const.h"
#include "Object/expr/pstring_const.h"
#include "Object/traits/value_traits.h"

#include "Object/expr/meta_call_traits.h"
#include "Object/expr/const_param_expr_list.h"

namespace HAC {
namespace entity {
using util::memory::never_ptr;
using std::string;
using std::ostringstream;
#include "util/using_ostream.h"


typedef	std::map<string, mapped_func_entry_type>	
					meta_func_map_type;

static
meta_func_map_type			meta_function_map;

/**
	By default, echo each function name as it is registered.
	Not to be confused with the one defined in Object/expr/dlfunction.cc
 */
static
const
bool
ack_loaded_functions = false;


#if !LOADABLE_META_FUNCTIONS
static		// restricted to this translation unit only
REGISTER_META_FUNCTION_PROTO;
#endif

//=============================================================================
// I thought this reverse-map code already existed somewhere...
// in Object/expr/..., Object/traits/... ?
// maybe this belongs in Object/expr/meta_call_traits.h ?

template <typename T>
struct reverse_type_map;

template <>
struct reverse_type_map<bool_value_type> {
	typedef	pbool_tag		meta_type;
	typedef	bool_tag		data_type;
};

template <>
struct reverse_type_map<int_value_type> {
	typedef	pint_tag		meta_type;
	typedef	int_tag			data_type;
};

template <>
struct reverse_type_map<real_value_type> {
	typedef	preal_tag		meta_type;
	typedef	real_tag		data_type;
};

template <>
struct reverse_type_map<string_value_type> {
	typedef	pstring_tag		meta_type;
	typedef	string_tag		data_type;
};

#define	META_RETURN_TYPE_ENUM(f)					\
class_traits<reverse_type_map<RESULT_OF(f)>::meta_type>::type_tag_enum_value

//=============================================================================
/**
        Helper class for automatically registering function
        upon dlopening of a module.  
 */
class meta_function_registrar {
public:
	/**
		\throw exception on failure.
	 */
	meta_function_registrar(const std::string&,
		const meta_function_ptr_type, const char);

	meta_function_registrar(const std::string&,
		meta_function_type* const, const char);

	~meta_function_registrar();

} /* __ATTRIBUTE_UNUSED__ */ ;

meta_function_registrar::meta_function_registrar(
		const string& fn, const meta_function_ptr_type fp, 
		const char t) {
	if (register_meta_function(fn, fp, t)) {
		THROW_EXIT;
	}
}

meta_function_registrar::meta_function_registrar(
		const string& fn, meta_function_type* const fp, 
		const char t) {
	if (register_meta_function(fn, meta_function_ptr_type(fp), t)) {
		THROW_EXIT;
	}
}

meta_function_registrar::~meta_function_registrar() {
// un-register?
}

//=============================================================================
/**
	Preloads map with function.  
	Calling this isn't necessary, only requirement is that 
	modules are loaded before searching for symbols.  
	With this interface, functions can be mapped using names different
	than their symbol name, which circumvents symbol name mangling.  
	\return 0 for success, anything else for error.
 */
// REGISTER_META_FUNCTION_PROTO
int
register_meta_function(const string& fn, const meta_function_ptr_type fp, 
		const char type_enum) {
	STACKTRACE_VERBOSE;
	NEVER_NULL(fp);
	mapped_func_entry_type& mf(meta_function_map[fn]);
	if (!mf.first) {
	if (ack_loaded_functions) {
		// confirmation:
		cout << "loaded function: `" << fn << "'." << endl;
	}
		mf.first = meta_mapped_func_ptr_type(fp);
		mf.second = type_enum;
	} else {
		cerr << "ERROR: CHP function symbol `" << fn <<
			"\' is already bound." << endl;
		return 1;
	}
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Automatically looks up function and caches resolved symbol.  
	The dlsym lookup is a last resort if it wasn't found in the
	managed function map already.  
	CORRECTION: do NOT dl_find_sym, only use function registry interface.
 */
const mapped_func_entry_type&
lookup_meta_function(const std::string& fn) {
	typedef	meta_func_map_type::const_iterator	const_iterator;
	static const mapped_func_entry_type
		null(meta_mapped_func_ptr_type(NULL), char(META_TYPE_NONE));
	STACKTRACE_VERBOSE;
	const const_iterator i(meta_function_map.find(fn));
	if (i == meta_function_map.end()) {
		cerr << "ERROR: symbol `" << fn << "\' not found "
			"in presently loaded modules." << endl;
		return null;
	} else {
		return i->second;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print a list of all bound functions.  
 */
void
list_meta_functions(std::ostream& o) {
	typedef	meta_func_map_type::const_iterator	const_iterator;
	const_iterator i(meta_function_map.begin()), e(meta_function_map.end());
	o << "Functions bound to module symbols:" << endl;
	for ( ; i!=e; ++i) {
		NEVER_NULL(i->second.first);
		o << "\t" << i->first << endl;
	}
}

//=============================================================================
// the built-in functions
// macros inspired by "sim/chpsim/chpsim_dlfunctions.h"
// if we define all functions here, we don't have to worry about
// global static construction ordering w.r.t. the function map.

#if 0
/**
	Declare an object that auto-loads the named function on construction.
	The function name is retained, not wrapped.  
	Part of public interface for 'advanced' users.
 */
#define	REGISTER_META_FUNCTION_RAW(key, fname)				\
static const meta_function_registrar					\
UNIQUIFY(fname ## _receipt_) (key, fname, META_RETURN_TYPE_ENUM(fname));
#endif

/**
	Name transformation, not for general use.  
 */
#define WRAP_META_FUNCTION_NAME(fname)	UNIQUIFY(wrapped_ ## fname ## _)

/**
	Declare an object that auto-loads the named function on construction.
	Not intended for general use.  
 */
#define REGISTER_META_FUNCTION(key, fname)				\
static const meta_function_registrar					\
UNIQUIFY(fname ## _receipt_) (key, WRAP_META_FUNCTION_NAME(fname), 	\
	META_RETURN_TYPE_ENUM(fname));

/**
	Intended for general use.  
	\param key is the string for the function name.
	\param fname is the function in the local module to register.
 */
#define META_FUNCTION_LOAD_DEFAULT(key, fname)				\
static									\
meta_function_return_type						\
WRAP_META_FUNCTION_NAME(fname) (const meta_function_argument_list_type& a) { \
	return auto_wrap_dlfunction(fname, a);				\
}									\
REGISTER_META_FUNCTION(key, fname)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// all functions here given static linkage, no need to export
// checkout ideas from CHP function libary "lib/libchpfn/*.cc"
// is there a better way to unify these, rather than having separate registries?
// currently these definitions are duplicated

// copied from "lib/libchpfn/string.cc"
/***
@texinfo fn/strcat.texi
@deffn Fucntion strcat args...
@deffnx Fucntion sprint args...
Stringify all @var{args} and concatenate into a single string (returned).  
This can be used to convert argument types to a string.  
Does not include terminating newline.  
@end deffn
@end texinfo
***/
static
string_value_type
strcat(const meta_function_argument_list_type& args) {
	ostringstream oss;
	args.dump_raw(oss);
	return oss.str();
}

META_FUNCTION_LOAD_DEFAULT("strcat", strcat)
META_FUNCTION_LOAD_DEFAULT("sprint", strcat)

// TODO: substring, character operations, regular expression matching...

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// copied from "lib/libchpfn/conditional.cc"
template <class T>
static
T
_cond(const bool_value_type z, const T a, const T b) {
	return z ? a : b;
}

/**
@texinfo fn/cond.texi
@deffn Function bcond z a b
@deffnx Function zcond z a b
@deffnx Function rcond z a b
@deffnx Function strcond z a b
Conditional expression, for boolean, integer, real, 
and string rvalues, respectively.  
If @var{z} is true, return @var{a}, else return @var{b}.
NOTE: both expressions @var{a} and @var{b} 
are evaluated @emph{unconditionally}.
@end deffn
@end texinfo
 */
static
bool_value_type
bcond(const bool_value_type z,
		const bool_value_type a, const bool_value_type b) {
	return _cond(z, a, b);
}

static
int_value_type
zcond(const bool_value_type z,
		const int_value_type a, const int_value_type b) {
	return _cond(z, a, b);
}

static
real_value_type
rcond(const bool_value_type z,
		const real_value_type a, const real_value_type b) {
	return _cond(z, a, b);
}

static
string_value_type
strcond(const bool_value_type z,
		const string_value_type a, const string_value_type b) {
	return _cond(z, a, b);
}

#if 0
/***
@texinfo fn/select.texi
@deffn Function select index args...
Returns expression @i{args[index]}, where @var{index} is 0-based.  
Throws run-time exception if @var{index} is out-of-range.  
@end deffn
@end texinfo
***/
static
meta_function_return_type
select(const meta_function_argument_list_type& args) {
	STACKTRACE_VERBOSE;
	const int_value_type index = extract_int(*args[0]);
	const meta_function_return_type& ret(args[index +1]);
	NEVER_NULL(ret);
	return ret;
}
#endif

META_FUNCTION_LOAD_DEFAULT("bcond", bcond)
META_FUNCTION_LOAD_DEFAULT("zcond", zcond)
META_FUNCTION_LOAD_DEFAULT("rcond", rcond)
META_FUNCTION_LOAD_DEFAULT("strcond", strcond)

// conversion functions
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <typename T>
static
T
strto_(const char* fn, const string_value_type& str) {
	T ret;
	NEVER_NULL(fn);
	if (util::strings::string_to_num(str, ret)) {
		cerr << "Error in conversion: "
			<< fn << "(\"" << str << "\")" << endl;
		THROW_EXIT;
	}
	return ret;
}

/***
@texinfo fn/strtox.texi
@deffn Function strtoz str
@deffnx Function strtob str
@deffnx Function strtor str
Convert string @var{str} to an integer, boolean, real, respectively.  
Throws run-time exception if conversion fails.  
@end deffn
@end texinfo
***/
static
int_value_type
strtoz(const string_value_type& str) {
	return strto_<int_value_type>("strtoz", str);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
bool_value_type
strtob(const string_value_type& str) {
	return strto_<bool_value_type>("strtob", str);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
real_value_type
strtor(const string_value_type& str) {
	return strto_<real_value_type>("strtor", str);
}

META_FUNCTION_LOAD_DEFAULT("strtoz", strtoz)
META_FUNCTION_LOAD_DEFAULT("strtob", strtob)
META_FUNCTION_LOAD_DEFAULT("strtor", strtor)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// libm <math.h> <cmath> functions

#define	PORT_STD_MATH_FUNCTION_REAL1(fun)				\
static									\
real_value_type								\
fun(const real_value_type x) { return std::fun(x); }

#define	PORT_STD_MATH_FUNCTION_REAL2(fun)				\
static									\
real_value_type								\
fun(const real_value_type x, const real_value_type y) {			\
	return std::fun(x, y);						\
}

/***
@texinfo fn/math.texi
The following math functions are exported from the vendor's @file{libm}
library, which includes most symbols from @file{<math.h>}.
Arguments and return types are real-valued unless noted otherwise.
@itemize
@item @t{abs} -- absolute value
@item @t{fabs} -- absolute value
@item @t{ceil} -- round up
@item @t{floor} -- round down
@item @t{fmod} -- modulo (remainder)
@item @t{sqrt} -- square-root
@item @t{exp} -- exponential
@item @t{log} -- natural logarithm
@item @t{log10} -- base-10 logarithn
@item @t{pow} -- power function
@item @t{ldexp} --
@item @t{sin} -- sine
@item @t{cos} -- cosine
@item @t{tan} -- tangent
@item @t{asin} -- arcsine
@item @t{acos} -- arccosine
@item @t{atan} -- arctangent
@item @t{sinh} -- hyperbolic sine
@item @t{cosh} -- hyperbolic cosine
@item @t{tanh} -- hyperbolic tangent
@end itemize
@end texinfo
***/

PORT_STD_MATH_FUNCTION_REAL1(abs)
META_FUNCTION_LOAD_DEFAULT("abs", abs)
PORT_STD_MATH_FUNCTION_REAL1(fabs)
META_FUNCTION_LOAD_DEFAULT("fabs", fabs)
PORT_STD_MATH_FUNCTION_REAL1(ceil)
META_FUNCTION_LOAD_DEFAULT("ceil", ceil)
PORT_STD_MATH_FUNCTION_REAL1(floor)
META_FUNCTION_LOAD_DEFAULT("floor", floor)
PORT_STD_MATH_FUNCTION_REAL2(fmod)
META_FUNCTION_LOAD_DEFAULT("fmod", fmod)

PORT_STD_MATH_FUNCTION_REAL1(sqrt)
META_FUNCTION_LOAD_DEFAULT("sqrt", sqrt)
PORT_STD_MATH_FUNCTION_REAL1(exp)
META_FUNCTION_LOAD_DEFAULT("exp", exp)
PORT_STD_MATH_FUNCTION_REAL1(log)
META_FUNCTION_LOAD_DEFAULT("log", log)
PORT_STD_MATH_FUNCTION_REAL1(log10)
META_FUNCTION_LOAD_DEFAULT("log10", log10)
PORT_STD_MATH_FUNCTION_REAL2(pow)
META_FUNCTION_LOAD_DEFAULT("pow", pow)

static
real_value_type
ldexp(const real_value_type x, const int_value_type y) {
	return std::ldexp(x, y);
}
META_FUNCTION_LOAD_DEFAULT("ldexp", ldexp)

#if 0
PORT_STD_MATH_FUNCTION_REAL2(frexp)
META_FUNCTION_LOAD_DEFAULT("frexp", frexp)
#endif

// trigonometric
PORT_STD_MATH_FUNCTION_REAL1(sin)
META_FUNCTION_LOAD_DEFAULT("sin", sin)
PORT_STD_MATH_FUNCTION_REAL1(cos)
META_FUNCTION_LOAD_DEFAULT("cos", cos)
PORT_STD_MATH_FUNCTION_REAL1(tan)
META_FUNCTION_LOAD_DEFAULT("tan", tan)
PORT_STD_MATH_FUNCTION_REAL1(asin)
META_FUNCTION_LOAD_DEFAULT("asin", asin)
PORT_STD_MATH_FUNCTION_REAL1(acos)
META_FUNCTION_LOAD_DEFAULT("acos", acos)
PORT_STD_MATH_FUNCTION_REAL1(atan)
META_FUNCTION_LOAD_DEFAULT("atan", atan)

// hyperbolic
PORT_STD_MATH_FUNCTION_REAL1(sinh)
META_FUNCTION_LOAD_DEFAULT("sinh", sinh)
PORT_STD_MATH_FUNCTION_REAL1(cosh)
META_FUNCTION_LOAD_DEFAULT("cosh", cosh)
PORT_STD_MATH_FUNCTION_REAL1(tanh)
META_FUNCTION_LOAD_DEFAULT("tanh", tanh)

#if 0
void
foo(void) {
	string foon;
//	const result_type<&fabs>::type bar;
	TYPEOF(foon) bar;
	typedef RESULT_OF(sqrt) something;
}
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

DEFAULT_STATIC_TRACE_BEGIN
