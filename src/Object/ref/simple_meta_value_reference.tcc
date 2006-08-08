/**
	\file "Object/ref/simple_meta_value_reference.tcc"
	Class method definitions for semantic expression.  
	This file was reincarnated from "Object/art_object_value_reference.tcc".
 	$Id: simple_meta_value_reference.tcc,v 1.22 2006/08/08 05:46:43 fang Exp $
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
#include <stdexcept>
#include <algorithm>

#include "Object/ref/simple_meta_value_reference.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/aggregate_meta_instance_reference_base.h"
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
#include "Object/ref/meta_value_reference.h"
#include "common/ICE.h"
#include "common/TODO.h"

// experimental: suppressing automatic instantiation of template code
// #include "Object/common/extern_templates.h"

#include "util/multikey.h"
#include "util/packed_array.tcc"
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
		simple_meta_indexed_reference_base(), 
		parent_type(), 
		value_collection_ref(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_META_VALUE_REFERENCE_CLASS::simple_meta_value_reference(
		const value_collection_ptr_type pi) :
		simple_meta_indexed_reference_base(), 
		parent_type(), 
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
never_ptr<const param_value_collection>
SIMPLE_META_VALUE_REFERENCE_CLASS::get_coll_base(void) const {
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
	// shamelessly copied from new simple_meta_instance_reference::dump
	if (c.include_type_info) {
		this->what(o) << " ";
	}
	NEVER_NULL(this->value_collection_ref);
	if (c.enclosing_scope) {
		this->value_collection_ref->dump_hierarchical_name(o,
			dump_flags::no_definition_owner);
	} else {
		this->value_collection_ref->dump_hierarchical_name(o,
			dump_flags::default_value);
	}
	return simple_meta_indexed_reference_base::dump_indices(o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
size_t
SIMPLE_META_VALUE_REFERENCE_CLASS::dimensions(void) const {
	// copied from simple_meta_indexed_reference_base::dimensions();
	// size_t dim = get_coll_base()->get_dimensions();
	size_t dim = this->value_collection_ref->get_dimensions();
	if (this->array_indices) {
		const size_t c = this->array_indices->dimensions_collapsed();
		INVARIANT(c <= dim);
		return dim -c;
	} else	return dim;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Copy-reduced from simple_meta_indexed_reference_base.
 */
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
SIMPLE_META_VALUE_REFERENCE_CLASS::attach_indices(
		excl_ptr<index_list_type>& i) {
	INVARIANT(!array_indices);
	NEVER_NULL(i);
	// dimension-check:
	// number of indices must be <= dimension of instance collection.  
	const size_t max_dim = dimensions();    // depends on indices
	if (i->size() > max_dim) {
		cerr << "ERROR: instance collection " <<
			this->value_collection_ref->get_name()
			<< " is " << max_dim << "-dimensional, and thus, "
			"cannot be indexed " << i->size() <<
			"-dimensionally!  ";
			// caller will say where
		return good_bool(false);
	}
	// no static dimension checking
	array_indices = i;
	return good_bool(true);
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
	return this->value_collection_ref->may_be_initialized();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
SIMPLE_META_VALUE_REFERENCE_CLASS::must_be_initialized(void) const {
	return this->value_collection_ref->must_be_initialized();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
SIMPLE_META_VALUE_REFERENCE_CLASS::is_static_constant(void) const {
	if (this->array_indices)
		return false;
	else if (this->value_collection_ref->get_dimensions())
		return false;
	else	return this->value_collection_ref->is_static_constant();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
SIMPLE_META_VALUE_REFERENCE_CLASS::is_relaxed_formal_dependent(void) const {
	return this->value_collection_ref->is_relaxed_template_formal() ||
		(this->array_indices ?
			this->array_indices->is_relaxed_formal_dependent()
			: false);
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
		// FINISH_ME(Fang);	// could do more checking...
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
		// stupid gcc-3.3 needs .operator()...
	if (_v.first) {
		// then our work is done, 
		// i has already been set as a loop variable
		return good_bool(true);
	}
	const value_collection_type& _vals(*_v.second);

	if (this->array_indices) {
		const const_index_list
			indices(this->array_indices->unroll_resolve_indices(c));
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
		substituted the call to resolve_index_list(), 
		passing in the unroll_context.
	TODO: write helper functions to factor out blatantly copied code.
 */
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
const_index_list
SIMPLE_META_VALUE_REFERENCE_CLASS::unroll_resolve_dimensions(
		const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	// criterion 1: indices (if any) must be resolved to constant values.  
	const_index_list c_i;
	if (this->array_indices) {
		c_i = this->array_indices->unroll_resolve_indices(c);
		if (c_i.empty()) {
			cerr << "ERROR: failed to unroll-resolve index list."
				<< endl;
			return c_i;
		}
	}
	// else let c_i remain empty, underspecified
	// check for implicit indices, that sub-arrays are
	// densely packed with the same dimensions.  
	// try to form dense index list for entire collection
	const const_index_list
		r_i(value_collection_ref->resolve_indices(c_i));
	if (r_i.empty()) {
		cerr << "ERROR: implicitly unroll-resolving index list."
			<< endl;
	}
	return r_i;
	// Elsewhere (during assign) check for initialization.  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: most of this code ripped from unroll_resolve_rvalues
	so take care when maintaining!
	TODO: factor out to eliminate duplication...
	Resolution: just wrap and save myself the trouble...
 */
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<const typename SIMPLE_META_VALUE_REFERENCE_CLASS::const_expr_type>
SIMPLE_META_VALUE_REFERENCE_CLASS::__unroll_resolve_rvalue(
		const unroll_context& c, 
		const count_ptr<const expr_base_type>& p) const {
	typedef	count_ptr<const const_expr_type>	return_type;
	const count_ptr<const const_param>
		temp(this->unroll_resolve_rvalues(c, p));
if (temp) {
	const return_type ret(temp.template is_a<const const_expr_type>());
	if (ret) {
		return ret;
	} else {
		const count_ptr<const const_collection_type>
			cret(temp.template is_a<const const_collection_type>());
		NEVER_NULL(cret);	// what else could it be?
		if (cret->dimensions()) {
			cerr << "Error: got non-scalar value where "
				"scalar value was expected." << endl;
			return return_type(NULL);
		} else {
			return return_type(new const_expr_type(cret->front()));
		}
	}
} else {
	return return_type(NULL);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resolves a scalar or collective instance reference into a 
	packed array of values.  
	\param c unrolling context, may contain template actuals.
	\return dense array of values, NULL if error.  
		When result is scalar, always returns the const_expr_type.  
 */
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<const const_param>
SIMPLE_META_VALUE_REFERENCE_CLASS::unroll_resolve_rvalues(
		const unroll_context& c, 
		const count_ptr<const expr_base_type>& p) const {
	typedef	count_ptr<const const_param>		return_type;
	STACKTRACE_VERBOSE;
	INVARIANT(p == this);
#if ENABLE_STACKTRACE
	this->dump(STACKTRACE_INDENT << "this reference = ", 
		expr_dump_context::default_value) << endl;
#endif
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
		// since this is a template formal, we can be sure that the
		// collection of constants is complete, and that if 
		// !array_indices, it refers to the entire collection.
		// the actual parameter value collection cannot be augmented
		// by sparse declarations, as dictated by the language.
		const const_collection_type&
			ce(IS_A(const const_collection_type&, *cpptr));
		// NOTE: not reached yet by any test cases
		// ... until NOW! (20060211)
		if (this->array_indices) {
			// no need to use the collection's resolving
			// because we already know the dimensions of the 
			// collection, so we only need to resolve
			// parameter references in the indices.
			const const_index_list
				cil(this->array_indices->unroll_resolve_indices(c));
			// damn it, array_indices is an excl_ptr :S
			// can't use static meta_index_list::unroll_resolve_rvalues
			if (cil.empty()) {
				cerr << "Error resolving indices of " <<
					util::what<this_type>::name() << endl;
				return return_type(NULL);
			}
			// NOTE: still need to handle implicit indices
			// but is efficient, because no searching required.  
			try {
				return return_type(new const_collection_type(
					ce.make_value_slice(cil)));
			} catch (const std::out_of_range&) {
				// cerr << r.what() << endl;
				cerr << "Error: indices out of range of " <<
					util::what<this_type>::name() << 
					".  got: ";
				cil.dump(cerr) << endl;
				return return_type(NULL);
			} catch (...) {
				// have no idea! re-throw
				throw;
			}
		} else {
			// just return deep copy of the whole const_collection
			return return_type(new const_collection_type(ce));
		}
	} else {
		// is scalar
		const const_expr_type&
			ce(IS_A(const const_expr_type&, *cpptr));
		return return_type(new const_expr_type(ce));
	}
} else {
	// non template formal, normal param collection referenced
	// catches case of loop variable resolution (hackish)
	value_type cv = 0;
	const pair<bool, const value_collection_type*>
		_r(unroll_context_value_resolver<Tag>().operator()
			(c, *value_collection_ref, cv));
	if (_r.first) {
		// then we resolved a loop variable (scalar)
		return count_ptr<const_expr_type>(new const_expr_type(cv));
	}
	const value_collection_type& vcref(*_r.second);
	if (vcref.get_dimensions()) {
		// dimension resolution should depend on current 
		// state of instance collection, not static analysis
		// from compile phase.
		// Also, this is not a template-formal parameter
		// so its collection may be augmented at any time.  
		const_index_list cil;		// initialize empty
		if (this->array_indices) {
			cil = this->array_indices->unroll_resolve_indices(c);
			if (cil.empty()) {
				cerr << "Error resolving indices of " <<
					util::what<this_type>::name() << endl;
				return return_type(NULL);
			}
		}
		// now check the state of the collection
		const const_index_list rdim(vcref.resolve_indices(cil));
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
			if (!vcref.lookup_value(val, key_gen, c).good) {
#if 0
				// callee already has error message
				cerr << "ERROR: looking up index " <<
					key_gen << " of " <<
					traits_type::tag_name <<
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
			ps(IS_A(const value_scalar_type*, &vcref));
		INVARIANT(ps);
		const bad_bool valid(ps->lookup_value(_val, c));
		if (valid.bad) {
			cerr << "ERROR: in unroll_resolve-ing "
				"simple_meta_value_reference, "
				"uninitialized value." << endl;
			return return_type(NULL);
		} else {
			return return_type(new const_expr_type(_val));
		}
	}
}
}	// end method unroll_resolve_rvalues

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapped call to unroll_resolve_rvalues that returns resolved
	values suitable for nonmeta representations.  
 */
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<const typename SIMPLE_META_VALUE_REFERENCE_CLASS::expr_base_type>
SIMPLE_META_VALUE_REFERENCE_CLASS::unroll_resolve_copy(
		const unroll_context& c,
		const count_ptr<const expr_base_type>& p) const {
	INVARIANT(p == this);
	return this->unroll_resolve_rvalues(c, p)
		.template is_a<const expr_base_type>();
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
	this->collect_transient_info_base(m);
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
	this->write_object_base(m, f);
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
	this->load_object_base(m, f);
}

//-----------------------------------------------------------------------------
/**
	Implementation partly ripped off from 
	simple_meta_instance_reference::unroll_references_packed_helper_no_lookup.  
	Needs additional check for template formal however.  
 */
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bad_bool
SIMPLE_META_VALUE_REFERENCE_CLASS::unroll_lvalue_references(
		const unroll_context& c, 
		value_reference_collection_type& a) const {
	STACKTRACE_VERBOSE;
	value_collection_type&
		_vals(unroll_context_value_resolver<Tag>().operator()
			(c, *value_collection_ref));
if (_vals.get_dimensions()) {
	STACKTRACE("is array");
	const_index_list cil;
	if (this->array_indices) {
		cil = this->array_indices->unroll_resolve_indices(c);
		if (cil.empty()) {
			cerr << "ERROR: Failed to resolve indices at "
				"unroll-time!" << endl;
			return bad_bool(true);
		}
	}
	// else empty, implicitly refer to whole collection if it is dense
	// we have resolve constant indices
	const const_index_list
		full_indices(_vals.resolve_indices(cil));
	if (full_indices.empty()) {
		// might fail because implicit slice reference is not packed
		cerr << "ERROR: failed to resolve implicit indices from "
			"a collection whose subarray is not dense."  << endl;
		cil.dump(_vals.dump_hierarchical_name(
				cerr << "\tindices referenced: ",
				dump_flags::verbose),
			expr_dump_context::default_value) << endl;
		_vals.dump(cerr << "\tcollection state: ", dump_flags::verbose)
			<< endl;
		// _vals.dump_unrolled_instances(cerr, dump_flags::verbose);
		return bad_bool(true);
	}       
	// resize the array according to the collapsed dimensions, 
	// before passing it to unroll_aliases.
	{
	const const_range_list
		crl(full_indices.collapsed_dimension_ranges());
	try {
		const multikey_index_type
			array_sizes(crl.resolve_sizes());
		a.resize(array_sizes);
		// a.resize(upper -lower +ones);
	} catch (const_range_list::bad_range r) {
		const_range::diagnose_bad_range(cerr << "got: ", r) << endl;
		cerr << "Error during resolution of indexed reference to:"
			<< endl;;
		_vals.dump(cerr << "\tcollection state: ",
			dump_flags::verbose) << endl;
		return bad_bool(true);
	}
	}

	// construct the range of aliases to collect
	const multikey_index_type lower(full_indices.lower_multikey());
	const multikey_index_type upper(full_indices.upper_multikey());
	// this will set the size and dimensions of packed_array a
	if (_vals.unroll_lvalue_references(lower, upper, a).bad) {
		cerr << "ERROR: unrolling aliases." << endl;
		return bad_bool(true);
	}
	// success!
	return bad_bool(false);
} else {
	STACKTRACE("is scalar");
	// is a scalar instance
	// size the alias_collection_type appropriately
	a.resize();             // empty
	const multikey_index_type bogus;
	if (_vals.unroll_lvalue_references(bogus, bogus, a).bad) {
		cerr << "ERROR: unrolling aliases." << endl;
		return bad_bool(true);
	}
	return bad_bool(false);
}
}

//=============================================================================
}	// end namepace entity
}	// end namepace HAC

#endif	// __HAC_OBJECT_REF_SIMPLE_META_VALUE_REFERENCE_TCC__

