#include "../include/ast_builder.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

// Token stream class to handle token reading and matching
class TokenStream {
public:
    TokenStream(const std::vector<token::Token>& tokens)
        : tokens(tokens), currentPos(0) {}
    
    const token::Token& current() const {
        if (currentPos < tokens.size()) {
            return tokens[currentPos];
        }
        
        // Return end-of-input token
        static token::Token eofToken;
        eofToken.type = "$";
        eofToken.value = "$";
        return eofToken;
    }
    
    void next() {
        if (currentPos < tokens.size()) {
            ++currentPos;
        }
    }
    
    bool match(const std::string& typeOrValue) const {
        const token::Token& token = current();
        return token.type == typeOrValue || token.value == typeOrValue;
    }
    
    bool consume(const std::string& typeOrValue) {
        if (match(typeOrValue)) {
            next();
            return true;
        }
        return false;
    }
    
    bool expect(const std::string& typeOrValue) {
        if (consume(typeOrValue)) {
            return true;
        }
        
        std::cerr << "Error: Expected " << typeOrValue << " but found " 
                 << current().type << " (" << current().value << ") at position " 
                 << currentPos << std::endl;
        return false;
    }
    
    size_t position() const {
        return currentPos;
    }
    
    bool atEnd() const {
        return currentPos >= tokens.size();
    }
    
    // Peek ahead without consuming
    const token::Token& peek(int offset = 1) const {
        if (currentPos + offset < tokens.size()) {
            return tokens[currentPos + offset];
        }
        
        // Return end-of-input token if we peek beyond the end
        static token::Token eofToken;
        eofToken.type = "$";
        eofToken.value = "$";
        return eofToken;
    }
    
private:
    const std::vector<token::Token>& tokens;
    size_t currentPos;
};

// AST builder class that constructs an AST from a token stream
class ASTBuilder {
public:
    ASTBuilder(const std::vector<token::Token>& tokens)
        : tokens(tokens) {}
    
    ASTNode* buildAST() {
        return parseProgram();
    }
    
private:
    TokenStream tokens;
    
    // Parsing functions for each nonterminal in the grammar
    Program* parseProgram();
    ClassDecl* parseClassDecl();
    FuncDecl* parseFuncDecl();
    VarDecl* parseVarDecl();
    Type* parseType();
    Statement* parseStatement();
    IfStatement* parseIfStatement();
    WhileStatement* parseWhileStatement();
    ReturnStatement* parseReturnStatement();
    AssignStatement* parseAssignStatement();
    Expression* parseExpression();
    Expression* parseTerm();
    Expression* parseFactor();
    BinaryExpression* parseBinaryExpression(Expression* left, const std::string& op);
    UnaryExpression* parseUnaryExpression();
    CallExpression* parseCallExpression(Identifier* callee);
    Identifier* parseIdentifier();
    IntegerLiteral* parseIntegerLiteral();
    FloatLiteral* parseFloatLiteral();
    
    // Helper functions
    Statement* parseBlock();
    std::vector<VarDecl*> parseParams();
    std::vector<Expression*> parseExpressionList();
    bool isType();
};

