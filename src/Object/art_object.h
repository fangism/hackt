/**
	\file "art_object.h"
	Includes most "art_object_*.h" headers.  
	Including this without thinking WILL unnecessarily slow down
	compilation.  
	$Id: art_object.h,v 1.7 2005/01/13 05:28:28 fang Exp $
 */

#ifndef	__ART_OBJECT_H__
#define	__ART_OBJECT_H__

// this list may be out-of-date, but maintaining this is 
// low priority because we carefully include only the necessary
// headers in our source files.  

#include "art_object_definition.h"
#include "art_object_type_ref.h"
#include "art_object_instance.h"
#include "art_object_instance_management_base.h"
#include "art_object_inst_ref.h"
#include "art_object_inst_stmt.h"
#include "art_object_expr.h"
#include "art_object_expr_param_ref.h"
#include "art_object_assign.h"
#include "art_object_connect.h"
#include "art_object_control.h"
#include "art_object_module.h"
#include "art_object_namespace.h"

#endif	// __ART_OBJECT_H__

