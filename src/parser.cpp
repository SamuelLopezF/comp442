#include "../include/parser.h"
#include "../include/ast_builder.h"
#include <iostream>
#include <sstream>
using namespace std;
using token::Token; // Bring Token into scope

// Vector to store tokens for AST building
static vector<Token> parsedTokens;

// --- Helper Function Implementation ---
vector<string> splitProduction(const string &prod) {
        vector<string> symbols;
        istringstream iss(prod);
        string symbol;
        while (iss >> symbol) {
                symbols.push_back(symbol);
        }
        return symbols;
}

map<string, map<string, string>> buildParsingTable() {
  map<string, map<string, string>> table;

  // START -> PROG
  table["START"]["class"] = "PROG";
  table["START"]["implementation"] = "PROG";
  table["START"]["function"] = "PROG";
  table["START"]["constructor"] = "PROG";
  table["START"]["$"] = "PROG";
  table["START"]["int"] = "PROG";
  table["START"]["float"] = "PROG";
  table["START"]["program"] = "PROG";

  // PROG -> CLASSIMPLFUNC PROG | EPSILON
  table["PROG"]["class"] = "CLASSIMPLFUNC PROG";
  table["PROG"]["implementation"] = "CLASSIMPLFUNC PROG";
  table["PROG"]["int"] = "FUNCDEF PROG";
  table["PROG"]["float"] = "FUNCDEF PROG";
  table["PROG"]["function"] = "FUNCDEF PROG";
  table["PROG"]["constructor"] = "FUNCDEF PROG";
  table["PROG"]["program"] = "PROGBLOCK PROG";
  table["PROG"]["$"] = "EPSILON";

  // PROGBLOCK -> program { STATEMENTS }
  table["PROGBLOCK"]["program"] = "program { STATEMENTS }";

  // CLASSIMPLFUNC -> CLASSDECL | IMPLDEF | FUNCDEF
  table["CLASSIMPLFUNC"]["class"] = "CLASSDECL";
  table["CLASSIMPLFUNC"]["implementation"] = "IMPLDEF";
  table["CLASSIMPLFUNC"]["function"] = "FUNCDEF";
  table["CLASSIMPLFUNC"]["constructor"] = "FUNCDEF";

  // CLASSDECL -> class id ISA1 lbrace VISMEMBERDECL rbrace
  table["CLASSDECL"]["class"] = "class id ISA1 { VISMEMBERDECL }";

  // VISMEMBERDECL -> VISIBILITY MEMDECL VISMEMBERDECL | EPSILON
  table["VISMEMBERDECL"]["public"] = "VISIBILITY MEMDECL VISMEMBERDECL";
  table["VISMEMBERDECL"]["private"] = "VISIBILITY MEMDECL VISMEMBERDECL";
  table["VISMEMBERDECL"]["}"] = "EPSILON";
  // Support for class members without visibility modifiers
  table["VISMEMBERDECL"]["float"] = "ATTRDECL VISMEMBERDECL";
  table["VISMEMBERDECL"]["int"] = "ATTRDECL VISMEMBERDECL";
  table["VISMEMBERDECL"]["id"] = "ATTRDECL VISMEMBERDECL";
  table["VISMEMBERDECL"]["function"] = "FUNCDECL VISMEMBERDECL";
  table["VISMEMBERDECL"]["constructor"] = "FUNCDECL VISMEMBERDECL";
  table["VISMEMBERDECL"]["attribute"] = "ATTRDECL VISMEMBERDECL";

  // ISA1 -> isa id ISA2 | EPSILON
  table["ISA1"]["isa"] = "isa id ISA2";
  table["ISA1"]["{"] = "EPSILON";

  // ISA2 -> comma id ISA2 | EPSILON
  table["ISA2"][","] = ", id ISA2";
  table["ISA2"]["{"] = "EPSILON";

  // IMPLDEF -> implementation id lbrace IMPLBODY rbrace
  table["IMPLDEF"]["implementation"] = "implementation id { IMPLBODY }";

  // IMPLBODY -> FUNCDEF IMPLBODY | EPSILON
  table["IMPLBODY"]["function"] = "FUNCDEF IMPLBODY";
  table["IMPLBODY"]["constructor"] = "FUNCDEF IMPLBODY";
  table["IMPLBODY"]["}"] = "EPSILON";

  // FUNCDEF -> TYPE id ( FPARAMS ) FUNCBODY | FUNCHEAD FUNCBODY
  table["FUNCDEF"]["function"] = "FUNCHEAD FUNCBODY";
  table["FUNCDEF"]["constructor"] = "FUNCHEAD FUNCBODY";
  table["FUNCDEF"]["int"] = "TYPE id ( FPARAMS ) FUNCBODY";
  table["FUNCDEF"]["float"] = "TYPE id ( FPARAMS ) FUNCBODY";

  // VISIBILITY -> public | private
  table["VISIBILITY"]["public"] = "public";
  table["VISIBILITY"]["private"] = "private";

  // MEMDECL -> FUNCDECL | ATTRDECL
  table["MEMDECL"]["function"] = "FUNCDECL";
  table["MEMDECL"]["constructor"] = "FUNCDECL";
  table["MEMDECL"]["attribute"] = "ATTRDECL";
  table["MEMDECL"]["float"] = "ATTRDECL";
  table["MEMDECL"]["int"] = "ATTRDECL";
  table["MEMDECL"]["id"] = "ATTRDECL";

  // FUNCDECL -> FUNCHEAD semicolon
  table["FUNCDECL"]["function"] = "FUNCHEAD ;";
  table["FUNCDECL"]["constructor"] = "FUNCHEAD ;";

  // FUNCHEAD -> function id lparen FPARAMS rparen arrow RETURNTYPE | constructor lparen FPARAMS rparen
  table["FUNCHEAD"]["function"] = "function id ( FPARAMS ) => RETURNTYPE";
  table["FUNCHEAD"]["constructor"] = "constructor ( FPARAMS )";

  // FUNCBODY -> lbrace STATEMENTS rbrace
  table["FUNCBODY"]["{"] = "{ STATEMENTS }";

  // ATTRDECL -> attribute VARDECL | TYPE id ARRAYSIZES ;
  table["ATTRDECL"]["attribute"] = "attribute VARDECL";
  // Modified to handle types directly
  table["ATTRDECL"]["float"] = "TYPE id ARRAYSIZES ;";
  table["ATTRDECL"]["int"] = "TYPE id ARRAYSIZES ;";
  table["ATTRDECL"]["id"] = "id : TYPE ARRAYSIZES ;";

  // VARDECL -> id colon TYPE ARRAYSIZES semicolon | TYPE id ARRAYSIZES semicolon
  table["VARDECL"]["id"] = "id : TYPE ARRAYSIZES ;";
  table["VARDECL"]["float"] = "TYPE id ARRAYSIZES ;";
  table["VARDECL"]["int"] = "TYPE id ARRAYSIZES ;";

  // STATEMENTS -> STATEMENT STATEMENTS | EPSILON | VARDECL STATEMENTS
  table["STATEMENTS"]["id"] = "STATEMENT STATEMENTS";
  table["STATEMENTS"]["self"] = "STATEMENT STATEMENTS";
  table["STATEMENTS"]["if"] = "STATEMENT STATEMENTS";
  table["STATEMENTS"]["while"] = "STATEMENT STATEMENTS";
  table["STATEMENTS"]["read"] = "STATEMENT STATEMENTS";
  table["STATEMENTS"]["write"] = "STATEMENT STATEMENTS";
  table["STATEMENTS"]["return"] = "STATEMENT STATEMENTS";
  table["STATEMENTS"]["put"] = "STATEMENT STATEMENTS";
  table["STATEMENTS"]["float"] = "VARDECL STATEMENTS";
  table["STATEMENTS"]["int"] = "VARDECL STATEMENTS";
  table["STATEMENTS"]["}"] = "EPSILON";

  // STATEMENT -> ASSIGNMENT | if lparen RELEXPR rparen then STATBLOCK else STATBLOCK semicolon |
  //              while lparen RELEXPR rparen STATBLOCK semicolon | read lparen VARIABLE rparen semicolon |
  //              write lparen EXPR rparen semicolon | return lparen EXPR rparen semicolon |
  //              put lparen EXPR rparen semicolon
  table["STATEMENT"]["id"] = "ASSIGNMENT";
  table["STATEMENT"]["self"] = "ASSIGNMENT";
  table["STATEMENT"]["if"] = "if ( RELEXPR ) then STATBLOCK else STATBLOCK ;";
  table["STATEMENT"]["while"] = "while ( RELEXPR ) STATBLOCK ;";
  table["STATEMENT"]["read"] = "read ( VARIABLE ) ;";
  table["STATEMENT"]["write"] = "write ( EXPR ) ;";
  table["STATEMENT"]["put"] = "put ( EXPR ) ;";
  table["STATEMENT"]["return"] = "return ( EXPR ) ;";

  // ASSIGNMENT -> id = EXPR ;
  table["ASSIGNMENT"]["id"] = "id = EXPR ;";
  table["ASSIGNMENT"]["self"] = "self = EXPR ;";

  // STATBLOCK -> lbrace STATEMENTS rbrace | STATEMENT | EPSILON
  table["STATBLOCK"]["{"] = "{ STATEMENTS }";
  table["STATBLOCK"]["id"] = "STATEMENT";
  table["STATBLOCK"]["self"] = "STATEMENT";
  table["STATBLOCK"]["if"] = "STATEMENT";
  table["STATBLOCK"]["while"] = "STATEMENT";
  table["STATBLOCK"]["read"] = "STATEMENT";
  table["STATBLOCK"]["write"] = "STATEMENT";
  table["STATBLOCK"]["put"] = "STATEMENT";
  table["STATBLOCK"]["return"] = "STATEMENT";
  table["STATBLOCK"][";"] = "EPSILON"; // For the while statement's optional STATBLOCK

  // EXPR -> ARITHEXPR EXPR2
  table["EXPR"]["id"] = "ARITHEXPR EXPR2";
  table["EXPR"]["self"] = "ARITHEXPR EXPR2";
  table["EXPR"]["float"] = "ARITHEXPR EXPR2";
  table["EXPR"]["floatlit"] = "ARITHEXPR EXPR2";
  table["EXPR"]["integer"] = "ARITHEXPR EXPR2";
  table["EXPR"]["intlit"] = "ARITHEXPR EXPR2";
  table["EXPR"]["("] = "ARITHEXPR EXPR2";
  table["EXPR"]["not"] = "ARITHEXPR EXPR2";
  table["EXPR"]["+"] = "ARITHEXPR EXPR2";
  table["EXPR"]["-"] = "ARITHEXPR EXPR2";

  // EXPR2 -> RELOP ARITHEXPR | EPSILON
  table["EXPR2"]["=="] = "RELOP ARITHEXPR";
  table["EXPR2"]["<>"] = "RELOP ARITHEXPR";
  table["EXPR2"]["<"] = "RELOP ARITHEXPR";
  table["EXPR2"][">"] = "RELOP ARITHEXPR";
  table["EXPR2"]["<="] = "RELOP ARITHEXPR";
  table["EXPR2"][">="] = "RELOP ARITHEXPR";
  table["EXPR2"][")"] = "EPSILON";
  table["EXPR2"][";"] = "EPSILON";
  table["EXPR2"][","] = "EPSILON";

  // RELEXPR -> ARITHEXPR RELOP ARITHEXPR
  table["RELEXPR"]["id"] = "ARITHEXPR RELOP ARITHEXPR";
  table["RELEXPR"]["self"] = "ARITHEXPR RELOP ARITHEXPR";
  table["RELEXPR"]["floatlit"] = "ARITHEXPR RELOP ARITHEXPR";
  table["RELEXPR"]["intlit"] = "ARITHEXPR RELOP ARITHEXPR";
  table["RELEXPR"]["("] = "ARITHEXPR RELOP ARITHEXPR";
  table["RELEXPR"]["not"] = "ARITHEXPR RELOP ARITHEXPR";
  table["RELEXPR"]["+"] = "ARITHEXPR RELOP ARITHEXPR";
  table["RELEXPR"]["-"] = "ARITHEXPR RELOP ARITHEXPR";

  // ARITHEXPR -> TERM RIGHTRECARITHEXPR
  table["ARITHEXPR"]["id"] = "TERM RIGHTRECARITHEXPR";
  table["ARITHEXPR"]["self"] = "TERM RIGHTRECARITHEXPR";
  table["ARITHEXPR"]["floatlit"] = "TERM RIGHTRECARITHEXPR";
  table["ARITHEXPR"]["intlit"] = "TERM RIGHTRECARITHEXPR";
  table["ARITHEXPR"]["integer"] = "TERM RIGHTRECARITHEXPR";
  table["ARITHEXPR"]["("] = "TERM RIGHTRECARITHEXPR";
  table["ARITHEXPR"]["not"] = "TERM RIGHTRECARITHEXPR";
  table["ARITHEXPR"]["+"] = "TERM RIGHTRECARITHEXPR";
  table["ARITHEXPR"]["-"] = "TERM RIGHTRECARITHEXPR";

  // RIGHTRECARITHEXPR -> ADDOP TERM RIGHTRECARITHEXPR | EPSILON
  table["RIGHTRECARITHEXPR"]["+"] = "ADDOP TERM RIGHTRECARITHEXPR";
  table["RIGHTRECARITHEXPR"]["-"] = "ADDOP TERM RIGHTRECARITHEXPR";
  table["RIGHTRECARITHEXPR"]["or"] = "ADDOP TERM RIGHTRECARITHEXPR";
  table["RIGHTRECARITHEXPR"][")"] = "EPSILON";
  table["RIGHTRECARITHEXPR"][";"] = "EPSILON";
  table["RIGHTRECARITHEXPR"][","] = "EPSILON";
  table["RIGHTRECARITHEXPR"]["=="] = "EPSILON";
  table["RIGHTRECARITHEXPR"]["<>"] = "EPSILON";
  table["RIGHTRECARITHEXPR"]["<"] = "EPSILON";
  table["RIGHTRECARITHEXPR"][">"] = "EPSILON";
  table["RIGHTRECARITHEXPR"]["<="] = "EPSILON";
  table["RIGHTRECARITHEXPR"][">="] = "EPSILON";

  // SIGN -> plus | minus
  table["SIGN"]["+"] = "+";
  table["SIGN"]["-"] = "-";

  // TERM -> FACTOR RIGHTRECTERM
  table["TERM"]["id"] = "FACTOR RIGHTRECTERM";
  table["TERM"]["self"] = "FACTOR RIGHTRECTERM";
  table["TERM"]["floatlit"] = "FACTOR RIGHTRECTERM";
  table["TERM"]["intlit"] = "FACTOR RIGHTRECTERM";
  table["TERM"]["integer"] = "FACTOR RIGHTRECTERM";
  table["TERM"]["("] = "FACTOR RIGHTRECTERM";
  table["TERM"]["not"] = "FACTOR RIGHTRECTERM";
  table["TERM"]["+"] = "FACTOR RIGHTRECTERM";
  table["TERM"]["-"] = "FACTOR RIGHTRECTERM";

  // RIGHTRECTERM -> MULTOP FACTOR RIGHTRECTERM | EPSILON
  table["RIGHTRECTERM"]["*"] = "MULTOP FACTOR RIGHTRECTERM";
  table["RIGHTRECTERM"]["/"] = "MULTOP FACTOR RIGHTRECTERM";
  table["RIGHTRECTERM"]["and"] = "MULTOP FACTOR RIGHTRECTERM";
  table["RIGHTRECTERM"]["+"] = "EPSILON";
  table["RIGHTRECTERM"]["-"] = "EPSILON";
  table["RIGHTRECTERM"]["or"] = "EPSILON";
  table["RIGHTRECTERM"][")"] = "EPSILON";
  table["RIGHTRECTERM"][";"] = "EPSILON";
  table["RIGHTRECTERM"][","] = "EPSILON";
  table["RIGHTRECTERM"]["=="] = "EPSILON";
  table["RIGHTRECTERM"]["<>"] = "EPSILON";
  table["RIGHTRECTERM"]["<"] = "EPSILON";
  table["RIGHTRECTERM"][">"] = "EPSILON";
  table["RIGHTRECTERM"]["<="] = "EPSILON";
  table["RIGHTRECTERM"][">="] = "EPSILON";
  table["RIGHTRECTERM"]["("] = "EPSILON"; // Accept '(' for function calls

  // FACTOR -> id | floatlit | intlit | lparen ARITHEXPR rparen | not FACTOR | SIGN FACTOR
  // Replace existing rule to support function calls
  table["FACTOR"]["id"] = "IDNEST";
  table["FACTOR"]["self"] = "self";
  table["FACTOR"]["floatlit"] = "floatlit";
  table["FACTOR"]["intlit"] = "intlit";
  table["FACTOR"]["integer"] = "integer";
  table["FACTOR"]["("] = "( ARITHEXPR )";
  table["FACTOR"]["not"] = "not FACTOR";
  table["FACTOR"]["+"] = "SIGN FACTOR";
  table["FACTOR"]["-"] = "SIGN FACTOR";

  // IDNEST rule for both simple identifiers and function calls
  table["IDNEST"]["id"] = "id IDNESTOP";

  // IDNESTOP can be either empty (simple variable) or open paren (function call)
  table["IDNESTOP"]["("] = "( ARGS )";
  table["IDNESTOP"]["+"] = "EPSILON";
  table["IDNESTOP"]["-"] = "EPSILON";
  table["IDNESTOP"]["*"] = "EPSILON";
  table["IDNESTOP"]["/"] = "EPSILON";
  table["IDNESTOP"][";"] = "EPSILON";
  table["IDNESTOP"][")"] = "EPSILON";
  table["IDNESTOP"][","] = "EPSILON";
  table["IDNESTOP"]["=="] = "EPSILON";
  table["IDNESTOP"]["<>"] = "EPSILON";
  table["IDNESTOP"]["<"] = "EPSILON";
  table["IDNESTOP"][">"] = "EPSILON";
  table["IDNESTOP"]["<="] = "EPSILON";
  table["IDNESTOP"][">="] = "EPSILON";
  table["IDNESTOP"]["and"] = "EPSILON";
  table["IDNESTOP"]["or"] = "EPSILON";

  // Add rules for function arguments
  table["ARGS"]["id"] = "EXPR ARGSTAIL";
  table["ARGS"]["self"] = "EXPR ARGSTAIL";
  table["ARGS"]["("] = "EXPR ARGSTAIL";
  table["ARGS"]["integer"] = "EXPR ARGSTAIL";
  table["ARGS"]["floatlit"] = "EXPR ARGSTAIL";
  table["ARGS"]["intlit"] = "EXPR ARGSTAIL";
  table["ARGS"]["+"] = "EXPR ARGSTAIL";
  table["ARGS"]["-"] = "EXPR ARGSTAIL";
  table["ARGS"]["not"] = "EXPR ARGSTAIL";
  table["ARGS"][")"] = "EPSILON"; // Empty args

  // ARGSTAIL handles comma-separated arguments
  table["ARGSTAIL"][","] = ", EXPR ARGSTAIL";
  table["ARGSTAIL"][")"] = "EPSILON";

  // VARIABLE -> IDORSELF
  table["VARIABLE"]["id"] = "id";
  table["VARIABLE"]["self"] = "self";

  // ARRAYSIZE -> lsqbr ARRAYSIZE2
  table["ARRAYSIZE"]["["] = "[ ARRAYSIZE2";

  // ARRAYSIZE2 -> intlit rsqbr | rsqbr
  table["ARRAYSIZE2"]["intlit"] = "intlit ]";
  table["ARRAYSIZE2"]["integer"] = "integer ]"; // Handle the token type from tokenizer
  table["ARRAYSIZE2"]["]"] = "]";

  // ARRAYSIZES -> ARRAYSIZE ARRAYSIZES | EPSILON
  table["ARRAYSIZES"]["["] = "ARRAYSIZE ARRAYSIZES";
  table["ARRAYSIZES"][";"] = "EPSILON";
  table["ARRAYSIZES"][","] = "EPSILON";
  table["ARRAYSIZES"][")"] = "EPSILON";

  // TYPE -> int | float | id
  table["TYPE"]["int"] = "int";
  table["TYPE"]["float"] = "float";
  table["TYPE"]["id"] = "id";

  // RETURNTYPE -> TYPE | void
  table["RETURNTYPE"]["int"] = "TYPE";
  table["RETURNTYPE"]["float"] = "TYPE";
  table["RETURNTYPE"]["id"] = "TYPE";
  table["RETURNTYPE"]["void"] = "void";

  // FPARAMS -> PARAM PARAMSTAIL | EPSILON
  table["FPARAMS"]["id"] = "PARAM PARAMSTAIL";
  table["FPARAMS"]["int"] = "PARAM PARAMSTAIL";
  table["FPARAMS"]["float"] = "PARAM PARAMSTAIL";
  table["FPARAMS"][")"] = "EPSILON";

  // PARAM -> TYPE id ARRAYSIZES | id : TYPE ARRAYSIZES
  table["PARAM"]["int"] = "TYPE id ARRAYSIZES";
  table["PARAM"]["float"] = "TYPE id ARRAYSIZES";
  table["PARAM"]["id"] = "id : TYPE ARRAYSIZES";

  // PARAMSTAIL -> , PARAM PARAMSTAIL | ; PARAMEND | EPSILON
  table["PARAMSTAIL"][","] = ", PARAM PARAMSTAIL";
  table["PARAMSTAIL"][";"] = "; PARAMEND";
  table["PARAMSTAIL"][")"] = "EPSILON";

  // PARAMEND -> PARAM PARAMSTAIL | EPSILON
  table["PARAMEND"]["int"] = "PARAM PARAMSTAIL";
  table["PARAMEND"]["float"] = "PARAM PARAMSTAIL";
  table["PARAMEND"]["id"] = "PARAM PARAMSTAIL";
  table["PARAMEND"][")"] = "EPSILON";  // Handle trailing semicolon

  // RELOP -> eq | neq | lt | gt | lteq | gteq
  table["RELOP"]["=="] = "==";
  table["RELOP"]["<>"] = "<>";
  table["RELOP"]["<"] = "<";
  table["RELOP"][">"] = ">";
  table["RELOP"]["<="] = "<=";
  table["RELOP"][">="] = ">=";

  // ADDOP -> plus | minus | or
  table["ADDOP"]["+"] = "+";
  table["ADDOP"]["-"] = "-";
  table["ADDOP"]["or"] = "or";

  // MULTOP -> * | / | and
  table["MULTOP"]["*"] = "*";
  table["MULTOP"]["/"] = "/";
  table["MULTOP"]["and"] = "and";

  // IDORSELF -> id | self
  table["IDORSELF"]["id"] = "id";
  table["IDORSELF"]["self"] = "self";

  // Some special terminal substitutions
  table["EPSILON"]["$"] = ""; // Epsilon matches nothing

  return table;
}

