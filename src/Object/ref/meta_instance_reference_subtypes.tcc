/**
	\file "Object/ref/meta_instance_reference_subtypes.tcc"
	$Id: meta_instance_reference_subtypes.tcc,v 1.16.4.4 2006/11/07 00:48:02 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_META_INSTANCE_REFERENCE_SUBTYPES_TCC__
#define	__HAC_OBJECT_REF_META_INSTANCE_REFERENCE_SUBTYPES_TCC__

#include <iostream>
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/ref/aggregate_meta_instance_reference.h"
#include "Object/module.tcc"
#include "Object/unroll/port_connection_base.h"
#include "Object/unroll/alias_connection.h"
#include "Object/unroll/unroll_context.h"
#include "Object/def/footprint.h"
#include "Object/type/fundamental_type_reference.h"
#include "Object/inst/physical_instance_collection.h"
#include "Object/expr/const_index_list.h"
#include "Object/expr/const_range_list.h"
#include "Object/expr/const_range.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/common/dump_flags.h"
#include "util/packed_array.tcc"	// for packed_array_generic<>::resize()
#include "util/stacktrace.h"
#include "Object/inst/alias_matcher.h"
#include "common/TODO.h"
#include "util/macros.h"

namespace HAC {
namespace entity {
using util::string_list;
#include "util/using_ostream.h"
//=============================================================================
// class meta_instance_reference method definitions

/**
	Implementation copied-modified from 
	simple_meta_indexed_reference_base::may_be_type_equivalent().
	Conservatively returns true.
	NOTE: does not attempt to reason about static dimensions of arrays.  
	TODO: be conservative w.r.t. aggregates?
 */
