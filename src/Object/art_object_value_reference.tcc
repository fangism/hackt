/**
	\file "art_object_expr.cc"
	Class method definitions for semantic expression.  
 	$Id: art_object_value_reference.tcc,v 1.1.6.1 2005/03/11 01:16:21 fang Exp $
 */

#ifndef	__ART_OBJECT_VALUE_REFERENCE_TCC__
#define	__ART_OBJECT_VALUE_REFERENCE_TCC__

// flags for controlling conditional compilation, mostly for debugging
#ifndef	DEBUG_LIST_VECTOR_POOL
#define	DEBUG_LIST_VECTOR_POOL				0
#endif

#ifndef	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0
#endif

#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE				0
#endif

#ifndef	STACKTRACE_DESTRUCTORS
#define	STACKTRACE_DESTRUCTORS				0 && ENABLE_STACKTRACE
#endif

#ifndef	STACKTRACE_PERSISTENTS
#define	STACKTRACE_PERSISTENTS				0 && ENABLE_STACKTRACE
#endif

#include <exception>
#include <algorithm>

// consider: (for reducing expression storage overhead)
// #define NO_OBJECT_SANITY	1
// this will override the definition in "art_object_base.h"

#include "art_object_value_reference.h"
#include "art_object_classification_details.h"

// experimental: suppressing automatic instantiation of template code
#include "art_object_extern_templates.h"

#if 0
#include "what.tcc"
#include "STL/list.tcc"
#include "qmap.tcc"
#include "stacktrace.h"
#include "static_trace.h"
#include "memory/list_vector_pool.tcc"
#include "persistent_object_manager.tcc"
// #include "memory/pointer_classes.h"
#include "sstream.h"			// for ostringstring, used by dump
#include "discrete_interval_set.tcc"
#include "compose.h"
#include "conditional.h"		// for compare_if
#include "ptrs_functional.h"
#include "dereference.h"
#endif

// these conditional definitions must appear after inclusion of "stacktrace.h"
#ifndef	STACKTRACE_DTOR
#if STACKTRACE_DESTRUCTORS
	#define	STACKTRACE_DTOR(x)		STACKTRACE(x)
#else
	#define	STACKTRACE_DTOR(x)
#endif
#endif

#ifndef	STACKTRACE_PERSISTENT
#if STACKTRACE_PERSISTENTS
	#define	STACKTRACE_PERSISTENT(x)	STACKTRACE(x)
#else
	#define	STACKTRACE_PERSISTENT(x)
#endif
#endif

