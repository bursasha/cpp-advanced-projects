# The task is to implement effective data structure fulfilling the following conditions and interface.

## Program Interface

Task is to implement a `Bestsellers` class template parameterized by the `Product` type, which serves as an identifier for individual products. About the `Product` type, it is promised that it has a copy constructor (and an assignment operator), a destructor, comparison operators, and `std::hash` is implemented for it. The `Bestsellers` class must implement the following public methods:

- `size_t products() const`: Returning the number of registered goods (i.e., not the number of units sold).

- `void sell(const Product& p, size_t amount)`: Registering the sale of the `p` product `amount` times.

- `size_t rank(const Product& p) const`: Returns the number of times a given product has sold the same number as 1 (if more than one product has sold the same number, you can sort them as you like).

- `const Product& product(size_t rank) const`: Inverse function to `rank` (i.e., if no exception occurs, `product(rank(p)) == p` and `rank(product(r)) == r`).

- `size_t sold(size_t r) const`: Number of units sold `r`th best-selling item.

- `size_t sold(size_t from, size_t to) const`: Analogous to the one-parameter version, but returns the sum of units sold over the interval from `from` to `to` (including both edges; thus, `sold(r, r) == sold(r)` and `sold(1, products())` is the total number of units sold over all types of goods; input to `< from` is considered invalid).
