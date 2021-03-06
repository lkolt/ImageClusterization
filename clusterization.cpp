#include "imageClusterization.h"
#include "boost/filesystem.hpp"

using namespace std;
using namespace cv;
namespace fs = boost::filesystem;

int main(int argc, char* argv[])
{
	int n = argc;
	if (n < 2) {
		cout << "Not enough arguments";
		return -1;
	}

	if (!strcmp(argv[1], "-d")) {
		string path = argv[2];
		for (int i = 3; i < n; i++) {
			path += " ";
			path += argv[i];
		}

		if (path.back() != '\\') {
			path += '\\';
		}

		vector <string> vs;
		for (fs::recursive_directory_iterator it(path), end; it != end; ++it) {
			if (it->status().type() != 3) {
				vs.push_back(it->path().string());
			}
		}

		char **args = (char**)malloc(sizeof(char*) * (vs.size() + 2));

		for (int i = 0; i < vs.size(); i++) {
			char *cstr = new char[vs[i].length() + 1];
			strcpy(cstr, vs[i].c_str());
			args[i + 2] = cstr;
		}

		return clusterization(vs.size() + 2, args);
	} else if (!strcmp(argv[1], "-f")) {
		return clusterization(argc, argv);
	} else {
		cout << "Can't find argument";
		return -1;
	}

	return 0;
}


