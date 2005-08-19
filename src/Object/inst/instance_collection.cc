/**
	\file "Object/inst/instance_collection.cc"
	Method definitions for instance collection classes.
	This file was originally "Object/art_object_instance.cc"
		in a previous (long) life.  
 	$Id: instance_collection.cc,v 1.3.2.2 2005/08/19 05:17:38 fang Exp $
 */

#ifndef	__OBJECT_INST_INSTANCE_COLLECTION_CC__
#define	__OBJECT_INST_INSTANCE_COLLECTION_CC__

#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_DESTRUCTORS		0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS		0 && ENABLE_STACKTRACE


#include <iostream>
#include <algorithm>

#include "Object/def/definition_base.h"
#include "Object/def/footprint.h"
#include "Object/type/fundamental_type_reference.h"
#include "Object/inst/datatype_instance_collection.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/unroll/instantiation_statement.h"
#include "Object/expr/const_range.h"
#include "Object/expr/const_range_list.h"
#include "Object/common/namespace.h"
#include "Object/persistent_type_hash.h"
#include "Object/inst/substructure_alias_base.h"
#include "common/TODO.h"

// the following are required by use of canonical_type<>
// see also the temporary hack in datatype_instance_collection
#include "Object/def/enum_datatype_def.h"
#include "Object/expr/pint_expr.h"
#include "Object/expr/const_param.h"
#include "Object/type/canonical_type.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/alias_actuals.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/bool_instance_collection.h"
#include "Object/inst/int_instance_collection.h"
#include "Object/inst/enum_instance_collection.h"
#include "Object/inst/struct_instance_collection.h"

#include "util/STL/list.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/compose.h"
#include "util/binders.h"
#include "util/ptrs_functional.h"
#include "util/dereference.h"
#include "util/indent.h"
#include "util/stacktrace.h"


