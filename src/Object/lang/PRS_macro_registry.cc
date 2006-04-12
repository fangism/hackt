/**
	\file "Object/lang/PRS_macro_registry.cc"
	Macro definitions belong here.  
	$Id: PRS_macro_registry.cc,v 1.5.12.3 2006/04/12 06:35:04 fang Exp $
 */

#include <iostream>
#include <vector>
#include <set>
#include "Object/lang/PRS_macro_registry.h"
#include "Object/lang/cflat_printer.h"
#include "Object/lang/directive_base.h"
#include "main/cflat_options.h"
#include "util/qmap.tcc"
#include "common/TODO.h"

namespace util {
//=============================================================================
// explicit template instantiations
#if 1
template class qmap<std::string, HAC::entity::PRS::macro_definition_entry>;
// template macro_definition_entry
//	qmap<string, macro_definition_entry>::operator[](const string&) const;
#else
template macro_registry_type;
#endif
//=============================================================================
}	// end namespace util

namespace HAC {
namespace entity {
namespace PRS {
using util::qmap;
#include "util/using_ostream.h"

//=============================================================================
/**
	Global static initialization.
	This must appear first before any registrations take place in this 
	translation unit.
 */
const macro_registry_type
macro_registry;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Local static modifiable reference to use with registration.  
 */
static
macro_registry_type&
__macro_registry(const_cast<macro_registry_type&>(macro_registry));

//=============================================================================
/**
	Macro class registration function.  
 */
template <class T>
static
size_t
register_macro_class(void) {
	typedef	macro_registry_type::iterator		iterator;
	typedef	macro_registry_type::mapped_type	mapped_type;
	const string k(T::name);
	mapped_type& m = __macro_registry[k];
	if (m) {
		cerr << "Error: PRS macro by the name \'" << k <<
			"\' has already been registered!" << endl;
		THROW_EXIT;
	}
	m = macro_definition_entry(k, &T::main,
		&T::check_num_params, &T::check_num_nodes, 
		&T::check_param_args, &T::check_node_args);
	// oddly, this is needed to force instantiation of the [] const operator
	const mapped_type& n __ATTRIBUTE_UNUSED__ = macro_registry[k];
	INVARIANT(n);
	return macro_registry.size();
}

//=============================================================================
/**
	Convenient home for user-defined macro classes.  
	Could make this namespace anonymous...
 */
namespace __macros__ {

/**
	Convenient macro for declaring macro classes.
	TODO: consider adding a description string.  
 */
#define	DECLARE_PRS_MACRO_CLASS(class_name, macro_name)			\
struct class_name {							\
	typedef	class_name				this_type;	\
	typedef	macro_definition_entry::node_args_type	node_args_type;	\
	typedef	macro_definition_entry::param_args_type	param_args_type;\
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
const char class_name::name[] = macro_name;				\
const size_t class_name::id = register_macro_class<class_name>();

/**
	Default check for arguments.  
 */
#define	DEFINE_DEFAULT_PRS_MACRO_CLASS_CHECK_NUM_PARAMS(class_name)	\
good_bool								\
class_name::check_num_params(const size_t) {				\
	return good_bool(true);						\
}

#define	DEFINE_DEFAULT_PRS_MACRO_CLASS_CHECK_NUM_NODES(class_name)	\
good_bool								\
class_name::check_num_nodes(const size_t) {				\
	return good_bool(true);						\
}

#define	DEFINE_DEFAULT_PRS_MACRO_CLASS_CHECK_PARAMS(class_name)		\
good_bool								\
class_name::check_param_args(const param_args_type&) {			\
	return good_bool(true);						\
}

#define	DEFINE_DEFAULT_PRS_MACRO_CLASS_CHECK_NODES(class_name)		\
good_bool								\
class_name::check_node_args(const node_args_type& a) {			\
	return __no_grouped_node_args(name, a);				\
}

/**
	Currently no macros are allowed to take grouped arguments
	that result in more than one node.  
 */
static
good_bool
__no_grouped_node_args(const char* name,
		const macro_definition_entry::node_args_type& a) {
	typedef	macro_definition_entry::node_args_type	node_args_type;
	typedef	node_args_type::const_iterator	const_iterator;
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

//-----------------------------------------------------------------------------
/***
	IDEAS:
	assert_disjoint() -- node ID must NOT match
	assert_alias() -- node IDs must match
	order()
	stat()
	unstat()
	exclhi()
	excllo()
	passn()
	passp()
***/

//-----------------------------------------------------------------------------
typedef	macro_definition_entry::node_args_type	node_args_type;
typedef	macro_definition_entry::param_args_type	param_args_type;

#if 0
static
ostream&
print_param_args_list(cflat_prs_printer& p, const param_args_type& params) {
	typedef	node_args_type::const_iterator		const_iterator;
	NEVER_NULL(delim);
	ostream& o(p.os);
	return directive_base::dump_params(params, o);
}
#endif

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
#if 0
// uncomment when we want to use it
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
good_bool
__takes_no_parameters(const string& m, const size_t s) {
	if (s) {
		cerr << "Error: the \'" << m <<
			"\' macro takes no parameters." << endl;
		return good_bool(false);
	} else	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
good_bool
__takes_no_parameters(const string& m, const param_args_type& p) {
	return __takes_no_parameters(m, p.size());
}

//-----------------------------------------------------------------------------
DECLARE_PRS_MACRO_CLASS(Echo, "echo")

/**
	Pretty much a diagnostic tool only.
	For all tool modes, prints out "echo(...)" with canonical
	hierarchical names substituted for the local literals.  
	TODO: print out each canonical node name.  
 */
void
Echo::main(cflat_prs_printer& p, const param_args_type& params, 
		const node_args_type& nodes) {
	typedef	node_args_type::const_iterator	const_iterator;
	ostream& o(p.os);
	o << name;
	directive_base::dump_params(params, o);
	o << '(';
	print_node_args_list(p, nodes, ", ");
	o << ')' << endl;
}

/// no constraints on arguments, or the number of arguments
DEFINE_DEFAULT_PRS_MACRO_CLASS_CHECK_NUM_PARAMS(Echo)
DEFINE_DEFAULT_PRS_MACRO_CLASS_CHECK_NUM_NODES(Echo)
DEFINE_DEFAULT_PRS_MACRO_CLASS_CHECK_PARAMS(Echo)
DEFINE_DEFAULT_PRS_MACRO_CLASS_CHECK_NODES(Echo)

//-----------------------------------------------------------------------------
DECLARE_PRS_MACRO_CLASS(PassN, "passn")
DECLARE_PRS_MACRO_CLASS(PassP, "passp")

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
PassN::main(cflat_prs_printer& p, const param_args_type&, 
		const node_args_type& nodes) {
	ostream& o(p.os);
	switch (p.cfopts.primary_tool) {
	case cflat_options::TOOL_PRSIM:
		o << "after 0\t";
		p.__dump_canonical_literal(*nodes[0].begin());
		o << " & ~";
		p.__dump_canonical_literal(*nodes[1].begin());
		o << " -> ";
		p.__dump_canonical_literal(*nodes[2].begin());
		o << '-' << endl;
		break;
	case cflat_options::TOOL_LVS:
		o << "passn(";
		print_node_args_list(p, nodes, ", ");
		o << ')' << endl;
		break;
	default:
		// nothing
		break;
	}
}

void
PassP::main(cflat_prs_printer& p, const param_args_type&, 
		const node_args_type& nodes) {
	ostream& o(p.os);
	switch (p.cfopts.primary_tool) {
	case cflat_options::TOOL_PRSIM:
		o << "after 0\t~";
		p.__dump_canonical_literal(*nodes[0].begin());
		o << " & ";
		p.__dump_canonical_literal(*nodes[1].begin());
		o << " -> ";
		p.__dump_canonical_literal(*nodes[2].begin());
		o << '+' << endl;
		break;
	case cflat_options::TOOL_LVS:
		o << "passp(";
		print_node_args_list(p, nodes, ", ");
		o << ')' << endl;
		break;
	default:
		// nothing
		break;
	}
}

good_bool
PassN::check_num_params(const size_t n) {
	return __takes_no_parameters(name, n);
}

good_bool
PassP::check_num_params(const size_t n) {
	return __takes_no_parameters(name, n);
}

good_bool
PassN::check_param_args(const param_args_type& p) {
	return __takes_no_parameters(name, p);
}

good_bool
PassP::check_param_args(const param_args_type& p) {
	return __takes_no_parameters(name, p);
}

/**
	Exactly 3 arguments: gate, source, drain.
 */
good_bool
PassN::check_num_nodes(const size_t n) {
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
PassP::check_num_nodes(const size_t n) {
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
DEFINE_DEFAULT_PRS_MACRO_CLASS_CHECK_NODES(PassN)
DEFINE_DEFAULT_PRS_MACRO_CLASS_CHECK_NODES(PassP)

//-----------------------------------------------------------------------------
#undef	DECLARE_PRS_MACRO_CLASS
}	// end namespace __macros__

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

