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
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>
#include <random>

// ---------------------------------------------------------------------------------------------------------------------

using namespace std;

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Template class to manage best-selling products.
 *
 * @tparam Product The type of the product being tracked.
 */
template < typename Product >
struct Bestsellers {
  /**
   * @brief Construct a new Bestsellers object.
   */
  Bestsellers() : m_Shop() {}

  // -------------------------------------------------------------------------------------------------------------------

	/**
	 * @brief Get the total number of tracked products.
	 *
	 * @return size_t The number of unique products.
	 */
  size_t products() const { return m_Shop.get_uniques(); }

	/**
	 * @brief Register the sale of a product.
	 *
	 * @param p The product being sold.
	 * @param amount The amount sold.
	 */
  void sell(const Product& p, size_t amount) { m_Shop.insert_node(p, amount); }

	/**
	 * @brief Get the rank of a product based on its sales.
	 *
	 * @param p The product whose rank is to be retrieved.
	 * @return size_t The rank of the product.
	 */
  size_t rank(const Product& p) const { return m_Shop.get_rank(p); }

	/**
	 * @brief Get the product with the given rank.
	 *
	 * @param rank The rank of the product.
	 * @return const Product& The product with the specified rank.
	 */
  const Product& product(size_t rank) const { return m_Shop.get_product(rank); }

	/**
	 * @brief Get the number of copies sold of the product with the given rank.
	 *
	 * @param rank The rank of the product.
	 * @return size_t The number of copies sold.
	 */
  size_t sold(size_t rank) const { return m_Shop.get_sold(rank); }

	/**
	 * @brief Get the total number of copies sold for products in the given rank interval.
	 *
	 * @param from The starting rank.
	 * @param to The ending rank.
	 * @return size_t The total number of copies sold.
	 */
  size_t sold(size_t from, size_t to) const { return (from == to) ? m_Shop.get_sold(from) : m_Shop.get_sold(from, to); }

  // -------------------------------------------------------------------------------------------------------------------

	/**
	 * @brief Get the first rank where the number of copies sold matches the specified rank.
	 *
	 * @param r The rank to check.
	 * @return size_t The first rank with the same number of copies sold.
	 */
  size_t first_same(size_t r) const { return m_Shop.first_same_rank(r); }

	/**
	 * @brief Get the last rank where the number of copies sold matches the specified rank.
	 *
	 * @param r The rank to check.
	 * @return size_t The last rank with the same number of copies sold.
	 */
  size_t last_same(size_t r) const { return m_Shop.last_same_rank(r); }

  // -------------------------------------------------------------------------------------------------------------------

	/**
	 * @brief Class to manage the tree structure of products.
	 */
  class ProductTree {
  public:
	  ProductTree() : m_Products(), m_Root(nullptr) {}
	  ~ProductTree() { delete_tree(m_Root); }

  	  // ---------------------------------------------------------------------------------------------------------------

  		/**
		 * @brief Get the number of unique products.
		 *
		 * @return size_t The number of unique products.
		 */
	  size_t get_uniques() const { return m_Root->m_SumNames; }

  	    /**
		 * @brief Insert a product node into the tree.
		 *
		 * @param name The product name.
		 * @param count The number of units sold.
		 */
	  void insert_node(const Product &name, size_t count)
	  {
		  if (!(m_Products.count(name))) { m_Products.insert({name, count}); m_Root = insert_node(m_Root, name, count); }
		  else
		  {
			  size_t countOld = m_Products.at(name); m_Products.at(name) += count; size_t countNew = m_Products.at(name);
			  m_Root = delete_node(m_Root, name, countOld); m_Root = insert_node(m_Root, name, countNew);
		  }
	  }