//=============================================================================
namespace ART {
namespace entity {
using namespace ADS;		// for composition functors
using util::dereference;
#include "util/using_ostream.h"
using std::mem_fun_ref;
using std::bind2nd_argval_void;
USING_STACKTRACE
using util::indent;
using util::auto_indent;
using util::write_string;
using util::read_string;

//=============================================================================
// class instance_collection_base method definitions

const never_ptr<const instance_collection_base>
instance_collection_base::null(NULL);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instantiation base constructor.  
	The first time an instance is declared, its dimensions are
	set by the array-dimension list, if provided, else 0.
	The first set of indices given will be pushed onto the 
	instance collection stack.  
	\param o the owning scope.  
	\param n the name of the instance (collection).
	\param d the number of dimensions of this collection ([0,4]).  
		WAS: initial collection of indices, already resolved 
		as param_expr's.  
 */
// inline
instance_collection_base::instance_collection_base(const scopespace& o, 
		const string& n, const size_t d) : 
		object(), owner(owner_ptr_type(&o)),
		key(n), index_collection(), dimensions(d), 
		super_instance() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
instance_collection_base::~instance_collection_base() {
	STACKTRACE_DTOR("~instance_collection_base()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Overridden by param_value_collection.  
 */
ostream&
instance_collection_base::dump(ostream& o) const {
	// but we need a version for unrolled and resolved parameters.  
	if (is_partially_unrolled()) {
		type_dump(o);		// pure virtual
	} else {
		// this dump is appropriate for pre-unrolled, unresolved dumping
		// get_type_ref just grabs the type of the first statement
		get_type_ref()->dump(o);
	}
	o << " " << key;

	if (dimensions) {
		INVARIANT(!index_collection.empty());
		o << " with indices: {" << endl;
	{	// indentation scope
		INDENT_SECTION(o);
		index_collection_type::const_iterator
			i(index_collection.begin());
		const index_collection_type::const_iterator
			e(index_collection.end());
		for ( ; i!=e; i++) {
			NEVER_NULL(*i);
			const index_collection_item_ptr_type
				ind((*i)->get_indices());
			// ind can be NULL?
			NEVER_NULL(ind);
			ind->dump(o << auto_indent) << endl;
		}
	}	// end indentation scope
		o << auto_indent << "}" << endl;
	} else {
		INVARIANT(index_collection.size() == 1);
		// the list contains exactly one instantiation statement
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
instance_collection_base::pair_dump(ostream& o) const {
	o << auto_indent << get_name() << " = ";
	return dump(o) << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
instance_collection_base::get_qualified_name(void) const {
	if (owner)
		return owner->get_qualified_name() +"::" +key;
		// "::" should be the same as ART::parser::scope
	else return key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
instance_collection_base::dump_qualified_name(ostream& o) const {
	if (owner)
		return owner->dump_qualified_name(o) << "::" << key;
	else	return o << key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
instance_collection_base::dump_hierarchical_name(ostream& o) const {
	STACKTRACE_VERBOSE;
	if (super_instance) {
		return super_instance->dump_hierarchical_name(o) << '.' << key;
	} else {
		return dump_qualified_name(o);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Return's the type's base definition.
 */
never_ptr<const definition_base>
instance_collection_base::get_base_def(void) const {
	return get_type_ref()->get_base_def();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: can't call this on formal template parameter collections, 
	because they don't have an index collection.
	TODO: subtype index collections.  
 */
count_ptr<const fundamental_type_reference>
instance_collection_base::get_type_ref(void) const {
	INVARIANT(!index_collection.empty());
	return (*index_collection.begin())->get_type_ref();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Grabs the current top of the deque of the index collection, 
	so the encapsulating instance reference know what
	instances were visible at the time of reference.  
	QUESTION: what if it's empty because it is not collective?
		will begin() = end()? should be...
 */
instantiation_state
instance_collection_base::current_collection_state(void) const {
	return index_collection.begin();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	By "end", we mean the beginning of the collection state deque, 
	the first item added to the the collection stack.  
	Can't actually dereference the returned iterator, 
	it's only useful for ending looped iterations.  
 */
instantiation_state
instance_collection_base::collection_state_end(void) const {
	return index_collection.end();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Will need two flavors: may and must?
	
	\return true if the new range *definitely* overlaps with previous
		static constant ranges.  Comparisons with dynamic ranges
		will conservatively return false; they will be resolved
		at unroll-time.  Also returns true if there was an error.  
		By "true", we mean a valid precise range of overlap.  
 */
const_range_list
instance_collection_base::detect_static_overlap(
		const index_collection_item_ptr_type& r) const {
	NEVER_NULL(r);
	INVARIANT(r->dimensions() == dimensions);
#if 0
	// DEBUG
	cerr << "In instance_collection_base::detect_static_overlap with this = "
		<< this << endl;
	r->dump(cerr << "index_collection_item_ptr_type r = ") << endl;
#endif
	if (r.is_a<const const_range_list>()) {
	index_collection_type::const_iterator i(index_collection.begin());
	for ( ; i!=index_collection.end(); i++) {
		// return upon first overlap error
		// later accumulate all overlaps.  
		const const_range_list
			ovlp((*i)->get_indices()->static_overlap(*r));
		if (!ovlp.empty()) {
			return ovlp;
		}
		// else keep checking...
	}
	// if this point reached, then return false
	} // else just return false, can't know statically without analysis
	return const_range_list();	// empty constructed list
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: this can only be done with non-formals.  Check this.  
	If this instance is a collection, add the new range of indices
	which may be sparse or dense.  

	TODO: type-check here?
	see scopespace::add_instance's definition body
	2005-07-07: need to register type of the first declaration up-front, 
		i.e. its strict parameters, and relaxed parameters if available

	This is only applicable if this instantiation was initialized
	as a collective.  
	Pre-condition: The dimensions better damn well match!  
	\param r the instantiation statement with index ranges to be added.  
	\return Overlapping range (true) if error condition. 
	\sa detect_static_overlap
 */
const_range_list
instance_collection_base::add_instantiation_statement(
		const index_collection_type::value_type& r) {
	STACKTRACE("instance_collection_base::add_instantiation_statement()");
	NEVER_NULL(r);
	const index_collection_item_ptr_type i(r->get_indices());
	INVARIANT(dimensions || index_collection.empty());	// catches 0-D
	// TYPE CHECK!!!
	const_range_list overlap;
	if (i) {
		INVARIANT(dimensions == i->dimensions());
		overlap = detect_static_overlap(i);
	} else {
		INVARIANT(!dimensions);
	}
	// can the following accept NULL?
	index_collection.push_back(r);
	return overlap;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks whether or not this is a relaxed template formal parameter.  
 */
bool
instance_collection_base::is_relaxed_template_formal(void) const {
	const never_ptr<const definition_base>
		def(owner.is_a<const definition_base>());
	if (def) {
		return def->probe_relaxed_template_formal(key);
	} else return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Queries whether or not this is a template formal, by 
	checking its membership in the owner.  
	\return 0 (false) if is not a template formal, 
		otherwise returns the position (1-indexed)
		of the instance referenced, 
		useful for determining template parameter equivalence.  
	TODO: is there potential confusion here if the key shadows
		a declaration else where?
 */
size_t
instance_collection_base::is_template_formal(void) const {
	const never_ptr<const definition_base>
		def(owner.is_a<const definition_base>());
	if (def)
		return def->lookup_template_formal_position(key);
	else {
		// owner is not a definition
		INVARIANT(owner.is_a<const name_space>());
		// is owned by a namespace, i.e. actually instantiated
		return 0;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Queries whether or not this is a port formal, by 
	checking its membership in the owner.  
	\return 1-indexed position into port list, else 0 if not found.
 */
size_t
instance_collection_base::is_port_formal(void) const {
	const never_ptr<const definition_base>
		def(owner.is_a<const definition_base>());
	return def ? def->lookup_port_formal_position(*this) : 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Whether or not this instance is a reference to a collection
	local to a definition, else is a top-level (global).
 */
bool
instance_collection_base::is_local_to_definition(void) const {
	return owner.is_a<const definition_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Much like equivalence for template formals, except that
	names also need to match for port formals.  
	Rationale: need to be able to refer to the public ports
	of a prototype, which must correspond to those of the definition, 
	and vice versa.  
 */
bool
instance_collection_base::port_formal_equivalent(const this_type& b) const {
	// first make sure base types are equivalent.  
	const count_ptr<const fundamental_type_reference>
		t_type(get_type_ref());
	const count_ptr<const fundamental_type_reference>
		b_type(b.get_type_ref());
	if (!t_type->may_be_connectibly_type_equivalent(*b_type)) {
		// then their instantiation types differ
		return false;
	}
	// then compare sizes and dimensionality
	if (!formal_size_equivalent(b))
		return false;
	// last, but not least, name must match
	return key == b.get_name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just compares dimensionality and sizes of an instantiation
	in a template formal context.  
	This applies to both template formals and port formals.  
	Is conservative, not precise, in the case where one of the
	parameter sizes (dimension) is dynamic.  
	\param b the other template formal instantiation to compare against.  
	\return true if dimensionality and sizes are equal.  
 */
bool
instance_collection_base::formal_size_equivalent(const this_type& b) const {
	if (dimensions != b.dimensions) {
		// useful error message here: dimensions don't match
		return false;
	}
	// formal instances can only be declared once, i.e. 
	// can't add instances to their collection.
	// and they must be dense arrays.  
	const size_t this_coll = index_collection.size();
	const size_t b_coll = b.index_collection.size();
	INVARIANT(this_coll <= 1);
	INVARIANT(b_coll <= 1);
	if (this_coll != b_coll) {
		// one is scalar, the other is array
		return false;
	}
	if (this_coll == 1) {
		// compare their collections
		const index_collection_type::const_iterator
			i(index_collection.begin());
		const index_collection_type::const_iterator
			j(b.index_collection.begin());
		// difficult: what if some dimensions are not static?
		// depends on some other former parameter?
		// This is when it would help to walk the 
		// former template formals list when visited with the second.  

		// NEW (2005-01-30):
		// For template, need notion of positional parameter 
		// equivalence -- expressions referring to earlier
		// formal parameters.  
		// is count_ptr<meta_range_list>
		const index_collection_item_ptr_type ii((*i)->get_indices());
		const index_collection_item_ptr_type ji((*j)->get_indices());
		if (ii && ji) {
			return ii->must_be_formal_size_equivalent(*ji);
		} else 	return (!ii && !ji);
			// both NULL is ok too
	} else {
		// both are scalar, single instances
		return true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Upward recursive: find top-most super-instance and allocate 
	state from there top-down.  
 */
good_bool
instance_collection_base::create_super_instance(footprint& f) {
	// super-instance corresponds to a substructure alias
	// some traversal similar to dump_hierarchical_name.
	INVARIANT(super_instance);
	return good_bool(super_instance->allocate_state(f) != 0);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Utility function for walking index collection list
	and collecting pointers.  
 */
inline
void
instance_collection_base::collect_index_collection_pointers(
		persistent_object_manager& m) const {
//	STACKTRACE_PERSISTENT("instance_collection_base::collect_index_collection_pointers()");
#if 0
	// keep this around for debugging, does same thing, but readable in gdb
	index_collection_type::const_iterator i(index_collection.begin());
	const index_collection_type::const_iterator e(index_collection.end());
	for ( ; i!=e; i++) {
		STACKTRACE_PERSISTENT("for all index_collection:");
		NEVER_NULL(*i);
#if 0
		(*i)->what(STACKTRACE_STREAM << "at " << &**i << ", ") << endl;
#endif
		(*i)->collect_transient_info(m);
	}
#else
	for_each(index_collection.begin(), index_collection.end(), 
	unary_compose_void(
		bind2nd_argval_void(mem_fun_ref(
			&instance_management_base::collect_transient_info), m), 
		dereference<never_ptr<const instance_management_base> >()
	)
	);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instance_collection_base::collect_transient_info_base(
		persistent_object_manager& m) const {
//	STACKTRACE_PERSISTENT("instance_collection_base::collect_transient_info_base()");
	collect_index_collection_pointers(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Write out serial list of pointers to index collection items, 
	with pointers translated into indicies.  
	Does NOT save the dimensions -- it is set upon reconstruction.
 */
inline
void
instance_collection_base::write_index_collection_pointers(
		const persistent_object_manager& m, ostream& o) const {
	STACKTRACE_PERSISTENT("inst_coll_base::write_index_collection_pointers()");
	m.write_pointer(o, owner);
	write_string(o, key);
	m.write_pointer_list(o, index_collection);
		// is actually specialized for count_ptr's :)
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instance_collection_base::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	write_index_collection_pointers(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Loads serial list of pointers into index collection items, 
	with indices translated into pointers.  
	Does NOT restore dimensions -- it is set at reconstruction.  
 */
inline
void
instance_collection_base::load_index_collection_pointers(
		const persistent_object_manager& m, istream& i) {
	STACKTRACE_PERSISTENT("inst_coll_base::load_index_collection_pointers()");
	m.read_pointer_list(i, index_collection);
		// is actually specialized for count_ptr's :)
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instance_collection_base::load_object_base(
		const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, owner);
	read_string(i, const_cast<string&>(key));
	load_index_collection_pointers(m, i);
}

//=============================================================================
// class physical_instance_collection method definitions

physical_instance_collection::physical_instance_collection(
		const scopespace& o, const string& n, const size_t d) :
		parent_type(o, n, d) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
physical_instance_collection::~physical_instance_collection() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
physical_instance_collection::dump(ostream& o) const {
	parent_type::dump(o);
	if (is_partially_unrolled()) {
		if (dimensions) {
			INDENT_SECTION(o);
			o << auto_indent << "unrolled indices: {" << endl;
			{
				// INDENT_SECTION macro not making unique IDs
				INDENT_SECTION(o);
				dump_unrolled_instances(o);
			}
			o << auto_indent << "}";        // << endl;
		} else {
			// else nothing to say, just one scalar instance
			dump_unrolled_instances(o << " (instantiated)");
		}
	}
	return o;
}

//=============================================================================
// class datatype_instance_collection method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Private empty constructor.
 */
datatype_instance_collection::datatype_instance_collection() :
		parent_type() {
	// no assert
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
datatype_instance_collection::datatype_instance_collection(
		const scopespace& o, const string& n, const size_t d) :
		parent_type(o, n, d) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
datatype_instance_collection::~datatype_instance_collection() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default action for request for resolved param list.  
	This is appropriate for collection types that have no 
	template parameters.  
	\return null list of parameters.  
 */
never_ptr<const const_param_expr_list>
datatype_instance_collection::get_actual_param_list(void) const {
	return never_ptr<const const_param_expr_list>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Temporary ugly hack.  :(
	TODO: implement for real.  
	REMARK: want to use a virtual function, but can't because
		children types expect different argument types!
	Possible to fake it...?
 */
void
datatype_instance_collection::establish_collection_type(
		const instance_collection_parameter_type& p) {
{
	bool_instance_collection* const
		b(IS_A(bool_instance_collection*, this));
	if (b) {
		b->establish_collection_type(
			bool_instance_collection::instance_collection_parameter_type());
		return;
	}
}{
	int_instance_collection* const
		i(IS_A(int_instance_collection*, this));
	if (i) {
		const canonical_type_base::const_param_list_ptr_type&
			pp(p.get_raw_template_params());
		NEVER_NULL(pp);
		INVARIANT(pp->size() == 1);
		const int_instance_collection::instance_collection_parameter_type
			w = IS_A(const pint_expr&, *pp->front())
				.static_constant_value();
		i->establish_collection_type(w);
		return;
	}
}{
	enum_instance_collection* const
		e(IS_A(enum_instance_collection*, this));
	if (e) {
		const enum_instance_collection::instance_collection_parameter_type
			d = p.get_base_def().is_a<const enum_datatype_def>();
		e->establish_collection_type(d);
		return;
	}
}
	// TODO: user-def-structs
	FINISH_ME(Fang);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bad_bool
datatype_instance_collection::check_established_type(
		const instance_collection_parameter_type& p) const {
{
	const bool_instance_collection* const
		b(IS_A(const bool_instance_collection*, this));
	if (b) {
		return b->check_established_type(
			bool_instance_collection::instance_collection_parameter_type());
	}
}{
	const int_instance_collection* const
		i(IS_A(const int_instance_collection*, this));
	if (i) {
		const canonical_type_base::const_param_list_ptr_type&
			pp(p.get_raw_template_params());
		NEVER_NULL(pp);
		INVARIANT(pp->size() == 1);
		const int_instance_collection::instance_collection_parameter_type
			w = IS_A(const pint_expr&, *pp->front())
				.static_constant_value();
		return i->check_established_type(w);
	}
}{
	const enum_instance_collection* const
		e(IS_A(const enum_instance_collection*, this));
	if (e) {
		const enum_instance_collection::instance_collection_parameter_type
			d = p.get_base_def().is_a<const enum_datatype_def>();
		return e->check_established_type(d);
	}
}
	// TODO: user-def-structs
	FINISH_ME(Fang);
	return bad_bool(true);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_INSTANCE_COLLECTION_CC__

