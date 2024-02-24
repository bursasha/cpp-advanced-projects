# The task of this scored programming task is to implement algorithms that allow you to find a minimal deterministic finite automaton that accepts the intersection, respectively, the unification of languages specified by a pair of Finite Automata. 

## Thus, the goal is to implement a set of two functions in C++ with signatures:

- `DFA unify(const NFA&, const NFA&);`
- `DFA intersect(const NFA&, const NFA&);`

Both of these functions must return the minimum automaton for the language.

The input or output of algorithms are finite automata in the form of NFA or DFA structures. 

The first structure, `NFA`, represents a nondeterministic finite automaton (but for some transition functions, it can be declared deterministic). 
The second structure, `DFA`, represents only a deterministic finite automaton. 
These structures are simple data structures that maintain data representing the automaton and do not perform any validation checks on the content. Proper initialization and filling with content is the concern of the one who creates them.

These structures are defined in the test environment (so do not define them in your task), see the sample code in the attached sample file. For simplicity, states are defined as values of type `unsigned`, and alphabetic symbols as values of type `uint8_t`.

## Input Characteristics

It is guaranteed that the input of the `unify` and `intersect` functions will be valid non-deterministic finite automata. They will meet the following characteristics:

- The sets of states (`NFA::m_States`) and alphabet symbols (`NFA::m_Alphabet`) will be non-empty.
- The starting and ending States (`NFA::m_InitialState` and `NFA::m_FinalStates`, respectively) are elements of the `NFA::m_States` state set.
- If for some state there will be no `q` and an alphabet symbol and a defined transition in the automaton, then in `NFA::m_Transitions` the key `(q, a)` will not exist at all.
- The transition table `NFA::m_Transitions` also contains only those symbols and statuses that are specified in the alphabet symbol set and the state set.
