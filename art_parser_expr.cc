// "art_parser_expr.cc"
// class method definitions for ART::parser, related to expr

// template instantiations are at the end of the file

#include <iostream>

#include "art_macros.h"
#include "art_parser_template_methods.h"

// will need these come time for type-checking
// #include "art_symbol_table.h"
#include "art_object.h"
#include "art_object_expr.h"

// enable or disable constructor inlining, undefined at the end of file
// leave blank do disable, define as inline to enable
#define	CONSTRUCTOR_INLINE		
#define	DESTRUCTOR_INLINE		

//=============================================================================
// debug flags
#define	DEBUG_ID_EXPR	0

//=============================================================================
namespace ART {
using namespace entity;

namespace parser {

//=============================================================================
// class expr method definitions

/// Empty constructor
CONSTRUCTOR_INLINE
expr::expr() : node() { }

/// Empty virtual destructor
DESTRUCTOR_INLINE
expr::~expr() { }

//=============================================================================
// class paren_expr method definitions

CONSTRUCTOR_INLINE
paren_expr::paren_expr(const token_char* l, const expr* n, 
		const token_char* r) : expr(),
		lp(l), e(n), rp(r) {
	assert(lp); assert(e); assert(rp);
}

DESTRUCTOR_INLINE
paren_expr::~paren_expr() {
}

ostream&
paren_expr::what(ostream& o) const {
	return o << "(paren-expr)";
}

line_position
paren_expr::leftmost(void) const {
	if (lp)	return lp->leftmost();
	else	return e->leftmost();
}

line_position
paren_expr::rightmost(void) const {
	if (rp)	return rp->rightmost();
	else	return e->rightmost();
}

never_const_ptr<object>
paren_expr::check_build(never_ptr<context> c) const {
	return e->check_build(c);
}

//=============================================================================
// class qualified_id method definitions

CONSTRUCTOR_INLINE
qualified_id::qualified_id(token_identifier* n) : 
	qualified_id_base(n), absolute(NULL) {
}

/// copy constructor, no transfer of ownership
CONSTRUCTOR_INLINE
qualified_id::qualified_id(const qualified_id& i) :
		node(), qualified_id_base(i), absolute(NULL) {
#if DEBUG_ID_EXPR
	cerr << "qualified_id::qualified_id(const qualified_id&);" << endl;
#endif
	if (i.absolute) {
		absolute = new token_string(*i.absolute);
		// actually *copy* the token
		assert(absolute);
	}
}

DESTRUCTOR_INLINE
qualified_id::~qualified_id() {
	SAFEDELETE(absolute);
}

/**
	Call this function in the parser to mark an un/qualified identifier
	as absolute, as oppposed to relative.  
	See class definition of qualified_id for an explanation.  
	\param s should be a scope (::) token.  
	\return pointer to this object
 */
qualified_id*
qualified_id::force_absolute(token_string* s) {
	absolute = s;
	assert(absolute);
	return this;
}

ostream&
qualified_id::what(ostream& o) const {
	return o << "(id-expr)";
}

qualified_id*
qualified_id::append(terminal* d, token_identifier* n) {
	return IS_A(qualified_id*, qualified_id_base::append(d,n));
}

line_position
qualified_id::leftmost(void) const {
	return qualified_id_base::leftmost();
}

line_position
qualified_id::rightmost(void) const {
	return qualified_id_base::rightmost();
}

/***
	Future: instead of copying, give an iterator range.
***/

qualified_id
qualified_id::copy_namespace_portion(void) const {
	qualified_id ret(*this);		// copy, not-owned
	if (!ret.empty())
		ret.pop_back();		// remove last element
	if (!ret.delim.empty())
		ret.delim.pop_back();
	return ret;
}

qualified_id
qualified_id::copy_beheaded(void) const {
	qualified_id ret(*this);		// copy, not-owned
	if (!ret.empty())
		ret.pop_front();		// remove last element
	if (!ret.delim.empty())
		ret.delim.pop_front();
	return ret;
}

/**
	Finds an object referenced by the name, be it type or instance.  
	Remember to check the return type in the caller, even virtual
	calls through the abstract expr class.  
	\param c the context from which the search starts.  
	\return a pointer to a definition_base or an instantiation_base 
		with the matching [un]qualified identifier if found, else NULL.
		Other possibilities: namespace?
		Consumer should wrap in instance_reference?
			might be collective, in the case of an array
 */
never_const_ptr<object>
qualified_id::check_build(never_ptr<context> c) const {
	return c->lookup_object(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// non-member functions

// friend operator
ostream& operator << (ostream& o, const qualified_id& id) {
//	o << "(size = " << id.size() << ", empty = " << id.empty() << ")";
	if (id.empty()) {
		return o << "<null qualified_id>";
	} else {
		qualified_id::const_iterator i = id.begin();
		if (id.is_absolute())
			o << scope;
		count_const_ptr<token_identifier> tid(*i);
		assert(tid);
		o << *tid;
		for (i++ ; i!=id.end(); i++) {
			tid = *i;
			assert(tid);
			o << scope << *tid;
		}
		return o;
	}
}

// friend operator
ostream& operator << (ostream& o, const qualified_id_slice& id) {
	if (id.empty()) {
		return o << "<null qualified_id_slice>";
	} else {
		qualified_id_slice::const_iterator i = id.begin();
		if (id.is_absolute())
			o << scope;
		count_const_ptr<token_identifier> tid(*i);
		assert(tid);
		o << *tid;
		for (i++ ; i!=id.end(); i++) {
			tid = *i;
			assert(tid);
			o << scope << *tid;
		}
		return o;
	}
}

//=============================================================================
// class id_expr method definitions

id_expr::id_expr(qualified_id* i) : expr(), qid(i) {
	assert(qid);
}

id_expr::id_expr(const id_expr& i) :
		node(), expr(), qid(new qualified_id(*i.qid)) {
	assert(qid);
}

id_expr::~id_expr() {
}

ostream&
id_expr::what(ostream& o) const {
        return o << "(namespace-id): " << *qid;
}

line_position     
id_expr::leftmost(void) const {
        return qid->leftmost();
}

line_position
id_expr::rightmost(void) const {
        return qid->rightmost();  
}

/**
	The qualified_id member's check build can return a definition 
	or instance pointer.  
	\param c the context where to begin searching for named object.  
	\return newly allocated param_literal containing reference to the
		instance if found, else NULL.
	FIX ME: should return instance_reference!, not instance!
 */
never_const_ptr<object>
id_expr::check_build(never_ptr<context> c) const {
	never_const_ptr<object> o;
	never_const_ptr<instantiation_base> inst;
	o = qid->check_build(c);		// will lookup_object
	if (o) {
		inst = o.is_a<instantiation_base>();
		if (inst) {
			// we found an instance which may be single
			// or collective... info is in inst.
			inst->make_instance_reference(*c);
			// pushes the created reference onto
			// context's instance_reference_stack.
			// if indexed, check in the caller, and modify
			//	in index_expr...

			// doesn't have to be a parameter, does it?
		} else {
			cerr << "object " << *qid <<
				" is not an instance, ERROR!";
		}
	} else {
		cerr << "object " << *qid << " not found, ERROR!";
	}
	return never_const_ptr<object>(NULL);
//	return c->lookup_instance(*qid);
// also accomplishes same thing?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// non-member functions
ostream& operator << (ostream& o, const id_expr& id) {
	return o << *id.qid;
}

//=============================================================================
// class range method definitions

CONSTRUCTOR_INLINE
range::range(const expr* l) : lower(l), op(NULL), upper(NULL) {
	assert(lower); 
}

CONSTRUCTOR_INLINE
range::range(const expr* l, const terminal* o, const expr* u) : 
		lower(l), op(o), upper(u) {
	assert(lower); assert(op); assert(upper);
}

DESTRUCTOR_INLINE
range::~range() {
}

ostream&
range::what(ostream& o) const {
	return o << "(range)";
}

line_position
range::leftmost(void) const {
	return lower->leftmost();
}

line_position
range::rightmost(void) const {
        if (upper)      return upper->rightmost();
        else if (op)    return op->rightmost();
        else            return lower->rightmost();
}

/**
	Both expressions of the range should be of type pint.  
	No collective expressions, only single pints.  
	Do we check for constant cases?
	TO DO: finish me
	How do we interpret x[i]?
	\param c the context where to start resolving identifiers.  
	\return I don't know.
 */
never_const_ptr<object>
range::check_build(never_ptr<context> c) const {
//	cerr << "range::check_build(): INCOMPLETE, FINISH ME!" << endl;
	never_const_ptr<object> o;

//	never_const_ptr<param_type_reference> pint_type = c->global_namespace->
//		lookup_object("pint").is_a<param_type_reference>();
//	assert(pint_type);

	o = lower->check_build(c);	// useless return value
	// puts param_expr on stack
	count_ptr<object> l(c->pop_top_object_stack());
	count_ptr<pint_expr> lp(l.is_a<pint_expr>());
	if (l) {
		if (!lp) {
			cerr << "Expression is not a pint-type, ERROR!  " <<
				lower->where() << endl;
			exit(1);
		}
		// check if expression is initialized
	} else {
		cerr << endl;
		cerr << "Error resolving expression " << lower->where()
			<< endl;
		exit(1);
	}

	if (upper) {
		o = upper->check_build(c);
		// puts param_expr on stack
		// grab the last two expressions, 
		// check that they are both pints, 
		// and make a range object
		count_ptr<object> u(c->pop_top_object_stack());
		count_ptr<pint_expr> up(u.is_a<pint_expr>());
		if (u) {
			if (!up) {
				cerr << "Expression is not a pint-type, "
					"ERROR!  " << upper->where() << endl;
				exit(1);
			}
			// check if expression is initialized
		} else {
			cerr << "Error resolving expression " << upper->where()
				<< endl;
			exit(1);
		}
		// at this point, is ok

		// later: resolve constant if possible...
		// modularize this into a method in art_object_expr
		if (lp->is_static_constant() && up->is_static_constant()) {
			const int lb = lp->static_constant_int();
			const int ub = up->static_constant_int();
			if (lb > ub) {
				// error!  can't construct invalid const_range
				cerr << "Lower bound of range " <<
					lower->where() << " is greater than "
					"upper bound " << upper->where() <<
					".  ERROR!" << endl;
				c->push_object_stack(
					count_ptr<const_range>(NULL));
				// exit(1);
				// or let error get caught elsewhere?
			} else {
				// make valid constant range
				c->push_object_stack(count_ptr<const_range>(
					new const_range(lb, ub)));
			}
		} else {
			// can't determine validity of bounds statically
			c->push_object_stack(count_ptr<pint_range>(
				new pint_range(lp, up)));
		}
	} else {
		// but check that it is of type pint
		// and push it back onto stack
		// the caller will interpret it
		c->push_object_stack(l);
		// passing lp: ART::entity::object ambiguous base class
		//	of pint_expr :S
	}
	// not done yet
	return o;
}

//=============================================================================
// class range_list method definitions

range_list::range_list(const range* r) : parent(r) {
}

range_list::~range_list() { }

/**
	Note: limited to 4 dimensions.  

	Parent's check_build will result in each index dimension
	being pushed onto the context's object stack.  
	Grab them off the stack to form an object list.  

	Or just convert directly to a range list?
	No range list has two different semantics
	(depending on instantiation vs. reference),
	so just leave as object_list.  

	Should we take this opportunity to const-ify as much as possible?

	IMPORTANT:
	Only puts an object_list onto object_stack.  
	It is up to caller to interpret either as a sparse_range list
	in the case of an array declaration or as an index_list in the
	case of an indexed instance reference.  

	This is called by instance_array::check_build and 
	index_expr::check_build.  

	\return NULL, useless.
 */
never_const_ptr<object>
range_list::check_build(never_ptr<context> c) const {
	parent::check_build(c);
	count_ptr<object_list> ol(new object_list);
	size_t i = 0;
	for ( ; i<size(); i++) {
		count_ptr<object> o(c->pop_top_object_stack());
		if (!o) {
			cerr << "Problem with dimension " << i+1 <<
				" of sparse_range_list between "
				<< where() << endl;
			exit(1);		// terminate?
		} else if (!o.is_a<ART::entity::index_expr>()) {
			// pint_const *should* => index_expr ...
			// make sure each item is a range expression
			cerr << "Expression in dimension " << i+1 <<
				" of sparse_range_list is not valid!  "
				<< where() << endl;
//			o->what(cerr << "object is a ") << endl;
//			o->dump(cerr << "object dump: ") << endl;
			exit(1);
		}
		// else o is an index_expr
		ol->push_front(o);
	}
	if (size() > 4) {		// define constant somewhere
		cerr << "ERROR!  Exceeded dimension limit of 4.  "
			<< where() << endl;
		c->push_object_stack(count_ptr<object>(NULL));
	} else {
//		c->push_object_stack(ol->make_sparse_range_list());
		// don't necessarily want to interpret as sparse_range
		// may want it as an index!
		c->push_object_stack(ol);
	}
	return never_const_ptr<object>(NULL);
}

//=============================================================================
// class dense_range_list method definitions

dense_range_list::dense_range_list(const expr* r) : parent(r) {
}

dense_range_list::~dense_range_list() {
}

/**
	Dense range lists are reserved for formal parameters and ports, 
	which must be dense arrays, cannot be sparse.  
	Limited to 4 dimensions.  
 */
never_const_ptr<object>
dense_range_list::check_build(never_ptr<context> c) const {
	parent::check_build(c);
	count_ptr<object_list> ol(new object_list);
	size_t i = 0;
	for ( ; i<size(); i++) {
		count_ptr<object> o(c->pop_top_object_stack());
		if (!o) {
			cerr << "Problem with dimension " << i+1 <<
				" of dense_range_list between "
				<< where() << endl;
			exit(1);		// terminate?
		} else if (!o.is_a<pint_expr>()) {
			// make sure that each item is an integer expr
			cerr << "Expression in dimension " << i+1 <<
				" of dense_range_list is not integer!  "
				<< where() << endl;
			exit(1);
		}
		ol->push_front(o);
	}
	if (size() > 4) {		// define constant somewhere
		cerr << "ERROR!  Exceeded dimension limit of 4.  "
			<< where() << endl;
		c->push_object_stack(count_ptr<object>(NULL));
	} else {
		c->push_object_stack(ol->make_formal_dense_range_list());
	}
	return never_const_ptr<object>(NULL);
}

//=============================================================================
// class unary_expr method definitions

/**
	Failure to dynamic_cast will result in assignment to a NULL pointer,
	which will be detected, and properly memory managed, assuming
	that the arguments exclusively "owned" their memory locations.
 */
CONSTRUCTOR_INLINE
unary_expr::unary_expr(const expr* n, const terminal* o) : expr(), 
		e(n), op(o) {
}

DESTRUCTOR_INLINE
unary_expr::~unary_expr() {
}

//=============================================================================
// class prefix_expr method definitions

CONSTRUCTOR_INLINE
prefix_expr::prefix_expr(const terminal* o, const expr* n) :
		unary_expr(n,o) {
}

DESTRUCTOR_INLINE
prefix_expr::~prefix_expr() { }

ostream&
prefix_expr::what(ostream& o) const {
	return o << "(prefix-expr)";
}

line_position
prefix_expr::leftmost(void) const {
	return op->leftmost();
}

line_position
prefix_expr::rightmost(void) const {
	return e->rightmost();
}

never_const_ptr<object>
prefix_expr::check_build(never_ptr<context> c) const {
	cerr << "prefix_expr::check_build(): I'm not done yet!" << endl;
	e->check_build(c);
	return never_const_ptr<object>(NULL);
}

//=============================================================================
// class postfix_expr method definitions

CONSTRUCTOR_INLINE
postfix_expr::postfix_expr(const expr* n, const terminal* o) :
		unary_expr(n,o) {
}

DESTRUCTOR_INLINE
postfix_expr::~postfix_expr() { }

line_position
postfix_expr::leftmost(void) const {
	return e->leftmost();
}

line_position
postfix_expr::rightmost(void) const {
	return op->rightmost();
}

//=============================================================================
// class member_expr method definitions

CONSTRUCTOR_INLINE
member_expr::member_expr(const expr* l, const terminal* op, 
		const token_identifier* m) :
		postfix_expr(l,op), member(m) {
	assert(member);
}

DESTRUCTOR_INLINE
member_expr::~member_expr() { }

ostream&
member_expr::what(ostream& o) const {
	return o << "(member-expr)";
}

line_position
member_expr::rightmost(void) const {
	return member->rightmost();
}

/**
	Type-check of member reference.  
 */
never_const_ptr<object>
member_expr::check_build(never_ptr<context> c) const {
	never_const_ptr<object> o;

	o = e->check_build(c);
	// expect: fundamental_type_reference

	// this should return a reference to an instance
	// of some type that has members, such as data, channel, process.  
	// l should resolve to a *single* instance of something, 
	// cannot be an array.  

	// use that instance_reference, get its referenced definition_base, 
	// and make sure it has a member m.

	// what should this return?  the same thing it expects:
	// a reference to an instance of some type.  
	// Problem: instances aren't concrete until they are unrolled.
	// What is available in type-check phase?
	// Maybe we don't care about the instances themselves, 
	// rather the *type* returned.  
	// after all this is type-checking, not range checking.  

	assert(0);
	return never_const_ptr<object>(NULL);
}

//=============================================================================
// class index_expr method definitions

CONSTRUCTOR_INLINE
index_expr::index_expr(const expr* l, const range_list* i) :
		postfix_expr(l, NULL),
		ranges(i) {
	assert(ranges);
}

DESTRUCTOR_INLINE
index_expr::~index_expr() { }

ostream&
index_expr::what(ostream& o) const {
	return o << "(index-expr)";
}

line_position
index_expr::rightmost(void) const {
	return ranges->rightmost();
}

/**
	TO DO: FINISH ME
	Check index expression first, must be an integer type.  
	Checking identifier should place an instance_reference 
	on the context's object stack.  
	For an indexed instance reference, we need to take it off the 
	stack, modify it, and replace it back onto the stack.  
 */
never_const_ptr<object>
index_expr::check_build(never_ptr<context> c) const {
//	cerr << "index_expr::check_build(): FINISH ME!" << endl;
//	never_const_ptr<object> o;

	ranges->check_build(c);		// useless return value
	// should result in a ART::entity::index_list on the stack
	count_ptr<object> index_obj(c->pop_top_object_stack());
	// see range_list::check_build()
	if (!index_obj) {
		cerr << "ERROR in indices!  " << ranges->where() << endl;
		exit(1);
	}
	count_ptr<object_list> ol(index_obj.is_a<object_list>());
	assert(ol);
	// would rather have excl_ptr...
	excl_ptr<index_list> index_list_obj = ol->make_index_list();
	if (!index_list_obj) {
		cerr << "ERROR in index list!  "
			<< ranges->where() << endl;
		exit(1);
	}
	assert(index_list_obj);

	e->check_build(c);
	// should result in an instance_reference on the stack.  
	count_ptr<object> base_obj(c->pop_top_object_stack());
	if (!base_obj) {
		cerr << "ERROR in base instance_reference!  "
			<< e->where() << endl;
		exit(1);
	}
	count_ptr<simple_instance_reference>
		base_inst(base_obj.is_a<simple_instance_reference>());
	assert(base_inst);

	const bool ai = base_inst->attach_indices(index_list_obj);
	if (!ai) {
		cerr << ranges->where() << endl;
		exit(1);
	}
	// push indexed instance reference back onto stack
	c->push_object_stack(base_inst);
	return never_const_ptr<object>(NULL);
}

//=============================================================================
// class binary_expr method definitions

CONSTRUCTOR_INLINE
binary_expr::binary_expr(const expr* left, const terminal* o, 
		const expr* right) :
		expr(), l(left), op(o), r(right) {
	assert(l); assert(op); assert(r);
}

DESTRUCTOR_INLINE
binary_expr::~binary_expr() {
}

line_position
binary_expr::leftmost(void) const {
	return l->leftmost();
}

line_position
binary_expr::rightmost(void) const {
	return r->rightmost();
}

/** this should be abstract, not exist */
never_const_ptr<object>
binary_expr::check_build(never_ptr<context> c) const {
	never_const_ptr<object> lo, ro;
	cerr << "binary_expr::check_build(): FINISH ME!";
	lo = l->check_build(c);		// expect some object expression
	assert(lo);			// temporary
	ro = r->check_build(c);		// expect some object expression
	assert(ro);			// temporary
	// pop them off object stack
	// switch on operation
	return never_const_ptr<object>(NULL);
}

//=============================================================================
// class arith_expr method definitions

CONSTRUCTOR_INLINE
arith_expr::arith_expr(const expr* left, const terminal* o, 
		const expr* right) :
		binary_expr(left, o, right) {
}

DESTRUCTOR_INLINE
arith_expr::~arith_expr() { }

ostream&
arith_expr::what(ostream& o) const {
	return o << "(arith-expr)";
}

//=============================================================================
// class relational_expr method definitions

CONSTRUCTOR_INLINE
relational_expr::relational_expr(const expr* left, const terminal* o, 
		const expr* right) :
		binary_expr(left, o, right) {
}

DESTRUCTOR_INLINE
relational_expr::~relational_expr() { }

ostream&
relational_expr::what(ostream& o) const {
	return o << "(relational-expr)";
}

//=============================================================================
// class logical_expr method definitions

CONSTRUCTOR_INLINE
logical_expr::logical_expr(const expr* left, const terminal* o, 
		const expr* right) :
		binary_expr(left, o, right) {
}

DESTRUCTOR_INLINE
logical_expr::~logical_expr() { }

ostream&
logical_expr::what(ostream& o) const {
	return o << "(logical-expr)";
}

//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS -- entire classes
							// also known as...
template class node_list<expr,comma>;			// expr_list
template class node_list<token_identifier,scope>;	// qualified_id_base
template class node_list<range,comma>;			// range_list


//=============================================================================
};	// end namespace parser
};	// end namespace ART

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE


