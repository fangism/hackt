/**
	\file "Object/lang/CHP_footprint_expr.h"
	Expression structures for unrolled CHP constructs.  
	CHP expressions currently only operate on fundamental types
	bool and int<>.  
	$Id: CHP_footprint_expr.h,v 1.2 2006/06/26 01:46:18 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_CHP_FOOTPRINT_EXPR_H__
#define	__HAC_OBJECT_LANG_CHP_FOOTPRINT_EXPR_H__

#include "util/memory/excl_ptr.h"
#include <valarray>

namespace HAC {
namespace entity {
template <class Tag>	struct class_traits;

namespace CHP {
using util::memory::never_ptr;
//=============================================================================
/**
	Enumerations for unrolled CHP expression types.  
 */
enum {
	CHP_EXPR_INVALID = 0,
	CHP_EXPR_BOOL_CONST,	// can be resolved meta-value
	CHP_EXPR_INT_CONST,	// can be resolved meta-value
	CHP_EXPR_INT_ARITHMETIC,	// binary arithmetic integer operation
	CHP_EXPR_INT_RELATIONAL,	// binary relational integer comparison
	CHP_EXPR_INT_NEGATE,	// unary integer negation
	CHP_EXPR_LOGICAL,	// binary logical boolean operation
	CHP_EXPR_NOT,		// unary logical negation operation
	CHP_EXPR_META_BOOL_REF,	// reference to bound boolean instance
	CHP_EXPR_META_INT_REF,	// reference to bound integer instance
	CHP_EXPR_NONMETA_BOOL_REF,	// reference to unbound boolean instance
	CHP_EXPR_NONMETA_INT_REF	// reference to unbound integer instance
	// what about other data types, structs?
};

//=============================================================================
/**
	General node of footprint unroll-resolved expression.  
	A massive union of nonmeta expression types.  :S
 */
class fp_expr_entry {
	/**
		Encode the expression type.  
		Using the above enumerations.  
	 */
	unsigned char			type_enum;
	/**
		Type-dependent field to subclassify.
		e.g. used to encode operators.  
		BOOL_CONST: zero or non-zero represent true/false.
	 */
	unsigned char			sub_type;
	/**
		Primary index, whose meaning is dependent on the type_enum.  
		In the case of instance references, the indices
		refer to IDs assigned to the local footprint.  
		For unary expressions, this is just interpreted as the 
		index to the operand expession.  
		In the case of N-ary expressions, these
		are indices into the local footprint's Nary-expression pool.  
		For meta-references, this offset indexes directly
		into the local footprint's frame.  
		For nonmeta-references, this offset points to another
		nonmeta references pool.  
	 */
	size_t				index;
#if 0
	/**
		Second index is only used for binary expressions.  
		Otherwise, field is unused.  
	 */
	size_t				index2;
#endif

};	// end class fp_expr_entry

//=============================================================================
/**
	Placeholder for Nary expressions.  
	Contents are just indices to the expression pool.  
 */
typedef std::valarray<size_t>		fp_Nary_expr_node;

//=============================================================================
/**
	NOTE: not all subscripts need to be nonmeta, could be mixed!
	NOTE: a purely meta instance references will not need/use this, 
		because they can be resolved to a local ID at 
		unroll compile-time.  
 */
template <class Tag>
struct fp_subscript_ref {
	typedef	class_traits<Tag>	traits_type;
	
	/**
		Instance collection or value collection?
	 */
	typedef	typename Tag::template collection<Tag>::type
					collection_type;
	/**
		Points to the collection that is indexed.  
		Collection belongs to the local unrolled footprint, 
		as opposed to the definition's placeholder.  
	 */
	never_ptr<const collection_type>	base_ref;
	/**
		List of indices into general expression pool.  
	 */
	std::valarray<size_t>		subscripts;
};	// end struct fp_subscript_ref

//=============================================================================
}	// end namespace CHP
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_CHP_FOOTPRINT_EXPR_H__

