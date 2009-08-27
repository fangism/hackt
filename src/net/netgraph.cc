/**
	\file "net/netgraph.cc"
	$Id: netgraph.cc,v 1.1.2.22 2009/08/27 20:38:45 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <sstream>		// for ostringstream
#include <iterator>		// for ostream_iterator
#include <map>
#include "net/netgraph.h"
#include "Object/state_manager.h"
#include "Object/global_entry.h"
#include "Object/global_channel_entry.h"
#include "Object/global_entry_context.h"
#include "Object/common/dump_flags.h"
#include "Object/def/footprint.h"
#include "Object/def/process_definition.h"
#include "Object/expr/preal_const.h"
#include "Object/traits/instance_traits.h"
#include "Object/lang/PRS_footprint.h"
#include "Object/inst/physical_instance_placeholder.h"
#include "Object/inst/physical_instance_collection.h"
#include "Object/inst/port_alias_tracker.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/alias_actuals.h"
#include "Object/inst/bool_port_collector.tcc"
#include "util/unique_list.tcc"
#include "util/string.tcc"		// for strgsub, string_to_num
#include "util/stacktrace.h"

namespace HAC {
namespace NET {
#include "util/using_ostream.h"
using entity::port_alias_tracker;
using entity::footprint_frame_map_type;
using entity::dump_flags;
using entity::bool_port_collector;
using entity::instance_alias_info;
using entity::bool_tag;
using entity::state_instance;
using entity::port_formals_manager;
using entity::process_definition;
using entity::physical_instance_collection;
using entity::port_alias_tracker_base;
using entity::alias_reference_set;
using std::map;
using std::ostringstream;
using std::ostream_iterator;
using util::value_saver;
using entity::PRS::PRS_LITERAL_TYPE_ENUM;
using entity::PRS::PRS_NOT_EXPR_TYPE_ENUM;
using entity::PRS::PRS_AND_EXPR_TYPE_ENUM;
using entity::PRS::PRS_OR_EXPR_TYPE_ENUM;
using entity::PRS::PRS_NODE_TYPE_ENUM;
using entity::directive_base_params_type;
using entity::preal_value_type;
using util::unique_list;
using util::strings::strgsub;
using util::option_value;
using util::option_value_list;
using util::strings::string_to_num;

//=============================================================================
// class netlist_options method definitions

netlist_options::netlist_options() :
		std_n_width(5.0),
		std_p_width(5.0),
		std_n_length(2.0),
		std_p_length(2.0),
		stat_n_width(3.0),
		stat_p_width(3.0),
		stat_n_length(20.0),
		stat_p_length(10.0),
		length_unit("u"),
		lambda(1.0), 
		nested_subcircuits(false),
		emit_top(true)
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sets a value of a structure member according to first value
	associated with option.  
	Such functions should be re-usable.  
	\param T is value type, can be deduced from arguments!  
	\param opt key=values option value association.  
	\param mem is a pointer-to-member of type T.
 */
