#ifndef SEMANTIC_SYMBOL_TABLE_HPP
#define SEMANTIC_SYMBOL_TABLE_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "type.hpp"

class Symbol;
using SymbolPtr = std::shared_ptr<Symbol>;
class Symbol {
 public:
  /// @brief The name of the symbol
  std::string name;
  /// @brief The unique name of the symbol
  std::string unique_name;
  /// @brief The type of the symbol
  TypePtr type;
	/// @brief The depth of scope
	int depth;
	/// @brief The symbol is defined
	bool is_defined = false;

  Symbol(std::string name, TypePtr type, int depth, bool is_defined) 
							: name(name), type(type), depth(depth), is_defined(is_defined) {}
  static SymbolPtr create(std::string name, TypePtr type, int depth, bool is_defined) {
    return std::make_shared<Symbol>(name, type, depth, is_defined);
  }
};

class SymbolTable;
using SymbolTablePtr = std::shared_ptr<SymbolTable>;
class SymbolTable {
 public:
	SymbolTable(int depth = 0, SymbolTablePtr parent = nullptr) : depth(depth), parent(parent) {}

  /// @brief Add a symbol to the table and return the unique name of the symbol
  /// @param name The name of the symbol
  /// @param type The type of the symbol
  /// @return The added symbol if added successfully, nullptr otherwise
  SymbolPtr add_symbol(std::string name, TypePtr type);

  /// @brief Find a symbol by name
  /// @param name The name of the symbol
  /// @param in_current_scope Whether to search only in the current scope
  /// @return The symbol if found, nullptr otherwise
  SymbolPtr find_symbol(std::string name, bool in_current_scope = false) const;

  /// @brief Enter a new scope
  void enter_scope();

  /// @brief Exit the current scope
  void exit_scope();

	SymbolTablePtr parent;

	SymbolTablePtr next;

	std::vector<SymbolPtr> node = {};
	
	int depth;

// #warning Not implemented: SymbolTable
};

#endif  // SEMANTIC_SYMBOL_TABLE_HPP