/**
	\file "Object/lang/SPEC_registry.cc"
	Definitions of spec directives belong here.  
	$Id: SPEC_registry.cc,v 1.26 2010/09/02 00:34:38 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include "util/static_trace.hh"
DEFAULT_STATIC_TRACE_BEGIN

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include "Object/lang/SPEC_registry.tcc"
#include "Object/lang/SPEC_common.hh"
#include "Object/lang/directive_base.hh"
#include "Object/lang/cflat_printer.tcc"
#include "Object/expr/const_param.hh"
#include "Object/expr/const_param_expr_list.hh"
#include "main/cflat_options.hh"
#include "common/TODO.hh"
#include "util/stacktrace.hh"

namespace HAC {
namespace entity {
namespace SPEC {
#include "util/using_ostream.hh"

// explicit template instantiation
template class spec_visitor_entry<cflat_prs_printer>;

//=============================================================================

/**
	Local static modifiable reference to use with registration.  
 */
static
cflat_spec_registry_type __cflat_spec_registry;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Global static initialization.
	This must appear first before any registrations take place in this 
	translation unit.
 */
const cflat_spec_registry_type&
cflat_spec_registry(__cflat_spec_registry);

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
	typedef	cflat_spec_registry_type::value_type	value_type;
	const cflat_spec_definition_entry e(T::name, T::type, 
		&T::main,
		&T::check_num_params, &T::check_num_nodes,
		&T::check_param_args, &T::check_node_args);
	const value_type k(T::name, e);
	const std::pair<iterator, bool>
		m(__cflat_spec_registry.insert(k));
	if (!m.second) {
		cerr << "Error: spec directive by the name \'" << k.first <<
			"\' has already been registered!" << endl;
		THROW_EXIT;
	}
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
template <class Tag>
static
ostream&
print_args_list(cflat_prs_printer& p, const node_args_type& nodes,
		const char* delim,
		const char* gl, const char* gd, const char* gr) {
	typedef	node_args_type::const_iterator		const_iterator;
	NEVER_NULL(delim);
	ostream& o(p.os);
	const_iterator i(nodes.begin());
	const const_iterator e(nodes.end());
	INVARIANT(i!=e);
	p.template __dump_canonical_literal_group<Tag>(*i, gl, gd, gr);
	for (++i; i!=e; ++i) {
		o << delim;
		p.template __dump_canonical_literal_group<Tag>(*i, gl, gd, gr);
	}
	return o;
}