//=============================================================================
namespace ART {
namespace entity {
//=============================================================================
// #include "using_ostream.h"
using namespace util::memory;
#if 0
USING_UTIL_COMPOSE
USING_UTIL_OPERATIONS
using util::discrete_interval_set;
using std::_Select1st;
using std::_Select2nd;
using std::mem_fun_ref;
using util::dereference;
using std::ostringstream;
using util::write_value;
using util::read_value;
using util::write_string;
using util::read_string;
#endif
USING_STACKTRACE
using util::persistent_traits;

#if DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE && ENABLE_STACKTRACE
REQUIRES_STACKTRACE_STATIC_INIT
// the robust list_vector_pool requires this.  
#endif

//=============================================================================
// class value_reference method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.  
 */
VALUE_REFERENCE_TEMPLATE_SIGNATURE
VALUE_REFERENCE_CLASS::value_reference() :
		parent_type(), interface_type(), value_collection_ref(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_REFERENCE_TEMPLATE_SIGNATURE
VALUE_REFERENCE_CLASS::value_reference(
		const never_ptr<value_collection_type> pi) :
		parent_type(pi->current_collection_state()),
		interface_type(), 
		value_collection_ref(pi) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
VALUE_REFERENCE_TEMPLATE_SIGNATURE
VALUE_REFERENCE_CLASS::value_reference(
		const never_ptr<value_collection_type> pi,
		excl_ptr<index_list>& i) :
		parent_type(i, pi->current_collection_state()),
		interface_type(), 
		value_collection_ref(pi) {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default destructor.
 */
VALUE_REFERENCE_TEMPLATE_SIGNATURE
VALUE_REFERENCE_CLASS::~value_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_REFERENCE_TEMPLATE_SIGNATURE
never_ptr<const instance_collection_base>
VALUE_REFERENCE_CLASS::get_inst_base(void) const {
	return value_collection_ref;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_REFERENCE_TEMPLATE_SIGNATURE
never_ptr<const typename VALUE_REFERENCE_CLASS::value_collection_parent_type>
VALUE_REFERENCE_CLASS::get_param_inst_base(void) const {
	return value_collection_ref;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
VALUE_REFERENCE_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
VALUE_REFERENCE_CLASS::dump(ostream& o) const {
	return grandparent_type::dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_REFERENCE_TEMPLATE_SIGNATURE
string
VALUE_REFERENCE_CLASS::hash_string(void) const {
	return grandparent_type::hash_string();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_REFERENCE_TEMPLATE_SIGNATURE
size_t
VALUE_REFERENCE_CLASS::dimensions(void) const {
	return grandparent_type::dimensions();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
VALUE_REFERENCE_CLASS::has_static_constant_dimensions(void) const {
	return grandparent_type::has_static_constant_dimensions();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_REFERENCE_TEMPLATE_SIGNATURE
const_range_list
VALUE_REFERENCE_CLASS::static_constant_dimensions(void) const {
	return grandparent_type::static_constant_dimensions();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	IMPORTANT: This initialization is only for static analysis and is
	not the actual initialization that takes place during unrolling.  
	\return true if sucessfully initialized with valid expression.  
 */
VALUE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
VALUE_REFERENCE_CLASS::initialize(const init_arg_type& i) {
	return this->value_collection_ref->initialize(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
VALUE_REFERENCE_CLASS::may_be_initialized(void) const {
	return parent_type::may_be_initialized();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
VALUE_REFERENCE_CLASS::must_be_initialized(void) const {
	return parent_type::must_be_initialized();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
VALUE_REFERENCE_CLASS::is_static_constant(void) const {
	return parent_type::is_static_constant();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
VALUE_REFERENCE_CLASS::is_loop_independent(void) const {
	return parent_type::is_loop_independent();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
VALUE_REFERENCE_CLASS::is_unconditional(void) const {
	return parent_type::is_unconditional();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Better make sure that this is_static_constant before calling, 
	else will assert-fail.
 */
VALUE_REFERENCE_TEMPLATE_SIGNATURE
typename VALUE_REFERENCE_CLASS::value_type
VALUE_REFERENCE_CLASS::static_constant_value(void) const {
	INVARIANT(is_static_constant());
	return value_collection_ref->initial_value()->static_constant_value();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	If both this and argument are instance references, 
	we consider them equvalent if they reference the same position
	parameter in the template formals list.  
	This allows us to correctly compare the equivalence of 
	template signatures whose member depend on template parameters.  
	\return true if boolean instance references are equivalent.  
 */
VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
VALUE_REFERENCE_CLASS::must_be_equivalent(const expr_base_type& b) const {
	const this_type* const br = IS_A(const this_type*, &b);
	if (br) {
		// compare template formal parameter positions for equivalence!
		// INVARIANT (2005-01-30): if they are both template formals, 
		// then they refer to equivalent owners.  
		// This will not be true if the language allows nested 
		// templates, so beware in the distant future!

		// check owner pointer equivalence? not pointer equality!
		// same qualified name, namespace path...
		const size_t lpos =
			value_collection_ref->is_template_formal();
		const size_t rpos =
			br->value_collection_ref->is_template_formal();
		if (lpos && rpos && (lpos == rpos)) {
			if (array_indices && br->array_indices) {
				return array_indices->
					must_be_equivalent_indices(
						*br->array_indices);
			} else {
				return true;
			}
		} else {
			return false;
		}
	} else {
		// conservatively
		return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This version specifically asks for one integer value, 
	thus the array indices must be scalar (0-D).  
	This code is grossly replicated... damn copy-paste...
	\return true if resolution succeeds, else false.
 */
VALUE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
VALUE_REFERENCE_CLASS::unroll_resolve_value(
		const unroll_context& c, value_type& i) const {
	// lookup pbool_instance_collection
	if (array_indices) {
		const const_index_list
			indices(array_indices->unroll_resolve(c));
		if (!indices.empty()) {
			const multikey_index_type
				lower(indices.lower_multikey());
			const multikey_index_type
				upper(indices.upper_multikey());
			if (lower != upper) {
				cerr << "ERROR: upper != lower" << endl;
				return good_bool(false);
			}
			return value_collection_ref->lookup_value(i, lower);
		} else {
			cerr << "Unable to unroll-resolve array_indices!" << endl;
			return good_bool(false);
		}
	} else {
		const never_ptr<value_scalar_type>
			scalar_inst(value_collection_ref.template is_a<value_scalar_type>());
		NEVER_NULL(scalar_inst);
		return scalar_inst->lookup_value(i);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This version specifically asks for one integer value, 
	thus the array indices must be scalar (0-D).  
	\return true if resolution succeeds, else false.
 */
VALUE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
VALUE_REFERENCE_CLASS::resolve_value(value_type& i) const {
	// lookup pbool_instance_collection
	if (array_indices) {
		const const_index_list
			indices(array_indices->resolve_index_list());
		if (!indices.empty()) {
			const multikey_index_type
				lower(indices.lower_multikey());
			const multikey_index_type
				upper(indices.upper_multikey());
			if (lower != upper) {
				cerr << "ERROR: upper != lower" << endl;
				return good_bool(false);
			}
			return value_collection_ref->lookup_value(i, lower);
		} else {
			cerr << "Unable to resolve array_indices!" << endl;
			return good_bool(false);
		}
	} else {
		const never_ptr<value_scalar_type>
			scalar_inst(value_collection_ref.template is_a<value_scalar_type>());
		NEVER_NULL(scalar_inst);
		return scalar_inst->lookup_value(i);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre This is called only if is an indexed (implicit or explicit)
		instance reference, and under no circumstances
		should this be invoked for scalars for which 
		resolve_dimensions() always returns an empty list.  
	\param l the list in which to accumulate values.
	\return false if there was error.  
 */
VALUE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
VALUE_REFERENCE_CLASS::resolve_values_into_flat_list(
		list<value_type>& l) const {
	// base collection must be non-scalar
	INVARIANT(value_collection_ref->get_dimensions());
	const const_index_list
		ranges(resolve_dimensions());
	if (ranges.empty()) {
		cerr << "ERROR: could not unroll " <<
			class_traits<Tag>::tag_name <<
			"values with bad index." << endl;
		return good_bool(false);
	}
	else	return value_collection_ref->lookup_value_collection(
			l, const_range_list(ranges));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns the dimensions of the collection in the current state, 
	ONLY IF, the indexed reference to the current state is all valid.  
	Otherwise, returns an empty list, which is interpreted as an error.  

	Really this should be independent of type?
	Except for checking implicit indices...
 */
VALUE_REFERENCE_TEMPLATE_SIGNATURE
const_index_list
VALUE_REFERENCE_CLASS::resolve_dimensions(void) const {
	// criterion 1: indices (if any) must be resolved to constant values.  
	if (array_indices) {
		const const_index_list
			c_i(array_indices->resolve_index_list());
		if (c_i.empty()) {
			cerr << "ERROR: failed to resolve index list." << endl;
			return c_i;
		}
		// else let c_i remain empty, underspecified
		// check for implicit indices, that sub-arrays are
		// densely packed with the same dimensions.  
		const const_index_list
			r_i(value_collection_ref->resolve_indices(c_i));
		if (r_i.empty()) {
			cerr << "ERROR: implicitly resolving index list."
				<< endl;
		}
		return r_i;
		// Elsewhere (during assign) check for initialization.
	}
	else return const_index_list();
	// Elsewhere (during assign) check for initialization.  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resolves a scalar or collective instance reference into a 
	packed array of values.  
	\param c unrolling context.
	\return dense array of values, NULL if error.  
 */
VALUE_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<const_param>
VALUE_REFERENCE_CLASS::unroll_resolve(const unroll_context& c) const {
	typedef	count_ptr<const_param>		return_type;
	STACKTRACE("value_reference<>::unroll_resolve()");
	if (value_collection_ref->get_dimensions()) {
		// dimension resolution should depend on current 
		// state of instance collection, not static analysis
		// from compile phase.
		const const_index_list rdim(resolve_dimensions(/*c*/));
		if (rdim.empty())
			return return_type(NULL);
		// else we have fully specified dimensions

		const const_range_list crl(rdim.collapsed_dimension_ranges());
		INVARIANT(!crl.empty());
		// pint_const_collection::array_type::key_type
		// is a multikey_generic<size_t>
		const count_ptr<const_collection_type>
			ret(new const_collection_type(crl.resolve_sizes()));
			// no index offset
		NEVER_NULL(ret);

		generic_index_generator_type key_gen(rdim.size());
		// automatic and temporarily allocated
		key_gen.get_lower_corner() = rdim.lower_multikey();
		key_gen.get_upper_corner() = rdim.upper_multikey();
		key_gen.initialize();
		bad_bool lookup_err(false);
		typename const_collection_type::iterator
			coll_iter(ret->begin());
		do {
			// populate the collection with values
			// lookup_value returns true on success, false on error
			// using local value is necessary because bool's 
			// reference is std::_Bit_reference.
			value_type val;
			if (!value_collection_ref->lookup_value(val, key_gen).good) {
				cerr << "ERROR: looking up index " <<
					key_gen << " of " <<
					class_traits<Tag>::tag_name <<
					" collection " <<
					value_collection_ref->get_qualified_name() <<
					"." << endl;
				lookup_err.bad = true;
			}
			*coll_iter = val;
			coll_iter++;			// unsafe, but checked
			key_gen++;
		} while (key_gen != key_gen.get_upper_corner());
		INVARIANT(coll_iter == ret->end());	// sanity check
		if (lookup_err.bad) {
			// discard incomplete results
			cerr << "ERROR: in unroll_resolve-ing "
				"pint_instance_reference." << endl;
			return return_type(NULL);
		} else {
			// safe up-cast
			return return_type(ret);
		}
	} else {
		// is 0-dimensional, scalar
		value_type _val;
		const never_ptr<value_scalar_type>
			ps(value_collection_ref.template is_a<value_scalar_type>());
		INVARIANT(ps);
		const bad_bool valid(ps->lookup_value(_val));
		if (valid.bad) {
			cerr << "ERROR: in unroll_resolve-ing "
				"value_reference, "
				"uninitialized value." << endl;
			return return_type(NULL);
		} else
			return return_type(new const_expr_type(_val));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 1
/**
	Parameters have value semantics, not alias semantics!
 */
VALUE_REFERENCE_TEMPLATE_SIGNATURE
excl_ptr<aliases_connection_base>
VALUE_REFERENCE_CLASS::make_aliases_connection_private(void) const {
	DIE;
	return excl_ptr<aliases_connection_base>(NULL);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visits children nodes and register pointers to object manager
	for serialization.
	\param m the persistent object manager.
 */
VALUE_REFERENCE_TEMPLATE_SIGNATURE
void
VALUE_REFERENCE_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {  
	collect_transient_info_base(m);
	value_collection_ref->collect_transient_info(m);
	// instantiation_state has no pointers
}
// else already visited
}
		
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Writes the instance reference to output stream, translating
	pointers to indices as it goes along.
	Note: the instantiation base must be written before the
		state information, for reconstruction purposes.
	\param m the persistent object manager.
 */
VALUE_REFERENCE_TEMPLATE_SIGNATURE
void    
VALUE_REFERENCE_CLASS::write_object(
		const persistent_object_manager& m, ostream& f) const {
	m.write_pointer(f, value_collection_ref);
	write_object_base(m, f);
}
	
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/** 
	Loads the instance reference from an input stream, translating
	indices to pointers.
	Note: the instantiation base must be loaded before the
		state information, because the instantiation state
		depends on the instantiation base being complete.
	\param m the persistent object manager.
 */
VALUE_REFERENCE_TEMPLATE_SIGNATURE
void
VALUE_REFERENCE_CLASS::load_object(const persistent_object_manager& m, 
		istream& f) {
	m.read_pointer(f, value_collection_ref);
	NEVER_NULL(value_collection_ref);
	m.load_object_once(
		const_cast<value_collection_type*>(&*value_collection_ref));
	load_object_base(m, f);
}

//-----------------------------------------------------------------------------
// class VALUE_REFERENCE_CLASS::assigner method definitions

/**
	Constructor caches the sequence of values for assigning to 
	an integer instance collection.  
 */
VALUE_REFERENCE_TEMPLATE_SIGNATURE
VALUE_REFERENCE_CLASS::assigner::assigner(const expr_base_type& p) :
		src(p), ranges(), vals() {
	if (src.dimensions()) {
		ranges = src.resolve_dimensions();
		if (ranges.empty()) {
			// if empty list returned, there was an error,
			// because we know that the # dimensions is > 0.
			cerr << "ERROR: assignment unrolling expecting "
				"valid dimensions!" << endl;
			// or throw exception
			THROW_EXIT;
		}
		// load values into cache list as a sequence
		// pass list by reference to a virtual func?
		const bad_bool err(src.resolve_values_into_flat_list(vals));
		if (err.bad) {
			cerr << "ERROR: in flattening " <<
				class_traits<Tag>::value_type_name <<
				" values." << endl;
			THROW_EXIT;
		}
	} else {	// is just scalar value
		// leave ranges empty
		value_type i;
		if (src.resolve_value(i).good) {
			vals.push_back(i);
		} else {
			cerr << "ERROR: resolving scalar " <<
				class_traits<Tag>::value_type_name <<
				" value!" << endl;
			THROW_EXIT;
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigns cached list of unrolled values to the destination
	instance collection.  
	\param b the cumulative error status.
	\param p the destination instance reference.  
	\return error (true) if anything goes wrong, or has gone wrong before.  
 */
VALUE_REFERENCE_TEMPLATE_SIGNATURE
bad_bool
VALUE_REFERENCE_CLASS::assigner::operator() (const bad_bool b, 
		const VALUE_REFERENCE_CLASS& p) const {
	// check dimensions for match first
	if (ranges.empty()) {
		INVARIANT(vals.size() == 1);
		// is scalar assignment, but may be indexed
		const never_ptr<value_scalar_type> 
			scalar_inst(p.value_collection_ref.
				template is_a<value_scalar_type>());
		if (scalar_inst) {
			return bad_bool(scalar_inst->assign(vals.front())) || b;
		}
	}
	// else is scalar or array, but must resolve indices
	const const_index_list dim(p.resolve_dimensions());
	if (dim.empty()) {
		cerr << "ERROR: unable to resolve constant dimensions."
			<< endl;
		THROW_EXIT;
		// return true;
	}
	// We are assured that the dimensions of the references
	// are equal, b/c dimensionality is statically checked.  
	// However, ranges may be of different length because
	// of collapsible dimensions.  
	// Compare dim against ranges: sizes of each dimension...
	// but what about collapsed dimensions?
	if (!ranges.empty() && !ranges.equal_dimensions(dim)) {
		// if range.empty(), then there is no need to match dimensions,
		// dimensions must be equal because both src/dest are scalar.
		cerr << "ERROR: resolved indices are not "
			"dimension-equivalent!" << endl;
		ranges.dump(cerr << "got: ");
		dim.dump(cerr << " and: ") << endl;
		THROW_EXIT;
		// return true;
	}
	// else good to continue

	generic_index_generator_type key_gen(dim.size());
	key_gen.get_lower_corner() = dim.lower_multikey();
	key_gen.get_upper_corner() = dim.upper_multikey();
	key_gen.initialize();

	typename list<value_type>::const_iterator list_iter = vals.begin();
	bad_bool assign_err(false);
	do {
		if (p.value_collection_ref->assign(key_gen, *list_iter).bad) {
			cerr << "ERROR: assigning index " << key_gen << 
				" of " << class_traits<Tag>::tag_name <<
				" collection " <<
				p.value_collection_ref->get_qualified_name() <<
				"." << endl;
			assign_err.bad = true;
		}
		list_iter++;			// unsafe, but checked
		key_gen++;
	} while (key_gen != key_gen.get_upper_corner());
	INVARIANT(list_iter == vals.end());	// sanity check
	return assign_err || b;
}

//=============================================================================
}	// end namepace entity
}	// end namepace ART

#endif	// __ART_OBJECT_VALUE_REFERENCE_TCC__

