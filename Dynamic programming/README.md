# The task involves the concept of a dynamic programming algorithm with memoization.

## Program Interface

Your task is to implement the function `uint64_t solve(const TreeProblem&)`, which will calculate how many gifts can be saved in the best possible arrangement of the guards. The instance of the problem is described by the `TreeProblem` structure, which contains items:

- `max_group_size`: the maximum size of a group of Guardians. This value is 2 in the case of a bonus test and 1 otherwise.

- `gifts`: Vector indicating how many gifts are under which tree. The length of this vector also indicates the total number of trees.

- `connections`: a vector of pairs indicating which trees are at opposite ends of the same street, so if both are guarded, their guards belong to the same group. We remind you that the network of streets of Kdovikova forms a tree.