Program* ASTBuilder::parseProgram() {
    std::vector<ASTNode*> declarations;
    
    while (!tokens.atEnd()) {
        if (tokens.match("class")) {
            declarations.push_back(parseClassDecl());
        } 
        else if (tokens.match("function")) {
            declarations.push_back(parseFuncDecl());
        }
        else if (tokens.match("constructor")) {
            declarations.push_back(parseFuncDecl());
        }
        else if (isType()) {
            // Check if this is a function declaration (type followed by id and '(')
            size_t startPos = tokens.position();
            Type* type = parseType();
            
            if (tokens.match("id")) {
                std::string name = tokens.current().value;
                tokens.next();
                
                if (tokens.match("(")) {
                    // This is a function declaration
                    tokens.next(); // consume '('
                    std::vector<VarDecl*> params = parseParams();
                    tokens.expect(")");
                    
                    // Function body
                    Statement* body = nullptr;
                    if (tokens.match("{")) {
                        body = parseBlock();
                    } else {
                        tokens.expect(";");
                    }
                    
                    declarations.push_back(new FuncDecl(name, params, type, body));
                } else {
                    // This is a variable declaration
                    // Handle array dimensions if any
                    while (tokens.consume("[")) {
                        if (tokens.match("intlit") || tokens.match("integer")) {
                            tokens.next();
                        }
                        tokens.expect("]");
                    }
                    
                    tokens.expect(";");
                    declarations.push_back(new VarDecl(name, type));
                }
            } else {
                std::cerr << "Error: Expected identifier after type at position " << tokens.position() << std::endl;
                // Skip to semicolon
                while (!tokens.atEnd() && !tokens.match(";")) {
                    tokens.next();
                }
                if (!tokens.atEnd()) tokens.next(); // Skip semicolon
            }
        } 
        else if (tokens.match("program")) {
            // Parse program block
            tokens.expect("program");
            tokens.expect("{");
            
            // Parse statements and variable declarations
            while (!tokens.match("}")) {
                if (tokens.atEnd()) {
                    std::cerr << "Error: Unexpected end of input while parsing program block" << std::endl;
                    break;
                }
                
                if (isType()) {
                    // Variable declaration
                    declarations.push_back(parseVarDecl());
                } else {
                    // Statement
                    Statement* stmt = parseStatement();
                    if (stmt) {
                        declarations.push_back(stmt);
                    } else {
                        // Skip unexpected token
                        std::cerr << "Skipping unexpected token in program block: " << tokens.current().type 
                                 << " (" << tokens.current().value << ")" << std::endl;
                        tokens.next();
                    }
                }
            }
            
            tokens.expect("}");
        } 
        else {
            // Skip unknown token
            std::cerr << "Skipping unexpected token: " << tokens.current().type 
                      << " (" << tokens.current().value << ")" << std::endl;
            tokens.next();
        }
    }
    
    return new Program(declarations);
}

bool ASTBuilder::isType() {
    return tokens.match("int") || tokens.match("float") || 
           (tokens.match("id") && (tokens.peek().type == "id" || tokens.peek().type == ":"));
}

ClassDecl* ASTBuilder::parseClassDecl() {
    tokens.expect("class");
    
    // Parse class name
    std::string name;
    if (tokens.match("id")) {
        name = tokens.current().value;
        tokens.next();
    }
    
    // Skip "isa" inheritance
    if (tokens.consume("isa")) {
        while (tokens.match("id")) {
            tokens.next();
            if (!tokens.consume(",")) {
                break;
            }
        }
    }
    
    tokens.expect("{");
    
    // Parse class members
    std::vector<ASTNode*> members;
    while (!tokens.match("}")) {
        if (tokens.match("public") || tokens.match("private")) {
            tokens.next(); // Skip visibility modifier
        }
        
        if (tokens.match("function") || tokens.match("constructor")) {
            members.push_back(parseFuncDecl());
        } else if (isType() || tokens.match("attribute")) {
            members.push_back(parseVarDecl());
        } else {
            // Skip unknown token
            std::cerr << "Skipping unexpected token in class: " << tokens.current().type 
                      << " (" << tokens.current().value << ")" << std::endl;
            tokens.next();
        }
    }
    
    tokens.expect("}");
    
    return new ClassDecl(name, members);
}

FuncDecl* ASTBuilder::parseFuncDecl() {
    std::string name;
    std::vector<VarDecl*> params;
    Type* returnType = nullptr;
    Statement* body = nullptr;
    
    // Function head
    if (tokens.consume("function")) {
        if (tokens.match("id")) {
            name = tokens.current().value;
            tokens.next();
        }
        
        tokens.expect("(");
        params = parseParams();
        tokens.expect(")");
        
        // Return type
        if (tokens.consume("=>")) {
            returnType = parseType();
        } else {
            // Default to void if not specified
            returnType = new Type("void");
        }
        
        // Function body
        if (tokens.match("{")) {
            body = parseBlock();
        } else {
            tokens.expect(";");
        }
    } else if (tokens.consume("constructor")) {
        name = "constructor";
        
        tokens.expect("(");
        params = parseParams();
        tokens.expect(")");
        
        // Constructor doesn't have a return type
        returnType = new Type("void");
        
        // Function body
        if (tokens.match("{")) {
            body = parseBlock();
        } else {
            tokens.expect(";");
        }
    }
    
    return new FuncDecl(name, params, returnType, body);
}

