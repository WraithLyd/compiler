#ifndef SEMANTIC_TYPE_CHECKER_HPP
#define SEMANTIC_TYPE_CHECKER_HPP

#include <memory>
#include <algorithm>

#include "ast/tree.hpp"
#include "symbol_table.hpp"

class TypeChecker {
 public:

	TypePtr func_ret_type;
 
	TypeChecker();

  TypePtr check(AST::NodePtr node);

 private:
  /// @brief The symbol table
  SymbolTablePtr symbol_table = std::make_shared<SymbolTable>();

  TypePtr checkIntConst(AST::IntConstPtr node);
  TypePtr checkLVal(AST::LValPtr node);
	TypePtr checkInitVal(AST::InitValPtr node, std::optional<AST::ArrListsPtr> arr = std::nullopt, int checked_cnt = 0, std::string str="error");
  TypePtr checkUnaryExp(AST::UnaryExpPtr node);
  TypePtr checkBinaryExp(AST::BinaryExpPtr node);
  TypePtr checkFuncCall(AST::FuncCallPtr node);
  TypePtr checkBlock(AST::BlockPtr node, bool new_scope = true);
  TypePtr checkAssignStmt(AST::AssignStmtPtr node);
  TypePtr checkReturnStmt(AST::ReturnStmtPtr node);
	TypePtr checkIfStmt(AST::IfStmtPtr node);
	TypePtr checkWhileStmt(AST::WhileStmtPtr node);
	TypePtr checkNullStmt(AST::NullStmtPtr node);
  TypePtr checkVarDef(AST::VarDefPtr node, BasicType var_type);
  TypePtr checkVarDecl(AST::VarDeclPtr node);
	TypePtr checkArrDef(AST::ArrDefPtr node, BasicType var_type = BasicType::Int);
  TypePtr checkArrDecl(AST::ArrDeclPtr node);
	TypePtr checkArrLists(AST::ArrListsPtr node);
  TypePtr checkFuncDef(AST::FuncDefPtr node);
	TypePtr checkDecl(AST::DeclPtr node);
  TypePtr checkCompUnit(AST::CompUnitPtr node);

};

#endif  // SEMANTIC_TYPE_CHECKER_HPP