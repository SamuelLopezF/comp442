#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>

namespace ast {

// Forward declarations
class ASTNode;
class Expression;
class Statement;
class Type;

// Utility function to generate indentation
std::string getIndent(int indent);

// Binary operation types
enum class BinaryOp {
    Add, Sub, Mul, Div, And, Or,
    Eq, Neq, Lt, Gt, Leq, Geq
};

// Convert binary operation to string
std::string binaryOpToString(BinaryOp op);

// Primitive types
class PrimitiveType {
public:
    enum class Kind {
        Int, Float, Void
    };
    
    Kind kind;
    
    PrimitiveType(Kind kind) : kind(kind) {}
    virtual void print(std::ostream& os, int indent) const;
};

std::string primitiveTypeToString(PrimitiveType::Kind kind);

// Base node class
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void print(std::ostream& os, int indent) const = 0;
};

// Expression nodes
class Expression : public ASTNode {
public:
    virtual ~Expression() = default;
};

// Integer literal
class IntegerLiteral : public Expression {
public:
    int value;
    
    IntegerLiteral(int value) : value(value) {}
    void print(std::ostream& os, int indent) const override;
};

// Float literal
class FloatLiteral : public Expression {
public:
    float value;
    
    FloatLiteral(float value) : value(value) {}
    void print(std::ostream& os, int indent) const override;
};

// Variable reference
class VariableReference : public Expression {
public:
    std::string name;
    
    VariableReference(const std::string& name) : name(name) {}
    void print(std::ostream& os, int indent) const override;
};

// Binary operation
class BinaryOperation : public Expression {
public:
    BinaryOp op;
    std::shared_ptr<Expression> left;
    std::shared_ptr<Expression> right;
    
    BinaryOperation(BinaryOp op, std::shared_ptr<Expression> left, std::shared_ptr<Expression> right)
        : op(op), left(left), right(right) {}
    
    void print(std::ostream& os, int indent) const override;
};

// Function call
class FunctionCall : public Expression {
public:
    std::string name;
    std::vector<std::shared_ptr<Expression>> args;
    
    FunctionCall(const std::string& name) : name(name) {}
    void print(std::ostream& os, int indent) const override;
};

// Type nodes
class Type : public ASTNode {
public:
    virtual ~Type() = default;
};

// Array type
class ArrayType : public Type {
public:
    std::shared_ptr<Type> baseType;
    std::vector<int> dimensions;
    
    ArrayType(std::shared_ptr<Type> baseType) : baseType(baseType) {}
    void print(std::ostream& os, int indent) const override;
};

// Statement nodes
class Statement : public ASTNode {
public:
    virtual ~Statement() = default;
};

// Assignment statement
class AssignmentStatement : public Statement {
public:
    std::string variable;
    std::shared_ptr<Expression> expr;
    
    AssignmentStatement(const std::string& variable, std::shared_ptr<Expression> expr)
        : variable(variable), expr(expr) {}
    
    void print(std::ostream& os, int indent) const override;
};

// Return statement
class ReturnStatement : public Statement {
public:
    std::shared_ptr<Expression> expr;
    
    ReturnStatement(std::shared_ptr<Expression> expr) : expr(expr) {}
    void print(std::ostream& os, int indent) const override;
};

// Expression statement
class ExpressionStatement : public Statement {
public:
    std::shared_ptr<Expression> expr;
    
    ExpressionStatement(std::shared_ptr<Expression> expr) : expr(expr) {}
    void print(std::ostream& os, int indent) const override;
};

// Variable declaration
class VariableDeclaration : public Statement {
public:
    std::string name;
    std::shared_ptr<Type> type;
    
    VariableDeclaration(const std::string& name, std::shared_ptr<Type> type)
        : name(name), type(type) {}
    
    void print(std::ostream& os, int indent) const override;
};

// Function declaration
class FunctionDeclaration : public ASTNode {
public:
    std::string name;
    std::shared_ptr<Type> returnType;
    std::vector<std::shared_ptr<VariableDeclaration>> params;
    std::vector<std::shared_ptr<VariableDeclaration>> locals;
    std::vector<std::shared_ptr<Statement>> body;
    
    FunctionDeclaration(const std::string& name, std::shared_ptr<Type> returnType)
        : name(name), returnType(returnType) {}
    
    void print(std::ostream& os, int indent) const override;
};

// Class declaration
class ClassDeclaration : public ASTNode {
public:
    std::string name;
    std::vector<std::string> baseClasses;
    std::vector<std::shared_ptr<VariableDeclaration>> attributes;
    std::vector<std::shared_ptr<FunctionDeclaration>> methods;
    
    ClassDeclaration(const std::string& name) : name(name) {}
    void print(std::ostream& os, int indent) const override;
};

// Program node (root)
class Program : public ASTNode {
public:
    std::vector<std::shared_ptr<ASTNode>> declarations;
    
    void print(std::ostream& os, int indent) const override;
};

} // namespace ast

#endif // AST_H
