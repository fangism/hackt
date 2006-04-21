/**
	\file "Object/lang/SPEC_registry.cc"
	Definitions of spec directives belong here.  
	$Id: SPEC_registry.cc,v 1.9.2.2 2006/04/21 02:45:57 fang Exp $
 */

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <iostream>
#include <vector>
#include <set>
#include "Object/lang/SPEC_registry.tcc"
#include "Object/lang/SPEC_common.h"
#include "Object/lang/directive_base.h"
#include "Object/lang/cflat_printer.h"
#include "Object/expr/const_param.h"
#include "Object/expr/const_param_expr_list.h"
#include "main/cflat_options.h"
#include "common/TODO.h"
#include "util/qmap.tcc"

namespace util {
//=============================================================================
// explicit template instantiations
#if 1
template class qmap<std::string, HAC::entity::SPEC::cflat_spec_definition_entry>;
#else
template cflat_spec_registry_type;
#endif
//=============================================================================
}	// end namespace util

namespace HAC {
namespace entity {
namespace SPEC {
using util::qmap;
#include "util/using_ostream.h"

// explicit template instantiation
template class spec_visitor_entry<cflat_prs_printer>;

//=============================================================================
/**
	Global static initialization.
	This must appear first before any registrations take place in this 
	translation unit.
 */
const cflat_spec_registry_type
cflat_spec_registry;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Local static modifiable reference to use with registration.  
 */
static
cflat_spec_registry_type& __cflat_spec_registry(
	const_cast<cflat_spec_registry_type&>(cflat_spec_registry));

//=============================================================================
// class cflat_spec_definition_entry method definitions

//=============================================================================
/**
	Macro class registration function.  
 */
template <class T>
static
size_t
register_cflat_spec_class(void) {
	typedef	cflat_spec_registry_type::iterator	iterator;
	typedef	cflat_spec_registry_type::mapped_type	mapped_type;
	const string k(T::name);
	mapped_type& m(__cflat_spec_registry[k]);
	if (m) {
		cerr << "Error: spec directive by the name \'" << k <<
			"\' has already been registered!" << endl;
		THROW_EXIT;
	}
	m = cflat_spec_definition_entry(k, &T::main,
		&T::check_num_params, &T::check_num_nodes,
		&T::check_param_args, &T::check_node_args);
	// oddly, this is needed to force instantiation of the [] const operator
	const mapped_type& n
		__ATTRIBUTE_UNUSED_CTOR__((cflat_spec_registry[k]));
	INVARIANT(n);
	return cflat_spec_registry.size();
}

//=============================================================================
/**
	Convenient home for user-defined spec classes.  
	Could make this namespace anonymous...
 */
namespace cflat_specs {
typedef	cflat_spec_definition_entry::node_args_type	node_args_type;
typedef	cflat_spec_definition_entry::param_args_type	param_args_type;

/**
	Convenient spec for declaring spec classes.
	TODO: consider adding a description string.  
 */
#define	DECLARE_AND_DEFINE_CFLAT_SPEC_DIRECTIVE_CLASS(class_name, spec_name) \
	DECLARE_SPEC_DIRECTIVE_CLASS(class_name, cflat_prs_printer)	\
	DEFINE_SPEC_DIRECTIVE_CLASS(class_name, spec_name, 		\
		register_cflat_spec_class)

//-----------------------------------------------------------------------------

/**
	Blatantly copied from PRS_macro_registry.cc.
	\param delim the delimiter between groups
	\param gl group's left wrapper
	\param gd group's inner delimiter
	\param gr group's right wrapper
 */
static
ostream&
print_node_args_list(cflat_prs_printer& p, const node_args_type& nodes,
		const char* delim,
		const char* gl, const char* gd, const char* gr) {
	typedef	node_args_type::const_iterator		const_iterator;
	NEVER_NULL(delim);
	ostream& o(p.os);
	const_iterator i(nodes.begin());
	const const_iterator e(nodes.end());
	INVARIANT(i!=e);
	p.__dump_canonical_literal_group(*i, gl, gd, gr);
	for (++i; i!=e; ++i) {
		o << delim;
		p.__dump_canonical_literal_group(*i, gl, gd, gr);
	}
	return o;
}

static
ostream&
print_node_args_list(cflat_prs_printer& p, const node_args_type& nodes, 
		const char* delim) {
	return print_node_args_list(p, nodes, delim, "{", ",", "}");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is the default thing to do, much too common and reusable.  
 */
template <class T>
static
ostream&
default_spec_output(cflat_prs_printer& p, const param_args_type& params, 
		const node_args_type& a) {
	ostream& o(p.os);
	o << T::name;
	directive_base::dump_params(params, o);
	o << '(';
	print_node_args_list(p, a, ", ");
	return o << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Expand grouped attribute directives to one-per-node.  
	Automatically separates with newlines.  
	NOTE: doesn't take any parameters.  
 */
template <class T>
static
ostream&
default_expand_into_singles_output(cflat_prs_printer& p, 
		const node_args_type& a) {
	typedef	node_args_type::const_iterator		const_iterator;
	ostream& o(p.os);
	const_iterator i(a.begin());
	const const_iterator e(a.end());
	for ( ; i!=e; ++i) {
		o << T::name << '(';
		p.__dump_canonical_literal_group(*i);
		o << ')' << endl;
	}
	return o;
}

//-----------------------------------------------------------------------------
//
//	Define spec directives HERE.  
//
//-----------------------------------------------------------------------------
/**
	Possibly think of a better name for this directive:
	'distinct', 'disjoint', 'unconnected'
 */
DECLARE_AND_DEFINE_CFLAT_SPEC_DIRECTIVE_CLASS(UnAliased, "unaliased")

/**
	Does nothing, the sole purpose of this directive is to issue
	an error if any nodes in different group arguments 
	are aliased to each other.  
	TODO: put this along with other checking functions into a base class.  
	\throw generic exception if there is an alias violation.  
 */
void
UnAliased::main(cflat_prs_printer& p, const param_args_type& a,
		const node_args_type& n) {
	// does nothing but checks
	typedef	node_args_type::value_type	node_group_type;
	typedef	node_args_type::const_iterator	source_iterator;
	typedef	node_args_type::iterator	dest_iterator;
	node_args_type resolved_node_groups(a.size());
{
	source_iterator i(n.begin()), e(n.end());
	dest_iterator j(resolved_node_groups.begin());
	// std::transform pattern
	for ( ; i!=e; ++i, ++j) {
		p.__resolve_unique_literal_group(*i, *j);
	}
}
{
	source_iterator i(resolved_node_groups.begin()),
		e(resolved_node_groups.end());
	// accumulate all nodes in this set
	node_group_type temp(*i);
	for (++i; i!=e; ++i) {
		typedef	node_group_type::const_iterator	set_iterator;
		set_iterator ii(i->begin()), ie(i->end());
		for ( ; ii!=ie; ++ii) {
			if (temp.insert(*ii).second) {
				cerr << "Error: detected aliased nodes during "
					"processing of \'unaliased\' directive."
					<< endl;
				THROW_EXIT;
			}
		}
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_CFLAT_SPEC_DIRECTIVE_CLASS(Assert, "assert")

void
Assert::main(cflat_prs_printer& p, const param_args_type& a, 
		const node_args_type&) {
	// does nothing
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_CFLAT_SPEC_DIRECTIVE_CLASS(LVS_exclhi, "exclhi")
DECLARE_AND_DEFINE_CFLAT_SPEC_DIRECTIVE_CLASS(LVS_excllo, "excllo")

/**
	exclhi -- for LVS: asserts that a set of nodes may only
		contain one logic high value.  
		Useful for charge-sharing and sneak-path analysis.  
 */
void
LVS_exclhi::main(cflat_prs_printer& p, const param_args_type& v,
		const node_args_type& a) {
	switch (p.cfopts.primary_tool) {
	case cflat_options::TOOL_LVS:
		// or other tools
		default_spec_output<this_type>(p, v, a) << endl;
		break;
	default:
		break;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	excllo -- for LVS: asserts that a set of nodes may only
		contain one logic low value.  
 */
void
LVS_excllo::main(cflat_prs_printer& p, const param_args_type& v,
		const node_args_type& a) {
	switch (p.cfopts.primary_tool) {
	case cflat_options::TOOL_LVS:
		// or other tools
		default_spec_output<this_type>(p, v, a) << endl;
		break;
	default:
		break;
	}
}

//-----------------------------------------------------------------------------
DECLARE_AND_DEFINE_CFLAT_SPEC_DIRECTIVE_CLASS(LVS_BDD_order, "order")

/**
	order -- for LVS: binary decision diagram ordering for 
		accelerating checking.  
 */
void
LVS_BDD_order::main(cflat_prs_printer& p, const param_args_type& v, 
		const node_args_type& a) {
	switch (p.cfopts.primary_tool) {
	case cflat_options::TOOL_LVS:
		// or other tools
		default_spec_output<this_type>(p, v, a) << endl;
		break;
	default:
		break;
	}
}

//-----------------------------------------------------------------------------
DECLARE_AND_DEFINE_CFLAT_SPEC_DIRECTIVE_CLASS(LVS_unstaticized, "unstaticized")

/**
	unstaticized -- for LVS: asserts that a set of nodes may only
		contain one logic high value.  
		Useful for charge-sharing and sneak-path analysis.  
	This automatically expands multiple arguments into 
		outputs on separate lines.  
 */
void
LVS_unstaticized::main(cflat_prs_printer& p, const param_args_type& v,
		const node_args_type& a) {
	switch (p.cfopts.primary_tool) {
	case cflat_options::TOOL_LVS:
		// or other tools
		default_expand_into_singles_output<this_type>(p, a);
		break;
	default:
		break;
	}
}

//-----------------------------------------------------------------------------
DECLARE_AND_DEFINE_CFLAT_SPEC_DIRECTIVE_CLASS(SIM_force_exclhi, "mk_exclhi")
DECLARE_AND_DEFINE_CFLAT_SPEC_DIRECTIVE_CLASS(SIM_force_excllo, "mk_excllo")

/**
	mk_exclhi -- for simulations only, 
		coerces exclusive high among nodes.  
 */
void
SIM_force_exclhi::main(cflat_prs_printer& p, const param_args_type& v,
		const node_args_type& a) {
	switch (p.cfopts.primary_tool) {
	case cflat_options::TOOL_PRSIM:
		// or other simulator tool
		default_spec_output<this_type>(p, v, a) << endl;
		break;
	default:
		break;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	mk_excllo -- for simulations only, 
		coerces exclusive high among nodes.  
 */
void
SIM_force_excllo::main(cflat_prs_printer& p, const param_args_type& v,
		const node_args_type& a) {
	switch (p.cfopts.primary_tool) {
	case cflat_options::TOOL_PRSIM:
		// or other simulator tool
		default_spec_output<this_type>(p, v, a) << endl;
		break;
	default:
		break;
	}
}

//-----------------------------------------------------------------------------
/**
	Namespace for layout directives.  
 */
namespace layout {

/**
	Default output formatting for layout directives.  
 */
template <class T>
static
ostream&
default_layout_spec_output(cflat_prs_printer& p, const param_args_type& params, 
		const node_args_type& nodes) {
	ostream& o(p.os);
	o << T::name;
	directive_base::dump_params(params, o);
	o << '(';
	print_node_args_list(p, nodes, "; ", NULL, ",", NULL);
	return o << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_CFLAT_SPEC_DIRECTIVE_CLASS(layout_min_sep, "min_sep")

/**
	\param a node arguments are processed in groups, so e.g.
		min_sep({a,b},{c,d})
		groups are {a,b} and {c,d}.  
 */
void
layout_min_sep::main(cflat_prs_printer& p, const param_args_type& v, 
		const node_args_type& a) {
	switch (p.cfopts.primary_tool) {
	case cflat_options::TOOL_LAYOUT:
		default_layout_spec_output<this_type>(p, v, a) << endl;
		break;
	case cflat_options::TOOL_PRSIM:
		// but not for old plain prsim
		if (p.cfopts.with_SEU()) {
			default_layout_spec_output<this_type>(p, v, a) << endl;
		}
		break;
	default:
		break;
	}
}

}	// end namespace layout
//-----------------------------------------------------------------------------
#undef	DECLARE_AND_DEFINE_CFLAT_SPEC_DIRECTIVE_CLASS
}	// end namespace cflat_specs

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

