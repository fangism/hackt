/**
	\file "Object/art_object.h"
	Includes most "art_object_*.h" headers.  
	Including this without thinking WILL unnecessarily slow down
	compilation.  
	$Id: art_object.h,v 1.8.14.1 2005/07/05 07:59:32 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_H__
#define	__OBJECT_ART_OBJECT_H__

// this list may be out-of-date, but maintaining this is 
// low priority because we carefully include only the necessary
// headers in our source files.  

#include "Object/art_object_definition.h"
#include "Object/art_object_type_ref.h"
#include "Object/art_object_instance.h"
#include "Object/art_object_instance_management_base.h"
#include "Object/art_object_inst_ref.h"
#include "Object/art_object_inst_stmt.h"
// #include "Object/art_object_expr.h"
	// replaced with a bunch of headers
#include "Object/art_object_expr_param_ref.h"
#include "Object/art_object_assign.h"
#include "Object/art_object_connect.h"
#include "Object/art_object_control.h"
#include "Object/art_object_module.h"
#include "Object/art_object_namespace.h"

#endif	// __OBJECT_ART_OBJECT_H__

