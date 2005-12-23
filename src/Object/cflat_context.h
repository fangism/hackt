/**
	\file "Object/cflat_context.h"
	Structure containing all the minimal information
	needed for a cflat traversal over instances.  
	$Id: cflat_context.h,v 1.1.2.1 2005/12/23 05:44:06 fang Exp $
 */

#ifndef	__HAC_OBJECT_CFLAT_CONTEXT_H__
#define	__HAC_OBJECT_CFLAT_CONTEXT_H__

#include <iosfwd>

namespace HAC {
namespace entity {
class footprint;
class footprint_frame;
class state_manager;
using std::ostream;

//=============================================================================
/**
	This structure contains references to the structures required
	to perform a meaninful traversal of the instance hierarchy, 
	such as for cflatting, or allocating global expressions.  
 */
struct cflat_context_base {
	const state_manager&			sm;
	const footprint&			fp;

	cflat_context_base(const state_manager& _sm, const footprint& _fp) :
		sm(_sm), fp(_fp) { }

};	// end struct cflat_context_base

//-----------------------------------------------------------------------------
/**
	Add a footprint frame when recursively traversing substructures
	such as processes.  
 */
struct cflat_context : public cflat_context_base {
	const footprint_frame&			fpf;

	cflat_context(const state_manager& _sm, const footprint& _fp, 
			const footprint_frame& _fpf) :
		cflat_context_base(_sm, _fp), fpf(_fpf) { }

};	// end struct cflat_context

//-----------------------------------------------------------------------------
#if 0
/**
 */
struct cflat_functor_base {
virtual	void
	operator () (...) = 0;
};	//

//-----------------------------------------------------------------------------
/**
	Functor for flattening production rules.  
	TODO: derive from some virtual base functor.  
 */
struct prs_cflat_printer {
	ostream&				os;
	const cflat_context&			context;
	const cflat_options&			cfopts;

	prs_cflat_printer(ostream& _os, const cflat_context&);

	void
	operator () (void);
};	// end struct prs_cflattener
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_CFLAT_CONTEXT_H__