META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
bool
META_INSTANCE_REFERENCE_CLASS::may_be_type_equivalent(
		const meta_instance_reference_base& r) const {
	const this_type* rr(IS_A(const this_type*, &r));
	if (!rr) {
		// could be more descriptive...
		cerr << "Meta-class type does not match!  got: ";
		this->what(cerr) << " and: ";
		r.what(cerr) << endl;
		return false;
	}
	const count_ptr<const fundamental_type_reference>
		ltr(this->get_unresolved_type_ref());
	const count_ptr<const fundamental_type_reference>
		rtr(rr->get_unresolved_type_ref());
	const bool type_eq = ltr->may_be_connectibly_type_equivalent(*rtr);
	// if base types differ, then cannot be equivalent
	if (!type_eq) {
		ltr->dump(cerr << "Types do not match! got: ") << " and: ";
		rtr->dump(cerr) << "." << endl;
		return false;
	}
	// else they match, continue to check dimensionality and size.  

	// TO DO: factor this section code out to a method for re-use.  
	// note: is dimensions of the *reference* not the instantiation!
	const size_t lid = this->dimensions();
	const size_t rid = rr->dimensions();
	if (lid != rid) {
		cerr << "Dimensions do not match! got: " << lid
			<< " and: " << rid << "." << endl;
		return false;
	}
	// catch cases where one of them is scalar (zero-dimensional)
	if (!lid) {
		INVARIANT(!rid);
		return true;
	}
	// else fall-through handle multidimensional case
	return true;            // conservatively
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	First resolves canonical globally allocated index.  
	Accumulates all aliases by traversing instance hierarchy
	and recording matches.  
	\param sm the global state manager with globally allocated
		map of all unique instances.  
	\param aliases the string container in which to accumulate aliases.  
	\pre m module is already allocated ('alloc' phase).  
	\pre this must be a scalar, simple_meta_instance_reference type, 
		member-references are acceptable.  
 */
META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
META_INSTANCE_REFERENCE_CLASS::collect_aliases(const module& mod, 
		string_list& aliases) const {
	// assert dynamic_cast
	const simple_reference_type&
		_this(IS_A(const simple_reference_type&, *this));
	const size_t index = _this.lookup_globally_allocated_index(
		mod.get_state_manager(), 
		// temporary kludge until we clean up
		const_cast<footprint&>(mod.get_footprint()));
	INVARIANT(index);	// because we already checked reference?
	mod.template match_aliases<Tag>(aliases, index);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Collects all subnodes of this reference.  
	If this is scalar, just visit the one, 
	else if this is aggregate (e.g. array slice) then visit
	all instances in range.  
 */
META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
META_INSTANCE_REFERENCE_CLASS::collect_subentries(const module& mod, 
		entry_collection& v) const {
	const simple_reference_type&
		_this(IS_A(const simple_reference_type&, *this));
	const state_manager& sm(mod.get_state_manager());
	// temporary kludge until we clean up
	footprint& top(const_cast<footprint&>(mod.get_footprint()));
	if (_this.dimensions()) {
		vector<size_t> inds;
		if (!_this.lookup_globally_allocated_indices(sm, 
				top, inds).good) {
			// got error message already
			THROW_EXIT;
		}
		// else we're good
		vector<size_t>::const_iterator i(inds.begin()), e(inds.end());
		for ( ; i!=e; ++i) {
			sm.template collect_subentries<Tag>(v, *i);
		}
	} else {
		const size_t index =
			_this.lookup_globally_allocated_index(sm, top);
		INVARIANT(index);	// because we already checked reference?
		sm.template collect_subentries<Tag>(v, index);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	<sarcasm>
	Shamelessly copied from 
	simple_meta_indexed_reference_base::must_be_type_equivalent().
	</sarcasm>
	This is probably never called.  
 */
META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
bool
META_INSTANCE_REFERENCE_CLASS::must_be_type_equivalent(
		const meta_instance_reference_base& r) const {
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static function.  
	All this does is take an unrolled instance (collection) belonging
	to a top-level or of footprint and lookups up the member
	addressed by the indices.  
	No additional lookup is done on the instance argument, so it
	must already be translated to a top-level or footprint level, 
	as opposed to a definition-local placeholder.  
 */
META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
bad_bool
META_INSTANCE_REFERENCE_CLASS::unroll_references_packed_helper_no_lookup(
		const unroll_context& c,
		const collection_interface_type& inst,
		const count_ptr<const index_list_type>& ind,
		alias_collection_type& a) {
	STACKTRACE_VERBOSE;
if (inst.get_dimensions()) {
	STACKTRACE("is array");
	const_index_list cil;
	if (ind) {
		cil = ind->unroll_resolve_indices(c);
		if (cil.empty()) {
			cerr << "ERROR: Failed to resolve indices at "
				"unroll-time!" << endl;
			return bad_bool(true);
		}
	}
#if ENABLE_STACKTRACE
	cil.dump(STACKTRACE_INDENT << "given indices: ", 
		expr_dump_context::default_value) << endl;
#endif
	// else empty, implicitly refer to whole collection if it is dense
	// we have resolve constant indices
	const const_index_list
		full_indices(inst.resolve_indices(cil));
	if (full_indices.empty()) {
		// might fail because implicit slice reference is not packed
		cerr << "ERROR: failed to resolve implicit indices from "
			"a collection whose subarray is not dense."  << endl;
		cil.dump(inst.dump_hierarchical_name(
				cerr << "\tindices referenced: ",
				dump_flags::verbose),
			expr_dump_context::default_value) << endl;
		inst.dump(cerr << "\tcollection state: ", dump_flags::verbose)
			<< endl;
		// inst.dump_unrolled_instances(cerr, dump_flags::verbose);
		return bad_bool(true);
	}
#if ENABLE_STACKTRACE
	full_indices.dump(STACKTRACE_INDENT << "expanded: ",
		expr_dump_context::default_value) << endl;
#endif
	// resize the array according to the collapsed dimensions, 
	// before passing it to unroll_aliases.
	{
	const const_range_list
		crl(full_indices.collapsed_dimension_ranges());
#if ENABLE_STACKTRACE
	crl.dump(STACKTRACE_INDENT << "range: ", 
		expr_dump_context::default_value) << endl;
#endif
	try {
		const multikey_index_type
			array_sizes(crl.resolve_sizes());
		a.resize(array_sizes);
		// a.resize(upper -lower +ones);
	} catch (const_range_list::bad_range r) {
		const_range::diagnose_bad_range(cerr << "got: ", r) << endl;
		cerr << "Error during resolution of indexed reference to:"
			<< endl;;
		inst.dump(cerr << "\tcollection state: ",
			dump_flags::verbose) << endl;
		return bad_bool(true);
	}
	}

	// construct the range of aliases to collect
	const multikey_index_type lower(full_indices.lower_multikey());
	const multikey_index_type upper(full_indices.upper_multikey());
	// this will set the size and dimensions of packed_array a
	if (inst.unroll_aliases(lower, upper, a).bad) {
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
	if (inst.unroll_aliases(bogus, bogus, a).bad) {
		cerr << "ERROR: unrolling aliases." << endl;
		return bad_bool(true);
	}
	return bad_bool(false);
}
}	// end method unroll_references_packed_helper_no_lookup

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Re-usable helper function, but do NOT call from 
		member_meta_instance_reference.
	TODO: what about global instance references?
	\param _inst a resolved actual instance (not formal).  
	Called by simple_meta_instance_reference unroll_references_packed.
	This uses the footprint of the context to perform a lookup
	translation of the definition instance to the footprint instance.  
 */
META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
bad_bool
META_INSTANCE_REFERENCE_CLASS::unroll_references_packed_helper(
		const unroll_context& c,
		const instance_placeholder_type& _inst,
		const count_ptr<const index_list_type>& ind,
		alias_collection_type& a) {
	STACKTRACE_VERBOSE;
	const never_ptr<physical_instance_collection>
		inst_p(c.lookup_instance_collection(_inst));
	NEVER_NULL(inst_p);
	// assert dynamic_cast
	const collection_interface_type&
		inst(IS_A(const collection_interface_type&, *inst_p));
	return unroll_references_packed_helper_no_lookup(c, inst, ind, a);
}	// end method unroll_references_packed_helper

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Expand both the port and the instance reference (this) into
		alias_collection_types, populated by resolved aliases, 
		and then connect them all if dimensions match.  
	NOTE: this need not be virtual because it already calls
		this->unroll_reference, which is virtual, and will give
		the desired result.  
	\param cl is a port member, a reference to a collection.
		Since c is a port, it must be densely packed if it's an array.
		NOTE: this is already resolved because it was passed in
		from subinstance_manager::connect_ports(), 
		which iterates over the direct ports lists.
		No lookup should be necssary.  
	\param c the unroll context.
 */
META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
bad_bool
META_INSTANCE_REFERENCE_CLASS::connect_port(
		physical_instance_collection& cl, 
		const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	// assert checked-cast, will throw bad_cast upon error
	collection_interface_type&
		coll(IS_A(collection_interface_type&, cl));

	alias_collection_type this_aliases;
	const bad_bool unroll_err(this->unroll_references_packed(c, this_aliases));
		// calls unroll_reference virtually, thus
		// automatically handling member instance references.   
		// will automatically size the array
	if (unroll_err.bad) {
		cerr << "ERROR unrolling port actual reference "
			"during port connection: ";
		this->dump(cerr, expr_dump_context::default_value) << endl;
		return bad_bool(true);
	}

	alias_collection_type port_aliases;
	// bug fixed here: 20060124 (fangism)
	// see comment: we can just use simplified helper function
	const bad_bool port_err(unroll_references_packed_helper_no_lookup(
		c, coll, 
		count_ptr<const index_list_type>(NULL), 
		port_aliases));
	if (port_err.bad) {
		cerr << "ERROR unrolling member instance reference "
			"during port connection: ";
		coll.dump(cerr, dump_flags::verbose) << endl;
		return bad_bool(true);
	}
	typedef typename alias_collection_type::key_type        key_type;
	const key_type t_size(this_aliases.size());
	const key_type p_size(port_aliases.size());
	if (t_size != p_size) {
		cerr << "ERROR sizes mismatch in port connection: " << endl;
		cerr << "\texpected: " << p_size << endl;
		cerr << "\tgot: " << t_size << endl;
		typedef typename alias_collection_type::const_iterator
								const_iterator;
		cerr << "\texternal references: ";
		const_iterator ti(this_aliases.begin());
		const const_iterator te(this_aliases.end());
		for ( ; ti!=te; ++ti) {
			(*ti)->dump_hierarchical_name(cerr) << ", ";
		}
		cerr << endl;
		cerr << "\tlocal port references: ";
		const_iterator pi(port_aliases.begin());
		const const_iterator pe(port_aliases.end());
		for ( ; pi!=pe; ++pi) {
			(*pi)->dump_hierarchical_name(cerr) << ", ";
		}
		cerr << endl;
		return bad_bool(true);
	}
	// else attempt to make connections, type-checking along the way
	typedef typename alias_collection_type::iterator        alias_iterator;
	alias_iterator li(this_aliases.begin());
	const alias_iterator le(this_aliases.end());
	alias_iterator ri(port_aliases.begin());
	// the following copied from alias_connection::unroll's do-loop
	for ( ; li!=le; li++, ri++) {
		const never_ptr<instance_alias_info_type> lp(*li);
		const never_ptr<instance_alias_info_type> rp(*ri);
		NEVER_NULL(lp);
		NEVER_NULL(rp);
		if (!instance_alias_info_type::checked_connect_port(
				*lp, *rp).good) {
			// already have error message
			return bad_bool(true);
		}
	}
	INVARIANT(ri == port_aliases.end());
	return bad_bool(false);
}	// end method connect_port

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
meta_instance_reference_base::alias_connection_ptr_type
META_INSTANCE_REFERENCE_CLASS::make_aliases_connection_private(void) const {
	return alias_connection_ptr_type(new alias_connection_type);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<aggregate_meta_instance_reference_base>
META_INSTANCE_REFERENCE_CLASS::make_aggregate_meta_instance_reference_private(
		void) const {
	typedef	count_ptr<aggregate_meta_instance_reference_base>
							return_type;
	typedef	aggregate_meta_instance_reference<Tag>	aggregate_type;
	return return_type(new aggregate_type);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_META_INSTANCE_REFERENCE_SUBTYPES_TCC__

