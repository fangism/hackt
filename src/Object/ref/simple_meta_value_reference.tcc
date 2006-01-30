/**
	\file "Object/ref/simple_meta_value_reference.tcc"
	Class method definitions for semantic expression.  
	This file was reincarnated from "Object/art_object_value_reference.tcc".
 	$Id: simple_meta_value_reference.tcc,v 1.9 2006/01/30 07:42:05 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_SIMPLE_META_VALUE_REFERENCE_TCC__
#define	__HAC_OBJECT_REF_SIMPLE_META_VALUE_REFERENCE_TCC__

// flags for controlling conditional compilation, mostly for debugging
#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE				0
#endif

#ifndef	STACKTRACE_DESTRUCTORS
#define	STACKTRACE_DESTRUCTORS				0 && ENABLE_STACKTRACE
#endif

#include <iostream>
#include <exception>
#include <algorithm>

#include "Object/ref/simple_meta_value_reference.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/traits/class_traits.h"
#include "Object/def/definition_base.h"
#include "Object/common/namespace.h"
#include "Object/common/multikey_index.h"
#include "Object/common/dump_flags.h"
#include "Object/expr/const_param.h"
#include "Object/expr/const_index.h"
#include "Object/expr/const_range.h"
#include "Object/expr/const_range_list.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/unroll/unroll_context_value_resolver.h"
#include "Object/def/footprint.h"
#include "common/ICE.h"

// experimental: suppressing automatic instantiation of template code
// #include "Object/common/extern_templates.h"

#include "util/multikey.h"
#include "util/macros.h"
#include "util/what.h"
#include "util/stacktrace.h"
#include "util/persistent_object_manager.h"
#include "util/memory/count_ptr.tcc"

//=============================================================================
namespace HAC {
namespace entity {
//=============================================================================
using namespace util::memory;
#include "util/using_ostream.h"
using util::persistent_traits;

//=============================================================================
// class simple_meta_value_reference method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.  
 */
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_META_VALUE_REFERENCE_CLASS::simple_meta_value_reference() :
		common_base_type(), 
		parent_type(), interface_type(), value_collection_ref(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_META_VALUE_REFERENCE_CLASS::simple_meta_value_reference(
		const value_collection_ptr_type pi) :
		common_base_type(
			pi->current_collection_state()), 
		parent_type(), 
		interface_type(), 
		value_collection_ref(pi) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_META_VALUE_REFERENCE_CLASS::simple_meta_value_reference(
		const value_collection_ptr_type pi,
		excl_ptr<index_list>& i) :
		common_base_type(), 
		parent_type(i, pi->current_collection_state()),
		interface_type(), 
		value_collection_ref(pi) {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default destructor.
 */
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_META_VALUE_REFERENCE_CLASS::~simple_meta_value_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
never_ptr<const instance_collection_base>
SIMPLE_META_VALUE_REFERENCE_CLASS::get_inst_base(void) const {
	return value_collection_ref;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
never_ptr<const typename SIMPLE_META_VALUE_REFERENCE_CLASS::value_collection_parent_type>
SIMPLE_META_VALUE_REFERENCE_CLASS::get_param_inst_base(void) const {
	return value_collection_ref;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
SIMPLE_META_VALUE_REFERENCE_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
SIMPLE_META_VALUE_REFERENCE_CLASS::dump(ostream& o,
		const expr_dump_context& c) const {
	return grandparent_type::dump(o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
size_t
SIMPLE_META_VALUE_REFERENCE_CLASS::dimensions(void) const {
	return grandparent_type::dimensions();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
SIMPLE_META_VALUE_REFERENCE_CLASS::has_static_constant_dimensions(void) const {
	return grandparent_type::has_static_constant_dimensions();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
const_range_list
SIMPLE_META_VALUE_REFERENCE_CLASS::static_constant_dimensions(void) const {
	return grandparent_type::static_constant_dimensions();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	IMPORTANT: This initialization is only for static analysis and is
	not the actual initialization that takes place during unrolling.  
	\return true if sucessfully initialized with valid expression.  
 */
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
SIMPLE_META_VALUE_REFERENCE_CLASS::initialize(const init_arg_type& i) {
	return this->value_collection_ref->initialize(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
SIMPLE_META_VALUE_REFERENCE_CLASS::may_be_initialized(void) const {
	return common_base_type::may_be_initialized();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
SIMPLE_META_VALUE_REFERENCE_CLASS::must_be_initialized(void) const {
	return common_base_type::must_be_initialized();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
SIMPLE_META_VALUE_REFERENCE_CLASS::is_static_constant(void) const {
	return common_base_type::is_static_constant();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
SIMPLE_META_VALUE_REFERENCE_CLASS::is_relaxed_formal_dependent(void) const {
	return common_base_type::is_relaxed_formal_dependent();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
SIMPLE_META_VALUE_REFERENCE_CLASS::is_template_dependent(void) const {
	return common_base_type::is_template_dependent();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
SIMPLE_META_VALUE_REFERENCE_CLASS::is_loop_independent(void) const {
	return common_base_type::is_loop_independent();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
SIMPLE_META_VALUE_REFERENCE_CLASS::is_unconditional(void) const {
	return common_base_type::is_unconditional();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Better make sure that this is_static_constant before calling, 
	else will assert-fail.
 */
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
typename SIMPLE_META_VALUE_REFERENCE_CLASS::value_type
SIMPLE_META_VALUE_REFERENCE_CLASS::static_constant_value(void) const {
	STACKTRACE_VERBOSE;
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
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
SIMPLE_META_VALUE_REFERENCE_CLASS::must_be_equivalent(
		const expr_base_type& b) const {
	STACKTRACE_VERBOSE;
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
			if (this->array_indices && br->array_indices) {
				return this->array_indices->
					must_be_equivalent_indices(
						*br->array_indices);
			} else {
				return true;
			}
		} else {
			return false;
		}
	} else {
		STACKTRACE("ACK!!");
		// conservatively
		return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This version specifically asks for one integer value, 
	thus the array indices must be scalar (0-D).  
	Now checks unroll context to see if the referenced
	value collection belongs to a complete type (definition) scope.  
	\return good if resolution succeeds.
	NOTE: loop induction variables (pint) do not exist in footprint!
 */
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
SIMPLE_META_VALUE_REFERENCE_CLASS::unroll_resolve_value(
		const unroll_context& c, value_type& i) const {
	STACKTRACE_VERBOSE;
	// using a policy to specialize for lookups covering
	// local loop variables
	const pair<bool, const value_collection_type*>
		_v(unroll_context_value_resolver<Tag>().operator()
			(c, *value_collection_ref, i));
	if (_v.first) {
		// then our work is done, 
		// i has already been set as a loop variable
		return good_bool(true);
	}
	// stupid gcc-3.3 needs .operator()...
	const value_collection_type& _vals(*_v.second);

	if (this->array_indices) {
		const const_index_list
			indices(this->array_indices->unroll_resolve(c));
		if (!indices.empty()) {
			const multikey_index_type
				lower(indices.lower_multikey());
			const multikey_index_type
				upper(indices.upper_multikey());
			if (lower != upper) {
				cerr << "ERROR: upper != lower" << endl;
				return good_bool(false);
			}
			// what if this references a formal parameter?
			// then we need to get the template actuals
			return _vals.lookup_value(i, lower, c);
		} else {
			cerr << "Unable to unroll-resolve array_indices!"
				<< endl;
			return good_bool(false);
		}
	} else {
		// assert dynamic cast
		// what if is pbool_const or pint_const?
		const value_scalar_type&
			scalar_inst(IS_A(const value_scalar_type&, _vals));
		// c.dump(STACKTRACE_INDENT) << endl;
		return scalar_inst.lookup_value(i, c);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This version specifically asks for one integer value, 
	thus the array indices must be scalar (0-D).  
	\return true if resolution succeeds, else false.
 */
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
SIMPLE_META_VALUE_REFERENCE_CLASS::resolve_value(value_type& i) const {
	if (this->array_indices) {
		const const_index_list
			indices(this->array_indices->resolve_index_list());
		if (!indices.empty()) {
			const multikey_index_type
				lower(indices.lower_multikey());
			const multikey_index_type
				upper(indices.upper_multikey());
			if (lower != upper) {
				cerr << "ERROR: upper != lower" << endl;
				return good_bool(false);
			}
			return value_collection_ref->lookup_value(
				i, lower, unroll_context());
		} else {
			cerr << "Unable to resolve array_indices!" << endl;
			return good_bool(false);
		}
	} else {
		const never_ptr<value_scalar_type>
			scalar_inst(value_collection_ref.template is_a<value_scalar_type>());
		NEVER_NULL(scalar_inst);
		return scalar_inst->lookup_value(i, unroll_context());
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns the dimensions of the collection in the current state, 
	ONLY IF, the indexed reference to the current state is all valid.  
	Otherwise, returns an empty list, which is interpreted as an error.  

	Really this should be independent of type?
	Except for checking implicit indices...
 */
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
const_index_list
SIMPLE_META_VALUE_REFERENCE_CLASS::resolve_dimensions(void) const {
	// criterion 1: indices (if any) must be resolved to constant values.  
	if (this->array_indices) {
		const const_index_list
			c_i(this->array_indices->resolve_index_list());
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
	Resolves a list of indices during the unroll phase.  

	Source: copied from above ::resolve_dimensions (2005-07-02) and 
		substituted the call to resolve_index_list().
 */
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
const_index_list
SIMPLE_META_VALUE_REFERENCE_CLASS::unroll_resolve_dimensions(
		const unroll_context& c) const {
	// criterion 1: indices (if any) must be resolved to constant values.  
	if (this->array_indices) {
		const const_index_list
			c_i(this->array_indices->unroll_resolve(c));
		if (c_i.empty()) {
			cerr << "ERROR: failed to unroll-resolve index list." << endl;
			return c_i;
		}
		// else let c_i remain empty, underspecified
		// check for implicit indices, that sub-arrays are
		// densely packed with the same dimensions.  
		const const_index_list
			r_i(value_collection_ref->resolve_indices(c_i));
		if (r_i.empty()) {
			cerr << "ERROR: implicitly unroll-resolving index list."
				<< endl;
		}
		return r_i;
		// Elsewhere (during assign) check for initialization.
	} else {
		// TODO: factor out common code above
		// should try to form dense index list 
		// for entire collection
		const const_index_list
			r_i(value_collection_ref->resolve_indices(
				const_index_list()));
		if (r_i.empty()) {
			cerr << "ERROR: implicitly unroll-resolving index list."
				<< endl;
		}
		return r_i;
	}
	// Elsewhere (during assign) check for initialization.  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resolves a scalar or collective instance reference into a 
	packed array of values.  
	\param c unrolling context, may contain template actuals.
	\return dense array of values, NULL if error.  
 */
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<const_param>
SIMPLE_META_VALUE_REFERENCE_CLASS::unroll_resolve(
		const unroll_context& c) const {
	typedef	count_ptr<const_param>		return_type;
	STACKTRACE("simple_meta_value_reference<>::unroll_resolve()");
	// this replaces template formal references with template
	// actuals from the context where necessary (2005-06-30)
if (value_collection_ref->is_template_formal()) {
	const count_ptr<const const_param>
		// beware mutual recursion...
		cpptr(c.lookup_actual(*value_collection_ref));
	if (!cpptr) {
		cerr << "Error unroll-resolving parameter values." << endl;
		return return_type(NULL);
	} else if (value_collection_ref->get_dimensions()) {
		const const_collection_type&
			ce(IS_A(const const_collection_type&, *cpptr));
		// NOTE: not reached yet by any test cases
		const const_index_list rdim(unroll_resolve_dimensions(c));
		if (rdim.empty()) {		// error, failed to resolve
			cerr << "Error: failed to resolve dimensions of "
				"collection referenced: ";
			this->dump(cerr, expr_dump_context::default_value)
				<< endl;
			return return_type(NULL);
		}
		return return_type(
			new const_collection_type(ce.make_value_slice(rdim)));
		// TODO: error handling necessary
		// hope the above elides constructor
	} else {
		// is scalar
		const const_expr_type&
			ce(IS_A(const const_expr_type&, *cpptr));
		return return_type(new const_expr_type(ce));
	}
} else {
	// non template formal, normal param collection referenced
	// catches case of loop variable resolution
#if 0
	// TODO: NOTE!!! could be definition-scope local variable!
	// could also be loop variable
	const value_collection_type& vcref(*value_collection_ref);
#else
	value_type cv = 0;
	const pair<bool, const value_collection_type*>
		_r(unroll_context_value_resolver<Tag>().operator()
			(c, *value_collection_ref, cv));
	if (_r.first) {
		// then we resolved a loop variable (scalar)
		return count_ptr<const_expr_type>(new const_expr_type(cv));
	}
	const value_collection_type& vcref(*_r.second);
#endif
	if (vcref.get_dimensions()) {
		// dimension resolution should depend on current 
		// state of instance collection, not static analysis
		// from compile phase.
		const const_index_list rdim(unroll_resolve_dimensions(c));
		if (rdim.empty()) {
			cerr << "ERROR: failed to resolve dimensions of "
				"collection referenced: ";
			vcref.dump(cerr, dump_flags::verbose) << endl;
			return return_type(NULL);
		}
		// else we have fully specified dimensions

		const const_range_list crl(rdim.collapsed_dimension_ranges());
#if 0
		// NOT TRUE: instance reference may be scalar, 
		// which results in a 0-dimensional reference when collapsed.
		if (crl.empty()) {
		ICE(cerr, 
			cerr << "got an empty range list after "
				"collapsing dimension ranges, from: " << endl;
			rdim.dump(cerr << "const_index_list = ") << endl;
		)
		}
		INVARIANT(!crl.empty());
#endif
		// pint_const_collection::array_type::key_type
		// is a multikey_generic<size_t>
		// NOTE: possible to have a bad dynamic range (e.g. backwards)
		// thus we need to catch it
#if 1
		multikey_index_type collection_dimensions;
		try {
			collection_dimensions = crl.resolve_sizes();
			// requires custom definition of 
			// multikey_generic assignment, due to valarray-ness.
			// ACK! this doesn't to the same thing as 
			// passing resolve_size() directly into 
			// const_collection_type's constructor, WTF!?!?
		} catch (...) {
			crl.dump_force(cerr << "ERROR: bad range in "
				"const_range_list, got: ") << endl;
			return return_type(NULL);
		}
		const count_ptr<const_collection_type>
			ret(new const_collection_type(collection_dimensions));
#else
		const count_ptr<const_collection_type>
			ret(new const_collection_type(crl.resolve_sizes()));
#endif
			// no index offset
		NEVER_NULL(ret);

		generic_index_generator_type key_gen(rdim.size());
		// automatic and temporarily allocated
		key_gen.get_lower_corner() = rdim.lower_multikey();
		key_gen.get_upper_corner() = rdim.upper_multikey();
		key_gen.initialize();
#if 0
		ret->dump(cerr << "ret = ") << endl;
#endif
		bad_bool lookup_err(false);
		typename const_collection_type::iterator
			coll_iter(ret->begin());
		do {
#if 0
			cerr << "key_gen = " << key_gen << endl;
#endif
			// populate the collection with values
			// lookup_value returns true on success, false on error
			// using local value is necessary because bool's 
			// reference is std::_Bit_reference.
			value_type val;
			if (!vcref.lookup_value(val, key_gen, c).good) {
#if 0
				// callee already has error message
				cerr << "ERROR: looking up index " <<
					key_gen << " of " <<
					class_traits<Tag>::tag_name <<
					" collection " <<
					vcref.get_qualified_name() <<
					"." << endl;
#endif
				lookup_err.bad = true;
			}
			*coll_iter = val;
			coll_iter++;			// unsafe, but checked
			key_gen++;
		} while (key_gen != key_gen.get_lower_corner());
		INVARIANT(coll_iter == ret->end());	// sanity check
		if (lookup_err.bad) {
			// discard incomplete results
			cerr << "ERROR: in unroll_resolve-ing "
				"simple_meta_instance_reference." << endl;
			return return_type(NULL);
		} else {
			// safe up-cast
			return return_type(ret);
		}
	} else {
		// is 0-dimensional, scalar
		value_type _val;
		const never_ptr<const value_scalar_type>
#if 0
			ps(value_collection_ref.template is_a<value_scalar_type>());
#else
			ps(IS_A(const value_scalar_type*, &vcref));
#endif
		INVARIANT(ps);
		const bad_bool valid(ps->lookup_value(_val, c));
		if (valid.bad) {
			cerr << "ERROR: in unroll_resolve-ing "
				"simple_meta_value_reference, "
				"uninitialized value." << endl;
			return return_type(NULL);
		} else
			return return_type(new const_expr_type(_val));
	}
}
}	// end method unroll_resolve

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Parameters have value semantics, not alias semantics!
 */
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
excl_ptr<aliases_connection_base>
SIMPLE_META_VALUE_REFERENCE_CLASS::make_aliases_connection_private(void) const {
	ICE_NEVER_CALL(cerr);
	return excl_ptr<aliases_connection_base>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This should never be called.  
 */
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
never_ptr<substructure_alias>
SIMPLE_META_VALUE_REFERENCE_CLASS::unroll_scalar_substructure_reference(
		const unroll_context& ) const {
	ICE_NEVER_CALL(cerr);
	return never_ptr<substructure_alias>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This should never be called.  
 */
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bad_bool
SIMPLE_META_VALUE_REFERENCE_CLASS::connect_port(
		instance_collection_base&, 
		const unroll_context&) const {
	ICE_NEVER_CALL(cerr);
	return bad_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visits children nodes and register pointers to object manager
	for serialization.
	\param m the persistent object manager.
 */
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_META_VALUE_REFERENCE_CLASS::collect_transient_info(
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
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
void    
SIMPLE_META_VALUE_REFERENCE_CLASS::write_object(
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
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_META_VALUE_REFERENCE_CLASS::load_object(
		const persistent_object_manager& m, istream& f) {
	m.read_pointer(f, value_collection_ref);
	NEVER_NULL(value_collection_ref);
	m.load_object_once(
		const_cast<value_collection_type*>(&*value_collection_ref));
	load_object_base(m, f);
}

//-----------------------------------------------------------------------------
/**
	TODO: This will need some context eventually.
	assignment's destination is either a top-level value collection 
	or a definition-local, private value-collection.  
 */
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bad_bool
SIMPLE_META_VALUE_REFERENCE_CLASS::assign_value_collection(
		const const_collection_type& values, 
		const unroll_context& c) const {
#if 0
	value_collection_type& _vals(*value_collection_ref);
	if (_vals.get_owner().template is_a<const definition_base>()) {
		cerr << "FANG, enable definition-local private "
			"value reference resolution in "
			"simple_meta_value_reference::assign_value_collection!"
			<< endl;
		return bad_bool(true);
	}
#else
	value_collection_type&
		_vals(unroll_context_value_resolver<Tag>().operator()
			(c, *value_collection_ref));
#endif
	// else we have top-level value reference
	// TODO: repeated call to same invariant source value is wasteful...
	const const_range_list src_ranges(values.static_constant_dimensions());
	if (src_ranges.empty()) {
		INVARIANT(!values.dimensions());
		// is scalar assignment, but may be indexed
		value_scalar_type* const
			scalar_inst(IS_A(value_scalar_type*, &_vals));
		if (scalar_inst) {
			return scalar_inst->assign(*values.begin());
		}
	}

	const const_index_list dim(this->unroll_resolve_dimensions(c));
	if (dim.empty()) {
		cerr << "ERROR: unable to resolve constant dimensions."
			<< endl;
		return bad_bool(true);
	}
	// We are assured that the dimensions of the references
	// are equal, b/c dimensionality is statically checked.  
	// However, ranges may be of different length because
	// of collapsible dimensions.  
	// Compare dim against ranges: sizes of each dimension...
	// but what about collapsed dimensions?
	const const_range_list dst_ranges(dim.collapsed_dimension_ranges());
	if (!src_ranges.empty() && !src_ranges.is_size_equivalent(dst_ranges)) {
		// if range.empty(), then there is no need to match dimensions,
		// dimensions must be equal because both src/dest are scalar.
		cerr << "ERROR: resolved indices are not "
			"dimension-equivalent!" << endl;
		src_ranges.dump(cerr << "got: ",
			expr_dump_context::default_value);
		dim.dump(cerr << " and: ",
			expr_dump_context::default_value) << endl;
		return bad_bool(true);
	}
	// else good to continue
	generic_index_generator_type key_gen(dim.size());
	key_gen.get_lower_corner() = dim.lower_multikey();
	key_gen.get_upper_corner() = dim.upper_multikey();
	key_gen.initialize();

	typename const_collection_type::const_iterator
		val_iter(values.begin());
	bad_bool assign_err(false);
	do {
		if (_vals.assign(key_gen, *val_iter).bad) {
			cerr << "ERROR: assigning index " << key_gen << 
				" of " << class_traits<Tag>::tag_name <<
				" collection " << _vals.get_qualified_name() <<
				"." << endl;
			assign_err.bad = true;
		}
		val_iter++;			// unsafe, but checked
		key_gen++;
	} while (key_gen != key_gen.get_upper_corner());
	INVARIANT(val_iter == values.end());	// sanity check
	return assign_err;
}	// end method assign_value_collection

//=============================================================================
}	// end namepace entity
}	// end namepace HAC

#endif	// __HAC_OBJECT_REF_SIMPLE_META_VALUE_REFERENCE_TCC__