  		/**
		 * @brief Get the rank of a product.
		 *
		 * @param name The product name.
		 * @return size_t The rank of the product.
		 */
	  size_t get_rank(const Product &name) const
	  {
		  if (!(m_Products.count(name))) throw out_of_range("");

		  size_t count = m_Products.at(name), rank = 0; ProductNode *tmp = m_Root;
		  while (tmp)
		  {
			  if (count < tmp->m_Count) { rank += tmp->m_SumNames; if (tmp->m_Left) rank -= tmp->m_Left->m_SumNames; tmp = tmp->m_Left; }
			  else if (count > tmp->m_Count) tmp = tmp->m_Right;
			  else { auto it = find(tmp->m_Names.begin(), tmp->m_Names.end(), name); rank += (tmp->m_SumNames - tmp->m_Names.size() + 1) + (it - tmp->m_Names.begin()); if (tmp->m_Left) rank -= tmp->m_Left->m_SumNames; break; }
		  }

		  return rank;
	  }

  		/**
		 * @brief Get the product at the specified rank.
		 *
		 * @param rank The rank of the product.
		 * @return const Product& The product at the specified rank.
		 */
	  const Product& get_product(size_t rank) const
	  {
		  if (rank > m_Root->m_SumNames || rank < 1) throw out_of_range("");

		  size_t rankTmp = 0; ProductNode *tmp = m_Root; Product product;
		  while (tmp)
		  {
			  size_t rankTmpOld = rankTmp; rankTmp += tmp->m_SumNames; if (tmp->m_Left) rankTmp -= tmp->m_Left->m_SumNames;
			  size_t rankTmpHigh = rankTmp, rankTmpLow = (rankTmp - tmp->m_Names.size() + 1);

			  if (rank < rankTmpLow) { rankTmp = rankTmpOld; tmp = tmp->m_Right; }
			  else if (rank > rankTmpHigh) { tmp = tmp->m_Left; }
			  else { product = tmp->m_Names[(rank - rankTmpLow)]; break; }
		  }

		  return m_Products.find(product)->first;
	  }

  		/**
	     * @brief Get the number of copies sold of the product at the specified rank.
	     *
	     * @param rank The rank of the product.
		 * @return size_t The number of copies sold.
		 */
	  size_t get_sold(size_t rank) const
	  {
		  if (rank > m_Root->m_SumNames || rank < 1) throw out_of_range("");

		  size_t rankTmp = 0; ProductNode *tmp = m_Root; size_t sold = 0;
		  while (tmp)
		  {
			  size_t rankTmpOld = rankTmp; rankTmp += tmp->m_SumNames; if (tmp->m_Left) rankTmp -= tmp->m_Left->m_SumNames;
			  size_t rankTmpHigh = rankTmp, rankTmpLow = (rankTmp - tmp->m_Names.size() + 1);

			  if (rank < rankTmpLow) { rankTmp = rankTmpOld; tmp = tmp->m_Right; }
			  else if (rank > rankTmpHigh) { tmp = tmp->m_Left; }
			  else { sold = tmp->m_Count; break; }
		  }

		  return sold;
	  }

