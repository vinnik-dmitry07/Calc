#pragma once

string read(string req) {
	clog << req;
	string s;
	getline(cin, s);
	return s;
}

template <typename T>
string to_string_max_precision(T value) {
	ostringstream out;
	out.precision(numeric_limits<double>::max_digits10);
	out << fixed << value;
	return out.str();
}

static inline void ltrim(string &s) {
	s.erase(s.begin(), find_if(s.begin(), s.end(),
									[](unsigned char ch) { return !isspace(ch); }));
}

static inline void rtrim(string &s) {
	s.erase(find_if(s.rbegin(), s.rend(),
					[](unsigned char ch) { return !isspace(ch); })
				.base(),
			s.end());
}

static inline void trim(string &s) {
	ltrim(s);
	rtrim(s);
}

static inline string ltrim_copy(string s) {
	ltrim(s);
	return s;
}

static inline string rtrim_copy(string s) {
	rtrim(s);
	return s;
}

static inline string trim_copy(string s) {
	ltrim(s);
	rtrim(s);
	return s;
}