/**
	\file "art_parser_expr.cc"
	Class method definitions for ART::parser, related to expressions.  
	$Id: art_parser_expr.cc,v 1.4 2004/11/02 07:51:38 fang Exp $
 */

#include <iostream>

#include "art_parser_token.h"
#include "art_parser_expr.h"
#include "art_parser.tcc"

// will need these come time for type-checking
// #include "art_symbol_table.h"	// this file is context-independent!
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
// class expr_list method definitions

expr_list::expr_list() : parent() { }

expr_list::expr_list(const expr* e) : parent(e) { }

expr_list::~expr_list() { }

ostream&
expr_list::what(ostream& o) const {
	return o << "(expr-list)";
}

/**
	Type-checker checks each individual expression and 
	returns a collection of built object expressions.  
	NULL members will be represented with NULL object 
	expression place-holders.  
	Caller just has to grab the object_list off the stack
	and go from there.  
 */
never_const_ptr<object>
expr_list::check_build(never_ptr<context> c) const {
	count_ptr<object_list> o(new object_list);
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		if (*i) {
			(*i)->check_build(c);
			// ignore useless return values (should be always NULL)
			o->push_back(c->pop_top_object_stack());
			// do error checking on list elsewhere
		} else {
			// add NULL placeholder
			o->push_back(count_ptr<object>(NULL));
		}
	}
	c->push_object_stack(o);
	return never_const_ptr<object>(NULL);
}

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
qualified_id::qualified_id(const token_identifier* n) : 
	parent(n), absolute(NULL) {
}

/// copy constructor, no transfer of ownership
CONSTRUCTOR_INLINE
qualified_id::qualified_id(const qualified_id& i) :
		node(), qualified_id_base(i), absolute(NULL) {
#if DEBUG_ID_EXPR
	cerr << "qualified_id::qualified_id(const qualified_id&);" << endl;
#endif
	if (i.absolute) {
		absolute = excl_const_ptr<token_string>(
			new token_string(*i.absolute));
		// actually *copy* the token
		assert(absolute);
	}
}

DESTRUCTOR_INLINE
qualified_id::~qualified_id() {
}

/**
	Call this function in the parser to mark an un/qualified identifier
	as absolute, as oppposed to relative.  
	See class definition of qualified_id for an explanation.  
	\param s should be a scope (::) token.  
	\return pointer to this object
 */