  		/**
		 * @brief Get the total number of copies sold for products in the given rank interval.
		 *
		 * @param from The starting rank.
		 * @param to The ending rank.
		 * @return size_t The total number of copies sold.
		 */
	  size_t get_sold(size_t from, size_t to) const
	  {
		  if (to > m_Root->m_SumNames || from < 1 || from > to) throw out_of_range("");

		  size_t rankTmp = 0; ProductNode *tmp = m_Root; size_t excess = 0;
		  while (tmp)
		  {
			  size_t rankTmpOld = rankTmp; rankTmp += tmp->m_SumNames; if (tmp->m_Left) rankTmp -= tmp->m_Left->m_SumNames;
			  size_t rankTmpHigh = rankTmp, rankTmpLow = (rankTmp - tmp->m_Names.size() + 1);

			  if (from < rankTmpLow) { rankTmp = rankTmpOld; tmp = tmp->m_Right; }
			  else if (from > rankTmpHigh) { excess += (tmp->m_Count * tmp->m_Names.size()); if (tmp->m_Right) excess += tmp->m_Right->m_SumCounts; tmp = tmp->m_Left; }
			  else { excess += ((from - rankTmpLow) * tmp->m_Count); if (tmp->m_Right) excess += tmp->m_Right->m_SumCounts; break; }
		  }

		  rankTmp = 0; tmp = m_Root;
		  while (tmp)
		  {
			  size_t rankTmpOld = rankTmp; rankTmp += tmp->m_SumNames; if (tmp->m_Left) rankTmp -= tmp->m_Left->m_SumNames;
			  size_t rankTmpHigh = rankTmp, rankTmpLow = (rankTmp - tmp->m_Names.size() + 1);

			  if (to < rankTmpLow) { excess += (tmp->m_Count * tmp->m_Names.size()); if (tmp->m_Left) excess += tmp->m_Left->m_SumCounts; rankTmp = rankTmpOld; tmp = tmp->m_Right; }
			  else if (to > rankTmpHigh) tmp = tmp->m_Left;
			  else { excess += ((rankTmpHigh - to) * tmp->m_Count); if (tmp->m_Left) excess += tmp->m_Left->m_SumCounts; break; }
		  }

		  size_t sold = 0; if (m_Root) sold = (m_Root->m_SumCounts - excess);

		  return sold;
	  }

  		/**
		 * @brief Get the first rank where the number of copies sold matches the specified rank.
		 *
		 * @param rank The rank to check.
		 * @return size_t The first rank with the same number of copies sold.
		 */
	  size_t first_same_rank(size_t rank) const
	  {
		  if (rank > m_Root->m_SumNames || rank < 1) throw out_of_range("");

		  size_t rankTmp = 0; ProductNode *tmp = m_Root; size_t firstSameRank = 0;
		  while (tmp)
		  {
			  size_t rankTmpOld = rankTmp; rankTmp += tmp->m_SumNames; if (tmp->m_Left) rankTmp -= tmp->m_Left->m_SumNames;
			  size_t rankTmpHigh = rankTmp, rankTmpLow = (rankTmp - tmp->m_Names.size() + 1);

			  if (rank < rankTmpLow) { rankTmp = rankTmpOld; tmp = tmp->m_Right; }
			  else if (rank > rankTmpHigh) { tmp = tmp->m_Left; }
			  else { firstSameRank = rankTmpLow; break; }
		  }

		  return firstSameRank;
	  }

  		/**
		 * @brief Get the last rank where the number of copies sold matches the specified rank.
		 *
		 * @param rank The rank to check.
		 * @return size_t The last rank with the same number of copies sold.
	 	 */
	  size_t last_same_rank(size_t rank) const
	  {
		  if (rank > m_Root->m_SumNames || rank < 1) throw out_of_range("");

		  size_t rankTmp = 0; ProductNode *tmp = m_Root; size_t lastSameRank = 0;
		  while (tmp)
		  {
			  size_t rankTmpOld = rankTmp; rankTmp += tmp->m_SumNames; if (tmp->m_Left) rankTmp -= tmp->m_Left->m_SumNames;
			  size_t rankTmpHigh = rankTmp, rankTmpLow = (rankTmp - tmp->m_Names.size() + 1);

			  if (rank < rankTmpLow) { rankTmp = rankTmpOld; tmp = tmp->m_Right; }
			  else if (rank > rankTmpHigh) { tmp = tmp->m_Left; }
			  else { lastSameRank = rankTmpHigh; break; }
		  }

		  return lastSameRank;
	  }

  private:
	  struct ProductNode {
		  vector<Product> m_Names; size_t m_Count; size_t m_SumNames, m_SumCounts;
		  ProductNode *m_Left, *m_Right;
		  size_t m_Height;

