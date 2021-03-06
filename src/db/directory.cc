#include "db/directory.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>


using namespace std;

namespace db {
	Directory::Directory(const string &p) : path(p) {
		closedir(open_dir()); // Create and check access.
	}

	Directory::iterator Directory::begin() {
		return DirIterator(open_dir());
	}

	Directory::const_iterator Directory::begin() const {
		return DirIterator(open_dir());
	}

	vector<string> Directory::list_files() const {
		return list_type(DT_REG);
	}

	vector<string> Directory::list_dirs() const {
		return list_type(DT_DIR);
	}

	void Directory::mk_dir(const string &name) {
		mk_dir_helper(path + "/" + name);
	}

	void Directory::delete_dir() {
		delete_dir_rec();
		errno = 0;
		if (remove(path.c_str()) != 0 ) {
			ostringstream ostr;
			ostr << "Error deleting  \"" << path << "\".";
			perror(ostr.str().c_str());
		}
	}

	void Directory::delete_file(const string &file_name) {
		errno = 0;
		string path = full_path(file_name);
		clog << "Deleting file:" << path << endl;
		if (remove(path.c_str()) != 0 ) {
			ostringstream ostr;
			ostr << "Error deleting  \"" << path << "\".";
			perror(ostr.str().c_str());
		}
	}

	void Directory::delete_dir_rec() {
		for (iterator it = begin(); it != end(); ++it) {
			dirent *entity = *it;
			if (entity->d_type == DT_DIR && strcmp(entity->d_name, ".") && strcmp(entity->d_name, "..")) {
				Directory d(full_path(entity->d_name));
				d.delete_dir();
			} else if (entity->d_type == DT_REG) {
				delete_file(entity->d_name);
			} else if (strcmp(entity->d_name, ".") && strcmp(entity->d_name, "..")) {
				cerr << "Could not delete file \"" << entity->d_name << "\"." << endl;
				exit(EXIT_FAILURE);
			}
		}
	}

	bool Directory::file_exists(const string &file_name) const {
		return find_if(begin(), end(), bind2nd(equal_file_name(), file_name)) != end();
	}

	void Directory::mk_dir_helper(string full_path) const {
		errno = 0;
		mkdir(full_path.c_str(), 0777); // Whatever perms in umask.
		if (errno) {
			ostringstream ostr;
			ostr << "Error creating directory \"" << full_path << "\"";
			perror(ostr.str().c_str());
			exit(errno); // Unrecoverable.
		} else {
			clog << "Directory \"" << path << "\" is mkdir'd." << endl;
		}
	}

	DIR *Directory::open_dir() const {
		errno = 0;
		DIR *dir = opendir(path.c_str()); // Check for availability of the directory.
		if (!dir) {
			ostringstream ostr;
			ostr << "Error opening path \"" << path << "\"";
			perror(ostr.str().c_str());
			if (errno == ENOENT) { // No directory.
				mk_dir_helper(path);
				dir = opendir(path.c_str());
			} else {
				exit(errno); // Unrecoverable.
			}
		}
		return dir;
	}

	vector<string> Directory::list_type(unsigned int ent_type) const {
		vector<string> contents;
		for (const_iterator it = begin(); it != end(); ++it) {
			const dirent *entity = *it;
			if (entity->d_type == ent_type && strcmp(entity->d_name, ".") && strcmp(entity->d_name, "..")) {
				contents.push_back(entity->d_name);
			}
		}
		return contents;
	}
}
