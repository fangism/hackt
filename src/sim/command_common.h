/**
	\file "sim/command_common.h"
	Common commands that are trivially different (by state type)
	in various simulators.  We define them as trivial templates
	for ease of re-use.  
	To use, just instantiate template.  
	$Id: command_common.h,v 1.3 2007/07/31 23:23:30 fang Exp $
 */

#ifndef	__HAC_SIM_COMMAND_COMMON_H__
#define	__HAC_SIM_COMMAND_COMMON_H__

#include <iosfwd>
#include "util/size_t.h"
#include "util/tokenize_fwd.h"

namespace HAC {
namespace SIM {
using std::ostream;
using util::string_list;
template <class> class Command;
template <class> class command_category;
template <class> class command_registry;

/**
	TODO: factor usage() out into base class function?
 */
#define	DECLARE_COMMON_COMMAND_CLASS(_class)				\
template <class State>							\
class _class {								\
public:									\
	typedef	State				state_type;		\
	typedef	Command<state_type>		command_type;		\
	typedef typename command_type::command_category_type		\
						command_category_type;	\
	typedef	command_registry<command_type>	command_registry_type;	\
	static const char			name[];			\
	static const char			brief[];		\
	static command_category_type&		category;		\
	static int	main(state_type&, const string_list&);		\
	static void	usage(ostream&);				\
private:								\
	static const size_t			receipt_id;		\
};	// end class _class

//=============================================================================
DECLARE_COMMON_COMMAND_CLASS(All)
DECLARE_COMMON_COMMAND_CLASS(Alias)
DECLARE_COMMON_COMMAND_CLASS(UnAlias)
DECLARE_COMMON_COMMAND_CLASS(UnAliasAll)
DECLARE_COMMON_COMMAND_CLASS(Aliases)

DECLARE_COMMON_COMMAND_CLASS(Source)
DECLARE_COMMON_COMMAND_CLASS(AddPath)
DECLARE_COMMON_COMMAND_CLASS(Paths)

DECLARE_COMMON_COMMAND_CLASS(Initialize)
DECLARE_COMMON_COMMAND_CLASS(Reset)
DECLARE_COMMON_COMMAND_CLASS(Queue)
DECLARE_COMMON_COMMAND_CLASS(Save)
DECLARE_COMMON_COMMAND_CLASS(Load)
DECLARE_COMMON_COMMAND_CLASS(Who)
DECLARE_COMMON_COMMAND_CLASS(What)
DECLARE_COMMON_COMMAND_CLASS(Time)
DECLARE_COMMON_COMMAND_CLASS(AssertQueue)
DECLARE_COMMON_COMMAND_CLASS(WatchQueue)
DECLARE_COMMON_COMMAND_CLASS(NoWatchQueue)

DECLARE_COMMON_COMMAND_CLASS(DLOpen)
DECLARE_COMMON_COMMAND_CLASS(DLPaths)
DECLARE_COMMON_COMMAND_CLASS(DLAddPath)
// DECLARE_COMMON_COMMAND_CLASS(DLClose)
DECLARE_COMMON_COMMAND_CLASS(DLCheckFunc)
DECLARE_COMMON_COMMAND_CLASS(DLAssertFunc)
DECLARE_COMMON_COMMAND_CLASS(DLFuncs)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generally helpful class -- ha ha.  
 */
template <class State>
class Help {
public:
	typedef	State				state_type;
	typedef	Command<state_type>		command_type;
	typedef typename command_type::command_category_type
						command_category_type;
	typedef	command_registry<command_type>	command_registry_type;
	static const char			name[];
	static const char			brief[];
	static command_category_type&		category;
	static int	main(const string_list&);
	static int	main(state_type&, const string_list&);
	static void	usage(ostream&);
private:
	static const size_t			receipt_id;
};	// end class Help

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_COMMAND_COMMON_H__

