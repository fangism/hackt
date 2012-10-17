/**
	\file "Object/lang/SPEC_registry.cc"
	Definitions of spec directives belong here.  
	$Id: SPEC_common.cc,v 1.11 2011/02/02 23:54:21 fang Exp $
 */

#include <iostream>
#include <vector>
#include <set>
#include "Object/lang/SPEC_common.hh"
#include "Object/expr/const_param_expr_list.hh"
#include "Object/lang/cflat_context_visitor.hh"
#include "Object/traits/bool_traits.hh"
#include "Object/traits/proc_traits.hh"

namespace HAC {
namespace entity {
namespace SPEC {
namespace directives {
#include "util/using_ostream.hh"

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

#define	DEFINE_SPEC_DIRECTIVE_META_TYPE(class_name, Tag)		\
const meta_type_tag_enum class_name::type =				\
	class_traits<Tag>::type_tag_enum_value;

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
UnAliased_base::__check_num_params(const char* name, const size_t s) {
	return __takes_no_params(name, s);
}

good_bool
UnAliased_base::__check_num_nodes(const char* name, const size_t s) {
	return min_num_nodes(name, 2, s);
}

DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_PARAMS(UnAliased_base)

/**
	Allowed to take grouped arguments.  
	\return bad if any nodes in different groups are aliased.  
 */
good_bool
UnAliased_base::__check_node_args(const char* name, const node_args_type& a) {
	return good_bool(true);
}

DEFINE_SPEC_DIRECTIVE_META_TYPE(UnAliased_base, bool_tag)
/**
	Checks to make sure that no nodes in different group arguments
	are aliased to each other.  
	This may be called from any other function.  
 */
good_bool
UnAliased::__main(cflat_context_visitor& v, const node_args_type& n) {
	// does nothing but checks
	typedef	node_args_type::value_type	node_group_type;
	typedef	node_args_type::const_iterator	source_iterator;
	typedef	node_args_type::iterator	dest_iterator;
	node_args_type resolved_node_groups(n.size());
{
	source_iterator i(n.begin()), e(n.end());
	dest_iterator j(resolved_node_groups.begin());
	// std::transform pattern
	for ( ; i!=e; ++i, ++j) {
		v.__resolve_unique_literal_group<bool_tag>(*i, *j);
	}
}
{
	source_iterator i(resolved_node_groups.begin()),
		e(resolved_node_groups.end());
	// accumulate all nodes in this set
	// TODO: [algo] can't we just use set_intersection?
	node_group_type temp(*i);
	for (++i; i!=e; ++i) {
		typedef node_group_type::const_iterator set_iterator;
		set_iterator ii(i->begin()), ie(i->end());
		for ( ; ii!=ie; ++ii) {
			if (temp.insert(*ii).second) {
				// error message?
				return good_bool(false);
			}
		}
	}
}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DEFINE_SPEC_DIRECTIVE_META_TYPE(Assert, bool_tag)

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
	const bool t = const_param_expr_list::is_all_true(a);
	if (!t) {
		cerr << "ERROR: " << name << " spec directive failed." << endl;
		// already resolved to constant at this point
		// TODO: back-reference to source
	}
	return good_bool(t);
}

good_bool
Assert::__check_node_args(const char* name, const node_args_type&) {
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DEFINE_SPEC_DIRECTIVE_META_TYPE(LVS_exclhi, bool_tag)

good_bool
LVS_exclhi::__check_num_params(const char* name, const size_t s) {
	return __takes_no_params(name, s);
}

/**
	Accept one argument (may be group or aggregate).  
 */
good_bool
LVS_exclhi::__check_num_nodes(const char* name, const size_t s) {
	return min_num_nodes(name, 1, s);
}

DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_PARAMS(LVS_exclhi)

// make sure nodes aren't aliased accidentally?
#if 0
DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_NODES(LVS_exclhi)
#else
/**
	Allow any arbitrary grouping, just treat all nodes in all groups
	as one flat list for one exclusive ring.  
 */
good_bool
LVS_exclhi::__check_node_args(const char* name, const node_args_type& a) {
	return good_bool(true);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DEFINE_SPEC_DIRECTIVE_META_TYPE(LVS_excllo, bool_tag)

good_bool
LVS_excllo::__check_num_params(const char* name, const size_t s) {
	return __takes_no_params(name, s);
}

/**
	Accept one argument (may be group or aggregate).  
 */
good_bool
LVS_excllo::__check_num_nodes(const char* name, const size_t s) {
	return min_num_nodes(name, 1, s);
}

DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_PARAMS(LVS_excllo)

// make sure nodes aren't aliased accidentally?
#if 0
DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_NODES(LVS_excllo)
#else
/**
	Allow any arbitrary grouping, just treat all nodes in all groups
	as one flat list for one exclusive ring.  
 */
good_bool
LVS_excllo::__check_node_args(const char* name, const node_args_type& a) {
	return good_bool(true);
}
#endif

//-----------------------------------------------------------------------------
DEFINE_SPEC_DIRECTIVE_META_TYPE(LVS_BDD_order, bool_tag)

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
/**
	This should be deprecated in favor of using node attributes.
 */
DEFINE_SPEC_DIRECTIVE_META_TYPE(LVS_unstaticized, bool_tag)

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
DEFINE_SPEC_DIRECTIVE_META_TYPE(LVS_cross_coupled_inverters, bool_tag)

good_bool
LVS_cross_coupled_inverters::__check_num_params(
		const char* name, const size_t s) {
	return __takes_no_params(name, s);
}

good_bool
LVS_cross_coupled_inverters::__check_num_nodes(
		const char* name, const size_t s) {
	return exact_num_nodes(name, 2, s);
}

DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_PARAMS(LVS_cross_coupled_inverters)
DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_NODES(LVS_cross_coupled_inverters)

//-----------------------------------------------------------------------------
DEFINE_SPEC_DIRECTIVE_META_TYPE(SIM_force_exclhi, bool_tag)

good_bool
SIM_force_exclhi::__check_num_params(const char* name, const size_t s) {
	return __takes_no_params(name, s);
}

good_bool
SIM_force_exclhi::__check_num_nodes(const char* name, const size_t s) {
	return min_num_nodes(name, 1, s);
}

DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_PARAMS(SIM_force_exclhi)
// TODO: make sure node arguments aren't actually aliased?
#if 0
DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_NODES(SIM_force_exclhi)
#else
good_bool
SIM_force_exclhi::__check_node_args(const char* name, const node_args_type& a) {
	return good_bool(true);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DEFINE_SPEC_DIRECTIVE_META_TYPE(SIM_force_excllo, bool_tag)

good_bool
SIM_force_excllo::__check_num_params(const char* name, const size_t s) {
	return __takes_no_params(name, s);
}

good_bool
SIM_force_excllo::__check_num_nodes(const char* name, const size_t s) {
	return min_num_nodes(name, 1, s);
}

DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_PARAMS(SIM_force_excllo)
// TODO: make sure node arguments aren't actually aliased?
#if 0
DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_NODES(SIM_force_excllo)
#else
good_bool
SIM_force_excllo::__check_node_args(const char* name, const node_args_type& a) {
	return good_bool(true);
}
#endif

//-----------------------------------------------------------------------------
DEFINE_SPEC_DIRECTIVE_META_TYPE(layout_min_sep, bool_tag)

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
/**
	Specifies minimum physical separation between two sets of processes.
 */
DEFINE_SPEC_DIRECTIVE_META_TYPE(layout_min_sep_proc, process_tag)

/**
	Namespace for layout directives.  
 */
good_bool
layout_min_sep_proc::__check_num_params(const char* name, const size_t s) {
	return exact_num_params(name, 1, s);
}

good_bool
layout_min_sep_proc::__check_num_nodes(const char* name, const size_t s) {
//	return min_num_nodes(name, 2, s);
	return exact_num_nodes(name, 2, s);
}

DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_PARAMS(layout_min_sep_proc)

/**
	Grouped arguments ARE allowed here.  
	TODO: could check that same node doesn't appear in different groups...
 */
good_bool
layout_min_sep_proc::__check_node_args(
		const char* name, const node_args_type& a) {
	return good_bool(true);
}

//-----------------------------------------------------------------------------
// sypply_x method definitions

DEFINE_SPEC_DIRECTIVE_META_TYPE(supply_x, bool_tag)

good_bool
supply_x::__check_num_params(const char* name, const size_t s) {
	return exact_num_params(name, 0, s);
}

good_bool
supply_x::__check_num_nodes(const char* name, const size_t s) {
	return exact_num_nodes(name, 2, s);
}

DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_PARAMS(supply_x)

/**
	TODO: check?
 */
good_bool
supply_x::__check_node_args(const char* name, const node_args_type& a) {
	return good_bool(true);
}

//-----------------------------------------------------------------------------
// RunModeStatic method definitions

DEFINE_SPEC_DIRECTIVE_META_TYPE(RunModeStatic, bool_tag)

good_bool
RunModeStatic::__check_num_params(const char* name, const size_t s) {
	return exact_num_params(name, 0, s);
}

good_bool
RunModeStatic::__check_num_nodes(const char* name, const size_t s) {
	return exact_num_nodes(name, 1, s);
}

DEFINE_DEFAULT_SPEC_DIRECTIVE_CHECK_PARAMS(RunModeStatic)

/**
	nothing to check
 */
good_bool
RunModeStatic::__check_node_args(const char* name, const node_args_type& a) {
	return good_bool(true);
}

//-----------------------------------------------------------------------------
}	// end namespace directives
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

