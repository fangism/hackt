/**
	\file "Object/lang/SPEC_registry.cc"
	Definitions of spec directives belong here.  
	$Id: SPEC_common.cc,v 1.1.2.1 2006/04/20 03:34:51 fang Exp $
 */

#include <iostream>
#include <vector>
#include <set>
#include "Object/lang/SPEC_common.h"
#include "Object/expr/const_param_expr_list.h"

namespace HAC {
namespace entity {
namespace SPEC {
namespace directives {
#include "util/using_ostream.h"

/**
	Default check for number of arguments.  
 */
#define	DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_NUM_PARAMS(class_name)	\
good_bool								\
class_name::__check_num_params(const char*, const size_t) {		\
	return good_bool(true);						\
}

#define	DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_NUM_NODES(class_name)	\
good_bool								\
class_name::__check_num_nodes(const char*, const size_t) {		\
	return good_bool(true);						\
}

#define	DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_PARAMS(class_name)		\
good_bool								\
class_name::__check_param_args(const char*, const param_args_type&) {	\
	return good_bool(true);						\
}

#define	DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_NODES(class_name)		\
good_bool								\
class_name::__check_node_args(const char* name, const node_args_type& a) { \
	return __no_grouped_node_args(name, a);				\
}

//-----------------------------------------------------------------------------
/**
	Reusable function for specifying the minimum number of arguments.  
 */
static
good_bool
__takes_no_params(const char* name, const size_t args) {
	if (args) {
		cerr << "The \'" << name <<
			"\' directive takes no parameter arguments." << endl;
		return good_bool(false);
	} else	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reusable function for specifying the minimum number of arguments.  
 */
static
good_bool
min_num_params(const char* name, const size_t min, const size_t args) {
	if (args < min) {
		cerr << "The \'" << name << "\' directive requires at least " <<
			min << " parameters." << endl;
		return good_bool(false);
	} else	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reusable function for specifying the exact number of arguments.  
 */
static
good_bool
exact_num_params(const char* name, const size_t req, const size_t args) {
	if (args != req) {
		cerr << "The \'" << name << "\' directive requires exactly " <<
			req << " parameters." << endl;
		return good_bool(false);
	} else	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reusable function for specifying the minimum number of arguments.  
 */
static
good_bool
min_num_nodes(const char* name, const size_t min, const size_t args) {
	if (args < min) {
		cerr << "The \'" << name << "\' directive requires at least " <<
			min << " arguments." << endl;
		return good_bool(false);
	} else	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reusable function for specifying the exact number of arguments.  
 */
static
good_bool
exact_num_nodes(const char* name, const size_t req, const size_t args) {
	if (args != req) {
		cerr << "The \'" << name << "\' directive requires exactly " <<
			req << " arguments." << endl;
		return good_bool(false);
	} else	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**     
	Most spec directives expect only single nodes, even in grouped 
	arguments.  
 */
static
good_bool
__no_grouped_node_args(const char* name, const node_args_type& a) {
	typedef node_args_type::const_iterator		const_iterator;
	const_iterator i(a.begin()), e(a.end());
	for ( ; i!=e; ++i) {
		const size_t s = i->size();
		if (s > 1) {
			cerr << "SPEC directive \'" << name <<
				"\' takes no grouped arguments." << endl;
			cerr << "\tgot: " << s <<
				" nodes in argument position " <<
				distance(a.begin(), i) +1 << endl;
			return good_bool(false);
		}
	}
	return good_bool(true); 
}       

//-----------------------------------------------------------------------------
//
//	Define spec directives HERE.  
//
//-----------------------------------------------------------------------------

good_bool
UnAliased::__check_num_params(const char* name, const size_t s) {
	return __takes_no_params(name, s);
}

good_bool
UnAliased::__check_num_nodes(const char* name, const size_t s) {
	return min_num_nodes(name, 2, s);
}

DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_PARAMS(UnAliased)

/**
	Allowed to take grouped arguments.  
	\return bad if any nodes in different groups are aliased.  
 */
good_bool
UnAliased::__check_node_args(const char* name, const node_args_type& a) {
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
	Takes any number of parameters, checks that they are all true later.  
 */
good_bool
Assert::__check_num_params(const char* name, const size_t s) {
	return min_num_params(name, 1, s);
}

/**
	This spec directive in unique in that it takes no node arguments.  
 */
good_bool
Assert::__check_num_nodes(const char* name, const size_t s) {
	return exact_num_nodes(name, 0, s);
}

/**
	For boolean expressions, makes sure that value is 'true'.
	For integer expessions, makes sure that value is non-zero.
	Does not accept preal-const, floating point arguments.  
 */
good_bool
Assert::__check_param_args(const char* name, const param_args_type& a) {
	return good_bool(const_param_expr_list::is_all_true(a));
}

good_bool
Assert::__check_node_args(const char* name, const node_args_type&) {
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
LVS_exclhi::__check_num_params(const char* name, const size_t s) {
	return __takes_no_params(name, s);
}

good_bool
LVS_exclhi::__check_num_nodes(const char* name, const size_t s) {
	return min_num_nodes(name, 2, s);
}

DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_PARAMS(LVS_exclhi)
// make sure nodes aren't aliased accidentally?
DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_NODES(LVS_exclhi)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
LVS_excllo::__check_num_params(const char* name, const size_t s) {
	return __takes_no_params(name, s);
}

good_bool
LVS_excllo::__check_num_nodes(const char* name, const size_t s) {
	return min_num_nodes(name, 2, s);
}

DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_PARAMS(LVS_excllo)
// make sure nodes aren't aliased accidentally?
DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_NODES(LVS_excllo)

//-----------------------------------------------------------------------------
good_bool
LVS_BDD_order::__check_num_params(const char* name, const size_t s) {
	return __takes_no_params(name, s);
}

good_bool
LVS_BDD_order::__check_num_nodes(const char* name, const size_t s) {
	return min_num_nodes(name, 2, s);
}

DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_PARAMS(LVS_BDD_order)
// make sure nodes aren't aliased accidentally?
DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_NODES(LVS_BDD_order)

//-----------------------------------------------------------------------------
good_bool
LVS_unstaticized::__check_num_params(const char* name, const size_t s) {
	return __takes_no_params(name, s);
}

good_bool
LVS_unstaticized::__check_num_nodes(const char* name, const size_t s) {
	return min_num_nodes(name, 1, s);
}

DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_PARAMS(LVS_unstaticized)
DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_NODES(LVS_unstaticized)

//-----------------------------------------------------------------------------
good_bool
SIM_force_exclhi::__check_num_params(const char* name, const size_t s) {
	return __takes_no_params(name, s);
}

good_bool
SIM_force_exclhi::__check_num_nodes(const char* name, const size_t s) {
	return min_num_nodes(name, 2, s);
}

DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_PARAMS(SIM_force_exclhi)
// make sure node arguments aren't actually aliased?
DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_NODES(SIM_force_exclhi)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
SIM_force_excllo::__check_num_params(const char* name, const size_t s) {
	return __takes_no_params(name, s);
}

good_bool
SIM_force_excllo::__check_num_nodes(const char* name, const size_t s) {
	return min_num_nodes(name, 2, s);
}

DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_PARAMS(SIM_force_excllo)
// make sure node arguments aren't actually aliased?
DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_NODES(SIM_force_excllo)

//-----------------------------------------------------------------------------
/**
	Namespace for layout directives.  
 */
good_bool
layout_min_sep::__check_num_params(const char* name, const size_t s) {
	return exact_num_params(name, 1, s);
}

good_bool
layout_min_sep::__check_num_nodes(const char* name, const size_t s) {
//	return min_num_nodes(name, 2, s);
	return exact_num_nodes(name, 2, s);
}

DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_PARAMS(layout_min_sep)

/**
	Grouped arguments ARE allowed here.  
	TODO: could check that same node doesn't appear in different groups...
 */
good_bool
layout_min_sep::__check_node_args(const char* name, const node_args_type& a) {
	return good_bool(true);
}

//-----------------------------------------------------------------------------
}	// end namespace directives
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