		  ProductNode(Product name, size_t count) { m_Names.push_back(name); m_Count = count; m_Left = m_Right = nullptr; m_Height = 1; m_SumNames = 1; m_SumCounts = m_Count; }
	  };

	  unordered_map<Product, size_t> m_Products; ProductNode *m_Root;

  	   /**
		* @brief Delete the tree structure recursively.
		*
		* @param root The root node to delete.
		*/
	  void delete_tree(ProductNode *root) { if (m_Root) { if (root->m_Left) delete_tree(root->m_Left); if (root->m_Right) delete_tree(root->m_Right); delete root; } }

  	   /**
		* @brief Get the height of a node.
		*
		* @param root The node to get the height of.
		* @return size_t The height of the node.
		*/
	  size_t get_height(ProductNode *root) const { return root ? root->m_Height : 0; }

  	 /**
	  * @brief Get the balance factor of a node.
	  *
	  * @param root The node to get the balance factor of.
	  * @return int The balance factor of the node.
	  */
	  int get_balance(ProductNode *root) const { return get_height(root->m_Right) - get_height(root->m_Left); }

  	   /**
		* @brief Get the node with the minimum value.
		*
		* @param root The root node.
		* @return ProductNode* The node with the minimum value.
		*/
	  ProductNode *get_minimum(ProductNode *root) const { return !(root->m_Left) ? root : get_minimum(root->m_Left); }

  	   /**
		* @brief Set the height of a node.
		*
		* @param root The node to set the height of.
		*/
	  void set_height(ProductNode *root)
	  {
		  size_t heightLeft = get_height(root->m_Left), heightRight = get_height(root->m_Right);
		  root->m_Height = (heightRight > heightLeft ? heightRight : heightLeft) + 1;
	  }

  	   /**
		* @brief Delete the minimum node.
		*
		* @param root The root node.
		* @return ProductNode* The new root after deletion.
		*/
	  ProductNode *delete_minimum(ProductNode *root)
	  {
		  if (!(root->m_Left)) return root->m_Right;
		  root->m_Left = delete_minimum(root->m_Left);

		  return balance_node(root);
	  }

  	   /**
		* @brief Rotate the tree to the left.
		*
		* @param root The root node.
		* @return ProductNode* The new root after rotation.
		*/
	  ProductNode *rotate_left(ProductNode *root)
	  {
		  ProductNode *subtreeRight = root->m_Right;
		  root->m_Right = subtreeRight->m_Left;
		  subtreeRight->m_Left = root;
		  set_height(root); set_height(subtreeRight);
		  update_auxiliary_info(root); update_auxiliary_info(subtreeRight);

		  return subtreeRight;
	  }

  	   /**
		* @brief Rotate the tree to the right.
		*
		* @param root The root node.
		* @return ProductNode* The new root after rotation.
		*/
	  ProductNode *rotate_right(ProductNode *root)
	  {
		  ProductNode *subtreeLeft = root->m_Left;
		  root->m_Left = subtreeLeft->m_Right;
		  subtreeLeft->m_Right = root;
		  set_height(root); set_height(subtreeLeft);
		  update_auxiliary_info(root); update_auxiliary_info(subtreeLeft);

		  return subtreeLeft;
	  }

  	   /**
		* @brief Update auxiliary information for a node.
		*
		* @param root The node to update.
		*/
	  void update_auxiliary_info(ProductNode *root)
	  {
		  root->m_SumNames = root->m_Names.size(); root->m_SumCounts = (root->m_SumNames * root->m_Count);
		  if (root->m_Left) { root->m_SumNames += root->m_Left->m_SumNames; root->m_SumCounts += root->m_Left->m_SumCounts; }
		  if (root->m_Right) { root->m_SumNames += root->m_Right->m_SumNames; root->m_SumCounts += root->m_Right->m_SumCounts; }
	  }

