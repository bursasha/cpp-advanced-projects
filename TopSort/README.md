# The task is to implement Topological Sort algorithm with the given interface and tests.

## Task Description:

Implement the function `std::pair<bool, std::vector<Vertex>> topsort(const Graph& G)`. The return value is either:

* `true` and a list of all vertices in topological order, or
* `false` and a list of vertices that form a cycle (there must also be an edge from the last vertex to the first vertex in the list).

The `Graph` class represents a directed graph. Important methods are:

* `vertices()`: Number of vertices.
* `operator[](Vertex v)`: List of neighbors of vertex `v`. Vertices are integers from 0.
* `begin()` and `end()` methods, which allow iteration over vertices and using the `Graph` class in a range-for loop like this `for (Vertex v : G) ....`
* `reversed()`: Returns a new graph created by reversing the orientation of all edges.

**Additional Information:**

* This is a classic implementation of the topological sort algorithm.
