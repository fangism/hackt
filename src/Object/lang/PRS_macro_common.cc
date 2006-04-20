/**
	\file "Object/lang/PRS_macro_common.cc"
	Definition of tool-independent parts of PRS macro classes.  
	$Id: PRS_macro_common.cc,v 1.1.2.1 2006/04/20 03:34:50 fang Exp $
 */

#include <iostream>
#include <vector>
#include <set>
#include "Object/lang/PRS_macro_common.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
namespace PRS {
namespace macros {
#include "util/using_ostream.h"

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
static
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
__takes_no_parameters(const char* m, const param_args_type& p) {
	return __takes_no_parameters(m, p.size());
}

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
	return __takes_no_parameters(name, n);
}

good_bool
PassP::__check_num_params(const char* name, const size_t n) {
	return __takes_no_parameters(name, n);
}

good_bool
PassN::__check_param_args(const char* name, const param_args_type& p) {
	return __takes_no_parameters(name, p);
}

good_bool
PassP::__check_param_args(const char* name, const param_args_type& p) {
	return __takes_no_parameters(name, p);
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

