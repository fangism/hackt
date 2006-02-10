/**
	\file "Object/lang/SPEC_registry.cc"
	Definitions of spec directives belong here.  
	$Id: SPEC_registry.cc,v 1.5 2006/02/10 21:50:40 fang Exp $
 */

#include <iostream>
#include <vector>
#include "Object/lang/SPEC_registry.h"
#include "Object/lang/directive_base.h"
#include "Object/lang/cflat_printer.h"
#include "main/cflat_options.h"
#include "common/TODO.h"
#include "util/qmap.tcc"

namespace HAC {
namespace entity {

namespace SPEC {
using util::qmap;
#include "util/using_ostream.h"

//=============================================================================
// explicit template instantiations
#if 1
template class qmap<string, spec_definition_entry>;
#else
template spec_registry_type;
#endif

//=============================================================================
/**
	Global static initialization.
	This must appear first before any registrations take place in this 
	translation unit.
 */
const spec_registry_type
spec_registry;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Local static modifiable reference to use with registration.  
 */
static
spec_registry_type&
__spec_registry(const_cast<spec_registry_type&>(spec_registry));

//=============================================================================
// class spec_definition_entry method definitions

//=============================================================================
/**
	Macro class registration function.  
 */
template <class T>
static
size_t
register_spec_class(void) {
	typedef	spec_registry_type::iterator		iterator;
	typedef	spec_registry_type::mapped_type	mapped_type;
	const string k(T::name);
	mapped_type& m = __spec_registry[k];
	if (m) {
		cerr << "Error: spec directive by the name \'" << k <<
			"\' has already been registered!" << endl;
		THROW_EXIT;
	}
	m = spec_definition_entry(k, &T::main,
		&T::check_num_params, &T::check_num_nodes,
		&T::check_param_args, &T::check_node_args);
	// oddly, this is needed to force instantiation of the [] const operator
	const mapped_type& n = spec_registry[k];
	INVARIANT(n);
	return spec_registry.size();
}

//=============================================================================
/**
	Convenient home for user-defined spec classes.  
	Could make this namespace anonymous...
 */
namespace __specs__ {

/**
	Convenient spec for declaring spec classes.
	TODO: consider adding a description string.  
 */
#define	DECLARE_SPEC_DIRECTIVE_CLASS(class_name, spec_name)		\
struct class_name {							\
	typedef	class_name				this_type;	\
	typedef	spec_definition_entry::node_args_type	node_args_type;	\
	typedef	spec_definition_entry::param_args_type	param_args_type;\
public:									\
	static const char			name[];			\
	static void main(cflat_prs_printer&, const param_args_type&,	\
		const node_args_type&);					\
	static good_bool check_num_params(const size_t);		\
	static good_bool check_num_nodes(const size_t);			\
	static good_bool check_param_args(const param_args_type&);	\
	static good_bool check_node_args(const node_args_type&);	\
private:								\
	static const size_t			id;			\
};									\
const char class_name::name[] = spec_name;				\
const size_t class_name::id = register_spec_class<class_name>();

/**
	Default check for number of arguments.  
 */
#define	DEFINE_DEFAULT_SPEC_DIRECTIVE_CLASS_CHECK_NUM_PARAMS(class_name)\
good_bool								\
class_name::check_num_params(const size_t) {				\
	return good_bool(true);						\
}

#define	DEFINE_DEFAULT_SPEC_DIRECTIVE_CLASS_CHECK_NUM_NODES(class_name)	\
good_bool								\
class_name::check_num_nodes(const size_t) {				\
	return good_bool(true);						\
}

#define	DEFINE_DEFAULT_SPEC_DIRECTIVE_CLASS_CHECK_PARAMS(class_name)	\
good_bool								\
class_name::check_param_args(const param_args_type&) {			\
	return good_bool(true);						\
}

#define	DEFINE_DEFAULT_SPEC_DIRECTIVE_CLASS_CHECK_NODES(class_name)	\
good_bool								\
class_name::check_node_args(const node_args_type&) {			\
	return good_bool(true);						\
}

//-----------------------------------------------------------------------------
typedef	spec_definition_entry::node_args_type		node_args_type;
typedef	spec_definition_entry::param_args_type		param_args_type;

/**
	Blatantly copied from PRS_macro_registry.cc.
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
	p.__dump_canonical_literal(*i);
	for (++i; i!=e; ++i) {
		o << delim;
		p.__dump_canonical_literal(*i);
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reusable function for specifying the minimum number of arguments.  
 */
good_bool
__takes_no_params(const string& name, const size_t args) {
	if (args) {
		cerr << "The \'" << name <<
			"\' directive takes no parameters arguments." << endl;
		return good_bool(false);
	} else	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reusable function for specifying the minimum number of arguments.  
 */
good_bool
min_num_nodes(const string& name, const size_t min, const size_t args) {
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
good_bool
exact_num_nodes(const string& name, const size_t req, const size_t args) {
	if (args != req) {
		cerr << "The \'" << name << "\' directive requires exactly " <<
			req << " arguments." << endl;
		return good_bool(false);
	} else	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is the default thing to do, much too common and reusable.  
 */
template <class T>
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
ostream&
default_expand_into_singles_output(cflat_prs_printer& p, 
		const node_args_type& a) {
	typedef	node_args_type::const_iterator		const_iterator;
	ostream& o(p.os);
	const_iterator i(a.begin());
	const const_iterator e(a.end());
	for ( ; i!=e; ++i) {
		o << T::name << '(';
		p.__dump_canonical_literal(*i);
		o << ')' << endl;
	}
	return o;
}

//-----------------------------------------------------------------------------
//
//	Define spec directives HERE.  
//
//-----------------------------------------------------------------------------

DECLARE_SPEC_DIRECTIVE_CLASS(LVS_exclhi, "exclhi")
DECLARE_SPEC_DIRECTIVE_CLASS(LVS_excllo, "excllo")

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

good_bool
LVS_exclhi::check_num_params(const size_t s) {
	return __takes_no_params(name, s);
}

good_bool
LVS_exclhi::check_num_nodes(const size_t s) {
	return min_num_nodes(name, 2, s);
}

DEFINE_DEFAULT_SPEC_DIRECTIVE_CLASS_CHECK_PARAMS(LVS_exclhi)
// make sure nodes aren't aliased accidentally?
DEFINE_DEFAULT_SPEC_DIRECTIVE_CLASS_CHECK_NODES(LVS_exclhi)

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

good_bool
LVS_excllo::check_num_params(const size_t s) {
	return __takes_no_params(name, s);
}

good_bool
LVS_excllo::check_num_nodes(const size_t s) {
	return min_num_nodes(name, 2, s);
}

DEFINE_DEFAULT_SPEC_DIRECTIVE_CLASS_CHECK_PARAMS(LVS_excllo)
// make sure nodes aren't aliased accidentally?
DEFINE_DEFAULT_SPEC_DIRECTIVE_CLASS_CHECK_NODES(LVS_excllo)

//-----------------------------------------------------------------------------
DECLARE_SPEC_DIRECTIVE_CLASS(LVS_BDD_order, "order")

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

good_bool
LVS_BDD_order::check_num_params(const size_t s) {
	return __takes_no_params(name, s);
}

good_bool
LVS_BDD_order::check_num_nodes(const size_t s) {
	return min_num_nodes(name, 2, s);
}

DEFINE_DEFAULT_SPEC_DIRECTIVE_CLASS_CHECK_PARAMS(LVS_BDD_order)
// make sure nodes aren't aliased accidentally?
DEFINE_DEFAULT_SPEC_DIRECTIVE_CLASS_CHECK_NODES(LVS_BDD_order)

//-----------------------------------------------------------------------------
DECLARE_SPEC_DIRECTIVE_CLASS(LVS_unstaticized, "unstaticized")

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

good_bool
LVS_unstaticized::check_num_params(const size_t s) {
	return __takes_no_params(name, s);
}

good_bool
LVS_unstaticized::check_num_nodes(const size_t s) {
	return min_num_nodes(name, 1, s);
}

DEFINE_DEFAULT_SPEC_DIRECTIVE_CLASS_CHECK_PARAMS(LVS_unstaticized)
DEFINE_DEFAULT_SPEC_DIRECTIVE_CLASS_CHECK_NODES(LVS_unstaticized)

//-----------------------------------------------------------------------------
DECLARE_SPEC_DIRECTIVE_CLASS(SIM_force_exclhi, "mk_exclhi")
DECLARE_SPEC_DIRECTIVE_CLASS(SIM_force_excllo, "mk_excllo")

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

good_bool
SIM_force_exclhi::check_num_params(const size_t s) {
	return __takes_no_params(name, s);
}

good_bool
SIM_force_exclhi::check_num_nodes(const size_t s) {
	return min_num_nodes(name, 2, s);
}

DEFINE_DEFAULT_SPEC_DIRECTIVE_CLASS_CHECK_PARAMS(SIM_force_exclhi)
// make sure node arguments aren't actually aliased?
DEFINE_DEFAULT_SPEC_DIRECTIVE_CLASS_CHECK_NODES(SIM_force_exclhi)

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

good_bool
SIM_force_excllo::check_num_params(const size_t s) {
	return __takes_no_params(name, s);
}

good_bool
SIM_force_excllo::check_num_nodes(const size_t s) {
	return min_num_nodes(name, 2, s);
}

DEFINE_DEFAULT_SPEC_DIRECTIVE_CLASS_CHECK_PARAMS(SIM_force_excllo)
// make sure node arguments aren't actually aliased?
DEFINE_DEFAULT_SPEC_DIRECTIVE_CLASS_CHECK_NODES(SIM_force_excllo)

//-----------------------------------------------------------------------------
#undef	DECLARE_SPEC_DIRECTIVE_CLASS
}	// end namespace __specs__

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

