// "art_object_expr.h"

#ifndef __ART_OBJECT_EXPR_H__
#define __ART_OBJECT_EXPR_H__

#include <string>
using std::string;
#include "art_object.h"

//=============================================================================
// note: need some way of hashing expression? 
//	using string of fully-qualified name?

//=============================================================================
namespace ART {
namespace entity {
//=============================================================================
// class forward declarations
class param_instantiation;

/**
	Array dimensions are specified as a list of parameter expressions.  
	Range expressions also allowed.
	Expressions may contain other literals referring to other parameters,
	and need not necessarily be constants.
	Non-constants will be checked at instantiation time.
	Elements should be param_expr (owned pointers).
 */
typedef list_of_ptr<param_expr>			array_dim_list;

//============================================================================= 
/**
	Abstract base class for symbolic expressions to be written 
	to an object file.  
	Expression graphs must be directed-acyclic.  
	Deriving from object, so that it may be cached in 
	scopespaces' used_id_map.  
 */
class param_expr : public object {
protected:
// ownership flag, reference count?
//	mutable int ref_count;
public:
	param_expr() : object() { }
virtual	~param_expr() { }
virtual	ostream& what(ostream& o) const = 0;
virtual	string hash_string(void) const = 0;
};	// end class param_expr

//-----------------------------------------------------------------------------
/**
	For arrays of expressions.
 */
class param_expr_collective : public param_expr {
protected:
	list_of_ptr<param_expr>		elist;
public:
	param_expr_collective();
	~param_expr_collective();

	ostream& what(ostream& o) const;
	string hash_string(void) const;
};	// end class param_expr_collective

//-----------------------------------------------------------------------------
/**
	A reference to single parameter instance.  
 */
class param_literal : public param_expr {
protected:
	/**
		The referencing pointer to the parameter instance.  
		Is read-only, and never deleted.  
	 */
	const param_instance_reference*	var;
//	bool				init;		// initialized?
public:
	param_literal(const param_instance_reference& v);
	~param_literal();

	ostream& what(ostream& o) const;
	string hash_string(void) const;
};	// end class param_literal

//-----------------------------------------------------------------------------
/**
	Constant integer parameters.  
	Currently limited in width by the machine's long size.  
 */
class param_const_int : public param_expr {
protected:
	const long			val;
public:
	param_const_int(const long v) : param_expr(), val(v) { }
	~param_const_int() { }
	ostream& what(ostream& o) const;
	string hash_string(void) const;
};	// end class param_const_int

//-----------------------------------------------------------------------------
/**
	Constant boolean parameters, true or false.  
 */
class param_const_bool : public param_expr {
protected:
	const bool			val;
public:
	param_const_bool(const bool v) : param_expr(), val(v) { }
	~param_const_bool() { }
	ostream& what(ostream& o) const;
	string hash_string(void) const;
};	// end class param_const_bool

//-----------------------------------------------------------------------------
/**
	Unary parameter expressions.  
 */
class param_unary_expr : public param_expr {
public:
protected:
	const char			op;
	const param_expr*		ex;
public:
	param_unary_expr(const char o, const param_expr* e);
	param_unary_expr(const param_expr* e, const char o);
	~param_unary_expr() { }
	ostream& what(ostream& o) const;
	string hash_string(void) const;
};	// end class param_unary_expr

//-----------------------------------------------------------------------------
/**
	Binary parameter expressions.  
 */
class param_binary_expr : public param_expr {
public:
protected:
	const char			op;
	const param_expr*		lx;
	const param_expr*		rx;
public:
	param_binary_expr(const param_expr* l, const char o, 
		const param_expr* r);
	~param_binary_expr() { }
	ostream& what(ostream& o) const;
	string hash_string(void) const;
};	// end class param_binary_expr

//=============================================================================
};	// end namespace ART
};	// end namespace entity

#endif	// __ART_OBJECT_EXPR_H__

