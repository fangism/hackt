// "art_object_expr.h"

#ifndef __ART_OBJECT_EXPR_H__
#define __ART_OBJECT_EXPR_H__

#include <string>
#include "art_object.h"

//=============================================================================
// note: need some way of hashing expression? 
//	using string of fully-qualified name?

//=============================================================================
namespace ART {
namespace entity {

using std::string;

//=============================================================================
// class forward declarations
class param_instantiation;

//============================================================================= 
/**
	Abstract base class for symbolic expressions to be written 
	to an object file.  
	Expression graphs must be directed-acyclic.  
	Deriving from object, so that it may be cached in 
	scopespaces' used_id_map.  
 */
class param_expr : public object {
public:
	param_expr() : object() { }
virtual	~param_expr() { }
virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const;		// temporary
virtual	string hash_string(void) const = 0;

/** is initialized if is resolved to constant or some other formal */
virtual bool is_initialized(void) const = 0;
virtual bool is_static_constant(void) const { return false; }
virtual int static_constant_int(void) const { assert(0); return -666; }
virtual bool static_constant_bool(void) const { assert(0); return false; }
virtual bool is_loop_independent(void) const { return false; }
virtual bool is_unconditional(void) const { return false; }
};	// end class param_expr

//-----------------------------------------------------------------------------
/**
	NOT SURE THIS CLASS IS USEFUL.  
	For arrays of expressions.
class param_expr_collective : public param_expr {
protected:
	list<excl_ptr<param_expr> >	elist;
public:
	param_expr_collective();
	~param_expr_collective();

	ostream& what(ostream& o) const;
	string hash_string(void) const;
};	// end class param_expr_collective
**/

//-----------------------------------------------------------------------------
/**
	A reference to single parameter instance.  
	Actually, can be collective too, just depends on var.  
 */
class param_literal : public param_expr {
protected:
	/**
		The referencing pointer to the parameter instance.  
		Is modifiable because parameters may be initialized later. 
	 */
	count_ptr<param_instance_reference>	var;
public:
	param_literal(count_ptr<param_instance_reference> v);
	~param_literal();

	ostream& what(ostream& o) const;
	string hash_string(void) const;
	// implement later.
	bool is_initialized(void) const;
	bool is_static_constant(void) const;
	void initialize(count_const_ptr<param_expr> i);
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
	bool is_initialized(void) const { return true; }
	bool is_static_constant(void) const { return true; }
	int static_constant_int(void) const { return val; }
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
	bool is_initialized(void) const { return true; }
	bool is_static_constant(void) const { return true; }
	bool static_constant_bool(void) const { return val; }
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
	bool is_initialized(void) const { return ex->is_initialized(); }
	bool is_static_constant(void) const { return ex->is_static_constant(); }
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
	bool is_initialized(void) const {
		return lx->is_initialized() && rx->is_initialized();
	}
	bool is_static_constant(void) const {
		return lx->is_static_constant() && rx->is_static_constant();
	}
};	// end class param_binary_expr

//=============================================================================
}	// end namespace ART
}	// end namespace entity

#endif	// __ART_OBJECT_EXPR_H__

