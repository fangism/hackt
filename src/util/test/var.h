// "var.h"
// dummy classes for testing pointer-classes and polymorphism

#ifndef	__VAR_H__
#define	__VAR_H__

#include <string>
using std::string;
//=============================================================================
// forward declaration
class var_base;
class var_anon;
class var_named;

//=============================================================================
class var_base {
protected:
	int val;
public:
explicit var_base(const int v) : val(v) { }
virtual	~var_base() { }

int	get_val(void) const { return val; }
virtual	string get_name(void) const = 0;
	var_base& operator = (const int k);
	var_base& operator = (const var_base& v);
	var_anon operator + (const var_base& v) const;
};	// end class var_base

//=============================================================================
class var_anon : public var_base {
public:
explicit var_anon(const int v) : var_base(v) { }
	~var_anon() { }

	string get_name(void) const { return "<anon>"; }
};	// end class var_anon

//=============================================================================
class var_named : public var_base {
protected:
	string	name;
public:
explicit var_named(const int v) : var_base(v), name("") { }
	var_named(const int v, const string& s) : var_base(v), name(s) { }
	~var_named() { }

	string get_name(void) const { return name; }
};	// end class var_named

//=============================================================================
// method definitions

var_base&
var_base::operator = (const int k) {
	val = k;
	return *this;
}

var_base&
var_base::operator = (const var_base& v) {
	val = v.val;
	return *this;
}

var_anon
var_base::operator + (const var_base& v) const {
	return var_anon(val + v.val);
}

//=============================================================================
#endif	//	__VAR_H__

