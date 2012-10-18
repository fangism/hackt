/**
	\file "Object/expr/data_expr.hh"
	Base classes for data expressions.  
	NOTE: for revision histry purposes, this file was copied from the
		last revision of "Object/art_object_data_expr_base.h"
		on the HACXX-00-01-04-main-00-48-connect-01 branch, 
		branch revision -11.
	TODO: future rename this file to nonmeta_expr_base.h
	$Id: data_expr.hh,v 1.11 2007/07/31 23:23:12 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_DATA_EXPR_H__
#define	__HAC_OBJECT_EXPR_DATA_EXPR_H__

#include "util/persistent.hh"
#include "util/memory/pointer_classes_fwd.hh"
// unroll_resolve_copy is only needed because CHP assignments
// are not (yet) subtyped, if we ever update this, we can trim here
#include "Object/type/canonical_type_fwd.hh"

#define	USE_DATA_EXPR_EQUIVALENCE	0

namespace HAC {
namespace entity {
class data_type_reference;
class unroll_context;		// from "Object/unroll/unroll_context.hh"
class nonmeta_context_base;	// from "Object/nonmeta_context.hh"
struct expr_dump_context;	// from "Object/expr/expr_dump_context.hh"
class nonmeta_expr_visitor;
class const_param;
struct channel_data_writer;	// from "Object/nonmeta_channel_manipulator.hh"
using std::ostream;
using util::persistent;
using util::memory::count_ptr;
//=============================================================================
/**
	Base class for all datatype expressions.  
	Datatype expressions are classified as non-meta expressions, 
	i.e. their values are only determined at run-time.  
	It is always safe to accept a meta expression in place of a 
	nonmeta expression.  
	TODO: perhaps call this nonmeta_data_expr for clarity?
 */
class data_expr : virtual public persistent {
protected:
	data_expr() : persistent() { }
public:
virtual	~data_expr() { }

virtual	ostream&
	what(ostream&) const = 0;

virtual	ostream&
	dump(ostream&, const expr_dump_context&) const = 0;

virtual	size_t
	dimensions(void) const = 0;

#define	GET_UNRESOLVED_DATA_TYPE_REF_PROTO				\
	count_ptr<const data_type_reference>				\
	get_unresolved_data_type_ref(void) const

virtual	GET_UNRESOLVED_DATA_TYPE_REF_PROTO = 0;

// should be canonical data type
#define	GET_RESOLVED_DATA_TYPE_REF_PROTO				\
	canonical_generic_datatype					\
	get_resolved_data_type_ref(const unroll_context&) const

virtual	GET_RESOLVED_DATA_TYPE_REF_PROTO = 0;

#if USE_DATA_EXPR_EQUIVALENCE
#define	DATA_EXPR_MAY_EQUIVALENCE_PROTO					\
	bool may_be_type_equivalent(const data_expr&) const

virtual	DATA_EXPR_MAY_EQUIVALENCE_PROTO = 0;
#endif

#define	UNROLL_RESOLVE_COPY_DATA_PROTO					\
	count_ptr<const data_expr>					\
	unroll_resolve_copy(const unroll_context&, 			\
		const count_ptr<const data_expr>&) const

virtual	UNROLL_RESOLVE_COPY_DATA_PROTO = 0;


#define	NONMETA_RESOLVE_COPY_DATA_PROTO					\
	count_ptr<const const_param>					\
	nonmeta_resolve_copy(const nonmeta_context_base&,		\
		const count_ptr<const data_expr>&) const

virtual	NONMETA_RESOLVE_COPY_DATA_PROTO = 0;

#define	EVALUATE_WRITE_PROTO						\
	void								\
	evaluate_write(const nonmeta_context_base&, 			\
		channel_data_writer&, 					\
		const count_ptr<const data_expr>&) const
virtual	EVALUATE_WRITE_PROTO = 0;

#define	EXPR_ACCEPT_VISITOR_PROTO					\
	void								\
	accept(nonmeta_expr_visitor&) const

virtual	EXPR_ACCEPT_VISITOR_PROTO = 0;

	struct unroller;
	struct resolver;

};	// end class data_expr

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_DATA_EXPR_H__

