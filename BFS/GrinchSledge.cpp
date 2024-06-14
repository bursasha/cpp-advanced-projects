#include <cassert>
#include <iostream>
#include <memory>
#include <limits>
#include <optional>
#include <algorithm>
#include <bitset>
#include <list>
#include <array>
#include <vector>
#include <deque>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>

// ---------------------------------------------------------------------------------------------------------------------

using Place = size_t;

// ---------------------------------------------------------------------------------------------------------------------

// Structure representing the map with all the necessary details
struct Map {
  size_t places; // Number of rooms
  Place start, end; // Start and end rooms
  std::vector<std::pair<Place, Place>> connections; // List of pairs of rooms describing corridors
  std::vector<std::vector<Place>> items; // List of lists, items[i] is a list of rooms where the i-th component is located
};

// ---------------------------------------------------------------------------------------------------------------------

// Custom hash function for pairs to be used in unordered containers
template < typename F, typename S >
struct std::hash<std::pair<F, S>> {
  std::size_t operator () (const std::pair<F, S> &p) const noexcept {
    // something like boost::combine would be much better
    return std::hash<F>()(p.first) ^ (std::hash<S>()(p.second) << 1);
  }
};

// ---------------------------------------------------------------------------------------------------------------------

using namespace std;
using State = string;// State representation as a string
using Graph = unordered_map<size_t, pair<vector<size_t>, set<size_t>>>; // vertex: items, neighbours

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Creates the graph from the given connections and items.
 *
 * @param connections List of pairs of places describing corridors
 * @param items List of lists, items[i] is a list of rooms where the i-th component is located
 * @return The created graph
 */
Graph create_graph (const vector<pair<Place, Place>> &connections, const vector<vector<Place>> &items)
{
	Graph graph;

	for (const auto &connection : connections)
	{ graph[connection.first].second.insert (connection.second); graph[connection.second].second.insert (connection.first); }

	for (size_t item = 0; item < items.size (); item++) { for (const size_t &place : items[item]) { graph[place].first.push_back (item); } }

	return graph;
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Updates the state by collecting items at the current vertex.
 *
 * @param stateActual The current state
 * @param itemsOfVertex The items at the current vertex
 * @return The updated state
 */
State create_state (const State &stateActual, const vector<size_t> &itemsOfVertex)
{ State stateNew = stateActual; for (const auto &item : itemsOfVertex) { stateNew[item] = '1'; } return stateNew; }

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Finds the shortest path in the map that collects at least one component of each type.
 *
 * @param map The map with rooms, connections, and items
 * @return A list of places representing the shortest path, or an empty list if no such path exists
 */
list<Place> find_path(const Map &map)
{
	Graph graph = create_graph (map.connections, map.items);
	const size_t itemsCount = map.items.size ();
	State stateStart = string (itemsCount, '0'); State stateStartNew = create_state (stateStart, graph[map.start].first);
	State stateEndFull = to_string (map.end) + '_' + string (itemsCount, '1');

	unordered_map<State, State> paths; paths.emplace (to_string (map.start) + '_' + stateStartNew, "");

	queue<pair<size_t, State>> verticesToVisit; // vertex, state
	verticesToVisit.push (make_pair (map.start, stateStartNew));

	while (!verticesToVisit.empty ())
	{
		if (paths.count (stateEndFull)) break;

		auto vertexActual = verticesToVisit.front (); verticesToVisit.pop ();

		for (const auto &neighbour : graph[vertexActual.first].second)
		{
			State stateNew = create_state (vertexActual.second, graph[neighbour].first);
			State stateNewFull = to_string (neighbour) + '_' + stateNew, stateParentFull = to_string (vertexActual.first) + '_' + vertexActual.second;

			if (!paths.count (stateNewFull))
			{ paths.emplace (stateNewFull, stateParentFull); verticesToVisit.push (make_pair (neighbour, stateNew)); }
		}
	}

	list<Place> pathShortest = {};

	if (paths.count (stateEndFull))
	{
		for (auto vertexWithState = stateEndFull; ; )
		{
			State stateToGet; for (auto number : vertexWithState) { if (number == '_') break; stateToGet += number; }
			Place vertex = size_t(stoi (stateToGet));
			pathShortest.push_front (vertex);

			if (paths[vertexWithState].empty ()) break;

			vertexWithState = paths[vertexWithState];
		}
	}

	return pathShortest;
}

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

using TestCase = std::pair<size_t, Map>;

// ---------------------------------------------------------------------------------------------------------------------

// Class template argument deduction exists since C++17 :-)
const std::array examples = {
  TestCase{ 1, Map{ 2, 0, 0,
    { { 0, 1 } },
    { { 0 } }
  }},
  TestCase{ 3, Map{ 2, 0, 0,
    { { 0, 1 } },
    { { 1 } }
  }},
  TestCase{ 3, Map{ 4, 0, 1,
    { { 0, 2 }, { 2, 3 }, { 0, 3 }, { 3, 1 } },
    {}
  }},
  TestCase{ 4, Map{ 4, 0, 1,
    { { 0, 2 }, { 2, 3 }, { 0, 3 }, { 3, 1 } },
    { { 2 } }
  }},
  TestCase{ 0, Map{ 4, 0, 1,
    { { 0, 2 }, { 2, 3 }, { 0, 3 }, { 3, 1 } },
    { { 2 }, {} }
  }},
};

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

int main() {
  int fail = 0;
  for (size_t i = 0; i < examples.size(); i++) {
    auto sol = find_path(examples[i].second);
    if (sol.size() != examples[i].first) {
      std::cout << "Wrong answer for map " << i << std::endl;
      fail++;
    }
  }

  if (fail) std::cout << "Failed " << fail << " tests" << std::endl;
  else std::cout << "All tests completed" << std::endl;

  return 0;
}
