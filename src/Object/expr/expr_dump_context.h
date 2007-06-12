/**
	\file "Object/expr/expr_dump_context.h"
	Flags and modifiers for manipulating expression dumps.  
	$Id: expr_dump_context.h,v 1.6 2007/06/12 05:12:41 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_EXPR_DUMP_CONTEXT_H__
#define	__HAC_OBJECT_EXPR_EXPR_DUMP_CONTEXT_H__

#include "util/memory/excl_ptr.h"
#include "util/member_saver.h"
#include "util/attributes.h"

namespace HAC {
namespace entity {
class scopespace;
using util::memory::never_ptr;

namespace PRS {
	struct expr_dump_context;
}

//=============================================================================
/**
	Contains contextual information relevant to dumping expressions.  
	Also contains modifier flags.  
 */
struct expr_dump_context {
	/**
		Operator precedence of the caller, 
		for sake of parenthesization.  
		What should default value be?
		See "Object/expr/operator_precedence.h" for list.  
	 */
	char					caller_stamp;
	/**
		Optional: set to non-null so instance references in
		the same scope don't print with fully qualified names.  
		May make things more readable.  
		TODO: this doesn't capture template parameters of 
		enclosing canonical type, only its base name.  
		e.g. a local name 'x' in definition 'A' becomes "A::x"
	 */
	never_ptr<const scopespace>		enclosing_scope;
	/**
		Optional: parent instance prefix string.
		NOTE: does not always result in canonical name, 
		but rather, an equivalent alias.  
		The choice to use a shallow char* was arbitrary, 
		the string is expected to be short-lived.  
		This takes precedence over enclosing_scope.  
		e.g. subinstance 'y' of 'x' becomes "x.y".
	 */
	const char*				parent_instance_name;
	/**
		Whether or not type information should be included
		with instance reference dumps.  
	 */
	bool					include_type_info;
	/**
		Whether or not parent operator is associative.  
		(Like +, *, &&, ||)
		The only dump operations that care are those 
		of arithmetic operations, which include 
		associative and non-associative operators.  
		Most of the time, the value of this won't matter.  
	 */
	bool					parent_associativity;

	expr_dump_context();

	// default copy-constructor

	// implicit
	expr_dump_context(const PRS::expr_dump_context&);

	explicit
	expr_dump_context(const scopespace*);

	explicit
	expr_dump_context(const char*);

	expr_dump_context(const char, const scopespace*, 
		const bool);

	~expr_dump_context();

	static const expr_dump_context		default_value;
	static const expr_dump_context		error_mode;
	static const expr_dump_context		brief;

	bool
	need_parentheses(const char, const bool assoc = false) const;


	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	/**
		Pass NULL to temporarily suppress parent instance.  
	 */
	typedef util::member_saver<expr_dump_context, const char*, 
			&expr_dump_context::parent_instance_name>
						parent_suppressor;

	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	/**
		For the duration of the scope, modifies the context, 
		even if it is const, but promises to restore it by the 
		end of the scope when the modifier object expires.  
		The point of this is to avoid repeated copy-modifying
		of expr_dump_context.  
	 */
	class stamp_modifier {
	private:
		expr_dump_context& 			_context;
		const char				_old_caller_stamp;
		const bool				_old_assoc;
	public:
		stamp_modifier(const expr_dump_context&, const char, 
			const bool = false);
		~stamp_modifier();
	} __ATTRIBUTE_UNUSED__;

};	// end struct expr_dump_context


//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_EXPR_DUMP_CONTEXT_H__

