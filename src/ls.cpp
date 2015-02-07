#include <iostream>
#include <algorithm>
#include <sys/types.h>
#include <getopt.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iomanip>

//#define BLUEDIR "\033[34m";
//#define GREENEXE "\033[32m";
//#define GRAYHIDDEN "\033";
//#define DEFAULTCOL "\033[0m";
using namespace std;

//void permis(struct stat s);

int main(int argc, char **argv)
{
//	char *dirName = ".";
//	DIR *dirp = opendir(dirName);
//	dirent *direntp;
//	while ((direntp = readdir(dirp)))
//		cout << direntp->d_name << endl; // use stat here to find attributes of file
//		closedir(dirp);
/*	int flags = 0;
*/
	vector <string> userinput;
	vector <string> thefiles;
	vector <string> thedirectories;
	string thedot = ".";
	bool ishidden;
	bool islist;
	bool isrecursive;
	
	//determine the filenames or directories entered by the user
	for(int i = 0; i < argc; ++i)
	{
		//if flag
		if (argv[i][0] == '-')
		{
			for(int j = 1; argv[i][j] != ' '; j++)
			{
				continue;
			}
		}
		//else its a filename or directory,
		else {userinput.push_back(argv[i]);}
	}

	//determine which of the three flags to execute
	int count; 
	while((count = getopt(argc, argv,"alR")) != -1)
	{
		if (count == '?')
		{
			perror("getopt() error\n");
			exit(1);
		}

		switch(count)
		{
		case 'a':
			ishidden = true;
			break;

		case 'l':
			islist = true;
			break;

		case 'R':
			isrecursive = true;
			break;
		
		default:
			cout << "Error, 'a' 'l' 'R' flags only.\n";
			exit(1);
			break;
		}
	}

	if(ishidden == true) cout << "-a true\n";
	else cout << "-a false\n";
	if(islist == true) cout << "-l true\n";
	else cout << "-l false\n";
	if(isrecursive == true) cout << "-R true\n";
	else cout << "-R false\n";

	//first thing in vector will always be executable i.e(./a.out)
	//this will make vector so it only contains the filenames & directoies
	userinput.erase(userinput.begin());
//	for (int i = 0; i < userinput.size(); i++)
//	{
//		cout << i << ": "  << userinput.at(i) << endl;
//	}


	//sort the user inputs alphabetically
	sort(userinput.begin(), userinput.end(), less<string>());	
	//determine inputs: if its file, directory, or valid at all
	for(int i = 0; i < userinput.size(); i++)
	{
		cout << userinput.at(i) << endl;
	}
	//now sort user input alphabetically


	return 0;
}//end of main()
/*
void permis(struct stat s)
{
	struct passwd *mypasswd;
	struct group *mygroup;

	if(S_ISREG(s.st_mode)) cout << "-";
	else if(S_ISDIR(s.st_mode)) cout << "d";
	else if(S_ISCHR(s.st_mode)) cout << "c";
	else if(S_ISBLK(s.st_mode)) cout << "b";
	else if(S_ISFIFO(s.st_mode)) cout << "f";
	else if(S_ISLINK(s.st_mode)) cout << "l";
	else if(S_ISSOCK(s.st_mode)) cout << "s";
	else cout << "?";
	cout << ((s.st_mode & S_IRUSR) ? "r":"-");
	cout << ((s.st_mode & S_IWUSR) ? "w":"-");
	cout << ((s.st_mode & S_IXUSR) ? "x":"-");
	cout << ((s.st_mode & S_IRGRP) ? "r":"-");
	cout << ((s.st_mode & S_IWGRP) ? "w":"-");
	cout << ((s.st_mode & S_IXGRP) ? "x":"-");
	cout << ((s.st_mode & S_IROTH) ? "r":"-");
	cout << ((s.st_mode & S_IWOTH) ? "w":"-");
	cout << ((s.st_mode & S_IXOTH) ? "x":"-");
	cout << " ";

	//INODE GOES HERE AFTER PERMISS BEFORE USERS
	
	mypasswd = getpwuid(s.st_uid);
	if(mypasswd == NULL) {perror("getpwuid() error");}
	mygroup = getgrgid(s.st_gid);
	if(mygroup == NULL) {perror("getgrgid() error");}

}//void permis()
*/
