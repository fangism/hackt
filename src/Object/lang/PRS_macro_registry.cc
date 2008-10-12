/**
	\file "Object/lang/PRS_macro_registry.cc"
	Macro definitions belong here.  
	$Id: PRS_macro_registry.cc,v 1.12 2008/10/12 00:21:38 fang Exp $
 */

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <iostream>
#include <vector>
#include <set>
#include "Object/lang/PRS_macro_registry.tcc"
#include "Object/lang/cflat_printer.h"
#include "Object/lang/directive_base.h"
#include "Object/lang/PRS_macro_common.h"
#include "main/cflat_options.h"
#include "util/qmap.tcc"
#include "common/TODO.h"

namespace HAC {
namespace entity {
namespace PRS {
using util::qmap;
#include "util/using_ostream.h"

// explicit template instantiations
template class macro_visitor_entry<cflat_prs_printer>;

//=============================================================================
/**
	Global static initialization.
	This must appear first before any registrations take place in this 
	translation unit.
 */
const cflat_macro_registry_type
cflat_macro_registry;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Local static modifiable reference to use with registration.  
 */
static
cflat_macro_registry_type& __cflat_macro_registry(
	const_cast<cflat_macro_registry_type&>(cflat_macro_registry));

//=============================================================================
/**
	Macro class registration function.  
 */
template <class T>
static
size_t
register_cflat_macro_class(void) {
	typedef	cflat_macro_registry_type::iterator		iterator;
	typedef	cflat_macro_registry_type::mapped_type		mapped_type;
	const string k(T::name);
	mapped_type& m(__cflat_macro_registry[k]);
	if (m) {
		cerr << "Error: PRS macro by the name \'" << k <<
			"\' has already been registered!" << endl;
		THROW_EXIT;
	}
	m = cflat_macro_definition_entry(k, &T::main,
		&T::check_num_params, &T::check_num_nodes, 
		&T::check_param_args, &T::check_node_args);
	// oddly, this is needed to force instantiation of the [] const operator
	const mapped_type& n
		__ATTRIBUTE_UNUSED_CTOR__((cflat_macro_registry[k]));
	INVARIANT(n);
	return cflat_macro_registry.size();
}

//=============================================================================
/**
	Convenient home for user-defined macro classes.  
	Could make this namespace anonymous...
 */
namespace cflat_macros {
typedef	cflat_macro_definition_entry::node_args_type	node_args_type;
typedef	cflat_macro_definition_entry::param_args_type	param_args_type;

/**
	Convenient macro for declaring macro classes.
	TODO: consider adding a description string.  
 */
#define	DECLARE_AND_DEFINE_CFLAT_PRS_MACRO_CLASS(class_name, macro_name)\
	DECLARE_PRS_MACRO_CLASS(class_name, cflat_prs_printer)		\
	DEFINE_PRS_MACRO_CLASS(class_name, macro_name, 			\
		register_cflat_macro_class)

//-----------------------------------------------------------------------------
/***
	IDEAS:
	assert_disjoint() -- node ID must NOT match (implemented in spec)
	assert_alias() -- node IDs must (just connect them!)
	order()
	stat()
	unstat()
	exclhi()
	excllo()
***/

//-----------------------------------------------------------------------------
static
ostream&
print_param_args_list(cflat_prs_printer& p, const param_args_type& params) {
	return directive_base::dump_params(params, p.os);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints canonical names list of nodes, delimited by whatever.  
	Useful utility for many PRS macros.  
	TODO: write a variation that takes an iterator pair.  
	\pre each node group/set must have exactly one member.  
	\param delim the delimiter string between groups.  
 */
static
ostream&
print_node_args_list(cflat_prs_printer& p, const node_args_type& nodes, 
		const char* delim) {
	typedef	node_args_type::const_iterator		const_iterator;
	NEVER_NULL(delim);
	ostream& o(p.os);
	const_iterator i(nodes.begin());
	const const_iterator e(nodes.end());
	INVARIANT(i!=e);
	p.__dump_canonical_literal_group(*i);
	for (++i; i!=e; ++i) {
		o << delim;
		p.__dump_canonical_literal_group(*i);
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints (in grouped form) canonical names list of nodes, 
	delimited by whatever.  
	Potentially useful utility function for some (?) PRS macros.  
	Each node group/set may have multiple members.  
	\param delim the delimiter string between groups.  
	\param gl the open-group string, such as "{"
	\param gr the close-group string, such as "}"
	\param gd the delimiter within each group.  
 */
static
ostream&
print_grouped_node_args_list(cflat_prs_printer& p, const node_args_type& nodes, 
	const char* delim, const char* gl, const char* gd, const char* gr) {
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

//-----------------------------------------------------------------------------
/***
@texinfo prs/macro-echo.texi
@defmac echo nodes...
Diagnostic.  This macro just prints @samp{echo(...)} back out
where the original arguments are substituted with canonical hierarchical 
instance (node) names.
This demonstrates how one can add custom PRS macros.  
@end defmac
@end texinfo
***/
DECLARE_AND_DEFINE_CFLAT_PRS_MACRO_CLASS(Echo, "echo")

/**
	Pretty much a diagnostic tool only.
	For all tool modes, prints out "echo(...)" with canonical
	hierarchical names substituted for the local literals.  
 */
void
Echo::main(cflat_prs_printer& p, const param_args_type& params, 
		const node_args_type& nodes) {
	typedef	node_args_type::const_iterator	const_iterator;
	ostream& o(p.os);
	o << name;
	print_param_args_list(p, params);
	o << '(';
	print_grouped_node_args_list(p, nodes, ", ", "{", ",", "}");
	// print_node_args_list(p, nodes, ", ");
	o << ')' << endl;
}

//-----------------------------------------------------------------------------
/***
@texinfo prs/macro-passn.texi
@defmac passn W L g s d
Usage: @samp{passn<W,L>(g, s, d)} or @samp{passn(g, s, d)}

Declares an NFET pass-transistor with gate @var{g}, source @var{s}, 
and drain @var{d}.
Sizing parameters @var{W} and @var{L} are optional.
In @command{hflat prsim} mode, 
this prints a uni-directional (sized) production rule 
@example
after 0  g & ~s -> d-
@end example
In @command{hflat lvs} mode, this just prints @samp{passn(g, s, d)} back out.  
Sizes are printed only if @option{-fsizes} is passed to hflat.
@end defmac
@end texinfo

@texinfo prs/macro-passp.texi
@defmac passp W L g s d
Usage: @samp{passp<W,L>(g, s, d)} or @samp{passp(g, s, d)}

Declares a PFET pass-transistor with gate @var{g}, source @var{s}, 
and drain @var{d}.
Sizing parameters @var{W} and @var{L} are optional.
In @command{hflat prsim} mode, 
this prints a uni-directional (sized) production rule 
@example
after 0 ~g & s -> d+
@end example
In @command{hflat lvs} mode, this just prints @samp{passp(g, s, d)} back out.  
Sizes are printed only if @option{-fsizes} is passed to hflat.
@end defmac
@end texinfo
***/
DECLARE_AND_DEFINE_CFLAT_PRS_MACRO_CLASS(PassN, "passn")
DECLARE_AND_DEFINE_CFLAT_PRS_MACRO_CLASS(PassP, "passp")

/**
	cflat modes: lvs and prsim

	Clint:
	Basically within prs { } blocks we just need the following:
		passn(g,s,d)
		passp(g,s,d)
	If you do -lvs, these just get replicated in the prs file as
		passn(g,s,d)
		passp(g,s,d)
	and if you do -prsim, they get turned into:
		after 0 g & ~s -> d-
		after 0 ~g & s -> d+
 */
void
PassN::main(cflat_prs_printer& p, const param_args_type& params, 
		const node_args_type& nodes) {
	ostream& o(p.os);
	switch (p.cfopts.primary_tool) {
	case cflat_options::TOOL_PRSIM:
	if (p.cfopts.expand_pass_gates) {
		o << "after 0\t";
		p.__dump_canonical_literal(*nodes[0].begin());
		if (p.cfopts.size_prs && params.front()) {
			print_param_args_list(p, params);
		}
		o << " & ~";
		p.__dump_canonical_literal(*nodes[1].begin());
		o << " -> ";
		p.__dump_canonical_literal(*nodes[2].begin());
		o << '-' << endl;
		break;
	}
	// else fall-through, yes
	case cflat_options::TOOL_LVS:
		o << "passn";
		if (p.cfopts.size_prs && params.front()) {
			print_param_args_list(p, params);
		}
		o << "(";
		print_node_args_list(p, nodes, ", ");
		o << ')' << endl;
		break;
	default:
		// nothing
		break;
	}
}

void
PassP::main(cflat_prs_printer& p, const param_args_type& params, 
		const node_args_type& nodes) {
	ostream& o(p.os);
	switch (p.cfopts.primary_tool) {
	case cflat_options::TOOL_PRSIM:
	if (p.cfopts.expand_pass_gates) {
		o << "after 0\t~";
		p.__dump_canonical_literal(*nodes[0].begin());
		if (p.cfopts.size_prs && params.front()) {
			print_param_args_list(p, params);
		}
		o << " & ";
		p.__dump_canonical_literal(*nodes[1].begin());
		o << " -> ";
		p.__dump_canonical_literal(*nodes[2].begin());
		o << '+' << endl;
		break;
	}
	// else fall-through, yes
	case cflat_options::TOOL_LVS:
		o << "passp";
		if (p.cfopts.size_prs && params.front()) {
			print_param_args_list(p, params);
		}
		o << "(";
		print_node_args_list(p, nodes, ", ");
		o << ')' << endl;
		break;
	default:
		// nothing
		break;
	}
}

//-----------------------------------------------------------------------------
#undef	DECLARE_AND_DEFINE_CFLAT_PRS_MACRO_CLASS
}	// end namespace cflat_macros

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

