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

const object*
paren_expr::check_build(context* c) const {
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
		qualified_id_base(i), absolute(NULL) {
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
//	absolute = IS_A(token_string*, s);
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
const object*
qualified_id::check_build(context* c) const {
	return c->lookup_object(*this).unprotected_const_ptr();
//	return c->lookup_object(*this);
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

id_expr::id_expr(const id_expr& i) : expr(), qid(new qualified_id(*i.qid)) {
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
const object*
id_expr::check_build(context* c) const {
	const object* o;
	const instantiation_base* inst = NULL;
	o = qid->check_build(c);		// will lookup_object
	if (o) {
		inst = IS_A(const instantiation_base*, o);
		if (inst) {
			// we found an instance which may be single
			// or collective...
			// c->?
			return inst->make_instance_reference(*c);
			// doesn't have to be a parameter, does it?
			// return new param_literal(*inst);
		} else {
			cerr << "object " << *qid <<
				" is not an instance, ERROR!";
		}
	} else {
		cerr << "object " << *qid << " not found, ERROR!";
	}
	return NULL;
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
//	assert(!IS_A(range*, lower));
	assert(!lower.is_a<range>());
}

CONSTRUCTOR_INLINE
range::range(const expr* l, const terminal* o, const expr* u) : 
		lower(l), op(o), upper(u) {
	assert(lower); assert(op); assert(u);
//	assert(!IS_A(range*, lower));
//	assert(!IS_A(range*, upper));
	assert(!lower.is_a<range>());
	assert(!upper.is_a<range>());
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
	TO DO: finish me
	\param c the context where to start resolving identifiers.  
	\return I don't know.
 */
const object*
range::check_build(context* c) const {
	cerr << "range::check_build(): INCOMPLETE, FINISH ME!" << endl;
	const object* o;
//	const param_type_reference* pint_type = 
//		IS_A(const param_type_reference*, 
//			c->global_namespace->lookup_object("pint"));
	never_const_ptr<param_type_reference> pint_type = c->global_namespace->
		lookup_object("pint").is_a<param_type_reference>();
	assert(pint_type);
	o = lower->check_build(c);
	assert(o);
	if (upper)
		o = upper->check_build(c);
	// not done yet
	return o;
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

const object*
prefix_expr::check_build(context* c) const {
	cerr << "prefix_expr::check_build(): I'm not done yet!" << endl;
	e->check_build(c);
	return NULL;
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
const object*
member_expr::check_build(context* c) const {
	const object* o;

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
	return NULL;
}

//=============================================================================
// class index_expr method definitions

CONSTRUCTOR_INLINE
index_expr::index_expr(const expr* l, const range_list* i) :
		postfix_expr(l, NULL),
		ranges(i) {
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
	TO DO: finish me
 */
const object*
index_expr::check_build(context* c) const {
	cerr << "index_expr::check_build(): FINISH ME!" << endl;
	const object* o;
	o = e->check_build(c);
	// expect: collective_type_reference
	const collective_instance_reference* cir = 
		IS_A(const collective_instance_reference*, o);
	// check each of the ranges from left to right

	assert(cir);			// temporary
	return NULL;
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

const object*
binary_expr::check_build(context* c) const {
	const object *lo, *ro;
	cerr << "binary_expr::check_build(): FINISH ME!";
	lo = l->check_build(c);		// expect some object expression
	assert(lo);			// temporary
	ro = r->check_build(c);		// expect some object expression
	assert(ro);			// temporary
	// switch on operation
	return NULL;
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


