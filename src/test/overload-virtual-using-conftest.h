/**
	From configure test, this should pass both gcc and clang
	with proper compiler feature detection.
 */
#include "config.h"
class pint_const;
class const_param;
class nonmeta_index_expr_base {
public:
virtual ~nonmeta_index_expr_base();
virtual const pint_const* deep_copy(const nonmeta_index_expr_base*) const = 0;
};
typedef	nonmeta_index_expr_base	meta_index_expr;
class data_expr {
public:
virtual ~data_expr();
virtual const const_param* deep_copy(const data_expr*) const = 0;
};
class int_expr : virtual public nonmeta_index_expr_base, public data_expr {
public:
virtual ~int_expr();
virtual const const_param* deep_copy(const int_expr*) const = 0;
 const const_param* deep_copy(const data_expr*) const;
 const pint_const* deep_copy(const nonmeta_index_expr_base*) const;
};
class pint_expr :
  virtual public meta_index_expr,
  public int_expr {
public:
virtual ~pint_expr();
 const const_param* deep_copy(const int_expr*) const;
protected:
 using int_expr::deep_copy;
#if OVERLOAD_VIRTUAL_USING
 using meta_index_expr::deep_copy;	// clang requires, gcc forbids
#endif
};
