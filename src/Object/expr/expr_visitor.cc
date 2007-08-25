/**
	\file "Object/expr/expr_visitor.cc"
	$Id: expr_visitor.cc,v 1.4.6.1 2007/08/25 08:12:14 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include "Object/expr/expr_visitor.h"
#include "Object/expr/pbool_expr.h"
#include "Object/expr/pint_expr.h"
#include "Object/expr/preal_expr.h"
#include "Object/expr/const_param.h"
#include "Object/expr/nonmeta_index_expr_base.h"
#include "Object/expr/nonmeta_range_expr_base.h"
#include "Object/expr/param_expr_list.h"
#include "Object/expr/meta_range_expr.h"
#include "Object/expr/meta_range_list.h"
#include "Object/expr/meta_index_expr.h"
#include "Object/expr/meta_index_list.h"

#include "Object/expr/pint_range.h"
#include "Object/expr/const_range.h"

#include "Object/expr/nonmeta_index_list.h"
// #include "Object/expr/nonmeta_range_list.h"
#include "Object/expr/nonmeta_expr_list.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/expr/dynamic_param_expr_list.h"
#include "Object/expr/const_range_list.h"
#include "Object/expr/dynamic_meta_range_list.h"
#include "Object/expr/const_index_list.h"
#include "Object/expr/dynamic_meta_index_list.h"
#include "Object/expr/nonmeta_func_call.h"

#include "common/ICE.h"
#include "util/memory/excl_ptr.h"	// for never_ptr
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
//=============================================================================
// class nonmeta_expr_visitor method definitions

/**
	This really should not be called...
 */
#define	DEFINE_CATCH_ALL_EXPR_VISITOR(type)				\
void									\
nonmeta_expr_visitor::visit(const type& e) {				\
	STACKTRACE_VERBOSE;						\
	e.accept(*this);						\
}

DEFINE_CATCH_ALL_EXPR_VISITOR(data_expr)	// is this a good idea?
// DEFINE_CATCH_ALL_EXPR_VISITOR(bool_expr)
// DEFINE_CATCH_ALL_EXPR_VISITOR(int_expr)
// DEFINE_CATCH_ALL_EXPR_VISITOR(real_expr)
DEFINE_CATCH_ALL_EXPR_VISITOR(nonmeta_index_expr_base)
DEFINE_CATCH_ALL_EXPR_VISITOR(nonmeta_range_expr_base)
DEFINE_CATCH_ALL_EXPR_VISITOR(param_expr)
DEFINE_CATCH_ALL_EXPR_VISITOR(const_param)
DEFINE_CATCH_ALL_EXPR_VISITOR(param_expr_list)
DEFINE_CATCH_ALL_EXPR_VISITOR(meta_index_expr)
// DEFINE_CATCH_ALL_EXPR_VISITOR(pbool_expr)
// DEFINE_CATCH_ALL_EXPR_VISITOR(pint_expr)
// DEFINE_CATCH_ALL_EXPR_VISITOR(preal_expr)
DEFINE_CATCH_ALL_EXPR_VISITOR(meta_range_expr)
DEFINE_CATCH_ALL_EXPR_VISITOR(meta_range_list)
DEFINE_CATCH_ALL_EXPR_VISITOR(meta_index_list)

#undef	DEFINE_CATCH_ALL_EXPR_VISITOR

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	DEFINE_LIST_PTR_VISITOR(type)					\
void									\
nonmeta_expr_visitor::visit(const type& l) {				\
	STACKTRACE_VERBOSE;						\
	typedef	type::const_iterator	const_iterator;			\
	const_iterator i(l.begin()), e(l.end());			\
	for ( ; i!=e; ++i) {						\
		if (*i)							\
			(*i)->accept(*this);				\
	}								\
}

#define	DEFINE_LIST_REF_VISITOR(type)					\
void									\
nonmeta_expr_visitor::visit(const type& l) {				\
	STACKTRACE_VERBOSE;						\
	typedef	type::const_iterator	const_iterator;			\
	const_iterator i(l.begin()), e(l.end());			\
	for ( ; i!=e; ++i) {						\
		i->accept(*this);					\
	}								\
}

DEFINE_LIST_PTR_VISITOR(nonmeta_index_list)
// DEFINE_LIST_PTR_VISITOR(nonmeta_range_list)
DEFINE_LIST_PTR_VISITOR(nonmeta_expr_list)
DEFINE_LIST_PTR_VISITOR(const_param_expr_list)
DEFINE_LIST_PTR_VISITOR(dynamic_param_expr_list)
DEFINE_LIST_REF_VISITOR(const_range_list)
DEFINE_LIST_PTR_VISITOR(dynamic_meta_range_list)
DEFINE_LIST_PTR_VISITOR(const_index_list)
DEFINE_LIST_PTR_VISITOR(dynamic_meta_index_list)

#undef	DEFINE_LIST_PTR_VISITOR
#undef	DEFINE_LIST_REF_VISITOR

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

