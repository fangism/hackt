// "art_object_expr.h"

#ifndef __ART_OBJECT_EXPR_H__
#define __ART_OBJECT_EXPR_H__

namespace ART {
namespace entity {
//=============================================================================
// class forward declarations

class param_instantiation;

//=============================================================================
/**
	Base class for symbolic expressions to be written to an object file.  
	Expression graphs must be directed-acyclic.  
 */
class param_expr {
protected:
// ownership flag, reference count?
//	mutable int ref_count;
public:
virtual	~param_expr() { }
};	// end class param_expr

//-----------------------------------------------------------------------------
/**
	A reference to single parameter instance.  
 */
class param_literal : public param_expr {
protected:
	/**
		The referencing pointer to the parameter.  
		Is read-only, and never deleted.  
	 */
	const param_instantiation*	var;
//	bool				init;		// initialized?
public:
	param_literal(const param_instantiation* v);
virtual	~param_literal();

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
	param_const_int(const long v);
virtual	~param_const_int() { }
};

//-----------------------------------------------------------------------------
class param_const_bool : public param_expr {
protected:
	const bool			val;
public:
	param_const_bool(const bool v);
virtual	~param_const_bool() { }
};

//-----------------------------------------------------------------------------
class param_unary_expr : public param_expr {
public:
protected:
	const char			op;
	const param_expr*		ex;
public:
	param_unary_expr(const char o, const param_expr* e);
	param_unary_expr(const param_expr* e, const char o);
virtual	~param_unary_expr();

};

//-----------------------------------------------------------------------------
class param_binary_expr : public param_expr {
public:
protected:
	const char			op;
	const param_expr*		lx;
	const param_expr*		rx;
public:
	param_binary_expr(const param_expr* l, const char o, 
		const param_expr* r);
virtual	~param_binary_expr();

};

//=============================================================================
};	// end namespace ART
};	// end namespace entity

#endif	// __ART_OBJECT_EXPR_H__

