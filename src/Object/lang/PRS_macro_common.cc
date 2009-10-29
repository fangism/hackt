/**
	\file "Object/lang/PRS_macro_common.cc"
	Definition of tool-independent parts of PRS macro classes.  
	$Id: PRS_macro_common.cc,v 1.5 2009/10/29 23:00:26 fang Exp $
 */

#include <iostream>
#include <vector>
#include <set>
#include "Object/lang/PRS_macro_common.h"
#include "util/memory/count_ptr.h"
#include "util/attributes.h"
#include "Object/expr/preal_const.h"
#include "Object/expr/pint_const.h"

namespace HAC {
namespace entity {
namespace PRS {
namespace macros {
#include "util/using_ostream.h"
using util::memory::count_ptr;

//=============================================================================
// helper functions common to many macros' check functions

/**
	Currently no macros are allowed to take grouped arguments
	that result in more than one node.  
 */
static
good_bool
__no_grouped_node_args(const char* name, const node_args_type& a) {
	typedef node_args_type::const_iterator  const_iterator;
	const_iterator i(a.begin()), e(a.end());
	for ( ; i!=e; ++i) {
		const size_t s = i->size();
		if (s > 1) {
			cerr << "PRS macro \'" << name <<
				"\' takes no grouped arguments." << endl;
			cerr << "\tgot: " << s <<
				" nodes in argument position " <<
				distance(a.begin(), i) +1 << endl;
			return good_bool(false);
		}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
// current unused
static
good_bool
__optional_width_length_params(const char* m, const size_t s) {
	if (s > 2) {
		cerr << "Error: the \'" << m <<
			"\' macro takes 0-2 parameters." << endl;
		return good_bool(false);
	} else {
		return good_bool(true);
	}
}
#else
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
good_bool
__optional_width_length_type_params(const char* m, const size_t s) {
	if (s > 3) {
		cerr << "Error: the \'" << m <<
			"\' macro takes 0-3 parameters." << endl;
		return good_bool(false);
	} else {
		return good_bool(true);
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
good_bool
__takes_no_parameters(const char* m, const size_t s) __ATTRIBUTE_UNUSED__;

good_bool
__takes_no_parameters(const char* m, const size_t s) {
	if (s) {
		cerr << "Error: the \'" << m <<
			"\' macro takes no parameters." << endl;
		return good_bool(false);
	} else  return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
good_bool
__takes_no_parameters(const char* m, const param_args_type& p) __ATTRIBUTE_UNUSED__;

good_bool
__takes_no_parameters(const char* m, const param_args_type& p) {
	return __takes_no_parameters(m, p.size());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	\ret 'good' if value is > 0
	\param v pointer to value to check.
	\pre v must be non-NULL.
 */
static
good_bool
__param_must_be_positive(const param_args_type::value_type& v) {
	NEVER_NULL(v);
	const count_ptr<const preal_const>
		r(v.is_a<const preal_const>());
	const count_ptr<const pint_const>
		z(v.is_a<const pint_const>());
	if (r) {
		const preal_value_type S = r->static_constant_value();
		if (S <= 0.0) {
			cerr << "Error: value must be positive, "
				"but got: " << S << "." << endl;
			return good_bool(false);
		} else {
			return good_bool(true);
		}
	} else if (z) {
		const pint_value_type S = z->static_constant_value();
		if (S < 1) {
			cerr << "Error: value must be positive, "
				"but got: " << S << "." << endl;
			return good_bool(false);
		} else {
			return good_bool(true);
		}
	} else {
		cerr << "Error: expected int or real value." << endl;
		return good_bool(false);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre p has 0 or 1 parameters, this only checks the first.  
 */
static
good_bool
__optional_width_length_must_be_positive(
		const char* m, const param_args_type& p) {
	bool err = false;
	param_args_type::const_iterator i(p.begin()), e(p.end());
	for ( ; i!=e; ++i) {
		const count_ptr<const const_param>& s(*i);
		if (s && !__param_must_be_positive(s).good) {
			err = true;
		}
	}
	if (err) {
		cerr << "Error in \'" << m << "\' macro." << endl;
		return good_bool(false);
	}
	return good_bool(true);
}
#endif

//=============================================================================
/**     
	Default check for number of parameter arguments.  
 */     
#define DEFINE_DEFAULT_PRS_MACRO_CHECK_NUM_PARAMS(class_name)		\
good_bool								\
class_name::__check_num_params(const char*, const size_t) {		\
	return good_bool(true);						\
}

/**
	Default check for number of node group arguments.  
 */
#define	DEFINE_DEFAULT_PRS_MACRO_CHECK_NUM_NODES(class_name)		\
good_bool								\
class_name::__check_num_nodes(const char*, const size_t) {		\
	return good_bool(true);						\
}

/**
	Default checking for parameter argument constraints.  
 */
#define	DEFINE_DEFAULT_PRS_MACRO_CHECK_PARAMS(class_name)		\
good_bool								\
class_name::__check_param_args(const char*, const param_args_type&) {	\
	return good_bool(true);						\
}

/**
	Default checking for parameter arguments constraints.  
 */
#define	DEFINE_DEFAULT_PRS_MACRO_CHECK_NODES(class_name)		\
good_bool								\
class_name::__check_node_args(const char* name, const node_args_type& a) { \
	return __no_grouped_node_args(name, a);				\
}

//-----------------------------------------------------------------------------
// class Echo static method definitions

DEFINE_DEFAULT_PRS_MACRO_CHECK_NUM_PARAMS(Echo)
DEFINE_DEFAULT_PRS_MACRO_CHECK_NUM_NODES(Echo)
DEFINE_DEFAULT_PRS_MACRO_CHECK_PARAMS(Echo)
// TODO: allow grouped node arguments!
DEFINE_DEFAULT_PRS_MACRO_CHECK_NODES(Echo)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// class PassN and PassP static method definitions

good_bool
PassN::__check_num_params(const char* name, const size_t n) {
	return __optional_width_length_type_params(name, n);
}

good_bool
PassP::__check_num_params(const char* name, const size_t n) {
	return __optional_width_length_type_params(name, n);
}

good_bool
PassN::__check_param_args(const char* name, const param_args_type& p) {
	return good_bool(true);
}

good_bool
PassP::__check_param_args(const char* name, const param_args_type& p) {
	return good_bool(true);

}

/**
	Exactly 3 arguments: gate, source, drain.
 */
good_bool
PassN::__check_num_nodes(const char* name, const size_t n) {
	const bool good = (n == 3);
	if (!good) {
		cerr << name <<
			"() requires exactly 3 arguments: gate, source, drain."
			<< endl;
	}
	return good_bool(good);
}

/**
	Exactly 3 arguments: gate, source, drain.
 */
good_bool
PassP::__check_num_nodes(const char* name, const size_t n) {
	const bool good = (n == 3);
	if (!good) {
		cerr << name <<
			"() requires exactly 3 arguments: gate, source, drain."
			<< endl;
	}
	return good_bool(good);
}

/**
	No other constraints on the nodes.  
	TODO: check to make sure each group argument contains only one node.  
 */
DEFINE_DEFAULT_PRS_MACRO_CHECK_NODES(PassN)
DEFINE_DEFAULT_PRS_MACRO_CHECK_NODES(PassP)


//=============================================================================
}	// end namespace macros
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

