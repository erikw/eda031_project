#include "db/file_art.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

using namespace std;

namespace db {
	istream &operator>>(istream &in, FileArt &file_art) {
		getline(in, file_art.title);
		getline(in, file_art.author);
		in >> noskipws;
		file_art.text = string((istream_iterator<char>(in)), istream_iterator<char>());
		return in;
	}

	ostream &operator<<(ostream &out, const FileArt &file_art) {
		out << file_art.title << endl;
		out << file_art.author << endl;
		out << file_art.text;
		return out;
	}
}