  	   /**
		* @brief Balance the node.
		*
		* @param root The node to balance.
		* @return ProductNode* The new root after balancing.
		*/
	  ProductNode *balance_node(ProductNode *root)
	  {
		  set_height(root);

		  if (get_balance(root) == 2)
		  {
			  if (get_balance(root->m_Right) < 0) root->m_Right = rotate_right(root->m_Right);
			  return rotate_left(root);
		  }
		  if (get_balance(root) == -2)
		  {
			  if (get_balance(root->m_Left) > 0) root->m_Left = rotate_left(root->m_Left);
			  return rotate_right(root);
		  }
		  update_auxiliary_info(root);

		  return root;
	  }

  	      /**
		   * @brief Delete a node from the tree.
		   *
		   * @param root The root node.
		   * @param name The product name.
		   * @param count The number of units sold.
		   * @return ProductNode* The new root after deletion.
		   */
	  ProductNode *delete_node(ProductNode *root, const Product &name, size_t count)
	  {
		  if (!root) return nullptr;

		  if (count < root->m_Count) root->m_Left = delete_node(root->m_Left, name, count);
		  else if (count > root->m_Count) root->m_Right = delete_node(root->m_Right, name, count);
		  else
		  {
			  bool shouldDelete = false;
			  auto it = find(root->m_Names.begin(), root->m_Names.end(), name); if (it != root->m_Names.end()) root->m_Names.erase(it);
			  if (root->m_Names.empty()) shouldDelete = true;

			  if (shouldDelete)
			  {
				  ProductNode *subtreeLeft = root->m_Left, *subtreeRight = root->m_Right;
				  delete root;

				  if (!subtreeRight) return subtreeLeft;

				  ProductNode *minimum = get_minimum(subtreeRight);
				  minimum->m_Right = delete_minimum(subtreeRight);
				  minimum->m_Left = subtreeLeft;

				  return balance_node(minimum);
			  }
		  }

		  return balance_node(root);
	  }

  	      /**
		   * @brief Insert a node into the tree.
		   *
		   * @param root The root node.
		   * @param name The product name.
		   * @param count The number of units sold.
		   * @return ProductNode* The new root after insertion.
		   */
	  ProductNode *insert_node(ProductNode *root, const Product &name, size_t count)
	  {
		  if (!root) return new ProductNode(name, count);

		  if (count < root->m_Count) root->m_Left = insert_node(root->m_Left, name, count);
		  else if (count > root->m_Count) root->m_Right = insert_node(root->m_Right, name, count);
		  else root->m_Names.push_back(name);

		  return balance_node(root);
	  }

  };

  ProductTree m_Shop;
};

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

void test1() {
  Bestsellers<std::string> T;
  T.sell("coke", 32);
  T.sell("bread", 1);
  assert(T.products() == 2);
  T.sell("ham", 2);
  T.sell("mushrooms", 12);

  assert(T.products() == 4);
  assert(T.rank("ham") == 3);
  assert(T.rank("coke") == 1);
  assert(T.sold(1, 3) == 46);
  assert(T.product(2) == "mushrooms");

  T.sell("ham", 11);
  assert(T.products() == 4);
  assert(T.product(2) == "ham");
  assert(T.sold(2) == 13);
  assert(T.sold(2, 2) == 13);
  assert(T.sold(1, 2) == 45);
}

// ---------------------------------------------------------------------------------------------------------------------

void test2() {
# define CATCH(expr) \
  try { expr; assert(0); } catch (const std::out_of_range&) { assert(1); };

  Bestsellers<std::string> T;
  T.sell("coke", 32);
  T.sell("bread", 1);

  CATCH(T.rank("ham"));
  CATCH(T.product(3));
  CATCH(T.sold(0));
  CATCH(T.sold(9));
  CATCH(T.sold(0, 1));
  CATCH(T.sold(3, 2));
  CATCH(T.sold(1, 9));

#undef CATCH
}

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

int main() {
  test1();
  test2();
}
