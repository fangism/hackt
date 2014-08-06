/**
	\file "misc/sudoku/tableau.tcc"
	$Id: tableau.tcc,v 1.2 2006/01/22 06:53:23 fang Exp $
 */

#ifndef	__SUDOKU_TABLEAU_TCC__
#define	__SUDOKU_TABLEAU_TCC__

#include <iostream>
#include <algorithm>
#include "misc/sudoku/board.hh"
#include "misc/sudoku/rules.hh"
#include "misc/sudoku/pivot.hh"
#include "misc/sudoku/solve.hh"
#include "util/finite_vector.hh"

namespace sudoku {
using std::cerr;
using std::endl;
using std::for_each;
using util::finite_vector;
//=============================================================================
// class tableau method definitions

TABLEAU_TEMPLATE_SIGNATURE
TABLEAU_CLASS::tableau() : board(), rules() {
	rules.setup_board(this->board);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TABLEAU_TEMPLATE_SIGNATURE
TABLEAU_CLASS::~tableau() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This will modify the board, but only for the duration of
	this call.  
	TODO: pass solution collector.  
 */
TABLEAU_TEMPLATE_SIGNATURE
template <class SolutionsType>
void
TABLEAU_CLASS::solve(SolutionsType& sols) const {
	const_cast<TABLEAU_CLASS&>(*this).__solve(sols);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursive brute-force solution.  
	TODO: seek pivots in the board and the rule constraints.  
	TODO: need to define 2nd-order constraints (combinations)
		for smarter reasoning!
	TODO: be able to substitute pieces of the algorithm!
 */
TABLEAU_TEMPLATE_SIGNATURE
template <class SolutionsType>
void
TABLEAU_CLASS::__solve(SolutionsType& sols) {
	cell_pivot_finder<cell_type> pf;
	board.accept(pf);
	// rule_pivot_finder<cell_type>
	// rules.accept(pf);
if (pf.is_valid()) {
	// recurse
	typedef	typename cell_type::state_type	state_type;
	const state_type& pivot(pf.cell().top());
	// list of possible values:
	// TODO: use policy to determine finite-vector type (from state_type)
	typedef	finite_vector<uchar, 9> possible_values_type;
	possible_values_type possible_values;
	pivot.collect_possible_values(possible_values);
	for_each(possible_values.begin(), possible_values.end(), 
		subproblem_solver<board_type>(*this, pf.cell())
	);
} else {
	typedef	typename SolutionsType::value_type	solution_type;
	sols.push_back(solution_type());
	new (&sols.back()) solution_type(*this);
#if 1
	sols.back().dump(cerr << "SOLUTION " << sols.size() << ":" << endl);
#endif
}
}

//=============================================================================
}	// end namespace sudoku

#endif	// __SUDOKU_TABLEAU_TCC__