template <typename T>
static
bool
__set_member_single_numeric_value(const option_value& opt, 
		netlist_options& n_opt, 
		T netlist_options::*mem) {
	const size_t s = opt.values.size();
	if (s >= 1) {
		if (s > 1) {
		cerr << "Warning: extra arguments passed to \'" << opt.key
			<< "\' option ignored." << endl;
		}
		const string& arg(opt.values.front());
		const bool ret = string_to_num(arg, n_opt.*mem);
		if (ret) {
			cerr << "Error: processing argument of \'" << opt.key
				<< "\' option, expecting numeric value, "
				"but got: " << arg << endl;
		}
		return ret;
	} else {
		cerr << "Warning: blank argument passed to \'" << opt.key
			<< "\' option where one is expected.  Ignoring."
			<< endl;
		return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
bool
__set_member_single_string(const option_value& opt, 
		netlist_options& n_opt, 
		string netlist_options::*mem) {
	const size_t s = opt.values.size();
	if (s >= 1) {
		if (s > 1) {
		cerr << "Warning: extra arguments passed to \'" << opt.key
			<< "\' option ignored." << endl;
		}
		n_opt.*mem = opt.values.front();
	} else {
		// if blank value, erase the string
		(n_opt.*mem).clear();
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Leave undefined, specialize the rest.
	Use single overloaded function to automatically dispatch.
 */
template <typename T>
bool
__set_member_default(const option_value& opt, 
		netlist_options& n_opt, T netlist_options::*mem);

// specialize for bool
bool
__set_member_default(const option_value& opt, 
		netlist_options& n_opt, bool netlist_options::*mem) {
	return __set_member_single_numeric_value(opt, n_opt, mem);
}

// specialize for real_type
bool
__set_member_default(const option_value& opt, 
		netlist_options& n_opt, real_type netlist_options::*mem) {
	return __set_member_single_numeric_value(opt, n_opt, mem);
}

// specialize for string
bool
__set_member_default(const option_value& opt, 
		netlist_options& n_opt, string netlist_options::*mem) {
	return __set_member_single_string(opt, n_opt, mem);
}



//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	bool (*opt_func)(const option_value&, netlist_options&);
typedef	map<string, opt_func>		opt_map_type;
static	opt_map_type			netlist_option_map;

// define option functions
#define	DEFINE_OPTION_DEFAULT(member)					\
static									\
bool									\
__set_ ## member (const option_value& v, netlist_options& o) {		\
	return __set_member_default(v, o, &netlist_options::member);	\
}

#if 0
#define	DEFINE_NUMERIC_OPTION(member)					\
static									\
bool									\
__set_ ## member (const option_value& v, netlist_options& o) {		\
	return __set_member_single_numeric_value(v, o,			\
		&netlist_options::member);				\
}

#define	DEFINE_STRING_OPTION(member)					\
static									\
bool									\
__set_ ## member (const option_value& v, netlist_options& o) {		\
	return __set_member_single_string(v, o,				\
		&netlist_options::member);				\
}
#endif

// could just fold string into here instead of initialization function below...
// TODO: create help/documentation for every known option
// TODO: produce usage help for console and texinfo documentation aside
// TODO: make these declarations self-initializing/installing in the map
DEFINE_OPTION_DEFAULT(lambda)
DEFINE_OPTION_DEFAULT(length_unit)
DEFINE_OPTION_DEFAULT(std_n_width)
DEFINE_OPTION_DEFAULT(std_p_width)
DEFINE_OPTION_DEFAULT(std_n_length)
DEFINE_OPTION_DEFAULT(std_p_length)
DEFINE_OPTION_DEFAULT(stat_n_width)
DEFINE_OPTION_DEFAULT(stat_p_width)
DEFINE_OPTION_DEFAULT(stat_n_length)
DEFINE_OPTION_DEFAULT(stat_p_length)
DEFINE_OPTION_DEFAULT(nested_subcircuits)
DEFINE_OPTION_DEFAULT(emit_top)

/**
	static global initialization of option map.
 */
static
int
__init_netlist_option_map(void) {
	netlist_option_map["lambda"] = &__set_lambda;
	netlist_option_map["length_unit"] = &__set_length_unit;
	netlist_option_map["std_n_width"] = &__set_std_n_width;
	netlist_option_map["std_p_width"] = &__set_std_p_width;
	netlist_option_map["std_n_length"] = &__set_std_n_length;
	netlist_option_map["std_p_length"] = &__set_std_p_length;
	netlist_option_map["stat_n_width"] = &__set_stat_n_width;
	netlist_option_map["stat_p_width"] = &__set_stat_p_width;
	netlist_option_map["stat_n_length"] = &__set_stat_n_length;
	netlist_option_map["stat_p_length"] = &__set_stat_p_length;
	netlist_option_map["nested_subcircuits"] = &__set_nested_subcircuits;
	netlist_option_map["emit_top"] = &__set_emit_top;
	return 1;
}

#undef	DEFINE_OPTION_DEFAULT
#if 0
#undef	DEFINE_NUMERIC_OPTION
#undef	DEFINE_STRING_OPTION
#endif

/**
	Receipt of initialization.  
 */
static
const
int
__netlist_option_map_initialized__ = __init_netlist_option_map();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: this should be reusable boilerplate code...
	\return true on error.
 */
bool
netlist_options::set(const option_value_list& opts) {
	STACKTRACE_VERBOSE;
	size_t errs = 0;
	typedef	opt_map_type::const_iterator	map_iterator;
	const map_iterator me(netlist_option_map.end());
	option_value_list::const_iterator i(opts.begin()), e(opts.end());
	for ( ; i!=e; ++i) {
	if (i->key.length()) {
		map_iterator mf(netlist_option_map.find(i->key));
		if (mf != me) {
			if ((*mf->second)(*i, *this)) {
				++errs;
			}
		} else {
			cerr << "Warning: ignoring unknown netlist option \'"
				<< i->key << "\'." << endl;
		}
	}
	}
	if (errs) {
		cerr << "Error: " << errs <<
			" error(s) found in option processing." << endl;
	}
	return errs;
}

//=============================================================================
// class netlist_common method definitions

template <class NP>
void
netlist_common::mark_used_nodes(NP& node_pool) const {
	transistor_pool_type::const_iterator
		i(transistor_pool.begin()), e(transistor_pool.end());
	for ( ; i!=e; ++i) {
		i->mark_used_nodes(node_pool);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
netlist_common::is_empty(void) const {
	return transistor_pool.empty() && passive_device_pool.empty();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class NP>
ostream&
netlist_common::emit_devices(ostream& o, const NP& node_pool,
		const footprint& fp, const netlist_options& nopt) const {
	// emit devices
#if ENABLE_STACKTRACE
	o << "* devices:" << endl;
#endif
	typedef	transistor_pool_type::const_iterator	const_iterator;
	const_iterator i(transistor_pool.begin()), e(transistor_pool.end());
	// TODO: print originating rule in comments
	// TODO: use optional label designations
	size_t j = 0;
	for ( ; i!=e; ++i, ++j) {
		o << 'M' << j << '_';
		i->emit_attribute_suffixes(o) << ' ';
		i->emit(o, node_pool, fp, nopt) << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
netlist_common::dump_raw_devices(ostream& o) const {
	typedef	transistor_pool_type::const_iterator	const_iterator;
	const_iterator i(transistor_pool.begin()), e(transistor_pool.end());
	size_t j = 0;
	for ( ; i!=e; ++i, ++j) {
		i->dump_raw(o << "  " << j << ": ") << endl;
	}
	return o;
}

//=============================================================================
// class local_netlist method definitions

/**
	Summarizes subcircuit ports by looking at all used nodes.  
	TODO: passive devices
 */
template <class NP>
void
local_netlist::mark_used_nodes(NP& node_pool) {
	transistor_pool_type::const_iterator
		i(transistor_pool.begin()), e(transistor_pool.end());
	for ( ; i!=e; ++i) {
		node_index_map.insert(i->gate);
		node_pool[i->gate].used = true;
		node_index_map.insert(i->source);
		node_pool[i->source].used = true;
		node_index_map.insert(i->drain);
		node_pool[i->drain].used = true;
		node_index_map.insert(i->body);
		node_pool[i->body].used = true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
local_netlist::dump_raw(ostream& o, const netlist& n) const {
	o << name;
	typedef	node_index_map_type::const_iterator	const_iterator;
	const_iterator i(node_index_map.begin()), e(node_index_map.end());
	for ( ; i!=e; ++i) {
		n.node_pool[*i].dump_raw(o << ' ');
	}
	o << endl;
	o << "transistors:" << endl;
	dump_raw_devices(o);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
local_netlist::emit_definition(ostream& o, const netlist& n,
		const netlist_options& nopt) const {
	o << ".subckt ";
if (!nopt.nested_subcircuits) {
	// then use fully qualified type name
	o << n.name << "::";
}
	o << name;
	typedef	node_index_map_type::const_iterator	const_iterator;
	const_iterator i(node_index_map.begin()), e(node_index_map.end());
	for ( ; i!=e; ++i) {
		n.node_pool[*i].emit(o << ' ', *n.fp);	// options?
	}
	o << endl;
	// TODO: emit port-info comments
	emit_devices(o, n.node_pool, *n.fp, nopt);
	o << ".ends" << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instantiation uses the same names as the original ports.
 */
ostream&
local_netlist::emit_instance(ostream& o, const netlist& n,
		const netlist_options& nopt) const {
	o << 'x' << name << ":inst";
	typedef	node_index_map_type::const_iterator	const_iterator;
	const_iterator i(node_index_map.begin()), e(node_index_map.end());
	for ( ; i!=e; ++i) {
		n.node_pool[*i].emit(o << ' ', *n.fp);	// options?
	}
	o << ' ';
if (!nopt.nested_subcircuits) {
	// then use fully qualified type name
	o << n.name << "::";
}
	o << name << endl;
	return o;
}

//=============================================================================
// class node method definitions

/**
	How to format print each node's identity.  
	TODO: make special designators configurable.
	TODO: configurable optional name-mangling?
	\param fp the context of this node index, 
		used for named nodes and internal nodes.
 */
ostream&
node::emit(ostream& o, const footprint& fp) const {
switch (type) {
case NODE_TYPE_LOGICAL:
	// NEVER_NULL(fp.get_instance_pool<bool_tag>()[index].get_back_ref());
	// does this guarantee canonical name?  seems to
	fp.get_instance_pool<bool_tag>()[index].get_back_ref()
		->dump_hierarchical_name(o, dump_flags::no_definition_owner);
	// TODO: take options dump_flags for changing hierarchical separator
	// TODO: possibly perform mangling
	break;
case NODE_TYPE_INTERNAL:
	o << '@' << fp.get_prs_footprint().get_internal_node(index).name;
	break;
case NODE_TYPE_AUXILIARY:
	o << '#';
	if (name.length()) {
		o << name;
	} else {
		o << index;
	}
	break;
case NODE_TYPE_SUPPLY:
	o << name;	// prefix with any designator? '$' or '!' ?
	break;
default:
	DIE;
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For debugging interpretation.
 */
ostream&
node::dump_raw(ostream& o) const {
switch (type) {
case NODE_TYPE_LOGICAL:
	o << ':' << index;
	break;
case NODE_TYPE_INTERNAL:
	o << '@' << index;	// was name
	break;
case NODE_TYPE_AUXILIARY:
	o << '#';
	if (name.length()) {
		o << name;
	} else {
		o << index;
	}
	break;
case NODE_TYPE_SUPPLY:
	o << '!' << name;
	break;
default:
	o << "???";
}
	if (!used) o << " (unconnected)";
	return o;
}

//=============================================================================
// class instance method definitions

template <class NP>
void
instance::mark_used_nodes(NP& node_pool) const {
	actuals_list_type::const_iterator
		i(actuals.begin()), e(actuals.end());
	for ( ; i!=e; ++i) {
		node_pool[*i].used = true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
instance::is_empty(void) const {
	NEVER_NULL(type);
	return type->is_empty();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class NP>
ostream&
instance::emit(ostream& o, const NP& node_pool, const footprint& fp) const {
	o << 'x';
	fp.get_instance_pool<process_tag>()[pid].get_back_ref()
		->dump_hierarchical_name(o, dump_flags::no_definition_owner);
	actuals_list_type::const_iterator
		i(actuals.begin()), e(actuals.end());
	for ( ; i!=e; ++i) {
		node_pool[*i].emit(o << ' ', fp);
	}
	return o << ' ' << type->get_name();	// endl
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
instance::dump_raw(ostream& o) const {
	o << '[' << pid << "]: " << type->get_name() << ": (";
	copy(actuals.begin(), actuals.end(),
		ostream_iterator<index_type>(o, ","));
	return o << ')';
}


//=============================================================================
// class transistor method definitions

/**
	In a separate pass mark all nodes participating on transistor.
	Why?  Is possible that a supply node is not used.  
 */
template <class NP>
void
transistor::mark_used_nodes(NP& node_pool) const {
	node_pool[source].used = true;
	node_pool[gate].used = true;
	node_pool[drain].used = true;
	node_pool[body].used = true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class NP>
ostream&
transistor::emit(ostream& o, const NP& node_pool, const footprint& fp,
		const netlist_options& nopt) const {
	node_pool[source].emit(o, fp) << ' ';
	node_pool[gate].emit(o, fp) << ' ';
	node_pool[drain].emit(o, fp) << ' ';
	node_pool[body].emit(o, fp) << ' ';
	switch (type) {
	case NFET_TYPE: o << "nch"; break;
	case PFET_TYPE: o << "pch"; break;
	// TODO: honor different vt types and flavors
	default:
		o << "<type?>";
	}
	// TODO: restrict lengths and widths, from tech/conf file
	o << " W=" << width *nopt.lambda << nopt.length_unit <<
		" L=" << length *nopt.lambda << nopt.length_unit;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
transistor::emit_attribute_suffixes(ostream& o) const {
	if (attributes & IS_PRECHARGE)
		o << ":pchg";
	if (attributes & IS_STANDARD_KEEPER)
		o << ":keeper";
	if (attributes & IS_COMB_FEEDBACK)
		o << ":ckeeper";
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
transistor::dump_raw(ostream& o) const {
	switch (type) {
	case NFET_TYPE: o << "nch"; break;
	case PFET_TYPE: o << "pch"; break;
	// TODO: honor different vt types and flavors
	default:
		o << "<type?>";
	}
	o << ' ' << source << ' ' << gate << ' ' << drain << ' ' << body;
	o << " # W=" << width << " L=" << length;
	// unitless
	emit_attribute_suffixes(o << " ");
	return o;
}

//=============================================================================
// class netlist method definitions

// tag objects for convenience
const node::__logical_node_tag	node::logical_node_tag = __logical_node_tag();
const node::__supply_node_tag	node::supply_node_tag = __supply_node_tag();
const node::__internal_node_tag	node::internal_node_tag = __internal_node_tag();
const node::__auxiliary_node_tag	node::auxiliary_node_tag = __auxiliary_node_tag();

// case sensitive?
static const node void_node("__VOID__", node::auxiliary_node_tag);	// NULL
static const node GND_node("GND", node::supply_node_tag);
static const node Vdd_node("Vdd", node::supply_node_tag);

// universal node indices to every subcircuit
// these should correspond with the order of insertion in netlist's ctor
static const	index_type	void_index = 0;
static const	index_type	GND_index = 1;
static const	index_type	Vdd_index = 2;

// static const string	dummy_type_name("<name>");	// temporary

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
netlist::netlist() : netlist_common(), name(), 
		named_node_map(), node_pool(),
		instance_pool(), internal_node_map(), port_list(), 
		empty(false), 
		aux_count(0),
		subs_count(0) {
	// copy supply nodes
	node_pool.reserve(8);	// reasonable pre-allocation
	// following order should match above universal node indices
	node_pool.push_back(void_node);
	node_pool.push_back(GND_node);
	node_pool.push_back(Vdd_node);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
netlist::~netlist() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Common initialization routines based on footprint.  
 */
void
netlist::__bind_footprint(const footprint& f) {
	fp = &f;
	// pre-allocate, using same indexing and mapping as original pool
	// null index initially, and default owner is not subcircuit
	const prs_footprint& pfp(f.get_prs_footprint());
	internal_node_map.resize(pfp.get_internal_node_pool().size(),
		internal_node_entry_type(0, 0));
	const prs_footprint::subcircuit_map_type&
		subs(pfp.get_subcircuit_map());
	local_subcircuits.resize(subs.size());
	// import/generate subcircuit names
	prs_footprint::subcircuit_map_type::const_iterator
		si(subs.begin()), se(subs.end());
	local_subcircuit_list_type::iterator
		li(local_subcircuits.begin());
	for (; si!=se; ++si, ++li) {
		const string& nn(si->get_name());
		if (nn.length()) {
			li->name = nn;
		} else {
			ostringstream oss;
			oss << "INTSUB:" << subs_count;
			li->name = oss.str();
			++subs_count;
		}
	}
	// TODO: pre-size instance_pool based on proceses
	// TODO: pre-size named_node_map based on bools
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Like constructor, initializes footprint and related members.
 */
void
netlist::bind_footprint(const footprint& f, const netlist_options& nopt) {
	// TODO: format or mangle type name, e.g. template brackets
	__bind_footprint(f);
	ostringstream oss;
	f.dump_type(oss);
	name = oss.str();
	strgsub(name, " ", "");		// remove spaces (template params)
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Overload with overridden name, e.g. for top-level.
 */
void
netlist::bind_footprint(const footprint& f, const string& n) {
	__bind_footprint(f);
	name = n;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param subp global process entry containing footprint frame
		Should we use local footprint's instance instead?
		Technically, we shouldn't need global allocation information.
	\param subnet the netlist type that corresponds with instance.
	\param lpid local process id from footprint.
	\pre instance_pool is already pre-allocated to avoid 
		invalidating references due to re-allocation.
 */
void
netlist::append_instance(const global_entry<process_tag>& subp,
		const netlist& subnet, const index_type lpid) {
	STACKTRACE_VERBOSE;
	const footprint* subfp = subp._frame._footprint;
	// cannot use global allocated footprint_frame
//	const netlist& subnet(netmap.find(subfp)->second);
	INVARIANT(subfp == subnet.fp);
	// subnet's port list may be shorter than formals list, due to aliases
	// traverse formals list and lookup each actual to be passed
	// recall: only used nodes will be in this port list
	instance_pool.push_back(instance(subnet, lpid));
	instance& np(instance_pool.back());
	// local process instance needed to find local port actual id
	const instance_alias_info<process_tag>&
		lp(*fp->get_instance_pool<process_tag>()[lpid].get_back_ref());
	// ALERT: translates to global index, not what we want!
	netlist::port_list_type::const_iterator
		fi(subnet.port_list.begin()), fe(subnet.port_list.end());
	for ( ; fi!=fe; ++fi) {
		STACKTRACE_INDENT_PRINT("formal node = " << *fi << endl);
		const node& fn(subnet.node_pool[*fi]);	// formal node
		INVARIANT(fn.used);
		if (fn.is_supply_node()) {
			if (*fi == GND_index) {
				np.actuals.push_back(GND_index);
			} else if (*fi == Vdd_index) {
				np.actuals.push_back(Vdd_index);
			} else {
				cerr << "ERROR: unknown supply port." << endl;
				THROW_EXIT;
			}
		} else if (fn.is_logical_node()) {
			const index_type fid = fn.index;
			STACKTRACE_INDENT_PRINT("formal id = " << fid << endl);

			const instance_alias_info<bool_tag>&
				fb(*subnet.fp->get_instance_pool<bool_tag>()[fid].get_back_ref());
			INVARIANT(fb.is_aliased_to_port());
			index_type actual_id = 0;
			// ALERT: fb might pick a non-port alias!!!
			// picking *any* port-alias should suffice because
			// formal port-aliases have been replayed.
		if (!fb.is_port_alias()) {
			// HACK ALERT!
			// well, damn it, FIND me a suitable alias!
//			fb.dump_hierarchical_name(cerr << "ALIAS: ") << endl;
			typedef port_alias_tracker_base<bool_tag>::map_type
						alias_map_type;
			const alias_map_type&
				pat(subfp->get_scope_alias_tracker()
					.get_id_map<bool_tag>());
			alias_map_type::const_iterator
				f(pat.find(fid)), e(pat.end());
			INVARIANT(f != e);
			const alias_reference_set<bool_tag>& rs(f->second);
			alias_reference_set<bool_tag>::const_iterator
				ai(rs.begin()), ae(rs.end());
			for ( ; ai!=ae; ++ai) {
			if ((*ai)->is_port_alias()) {
				actual_id = (*ai)->trace_alias(lp).instance_index;
				break;
			}
			}
			INVARIANT(ai != ae);
		} else {
			const instance_alias_info<bool_tag>&
				ab(fb.trace_alias(lp));
			actual_id = ab.instance_index;
		}
			INVARIANT(actual_id);
			STACKTRACE_INDENT_PRINT("LOCAL actual id = " << actual_id << endl);
			const index_type actual_node =
				register_named_node(actual_id);
			STACKTRACE_INDENT_PRINT("actual node = " << actual_node << endl);
			np.actuals.push_back(actual_node);
		} else {
			cerr << "ERROR: unhandled instance port node type."
				<< endl;
			THROW_EXIT;
		}
		// else skip for now
	}
#if ENABLE_STACKTRACE
	np.dump_raw(STACKTRACE_INDENT_PRINT("new instance: ")) << endl;
#endif
	INVARIANT(np.actuals.size() == subnet.port_list.size());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Allocates a new auxiliary node, incrementing counter.
	\return index of new node, 1-indexed.
 */
index_type
netlist::create_auxiliary_node(void) {
	STACKTRACE_VERBOSE;
	const node n(aux_count, node::auxiliary_node_tag);
	++aux_count;
	const index_type ret = node_pool.size();
	node_pool.push_back(n);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Allocates a new named internal node, incrementing counter.
	\param ni is the footprint-local index of the internal node (0-indexed).
	\return index of new node, 1-indexed into this netlist.
 */
index_type
netlist::create_internal_node(const index_type ni, const index_type ei) {
	STACKTRACE_VERBOSE;
	const node n(ni, node::internal_node_tag);
	const index_type ret = node_pool.size();
	node_pool.push_back(n);
	INVARIANT(ni < internal_node_map.size());
	internal_node_map[ni].first = ret;
	internal_expr_map[ei] = ni;	// invariant: no duplicates
	// set owner later!
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Lookup a previously defined internal node, keyed by
	PRS footprint's expr index.
	\param ei local footprint's expr index corresponding to
		an internal node.
	\return footprint's internal node index, which can be 0.  
 */
index_type
netlist::lookup_internal_node(const index_type ei) const {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("expr-id: " << ei << endl);
	const internal_expr_map_type::const_iterator
		f(internal_expr_map.find(ei));
	INVARIANT(f != internal_expr_map.end());
	// ALERT: only true if we've processed all internal nodes in a priori
	return f->second;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	As each named logical node is visited for the first time, 
	register it with the map of named nodes, where the value
	is the corresponding index of the *subcircuit* node.
	This way, unused bools are not emitted.  
	The named_node_map will be 0-indexed, even though footprint_frames
		are 1-indexed (for compactness).  
	\param _i is the footprint's local node index, never 0.
	\return 1-indexed local netlist's generic node index, never 0.
 */
index_type
netlist::register_named_node(const index_type _i) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("local id (+1) = " << _i << endl);
	INVARIANT(_i);
	const index_type i = _i -1;	// locally 0-indexed, no gap
	INVARIANT(i < named_node_map.size());
	index_type& ret(named_node_map[i]);
	if (!ret) {
		// reserve a new slot and update it for subsequent visits
		const node new_named_node(_i, node::logical_node_tag);
		ret = node_pool.size();
		INVARIANT(ret);
		node_pool.push_back(new_named_node);
#if ENABLE_STACKTRACE
node_pool.back().dump_raw(STACKTRACE_INDENT_PRINT("new node: ")) << endl;
#endif
		// mark new node as used here?
	}	// else already mapped
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Note which nodes are actually used, to avoid emitting unused nodes.
 */
void
netlist::mark_used_nodes(void) {
	STACKTRACE_VERBOSE;
	netlist_common::mark_used_nodes(node_pool);
{
	instance_pool_type::iterator
		i(instance_pool.begin()), e(instance_pool.end());
	for ( ; i!=e; ++i) {
		i->mark_used_nodes(node_pool);
	}
}{
	// traverse subcircuits
	local_subcircuit_list_type::iterator
		i(local_subcircuits.begin()), e(local_subcircuits.end());
	for ( ; i!=e; ++i) {
		i->mark_used_nodes(node_pool);
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param ni is a footprint-local id for the referenced bool.
 */
bool
netlist::named_node_is_used(const index_type ni) const {
	// translate to netlist-local index
	const index_type lni = named_node_map[ni];
	// must be non-zero to count
	return lni && node_pool[lni].used;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre all dependent subinstances have been processed.
		This should really only be called from netlist_generator.
	\param sub is true if this is a subckt and should be wrapped
	in .subckt/.ends with ports declared.
	TODO: option to emit dangling unused internal nodes?
 */
ostream&
netlist::emit(ostream& o, const bool sub, const netlist_options& nopt) const {
if (!nopt.nested_subcircuits) {
	// print definition
	// print singleton instance
	typedef	local_subcircuit_list_type::const_iterator	const_iterator;
	const_iterator i(local_subcircuits.begin()), e(local_subcircuits.end());
	for ( ; i!=e; ++i) {
		i->emit_definition(o, *this, nopt);	// definition
	}
}
if (sub) {
	o << ".subckt " << name;
	typedef	port_list_type::const_iterator		const_iterator;
	const_iterator i(port_list.begin()), e(port_list.end());
	for ( ; i!=e; ++i) {
		node_pool[*i].emit(o << ' ', *fp);	// options?
	}
	o << endl;
	// TODO: emit port-info comments
}
if (sub || nopt.emit_top) {
	// option to suppress top-level instances and rules
{
	// emit subinstances
#if ENABLE_STACKTRACE
	o << "* instances:" << endl;
#endif
	typedef	instance_pool_type::const_iterator	const_iterator;
	size_t j = 0;	// DEBUG
	const_iterator i(instance_pool.begin()), e(instance_pool.end());
	for ( ; i!=e; ++i, ++j) {
		STACKTRACE_INDENT_PRINT("j = " << j << endl);
	if (!i->is_empty()) {	// TODO: netlist_option for empty_instances?
		i->emit(o, node_pool, *fp) << endl;	// options?
	}
	}
}{
#if ENABLE_STACKTRACE
	// nested subcircuits?
	o << "* local subcircuits:" << endl;
#endif
	// print definition
	// print singleton instance
	typedef	local_subcircuit_list_type::const_iterator	const_iterator;
	const_iterator i(local_subcircuits.begin()), e(local_subcircuits.end());
	for ( ; i!=e; ++i) {
	if (nopt.nested_subcircuits) {
		i->emit_definition(o, *this, nopt);	// definition
	}
		i->emit_instance(o, *this, nopt);	// instance
	}
	// alternately, emit subcircuits outside, not nested
}
	emit_devices(o, node_pool, *fp, nopt);
}
if (sub) {
	o << ".ends" << endl;
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Debug-only mode for printing raw structure of netlist.
 */
ostream&
netlist::dump_raw(ostream& o) const {
	ostream_iterator<index_type> osi(o, ",");
	o << "netlist {" << endl;
	o << "type: " << name << endl;
	o << "footprint @ " << fp << endl;
	fp->dump(o) << endl;
	o << "node pool:" << endl;
{
	size_t j = 0;
	for ( ; j<node_pool.size(); ++j) {
		o << "  [" << j << "]: ";
		const node& n(node_pool[j]);
		n.dump_raw(o) << " = ";
#if 1
		n.emit(o, *fp);	// may have to comment out for debug
#else
		o << "...";
#endif
		o << endl;
	}
}{
	o << "ports (node indices): ";
	copy(port_list.begin(), port_list.end(), osi);
	o << endl;
}{
	o << "named node map (footprint-index -> netlist-node-index):" << endl;
	size_t j = 0;
	for ( ; j<named_node_map.size(); ++j) {
		o << "  :" << j+1 << " -> [" << named_node_map[j] << ']' << endl;
	}
}{
	o << "internal node map (footprint-index -> netlist-node-index):" << endl;
	typedef	internal_node_map_type::const_iterator	const_iterator;
	const_iterator i(internal_node_map.begin()), e(internal_node_map.end());
	size_t j = 0;
	for ( ; i!=e; ++i) {
		const prs_footprint::node_expr_type&
			n(fp->get_prs_footprint().get_internal_node(j));
		o << "  @" << j << " -> [" << i->first << ']' <<
			(n.second ? '+' : '-');
		if (i->second) {
			o << " in subckt{" << i->second -1 << "}";
		}
		o << endl;
	}
}{
	o << "instances:" << endl;
	typedef	instance_pool_type::const_iterator	const_iterator;
	const_iterator i(instance_pool.begin()), e(instance_pool.end());
	for ( ; i!=e; ++i) {
		i->dump_raw(o << "  ") << endl;
	}
}{
#if ENABLE_STACKTRACE
	o << "subcircuits: {" << endl;
#endif
	// represents both definition and instance
	typedef	local_subcircuit_list_type::const_iterator	const_iterator;
	const_iterator i(local_subcircuits.begin()), e(local_subcircuits.end());
	for ( ; i!=e; ++i) {
		i->dump_raw(o, *this);
	}
	o << '}' << endl;
}{
	o << "transistors:" << endl;
	dump_raw_devices(o);
}
	return o << '}' << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct bool_port_alias_collector :
		public bool_port_collector<unique_list<index_type> > {
	typedef	bool_port_collector<unique_list<index_type> >
						parent_type;
	VISIT_INSTANCE_ALIAS_INFO_PROTO(bool_tag);
	using parent_type::visit;
};

void
bool_port_alias_collector::visit(const instance_alias_info<bool_tag>& a) {
	if (a.is_aliased_to_port()) {
		parent_type::visit(a);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Create a port summary so that other processes may correctly
	instantiate this.  
	This also summarizes the 'empty' flag for this netlist.  
	TODO: power supply ports
 */
void
netlist::summarize_ports(void) {
	STACKTRACE_VERBOSE;
	// could mark_used_nodes here instead?
	if (node_pool[GND_index].used) {
		port_list.push_back(GND_index);
	}
	if (node_pool[Vdd_index].used) {
		port_list.push_back(Vdd_index);
	}
{
	typedef	unique_list<index_type>	port_index_list_type;
	bool_port_alias_collector V;
	// this ordering is based on port_formal_manager, declaration order
	const port_formals_manager&
		fm(fp->get_owner_def().is_a<const process_definition>()
			->get_port_formals());
	port_formals_manager::const_list_iterator pi(fm.begin()), pe(fm.end());
	for ( ; pi!=pe; ++pi) {
		(*fp)[(*pi)->get_name()]
			.is_a<const physical_instance_collection>()->accept(V);
	}
	port_index_list_type::const_iterator
		i(V.bool_indices.begin()), e(V.bool_indices.end());
	port_list.reserve(V.bool_indices.size() +2);	// for supplies
	for ( ; i!=e; ++i) {
		// 1-indexed local id to 0-indexed named_node_map
		INVARIANT(*i);
		const index_type local_ind = *i -1;
		const index_type ni = named_node_map[local_ind];
		const node& n(node_pool[ni]);
	if (ni && n.used) {
		INVARIANT(n.is_logical_node());
		INVARIANT(n.index == *i);	// self-reference
		port_list.push_back(ni);
		// sorted_ports[local_ind] = ni;
	}
	}
}
	// empty is initially false
	bool MT = true;
	MT = transistor_pool.empty() && passive_device_pool.empty();
{
	// check subcircuits
	local_subcircuit_list_type::const_iterator
		i(local_subcircuits.begin()), e(local_subcircuits.end());
	for ( ; MT && i!=e; ++i) {
	if (!i->is_empty()) {
		MT = false;	// stop on non-empty subcircuit
	}
	}
}{
	// check instances
	instance_pool_type::const_iterator
		i(instance_pool.begin()), e(instance_pool.end());
	for ( ; MT && i!=e; ++i) {
	if (!i->is_empty()) {
		MT = false;	// stop on non-empty instance
	}
	}
}
	empty = MT;
}

//=============================================================================
// class netlist_generator method definitions

netlist_generator::netlist_generator(const state_manager& _sm,
		const footprint& _topfp, ostream& o, 
		const netlist_options& p) :
		cflat_context_visitor(_sm, _topfp), os(o), opt(p), netmap(),
		prs(NULL), 
		current_netlist(NULL), 
		current_local_netlist(NULL),
		foot_node(void_index), output_node(void_index),
		fet_type(transistor::NFET_TYPE), 	// don't care
		fet_attr(transistor::DEFAULT_ATTRIBUTE),
		negated(false),
		// TODO: set these to technology defaults
		last_width(0.0), 
		last_length(0.0) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
netlist_generator::~netlist_generator() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Main function for starting hierarchy traversal for netlist generation.
	\pre top-level footprint contains top-level instances in
		reserved process slot [0] in the state_manager.
	Algorithm: starting with top-level footprint
		for all dependent subcircuit types
			recurse (pre-order)
		process local rules
		create port map and summary
 */
void
netlist_generator::operator () (void) {
	STACKTRACE_VERBOSE;
	NEVER_NULL(sm);
	const global_entry<process_tag>& ptop(sm->get_pool<process_tag>()[0]);
	visit(ptop);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't know if anything needs to be done here.  
	Perhaps apply attributes to corresponding mapped local netlist nodes.  
 */
void
netlist_generator::visit(const global_entry<bool_tag>& p) {
	STACKTRACE_VERBOSE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Re-maps insert() member function into push_back().
 */
template <class Sequence>
struct set_adaptor : public Sequence {
	typedef	Sequence				sequence_type;
	typedef	typename sequence_type::const_reference	const_reference;

	// everything else inherited

	void
	insert(const_reference v) {
		sequence_type::push_back(v);
	}
};	// end struct set_adaptor

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visit all dependent subcircuits (processes) *first*.
	Each unique type is only visited once.  
	Remember that top-most level should not be wrapped in subcircuit.  
	Q: Is there a way to traverse the footprint without using the
	footprint_frame?  This should be a non-global hierarchical traversal.
 */
void
netlist_generator::visit(const global_entry<process_tag>& p) {
	STACKTRACE_VERBOSE;
	// don't need to temporarily set the footprint_frame
	// because algorithm is completely hierarchical, no flattening
	// will need p._frame when emitting subinstances
	const footprint* f(p._frame._footprint);
	NEVER_NULL(f);
	INVARIANT(f->is_created());	// don't need is_allocated()!!!
	netlist_map_type::iterator mi(netmap.find(f));
	const bool first_time = (mi == netmap.end());
	const bool top_level = !current_netlist;
if (first_time) {
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("processing unique type: ");
	f->dump_type(STACKTRACE_STREAM) << endl;
#endif
	netlist* nl = &netmap[f];	// insert default constructed
if (f == topfp) {
	nl->bind_footprint(*f, "<top-level>");
} else {
	nl->bind_footprint(*f, opt);
}
	// initialize netlist:
try {
#if 1
	const footprint_frame_map_type&
		bfm(p._frame.get_frame_map<bool_tag>());
		// ALERT: top-footprint frame's size will be +1!
	nl->named_node_map.resize(bfm.size(), void_index);	// 0-fill
	// resize(f->get_instance_pool<bool_tag>().size()) ???
	STACKTRACE_INDENT_PRINT("bfm.size = " << bfm.size() << endl);
	// set current netlist (duplicate for local):
	const value_saver<netlist*> __tmp(current_netlist, nl);
	const value_saver<netlist_common*> __tmp2(current_local_netlist, nl);
	// should not invalidate existing iterators
	const footprint_frame_map_type&
		pfm(p._frame.get_frame_map<process_tag>());
	// TODO: instead of relying on globally allocated footprint frame
	// just traverse the footprint's instance_pool<process_tag>
	// this way, don't require global allocation first, only create phase!
	STACKTRACE_INDENT_PRINT("pfm.size = " << pfm.size() << endl);
	nl->instance_pool.reserve(pfm.size());	// prevent reallocation!!!
	typedef	footprint_frame_map_type::const_iterator	const_iterator;
	const_iterator i(pfm.begin()), e(pfm.end());
	// skip first NULL slot?
	// ALERT: top-level's process frame starts at 1, not 0!
	index_type lpid = top_level ? 0 : 1;
	for (; i!=e; ++i, ++lpid) {
	if (*i) {
		STACKTRACE_INDENT_PRINT("examining sub-process id " << *i <<
			", local id " << lpid << endl);
		// const index_type lpid = *i +1;
		const global_entry<process_tag>&
			subp(sm->get_pool<process_tag>()[*i]);
		// no need to set footprint frames (global use only)
		visit(subp);	// recursion
		// guarantee that dependent type is processed with netlist
		// find out how local nodes are passed to *local* instance
		const footprint* subfp = subp._frame._footprint;
		const netlist& subnet(netmap.find(subfp)->second);
		nl->append_instance(subp, subnet, lpid);
	}
	}
#else
	// this would be easier if there was a local state_manager per footprint
	// TODO: do this after scalability re-work
	typedef	state_instance<process_tag>::pool_type	process_pool_type;
	const process_pool_type& pp(f->get_instance_pool<process_tag>());
	process_pool_type::const_iterator i(pp.begin()), e(pp.end());
	size_t j = ...;
	for ( ; i!=e; ++i) {
		...
		// visit local subprocess or fake one
	}
#endif
	// process local production rules and macros
	f->get_prs_footprint().accept(*this);
	// f->get_spec_footprint().accept(*this);	// ?
	if (!top_level) {
		nl->summarize_ports();
	}
} catch (...) {
	cerr << "ERROR producing netlist for " << nl->name << endl;
	throw;
}
	// finally, emit this process
#if ENABLE_STACKTRACE
	nl->dump_raw(cerr);	// DEBUG point
#endif
if (!nl->is_empty()) {		// TODO: netlist_option show_empty_subcircuits
	nl->emit(os, !top_level, opt) << endl;
} else {
	os << "* subcircuit " << nl->name << " is empty." << endl;
}
}
	// if this is not top-level, wrap emit in .subckt/.ends
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
netlist_generator::visit(const state_manager& s) {
	STACKTRACE_VERBOSE;
	// never called, do nothing
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Traverse footprint in *subcircuit* order first.  
	Would this traversal be useful to other classes?
 */
void
netlist_generator::visit(const entity::PRS::footprint& r) {
	STACKTRACE_VERBOSE;
	// no local name translation needed for subcircuits
	typedef	prs_footprint::subcircuit_map_type	subckt_map_type;
	typedef	subckt_map_type::const_iterator		const_iterator;
	// must be sorted ranges
	const value_saver<const prs_footprint*> __prs(prs, &r);
	// for now, default supplies
	const value_saver<index_type> __s1(low_supply, GND_index);
	const value_saver<index_type> __s2(high_supply, Vdd_index);
{
	STACKTRACE_INDENT_PRINT("reserving internal nodes..." << endl);
	// Internal node definitions may have a dependency ordering
	// that is not reflected by the container, thus, we cannot simply
	// iterate over the footprint's internal-node-map (string-keyed).
	// Instead, only visit internal nodes on-demand
	//	to solve problem of unused dangling internal nodes.
	// PRS::footprint maps from string (name of internal node) to 
	// a local internal node index, which is mapped to a subcircuit
	// node index here (reverse map).
	const prs_footprint::internal_node_pool_type&
		inode_pool(prs->get_internal_node_pool());
	prs_footprint::internal_node_pool_type::const_iterator
		i(inode_pool.begin()), e(inode_pool.end());
	size_t j = 0;
	for ( ; i!=e; ++i, ++j) {
		// each entry is a node_expr_type
		// where pair:first is expr-index and second is direction
		// using footprint's internal_node_pool
		// as basis for subcircuits internal_node_map
		const index_type& expr = i->first;
		// direction and name can be looked up later
		const index_type new_int =
			current_netlist->create_internal_node(j, expr);
		INVARIANT(new_int);
		// asserts map entry exists:
		current_netlist->lookup_internal_node(expr);
	}
	// now walk subcircuit map to assign owner subcircuits
	const subckt_map_type& subc_map(prs->get_subcircuit_map());
	const_iterator si(subc_map.begin()), se(subc_map.end());
	j = 1;	// owner 1-indexed, default upon ctor was 0
	for ( ; si!=se; ++si, ++j) {
		size_t k = si->int_nodes.first;
		for ( ; k < si->int_nodes.second; ++k) {
			current_netlist->internal_node_map[k].second = j;
		}
	}
#if 0 && ENABLE_STACKTRACE
	current_netlist->dump_raw(STACKTRACE_STREAM) << endl;
#endif
}
	// traverse subcircuits
	// ALERT: if we ever want to perform other operations before emitting
	// these local subcircuit structures need to be kept around longer
	// because the instances of them would be dangling pointers...
	const subckt_map_type& subc_map(prs->get_subcircuit_map());
{
	STACKTRACE_INDENT_PRINT("processing subcircuits (rules) ..." << endl);
	const_iterator si(subc_map.begin()), se(subc_map.end());
	netlist::local_subcircuit_list_type::iterator
		mi(current_netlist->local_subcircuits.begin());
	while (si!=se && si->rules_empty()) { ++si; ++mi; }	// skip empty
	const prs_footprint::rule_pool_type& rpool(prs->get_rule_pool());
	const size_t s = rpool.size();
	size_t i = 0;
	for ( ; i<s; ++i) {
	if (si!=se && (i >= si->rules.first)) {
		INVARIANT(mi != current_netlist->local_subcircuits.end());
		local_netlist& n(*mi);
		const value_saver<netlist_common*>
			__tmp(current_local_netlist, &n);
		for ( ; i < si->rules.second; ++i) {
			rpool[i].accept(*this);
		}
			--i;	// back-adjust before continue
		// advance to next non-empty subcircuit
		do { ++si; ++mi; } while (si!=se && si->rules_empty());
	} else {
		// rule is outside of subcircuits
		rpool[i].accept(*this);
	}
	}	// end for
}{
	STACKTRACE_INDENT_PRINT("processing macros..." << endl);
	const_iterator si(subc_map.begin()), se(subc_map.end());
	netlist::local_subcircuit_list_type::iterator
		mi(current_netlist->local_subcircuits.begin());
	while (si!=se && si->macros_empty()) { ++si; ++mi; }	// skip empty
	const prs_footprint::macro_pool_type& mpool(prs->get_macro_pool());
	const size_t s = mpool.size();
	size_t i = 0;
	for ( ; i<s; ++i) {
	if (si!=se && (i >= si->macros.first)) {
		// start of a subcircuit range, can be empty
		INVARIANT(mi != current_netlist->local_subcircuits.end());
		local_netlist& n(*mi);
			const value_saver<netlist_common*>
				__tmp(current_local_netlist, &n);
		for ( ; i < si->macros.second; ++i) {
			mpool[i].accept(*this);
		}
			--i;	// back-adjust before continue
		// advance to next non-empty subcircuit
		do { ++si; ++mi; } while (si!=se && si->macros_empty());
	} else {
		// macro is outside of subcircuits
		mpool[i].accept(*this);
	}
	}	// end for
}
	// process all subcircuits first, then remaining local rules/macros
	current_netlist->mark_used_nodes();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Construct subgraph from LHS expression, and connect final
	output(s) to output node.
	\pre current_netlist is set
 */
void
netlist_generator::visit(const entity::PRS::footprint_rule& r) {
	STACKTRACE_VERBOSE;
	// set foot_node and output_node and fet_type
	const value_saver<index_type>
		__t1(foot_node, (r.dir ? Vdd_index : GND_index)),
		__t2(output_node, register_named_node(r.output_index));
	const value_saver<transistor::fet_type>
		__t3(fet_type, (r.dir ? transistor::PFET_TYPE : transistor::NFET_TYPE));
	// TODO: honor prs supply override directives
	const prs_footprint::expr_pool_type& ep(prs->get_expr_pool());
try {
	ep[r.expr_index].accept(*this);
} catch (...) {
	// TODO: better diagnostic tracing message
	cerr << "ERROR in production rule." << endl;
	throw;
}
	// TODO: process rule attributes, labels, names...
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursive, generate on-demand internal nodes as they are visited.
	\pre all internal nodes have been allocated/mapped a priori, 
		just not necessarily defined (node::used).
	\param nid is the footprint-local internal node index (0-indexed).
	\return netlist-local node index representing the internal node.
	\invariant no cyclic definitions of internal nodes possible.
 */
index_type
netlist_generator::register_internal_node(const index_type nid) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("@id = " << nid << endl);
	const prs_footprint::expr_pool_type& ep(prs->get_expr_pool());
	const prs_footprint::node_expr_type& ret(prs->get_internal_node(nid));
	const index_type& defexpr = ret.first;
	STACKTRACE_INDENT_PRINT("@expr = " << defexpr << endl);
	const netlist::internal_node_entry_type&
		r(current_netlist->internal_node_map[nid]);
	const index_type& node_ind = r.first;
	const index_type& node_own = r.second;	// home
	node& n(current_netlist->node_pool[node_ind]);
if (!n.used) {
	STACKTRACE_INDENT_PRINT("defining internal node..." << endl);
	const bool dir = ret.second;
	// else need to define internal node once only
	// TODO: honor prs supply override directives
	const value_saver<index_type>
		__t1(foot_node, (dir ? Vdd_index : GND_index)),
		__t2(output_node, node_ind);
	const value_saver<transistor::fet_type>
		__t3(fet_type,
			(dir ? transistor::PFET_TYPE : transistor::NFET_TYPE));
	n.used = true;
	// mark before recursion, not after!
	// to prevent shared roots from being duplicated
	STACKTRACE_INDENT_PRINT("owner = " << node_own << endl);
	if (node_own) {
	// generate in correct owner subcircuit
	// internal nodes' partial rules can belong to local subcircuits
	// but are accessible to all sibling subcircuits
	// within a process definition.
		STACKTRACE("pointing to subcircuit");
		const value_saver<netlist_common*>
			__t4(current_local_netlist, 
				&current_netlist->local_subcircuits[node_own -1]);
		ep[defexpr].accept(*this);
	} else {
		// point back to main scope
		const value_saver<netlist_common*>
			__t4(current_local_netlist, current_netlist);
		ep[defexpr].accept(*this);
	}
}
	return node_ind;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers node for first time or just returns previously registered
	index.  Also marks node as used.  
 */
index_type
netlist_generator::register_named_node(const index_type n) {
	NEVER_NULL(current_netlist);
	const index_type ret = current_netlist->register_named_node(n);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper function for visiting precharge expressions.
 */
void
netlist_generator::visit(const footprint_expr_node::precharge_pull_type& p) {
	// recall: precharge is 0-indexed
	const prs_footprint::expr_pool_type& ep(prs->get_expr_pool());
	const index_type& pchgex = p.first;
	const bool dir = p.second;
	const value_saver<index_type>
		_t3(foot_node, dir ? high_supply : low_supply);
	const value_saver<transistor::fet_type>
		_t4(fet_type, dir ? transistor::PFET_TYPE
			: transistor::NFET_TYPE);
	const value_saver<transistor::flags>
		_t5(fet_attr, transistor::flags(
			fet_attr | transistor::IS_PRECHARGE));
	// use the same output node
	ep[pchgex].accept(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Walk expressions from left to right, emitting transistors.
	\pre current_netlist is set
	\pre foot_node and output_node are already set to existing nodes, 
		e.g. Vdd, GND, or auxiliary node.
 */
void
netlist_generator::visit(const footprint_expr_node& e) {
	STACKTRACE_VERBOSE;
	const prs_footprint::expr_pool_type& ep(prs->get_expr_pool());
	// exception: if encountering an internal node on left-most 
	// expression, then set foot_node instead of emitting a device.
const char type = e.get_type();
switch (type) {
case PRS_LITERAL_TYPE_ENUM: {
	STACKTRACE_INDENT_PRINT("expr is leaf node" << endl);
	// TODO: check for negation normalization
	if (negated ^ (fet_type == transistor::PFET_TYPE)) {
		cerr << "ERROR: rule-literal is not CMOS-implementable." << endl;
		THROW_EXIT;
	}
	transistor t;
	t.type = fet_type;
	// TODO: handle FET type override
	t.gate = register_named_node(e.only());
	t.source = foot_node;
	t.drain = output_node;
	// TODO: honor supply overrides
	t.body = (fet_type == transistor::NFET_TYPE ? low_supply : high_supply);
		// Vdd or GND
	// TODO: extract length/width parameters
	const directive_base_params_type& p(e.get_params());
	if (p.size() > 0) {
		t.width = p[0]->to_real_const();
	} else {
		t.width = (fet_type == transistor::NFET_TYPE ?
			opt.std_n_width : opt.std_p_width);
	}
	if (p.size() > 1) {
		t.length = p[1]->to_real_const();
	} else {
		t.length = (fet_type == transistor::NFET_TYPE ?
			opt.std_n_length : opt.std_p_length);
	}
	t.attributes = fet_attr;
	// TODO: import attributes from rule attributes?
	NEVER_NULL(current_local_netlist);
	current_local_netlist->transistor_pool.push_back(t);
	break;
}
case PRS_NOT_EXPR_TYPE_ENUM: {
	STACKTRACE_INDENT_PRINT("expr is negation" << endl);
	// automatically negation normalize
	// toggle negation of subexpressions
	const value_saver<bool> __tmp(negated, !negated);
	ep[e.only()].accept(*this);
	break;
}
case PRS_AND_EXPR_TYPE_ENUM:	// fall-through
case PRS_OR_EXPR_TYPE_ENUM: {
	const bool is_conjunctive = (type == PRS_AND_EXPR_TYPE_ENUM) ^ negated;
	size_t i = 1;
	const size_t s = e.size();
	if (is_conjunctive) {
		STACKTRACE_INDENT_PRINT("expr is conjunctive" << endl);
		typedef footprint_expr_node::precharge_map_type
				precharge_map_type;
		const footprint_expr_node::precharge_map_type&
			p(e.get_precharges());
		precharge_map_type::const_iterator
			pi(p.begin()), pe(p.end());
		// special case: first branch is an internal node
		index_type prev;
		const footprint_expr_node& left(ep[e[i]]);
		if (left.get_type() == PRS_NODE_TYPE_ENUM) {
			// left.only() is still an expression index, 
			// but we need internal-node-index.
			prev = register_internal_node(
				current_netlist->lookup_internal_node(
					left.only()));
			// confirm direction and sense of internal node
			// already checked at create-phase.
			// handle precharge
			if (pi != pe && pi->first == i-1) {
				const value_saver<index_type>
					_t1(output_node, prev);
				// recall: precharge is 0-indexed
				visit(pi->second);
				++pi;
			}
			++i;
		} else {
			prev = foot_node;
		}
		// create intermediate nodes
		for ( ; i<s; ++i) {		// all but last node
			// setup foot and output, then recurse!
			const value_saver<index_type>
				_t1(foot_node, prev),
				_t2(output_node, 
					current_netlist->create_auxiliary_node());
			ep[e[i]].accept(*this);
			prev = output_node;
			// handle precharge
			if (pi != pe && pi->first == i-1) {
				// recall: precharge is 0-indexed
				visit(pi->second);
				++pi;
			}
		}
		{
		// last node connected to output
			const value_saver<index_type> _t1(foot_node, prev);
			// use current output_node, not new auxiliary node
			ep[e[i]].accept(*this);
		}
	} else {
		STACKTRACE_INDENT_PRINT("expr is disjunctive" << endl);
		// take OR combination:
		// don't need to add any auxiliary nodes,
		// share the same foot and output nodes.
		for (; i<=s; ++i) {
			ep[e[i]].accept(*this);
		}
	}
	break;
}
case PRS_NODE_TYPE_ENUM: {
	// ERROR: unexpected internal node out of position
	cerr << "ERROR: found internal node in unexpected position." << endl;
	THROW_EXIT;
	break;
}
default:
	DIE;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Expand a macro into netlist.
	Examples: passn and passp for pass-gates.
	\pre current_netlist is set
	TODO: eventually implement using a function table/map
 */
void
netlist_generator::visit(const entity::PRS::footprint_macro& e) {
	STACKTRACE_VERBOSE;
	const bool passn = (e.name == "passn");
	const bool passp = (e.name == "passp");
if (passn || passp) {
	transistor t;
	t.type = passp ? transistor::PFET_TYPE : transistor::NFET_TYPE;
	// TODO: override with vt types
	t.gate = register_named_node(*e.nodes[0].begin());
	t.source = register_named_node(*e.nodes[1].begin());
	t.drain = register_named_node(*e.nodes[2].begin());
	t.body = passp ? high_supply : low_supply;
	const directive_base_params_type& p(e.params);
	if (p.size() > 0) {
		t.width = p[0]->to_real_const();
	} else {
		t.width = (passn ? opt.std_n_width : opt.std_p_width);
	}
	if (p.size() > 1) {
		t.length = p[1]->to_real_const();
	} else {
		t.length = (passn ? opt.std_n_length : opt.std_p_length);
	}
	t.attributes = fet_attr;
	// TODO: import attributes
	NEVER_NULL(current_local_netlist);
	current_local_netlist->transistor_pool.push_back(t);
} else if (e.name == "echo") {
	// do nothing
} else {
	cerr << "WARNING: unknown PRS macro " << e.name
		<< " ignored." << endl;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
netlist_generator::visit(const entity::SPEC::footprint&) {
	// nothing... yet
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
netlist_generator::visit(const entity::SPEC::footprint_directive&) {
	// nothing... yet
}

//=============================================================================
}	// end namespace NET
}	// end namespace HAC