// --- Incremental Parser Interface ---
// Static variables holding the parser state.
static vector<string> parseStack;
static map<string, map<string, string>> parsingTable;

void initParserState() {
        parsedTokens.clear(); // Clear the stored tokens
        parsingTable = buildParsingTable();
        parseStack.clear();
        parseStack.push_back("$");
        parseStack.push_back("START");
}

bool feedToken(const Token &token) {
        // Store the token for AST building
        parsedTokens.push_back(token);
        
        // Use the provided token as the current token.
        Token currentToken = token;
        // When the token type is "reserved" or "operator", use its value for lookup.
        string lookahead = currentToken.type;
        if (lookahead == "reserved" || lookahead == "operator") {
                lookahead = currentToken.value;
        }

        // Process until we either match the token or an error occurs.
        while (!parseStack.empty()) {
                string top = parseStack.back();
                // If both top and token indicate end-of-input.
                if (top == "$" && currentToken.type == "$") {
                        parseStack.pop_back();
                        return true;
                }
                bool isNonTerminal = (parsingTable.find(top) != parsingTable.end());
                if (!isNonTerminal) {
                        // Top is terminal: it must match the current token.
                        if (top == currentToken.type || top == currentToken.value) {
                                parseStack.pop_back();
                                return true; // Token successfully matched.
                        } else {
                                cout << "Syntax error: expected token '" << top << "', but found '" << currentToken.type << "' (value: " << currentToken.value << ")." << endl;
                                return false;
                        }
                } else {
                        // Top is nonterminal: look up production using lookahead.
                        auto rowIt = parsingTable.find(top);
                        if (rowIt == parsingTable.end() || rowIt->second.find(lookahead) == rowIt->second.end()) {
                                cout << "Syntax error: no production for nonterminal '" << top << "' with lookahead token '" << lookahead << "'." << endl;
                                return false;
                        }
                        string production = rowIt->second.at(lookahead);
                        cout << top << " -> " << production << endl;
                        parseStack.pop_back();
                        if (production != "EPSILON") { // Check for "EPSILON" instead of "ε"
                                vector<string> symbols = splitProduction(production);
                                // Push symbols in reverse order so that the first symbol is on top.
                                for (auto it = symbols.rbegin(); it != symbols.rend(); ++it) {
                                        parseStack.push_back(*it);
                                }
                        }
                        // Continue processing the same token.
                }
        }
        cout << "Syntax error: parse stack emptied before consuming token '" << currentToken.type << "' (value: " << currentToken.value << ")." << endl;
        return false;
}

ASTNode* buildASTFromTokens() {
    try {
        // Build AST from stored tokens
        if (!parsedTokens.empty()) {
            return buildAST(parsedTokens);
        } else {
            cout << "Warning: No tokens available to build AST" << endl;
            return nullptr;
        }
    } catch (const exception& e) {
        cerr << "Error building AST: " << e.what() << endl;
        return nullptr;
    }
}