std::vector<VarDecl*> ASTBuilder::parseParams() {
    std::vector<VarDecl*> params;
    
    if (!tokens.match(")")) {  // Check if parameter list is not empty
        do {
            if (isType()) {
                // Parameter with type-first syntax
                Type* type = parseType();
                
                std::string paramName;
                if (tokens.match("id")) {
                    paramName = tokens.current().value;
                    tokens.next();
                } else {
                    paramName = "unnamed";
                }
                
                // Handle array dimensions if any
                while (tokens.consume("[")) {
                    if (tokens.match("intlit") || tokens.match("integer")) {
                        tokens.next();
                    }
                    tokens.expect("]");
                }
                
                params.push_back(new VarDecl(paramName, type));
            } 
            else if (tokens.match("id")) {
                // Parameter with name-first syntax (id : type)
                std::string paramName = tokens.current().value;
                tokens.next();
                
                tokens.expect(":");
                Type* type = parseType();
                
                // Handle array dimensions if any
                while (tokens.consume("[")) {
                    if (tokens.match("intlit") || tokens.match("integer")) {
                        tokens.next();
                    }
                    tokens.expect("]");
                }
                
                params.push_back(new VarDecl(paramName, type));
            }
            
            // Handle parameter separator
            if (tokens.consume(";")) {
                // Semicolon separates parameter groups - continue parsing
                continue;
            }
            
        } while (tokens.consume(","));  // Continue if we see a comma
    }
    
    return params;
}

VarDecl* ASTBuilder::parseVarDecl() {
    if (tokens.consume("attribute")) {
        // Skip attribute keyword
    }
    
    std::string name;
    Type* type = nullptr;
    
    if (tokens.match("id") && tokens.peek().value == ":") {
        // Format: id : type
        name = tokens.current().value;
        tokens.next(); // skip id
        
        tokens.expect(":");
        type = parseType();
    } else {
        // Format: type id
        type = parseType();
        
        if (tokens.match("id")) {
            name = tokens.current().value;
            tokens.next();
        } else {
            name = "unnamed";
        }
    }
    
    // Skip array sizes
    while (tokens.consume("[")) {
        if (tokens.match("intlit") || tokens.match("integer")) {
            tokens.next();
        }
        tokens.expect("]");
    }
    
    tokens.expect(";");
    
    return new VarDecl(name, type);
}

Type* ASTBuilder::parseType() {
    std::string typeName;
    
    if (tokens.match("int")) {
        typeName = "int";
        tokens.next();
    } else if (tokens.match("float")) {
        typeName = "float";
        tokens.next();
    } else if (tokens.match("id")) {
        typeName = tokens.current().value;
        tokens.next();
    } else if (tokens.match("void")) {
        typeName = "void";
        tokens.next();
    } else {
        // If we can't determine the type, default to "unknown"
        typeName = "unknown";
        std::cerr << "Warning: Unknown type encountered, defaulting to 'unknown'" << std::endl;
    }
    
    return new Type(typeName);
}

Statement* ASTBuilder::parseStatement() {
    if (tokens.match("if")) {
        return parseIfStatement();
    } else if (tokens.match("while")) {
        return parseWhileStatement();
    } else if (tokens.match("return")) {
        return parseReturnStatement();
    } else if (tokens.match("id") || tokens.match("self")) {
        std::string idName = tokens.current().value;
        tokens.next();
        
        // Check if this is an assignment statement
        if (tokens.match("=")) {
            tokens.next();
            Expression* rhs = parseExpression();
            tokens.expect(";");
            return new AssignStatement(new Identifier(idName), rhs);
        } 
        // Check if this is a function call
        else if (tokens.match("(")) {
            // Backtrack to re-parse this as an expression
            tokens.consume("("); // Skip the open parenthesis
            
            std::vector<Expression*> args;
            if (!tokens.match(")")) {
                args = parseExpressionList();
            }
            
            tokens.expect(")");
            tokens.expect(";");
            
            // Create a call expression statement
            CallExpression* call = new CallExpression(new Identifier(idName), args);
            // This is a hack - we should have a proper ExpressionStatement class
            return new AssignStatement(new Identifier("_unused"), call);
        }
        
        // If not an assignment or call, skip to semicolon
        while (!tokens.atEnd() && !tokens.match(";")) {
            tokens.next();
        }
        tokens.consume(";");
        return nullptr;
    } else if (tokens.match("{")) {
        return parseBlock();
    } else if (isType()) {
        // Handle variable declarations 
        parseVarDecl(); // Process the variable declaration
        return nullptr; // But return nullptr since VarDecl is not a Statement
    } else if (!tokens.atEnd()) {
        // Skip unknown token
        std::cerr << "Skipping unexpected token in statement: " << tokens.current().type 
                  << " (" << tokens.current().value << ")" << std::endl;
        tokens.next();
        return nullptr;
    } else {
        return nullptr;
    }
}

