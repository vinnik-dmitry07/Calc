#pragma once
#include "Parser.h" 

using namespace std;

map<string, double> GLOBAL_SCOPE;

long long max_accurate = pow(2, DBL_MANT_DIG) - 1;

bool equal(double x, double y) {
	return abs(x - y) <= numeric_limits<double>::epsilon() * max(abs(x), abs(y));
}

class Interpreter {
	Parser parser;

	double visit_UnaryOp(UnaryOp node) {
		string op = node.op.type;
		if		(op == PLUS)	return visit(node.comp);
		else if (op == MINUS)	return -visit(node.comp);
		else if (op == SIN)		return sin(visit(node.comp));
		else if (op == COS)		return sin(visit(node.comp));
		else if (op == TAN)		return sin(visit(node.comp));
		else if (op == COT)		return 1 / tan(visit(node.comp));
		else if (op == LG)		return log10(visit(node.comp));
		else if (op == LN)		return log(visit(node.comp));
		else throw("There is no UnaryOp for operator " + op + "!");
	}

	double visit_BinOp(BinOp node) {
		if		(node.op.type == PLUS)			return (visit(node.left) + visit(node.right));
		else if (node.op.type == MINUS)			return (visit(node.left) - visit(node.right));
		else if (node.op.type == MUL)			return (visit(node.left) * visit(node.right));
		else if (node.op.type == POW)			return pow(visit(node.left), visit(node.right));
		else if (node.op.type == EQUAL)			return equal(double(visit(node.left)), double(visit(node.right)));
		else if (node.op.type == FLOAT_DIV)		return double(visit(node.left)) / double(visit(node.right));
		else if (node.op.type == LT)			return double(visit(node.left)) < double(visit(node.right));
		else if (node.op.type == GT)			return double(visit(node.left)) > double(visit(node.right));
		else if (node.op.type == LE)			return double(visit(node.left)) <= double(visit(node.right));
		else if (node.op.type == GE)			return double(visit(node.left)) >= double(visit(node.right));

		else if (node.op.type == INTEGER_DIV) {
			double div = visit(node.left) / visit(node.right);
			if	(div > numeric_limits<long long>::max())	return INFINITY;
			else if (isfinite(div))							return long long(div);
			else											return div;
		} else if (node.op.type == MOD) {
			long long r =
				long long(visit(node.right));
			if (r == 0) return NAN;
			else return long long(visit(node.left)) % r;
		} else {
			throw("There is no BinOp for operator " + node.op.type + "!");
		}
	}

	double visit_CondOp(CondOp node) {
		if		(visit(node.condition))					return visit(node.if_result);
		else if (!get_if<monostate>(&node.else_result)) return visit(node.else_result);
		else											return 0.0;
	}

	double visit_Num(Num node) {
		double result = stod(node.value);
		if (abs(result - int(result)) > numeric_limits<double>::epsilon())
			accurate = false;
		return result;
	}

	double visit_Assign(Assign node) {
		string var_name		= node.left.value;
		double var_value	= visit(node.right);
		if (node.op.type == PLUS_ASSIGN)
			GLOBAL_SCOPE[var_name] += var_value;
		else if (node.op.type == MINUS_ASSIGN) 
			GLOBAL_SCOPE[var_name] += var_value;
		else if (node.op.type == MUL_ASSIGN)
			GLOBAL_SCOPE[var_name] *= var_value;
		else if (node.op.type == DIV_ASSIGN)
			GLOBAL_SCOPE[var_name] /= var_value;
		else if (node.op.type == ASSIGN)
			GLOBAL_SCOPE[var_name] = var_value;
		return GLOBAL_SCOPE[var_name];
	}

	double visit_Var(Var node) {
		string var_name = node.value;
		auto var_value = GLOBAL_SCOPE.find(var_name);
		if (var_value == GLOBAL_SCOPE.end()) {
			GLOBAL_SCOPE[var_name] = 0.0;
			return 0;
		} else {
			return var_value->second;
		}	
	}

	double visit(TElement node) {
		double result;
		if		(auto num_ptr		= get_if<Num*>(&node))		result = visit_Num(**num_ptr);
		else if (auto bin_ptr		= get_if<BinOp*>(&node))	result = visit_BinOp(**bin_ptr);
		else if (auto unary_ptr		= get_if<UnaryOp*>(&node))	result = visit_UnaryOp(**unary_ptr);
		else if (auto assign_ptr	= get_if<Assign*>(&node))	result = visit_Assign(**assign_ptr);
		else if (auto var_ptr		= get_if<Var*>(&node))		result = visit_Var(**var_ptr);
		else if (auto cond_ptr		= get_if<CondOp*>(&node))	result = visit_CondOp(**cond_ptr);
		else if (auto cond_ptr		= get_if<monostate>(&node))	result = 0.0;
		if (isinf(result) || isnan(result))
			accurate = true;
		else if (result > max_accurate) accurate = false;
		return result;
	}

public:
	bool accurate;
	Interpreter(Parser parser) : accurate(true), parser(parser) {}

	string part() {
		string node = "";

		while (
				parser.current_token.type != LOP &&
				parser.current_token.type != ROP &&
				parser.current_token.type != EOF
		) {
			node += parser.current_token.value;
			parser.eat(parser.current_token.type);
		}
		if (parser.current_token.type == LOP) {
			parser.eat(LOP);
			Lexer l(calculated());
			Parser p(l);
			TElement t = p.parse();
			double r = visit(t);
			if		(r == -4.0) node += "ln";
			else if (r == -3.0) node += "lg";
			else if	(r == -3.0) node += "%";
			else if (r == -2.0) node += "//";
			else if (r == -1.0) node += "/";	
			else if	(r == 1.0) node += "+";
			else if (r == 2.0) node += "-";
			else if (r == 3.0) node += "*";
			else if (r == 4.0) node += "^";
			else if (r == 5.0) node += "<";
			else if (r == 6.0) node += "<=";
			else if (r == 7.0) node += ">";
			else if (r == 8.0) node += ">=";
			else if (r == 9.0) node += "==";
			else if (r == 10.0) node += "sin";
			else if (r == 11.0) node += "cos";
			else if (r == 12.0) node += "tan";
			else if (r == 13.0) node += "cot";
			else throw string("Invalid operation");
			parser.eat(ROP);	
		}
		return node;
	}

	string calculated() {
		string node = part();
		
		while (
			parser.current_token.type != ROP &&
			parser.current_token.type != EOF
		) {
			node += part();
		}
		return node;
	}

	double interpret() {
		Lexer lexer(calculated());
		Parser parser(lexer);
		TElement tree = parser.parse();
		return visit(tree);
	}
};