/**
	\file "art_object_instance.cc"
	Method definitions for instance collection classes.
 	$Id: art_object_instance.cc,v 1.39.2.7 2005/02/17 19:45:18 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_CC__
#define	__ART_OBJECT_INSTANCE_CC__

#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_DESTRUCTORS		0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS		0 && ENABLE_STACKTRACE


#include <iostream>
#include <algorithm>

#include "art_object_definition.h"
#include "art_object_type_ref.h"
#include "art_object_instance.h"
#include "art_object_instance_param.h"
#include "art_object_inst_ref.h"
#include "art_object_inst_stmt.h"
#include "art_object_expr_const.h"
#include "art_built_ins.h"
#include "art_object_type_hash.h"

#include "STL/list.tcc"
#include "persistent_object_manager.tcc"
#include "compose.h"
#include "binders.h"
#include "ptrs_functional.h"
#include "dereference.h"
#include "indent.h"
#include "stacktrace.h"

// conditional defines, after including "stacktrace.h"
#if STACKTRACE_DESTRUCTORS
	#define	STACKTRACE_DTOR(x)		STACKTRACE(x)
#else
	#define	STACKTRACE_DTOR(x)
#endif

#if STACKTRACE_PERSISTENTS
	#define	STACKTRACE_PERSISTENT(x)	STACKTRACE(x)
#else
	#define	STACKTRACE_PERSISTENT(x)
#endif


//=============================================================================
namespace ART {
namespace entity {
using namespace ADS;		// for composition functors
using util::dereference;
#include "using_ostream.h"
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
#if 0
/**
	Private empty constructor, but with dimension established.
 */
instance_collection_base::instance_collection_base() :
		object(), persistent(), 
		owner(NULL), key(), index_collection()
#if 0
		, depth(0)
#endif
		{
}
#endif

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
		key(n), index_collection(), dimensions(d) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