Statement* ASTBuilder::parseBlock() {
    tokens.expect("{");
    
    // Parse statements until we hit the closing brace
    Statement* lastStmt = nullptr;
    while (!tokens.match("}")) {
        if (tokens.atEnd()) {
            std::cerr << "Error: Unexpected end of input while parsing block" << std::endl;
            break;
        }
        
        if (isType()) {
            // Variable declaration
            parseVarDecl();
        } else {
            // Regular statement
            Statement* stmt = parseStatement();
            if (stmt) {
                lastStmt = stmt;
            }
        }
    }
    
    tokens.expect("}");
    
    // For simplicity, we just return the last statement
    // A more complete implementation would collect all statements in a block
    return lastStmt;
}

IfStatement* ASTBuilder::parseIfStatement() {
    tokens.expect("if");
    tokens.expect("(");
    
    Expression* condition = parseExpression();
    
    tokens.expect(")");
    tokens.expect("then");
    
    Statement* thenStmt = parseStatement();
    
    Statement* elseStmt = nullptr;
    if (tokens.consume("else")) {
        elseStmt = parseStatement();
    }
    
    tokens.expect(";");
    
    return new IfStatement(condition, thenStmt, elseStmt);
}

WhileStatement* ASTBuilder::parseWhileStatement() {
    tokens.expect("while");
    tokens.expect("(");
    
    Expression* condition = parseExpression();
    
    tokens.expect(")");
    
    Statement* body = parseStatement();
    
    tokens.expect(";");
    
    return new WhileStatement(condition, body);
}

ReturnStatement* ASTBuilder::parseReturnStatement() {
    tokens.expect("return");
    tokens.expect("(");
    
    Expression* expr = parseExpression();
    
    tokens.expect(")");
    tokens.expect(";");
    
    return new ReturnStatement(expr);
}

AssignStatement* ASTBuilder::parseAssignStatement() {
    Identifier* lhs = parseIdentifier();
    
    tokens.expect("=");
    
    Expression* rhs = parseExpression();
    
    tokens.expect(";");
    
    return new AssignStatement(lhs, rhs);
}

Expression* ASTBuilder::parseExpression() {
    // Parse the first term
    Expression* left = parseTerm();
    
    // Look for binary operators
    while (tokens.match("+") || tokens.match("-") || tokens.match("or")) {
        std::string op = tokens.current().value;
        tokens.next();
        
        Expression* right = parseTerm();
        left = new BinaryExpression(op, left, right);
    }
    
    return left;
}

Expression* ASTBuilder::parseTerm() {
    // Parse the first factor
    Expression* left = parseFactor();
    
    // Look for multiplicative operators
    while (tokens.match("*") || tokens.match("/") || tokens.match("and")) {
        std::string op = tokens.current().value;
        tokens.next();
        
        Expression* right = parseFactor();
        left = new BinaryExpression(op, left, right);
    }
    
    return left;
}