static
ostream&
print_node_args_list(cflat_prs_printer& p, const node_args_type& nodes,
		const char* delim,
		const char* gl, const char* gd, const char* gr) {
	return print_args_list<bool_tag>(p, nodes, delim, gl, gd, gr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapper using default output formatting.  
 */
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
	Flattens grouped node arguments into a single flat set.  
 */
template <class Tag>
static
void
flatten_canonicalized_set(cflat_context_visitor& p,
		const node_args_type& nodes, directive_node_group_type& o) {
	typedef	node_args_type::const_iterator		const_iterator;
	STACKTRACE_VERBOSE;
	const_iterator i(nodes.begin());
	const const_iterator e(nodes.end());
	for ( ; i!=e; ++i) {
		p.template __resolve_unique_literal_group<Tag>(*i, o);
	}
}

static
void
flatten_canonicalized_node_set(cflat_context_visitor& p,
		const node_args_type& nodes, directive_node_group_type& o) {
	flatten_canonicalized_set<bool_tag>(p, nodes, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
// enable this when it is actually used
/**
	Flattens arguments into single set, eliminating duplicates.  
	This variation prints the directive irrespective of the
	size of the node set.  
 */
template <class T>
static
ostream&
flatten_canonicalized_spec_output(cflat_prs_printer& p,
		const param_args_type& params, 
		const node_args_type& nodes) {
	directive_node_group_type node_set;
	flatten_canonicalized_node_set(p, nodes, node_set);
	{
		ostream& o(p.os);
		o << T::name;
		directive_base::dump_params(params, o);
		o << '(';
		p.__dump_resolved_literal_group<T>(node_set, "", ",", "");
		o << ')';
	}
	return o;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This variation prints the arguments if the canonical set
	has size greater than one.  
 */
template <class T, class Tag>
static
ostream&
flatten_canonicalized_spec_output_if_more_than_one(cflat_prs_printer& p,
		const param_args_type& params, 
		const node_args_type& nodes) {
	ostream& o(p.os);
	directive_node_group_type node_set;
	flatten_canonicalized_node_set(p, nodes, node_set);
	if (node_set.size() > 1) {
		o << T::name;
		directive_base::dump_params(params, o);
		o << '(';
		p.template __dump_resolved_literal_group<Tag>(
			node_set, "", ", ", "");
		o << ')';
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Expand grouped attribute directives to one-per-node.  
	Automatically separates with newlines.  
	NOTE: doesn't take any parameters.  
 */
template <class T, class Tag>
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
		p.template __dump_canonical_literal_group<Tag>(*i);
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
@texinfo spec/unaliased.texi
@deffn Directive unaliased nodes...
Usage: @samp{unaliased(...)}

Error out if any of @var{nodes} are aliased to each other.  
Tool-independent.  
Useful for verifying that certain nodes are not accidentally connected.
@end deffn
@end texinfo
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
UnAliased::main(cflat_prs_printer& p, const param_args_type&,
		const node_args_type& n) {
	STACKTRACE_VERBOSE;
	if (!__main(p, n).good) {
		cerr << "Error: detected aliased nodes during "
			"processing of \'unaliased\' directive."
			<< endl;
		THROW_EXIT;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo spec/assert.texi
@deffn Directive assert P
Usage: @samp{assert<P>()}

Error out if predicate expression @var{P} is false.
Note that this is a @emph{compile-time} check, which is enforced
during unroll/create compilation.  
Useful for enforcing parametric constraints.  
Tool-independent.
For run-time invariants, see @t{$(expr)}-syntax below.  
@end deffn
@end texinfo
***/
DECLARE_AND_DEFINE_CFLAT_SPEC_DIRECTIVE_CLASS(Assert, "assert")

void
Assert::main(cflat_prs_printer& p, const param_args_type& a, 
		const node_args_type& n) {
	// does nothing for hflat
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo spec/exclhi.texi
@deffn Directive exclhi nodes...
Usage: @samp{exclhi(...)}

Emits directives to check that @var{nodes} are mutually exclusive high
at run-time.  
(This corresponds to the old @t{CHECK_CHANNELS} method of checking 
for exclusivity.)
In @command{hacprsim}, these form checking rings.  
In @command{cflat lvs}, these directives affect 
charge-sharing and sneak-path analysis.  
@end deffn
@end texinfo

@texinfo spec/excllo.texi
@deffn Directive excllo nodes...
Usage: @samp{excllo(...)}

Emits directives to check that @var{nodes} are mutually exclusive low
at run-time.  
(This corresponds to the old @t{CHECK_CHANNELS} method of checking 
for exclusivity.)
In @command{hacprsim}, these form checking rings.  
In @command{cflat lvs}, these directives affect 
charge-sharing and sneak-path analysis.  
@end deffn
@end texinfo
***/
DECLARE_AND_DEFINE_CFLAT_SPEC_DIRECTIVE_CLASS(LVS_exclhi, "exclhi")
DECLARE_AND_DEFINE_CFLAT_SPEC_DIRECTIVE_CLASS(LVS_excllo, "excllo")

/**
	exclhi -- for LVS: asserts that a set of nodes may only
		contain one logic high value.  
		Useful for charge-sharing and sneak-path analysis.  
	TODO: is the default output in the correct format?
 */
void
LVS_exclhi::main(cflat_prs_printer& p, const param_args_type& v,
		const node_args_type& a) {
	STACKTRACE_VERBOSE;
	switch (p.cfopts.primary_tool) {
	case cflat_options::TOOL_LVS:
		// or other tools
		flatten_canonicalized_spec_output_if_more_than_one<
			this_type, bool_tag>(p, v, a) << endl;
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
	STACKTRACE_VERBOSE;
	switch (p.cfopts.primary_tool) {
	case cflat_options::TOOL_LVS:
		// or other tools
		flatten_canonicalized_spec_output_if_more_than_one<
			this_type, bool_tag>(p, v, a) << endl;
		break;
	default:
		break;
	}
}

//-----------------------------------------------------------------------------
/***
@texinfo spec/order.texi
@deffn Directive order nodes...
For @command{cflat lvs}, specify the node checking order for BDD algorithms.  
@end deffn
@end texinfo
***/
DECLARE_AND_DEFINE_CFLAT_SPEC_DIRECTIVE_CLASS(LVS_BDD_order, "order")

/**
	order -- for LVS: binary decision diagram ordering for 
		accelerating checking.  
 */
void
LVS_BDD_order::main(cflat_prs_printer& p, const param_args_type& v, 
		const node_args_type& a) {
	STACKTRACE_VERBOSE;
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
/***
@texinfo spec/unstaticized.texi
@deffn Directive unstaticized node
For @command{cflat lvs}, specify that node should remain unstaticized.  
@end deffn
@end texinfo
***/
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
	STACKTRACE_VERBOSE;
	switch (p.cfopts.primary_tool) {
	case cflat_options::TOOL_LVS:
		// or other tools
		default_expand_into_singles_output<this_type, bool_tag>(p, a);
		break;
	default:
		break;
	}
}

//-----------------------------------------------------------------------------
/***
@texinfo spec/cross_coupled_inverters.texi
@deffn Directive cross_coupled_inverters x y
For @command{cflat lvs}, just emit the directive back out with substituted
canonical node names.  
@end deffn
@end texinfo
***/
DECLARE_AND_DEFINE_CFLAT_SPEC_DIRECTIVE_CLASS(LVS_cross_coupled_inverters,
		"cross_coupled_inverters")

void
LVS_cross_coupled_inverters::main(cflat_prs_printer& p, 
		const param_args_type& v, const node_args_type& a) {
	STACKTRACE_VERBOSE;
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
/***
@texinfo spec/mk_exclhi.texi
@deffn Directive mk_exclhi nodes...
For @command{cflat prsim} and @command{hacprsim}, 
enforce logic-high mutual exclusion among @var{nodes}.  
This is often used in describing arbiters.  
@end deffn
@end texinfo

@texinfo spec/mk_excllo.texi
@deffn Directive mk_excllo nodes...
For @command{cflat prsim} and @command{hacprsim}, 
enforce logic-low mutual exclusion among @var{nodes}.  
This is often used in describing arbiters.  
@end deffn
@end texinfo
***/
DECLARE_AND_DEFINE_CFLAT_SPEC_DIRECTIVE_CLASS(SIM_force_exclhi, "mk_exclhi")
DECLARE_AND_DEFINE_CFLAT_SPEC_DIRECTIVE_CLASS(SIM_force_excllo, "mk_excllo")

/**
	mk_exclhi -- for simulations only, 
		coerces exclusive high among nodes.  
 */
void
SIM_force_exclhi::main(cflat_prs_printer& p, const param_args_type& v,
		const node_args_type& a) {
	STACKTRACE_VERBOSE;
	switch (p.cfopts.primary_tool) {
	case cflat_options::TOOL_PRSIM:
		// or other simulator tool
		flatten_canonicalized_spec_output_if_more_than_one<
			this_type, bool_tag>(p, v, a) << endl;
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
	STACKTRACE_VERBOSE;
	switch (p.cfopts.primary_tool) {
	case cflat_options::TOOL_PRSIM:
		// or other simulator tool
		flatten_canonicalized_spec_output_if_more_than_one<
			this_type, bool_tag>(p, v, a) << endl;
		break;
	default:
		break;
	}
}

//-----------------------------------------------------------------------------
DECLARE_AND_DEFINE_CFLAT_SPEC_DIRECTIVE_CLASS(supply_x, "supply_x")

void
supply_x::main(cflat_prs_printer& p, const param_args_type& v,
		const node_args_type& a) {
	STACKTRACE_VERBOSE;
	FINISH_ME(Fang);
}

//-----------------------------------------------------------------------------
/***
@texinfo spec/runmodestatic.texi
@deffn Directive runmodestatic node
For @command{cflat lvs},
mark this node as one that seldom or never changes value, 
for the sake of netlist analysis.
NOTE: eventually this directive will be deprecated in favor of
applying a proper node attribute.
The decision to implement this as a spec-directive is an unfortunate
consequence of compatibility with another tool.  
@end deffn
@end texinfo
***/
DECLARE_AND_DEFINE_CFLAT_SPEC_DIRECTIVE_CLASS(RunModeStatic, "runmodestatic")

void
RunModeStatic::main(cflat_prs_printer& p, const param_args_type& v,
		const node_args_type& a) {
	STACKTRACE_VERBOSE;
	switch (p.cfopts.primary_tool) {
	case cflat_options::TOOL_PRSIM:
		break;
	case cflat_options::TOOL_LVS:
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
template <class T, class Tag>
static
ostream&
default_layout_spec_output(cflat_prs_printer& p, const param_args_type& params, 
		const node_args_type& nodes) {
	ostream& o(p.os);
	o << T::name;
	directive_base::dump_params(params, o);
	o << '(';
	print_args_list<Tag>(p, nodes, "; ", NULL, ",", NULL);
	return o << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo spec/min_sep.texi
@deffn Directive min_sep dist nodes...
@deffnx Directive min_sep_proc dist procs...
Usage: @samp{min_sep<dist>(nodes...)}

Specify that @var{nodes} should have a miminum physical separation
of distance @var{dist}.  
@var{nodes} can be organized into aggregate groups:
for @samp{min_sep(@{a,b@},@{c,d@})}, @t{a} and @t{b} must be separated
from @t{c} and @t{d}.  
Affects @command{cflat} for layout and @command{prsim}.  

The @t{min_sep_proc} variation specifies a minimum distance between
two groups of processes.  
The referenced process in each group can be of different types.  

@example
bool x[2], y[2];
inv1 P, A;
inv2 Q, B;
spec @{
  min_sep<100>(x[0], y[0])
  min_sep<100>(x, y)
  min_sep<100>(@{x[0], y[0]@}, @{x[1], y[1]@})
  min_sep_proc<50>(P, Q)
  min_sep_proc<50>(@{P,Q@}, @{A,B@})
@}
@end example
@end deffn
@end texinfo
***/
DECLARE_AND_DEFINE_CFLAT_SPEC_DIRECTIVE_CLASS(layout_min_sep, "min_sep")

/**
	\param a node arguments are processed in groups, so e.g.
		min_sep({a,b},{c,d})
		groups are {a,b} and {c,d}.  
 */
void
layout_min_sep::main(cflat_prs_printer& p, const param_args_type& v, 
		const node_args_type& a) {
	STACKTRACE_VERBOSE;
	switch (p.cfopts.primary_tool) {
	case cflat_options::TOOL_LAYOUT:
		default_layout_spec_output<this_type, bool_tag>(p, v, a)
			<< endl;
		break;
	case cflat_options::TOOL_PRSIM:
		// but not for old plain prsim
	if (p.cfopts.with_SEU()) {
		default_layout_spec_output<this_type, bool_tag>(p, v, a)
			<< endl;
	}
		break;
	default:
		break;
	}
}

DECLARE_AND_DEFINE_CFLAT_SPEC_DIRECTIVE_CLASS(layout_min_sep_proc, "min_sep_proc")

/**
	\param a process arguments are processed in groups, so e.g.
		min_sep_proc({a,b},{c,d})
		groups are {a,b} and {c,d}.  
 */
void
layout_min_sep_proc::main(cflat_prs_printer& p, const param_args_type& v, 
		const node_args_type& a) {
	STACKTRACE_VERBOSE;
	switch (p.cfopts.primary_tool) {
	case cflat_options::TOOL_LAYOUT:
		default_layout_spec_output<this_type, process_tag>(p, v, a)
			<< endl;
		break;
	case cflat_options::TOOL_PRSIM:
		// but not for old plain prsim
	if (p.cfopts.with_SEU()) {
		default_layout_spec_output<this_type, process_tag>(p, v, a)
			<< endl;
	}
		break;
	default:
		break;
	}
}

}	// end namespace layout

//-----------------------------------------------------------------------------
// namespace for timing directives

namespace timing {
DECLARE_AND_DEFINE_CFLAT_SPEC_DIRECTIVE_CLASS(setup_pos, "setup_pos")
DECLARE_AND_DEFINE_CFLAT_SPEC_DIRECTIVE_CLASS(setup_neg, "setup_neg")
DECLARE_AND_DEFINE_CFLAT_SPEC_DIRECTIVE_CLASS(hold_pos, "hold_pos")
DECLARE_AND_DEFINE_CFLAT_SPEC_DIRECTIVE_CLASS(hold_neg, "hold_neg")

/**
	For specifying setup time relative to a positive edge.
 */
#define	DEFINE_TIMING_MAIN(d)						\
void									\
d::main(cflat_prs_printer& p, const param_args_type& v, 		\
		const node_args_type& a) {				\
	STACKTRACE_VERBOSE;						\
	switch (p.cfopts.primary_tool) {				\
	case cflat_options::TOOL_PRSIM:					\
		default_spec_output<this_type>(p, v, a) << endl;	\
		break;							\
	default:							\
		break;							\
	}								\
}

DEFINE_TIMING_MAIN(setup_pos)
DEFINE_TIMING_MAIN(setup_neg)
DEFINE_TIMING_MAIN(hold_pos)
DEFINE_TIMING_MAIN(hold_neg)

#undef	DEFINE_TIMING_MAIN

}	// end namespace timing

//-----------------------------------------------------------------------------
#undef	DECLARE_AND_DEFINE_CFLAT_SPEC_DIRECTIVE_CLASS
}	// end namespace cflat_specs

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

