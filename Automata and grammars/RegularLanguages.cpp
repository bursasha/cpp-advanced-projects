#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <variant>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

using namespace std;
using State = unsigned int;
using Symbol = uint8_t;

// ---------------------------------------------------------------------------------------------------------------------

// Structure to represent a Non-deterministic Finite Automaton (NFA)
struct NFA {
    std::set<State> m_States; // Set of states
    std::set<Symbol> m_Alphabet; // Set of alphabet symbols
    std::map<std::pair<State, Symbol>, std::set<State>> m_Transitions; // Transition function
    State m_InitialState; // Initial state
    std::set<State> m_FinalStates; // Set of final states
};

// Structure to represent a Deterministic Finite Automaton (DFA)
struct DFA {
    std::set<State> m_States; // Set of states
    std::set<Symbol> m_Alphabet; // Set of alphabet symbols
    std::map<std::pair<State, Symbol>, State> m_Transitions; // Transition function
    State m_InitialState; // Initial state
    std::set<State> m_FinalStates; // Set of final states
};

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Makes the given NFA complete by adding a "fail" state to handle missing transitions.
 *
 * @param nfa The input NFA.
 * @param alphabetSecond The alphabet of the second NFA.
 * @return NFA The complete NFA.
 */
NFA make_complete(const NFA& nfa, const set<Symbol>& alphabetSecond)
{
	set<Symbol> alphabetComplete = nfa.m_Alphabet; alphabetComplete.insert(alphabetSecond.begin(), alphabetSecond.end());
	if (nfa.m_Transitions.size() == (nfa.m_States.size() * alphabetComplete.size())) return nfa;

	NFA nfaComplete = nfa; nfaComplete.m_Alphabet = alphabetComplete; State fail = 9999; nfaComplete.m_States.insert(fail);
	for (const auto& state : nfaComplete.m_States)
	{
		for (const auto& symbol : nfaComplete.m_Alphabet)
		{
			pair<State, Symbol> transition = make_pair(state, symbol);
			if (!(nfa.m_Transitions.count(transition))) nfaComplete.m_Transitions.insert({transition, {fail}});
		}
	}

	return nfaComplete;
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Creates a parallel run NFA for unification or intersection.
 *
 * @param aComplete The first complete NFA.
 * @param bComplete The second complete NFA.
 * @param type True for unification, false for intersection.
 * @return NFA The resulting NFA.
 */
NFA make_parallel_run(const NFA& aComplete, const NFA& bComplete, bool type)
{
	queue<pair<State, State>> statesToRun; // state of Union, (state of A, state of B)
	set<State> statesInitial, statesFinal, states; map<pair<State, State>, State> statesVisited; State tmp = 1;
	set<Symbol> alphabet;
	pair<State, State> stateUnified = { aComplete.m_InitialState, bComplete.m_InitialState };
	map<pair<State, Symbol>, set<State>> transitions;

	alphabet = aComplete.m_Alphabet; states.insert(tmp); statesVisited.insert(make_pair(stateUnified, tmp));
	statesToRun.push( stateUnified);

	while (!(statesToRun.empty()))
	{
		auto stateToRun = statesToRun.front();

		if (type)
		{
			if (stateToRun.first == aComplete.m_InitialState || stateToRun.second == bComplete.m_InitialState) statesInitial.insert(statesVisited.at(stateToRun));
			if (aComplete.m_FinalStates.count(stateToRun.first) || bComplete.m_FinalStates.count(stateToRun.second)) statesFinal.insert(statesVisited.at(stateToRun));
		}
		else
		{
			if (stateToRun.first == aComplete.m_InitialState && stateToRun.second == bComplete.m_InitialState) statesInitial.insert(statesVisited.at(stateToRun));
			if (aComplete.m_FinalStates.count(stateToRun.first) && bComplete.m_FinalStates.count(stateToRun.second)) statesFinal.insert(statesVisited.at(stateToRun));
		}

		for (const auto& a : alphabet)
		{
			for (const auto& s1 : aComplete.m_Transitions.at({stateToRun.first, a}))
			{
				for (const auto& s2 : bComplete.m_Transitions.at({stateToRun.second, a}))
				{
					stateUnified = {s1, s2};

					if (!(statesVisited.count(stateUnified)))
					{
						statesVisited.insert(make_pair(stateUnified, ++tmp));
						states.insert(statesVisited.at(stateUnified));
						statesToRun.push(stateUnified);
					}

					pair<State, Symbol> transition = make_pair(statesVisited.at(stateToRun), a);
					transitions[transition].insert(statesVisited.at(stateUnified));
				}
			}
		}

		statesToRun.pop();
	}

	State stateInitial = *statesInitial.begin();

	return NFA {states, alphabet, transitions, stateInitial, statesFinal};
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Converts an NFA to a DFA.
 *
 * @param n The input NFA.
 * @return DFA The resulting DFA.
 */
DFA make_determined(const NFA& n)
{
	queue<set<State>> statesToRun;
	set<State> states, statesFinal; map<set<State>, State> statesVisited; map<pair<State, Symbol>, State> transitions;
	set<Symbol> alphabet = n.m_Alphabet; set<State> stateUnified = { n.m_InitialState }; State stateInitial, tmp = 0;

	states.insert(tmp); statesVisited.insert(make_pair(stateUnified, tmp)); stateInitial = tmp;
	statesToRun.push(stateUnified);

	while(!(statesToRun.empty()))
	{
		auto stateToRun = statesToRun.front();

		for (const auto& s : stateToRun) { if (n.m_FinalStates.count(s)) { statesFinal.insert(statesVisited.at(stateToRun)); break; } }

		for (const auto& a : alphabet)
		{
			stateUnified.clear();

			for (const auto& s : stateToRun)
			{ pair<State, Symbol> transition = make_pair(s, a); stateUnified.insert(n.m_Transitions.at(transition).begin(),n.m_Transitions.at(transition).end()); }

			if (!(statesVisited.count(stateUnified)))
			{
				statesVisited.insert(make_pair(stateUnified, ++tmp));
				states.insert(statesVisited.at(stateUnified));
				statesToRun.push(stateUnified);
			}

			pair<State, Symbol> transition = make_pair(statesVisited.at(stateToRun), a);
			transitions[transition] = statesVisited.at(stateUnified);
		}

		statesToRun.pop();
	}

	return DFA {states, alphabet, transitions, stateInitial, statesFinal};
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Filters the reachable states in a DFA.
 *
 * @param d The input DFA.
 * @return DFA The resulting DFA with only reachable states.
 */
DFA make_reachable(const DFA& d)
{
	queue<State> statesToRun; statesToRun.push(d.m_InitialState);
	set<State> statesReachable = {d.m_InitialState};

	while (!(statesToRun.empty()))
	{
		for (const auto& a : d.m_Alphabet)
		{ auto transition = make_pair(statesToRun.front(), a);
			if (!(statesReachable.count(d.m_Transitions.at(transition))))
			{ statesReachable.insert(d.m_Transitions.at(transition)); statesToRun.push(d.m_Transitions.at(transition)); } }

		statesToRun.pop();
	}

	set<State> statesUnreachable; set_difference(d.m_States.begin(), d.m_States.end(), statesReachable.begin(), statesReachable.end(),
												 inserter(statesUnreachable, statesUnreachable.end()));

	if (statesUnreachable.empty()) return d;

	set<State> states = statesReachable, statesFinal; map<pair<State, Symbol>, State> transitions = d.m_Transitions; set<Symbol> alphabet = d.m_Alphabet;
	State fail = 9999;
	set_difference(d.m_FinalStates.begin(), d.m_FinalStates.end(), statesUnreachable.begin(), statesUnreachable.end(),
	               inserter(statesFinal, statesFinal.end()));

	for (auto& t : transitions) {  if (statesUnreachable.count(t.second)) transitions[t.first] = fail; }
	for (const auto& a : alphabet) { for (const auto& u : statesUnreachable) { pair<State, Symbol> transition = make_pair(u, a); transitions.erase(transition); } }

	return DFA {states, alphabet, transitions, d.m_InitialState, statesFinal};
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Filters the useful states in a DFA.
 *
 * @param d The input DFA.
 * @return DFA The resulting DFA with only useful states.
 */
DFA make_useful(const DFA& d)
{
	queue<State> statesToRun; for (const auto& s : d.m_FinalStates) statesToRun.push(s);
	set<State> statesUseful = d.m_FinalStates;

	while (!(statesToRun.empty()))
	{
		for (const auto& t : d.m_Transitions)
		{ if (t.second == statesToRun.front()) { if (!(statesUseful.count(t.first.first))) { statesUseful.insert(t.first.first); statesToRun.push(t.first.first); } } }

		statesToRun.pop();
	}

	set<State> statesUnuseful; set_difference(d.m_States.begin(), d.m_States.end(), statesUseful.begin(), statesUseful.end(),
	                                             inserter(statesUnuseful, statesUnuseful.end()));

	if (statesUnuseful.empty()) return d;

	set<State> states = statesUseful, statesFinal = d.m_FinalStates; map<pair<State, Symbol>, State> transitions = d.m_Transitions; set<Symbol> alphabet = d.m_Alphabet;
	State fail = 9999;

	for (auto& t : transitions) { if (statesUnuseful.count(t.second)) transitions[t.first] = fail; }
	for (const auto& a : alphabet) { for (const auto& u : statesUnuseful) { pair<State, Symbol> transition = make_pair(u, a); transitions.erase(transition); } }

	return DFA {states, alphabet, transitions, d.m_InitialState, statesFinal};
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Minimizes a DFA.
 *
 * @param d The input DFA.
 * @return DFA The minimized DFA.
*/
DFA make_minimized(const DFA& d)
{
	DFA dReachable = make_reachable(d); DFA dUseful = make_useful(dReachable);

	if (dUseful.m_Transitions.empty() || dUseful.m_FinalStates.empty()) return DFA {{dUseful.m_InitialState}, dUseful.m_Alphabet, dUseful.m_Transitions, dUseful.m_InitialState, dUseful.m_FinalStates};

	map<pair<State, Symbol>, State> transitions = dUseful.m_Transitions;
	set<Symbol> alphabet = dUseful.m_Alphabet; Symbol symbolLast = *(--alphabet.end()); State fail = 9999;
	map<State, State> statesNaming; statesNaming.insert(make_pair(fail, fail));
	for (const auto& s : dUseful.m_States) { if (dUseful.m_FinalStates.count(s)) statesNaming.insert(make_pair(s, 0)); else statesNaming.insert(make_pair(s, 1)); }
	map<State, vector<pair<pair<State, Symbol>, State>>> statesMinimized;
	for (const auto& t : transitions) statesMinimized[statesNaming.at(t.first.first)].push_back(make_pair(t.first, t.second));
	queue<map<State, vector<pair<pair<State, Symbol>, State>>>> groupsToRun; groupsToRun.push(statesMinimized);
	map<vector<State>, set<State>> statesOfGroupToChangeTmp;

	while (!(groupsToRun.empty()))
	{
		auto groupToRun = groupsToRun.front(); groupsToRun.pop();
		map<vector<State>, set<State>> statesOfGroupToChange;

		for (const auto& g : groupToRun)
		{
			set<pair<pair<State, Symbol>, State>> transitionMinimized;

			for (const auto& t : g.second)
			{
				transitionMinimized.insert(make_pair(make_pair(statesNaming.at(t.first.first), t.first.second), statesNaming.at(t.second)));

				if (t.first.second == symbolLast)
				{
					vector<State> group = { statesNaming.at(t.first.first) }; for (const auto& s : transitionMinimized) group.push_back(s.second);
					statesOfGroupToChange[group].insert(t.first.first); transitionMinimized.clear();
				}
			}
		}

		if (statesOfGroupToChange.size() != statesOfGroupToChangeTmp.size())
		{
			statesOfGroupToChangeTmp = statesOfGroupToChange; State tmp = 0;
			for (const auto& g : statesOfGroupToChange) { for (const auto& s : g.second) { statesNaming.at(s) = tmp;} ++tmp; }

			statesMinimized.clear(); for (const auto& t : transitions) statesMinimized[statesNaming.at(t.first.first)].push_back(make_pair(t.first, t.second));
			groupsToRun.push(statesMinimized);
		}
	}

	State stateInitialMinimized = 0;
	set<State> states, statesFinalMinimized; for (const auto& s : statesMinimized) states.insert(s.first);
	map<pair<State, Symbol>, State> transitionsMinimized;
	for (const auto& t : transitions)
	{
		if (t.second != fail) transitionsMinimized.insert(make_pair(make_pair(statesNaming.at(t.first.first), t.first.second), statesNaming.at(t.second)));
		if (t.first.first == dUseful.m_InitialState) stateInitialMinimized = statesNaming.at(t.first.first);
		if (dUseful.m_FinalStates.count(t.first.first)) statesFinalMinimized.insert(statesNaming.at(t.first.first));
	}

	return DFA {states, alphabet, transitionsMinimized, stateInitialMinimized, statesFinalMinimized};
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Computes the minimal DFA that accepts the union of languages specified by two NFAs.
 *
 * @param a The first NFA.
 * @param b The second NFA.
 * @return DFA The resulting minimal DFA for the union.
*/
DFA unify(const NFA& a, const NFA& b)
{
	NFA aComplete = make_complete(a, b.m_Alphabet), bComplete = make_complete(b, a.m_Alphabet);
	NFA u = make_parallel_run(aComplete, bComplete, true); DFA uDetermined = make_determined(u); DFA uMinimized = make_minimized(uDetermined);

	return uMinimized;
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Computes the minimal DFA that accepts the intersection of languages specified by two NFAs.
 *
 * @param a The first NFA.
 * @param b The second NFA.
 * @return DFA The resulting minimal DFA for the intersection.
*/
DFA intersect(const NFA& a, const NFA& b)
{
	NFA aComplete = make_complete(a, b.m_Alphabet), bComplete = make_complete(b, a.m_Alphabet);
	NFA i = make_parallel_run(aComplete, bComplete, false); DFA iDetermined = make_determined(i); DFA iMinimized = make_minimized(iDetermined);

	return iMinimized;
}

// ---------------------------------------------------------------------------------------------------------------------

// You may need to update this function or the sample data if your state naming strategy differs.
bool operator==(const DFA& a, const DFA& b)
{
    return std::tie(a.m_States, a.m_Alphabet, a.m_Transitions, a.m_InitialState, a.m_FinalStates) == std::tie(b.m_States, b.m_Alphabet, b.m_Transitions, b.m_InitialState, b.m_FinalStates);
}

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

int main()
{
//	NFA a1{
//		{0, 1, 2, 3, 4},
//		{'a', 'b'},
//		{
//            {{0, 'a'}, {1}},
//            {{1, 'b'}, {2}},
//            {{2, 'a'}, {3}},
//            {{3, 'b'}, {4}}
//        },
//        0,
//        {4}
//	};
//	NFA a2{
//		{0, 1, 2, 3},
//		{'a', 'b'},
//		{
//					{{0, 'a'}, {0,1,2,3}}},
//					0,
//					{0,1,2,3}
//	};
//	DFA a = unify(a1, a2);
//	assert(1);

//	NFA a1{
//		{0, 1, 2, 3, 4},
//		{'a', 'b'},
//		{
//            {{0, 'a'}, {1}},
//            {{1, 'b'}, {2}},
//            {{2, 'a'}, {3}},
//            {{3, 'b'}, {4}}
//        },
//        0,
//        {4}
//	};
//	NFA a2{
//		{0, 1, 2},
//		{'a', 'b'},
//		{
//			{{0, 'a'}, {0,1}},
//			{{0, 'b'}, {0}},
//			{{1, 'b'}, {2}},
//			{{2, 'a'}, {2}},
//			{{2, 'b'}, {2}}
//			},
//			0,
//			{}
//	};
//	DFA a = unify(a1, a2);
//	assert(1);

//    NFA a1{
//        {0, 1, 2},
//        {'a', 'b'},
//        {
//            {{0, 'a'}, {0, 1}},
//            {{0, 'b'}, {0}},
//            {{1, 'a'}, {2}},
//        },
//        0,
//        {2},
//    };
//    NFA a2{
//        {0, 1, 2},
//        {'a', 'b'},
//        {
//            {{0, 'a'}, {1}},
//            {{1, 'a'}, {2}},
//            {{2, 'a'}, {2}},
//            {{2, 'b'}, {2}},
//        },
//        0,
//        {2},
//    };
//    DFA a{
//        {0, 1, 2, 3, 4},
//        {'a', 'b'},
//        {
//            {{0, 'a'}, {1}},
//            {{1, 'a'}, {2}},
//            {{2, 'a'}, {2}},
//            {{2, 'b'}, {3}},
//            {{3, 'a'}, {4}},
//            {{3, 'b'}, {3}},
//            {{4, 'a'}, {2}},
//            {{4, 'b'}, {3}},
//        },
//        0,
//        {2},
//    };
//    assert(intersect(a1, a2) == a);

//    NFA b1{
//        {0, 1, 2, 3, 4},
//        {'a', 'b'},
//        {
//            {{0, 'a'}, {1}},
//            {{0, 'b'}, {2}},
//            {{2, 'a'}, {2, 3}},
//            {{2, 'b'}, {2}},
//            {{3, 'a'}, {4}},
//        },
//        0,
//        {1, 4},
//    };
//    NFA b2{
//        {0, 1, 2, 3, 4},
//        {'a', 'b'},
//        {
//            {{0, 'b'}, {1}},
//            {{1, 'a'}, {2}},
//            {{2, 'b'}, {3}},
//            {{3, 'a'}, {4}},
//            {{4, 'a'}, {4}},
//            {{4, 'b'}, {4}},
//        },
//        0,
//        {4},
//    };
//    DFA b{
//        {0, 1, 2, 3, 4, 5, 6, 7, 8},
//        {'a', 'b'},
//        {
//            {{0, 'a'}, {1}},
//            {{0, 'b'}, {2}},
//            {{2, 'a'}, {3}},
//            {{2, 'b'}, {4}},
//            {{3, 'a'}, {5}},
//            {{3, 'b'}, {6}},
//            {{4, 'a'}, {7}},
//            {{4, 'b'}, {4}},
//            {{5, 'a'}, {5}},
//            {{5, 'b'}, {4}},
//            {{6, 'a'}, {8}},
//            {{6, 'b'}, {4}},
//            {{7, 'a'}, {5}},
//            {{7, 'b'}, {4}},
//            {{8, 'a'}, {8}},
//            {{8, 'b'}, {8}},
//        },
//        0,
//        {1, 5, 8},
//    };
//    assert(unify(b1, b2) == b);

//    NFA c1{
//        {0, 1, 2, 3, 4},
//        {'a', 'b'},
//        {
//            {{0, 'a'}, {1}},
//            {{0, 'b'}, {2}},
//            {{2, 'a'}, {2, 3}},
//            {{2, 'b'}, {2}},
//            {{3, 'a'}, {4}},
//        },
//        0,
//        {1, 4},
//    };
//    NFA c2{
//        {0, 1, 2},
//        {'a', 'b'},
//        {
//            {{0, 'a'}, {0}},
//            {{0, 'b'}, {0, 1}},
//            {{1, 'b'}, {2}},
//        },
//        0,
//        {2},
//    };
//    DFA c{
//        {0},
//        {'a', 'b'},
//        {},
//        0,
//        {},
//    };
//    assert(intersect(c1, c2) == c);

//    NFA d1{
//        {0, 1, 2, 3},
//        {'i', 'k', 'q'},
//        {
//            {{0, 'i'}, {2}},
//            {{0, 'k'}, {1, 2, 3}},
//            {{0, 'q'}, {0, 3}},
//            {{1, 'i'}, {1}},
//            {{1, 'k'}, {0}},
//            {{1, 'q'}, {1, 2, 3}},
//            {{2, 'i'}, {0, 2}},
//            {{3, 'i'}, {3}},
//            {{3, 'k'}, {1, 2}},
//        },
//        0,
//        {2, 3},
//    };
//    NFA d2{
//        {0, 1, 2, 3},
//        {'i', 'k'},
//        {
//            {{0, 'i'}, {3}},
//            {{0, 'k'}, {1, 2, 3}},
//            {{1, 'k'}, {2}},
//            {{2, 'i'}, {0, 1, 3}},
//            {{2, 'k'}, {0, 1}},
//        },
//        0,
//        {2, 3},
//    };
//    DFA d{
//        {0, 1, 2, 3},
//        {'i', 'k', 'q'},
//        {
//            {{0, 'i'}, {1}},
//            {{0, 'k'}, {2}},
//            {{2, 'i'}, {3}},
//            {{2, 'k'}, {2}},
//            {{3, 'i'}, {1}},
//            {{3, 'k'}, {2}},
//        },
//        0,
//        {1, 2, 3},
//    };
//	DFA d3 = intersect(d1,d2);
//    assert(intersect(d1, d2) == d);

	return 0;
}
