#pragma once

#include "Types.h"

struct Token {
	string type, value;

	Token() {}
	Token(string type, string value) : type(type), value(value) {}

	string str() { return "Token(" + type + ", " + value + ")"; }
};

map<string, Token> RESERVED_KEYWORDS{
	{"if",		Token(IF, "if")},
	{"else",	Token(ELSE, "else")},
	{"sin",		Token(SIN, "sin")},
	{"cos",		Token(COS, "cos")},
	{"tan",		Token(TAN, "tan")},
	{"cot",		Token(COT, "cot")},
	{"lg",		Token(LG, "lg")},
	{"ln",		Token(LN, "ln")},
	{"pi",		Token(PI, to_string_max_precision(M_PI))},
	{"exp",		Token(EXP, to_string_max_precision(M_E))}};

class Lexer {
	long long pos = 0;
	unsigned char current_char;
	string text;

	void advance() {
		pos++;
		if (unsigned(pos) > (text.length() - 1))
			current_char = '\0';
		else
			current_char = text[pos];
	}

	void skip_whitespace() {
		while ((current_char != '\0') && (isspace(current_char))) advance();
	}

	Token number() {
		string result = "";
		while ((current_char != '\0') && (isdigit(current_char))) {
			result += current_char;
			advance();
		}

		if (current_char == '.') {
			result += current_char;
			advance();
			while ((current_char != '\0') && (isdigit(current_char))) {
				result += current_char;
				advance();
			}
			Token token = Token(REAL_CONST, result);
			return token;
		} else {
			Token token = Token(INTEGER_CONST, result);
			return token;
		}
	}

	Token id() {
		string result = "";

		while ((current_char != '\0') && (isalpha(current_char))) {
			result += current_char;
			advance();
		}

		auto token = RESERVED_KEYWORDS.find(result);
		if (token == RESERVED_KEYWORDS.end()) return Token(ID, result);
		return token->second;
	}

   public:
	Lexer(string t) {
		t.erase(remove_if(t.begin(), t.end(), [](unsigned char c) { return isspace(c); }), t.end());
		text = t;
		current_char = t[0];
	}

	string getProcessedText() { return text.substr(0, pos); }

	void insert(string s) {
		text.insert(pos, s);
		current_char = text[pos];
	}

	Token get_next_token() {
		while (current_char != '\0') {
			if (isalpha(current_char)) {
				return id();
			} else if (isspace(current_char)) {
				skip_whitespace();
			} else if (isdigit(current_char) ||
					   (current_char == '.' && unsigned(pos) < text.length() - 1 &&
						isdigit(text[pos + 1]))) {
				return number();
			} else if (current_char == '=') {
				advance();
				if (current_char == '=') {
					advance();
					return Token(EQUAL, "==");
				}
				return Token(ASSIGN, "=");
			} else if (current_char == '+') {
				advance();
				if (current_char == '=') {
					advance();
					return Token(PLUS_ASSIGN, "+=");
				}
				return Token(PLUS, "+");
			} else if (current_char == '-') {
				advance();
				if (current_char == '=') {
					advance();
					return Token(MINUS_ASSIGN, "-=");
				}
				return Token(MINUS, "-");
			} else if (current_char == '*') {
				advance();
				if (current_char == '=') {
					advance();
					return Token(MUL_ASSIGN, "*=");
				}
				return Token(MUL, "*");
			} else if (current_char == '/') {
				advance();
				if (current_char == '/') {
					advance();
					return Token(INTEGER_DIV, "//");
				} else if (current_char == '=') {
					advance();
					return Token(DIV_ASSIGN, "/=");
				}
				return Token(FLOAT_DIV, "/");
			} else if (current_char == '%') {
				advance();
				return Token(MOD, "%");
			} else if (current_char == '^') {
				advance();
				return Token(POW, "^");
			} else if (current_char == '(') {
				advance();
				return Token(LPAREN, "(");
			} else if (current_char == ')') {
				advance();
				return Token(RPAREN, ")");
			} else if (current_char == '[') {
				advance();
				return Token(LOP, "[");
			} else if (current_char == ']') {
				advance();
				return Token(ROP, "]");
			} else if (current_char == '<') {
				advance();
				if (current_char == '=') {
					advance();
					return Token(LE, "<=");
				}
				return Token(LT, "<");
			} else if (current_char == '>') {
				advance();
				if (current_char == '=') {
					advance();
					return Token(GE, ">=");
				}
				return Token(GT, ">");
			} else {
				throw string("Invalid syntax");
			}
		}
		return Token(EOF, "\0");
	}
};