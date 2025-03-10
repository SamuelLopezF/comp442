#include "../include/ast.h"
#include <iostream>
#include <fstream>

class ASTPrintVisitor : public ASTVisitor {
public:
    std::ofstream out;
    int indentLevel = 0;

    ASTPrintVisitor(const std::string& outputFile) {
        out.open(outputFile);
    }

    ~ASTPrintVisitor() {
        out.close();
    }

    void indent() {
        for (int i = 0; i < indentLevel; ++i) {
            out << "  ";
        }
    }

    void visit(Program& node) override {
        out << "Program\n";
        ++indentLevel;
        for (auto decl : node.declarations) {
            decl->accept(*this);
        }
        --indentLevel;
    }

    void visit(ClassDecl& node) override {
        indent();
        out << "ClassDecl: " << node.name << "\n";
        ++indentLevel;
        for (auto member : node.members) {
            member->accept(*this);
        }
        --indentLevel;
    }

    void visit(FuncDecl& node) override {
        indent();
        out << "FuncDecl: " << node.name << "\n";
        ++indentLevel;
        for (auto param : node.params) {
            param->accept(*this);
        }
        node.returnType->accept(*this);
        node.body->accept(*this);
        --indentLevel;
    }

    void visit(VarDecl& node) override {
        indent();
        out << "VarDecl: " << node.name << "\n";
        ++indentLevel;
        node.type->accept(*this);
        --indentLevel;
    }

    void visit(Type& node) override {
        indent();
        out << "Type: " << node.name << "\n";
    }

    void visit(Statement& node) override {}

    void visit(IfStatement& node) override {
        indent();
        out << "IfStatement\n";
        ++indentLevel;
        node.condition->accept(*this);
        node.thenStmt->accept(*this);
        if (node.elseStmt) {
            node.elseStmt->accept(*this);
        }
        --indentLevel;
    }

    void visit(WhileStatement& node) override {
        indent();
        out << "WhileStatement\n";
        ++indentLevel;
        node.condition->accept(*this);
        node.body->accept(*this);
        --indentLevel;
    }

    void visit(ReturnStatement& node) override {
        indent();
        out << "ReturnStatement\n";
        ++indentLevel;
        node.expression->accept(*this);
        --indentLevel;
    }

    void visit(AssignStatement& node) override {
        indent();
        out << "AssignStatement\n";
        ++indentLevel;
        node.lhs->accept(*this);
        node.rhs->accept(*this);
        --indentLevel;  
    }

    void visit(Expression& node) override {}

    void visit(BinaryExpression& node) override {
        indent();
        out << "BinaryExpression: " << node.op << "\n";
        ++indentLevel;
        node.left->accept(*this);
        node.right->accept(*this);
        --indentLevel;
    }

    void visit(UnaryExpression& node) override {
        indent();   
        out << "UnaryExpression: " << node.op << "\n";
        ++indentLevel;
        node.expr->accept(*this);
        --indentLevel;
    }

    void visit(CallExpression& node) override {
        indent();
        out << "CallExpression\n";  
        ++indentLevel;
        node.callee->accept(*this);
        for (auto arg : node.args) {
            arg->accept(*this);
        }
        --indentLevel;
    }

    void visit(Identifier& node) override {
        indent();
        out << "Identifier: " << node.name << "\n";  
    }

    void visit(IntegerLiteral& node) override {
        indent();
        out << "IntegerLiteral: " << node.value << "\n";
    }

    void visit(FloatLiteral& node) override {
        indent(); 
        out << "FloatLiteral: " << node.value << "\n";
    }
};

void printAST(ASTNode* root, const std::string& outputFile) {
    ASTPrintVisitor visitor(outputFile);
    root->accept(visitor);
}
