/**
	\file "Object/inst/alias_matcher.h"
	$Id: alias_matcher.h,v 1.3.16.1 2010/01/12 02:48:47 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_ALIAS_MATCHER_H__
#define	__HAC_OBJECT_INST_ALIAS_MATCHER_H__

#include <string>
#include "Object/inst/alias_visitor.h"
#include "Object/common/cflat_args.h"
#include "util/tokenize_fwd.h"		// for util::string_list
#include "util/member_saver.h"

namespace HAC {
namespace entity {
using std::string;

//=============================================================================
struct alias_matcher_base : public alias_visitor, public cflat_args_base {
	/**
		The container of aliases in which to accumulate matches. 
	 */
	util::string_list&			aliases;
	const size_t				index;
	string					prefix;

protected:
	alias_matcher_base(
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		const state_manager& _sm, 
#endif
		const footprint& _f, 
		const footprint_frame* const _fpf, 
		util::string_list& _al, 
		const size_t _i, 
		const string& _p = string()) : alias_visitor(), 
		cflat_args_base(
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
			_sm, 
#endif
			_f, _fpf), aliases(_al), 
		index(_i), prefix(_p) {
	}

	~alias_matcher_base() { }

public:
	typedef	util::member_saver<alias_matcher_base, string, 
			&alias_matcher_base::prefix>
						save_prefix;

};	// end struct alias_matcher_base

//=============================================================================
/**
	Visitor class for printing matching aliases.  
	\param Tag the meta-class type: bool, int, proc...
 */
template <class Tag>
struct alias_matcher : public alias_matcher_base {

	alias_matcher(
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		const state_manager& _sm,
#endif
		const footprint& _f, 
		const footprint_frame* const _fpf, 
		util::string_list& _al, 
		const size_t _i, 
		const string& _p = string()) :
		alias_matcher_base(
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
			_sm,
#endif
			_f, _fpf, _al, _i, _p) {
	}

	// default dtor
	// default copy-ctor

	VISIT_INSTANCE_ALIAS_INFO_PROTOS()

private:
	// helper functions here
	template <class Tag2>
	void
	__visit(const instance_alias_info<Tag2>&);

	// non-copyable
	explicit
	alias_matcher(const alias_matcher&);

};	// end class alias_matcher

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_ALIAS_MATCHER_H__

