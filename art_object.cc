// "art_object.cc"

#include <iostream>

#include "art_parser_debug.h"
#include "art_parser_base.h"

// CAUTION on ordering of the following two include files!
// including "art_object.h" first will cause compiler to complain
// about redefinition of struct hash<> template upon specialization of
// hash<string>.  

#include "hash_specializations.h"		// substitute for the following

#include "art_object_base.h"
#include "art_object_expr.h"
#include "art_object_connect.h"

//=============================================================================
// DEBUG OPTIONS -- compare to MASTER_DEBUG_LEVEL from "art_debug.h"

#define		DEBUG_NAMESPACE			1 && DEBUG_CHECK_BUILD
#define		TRACE_QUERY			0	// bool, ok to change

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if		DEBUG_NAMESPACE
// ok to change these values, but should be > TRACE_CHECK_BUILD (5)
  #define	TRACE_NAMESPACE_NEW		8
  #define	TRACE_NAMESPACE_USING		8
  #define	TRACE_NAMESPACE_ALIAS		8
  #if		TRACE_QUERY
    #define	TRACE_NAMESPACE_QUERY		10
    #define	TRACE_NAMESPACE_SEARCH		TRACE_NAMESPACE_QUERY+5
  #else
    #define	TRACE_NAMESPACE_QUERY		MASTER_DEBUG_LEVEL
    #define	TRACE_NAMESPACE_SEARCH		MASTER_DEBUG_LEVEL
  #endif
#else
// defining as >= MASTER_DEBUG_LEVEL will turn it off
  #define	TRACE_NAMESPACE_QUERY		MASTER_DEBUG_LEVEL
  #define	TRACE_NAMESPACE_SEARCH		MASTER_DEBUG_LEVEL
  #define	TRACE_NAMESPACE_USING		MASTER_DEBUG_LEVEL
  #define	TRACE_NAMESPACE_ALIAS		MASTER_DEBUG_LEVEL
#endif


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if		DEBUG_DATATYPE && TRACE_QUERY
// ok to change these values
  #define	TRACE_DATATYPE_QUERY		10
  #define	TRACE_DATATYPE_SEARCH		TRACE_DATATYPE_QUERY+5
#else
// defining as >= MASTER_DEBUG_LEVEL will turn it off
  #define	TRACE_DATATYPE_QUERY		MASTER_DEBUG_LEVEL
  #define	TRACE_DATATYPE_SEARCH		MASTER_DEBUG_LEVEL
#endif


