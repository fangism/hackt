// "art_object_inst_ref.cc"
// for the instance_reference family of objects

#include <iostream>

#include "multidimensional_sparse_set.h"

// #include "art_parser_debug.h"		// need this?
#include "art_parser_base.h"
#include "art_symbol_table.h"
#include "art_object_instance.h"
#include "art_object_inst_ref.h"
#include "art_object_expr.h"
#include "art_built_ins.h"

//=============================================================================
// DEBUG OPTIONS -- compare to MASTER_DEBUG_LEVEL from "art_debug.h"

//=============================================================================
namespace ART {
namespace entity {

//=============================================================================
// class simple_instance_reference method definitions

simple_instance_reference::simple_instance_reference(
		excl_ptr<index_list> i, 
		const instantiation_state& st) :
		array_indices(i), 
		inst_state(st) {
	// in sub-type constructors, 
	// assert(array_indices->size < get_inst_base->dimensions());
}

simple_instance_reference::~simple_instance_reference() {
}

/**
	Dimensionality of an indexed references depends on
	which indexed dimensions are collapsed x[i], and which
	are not x[i..j].  
	A fully collapsed index list is equivalent to a 0-d array
	or a single instance.  
	\return the dimensions of the referenced array.  
 */
size_t
simple_instance_reference::dimensions(void) const {
	// THIS NEEDS FIXING
	if (array_indices)
		return array_indices->dimensions();
	else return get_inst_base()->dimensions();
}

ostream&
simple_instance_reference::dump(ostream& o) const {
	o << get_inst_base()->get_name();
	if (array_indices) {
		array_indices->dump(o);
	}
	return o;
}

string
simple_instance_reference::hash_string(void) const {
	string ret(get_inst_base()->get_qualified_name());
	if (array_indices) {
		ret += array_indices->hash_string();
	}
	return ret;
}

/**
	Adds indices to instance reference.  
	Must check with number of number of dimensions.  
	(Should we allow under-specification of dimensions?
		i.e. x[i] of a 2-D array to be a 1D array reference.)
	Then referring to x is referring to the entire array of x.  
	\return true if successful, else false.  
 */
bool
simple_instance_reference::attach_indices(excl_ptr<index_list> i) {
	// make sure not already indexed
	// side note: if indexing were truly recursive and not list-based, 
	//	we'd be able to append indices one-by-one.  
	assert(!array_indices);
	assert(i);
	// dimension-check:
	const never_const_ptr<instantiation_base> inst_base(get_inst_base());
	// number of indices must be <= dimension of instance collection.  
	const size_t max_dim = inst_base->dimensions();
	if (i->size() > max_dim) {
		cerr << "ERROR: instance collection " << inst_base->get_name()
			<< " is " << max_dim << "-dimensional, and thus, "
			"cannot be indexed " << i->size() <<
			"-dimensionally!  ";
			// caller will say where
		return false;
	} else {
		// allow under-specified dimensions?  yeah for now...
		// if indices are constant, check for total overlap
		// with existing instances from the point of reference.

		typedef base_multidimensional_sparse_set<int, const_range>
								mset_base;
		// overriding default implementation with pair<int, int>
		assert(max_dim <= mset_base::LIMIT);
		never_const_ptr<index_list> il(i);
		never_const_ptr<const_index_list>
			cil(il.is_a<const_index_list>());
		if (!cil)	// is dynamic, conservatively covers anything
			return false;
		// else is constant index list, can compute coverage
		//	using multidimensional_sparse_set
		instantiation_state iter = inst_state;
		const instantiation_state
			end(inst_base->collection_state_end());
		excl_ptr<mset_base> cov(
			mset_base::make_multidimensional_sparse_set(max_dim));
		assert(cov);
		{
			const_range_list crl(*cil);
			cov->add_ranges(crl);
		}
		for ( ; iter!=end; iter++) {
			if (iter->is_a<dynamic_range_list>()) {
				// all we can do conservatively...
				cov->clear();
				// empty means indices have been covered
				break;
			} else {
				count_const_ptr<const_range_list>
					crlp(iter->is_a<const_range_list>());
				assert(crlp);
				cov->delete_ranges(*crlp);
			}
		}
		// if this point reached, then all instance additions
		// were static constants.
		// now, covered set must completely contain indices
		if (!cov->empty()) {
			// empty means covered.  
			cerr << "ERROR: The following referenced indices "
				"have definitely not been instantiated: {";
			cov->dump(cerr << endl) << "} ";
			// cerr << where() << endl;	// caller
			// fancy: list indices not instantiated?
			return false;
		}
		array_indices = i;
		return true;
	}
}

//=============================================================================
#if 0
PHASE IN later...
// class collective_instance_reference method definitions

collective_instance_reference::collective_instance_reference(
		never_const_ptr<instance_reference_base> b, 
		const param_expr* l, const param_expr* r) :
		instance_reference_base(), 
		lower_index(never_const_ptr<param_expr>(l)),
		upper_index(never_const_ptr<param_expr>(r)) {
}

collective_instance_reference::~collective_instance_reference() {
}

ostream&
collective_instance_reference::what(ostream& o) const {
	return o << "collective-inst-ref";
}

ostream&
collective_instance_reference::dump(ostream& o) const {
	return what(o);
}

string
collective_instance_reference::hash_string(void) const {
	string ret(base_array->hash_string());
	ret += "[";
	ret += lower_index->hash_string();
	if (upper_index) {
		ret += "..";
		ret += upper_index->hash_string();
	}
	ret += "]";
	return ret;
}
#endif

//=============================================================================
// class param_instance_reference method definitions

/**
	\param pi needs to be modifiable for later initialization.  
 */
param_instance_reference::param_instance_reference(
		excl_ptr<index_list> i, 
		const instantiation_state& st) :
		simple_instance_reference(i, st) {
}

/**
	For single instances references, check whether it is initialized.  
	For collective instance references, and indexed references, 
	just conservatively say that it hasn't been initialized yet; 
	so don't bother checking until unroll time.  
 */
bool
param_instance_reference::is_initialized(void) const {
	never_const_ptr<instantiation_base> i(get_inst_base());
	assert(i);
	if (i->dimensions() > 0)
		return false;
	else 
		return i.is_a<param_instantiation>()->is_initialized();
}

/**
	Under what conditions is a reference to a param instance
	static and constant?
	Resolved to a compile-time constant value.  
	May refer to parameters that are unconditionally initialized
	exactly once.  
	For collective variables, we just conservatively return false.  
	Oh yeah, how do we keep track whether or not a variable
	has been conditionally initialized?
	We may need some assignment stack.... PUNT!
 */
bool
param_instance_reference::is_static_constant(void) const {
	// "collective": if either reference is indexed, 
	// 	(mind, this is conservative and not precise because
	//	we don't track values of arrays at compile-time)
	//	More thoughts later on how to be more precise...
	if (array_indices)
		return false;
	// or the instantiation_base is collective (not 0-dimensional)
	else if (get_inst_base()->dimensions())
		return false;
	// else if singular, whether or not it is initialized once
	// to another static constant (parameter variable or value).  
	else 
		return get_param_inst_base()->is_static_constant();
}

/**
	A reference is loop independent if it's indices
	contain no references to index loop variables.  
	Or if it's simply not in a loop, since references are type-checked.  
	BEWARE: referring to a collective instance without indexing
	can also be loop-variant if instances are added to the collection
	within some loop.  
	See if the iterator into instance_collection_stack points
	to a loop-scope.  
	(Of course, we need to actually use the instance_collection stack...)
 */
bool
param_instance_reference::is_loop_independent(void) const {
	if (array_indices)
		return array_indices->is_loop_independent();
	else 
		// no array indices, see if instantiation_base is collective
	if (get_inst_base()->dimensions()) {
		// if collective, check if the instance_reference itself 
		// is found within a loop that adds to the collection...
		// ... but I'm too lazy to do this entirely now
		return false;
		// FIX ME later
	} else {
		// is 0-dimension, look up and see if it happens to be
		// a loop index variable.  
		// Who owns the param_inst_base?
		never_const_ptr<scopespace>
			owner(get_inst_base()->get_owner());
		return !owner.is_a<loop_scope>();
	}
}

/**
	Whether or not this instance reference is found inside a 
	conditional body.  
	Check the iterator into the instantiation base's
	collection stack.  
	Is point of reference in some conditional body
		(may be nested in loop, don't forget!)
	What about the indices themselves?  (shouldn't need to check?)
 */
bool
param_instance_reference::is_unconditional(void) const {
	if (array_indices)
		return array_indices->is_unconditional();
	// else see if point of reference is within some conditional scope
	else if (get_inst_base()->dimensions()) {
		// if collective, see if the instance_reference itself
		// is found within a conditional body, by walking the 
		// collection_state_iterator forward.
		// ... but I'm too lazy to do this now, FIX ME later
		// what about checking history of assignments???
		//	implies assignment stack...
		return false;		// extremely conservative
	} else {
		// also need to check assignment stack...
		return true;
	}
}

//=============================================================================
#if 0
// moved to "art_object_expr.cc"
// class pbool_instance_reference method definitions

pbool_instance_reference::pbool_instance_reference(
		never_ptr<pbool_instantiation> pi,
		excl_ptr<index_list> i) :
		param_instance_reference(i, pi->current_collection_state()),
		pbool_inst_ref(pi) {
}

never_const_ptr<instantiation_base>
pbool_instance_reference::get_inst_base(void) const {
	return pbool_inst_ref;
}

never_const_ptr<param_instantiation>
pbool_instance_reference::get_param_inst_base(void) const {
	return pbool_inst_ref;
}

ostream&
pbool_instance_reference::what(ostream& o) const {
	return o << "pbool-inst-ref";
}

bool
pbool_instance_reference::initialize(count_const_ptr<param_expr> i) {
	return pbool_inst_ref->initialize(i.is_a<pbool_expr>());
}

/**
	\return newly constructed pbool literal if successful, 
		returns NULL if type mismatches.  
 */
count_ptr<param_expr>
pbool_instance_reference::make_param_literal(
		count_ptr<param_instance_reference> pr) {
	// make sure passed pointer is a self-ref count
	assert(pr == this);
	count_ptr<pbool_instance_reference> br(
		pr.is_a<pbool_instance_reference>());
	if (br)	return count_ptr<param_expr>(new pbool_literal(br));
	else	return count_ptr<param_expr>(NULL);
}

//=============================================================================
// class pint_instance_reference method definitions

pint_instance_reference::pint_instance_reference(
		never_ptr<pint_instantiation> pi,
		excl_ptr<index_list> i) :
		param_instance_reference(i, pi->current_collection_state()),
		pint_inst_ref(pi) {
}

never_const_ptr<instantiation_base>
pint_instance_reference::get_inst_base(void) const {
	return pint_inst_ref;
}

never_const_ptr<param_instantiation>
pint_instance_reference::get_param_inst_base(void) const {
	return pint_inst_ref;
}

ostream&
pint_instance_reference::what(ostream& o) const {
	return o << "pint-inst-ref";
}

bool
pint_instance_reference::initialize(count_const_ptr<param_expr> i) {
	return pint_inst_ref->initialize(i.is_a<pint_expr>());
}

/**
	\return newly constructed pint literal if successful, 
		returns NULL if type mismatches.  
 */
count_ptr<param_expr>
pint_instance_reference::make_param_literal(
		count_ptr<param_instance_reference> pr) {
	// make sure passed pointer is a self-ref count
	assert(pr == this);
	count_ptr<pint_instance_reference> ir(
		pr.is_a<pint_instance_reference>());
	if (ir)	return count_ptr<param_expr>(new pint_literal(ir));
	else	return count_ptr<param_expr>(NULL);
}
#endif

//=============================================================================
// class process_instance_reference method definitions

process_instance_reference::process_instance_reference(
		const process_instantiation& pi,
		excl_ptr<index_list> i) :
		simple_instance_reference(i, pi.current_collection_state()),
		process_inst_ref(&pi) {
}

process_instance_reference::~process_instance_reference() {
}

never_const_ptr<instantiation_base>
process_instance_reference::get_inst_base(void) const {
	return process_inst_ref;
}

ostream&
process_instance_reference::what(ostream& o) const {
	return o << "process-inst-ref";
}

//=============================================================================
// class datatype_instance_reference method definitions

datatype_instance_reference::datatype_instance_reference(
		const datatype_instantiation& di,
		excl_ptr<index_list> i) :
		simple_instance_reference(i, di.current_collection_state()),
		data_inst_ref(&di) {
}

datatype_instance_reference::~datatype_instance_reference() {
}

never_const_ptr<instantiation_base>
datatype_instance_reference::get_inst_base(void) const {
	return data_inst_ref;
}

ostream&
datatype_instance_reference::what(ostream& o) const {
	return o << "datatype-inst-ref";
}

ostream&
datatype_instance_reference::dump(ostream& o) const {
	return what(o);
}

//=============================================================================
// class channel_instance_reference method definitions

channel_instance_reference::channel_instance_reference(
		const channel_instantiation& ci,
		excl_ptr<index_list> i) :
		simple_instance_reference(i, ci.current_collection_state()),
		channel_inst_ref(&ci) {
}

channel_instance_reference::~channel_instance_reference() {
}

never_const_ptr<instantiation_base>
channel_instance_reference::get_inst_base(void) const {
	return channel_inst_ref;
}

ostream&
channel_instance_reference::what(ostream& o) const {
	return o << "channel-inst-ref";
}

ostream&
channel_instance_reference::dump(ostream& o) const {
	return what(o);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