qualified_id*
qualified_id::force_absolute(const token_string* s) {
	absolute = excl_const_ptr<token_string>(s);
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
	These might be obsoleted by the sublist slice interface.  
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
	\return a pointer to a definition_base or an instance_collection_base 
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

qualified_id*
id_expr::force_absolute(token_string* s) {
	return qid->force_absolute(s);
}

bool
id_expr::is_absolute(void) const {
	return qid->is_absolute();
}

/**
	UPDATE DOCUMENTATION.
	The qualified_id member's check build can return a definition 
	or instance pointer.  (Only instance pointer now.)
	\param c the context where to begin searching for named object.  
	\return pointer to the found instantiation base instance if found,
		else NULL.
	FIX ME: should return instance_reference!, not instance!
	ACTUALLY: instance_base, caller will wrap into instance reference. 
 */
never_const_ptr<object>
id_expr::check_build(never_ptr<context> c) const {
	never_const_ptr<object> o;
	never_const_ptr<instance_collection_base> inst;
	o = qid->check_build(c);		// will lookup_object
	if (o) {
		inst = o.is_a<instance_collection_base>();
		if (inst) {
			// we found an instance which may be single
			// or collective... info is in inst.
			c->push_object_stack(inst->make_instance_reference());
			// pushes the created reference onto
			// context's instance_reference_stack.
			// if indexed, check in the caller, and modify
			//	in index_expr...

			// doesn't have to be a parameter, does it?
		} else {
			// push NULL or error object to continue?
			cerr << "object \"" << *qid <<
				"\" is not an instance, ERROR!  "
				<< qid->where() << endl;
			exit(1);
		}
	} else {
		// push NULL or error object to continue?
		cerr << "object \"" << *qid << "\" not found, ERROR!  "
			<< qid->where() << endl;
		exit(1);
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

/**
	Does basic checking on expression.  
	Grabs last expression off top of stack and replaces it.  
	Always returns NULL, rather useless.  
 */
never_const_ptr<object>
prefix_expr::check_build(never_ptr<context> c) const {
	e->check_build(c);	// useless return value
	count_ptr<object> o(c->pop_top_object_stack());
	if (!o) {
		// error propagates up the stack
		cerr << "ERROR building expression at " << e->where() << endl;
		c->push_object_stack(count_ptr<object>(NULL));
		return never_const_ptr<object>(NULL);
	}
	count_ptr<param_expr> pe(o.is_a<param_expr>());
	assert(pe);	// must be a param expression!
	count_ptr<pint_expr> ie(pe.is_a<pint_expr>());
	count_ptr<pbool_expr> be(pe.is_a<pbool_expr>());

	const int ch = op.is_a<token_char>()->get_char();
	switch(ch) {
		case '-':
			// integer negation
			if (!ie) {
				cerr << "Unary \'-\' operator requires a "
					"pint argument, but got a ";
				pe->what(cerr) << ".  ERROR!  "
					<< e->where() << endl;
				c->push_object_stack(count_ptr<object>(NULL));
				break;
			}
			if (ie->is_static_constant()) {
				// constant simplification
				c->push_object_stack(count_ptr<pint_const>(
					new pint_const(
						- ie->static_constant_int())));
			} else {
				c->push_object_stack(count_ptr<pint_unary_expr>(
					new pint_unary_expr(ch, ie)));
			}
			break;
		case '!':
			// integer logical negation
			if (!ie) {
				cerr << "Unary \'!\' operator requires a "
					"pint argument, but got a ";
				pe->what(cerr) << ".  ERROR!  "
					<< e->where() << endl;
				c->push_object_stack(count_ptr<object>(NULL));
				break;
			}
			if (ie->is_static_constant()) {
				// constant simplification
				c->push_object_stack(count_ptr<pint_const>(
					new pint_const(
						! ie->static_constant_int())));
			} else {
				c->push_object_stack(count_ptr<pint_unary_expr>(
					new pint_unary_expr(ch, ie)));
			}
			break;
		case '~':
			// context-dependent? in PRS or not?
			// is bit-wise negation for ints, 
			// logical negation for bools?
			// for now, restrict to bools only...
			if (!be) {
				cerr << "Unary \'~\' operator requires a "
					"pint argument, but got a ";
				pe->what(cerr) << ".  ERROR!  "
					<< e->where() << endl;
				c->push_object_stack(count_ptr<object>(NULL));
				break;
			}
			if (be->is_static_constant()) {
				// constant simplification
				c->push_object_stack(count_ptr<pbool_const>(
					new pbool_const(
						!be->static_constant_bool())));
			} else {
				c->push_object_stack(
					count_ptr<pbool_unary_expr>(
						new pbool_unary_expr(be, ch)));
			}
			break;
		default:
			cerr << "Bad operator char \'" << ch << "\' in "
				"prefix_expr::check_build()!" << endl;
			assert(0);
	}
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
	Current restriction: left expression must be scalar 0-dimensional.
	\return NULL, but places an instance_reference object on the
		context's object stack.  
 */
never_const_ptr<object>
member_expr::check_build(never_ptr<context> c) const {
	e->check_build(c);
	// useless return value
	// expect: simple_instance_reference on object stack
	count_const_ptr<object> o(c->pop_top_object_stack());
	if (!o) {
		cerr << "ERROR in base instance reference of member expr at "
			<< e->where() << endl;
		exit(1);
	}
	count_const_ptr<simple_instance_reference>
		inst_ref(o.is_a<simple_instance_reference>());
	assert(inst_ref);
	if (inst_ref->dimensions()) {
		cerr << "ERROR: cannot take the member of a " <<
			inst_ref->dimensions() << "-dimension array, "
			"must be scalar!  (for now...)  " <<
			e->where() << endl;
		exit(1);
	}

	never_const_ptr<definition_base>
		base_def(inst_ref->get_base_def());
	assert(base_def);
	c->push_current_definition_reference(*base_def);
	// this should return a reference to an instance
	// of some type that has members, such as data, channel, process.  
	// should resolve to a *single* instance of something, 
	// cannot be an array.  

	// use that instance_reference, get its referenced definition_base, 
	// and make sure it has a member m, lookup ports only in the 
	// current_definition_reference, don't lookup anywhere else!

	// don't use context's general lookup
	never_const_ptr<instance_collection_base>
		member_inst(base_def->lookup_port_formal(*member));
	// LATER: check and make sure definition is signed, 
	//	after we introduce forward template declarations
	if (!member_inst) {
		base_def->what(cerr << "ERROR: ") << " " <<
			base_def->get_qualified_name() << 
			" has no public member named \"" << *member <<
			"\" at " << member->where() << endl;
		exit(1);
	}
	c->push_object_stack(
		member_inst->make_member_instance_reference(inst_ref));
	// useless return value: NULL
	// will result in member_instance_reference on object_stack

	// reset definition reference in context
	c->pop_current_definition_reference();

	// what should this return?  the same thing it expects:
	// a reference to an instance of some type.  
	// Problem: instances aren't concrete until they are unrolled.
	// What is available in type-check phase?
	// Maybe we don't care about the instances themselves, 
	// rather the *type* returned.  
	// after all this is type-checking, not range checking.  

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

	// later this may be a member_instance_reference...
	// should cast to instance_reference_base instead, 
	// abstract attach_indices
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

#if 0
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
#endif

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

never_const_ptr<object>
arith_expr::check_build(never_ptr<context> c) const {
	l->check_build(c);	// useless return value
	r->check_build(c);	// useless return value
	count_ptr<object> ro(c->pop_top_object_stack());
	count_ptr<object> lo(c->pop_top_object_stack());
	if (!ro || !lo) {
		if (!lo)
			cerr << "ERROR building expression at " << 
				l->where() << endl;
		if (!ro)
			cerr << "ERROR building expression at " << 
				r->where() << endl;
		c->push_object_stack(count_ptr<object>(NULL));
		return never_const_ptr<object>(NULL);
	}
	count_ptr<pint_expr> li(lo.is_a<pint_expr>());
	count_ptr<pint_expr> ri(ro.is_a<pint_expr>());
	if (!li || !ri) {
		if (!li) {
			cerr << "ERROR arith_expr expected a pint, but got a ";
			lo->what(cerr) << " at " << l->where() << endl;;
		}
		if (!ri) {
			cerr << "ERROR arith_expr expected a pint, but got a ";
			ro->what(cerr) << " at " << r->where() << endl;;
		}
		c->push_object_stack(count_ptr<object>(NULL));
		return never_const_ptr<object>(NULL);
	}
	// else is safe to make arith_expr object
	const char ch = op.is_a<token_char>()->get_char();
	if (li->is_static_constant() && ri->is_static_constant()) {
		const int lc = li->static_constant_int();
		const int rc = ri->static_constant_int();
		switch(ch) {
			case '+':
				c->push_object_stack(count_ptr<pint_const>(
					new pint_const(lc +rc)));
				break;
			case '-':
				c->push_object_stack(count_ptr<pint_const>(
					new pint_const(lc -rc)));
				break;
			case '*':
				c->push_object_stack(count_ptr<pint_const>(
					new pint_const(lc *rc)));
				break;
			case '/':
				c->push_object_stack(count_ptr<pint_const>(
					new pint_const(lc /rc)));
				break;
			case '%':
				c->push_object_stack(count_ptr<pint_const>(
					new pint_const(lc %rc)));
				break;
			default:
				cerr << "Bad operator char \'" << ch << "\' in "
					"arith_expr::check_build()!" << endl;
				assert(0);
		}
	} else {
		c->push_object_stack(count_ptr<entity::arith_expr>(
			new entity::arith_expr(li, ch, ri)));
	}
	return never_const_ptr<object>(NULL);
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

never_const_ptr<object>
relational_expr::check_build(never_ptr<context> c) const {
	// temporary
	return node::check_build(c);
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

never_const_ptr<object>
logical_expr::check_build(never_ptr<context> c) const {
	// temporary
	return node::check_build(c);
}

//=============================================================================
// class array_concatenation method definitions

array_concatenation::array_concatenation(const expr* e) : expr(), parent(e) {
	assert(e);
}

array_concatenation::~array_concatenation() {
}

ostream&
array_concatenation::what(ostream& o) const {
	return o << "(array-concatenation)";
}

line_position
array_concatenation::leftmost(void) const {
	return parent::leftmost();
}

line_position
array_concatenation::rightmost(void) const {
	return parent::rightmost();
}

/**
	If list contains only a single element, don't bother 
	constructing an aggregate object on the stack, 
	just do the check_build of the lone object.  
 */
never_const_ptr<object>
array_concatenation::check_build(never_ptr<context> c) const {
	if (size() == 1) {
		const const_iterator only = begin();
		return (*only)->check_build(c);
	} else {
		return node::check_build(c);
	}
}

//=============================================================================
// class loop_concatenation method definitions

loop_concatenation::loop_concatenation(
		const token_char* l, const token_char* h,
		const token_char* c1, const token_identifier* i,   
		const token_char* c2, const range* rng,
		const token_char* c3, const expr* e,
		const token_char* r) :
		lp(l), pd(h), col1(c1), id(i), col2(c2),
		bounds(rng), col3(c3), ex(e), rp(r) {
	assert(id); assert(bounds); assert(ex);
}
		
loop_concatenation::~loop_concatenation() {
}

ostream&
loop_concatenation::what(ostream& o) const {
	return o << "(loop-concatenation)";
}

line_position
loop_concatenation::leftmost(void) const {
	if (lp)		return lp->leftmost();
	else if (pd)	return pd->leftmost();
	else if (col1)	return col1->leftmost();
	else		return id->leftmost();
}

line_position
loop_concatenation::rightmost(void) const {
	if (rp)		return rp->rightmost();
	else 		return ex->rightmost();
}

never_const_ptr<object>
loop_concatenation::check_build(never_ptr<context> c) const {
	return node::check_build(c);
}

//=============================================================================
// class array_construction method definitions

array_construction::array_construction(const token_char* l,
		const expr* e, const token_char* r) : 
		expr(), lb(l), ex(e), rb(r) {
	assert(ex);
}

array_construction::~array_construction() {
}

ostream&
array_construction::what(ostream& o) const {
	return o << "(array-construction)";
}

line_position
array_construction::leftmost(void) const {
	if (lb)		return lb->leftmost();
	else		return ex->leftmost();
}

line_position
array_construction::rightmost(void) const {
	if (rb)		return rb->rightmost();
	else		return ex->rightmost();
}

never_const_ptr<object>
array_construction::check_build(never_ptr<context> c) const {
	return node::check_build(c);
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


