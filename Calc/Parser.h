#pragma once
#include "Lexer.h"
#include "Types.h"

struct Num;
struct Var;
struct UnaryOp;
struct BinOp;
struct Assign;
struct CondOp;
struct Assign;

typedef variant<Var*, Assign*, BinOp*, CondOp*, Num*, UnaryOp*, monostate>
	TElement;

struct Num {
	Token token;
	string value;
	Num(Token token) : token(token), value(token.value) {}
};

struct Var {
	Token token;
	string value;
	Var(Token token) : token(token), value(token.value) {}
};

struct UnaryOp {
	TElement comp;
	Token token, op;
	UnaryOp(Token op, TElement comp) : token(op), op(op), comp(comp) {}
};

struct BinOp {
	TElement left, right;
	Token token, op;
	BinOp(TElement left, Token op, TElement right)
		: left(left), token(op), op(op), right(right) {}
};

struct CondOp {
	TElement condition, if_result, else_result;
	CondOp(TElement condition, TElement if_result, TElement else_result)
		: condition(condition),
		  if_result(if_result),
		  else_result(else_result) {}
};

struct Assign {
	Var left;
	TElement right;
	Token token, op;
	Assign(Var left, Token op, TElement right)
		: left(left), token(op), op(op), right(right) {}
};

struct Parser {
	Lexer lexer;
	Token current_token;

	Parser(Lexer lexer) : lexer(lexer) {
		current_token = Parser::lexer.get_next_token();
	}

	void continue_() {
		lexer.insert(read("> " + lexer.getProcessedText()));
		eat(EOF);
	}

	void eat(string token_type) {
		if (current_token.type == token_type) {
			current_token = lexer.get_next_token();
		} else {
			if (current_token.type == EOF)
				continue_();
			else
				throw string("Unexpected token " + current_token.type);
		}
	}

	TElement factor() {
		Token token = current_token;
		if (token.type == PLUS) {
			eat(PLUS);
			TElement node = new UnaryOp(token, factor());
			return node;
		} else if (token.type == MINUS) {
			eat(MINUS);
			TElement node = new UnaryOp(token, factor());
			return node;
		} else if (token.type == INTEGER_CONST) {
			eat(INTEGER_CONST);
			return new Num(token);
		} else if (token.type == REAL_CONST) {
			eat(REAL_CONST);
			return new Num(token);
		} else if (token.type == LPAREN) {
			eat(LPAREN);
			TElement node = comp();
			eat(RPAREN);
			return node;
		} else if (token.type == EOF) {
			continue_();
			return factor();
		} else if (token.type == SIN || token.type == COS ||
				   token.type == TAN || token.type == COT ||
				   token.type == LG || token.type == LN) {
			eat(token.type);
			TElement param;
			if (current_token.type == LPAREN) {
				eat(LPAREN);
				param = comp();
				eat(RPAREN);
			} else {
				param = factor();
			}
			return new UnaryOp(token, param);
		} else if (token.type == IF) {
			eat(IF);
			eat(LPAREN);
			TElement condition = comp();
			TElement if_result;
			TElement else_result;
			eat(RPAREN);
			if (current_token.type != EOF) {
				if_result = comp();
			} else {
				if_result = monostate{};
			}
			if (current_token.type == ELSE) {
				eat(ELSE);
				else_result = comp();
			} else {
				else_result = monostate{};
			}
			return new CondOp(condition, if_result, else_result);
		} else {
			eat(ID);
			if (current_token.type == ASSIGN ||
				current_token.type == PLUS_ASSIGN ||
				current_token.type == MINUS_ASSIGN ||
				current_token.type == MUL_ASSIGN ||
				current_token.type == DIV_ASSIGN) {
				Var left(token);
				token = current_token;
				eat(current_token.type);
				TElement right = comp();
				TElement node = new Assign(left, token, right);
				return node;
			} else {
				TElement node = new Var(token);
				return node;
			}
		}
	}

	TElement exponent() {
		TElement node = factor();
		while (current_token.type == POW) {
			Token token = current_token;
			eat(POW);
			node = new BinOp(node, token, comp());
		}
		return node;
	}

	TElement term() {
		TElement node = exponent();
		while ((current_token.type == MUL) ||
			   (current_token.type == INTEGER_DIV) ||
			   (current_token.type == FLOAT_DIV) ||
			   (current_token.type == MOD)) {
			Token token = current_token;
			eat(token.type);
			node = new BinOp(node, token, exponent());
		}
		return node;
	}

	TElement expr() {
		TElement node = term();
		while ((current_token.type == PLUS) || (current_token.type == MINUS)) {
			Token token = current_token;
			eat(token.type);
			node = new BinOp(node, token, term());
		}

		return node;
	}

	TElement comp() {
		TElement node = expr();
		while ((current_token.type == EQUAL) || (current_token.type == LT) ||
			   (current_token.type == GT) || (current_token.type == LE) ||
			   (current_token.type == GE)) {
			Token token = current_token;
			eat(token.type);
			node = new BinOp(node, token, expr());
		}

		return node;
	}

	TElement parse() { return comp(); }
};