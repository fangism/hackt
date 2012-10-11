/**
	\file "PR/obstacle.h"
	$Id: obstacle.h,v 1.2 2011/05/03 19:20:46 fang Exp $
 */

#ifndef	__HAC_PR_OBSTACLE_H__
#define	__HAC_PR_OBSTACLE_H__

#include "PR/numerics.h"

namespace HAC {
namespace PR {

/**
	Obstacles can only be rectangular for now.
 */
struct obstacle {
	position_type			corner[2];
};	// end struct obstacle

}	// end namespace PR
}	// end namespace HAC

#endif	// __HAC_PR_OBSTACLE_H__

