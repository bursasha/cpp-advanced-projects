#ifndef __PROGTEST__
#include <cassert>
#include <cstdint>
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
#include <random>

using ChristmasTree = size_t;

struct TreeProblem {
  int max_group_size;
  std::vector<uint64_t> gifts;
  std::vector<std::pair<ChristmasTree, ChristmasTree>> connections;
};

#endif
using namespace std;
uint64_t solveRecursive(vector<vector<ChristmasTree>>& streetsConns, vector<pair<bool, bool>>& streetsInfo, vector<pair<uint64_t, uint64_t>>& streetsResults, const vector<uint64_t>& gifts, ChristmasTree street, bool agent = false)
{
	uint64_t giftsCount = agent ? gifts.at(street) : 0; streetsInfo.at(street).second = agent;
	for (const auto& s : streetsConns.at(street))
	{
		if (!(streetsInfo.at(s).first))
		{
			streetsInfo.at(s).first = true;
			if (agent) { if (streetsResults.at(s).second) giftsCount += streetsResults.at(s).second; else giftsCount += solveRecursive(streetsConns, streetsInfo, streetsResults, gifts, s); }
			else { vector<pair<bool, bool>> streetsInfoSecond = streetsInfo;
				if (streetsResults.at(s).first && streetsResults.at(s).second) giftsCount += max(streetsResults.at(s).first, streetsResults.at(s).second);
				else if (streetsResults.at(s).first && !(streetsResults.at(s).second)) giftsCount += max(streetsResults.at(s).first, solveRecursive(streetsConns, streetsInfoSecond, streetsResults, gifts, s));
				else if (!(streetsResults.at(s).first) && (streetsResults.at(s).second)) giftsCount += max(solveRecursive(streetsConns, streetsInfo, streetsResults, gifts, s, true), streetsResults.at(s).second);
				else giftsCount += max(solveRecursive(streetsConns, streetsInfo, streetsResults, gifts, s, true), solveRecursive(streetsConns, streetsInfoSecond, streetsResults, gifts, s)); }
		}
	}
	if (agent) streetsResults.at(street).first = giftsCount; else streetsResults.at(street).second = giftsCount;
	return giftsCount;
}
uint64_t solve(const TreeProblem& t)
{
	vector<pair<bool, bool>> streetsInfoEven(t.gifts.size(), {false,false}), streetsInfoOdd(t.gifts.size(), {false,false}); // visited, agent
	vector<vector<ChristmasTree>> streetsConns(t.gifts.size()); ChristmasTree start = 0; streetsInfoEven.at(start).first = streetsInfoOdd.at(start).first = true;
	for (const auto& c : t.connections) { streetsConns.at(c.first).push_back(c.second); streetsConns.at(c.second).push_back(c.first); }
	vector<pair<uint64_t, uint64_t>> streetsResults(t.gifts.size(), {0, 0}); // agent/no agent counts
	return max(solveRecursive(streetsConns, streetsInfoEven, streetsResults, t.gifts, start, true), solveRecursive(streetsConns, streetsInfoOdd, streetsResults, t.gifts, start));
}
#ifndef __PROGTEST__

using TestCase = std::pair<uint64_t, TreeProblem>;

const std::vector<TestCase> BASIC_TESTS = {
  { 3, { 1, { 1, 1, 1, 2 }, { {0,3}, {1,3}, {2,3} }}},
  { 4, { 1, { 1, 1, 1, 4 }, { {0,3}, {1,3}, {2,3} }}},
  { 57, { 1, {
    17, 11, 5, 13, 8, 12, 7, 4, 2, 8,
  }, {
    {1, 4}, {6, 1}, {2, 1}, {3, 8}, {8, 0}, {6, 0}, {5, 6}, {7, 2}, {0, 9},
  }}},
  { 85, { 1, {
    10, 16, 13, 4, 19, 8, 18, 17, 18, 19, 10,
  }, {
    {9, 7}, {9, 6}, {10, 4}, {4, 9}, {7, 1}, {0, 2}, {9, 2}, {3, 8}, {2, 3}, {5, 4},
  }}},
  { 79, { 1, {
    8, 14, 11, 8, 1, 13, 9, 14, 15, 12, 1, 11,
  }, {
    {9, 1}, {1, 2}, {1, 4}, {5, 10}, {7, 8}, {3, 7}, {11, 3}, {11, 10}, {6, 8}, {0, 1}, {0, 3},
  }}},
  { 102, { 1, {
    15, 10, 18, 18, 3, 4, 18, 12, 6, 19, 9, 19, 10,
  }, {
    {10, 2}, {11, 10}, {6, 3}, {10, 8}, {5, 3}, {11, 1}, {9, 5}, {0, 4}, {12, 3}, {9, 7}, {11, 9}, {4, 12},
  }}},
  { 93, { 1, {
    1, 7, 6, 18, 15, 2, 14, 15, 18, 8, 15, 1, 5, 6,
  }, {
    {0, 13}, {6, 12}, {0, 12}, {7, 8}, {8, 3}, {12, 11}, {12, 1}, {10, 12}, {2, 6}, {6, 9}, {12, 7}, {0, 4}, {0, 5},
  }}},
};

const std::vector<TestCase> BONUS_TESTS = {
  { 3, { 2, { 1, 1, 1, 2 }, { {0,3}, {1,3}, {2,3} }}},
  { 5, { 2, { 1, 1, 1, 4 }, { {0,3}, {1,3}, {2,3} }}},
};

void test(const std::vector<TestCase>& T) {
  int i = 0;
  for (auto &[s, t] : T) {
    if (s != solve(t))
      std::cout << "Error in " << i << " (returned " << solve(t) << ")"<< std::endl;
    i++;
  }
  std::cout << "Finished" << std::endl;
}

int main() {
  test(BASIC_TESTS);
  // test(BONUS_TESTS);
}

#endif


