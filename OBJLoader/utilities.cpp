#include "utilities.h"
#include <string>
#include <vector>
#include <sstream>

using namespace std;


vector<string> split(const string& line, char c) {
	// Return a vector of strings that were separated by the character c in line
	vector<string> v_strings;
	stringstream ss(line);
	string item;
	while (getline(ss, item, c)) {
		if (item[0] != 'v' && item[0] !='f') {
			v_strings.push_back(item);
		}
	}
	return v_strings;
}