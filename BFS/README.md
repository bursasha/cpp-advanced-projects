# The task is to implement an effective search algorithm based on the given program interface.

## Program Interface

Task is to implement the function `std::list<Place> find_path(const Map& map)`, which in the specified map will find the shortest possible walk that will collect at least one component of each type. The first element of the walk must be `map.start`, the last `map.end`, and between each two consecutive places must lead a corridor. If there is no such walk, return an empty list. The `Map` structure contains items:

- `places`: number of rooms,
- `start`, `end`: numbers of rooms where Max starts, respectively where he should bring the collected things (numbered from zero),
- `connections`: list of pairs of rooms describing corridors,
- `items`: list of lists, `items[i]` is a list of rooms where the i-th component is located.

You can assume that the room numbers are always from 0 to `places - 1` (inclusive).
