/**
	\file "art_object_inst_stmt_data.h"
	Contains definition of nested, specialized class_traits types.  
	$Id: art_object_inst_stmt_data.h,v 1.1.2.1 2005/03/07 23:28:48 fang Exp $
 */

#ifndef	__ART_OBJECT_INST_STMT_DATA_H__
#define	__ART_OBJECT_INST_STMT_DATA_H__

#include "art_object_classification_details.h"
#include "art_object_type_ref_base.h"

namespace ART {
namespace entity {
//=============================================================================
class class_traits<datatype_tag>::instantiation_statement_type_ref_base {
protected:
	type_ref_ptr_type				type;
protected:
	explicit
	instantiation_statement_type_ref_base(
		const type_ref_ptr_type& t) : type(t) { }

	count_ptr<const fundamental_type_reference>
	get_type(void) const { return type; }

};      // end class instantiation_statement_type_ref_base

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INST_STMT_DATA_H__

