#ifndef AST_H
#define AST_H

#include <string>
#include <vector>

class ASTNode;
class Program;
class ClassDecl;
class FuncDecl;
class VarDecl;
class Type;
class Statement;
class IfStatement;
class WhileStatement;
class ReturnStatement;
class AssignStatement;
class Expression;
class BinaryExpression;
class UnaryExpression;
class CallExpression;
class Identifier;
class IntegerLiteral;
class FloatLiteral;

// Add the printAST function declaration
void printAST(ASTNode* root, const std::string& outputFile);
class ASTVisitor {
public:
    virtual void visit(Program& node) = 0;
    virtual void visit(ClassDecl& node) = 0;
    virtual void visit(FuncDecl& node) = 0;
    virtual void visit(VarDecl& node) = 0;
    virtual void visit(Type& node) = 0;
    virtual void visit(Statement& node) = 0;
    virtual void visit(IfStatement& node) = 0;
    virtual void visit(WhileStatement& node) = 0;
    virtual void visit(ReturnStatement& node) = 0;
    virtual void visit(AssignStatement& node) = 0;
    virtual void visit(Expression& node) = 0;
    virtual void visit(BinaryExpression& node) = 0;
    virtual void visit(UnaryExpression& node) = 0;
    virtual void visit(CallExpression& node) = 0;
    virtual void visit(Identifier& node) = 0;
    virtual void visit(IntegerLiteral& node) = 0;
    virtual void visit(FloatLiteral& node) = 0;
};

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
};

class Program : public ASTNode {
public:
    std::vector<ASTNode*> declarations;

    Program(std::vector<ASTNode*> decls) : declarations(std::move(decls)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ClassDecl : public ASTNode {
public:
    std::string name;
    std::vector<ASTNode*> members;

    ClassDecl(std::string n, std::vector<ASTNode*> membs) : name(std::move(n)), members(std::move(membs)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class FuncDecl : public ASTNode {
public:
    std::string name;
    std::vector<VarDecl*> params;
    Type* returnType;
    Statement* body;

    FuncDecl(std::string n, std::vector<VarDecl*> p, Type* type, Statement* b) : 
        name(std::move(n)), params(std::move(p)), returnType(type), body(b) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }  
};

class VarDecl : public ASTNode {
public:
    std::string name;
    Type* type;

    VarDecl(std::string n, Type* t) : name(std::move(n)), type(t) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class Type : public ASTNode {
public:  
    std::string name;

    Type(std::string n) : name(std::move(n)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class Statement : public ASTNode {
public:
    virtual void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class IfStatement : public Statement {
public:
    Expression* condition;
    Statement* thenStmt;
    Statement* elseStmt;

    IfStatement(Expression* cond, Statement* thenS, Statement* elseS) : condition(cond), thenStmt(thenS), elseStmt(elseS) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }  
};

class WhileStatement : public Statement {  
public:
    Expression* condition;
    Statement* body;

    WhileStatement(Expression* cond, Statement* b) : condition(cond), body(b) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ReturnStatement : public Statement {
public:
    Expression* expression;

    ReturnStatement(Expression* expr) : expression(expr) {} 
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class AssignStatement : public Statement {
public:  
    Identifier* lhs;
    Expression* rhs;

    AssignStatement(Identifier* left, Expression* right) : lhs(left), rhs(right) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }  
};

class Expression : public ASTNode {
public:
    virtual void accept(ASTVisitor& visitor) override { visitor.visit(*this); }  
};

class BinaryExpression : public Expression {
public:
    std::string op;  
    Expression* left;
    Expression* right;

    BinaryExpression(std::string o, Expression* l, Expression* r) : op(std::move(o)), left(l), right(r) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class UnaryExpression : public Expression {
public:
    std::string op;
    Expression* expr;

    UnaryExpression(std::string o, Expression* e) : op(std::move(o)), expr(e) {}  
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class CallExpression : public Expression {
public:
    Identifier* callee; 
    std::vector<Expression*> args;

    CallExpression(Identifier* c, std::vector<Expression*> a) : callee(c), args(std::move(a)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }  
};

class Identifier : public Expression {
public:
    std::string name;

    Identifier(std::string n) : name(std::move(n)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class IntegerLiteral : public Expression {
public:  
    int value;

    IntegerLiteral(int v) : value(v) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }  
};

class FloatLiteral : public Expression {
public:
    float value;  

    FloatLiteral(float v) : value(v) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

#endif
