/**
	\file "Object/def/port_formals_manager.cc"
	Method definitions for port_formals_manager.
	This file was "Object/def/port_formals_manager.cc"
		in a former life.  
 	$Id: port_formals_manager.cc,v 1.18 2011/04/02 01:45:57 fang Exp $
 */

#define ENABLE_STACKTRACE		0

//=============================================================================
#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <exception>
#include <iostream>
#include <functional>
#include <iterator>

#include "util/hash_specializations.h"		// substitute for the following
#include "AST/token_string.h"	// for token_identifier

#include "Object/def/port_formals_manager.h"
#include "Object/persistent_type_hash.h"
#include "Object/inst/physical_instance_placeholder.h"
#include "Object/inst/physical_instance_collection.h"
#include "Object/inst/subinstance_manager.h"
#include "Object/ref/meta_instance_reference_base.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/common/dump_flags.h"

#include "util/memory/count_ptr.tcc"
#include "util/indent.h"
#include "util/stacktrace.h"
#include "util/persistent_object_manager.tcc"

//=============================================================================
namespace HAC {
namespace entity {
using std::_Select2nd;
#include "util/using_ostream.h"
using util::indent;
using util::auto_indent;
using util::write_value;
using util::read_value;
using util::write_string;
using util::read_string;
using std::distance;

//=============================================================================
// class port_formals_manager method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
port_formals_manager::port_formals_manager() :
		port_formals_list(), port_formals_map()
#if IMPLICIT_SUPPLY_PORTS
		, __implicit_ports(0), __explicit_ports(0)
#endif
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
port_formals_manager::~port_formals_manager() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
port_formals_manager::what(ostream& o) const {
	return o << "port-formals-manager";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Spill contents of the used_id_map.
	\param o the output stream.
	\return the same output stream.
 */
ostream&
port_formals_manager::dump(ostream& o) const {
//	STACKTRACE_VERBOSE;
	o << '(' << endl;
	{
		INDENT_SECTION(o);
		port_formals_list_type::const_iterator
			i(port_formals_list.begin());
		const port_formals_list_type::const_iterator
			e(port_formals_list.end());
		for ( ; i!=e; i++) {
			(*i)->dump_formal(o << auto_indent) << endl;
		}
	}
	return o << auto_indent << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Override's definition_base's port formal lookup.  
	\return pointer to port's instantiation if found, else NULL.  
 */
port_formals_manager::port_formals_value_type
port_formals_manager::lookup_port_formal(const string& id) const {
	const port_formals_map_type::const_iterator
		f(port_formals_map.find(id));
	return (f != port_formals_map.end()) ? f->second :
		port_formals_value_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return 1-indexed position in the list of the referenced
	      port formal, else 0 if not found.  
 */
size_t
port_formals_manager::lookup_port_formal_position(const string& id) const {
	const port_formals_value_type
		probe(lookup_port_formal(id));
	if (probe) {
		const port_formals_list_type::const_iterator
			ports_begin(port_formals_list.begin());
		return 1 +distance(ports_begin, 
			std::find(ports_begin, port_formals_list.end(), probe));
	} else {
		return 0;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Validates a list of objects (instance references) against
	the port formal specification.  
	\param ol actual instance references.
	\return true if type checks (is conservative).
 */
good_bool
port_formals_manager::certify_port_actuals(const checked_refs_type& ol) const {
	typedef	checked_refs_type	refs_list_type;
#if IMPLICIT_SUPPLY_PORTS
	const size_t num_formals = __explicit_ports;
#else
	const size_t num_formals = port_formals_list.size();
#endif
	const size_t num_actuals = ol.size();
	if (num_formals != ol.size()) {
		cerr << "Number of port actuals (" << num_actuals <<
			") doesn\'t match the number of port formals (" <<
			num_formals << ").  ERROR!  " << endl;
		return good_bool(false);
	}
	refs_list_type::const_iterator
		a_iter(ol.begin());
	port_formals_list_type::const_iterator
		f_iter(port_formals_list.begin());
	const port_formals_list_type::const_iterator
		f_end(port_formals_list.end());
#if IMPLICIT_SUPPLY_PORTS
	// skip implicit ports, as they are never passed in ol
	while ((f_iter != f_end) &&
		(*f_iter)->get_name().find('!') != string::npos) {
		++f_iter;
	}
#endif
	size_t i = 1;
	for ( ; f_iter!=f_end; f_iter++, a_iter++, i++) {
		const count_ptr<const meta_instance_reference_base> a_iref(*a_iter);
		if (a_iref) {
			const port_formals_value_type f_inst(*f_iter);
			const count_ptr<const meta_instance_reference_base>
				f_iref(f_inst->make_meta_instance_reference());
			if (!f_iref->may_be_type_equivalent(*a_iref)) {
				cerr << "ERROR: actual instance reference "
					<< i << " of port connection (";
				a_iref->dump(cerr,
					expr_dump_context::default_value);
				cerr <<	") doesn\'t match the formal (";
				f_iref->dump(cerr,
					expr_dump_context::default_value);
				cerr << ") type/size." << endl << "\tgot: ";
				a_iref->dump_type_size(cerr);
				f_iref->dump_type_size(
					cerr << ", expected: ") << endl;
				return good_bool(false);
			}
			// else continue checking
		}
		// else is NULL, no connection to check, just continue
	}
	// if we've made it here, then no errors!
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a port formal instance to this process definition.  
	This applies to both implicit ports (named with '!') and
	normal ports.  
	invariant: all global ports must appear together at the front 
		of the list!
 */
void
port_formals_manager::add_port_formal(const port_formals_value_type pf) {
	// since we already checked used_id_map, there cannot be a repeat
	// in the port_formals_list!
	const string& s(pf->get_name());
#if IMPLICIT_SUPPLY_PORTS
	if (s.find('!') == string::npos) {
		++__explicit_ports;
	} else {
		++__implicit_ports;
		INVARIANT(!__explicit_ports);
	}
#endif
	port_formals_list.push_back(pf);
	port_formals_map[s] = pf;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Port formals are equivalent if their order of instantiations
	matches exactly, type, size, and even name.  
 */
bool
port_formals_manager::equivalent_port_formals(
		const port_formals_manager& p) const {
	// shortcut reference
	const port_formals_list_type& pports(p.port_formals_list);
	// NOTE: this counts total parameters, not just the explicit ones (OK)
	if (port_formals_list.size() != pports.size()) {
		cerr << "ERROR: number of port formal parameters "
			"doesn\'t match!" << endl;
		return false;
	}
	port_formals_list_type::const_iterator i(port_formals_list.begin());
	port_formals_list_type::const_iterator j(pports.begin());
	for ( ; i!=port_formals_list.end() && j!=pports.end(); i++, j++) {
		const port_formals_value_type ipf(*i), jpf(*j);
		NEVER_NULL(ipf);
		NEVER_NULL(jpf);
		if (!ipf->port_formal_equivalent(*jpf)) {
			// descriptive error message, please
			cerr << "ERROR: port formals do not match!" << endl;
			return false;
		}
	}
	// sanity size check
	INVARIANT(i == port_formals_list.end() && j == pports.end());
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unpacks each port formal into port-actual placeholders
	in the subinstance_manager's port list.  
	TODO: in instance_collection implementations, 
		use packed array representation for port formals collections.  
 */
good_bool
port_formals_manager::unroll_ports(const unroll_context& c, 
		vector<never_ptr<physical_instance_collection> >& sub) const {
	STACKTRACE_VERBOSE;
	INVARIANT(sub.empty());
	sub.reserve(size());		// pre-allocate
	// applies to both explicit and implicit ports
	port_formals_list_type::const_iterator i(port_formals_list.begin());
	const port_formals_list_type::const_iterator e(port_formals_list.end());
	for ( ; i!=e; i++) {
		const port_formals_value_type pcb(*i);
		NEVER_NULL(pcb);
		// supposed to return a new copy of instance-collection
		const never_ptr<physical_instance_collection>
			new_port(pcb->unroll_port_only(c));
		// can throw
		if (!new_port) {
			// there was an error unrolling the port
			return good_bool(false);
		}
#if ENABLE_STACKTRACE
		new_port->dump(cerr << "new port: ") << endl;
#endif
		sub.push_back(new_port);
	}
	// relink somewhere?
#if ENABLE_STACKTRACE
	cerr << "Just unrolled " << sub.size() << " port instances." << endl;
#endif
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively collects reachable pointers and register them
	with the persistent object manager.  
 */
void
port_formals_manager::collect_transient_info_base(
		persistent_object_manager&) const {
	// no need to visit port formals, port formals, separately, 
	// b/c they're all registered in the used_id_map.  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Port formals will need to be in list order.
	Just write out the list, the map is redundant.  
 */
void
port_formals_manager::write_object_base(
		const persistent_object_manager& m, ostream& f) const {
	INVARIANT(port_formals_list.size() == port_formals_map.size());
	m.write_pointer_list(f, port_formals_list);
	// the map is redundant
	// port counts are cached and regenerated
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Port formals are loaded in list order.
	Remember that the redundant map also needs to be reconstructed.  
	Another method will add the entries to the corresponding
	used_id_map where appropriate.  
 */
void
port_formals_manager::load_object_base(
		const persistent_object_manager& m, istream& f) {
	port_formals_list_type temp;
	m.read_pointer_list(f, temp);
	port_formals_list_type::const_iterator
		i(temp.begin());
	const port_formals_list_type::const_iterator
		e(temp.end());
	for ( ; i!=e; ++i) {
		const port_formals_value_type inst_ptr(*i);
		NEVER_NULL(inst_ptr);
		m.load_object_once(const_cast<instance_placeholder_type*>(
			&*inst_ptr));
		add_port_formal(inst_ptr);	// updates cached port counts
		// port_formals_map[inst_ptr->get_name()] = inst_ptr;
	}
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