instance_collection_base::~instance_collection_base() {
	STACKTRACE_DTOR("~instance_collection_base()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Overridden by param_instance_collection.  
 */
ostream&
instance_collection_base::dump(ostream& o) const {
#if 0
	get_type_ref()->dump(o);
#else
	// but we need a version for unrolled and resolved parameters.  
	if (is_partially_unrolled()) {
		type_dump(o);		// pure virtual
	} else {
		// this dump is appropriate for pre-unrolled, unresolved dumping
		// get_type_ref just grabs the type of the first statement
		get_type_ref()->dump(o);
	}
#endif
	o << " " << key;

	if (dimensions) {
		INVARIANT(!index_collection.empty());
		o << " with indices: {" << endl;
	{	// indentation scope
		indent indenter(o);
		index_collection_type::const_iterator
			i = index_collection.begin();
		const index_collection_type::const_iterator
			e = index_collection.end();
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
/**
	Return's the type's base definition.
 */
never_ptr<const definition_base>
instance_collection_base::get_base_def(void) const {
	return get_type_ref()->get_base_def();
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
	index_collection_type::const_iterator i = index_collection.begin();
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
	TO DO: this can only be done with non-formals.  Check this.  
	If this instance is a collection, add the new range of indices
	which may be sparse or dense.  

	TO DO: type-check here?
	see scopespace::add_instance's definition body

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
	Queries whether or not this is a template formal, by 
	checking its membership in the owner.  
	\return 0 (false) if is not a template formal, 
		otherwise returns the position (1-indexed)
		of the instance referenced, 
		useful for determining template parameter equivalence.  
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
 */
bool
instance_collection_base::is_port_formal(void) const {
	const never_ptr<const definition_base>
		def(owner.is_a<const definition_base>());
	if (def)
		return def->lookup_port_formal(key);
	else return false;		// owner is not a definition
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For two template formals to be equivalent, their
	type and size must match, names need not.  
	Currently allows comparison of parameter and non-parameter
	formal types.  
	Is conservative because parameters (in sizes) may be dynamic, 
	or collective.  
 */
bool
instance_collection_base::template_formal_equivalent(
		const never_ptr<const instance_collection_base> b) const {
	NEVER_NULL(b);
	// first make sure base types are equivalent.  
	const count_ptr<const fundamental_type_reference>
		this_type(get_type_ref());
	const count_ptr<const fundamental_type_reference>
		b_type(b->get_type_ref());
	// used to be may_be_equivalent...
	if (!this_type->must_be_equivalent(*b_type)) {
		// then their instantiation types differ
		return false;
	}
	// then compare sizes and dimensionality
	return formal_size_equivalent(b);
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
instance_collection_base::port_formal_equivalent(
		const never_ptr<const instance_collection_base> b) const {
	NEVER_NULL(b);
	// first make sure base types are equivalent.  
	const count_ptr<const fundamental_type_reference>
		this_type(get_type_ref());
	const count_ptr<const fundamental_type_reference>
		b_type(b->get_type_ref());
	if (!this_type->may_be_equivalent(*b_type)) {
		// then their instantiation types differ
		return false;
	}
	// then compare sizes and dimensionality
	if (!formal_size_equivalent(b))
		return false;
	// last, but not least, name must match
	return key == b->get_name();
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
instance_collection_base::formal_size_equivalent(
		const never_ptr<const instance_collection_base> b) const {
	NEVER_NULL(b);
	if (dimensions != b->dimensions) {
		// useful error message here: dimensions don't match
		return false;
	}
	// formal instances can only be declared once, i.e. 
	// can't add instances to their collection.
	// and they must be dense arrays.  
	const size_t this_coll = index_collection.size();
	const size_t b_coll = b->index_collection.size();
	INVARIANT(this_coll <= 1);
	INVARIANT(b_coll <= 1);
	if (this_coll != b_coll) {
		// one is scalar, the other is array
		return false;
	}
	if (this_coll == 1) {
		// compare their collections
		const index_collection_type::const_iterator
			i = index_collection.begin();
		const index_collection_type::const_iterator
			j = b->index_collection.begin();
		// difficult: what if some dimensions are not static?
		// depends on some other former parameter?
		// This is when it would help to walk the 
		// former template formals list when visited with the second.  

		// NEW (2005-01-30):
		// For template, need notion of positional parameter 
		// equivalence -- expressions referring to earlier
		// formal parameters.  
#if 0
		const count_ptr<const const_range_list>
			ic((*i)->get_indices().is_a<const const_range_list>());
		const count_ptr<const const_range_list>
			jc((*j)->get_indices().is_a<const const_range_list>());
		if (ic && jc) {
			// compare dense ranges in each dimension
			// must be equal!
			return (*ic == *jc);
		} else {
			// one of them is dynamic, thus we must conservatively
			return true;
		}
#else
		// is count_ptr<range_expr_list>
		const index_collection_item_ptr_type ii = (*i)->get_indices();
		const index_collection_item_ptr_type ji = (*j)->get_indices();
		if (ii && ji) {
			return ii->must_be_formal_size_equivalent(*ji);
		} else 	return (!ii && !ji);
			// both NULL is ok too
#endif
	} else {
		// both are scalar, single instances
		return true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks for dimension and size equality between expression and 
	instantiation.  
	So far, only used by param_instance_collection derivatives, 
		in the context of checking template formals.  
	May be useful else where for connections.  
	\return true if dimensions *may* match.  
 */
bool
instance_collection_base::check_expression_dimensions(
		const param_expr& pe) const {
	MUST_BE_A(const param_instance_collection*, this);
	// else is not an expression class!

	// dimensions() used to be a pure virtual method
	// problem when dimensions() is called during construction:
	// error: pure virtual method called (during construction)
	// this occurs during static construction of the global 
	// built in definition object: ind_def, which is templated
	// with int width.  
	// Solutions: 
	// 1) make an unsafe/unchecked constructor for this special case.
	// 2) add the template parameter after contruction is complete, 
	//	which is safe as long as no other global (outside of
	//	art_built_ins.cc) depends on it.
	// we choose 2 because it is a general solution.  

	if (dimensions != pe.dimensions()) {
		// number of dimensions doesn't even match!
		// useful error message?
		return false;
	}
	// dimensions match
	if (dimensions != 0) {
		INVARIANT(index_collection.size() == 1);	// huh? true?
		// make sure sizes in each dimension
		index_collection_type::const_iterator i =
			index_collection.begin();
		const count_ptr<const const_range_list>
			crl((*i)->get_indices().is_a<const const_range_list>());
		if (crl) {
			if (pe.has_static_constant_dimensions()) {
				const const_range_list
					d(pe.static_constant_dimensions());
				return (*crl == d);
			} else {
				// is dynamic, conservatively return true
				return true;
			}
		} else {
			// is dynamic, conservatively return true
			return true;
		}
	} else {
		// dimensions == 0 means instantiation is a single instance.  
		// size may be zero b/c first statement hasn't been added yet
		INVARIANT(index_collection.size() <= 1);
		return (pe.dimensions() == 0);
	}
}

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
	index_collection_type::const_iterator i = index_collection.begin();
	const index_collection_type::const_iterator e = index_collection.end();
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
// class datatype_instance_collection method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Private empty constructor.
 */
datatype_instance_collection::datatype_instance_collection() :
		instance_collection_base() {
	// no assert
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
datatype_instance_collection::datatype_instance_collection(
		const scopespace& o, const string& n, const size_t d) :
		instance_collection_base(o, n, d) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
datatype_instance_collection::~datatype_instance_collection() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
datatype_instance_collection::dump(ostream& o) const {
	parent_type::dump(o);
	if (is_partially_unrolled()) {
		if (dimensions) {
			indent indenter(o);
			o << auto_indent << "unrolled indices: {" << endl;
			{
				indent indenter(o);
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
datatype_instance_collection::get_type_ref(void) const {
	INVARIANT(!index_collection.empty());
	return (*index_collection.begin())->get_type_ref();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a member reference to a datatype, 
	and pushes it onto the context's object_stack.  
	\param b is the parent owner of this instantiation referenced.  
 */
count_ptr<member_instance_reference_base>
datatype_instance_collection::make_member_instance_reference(
		const count_ptr<const simple_instance_reference>& b) const {
	NEVER_NULL(b);
	// maybe verify that b contains this, as sanity check
	return count_ptr<datatype_member_instance_reference>(
		new datatype_member_instance_reference(b,
			never_ptr<const datatype_instance_collection>(this)));
		// omitting index argument, set it later...
		// done by parser::instance_array::check_build()
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

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INSTANCE_CC__

