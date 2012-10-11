/**
	\file "priority_queue_test.cc"
	Proving whether or not order can be preserved.  
	$Id: priority_queue_test.cc,v 1.2 2008/03/17 23:03:21 fang Exp $
 */

#include <queue>
#include <iostream>
#include <map>

using std::cout;
using std::endl;
using std::priority_queue;
using std::multimap;

typedef	size_t 				int_value_type;

struct entry : std::pair<int, int_value_type> {
	typedef	std::pair<int, int_value_type>	parent_type;

	entry(const int a, const int_value_type b) : parent_type(a,b) { }

	bool
	operator < (const entry& r) const {
		return first > r.first;
	}
};

typedef	priority_queue<entry>	queue_type;
typedef	multimap<int, int_value_type>	map_type;

int
main(int, char*[]) {
	queue_type q;
	map_type m;
	int_value_type i = 0;
	for ( ; i<20; ++i) {
		q.push(entry(0, i));
		m.insert(std::make_pair(0, i));
	}
	cout << "priority queue:" << endl;
	i = 0;
	for ( ; i<20; ++i) {
		cout << q.top().second << endl;
		q.pop();
	}
	cout << "multimap:" << endl;
	map_type::const_iterator
		j(m.begin()), e(m.end());
	for ( ; j!=e; ++j) {
		cout << j->second << endl;
	}
	return 0;
}

