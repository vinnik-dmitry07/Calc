#include "stdafx.h"

using namespace std;

#include "Extern.h"
#include "Interpreter.h"

string fix(double in) {
	string s = to_string_max_precision(in);
	size_t dot = s.find_first_of('.'), last = s.find_last_not_of(".0");

	if (dot != string::npos) return s.substr(0, max(dot, last + 1));
	else return s;
}

void calc(string s) {
	if (trim_copy(s) == "check") {
		for (auto elem : GLOBAL_SCOPE) {
			cout << elem.first << " " << elem.second << "\n";
		}
	} else if (trim_copy(s) == "help") {
		clog << "Operation priority:\n"
			"-4 lg\n"
			"-3 mod\n"
			"-2 div\n"
			"-1 float div\n"
			"1 plus\n"
			"2 minus\n"
			"3 multiply\n"
			"4 power\n"
			"5 less\n"
			"6 less or equal\n"
			"7 greater\n"
			"8 greater or equal\n"
			"9 equal\n"
			"10 sin\n"
			"11 cos\n"
			"12 tan\n"
			"13 cot\n"
			"Built-in functions:\n"
			"lg - 10-based logariphm\n"
			"ln - exp-based logariphm\n"
			"sin/cos/tan/cot(x) - sinus/cos/tan/cot of x radians\n";
	} else if (s != "") {
		try {
			Lexer lexer(s);
			Parser parser(lexer);
			Interpreter interpreter(parser);
			double result = interpreter.interpret();

			cout << (interpreter.accurate ? "" : "~") << fix(result) << endl;
		}
		catch (const out_of_range &oor) {
			cerr << "Out of Range error: " << oor.what() << endl;
		}
		catch (string error) {
			cerr << error << endl;
		}
		catch (exception &e) {
			cerr << "Exception: " << e.what() << endl;
		}
	}
}

int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	setlocale(LC_ALL, "uk-UA");

	cout << fixed;
	cout.precision(numeric_limits<double>::max_digits10);

	while (true) {
		calc(read("> "));
	}
	return 0;
}
