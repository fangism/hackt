/**
	\file "sim/command_common.hh"
	Common commands that are trivially different (by state type)
	in various simulators.  We define them as trivial templates
	for ease of re-use.  
	To use, just instantiate template.  
	TODO: many of these functions only depend on the module
	and not the state, and could be refactored accordingly.
	$Id: command_common.hh,v 1.14 2010/07/07 23:01:25 fang Exp $
 */

#ifndef	__HAC_SIM_COMMAND_COMMON_H__
#define	__HAC_SIM_COMMAND_COMMON_H__

#include <iosfwd>
#include "util/size_t.h"
#include "util/tokenize_fwd.hh"
#include "sim/command_macros.hh"

namespace HAC {
namespace SIM {
using std::ostream;
using util::string_list;
template <class> class Command;
template <class> class command_category;
template <class> class command_registry;

//=============================================================================
DECLARE_COMMON_STATELESS_COMMAND_CLASS(Echo)
DECLARE_COMMON_STATELESS_COMMAND_CLASS(CommentPound)
DECLARE_COMMON_STATELESS_COMMAND_CLASS(CommentComment)
DECLARE_COMMON_STATELESS_COMMAND_CLASS(Exit)
DECLARE_COMMON_STATELESS_COMMAND_CLASS(Quit)
DECLARE_COMMON_STATELESS_COMMAND_CLASS(Abort)
DECLARE_COMMON_STATELESS_COMMAND_CLASS(Precision)

DECLARE_COMMON_COMMAND_CLASS(All)
DECLARE_COMMON_COMMAND_CLASS(Alias)
DECLARE_COMMON_COMMAND_CLASS(UnAlias)
DECLARE_COMMON_COMMAND_CLASS(UnAliasAll)
DECLARE_COMMON_COMMAND_CLASS(Aliases)
DECLARE_COMMON_COMMAND_CLASS(Interpret)
DECLARE_COMMON_COMMAND_CLASS(EchoCommands)
DECLARE_COMMON_COMMAND_CLASS(MeasTime)
DECLARE_COMMON_COMMAND_CLASS(Repeat)
DECLARE_COMMON_COMMAND_CLASS(History)
DECLARE_COMMON_COMMAND_CLASS(HistoryNonInteractive)
DECLARE_COMMON_COMMAND_CLASS(HistorySave)
DECLARE_COMMON_COMMAND_CLASS(HistoryRerun)

DECLARE_COMMON_COMMAND_CLASS(ChangeDir)
DECLARE_COMMON_COMMAND_CLASS(PushDir)
DECLARE_COMMON_COMMAND_CLASS(PopDir)
DECLARE_COMMON_COMMAND_CLASS(Dirs)
DECLARE_COMMON_COMMAND_CLASS(WorkingDir)

DECLARE_COMMON_COMMAND_CLASS(Source)
DECLARE_COMMON_COMMAND_CLASS(AddPath)
DECLARE_COMMON_COMMAND_CLASS(Paths)

DECLARE_COMMON_COMMAND_CLASS(Initialize)
DECLARE_COMMON_COMMAND_CLASS(Reset)
DECLARE_COMMON_COMMAND_CLASS(Queue)
DECLARE_COMMON_COMMAND_CLASS(Save)
DECLARE_COMMON_COMMAND_CLASS(Load)
DECLARE_COMMON_COMMAND_CLASS(AutoSave)

DECLARE_COMMON_COMMAND_CLASS(Who)
DECLARE_COMMON_COMMAND_CLASS(WhoNewline)
DECLARE_COMMON_COMMAND_CLASS(What)
DECLARE_COMMON_COMMAND_CLASS(LS)	// as in UNIX: ls

DECLARE_COMMON_COMMAND_CLASS(Time)
DECLARE_COMMON_COMMAND_CLASS(Seed48)
DECLARE_COMMON_COMMAND_CLASS(Confirm)
DECLARE_COMMON_COMMAND_CLASS(NoConfirm)
DECLARE_COMMON_COMMAND_CLASS(AssertQueue)
DECLARE_COMMON_COMMAND_CLASS(AssertNQueue)
DECLARE_COMMON_COMMAND_CLASS(WatchQueue)
DECLARE_COMMON_COMMAND_CLASS(NoWatchQueue)
DECLARE_COMMON_COMMAND_CLASS(WatchAllQueue)
DECLARE_COMMON_COMMAND_CLASS(NoWatchAllQueue)

DECLARE_COMMON_COMMAND_CLASS(TimeFmt)

// loading and handling of dynamic libraries
DECLARE_COMMON_STATELESS_COMMAND_CLASS(DLOpen)
DECLARE_COMMON_STATELESS_COMMAND_CLASS(DLPaths)
DECLARE_COMMON_STATELESS_COMMAND_CLASS(DLAddPath)
// DECLARE_COMMON_STATELESS_COMMAND_CLASS(DLClose)
DECLARE_COMMON_STATELESS_COMMAND_CLASS(DLCheckFunc)
DECLARE_COMMON_STATELESS_COMMAND_CLASS(DLAssertFunc)
DECLARE_COMMON_STATELESS_COMMAND_CLASS(DLFuncs)

// trace file interface
DECLARE_COMMON_COMMAND_CLASS(Trace)
DECLARE_COMMON_COMMAND_CLASS(TraceFile)
DECLARE_COMMON_COMMAND_CLASS(TraceClose)
DECLARE_COMMON_COMMAND_CLASS(TraceFlushNotify)
DECLARE_COMMON_COMMAND_CLASS(TraceFlushInterval)
DECLARE_COMMON_COMMAND_CLASS(TraceDump)

DECLARE_COMMON_COMMAND_CLASS(VCD)
DECLARE_COMMON_COMMAND_CLASS(VCDFile)
DECLARE_COMMON_COMMAND_CLASS(VCDClose)
DECLARE_COMMON_COMMAND_CLASS(VCDTimeScale)
#if 0
DECLARE_COMMON_COMMAND_CLASS(VCDFlushNotify)	// ?
DECLARE_COMMON_COMMAND_CLASS(VCDFlushInterval)	// ?
DECLARE_COMMON_COMMAND_CLASS(VCDDump)		// ?
#endif

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
	static const command_completer		completer;
private:
	static const size_t			receipt_id;
};	// end class Help

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_COMMAND_COMMON_H__

