# The task is to implement effective search algorithm by the given program interface.

## Program interface

Task is to implement the function std::list<Place> find_path (const Map &map), which 
in the specified map will find the shortest possible walk, which will collect at least 
one component of each type. The first element of the walk must be a map.start, last 
map.end and between each two consecutive places must lead a corridor. If there is no 
such walk, return an empty list. The Map structure contains items:

- places: number of rooms,
- start, end: numbers of rooms where Max starts, respectively where he should bring the collected things (numbered from zero),
- connections: list of pairs of rooms describing corridors,
- items: list of lists, items[i] is a list of rooms where the i-th component is located.

You can assume that the room numbers are always from 0 to places - 1 (inclusive).