Expression* ASTBuilder::parseFactor() {
    // Handle unary operators
    if (tokens.match("+") || tokens.match("-") || tokens.match("not")) {
        std::string op = tokens.current().value;
        tokens.next();
        Expression* operand = parseFactor();
        return new UnaryExpression(op, operand);
    }
    
    // Parse primary expression
    if (tokens.match("id")) {
        std::string id = tokens.current().value;
        tokens.next();
        
        // Check for function call
        if (tokens.match("(")) {
            tokens.next();
            std::vector<Expression*> args;
            if (!tokens.match(")")) {
                args = parseExpressionList();
            }
            tokens.expect(")");
            return new CallExpression(new Identifier(id), args);
        } else {
            return new Identifier(id);
        }
    } else if (tokens.match("intlit") || tokens.match("integer")) {
        int value = std::stoi(tokens.current().value);
        tokens.next();
        return new IntegerLiteral(value);
    } else if (tokens.match("floatlit") || tokens.match("float")) {
        float value = std::stof(tokens.current().value);
        tokens.next();
        return new FloatLiteral(value);
    } else if (tokens.match("(")) {
        tokens.next();
        Expression* expr = parseExpression();
        tokens.expect(")");
        return expr;
    } else if (tokens.match("self")) {
        tokens.next();
        return new Identifier("self");
    } else {
        // If we can't parse an expression, skip this token and return a placeholder
        std::cerr << "Warning: Unable to parse expression at token " 
                  << tokens.current().type << " (" << tokens.current().value 
                  << "), using placeholder" << std::endl;
        tokens.next();
        return new Identifier("error");
    }
}

BinaryExpression* ASTBuilder::parseBinaryExpression(Expression* left, const std::string& op) {
    Expression* right = parseFactor();
    return new BinaryExpression(op, left, right);
}

UnaryExpression* ASTBuilder::parseUnaryExpression() {
    std::string op;
    if (tokens.match("+") || tokens.match("-") || tokens.match("not")) {
        op = tokens.current().value;
        tokens.next();
    } else {
        return nullptr;
    }
    
    Expression* expr = parseFactor();
    
    return new UnaryExpression(op, expr);
}

CallExpression* ASTBuilder::parseCallExpression(Identifier* callee) {
    tokens.expect("(");
    
    std::vector<Expression*> args;
    if (!tokens.match(")")) {
        args = parseExpressionList();
    }
    
    tokens.expect(")");
    
    return new CallExpression(callee, args);
}

std::vector<Expression*> ASTBuilder::parseExpressionList() {
    std::vector<Expression*> expressions;
    
    // Parse at least one expression
    expressions.push_back(parseExpression());
    
    // Parse remaining expressions separated by commas
    while (tokens.consume(",")) {
        expressions.push_back(parseExpression());
    }
    
    return expressions;
}

Identifier* ASTBuilder::parseIdentifier() {
    std::string name;
    if (tokens.match("id")) {
        name = tokens.current().value;
        tokens.next();
    } else if (tokens.match("self")) {
        name = "self";
        tokens.next();
    } else {
        std::cerr << "Error: Expected identifier but found " 
                  << tokens.current().type << " (" << tokens.current().value << ")" << std::endl;
        name = "error";
    }
    
    return new Identifier(name);
}

IntegerLiteral* ASTBuilder::parseIntegerLiteral() {
    int value = 0;
    if (tokens.match("intlit") || tokens.match("integer")) {
        value = std::stoi(tokens.current().value);
        tokens.next();
    } else {
        std::cerr << "Error: Expected integer literal but found " 
                  << tokens.current().type << " (" << tokens.current().value << ")" << std::endl;
    }
    
    return new IntegerLiteral(value);
}

FloatLiteral* ASTBuilder::parseFloatLiteral() {
    float value = 0.0f;
    if (tokens.match("floatlit") || tokens.match("float")) {
        value = std::stof(tokens.current().value);
        tokens.next();
    } else {
        std::cerr << "Error: Expected float literal but found " 
                  << tokens.current().type << " (" << tokens.current().value << ")" << std::endl;
    }
    
    return new FloatLiteral(value);
}

// Static variable to hold the AST root
static ASTNode* astRoot = nullptr;

// Build an AST from a sequence of tokens
ASTNode* buildAST(const std::vector<token::Token>& tokens) {
    try {
        ASTBuilder builder(tokens);
        astRoot = builder.buildAST();
        return astRoot;
    } catch (const std::exception& e) {
        std::cerr << "Exception during AST building: " << e.what() << std::endl;
        return new Program(std::vector<ASTNode*>());  // Return an empty program on error
    }
}

// Get the root of the AST (implementation of function declared in ast_builder.h)
ASTNode* getASTRoot() {
    return astRoot;
}