//=============================================================================
namespace ART {
namespace entity {

//=============================================================================
// general non-member function definitions

//=============================================================================
// class object_handle method definitions

ostream&
object_handle::what(ostream& o) const {
	return obj.what(o);
}

ostream&
object_handle::dump(ostream& o) const {
	return obj.dump(o << "(handle)");
}

//=============================================================================
// class object_list method definitions

object_list::object_list() : object(), parent() {
}

object_list::~object_list() {
}

ostream&
object_list::what(ostream& o) const {
	return o << "object-list";
}

ostream&
object_list::dump(ostream& o) const {
	what(o) << ":" << endl;
	parent::const_iterator i = begin();
	for ( ; i!=end(); i++)
		dump(o) << endl;
	return o;
}

/**
	Creates an instance_collection_item.  
	The restriction with this version is that each item must
	be a single pint_expr, not a range.  
	Walks list twice, once to see what the best classification is, 
	again to copy-create.  
	Intended for use in resolving dense array dimensions
	of formal parameters and formal ports.  
	Cannot possibly be loop-dependent or conditional!
 */
count_ptr<range_expr_list>
object_list::make_formal_dense_range_list(void) const {
	// initialize some bools to true
	// and set them false approriately in iterations
	bool err = false;
	bool is_pint_expr = true;
	bool is_static_constant = true;
	bool is_initialized = true;
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		count_ptr<pint_expr> p(i->is_a<pint_expr>());
		if (!p) {
			is_pint_expr = false;
			cerr << "non-int expression found where single int "
				"is expected in dense range declaration.  "
				"ERROR!  " << endl;	// where?
			err = true;
		} else {
			if (p->dimensions() != 0) {
				cerr << "int expression must be 0-dimensional, "
					"but is actually " <<
					p->dimensions() << "-dimensional.  "
					"ERROR!  " << endl;
				err = true;
			}
			if (p->is_static_constant()) {
				continue;
			} else {
				is_static_constant = false;
			}
			if (p->may_be_initialized()) {
				continue;
			} else {
				is_initialized = false;
				// not initialized! error.  
				cerr << "int expression is not initialized.  "
					"ERROR!  " << endl;	// where?
				err = true;
			}
			// can it be initialized, but non-const?
			// yes, if a dimension depends on another formal param
			assert(p->is_loop_independent());
			assert(p->is_unconditional());
		}
	}
	if (err) {
		cerr << "Failed to construct a formal dense range list!  "
			<< endl;
		return count_ptr<range_expr_list>(NULL);
	} else if (is_static_constant) {
		const_iterator j = begin();
		count_ptr<const_range_list> ret(new const_range_list);
		for ( ; j!=end(); j++) {
			// should be safe to do this, since we checked above
			const int n = j->is_a<pint_expr>()
				->static_constant_int();
			if (n <= 0) {
				cerr << "Integer size for a dense array "
					"must be positive, but got: "
					<< n << ".  ERROR!  " << endl;
				// where? callers figure out...
				return count_ptr<const_range_list>(NULL);
			}
			ret->push_back(const_range(n));
		}
		return ret;
	} else if (is_initialized) {
		const_iterator j = begin();
		count_ptr<dynamic_range_list> ret(new dynamic_range_list);
		for ( ; j!=end(); j++) {
			// should be safe to do this, since we checked above
			ret->push_back(count_ptr<pint_range>(
				new pint_range(j->is_a<pint_expr>())));
		}
		return ret;
	} else {
		cerr << "Failed to construct a formal dense range list!  "
			<< endl;
		return count_ptr<range_expr_list>(NULL);
	}
}

/**
	Creates an instance_collection_item.  
	No restriction on this, may be single integer or pint_range.  
	Will automatically convert to ranges.
	Walks list twice, once to see what the best classification is, 
	again to copy-create.  
	Intended for use in resolving dense array dimensions
	of formal parameters and formal ports.  
	Cannot possibly be loop-dependent or conditional!
 */
count_ptr<range_expr_list>
object_list::make_sparse_range_list(void) const {
	// initialize some bools to true
	// and set them false approriately in iterations
	bool err = false;
	bool is_valid_range = true;
	bool is_static_constant = true;
	bool is_initialized = true;
	const_iterator i = begin();
	int k = 1;
	for ( ; i!=end(); i++, k++) {
		if (!*i) {
			cerr << "Error in dimension " << k <<
				" of array indices.  " << endl;
			continue;
		}
		count_ptr<pint_expr> p(i->is_a<pint_expr>());
		// may be pint_const or pint_literal
		count_ptr<range_expr> r(i->is_a<range_expr>());
		// may be const_range or pint_range
		if (p) {
			// later modularize to some method function...
			if (p->dimensions() != 0) {
				cerr << "int expression must be 0-dimensional, "
					"but is actually " <<
					p->dimensions() << "-dimensional.  "
					"ERROR!  " << endl;
				err = true;
			}
			if (p->is_static_constant()) {
				continue;
			} else {
				is_static_constant = false;
			}
			if (p->may_be_initialized()) {	// definite
				continue;
			} else {
				is_initialized = false;
				// not initialized! error.  
				cerr << "int expression is definitely "
					"not initialized.  ERROR!  "
					<< endl;	// where?
				err = true;
			}
			// can it be initialized, but non-const?
			// yes, if a dimension depends on another formal param
			// can be loop-independent, do we need to track?
			// can be conditional, do we need to track?
		} else if (r) {
			// same thing... copy
			if (r->dimensions() != 0) {
				cerr << "range expression must be 0-dimensional, "
					"but is actually " <<
					r->dimensions() << "-dimensional.  "
					"ERROR!  " << endl;
				err = true;
			}
			if (r->is_static_constant()) {
				continue;
			} else {
				is_static_constant = false;
			}
			if (r->may_be_initialized()) {	// definite
				continue;
			} else {
				is_initialized = false;
				// not initialized! error.  
				cerr << "range expression is definitely "
					"not initialized.  ERROR!  "
					<< endl;	// where?
				err = true;
			}
		} else {
			// is neither pint_expr nor range_expr
			assert(!i->is_a<pint_const>());
			assert(!i->is_a<const_range>());
			is_valid_range = false;
			(*i)->what(cerr << "Expected integer or range "
				"expression but got a ") << 
				" in dimension " << k << " of array ranges.  "
				"ERROR!  " << endl;	// where?
			err = true;
		}
	}
	if (err || !is_valid_range) {
		cerr << "Failed to construct a sparse range list!  "
			<< endl;
		return count_ptr<range_expr_list>(NULL);
	} else if (is_static_constant) {
		const_iterator j = begin();
		count_ptr<const_range_list> ret(new const_range_list);
		for ( ; j!=end(); j++) {
			// should be safe to do this, since we checked above
			count_ptr<pint_expr> p(j->is_a<pint_expr>());
			count_ptr<range_expr> r(j->is_a<range_expr>());
			// don't forget to range check
			if (p) {
				const int n = p->static_constant_int();
				if (n <= 0) {
					cerr << "Integer size for a dense array "
						"must be positive, but got: "
						<< n << ".  ERROR!  " << endl;
					// where? let caller figure out
					return count_ptr<const_range_list>(NULL);
				}
				ret->push_back(const_range(n));
			} else {
				assert(r);
				if (!r->is_sane()) {
					cerr << "Range is not valid. ERROR!  "
						<< endl;
					return count_ptr<const_range_list>(NULL);
				}
				count_ptr<const_range> cr(
					r.is_a<const_range>());
				if (cr) {
					// need deep copy, b/c not pointer list
					ret->push_back(const_range(*cr));
				} else {
					count_ptr<pint_range> pr(
						r.is_a<pint_range>());
					assert(pr);
					assert(pr->is_static_constant());
					ret->push_back(
						pr->static_constant_range());
				}
			}
		}
		return ret;
	} else if (is_initialized) {
		const_iterator j = begin();
		count_ptr<dynamic_range_list> ret(new dynamic_range_list);
		for ( ; j!=end(); j++) {
			if (j->is_a<pint_expr>()) {
				// convert N to 0..N-1
				ret->push_back(count_ptr<pint_range>(
					new pint_range(j->is_a<pint_expr>())));
			} else {
				assert(j->is_a<pint_range>());
				ret->push_back(j->is_a<pint_range>());
			}
		}
		return ret;
	} else {
		cerr << "Failed to construct a sparse range list!  "
			<< endl;
		return count_ptr<range_expr_list>(NULL);
	}
}

/**
	First half should be similar to make_sparse_range_list.  
	At the end build an index_list instead.  
 */
excl_ptr<index_list>
object_list::make_index_list(void) const {
	// initialize some bools to true
	// and set them false approriately in iterations
	bool err = false;
	bool is_valid_index = true;
	bool is_static_constant = true;
	bool is_initialized = true;
	const_iterator i = begin();
	int k = 1;
	for ( ; i!=end(); i++, k++) {
		if (!*i) {
			cerr << "Error in dimension " << k <<
				" of array indices.  " << endl;
			continue;
		}
		count_ptr<pint_expr> p(i->is_a<pint_expr>());
		// may be pint_const or pint_literal
		count_ptr<range_expr> r(i->is_a<range_expr>());
		// may be const_range or pint_range
		if (p) {
			// later modularize to some method function...
			if (p->dimensions() != 0) {
				cerr << "int expression must be 0-dimensional, "
					"but is actually " <<
					p->dimensions() << "-dimensional.  "
					"ERROR!  " << endl;
				err = true;
			}
			if (p->is_static_constant()) {
				continue;
			} else {
				is_static_constant = false;
			}
			if (p->may_be_initialized()) {	// definite
				continue;
			} else {
				is_initialized = false;
				// not initialized! error.  
				cerr << "int expression is definitely "
					"not initialized.  ERROR!  "
					<< endl;	// where?
				err = true;
			}
			// can it be initialized, but non-const?
			// yes, if a dimension depends on another formal param
			// can be loop-independent, do we need to track?
			// can be conditional, do we need to track?
		} else if (r) {
			// same thing... copy
			if (r->dimensions() != 0) {
				cerr << "range expression must be 0-dimensional, "
					"but is actually " <<
					r->dimensions() << "-dimensional.  "
					"ERROR!  " << endl;
				err = true;
			}
			if (r->is_static_constant()) {
				continue;
			} else {
				is_static_constant = false;
			}
			if (r->may_be_initialized()) {	// definite
				continue;
			} else {
				is_initialized = false;
				// not initialized! error.  
				cerr << "range expression is definitely "
					"not initialized.  ERROR!  "
					<< endl;	// where?
				err = true;
			}
		} else {
			// is neither pint_expr nor range_expr
			assert(!i->is_a<pint_const>());
			assert(!i->is_a<const_range>());
			is_valid_index = false;
			(*i)->what(cerr << "Expected integer or range "
				"expression but got a ") << 
				" in dimension " << k << " of array ranges.  "
				"ERROR!  " << endl;	// where?
			err = true;
		}
	}
	if (err || !is_valid_index) {
		cerr << "Failed to construct an index list!  "
			<< endl;
		return excl_ptr<index_list>(NULL);
	} else if (is_static_constant) {
		const_iterator j = begin();
		excl_ptr<const_index_list> ret(new const_index_list);
		for ( ; j!=end(); j++) {
			// should be safe to do this, since we checked above
			count_ptr<pint_expr> p(j->is_a<pint_expr>());
			count_ptr<range_expr> r(j->is_a<range_expr>());
			// don't forget to range check
			if (p) {
				const int n = p->static_constant_int();
				if (n < 0) {
					cerr << "Integer for an index must "
						"be non-negative, but got: "
						<< n << ".  ERROR!  " << endl;
					// where? let caller figure out
					return excl_ptr<index_list>(NULL);
				}
				ret->push_back(count_ptr<pint_const>(
					new pint_const(n)));
			} else {
				assert(r);
				if (!r->is_sane()) {
					cerr << "Index-range is not valid.  "
						"ERROR!  " << endl;
					return excl_ptr<index_list>(NULL);
				}
				count_ptr<const_range> cr(
					r.is_a<const_range>());
				if (cr) {
					// need deep copy, b/c not pointer list
					ret->push_back(
						count_ptr<const_index>(
							new const_range(*cr)));
				} else {
					count_ptr<pint_range> pr(
						r.is_a<pint_range>());
					assert(pr);
					assert(pr->is_static_constant());
					ret->push_back(count_ptr<const_index>(
						new const_range(
						pr->static_constant_range())));
				}
			}
		}
		return excl_ptr<index_list>(ret);
	} else if (is_initialized) {
		const_iterator j = begin();
		excl_ptr<dynamic_index_list> ret(new dynamic_index_list);
		for ( ; j!=end(); j++) {
			if (j->is_a<pint_expr>()) {
				// convert N to 0..N-1
				// check dimensionality!
				ret->push_back(j->is_a<pint_expr>());
			} else {
				// check dimensionality!
				assert(j->is_a<pint_range>());
				ret->push_back(j->is_a<pint_range>());
			}
		}
		return excl_ptr<index_list>(ret);
	} else {
		cerr << "Failed to construct a index list!  "
			<< endl;
		return excl_ptr<index_list>(NULL);
	}
}

/**
	Converts a list into a param_expression_assignment object.  
	This is non-const because assignment requires that we 
	initialize parameter instance references, which requires 
	modification.  
	\return newly constructed param_expresion_assignment 
		if there are no errors, else NULL.  
		(May change with more sophisticated error-handling.)
 */
excl_ptr<param_expression_assignment>
object_list::make_param_assignment(void) {
	// then expect subsequent items to be the same
	// or already param_expr in the case of some constants.
	// However, only the last item may be a constant.
	excl_ptr<param_expression_assignment>
		ret(new param_expression_assignment);
	assert(ret);
					
	// Mark all but the last expression as initialized
	// to the right-most expression.
	// TO DO: FINISH THIS PART
	// rvalue = ...
	// make sure rvalue is validly initialized
	// i.e. is a constant or a formal parameter

	bool err = false;

	const_iterator last_obj = end();
	last_obj--;		// safe because list is not empty
	count_const_ptr<param_expr> rhse(last_obj->is_a<param_expr>());

	// compare with this to make sure dimensions match
	size_t last_dim = 666;	// initially garbage
	if (!*last_obj) {
		cerr << "ERROR: rhs of expression assignment "
			"is malformed (null)" << endl;
		err = true;
	} else if (rhse) {
		// last term must be initialized or be
		// dependent on formals
		// if collective, conservative: may-be-initialized
		if (!rhse->may_be_initialized()) {
			rhse->dump(cerr << "ERROR: rhs of expr-"
				"assignment is not initialized or "
				"dependent on formals: ") << endl;
			err = true;
			exit(1);		// temporary
		}
		last_dim = rhse->dimensions();
	} else {
		(*last_obj)->what(
			cerr << "ERROR: rhs is unexpected object: ") << endl;
		err = true;
	}

	if (err)
		return excl_ptr<param_expression_assignment>(NULL);
	assert(last_dim != 666);		// stupid sanity check

	size_t k = 0;
	iterator iter = begin();
		// needs to be modifiable for initialization
	for ( ; iter!=last_obj; iter++, k++) {	// all but last one
		// consider modularizing this loop
		bool for_err = false;
		count_ptr<param_expr> ir(iter->is_a<param_expr>());
		if (!*iter) {
			cerr << "ERROR: in creating item " << k+1 <<
				" of alias-list." << endl;
			for_err = true;
		} else if (ir) {
			// make this body into subroutine...
			// a single parameter instance reference
			// make sure not already initialized!
			if (ir->dimensions() != last_dim) {
				cerr << "ERROR: dimensions of expression " <<
					k+1 << " (" << ir->dimensions() <<
					") doesn't match that of the rhs (" <<
					last_dim << ")." << endl;
				for_err = true;
			}
			if (ir->must_be_initialized()) {
				// definitely initialized or formal
				cerr << "ERROR: expression " << k+1 <<
					"is already initialized!"
					<< endl;
				// don't care if it's same value...
				for_err = true;
			} else if (rhse) {
				// what to do about collective arrays?
				count_ptr<pbool_instance_reference>
					bir(ir.is_a<pbool_instance_reference>());
				count_ptr<pint_instance_reference>
					pir(ir.is_a<pint_instance_reference>());
				// gotta be one class or the other
				// check this after re-write
				assert(rhse);
				bool init_ret;
				if (bir)
					init_ret = bir->initialize(rhse);
				else {
					assert(pir);
					init_ret = pir->initialize(rhse);
				}
				if (!init_ret) {
					cerr << "Error initializing "
					"item " << k+1 << " of alias-"
					"list.  " << endl;
					for_err = true;
				}
//				assert(ir->is_initialized());
			} // else already error in rhse
			if (for_err)
				ret->append_param_expression(
					count_ptr<param_expr>(NULL));
			else ret->append_param_expression(ir);
		} else {
			// is reference to something else
			// or might be collective...
			// ERROR
			cerr << "ERROR: unhandled case for item "
				<< k+1 << " of alias-list." << endl;
			for_err = true;
		}
		if (for_err)
			err = for_err;
	}

	// if there are any errors, discard everything?
	// later: track errors in partially constructed objects
	if (err)
		return excl_ptr<param_expression_assignment>(NULL);
	else	return ret;		// is ok
}

/**
	Doesn't do any checking, just re-wraps the list.  
	Now does checking for static constants.  
	Objects may be null, but all others must be param_expr.  
	For now always return a dynamic list.
 */
// excl_ptr<param_expr_list>
excl_ptr<dynamic_param_expr_list>
object_list::make_param_expr_list(void) const {
	// first walk to determine if it qualified as a const_param_expr_list
	const_iterator i = begin();
#if 0
	bool is_all_const = true;
	for ( ; i!=end(); i++) {
		count_const_ptr<object> o(*i);
		count_const_ptr<param_expr> pe(o.is_a<param_expr>());
		if (o) {
			assert(pe);
			if (!pe.is_a<const_param>())
				is_all_const = false;
		} else {
			is_all_const = false;
		}
	}
	
	i = begin();
	if (is_all_const) {
		excl_ptr<const_param_expr_list>
			ret(new const_param_expr_list);
		for ( ; i!=end(); i++) {
			count_const_ptr<object> o(*i);
			count_const_ptr<param_expr> pe(o.is_a<param_expr>());
			assert(pe);
			// can't be NULL, already checked above
			ret->push_back(pe.is_a<const_param>());
		}
		return ret.is_a_xfer<param_expr_list>();
	} else {
#endif
		excl_ptr<dynamic_param_expr_list>
			ret(new dynamic_param_expr_list);
		for ( ; i!=end(); i++) {
			count_const_ptr<object> o(*i);
			count_const_ptr<param_expr> pe(o.is_a<param_expr>());
			if (o)	assert(pe);
			ret->push_back(pe);	// NULL is ok.  
		}
		return ret;
#if 0
		return ret.is_a_xfer<param_expr_list>();
	}
#endif
}

/**
	Creates an alias connection object, given a list of instance
	references.  Performs type-checking.  
 */
excl_const_ptr<aliases_connection>
object_list::make_alias_connection(void) const {
	excl_ptr<aliases_connection>
		ret(new aliases_connection);
	const_iterator i = begin();
	assert(size() > 1);		// else what are you connecting?
	count_const_ptr<object> fo(*i);
	count_const_ptr<instance_reference_base>
		fir(fo.is_a<instance_reference_base>());
	// keep this around for type-checking comparisons
	ret->append_instance_reference(fir);
	// starting with second instance reference, type-check and alias
	int j = 2;
	for (i++; i!=end(); i++, j++) {
		count_const_ptr<object> o(*i);
		assert(o);
		count_const_ptr<instance_reference_base>
			ir(o.is_a<instance_reference_base>());
		assert(ir);
		if (!fir->may_be_type_equivalent(*ir)) {
			cerr << "ERROR: type/size of instance reference " 
				<< j << " of alias list doesn't match the "
				"type/size of the first instance reference!  "
				<< endl;
			return excl_const_ptr<aliases_connection>(NULL);
		} else {
			ret->append_instance_reference(ir);
		}
	}
	return excl_const_ptr<aliases_connection>(ret);	// const-ify
}

/**
	Creates a port connection object, given an invoking instance
	reference and a list of port actuals (instance references).
	\param ir the invoking instance to which port should connect.  
 */
excl_const_ptr<port_connection>
object_list::make_port_connection(
		count_const_ptr<simple_instance_reference> ir) const {
//	cerr << "In object_list::make_port_connection(): FINISH ME!" << endl;
	typedef	excl_const_ptr<port_connection>		return_type;
	excl_ptr<port_connection>
		ret(new port_connection(ir));
	never_const_ptr<definition_base>
		base_def(ir->get_base_def());

	const size_t ir_dim = ir->dimensions();
	if (ir_dim) {
		cerr << "Instance reference port connection must be scalar, "
			"but got a " << ir_dim << "-dim reference!" << endl;
		return return_type(NULL);
	} else if (base_def->certify_port_actuals(*this)) {
		const_iterator i = begin();
		for ( ; i!=end(); i++) {
			count_const_ptr<instance_reference_base>
				ir(i->is_a<instance_reference_base>());
			ret->append_instance_reference(ir);
		}
		return return_type(ret);
	} else {
		cerr << "At least one error in port connection.  " << endl;
		return return_type(NULL);
	}
}

//=============================================================================
// class scopespace method definitions
scopespace::scopespace() : object(),
		used_id_map(), connect_assign_list() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
scopespace::~scopespace() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic object lookup for unqualified identifier.  
	Doesn't care what sub-type the object actually is.  
	This variation only searches the current namespace, and 
	never searches the parents' scopes.  
	NOTE: MUST static_cast map as a const to guarantee
	that map will not be modified.  
	Can inline this.  
	\param id the unqualified name of the object sought.  
	\return an object with the same name, if found.  
 */
never_const_ptr<object>
scopespace::lookup_object_here(const string& id) const {
	return static_cast<const used_id_map_type&>(used_id_map)[id];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Same as regular map lookup, but returning a modifiable
	pointer.  
 */
never_ptr<object>
scopespace::lookup_object_here_with_modify(const string& id) const {
	return static_cast<const used_id_map_type&>(used_id_map)[id];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic object lookup for unqualified identifier.  
	Doesn't care what sub-type the object actually is.  
	The variation also queries parents's namespace and returns
	the first match found from an upward search.  
	(Consider making id a string? for cache-type/expr lookups?)
	\param id the unqualified name of the object sought.  
	\return an object with the same name, if found.  
 */
never_const_ptr<object>
scopespace::lookup_object(const string& id) const {
	never_const_ptr<object> o = lookup_object_here(id);
	never_const_ptr<scopespace> parent(get_parent());
	if (o) return o;
	else if (parent) return parent->lookup_object(id);
	else return never_const_ptr<object>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic object lookup for a qualified identifier.
	If id is only a single identifier, and it is not absolute, 
	then it is considered an unqualified identifier.  
 */
never_const_ptr<object>
scopespace::lookup_object(const qualified_id_slice& id) const {
if (id.is_absolute()) {
	never_const_ptr<scopespace> parent(get_parent());
	if (parent)
		return parent->lookup_object(id);
	else {	// we are the ROOT, start looking down namespaces
		qualified_id_slice idc(id);
		never_const_ptr<name_space> ns = 
			lookup_namespace(idc.betail()).is_a<name_space>();
		if (ns)
			return ns->lookup_object(**id.rend());
		else return never_const_ptr<object>(NULL);
	}
} else if (id.size() <= 1) {
	return lookup_object(**id.begin());
} else {
	// else need to resolve namespace portion first
	qualified_id_slice idc(id);
	never_const_ptr<name_space> ns = 
		lookup_namespace(idc.betail()).is_a<name_space>();
	if (ns)
		return ns->lookup_object(**id.rend());
	else return never_const_ptr<object>(NULL);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The search for a namespace should always start at the 
	outermost namespace given the current context.  
	Since the outermost context can be something else, such as a loop, 
	we default to parent's lookup namespace if this is not a namespace. 
	The name_space::lookup_namespace will override this.  
	\param id is the entire name of the namespace.
	\return pointer to the scope or namespace matched if found.  
 */
never_const_ptr<scopespace>
scopespace::lookup_namespace(const qualified_id_slice& id) const {
	never_const_ptr<scopespace> parent(get_parent());
	assert(parent);
	return parent->lookup_namespace(id);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
	Registers a new instance of some type in the used_id_map.  
	Need to check for collisions, with previously declared instances.  
	For collectives, such as sparse arrays and dense arrays, 
	redeclarations with new indices are permitted, as long as it 
	can't be immediately determined that indices overlap.  
	Types must match for collective additions of course.  
	(What if type-parameters depend on variables?  allow?)
	\param i the new instance, possibly with sparse indices.
	\return pointer to newly created instance if successful, 
		else NULL.  
 */
never_const_ptr<instantiation_base>
scopespace::add_instance(excl_ptr<instantiation_base> i) {
	assert(i);
#if 0
	// DEBUG
	cerr << "In scopespace::add_instance with this = " << this << endl;
	i->dump(cerr << "excl_ptr<instantiation_base> i = ") << endl;
	dump(cerr);	// dump the entire namespace
#endif
	never_ptr<object> probe(
		lookup_object_here_with_modify(i->get_name()));
	if (probe) {
		never_ptr<instantiation_base> probe_inst(
			probe.is_a<instantiation_base>());
		if (probe_inst) {
			// make sure is not a template or port formal instance!
			// can't append to those.  
			if (probe_inst->is_template_formal()) {
				cerr << "ERROR: cannot redeclare or append to "
					"a template formal parameter." << endl;
				return never_const_ptr<instantiation_base>(
					NULL);
			}
			if (probe_inst->is_port_formal()) {
				cerr << "ERROR: cannot redeclare or append to "
					"a port formal instance." << endl;
				return never_const_ptr<instantiation_base>(
					NULL);
			}
			// compare types, must match!
			count_const_ptr<fundamental_type_reference>
				old_type(probe_inst->get_type_ref());
			count_const_ptr<fundamental_type_reference>
				new_type(i->get_type_ref());
#if 0
			probe_inst->dump(cerr << "lookup found: ") << endl;
#endif
			// type comparison is conservative, in the 
			// case of dynamic template parameters.  
			if (!old_type->may_be_equivalent(*new_type)) {
				cerr << "ERROR: type of redeclaration of "
					<< i->get_name() << " does not match "
					"previous declaration: " << endl <<
					"\twas: ";
				old_type->dump(cerr) << ", got: ";
				new_type->dump(cerr) << " ERROR!  ";
				return never_const_ptr<instantiation_base>(
					NULL);
			}	// else good to continue
			
			// compare dimensions
			if (!probe_inst->dimensions()) {
				// if original declaration was not collective, 
				// then one cannot add more.  
				probe->dump(cerr) << " was originally declared "
					"as a single instance, and thus may "
					"not be extended or re-declared, "
					"ERROR!  ";
				return never_const_ptr<instantiation_base>(
					NULL);
			} else if (probe_inst->dimensions()!=i->dimensions()) {
				probe->dump(cerr) << " was originally declared "
					"as a " << probe_inst->dimensions() <<
					"-D array, so the new declaration "
					"cannot add a " << i->dimensions() <<
					"-D array, ERROR!  ";
				return never_const_ptr<instantiation_base>(
					NULL);
			}	// else dimensions match apropriately

			assert(i);	// sanity
			// here, we know we're referring to the same collection
			// check for overlap with existing static-const indices
			const_range_list overlap(
				probe_inst->merge_index_ranges(i));
			if (!overlap.empty()) {
				// returned true if there is definite overlap
				cerr << "Detected overlap in the "
					"sparse collection for " <<
					i->get_name() << ", precisely: ";
				overlap.dump(cerr);
				cerr << ".  ERROR!  ";
				return never_const_ptr<instantiation_base>(
					NULL);
			}
			// else didn't detect static conflict.  
			// We discard the new instantiation, i, 
			// and let it delete itself at the end of this scope.  
			// ... happy ending, or is it?
			return probe_inst;
		} else {
			probe->what(cerr << i->get_name() <<
				" is already declared ") << ", ERROR!  ";
			return never_const_ptr<instantiation_base>(NULL);
		}
	} else {
		// didn't exist before, just add new instance
		never_const_ptr<instantiation_base> ret(i);
		used_id_map[i->get_name()] = i;		// transfer ownership
#if 0
		ret->dump(cerr << "just added: ") << endl;
		never_const_ptr<object> probe(
			lookup_object_here(ret->get_name()));
		probe->dump(cerr << "re-lookup: ") << endl;
		dump(cerr);	// dump the entire namespace
#endif
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a definition name alias to this scope.  
 */
bool
scopespace::add_definition_alias(never_const_ptr<definition_base> d, 
		const string& a) {
	never_const_ptr<object> probe(lookup_object_here(a));
	if (probe) {
		cerr << "Identifier \"" << a << "\" already taken by a ";
		probe->what(cerr) << " in ";
		what(cerr) << " " << get_qualified_name() << ".  ERROR!  ";
		return false;
	} else {
//		used_id_map[a] = d;
		used_id_map[a] = excl_ptr<object_handle>(new object_handle(d));
		return true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void
scopespace::add_connection_to_scope(
		excl_const_ptr<connection_assignment_base> c) {
	connect_assign_list.push_back(c);
}

//=============================================================================
// class name_space method definitions

/**
	Constructor for a new namespace.  
	Inherits from its parents: type aliases to built-in types, 
	such as bool and int.  
	TO DO: is parent REDUNDANT?
	\param n the name.  
	\param p pointer to the parent namespace.  
 */
name_space::name_space(const string& n, never_const_ptr<name_space> p) : 
		scopespace(), 
		key(n), 
		parent(p), 
		open_spaces(), open_aliases() {
}

/**
	Constructor for the global namespace, which is the only
	namespace without a parent.  
	Written here instead of re-using the above constructor with
	default argument because old compilers can't accept
	default arguments (NULL) for class object formals.  
 */
name_space::name_space(const string& n) :
		scopespace(), 
		key(n), 
		parent(NULL), 
		open_spaces(), open_aliases() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The only only memory we need to delete is that owned by 
	this namespace.  We created, thus we delete.  
	All other pointers are shared non-owned pointers, 
	and will be deleted by their respective owners.  
 */
name_space::~name_space() {
	// default destructors will take care of everything
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the key.
 */
const string&
name_space::get_key(void) const {
	return key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the parent namespace.
 */
never_const_ptr<scopespace>
name_space::get_parent(void) const {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns the flattened name of this current namespace.  
 */
string
name_space::get_qualified_name(void) const {
#if 0
	if (parent) {
		if (parent->parent)
			return parent->get_qualified_name() +scope +key;
		else	return key;
	} else return "<global>";		// global
#else
	if (parent)
		return parent->get_qualified_name() +scope +key;
	else return "";			// global e.g. ::foo
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns pointer to global namespace by following parent pointers.
 */
never_const_ptr<name_space>
name_space::get_global_namespace(void) const {
	if (parent)
		return parent->get_global_namespace();
	else	// no parent
		return never_const_ptr<name_space>(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
name_space::what(ostream& o) const {
	return o << "entity::namespace";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Spill contents of the used_id_map.
	\param o the output stream.
	\return the same output stream.
 */
ostream&
name_space::dump(ostream& o) const {
	used_id_map_type::const_iterator i;
//	list<never_const_ptr<...> > bin;		// later sort
	o << "In namespace \"" << key << "\", we have: {" << endl;
	for (i=used_id_map.begin(); i!=used_id_map.end(); i++) {
		o << "  " << i->first << " = ";
//		i->second->what(o) << endl;		// 1 level for now
		i->second->dump(o) << endl;
	}
	return o << "}" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Downward (deeper) traversal of namespace hierarchy.  
	Either creates a new sub-namespace or opens if it already exists.  
	\param n the name of the namespace to enter.
	\return pointer to the referenced namespace, if found, else NULL.
	Details: First searches list of aliased namespaces to check for
	collision, which is currently reported as an error.  
	(We want to simply head off potential ambiguity here.)
	Then sees if name is already taken by some other definition 
	or instance in the used_id_map.  
	Then searches subnamespace to determine if already exists.
	If exists, re-open, else create new and link to parent.  
	\sa leave_namespace
 */
never_ptr<name_space>
name_space::add_open_namespace(const string& n) {
	never_ptr<name_space> ret;
	never_const_ptr<object> probe = lookup_object_here(n);
	if (probe) {
		never_const_ptr<name_space> probe_ns(probe.is_a<name_space>());
		// an alias may return with valid pointer!
		if (!probe_ns) {
			probe->what(cerr << n << " is already declared as a ")
				<< ", ERROR! ";
			return never_ptr<name_space>(NULL);
		} else if (open_aliases[n]) {
		// we have a valid namespace pointer, 
		// now we see if this is an alias, or true sub-namespace
			cerr << n << " is already declared an open alias, "
				"ERROR! ";
			return never_ptr<name_space>(NULL);
		} else {
		// therefore, probe_ns is a pointer to a valid sub-namespace
			DEBUG(TRACE_NAMESPACE_NEW, 
				cerr << n << " is already exists as subspace, "
					"re-opening")
			ret = lookup_object_here_with_modify(n)
				.is_a<name_space>();
			assert(probe_ns->key == ret->key);
		}
		assert(ret);
	} else {
		// create it, linking this as its parent
		DEBUG(TRACE_NAMESPACE_NEW, cerr << " ... creating new")
		excl_ptr<name_space> new_ns(
			new name_space(n, never_const_ptr<name_space>(this)));
		ret = new_ns;
		assert(ret);
		assert(new_ns.owned());
		// register it as a used id
		used_id_map[n] = new_ns;
			// explicit transfer
		assert(!new_ns);
	}

	// silly sanity checks
	assert(ret->parent == this);
	assert(ret->key == n);
	DEBUG(TRACE_NAMESPACE_NEW, 
		cerr << " with parent: " << ret->parent->key)
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Leaves the namespace.  
	The list of imported and aliased namespaces will be reset each time 
	this namespace is closed.  They will have to be added back the 
	next time it is re-opened.  
	Also reclaims the identifiers that were associated with namespace
	aliases.  
	Non-aliased imported namespace do not take up any identifier space, 
	and thus, are not checked against the used_id_map.  
	\return pointer to the parent namespace, should never be NULL.  
	\sa add_open_namespace
 */
never_const_ptr<name_space>
name_space::leave_namespace(void) {
	// for all open_aliases, release their names from used-map
	alias_map_type::const_iterator i = open_aliases.begin();
	for ( ; i!=open_aliases.end(); i++) {
		used_id_map.erase(used_id_map.find((*i).first));
	}
	open_spaces.clear();
	open_aliases.clear();
	return parent;
	// never NULL, can't leave global namespace!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a namespace using directive (import) to current namespace.  
	\param n the qualified identifier of the referenced namespace.
	\return valid pointer to imported namespace, or NULL if error. 
	\sa add_using_alias
 */
never_const_ptr<name_space>
name_space::add_using_directive(const qualified_id& n) {
	never_const_ptr<name_space> ret;
	namespace_list::const_iterator i;
	namespace_list candidates;		// empty list

	DEBUG(TRACE_NAMESPACE_USING, 
		cerr << endl << "adding using-directive in space: " 
			<< get_qualified_name())
	// see if namespace has already been declared within scope of search
	// remember: the qualified_id is a suffix to be appended onto root
	// find it/them, record to list
	query_import_namespace_match(candidates, n);
	i = candidates.begin();

	switch (candidates.size()) {
	// if list's size > 1, ambiguity
	// else if list is empty, unresolved namespace
	// else we've narrowed it down to one
		case 1: 
			ret = (*i);
			open_spaces.push_back(ret);
			break;
		case 0:	{
			cerr << "namespace " << n << " not found, ERROR! ";
			// or n is not a namespace
			ret = never_const_ptr<name_space>(NULL);
			break;	// no matches
			}
		default: {	// > 1
			ret = never_const_ptr<name_space>(NULL);
			cerr << " ERROR: ambiguous import of namespaces, "
				"need to be more specific.  candidates are: ";
				for ( ; i!=candidates.end(); i++)
					cerr << endl << "\t" << 
						(*i)->get_qualified_name();
			}
	}

	// if not already in our list of mapped serachable namespaces,
	// add it to our map of namespaces to search
	// else maybe warning... harmless

	return ret;		// NULL => error
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Aliases another namespace as a pseudo-sub-namespace with a different
	name.  Similar to add_using_directive, 
	but using a different name, and taking a spot in used_id_map.  
	\param n the referenced namespace qualified identifier
	\param a the new name local identifier
	Procedure outline: 
	Check if alias name is already taken by something else
	in this namespace.  Any local collision is reported as an error.  
	Note: name clashes with namespaces in higher scopes are permitted, 
	and in imported (unaliased) spaces.  
	This allows one to overshadow identifiers in higher namespaces.  
	\sa add_using_directive
 */
never_const_ptr<name_space>
name_space::add_using_alias(const qualified_id& n, const string& a) {
	never_const_ptr<object> probe;
	never_const_ptr<name_space> ret;
	namespace_list::const_iterator i;
	namespace_list candidates;		// empty list

	DEBUG(TRACE_NAMESPACE_ALIAS, 
		cerr << endl << "adding using-alias in space: " 
			<< get_qualified_name() << " as " << a)

	// need to force use of the constant version of the lookup
	// because this method is non-const.  
	// else it will modify the used_id_map!
	// perhaps wrap with a probe() const method...
	probe = lookup_object_here(a);
	if (probe) {
		probe = never_const_ptr<object>(&probe->self());
		// resolve handles
	}
	if (probe) {
		// then already, it conflicts with some other id
		// we report the conflict precisely as follows:
		ret = probe.is_a<name_space>();
		if (ret) {
			if(open_aliases[a]) {
				cerr << a << " is already an open alias, "
					"ERROR! ";
			} else {
				cerr << a << " is already a sub-namespace, "
					"ERROR! ";
			}
		} else {
			probe->what(cerr << a << " is already declared ") 
				<< ", ERROR! ";
			// if is another namespace, could be an alias
			//	which looks like an alias
			// perhaps make a namespace_alias class
			// to replace handle...
		}
		return never_const_ptr<name_space>(NULL);
	}

	// else we're ok to proceed to add alias
	// first find the referenced namespace...
	query_import_namespace_match(candidates, n);
	i = candidates.begin();

	switch (candidates.size()) {
	// if list's size > 1, ambiguity
	// else if list is empty, unresolved namespace
	// else we've narrowed it down to one
		case 1: {
			// SOMETHING GOES WRONG HERE, FIX ME FIRST!!!
			ret = (*i);
			open_aliases[a] = ret;
			// ret is owned by the namespace where it belongs
			// don't need to own it here in our used_id_map
			// or even open_aliases.  
			// however, used_id_map is non-const, 
			// so we need to wrap it in a const object_handle.  
			used_id_map[a] = excl_ptr<object_handle>(
				new object_handle(ret));
			break;
			}
		case 0:	{
			cerr << " ... not found, ERROR! ";
			ret = never_const_ptr<name_space>(NULL);
			break;	// no matches
			}
		default: {	// > 1
			ret = never_const_ptr<name_space>(NULL);
			cerr << " ERROR: ambiguous import of namespaces, "
				"need to be more specific.  candidates are: ";
				for ( ; i!=candidates.end(); i++)
					cerr << endl << "\t" << 
						(*i)->get_qualified_name();
			}
	}

	return ret;		// NULL => error
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns valid pointer to a namespace if a strict match is found.  
	(at most one precise match)
	This will serach both true subnamespaces and aliased subspaces.  
	This variation includes the invoking namespace in the pattern match.  
	Now honors the absolute flag of the qualified_id to start search
	from global namespace.  
	TO DO: re-use query_subnamespace_match
	\param id the qualified/scoped name of the namespace to match.
	\return pointer to found namespace.
 */
never_const_ptr<name_space>
name_space::query_namespace_match(const qualified_id_slice& id) const {
	// qualified_id_slice is a wrapper around qualified_id
	// recall that qualified_id is a node_list<token_identifier,scope>
	// and that token_identifier is a sub-type of string
	DEBUG(TRACE_NAMESPACE_QUERY, 
		cerr << "query_namespace_match: " << id 
			<< " in " << get_qualified_name() << endl)

	if (id.empty())	{	// what if it's absolute and empty?
		return (id.is_absolute()) ? get_global_namespace() : 
			never_const_ptr<name_space>(this);
	}
	qualified_id_slice::const_iterator i = id.begin();	assert(*i);
	count_const_ptr<token_identifier> tid(*i);
	assert(tid);
	DEBUG(TRACE_NAMESPACE_SEARCH, cerr << "\ttesting: " << *tid)
	never_const_ptr<name_space> ns =
		(id.is_absolute()) ? get_global_namespace()
		: never_const_ptr<name_space>(this);
	if (ns->key.compare(*tid)) {
		// if names differ, already failed, try alias spaces
		return never_const_ptr<name_space>(NULL);
	} else {
		for (i++; ns && i!=id.end(); i++) {
			never_const_ptr<name_space> next;
			// no need to skip scope tokens anymore
			tid = i->is_a<token_identifier>();
			assert(tid);
			DEBUG(TRACE_NAMESPACE_SEARCH, cerr << scope << *tid)
			// the [] operator of map<> doesn't have const 
			// semantics, even if looking up an entry!
			next = ns->lookup_object_here(*tid).is_a<name_space>();
			// if not found in subspaces, check aliases list
			// or should we not search aliases?
			ns = (next) ? next : ns->open_aliases[*tid];
		}

	// for loop terminates when ns is NULL or i is at the end
	// if i is not at the end, then we didn't find a matched namespace
	//	because there are still scoped id's trailing, 
	//	therefore ns is NULL, which means no match.  
	// if ns is not NULL, then i must be at the end, 
	//	which means that we've matched so far.
	// In either case, we return ns.  

		return ns;
	} 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns valid pointer to a namespace if a strict match is found.  
	(at most one precise match)
	This will treat open aliased namespaces as valid subspaces for search.
	This variation excludes the invoking namespace in the pattern match.  
	\param id the qualified/scoped name of the namespace to match
 */
never_const_ptr<name_space>
name_space::query_subnamespace_match(const qualified_id_slice& id) const {
	// qualified_id_slice is just a wrapper around qualified_id
	// recall that qualified_id is a node_list<token_identifier,scope>
	// and that token_identifier is a sub-type of string
	DEBUG(TRACE_NAMESPACE_QUERY, 
		cerr << endl << "query_subnamespace_match: " << id 
			<< " in " << get_qualified_name() << endl)

	// here, does NOT check for global-absoluteness
	if (id.empty())	{	// what if it's absolute and empty?
		return (id.is_absolute()) ? get_global_namespace() : 
			never_const_ptr<name_space>(this);
	}
	qualified_id_slice::const_iterator i = id.begin();
	count_const_ptr<token_identifier> tid(*i);
	assert(tid);
	DEBUG(TRACE_NAMESPACE_SEARCH, cerr << "\ttesting: " << *tid)
	// no check for absoluteness
	never_const_ptr<name_space> ns;
	if (id.is_absolute()) {
		ns = get_global_namespace()->
			lookup_object_here(*tid).is_a<name_space>();
	} else {
		// force use of const probe
		never_const_ptr<object> probe(lookup_object_here(*tid));
		ns = probe.is_a<name_space>();
	}

	if (!ns) {				// else lookup in aliases
		ns = open_aliases[*tid];	// replaced for const semantics
	}
	for (i++; ns && i!=id.end(); i++) {
		never_const_ptr<name_space> next;
		tid = i->is_a<token_identifier>();
		assert(tid);
		DEBUG(TRACE_NAMESPACE_SEARCH, cerr << scope << *tid)
		next = ns->lookup_object_here(*tid).is_a<name_space>();
		// if not found in subspaces, check aliases list
		ns = (next) ? next : ns->open_aliases[*tid];
	}
	// for loop terminates when ns is NULL or i is at the end
	// if i is not at the end, then we didn't find a matched namespace
	//	because there are still scoped id's trailing, 
	//	therefore ns is NULL, which means no match.  
	// if ns is not NULL, then i must be at the end, 
	//	which means that we've matched so far.
	// In either case, we return ns.  

	return ns;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns valid pointer to a namespace if a strict match is found.  
	(allows multiple matches, hence the use of a list reference)
	This variation checks whether the head of the identifier is the name
	of this scope, and if so, searches with the beheaded id.  
	This will catch: open foo::bar...
	\param m the accumulating list of matches
	\param id the qualified/scoped name of the namespace to match
 */
void
name_space::
query_import_namespace_match(namespace_list& m, const qualified_id& id) const {
	DEBUG(TRACE_NAMESPACE_QUERY, 
		cerr << endl << "query_import_namespace_match: " << id 
			<< " in " << get_qualified_name())
	{
		never_const_ptr<name_space> ret = query_subnamespace_match(id);
		if (ret) m.push_back(ret);
	}
	// always search these unconditionally? or only if not found so far?
	{	// with open namespaces list
		namespace_list::const_iterator i = open_spaces.begin();
		for ( ; i!=open_spaces.end(); i++) {
			never_const_ptr<name_space> ret = 
				(*i)->query_subnamespace_match(id);
			if (ret) m.push_back(ret);
		}
	}
	// When searching for imported namespaces matches found
	// in the deepest scopes will override those found in higher
	// scope without ambiguity... for now
	// (can be easily changed)
	// Only if the match list is empty, ask the parent to do the 
	// same query.  
#if 1
	if (m.empty() && parent)
#else
	if (parent)
#endif
		parent->query_import_namespace_match(m, id);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Finds a namespace ending with a (optionally) scoped identifier
	(allows multiple matches, hence the use of a list reference).
	Currently not used.  
 */
void
name_space::
find_namespace_ending_with(namespace_list& m, const qualified_id& id) const {
	// should we return first match, or all matches?
	//	for now: first match
	//	later, we'll complain about ambiguities that need resolving
	// search order:
	// 1) in this namespace's list of sub-namespaces
	//	(must be declared already)
	// 2) in this namespace's list of open/aliased namespaces
	//	*without* following its imports
	// 3) upward a namespace scope, which will search its 1,2
	//	including the global scope, if reached
	// terminates (returning NULL) if not found
	never_const_ptr<name_space> ret = query_subnamespace_match(id);
	if (ret)	m.push_back(ret);
	query_import_namespace_match(m, id);
	if (parent)
		parent->find_namespace_ending_with(m, id);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	THIS IS KEY.  
	Adds a definition to this namespace.  
	Can be used by declaration prototypes as well.  
	Need to check whether existing definition is_defined, 
		as opposed to just being declared.  
	Definition is newly created, so used_id_map is responsible
	for deleting it.  
	On failure, however, pointer is not added, so need to handle
	memory in the caller.  
	
	\param db the definition to add, newly created.
	\return modifiable pointer to definition if successful, else NULL.  
 */
never_ptr<definition_base>
name_space::add_definition(excl_ptr<definition_base> db) {
	assert(db);
	string k = db->get_name();
	never_const_ptr<object> probe(lookup_object_here(k));
	if (probe) {
		never_const_ptr<definition_base> probe_def(
			probe.is_a<definition_base>());
		if (probe_def) {
			assert(k == probe_def->get_name());	// consistency
			if (probe_def->require_signature_match(db)) {
				// definition signatures match
				// can discard new declaration
				// db will self-delete (excl_ptr)
				return never_ptr<definition_base>(db);
			} else {
				// signature mismatch!
				// also catches class type mismatch
				cerr << "new declaration for \"" << k <<
					"\" doesn't match previous "
					"declaration.  ERROR! ";
				// give details...
				return never_ptr<definition_base>(NULL);
			}
		} else {
			probe->what(cerr << "Identifier already taken by ")
				<< endl << "ERROR: Failed to add definition! ";
			return never_ptr<definition_base>(NULL);
		}
	} else {
		// used_id_map owns this type is reponsible for deleting it
		never_ptr<definition_base> ret = db;
		assert(ret);
		used_id_map[k] = db;
		assert(!db);		// after explicit transfer of ownership
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a fundamental_type_reference to the used_id_map, using
	the hash_string as the key.  
	Memory management: will delete tb if it is redundant!
	So tb is created before this method call.  
	Inefficiency: have to create and delete type_reference.
	TO DO: be able to lookup type in advance before creating...
	\param tb the fundamental_type_reference to lookup and add --
		MAY BE DELETED if a matching reference is found.
	\return the same fundamental_type_reference if none was previously 
		found, otherwise the existing matching reference.  
 */
never_const_ptr<fundamental_type_reference>
name_space::add_type_reference(excl_ptr<fundamental_type_reference> tb) {
	never_const_ptr<object> o;
	never_const_ptr<fundamental_type_reference> trb;
	assert(tb);
	string k = tb->hash_string();
	o = lookup_object_here(k);
	// see what is already there...
	trb = o.is_a<fundamental_type_reference>();

	if (o)	assert(trb);
	// else a non-type-reference hashed into the used_id_map
	// using a hash for a type-reference!!!

	if (trb) {
		// then found a match!, we can delete tb
		// since tb is an excl_ptr, it will delete itself
		return trb;
	} else {
		// if tb contains parameter literals that are only
		// local to this scope, we must add it here, 
		// else if tb contains only constants and resolved
		// parameters, then we have two options:
		// 1) search up parents' namespace until one is found.  
		//	if not found, add it locally.
		// 2) jump to the global namespace, to search for type
		//	if found, use that, else add it to global.
		//	problem: parent/global namespace is const, 
		//	so we can't modify it with dirty hack.  
		// 3) just add it locally to this namespace regardless...
		//	who gives a rat's a** about redundancy?
		// 3:
		never_const_ptr<fundamental_type_reference> ret(tb);
		assert(tb);
		used_id_map[k] = tb;
		assert(!tb);
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Overrides scopespace::lookup_namespace.
	\param id is the entire name of the namespace.
	\return pointer to the scope or namespace matched if found.  
 */
never_const_ptr<scopespace>
name_space::lookup_namespace(const qualified_id_slice& id) const {
	return query_subnamespace_match(id);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

