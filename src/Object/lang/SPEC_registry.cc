/**
	\file "Object/lang/SPEC_registry.cc"
	Definitions of spec directives belong here.  
	$Id: SPEC_registry.cc,v 1.1.2.2 2006/02/04 05:45:49 fang Exp $
 */

#include <iostream>
#include <vector>
#include "Object/lang/SPEC_registry.h"
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

void
spec_definition_entry::main(cflat_prs_printer& c,
		const node_args_type& n) const {
	NEVER_NULL(_main);
	(*_main)(c, n);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
spec_definition_entry::check_num_args(const size_t i) const {
	if (_check_num_args) {
		return (*_check_num_args)(i);
	} else	return good_bool(true);
}

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
	m = spec_definition_entry(k, &T::main, &T::check_num_args);
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
	typedef	spec_definition_entry::node_args_type	node_args_type;	\
public:									\
	static const char			name[];			\
	static void main(cflat_prs_printer&, const node_args_type&);	\
	static good_bool check_num_args(const size_t);			\
private:								\
	static const size_t			id;			\
};									\
const char class_name::name[] = spec_name;				\
const size_t class_name::id = register_spec_class<class_name>();

/**
	Default check for number of arguments.  
 */
#define	DEFINE_DEFAULT_SPEC_DIRECTIVE_CLASS_CHECK_NUM_ARGS(class_name)	\
good_bool								\
class_name::check_num_args(const size_t) {				\
	return good_bool(true);						\
}

//-----------------------------------------------------------------------------
typedef	spec_definition_entry::node_args_type		node_args_type;

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

good_bool
min_args(const string& name, const size_t min, const size_t args) {
	if (args < min) {
		cerr << "The " << name << " directive requires at least "
			"two arguments." << endl;
		return good_bool(false);
	} else	return good_bool(true);
}

//-----------------------------------------------------------------------------
DECLARE_SPEC_DIRECTIVE_CLASS(LVS_exclhi, "lvs_exclhi")
DECLARE_SPEC_DIRECTIVE_CLASS(LVS_excllo, "lvs_excllo")

/**
	lvs_exclhi -- for LVS only, asserts that a set of nodes may only
		contain one logic high value.  
 */
void
LVS_exclhi::main(cflat_prs_printer& p, const node_args_type& a) {
	ostream& o(p.os);
	switch (p.cfopts.primary_tool) {
	case cflat_options::TOOL_LVS:
		o << "exclhi(";
		print_node_args_list(p, a, ", ");
		o << ')' << endl;
		break;
	default:
		break;
	}
}

good_bool
LVS_exclhi::check_num_args(const size_t s) {
	return min_args(name, 2, s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	lvs_excllo -- for LVS only, asserts that a set of nodes may only
		contain one logic low value.  
 */
void
LVS_excllo::main(cflat_prs_printer& p, const node_args_type& a) {
	ostream& o(p.os);
	switch (p.cfopts.primary_tool) {
	case cflat_options::TOOL_LVS:
		o << "excllo(";
		print_node_args_list(p, a, ", ");
		o << ')' << endl;
		break;
	default:
		break;
	}
}

good_bool
LVS_excllo::check_num_args(const size_t s) {
	return min_args(name, 2, s);
}

//-----------------------------------------------------------------------------
DECLARE_SPEC_DIRECTIVE_CLASS(SIM_force_exclhi, "sim_force_exclhi")
DECLARE_SPEC_DIRECTIVE_CLASS(SIM_force_excllo, "sim_force_excllo")

/**
	sim_force_exclhi -- for simulations only, 
		coerces exclusive high among nodes.  
 */
void
SIM_force_exclhi::main(cflat_prs_printer& p, const node_args_type& a) {
	ostream& o(p.os);
	switch (p.cfopts.primary_tool) {
	case cflat_options::TOOL_PRSIM:
		// or other simulator tool
		o << "exclhi(";
		print_node_args_list(p, a, ", ");
		o << ')' << endl;
		break;
	default:
		break;
	}
}

good_bool
SIM_force_exclhi::check_num_args(const size_t s) {
	return min_args(name, 2, s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	sim_force_excllo -- for simulations only, 
		coerces exclusive high among nodes.  
 */
void
SIM_force_excllo::main(cflat_prs_printer& p, const node_args_type& a) {
	ostream& o(p.os);
	switch (p.cfopts.primary_tool) {
	case cflat_options::TOOL_PRSIM:
		// or other simulator tool
		o << "excllo(";
		print_node_args_list(p, a, ", ");
		o << ')' << endl;
		break;
	default:
		break;
	}
}

good_bool
SIM_force_excllo::check_num_args(const size_t s) {
	return min_args(name, 2, s);
}

//-----------------------------------------------------------------------------
DECLARE_SPEC_DIRECTIVE_CLASS(SIM_assert_exclhi, "sim_assert_exclhi")
DECLARE_SPEC_DIRECTIVE_CLASS(SIM_assert_excllo, "sim_assert_excllo")

/**
	sim_assert_exclhi -- for simulations only, checks exclusivity.  
	TODO: fill in the blanks.
 */
void
SIM_assert_exclhi::main(cflat_prs_printer& p, const node_args_type& a) {
	// ostream& o(p.os);
	switch (p.cfopts.primary_tool) {
	case cflat_options::TOOL_PRSIM:
		// or other simulator tool
		// emit check prs?
		break;
	default:
		break;
	}
}

good_bool
SIM_assert_exclhi::check_num_args(const size_t s) {
	return min_args(name, 2, s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	sim_assert_excllo -- for simulations only, checks exclusivity.
	TODO: fill in the blanks.
 */
void
SIM_assert_excllo::main(cflat_prs_printer& p, const node_args_type& a) {
	// ostream& o(p.os);
	switch (p.cfopts.primary_tool) {
	case cflat_options::TOOL_PRSIM:
		// or other simulator tool
		// emit check prs?
		break;
	default:
		break;
	}
}

good_bool
SIM_assert_excllo::check_num_args(const size_t s) {
	return min_args(name, 2, s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//-----------------------------------------------------------------------------
#undef	DECLARE_SPEC_DIRECTIVE_CLASS
}	// end namespace __specs__

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

