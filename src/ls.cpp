#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

using namespace std;

int main()
{
	char *dirName = ".";
	DIR *dirp = opendir(dirName);
	dirent *direntp;
	while ((direntp = readdir(dirp)))
		cout << direntp->d_name << endl; // use stat here to find attributes of file
		closedir(dirp);
}
