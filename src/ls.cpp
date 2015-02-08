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
#include <time.h>

//#define BLUEDIR "\033[34m";
#define GREENEXE cout <<"\033[32m";
#define DEFAULT cout << "\033[0m";
//#define GRAYHIDDEN "\033";
//#define DEFAULTCOL "\033[0m";
using namespace std;

//void permis(struct stat s);
void outputreg(const vector<string> losfiles, const vector<string> losdirs);
void outputhidden(const vector<string> losfiles, const vector<string> losdirs);
void checkflags(bool dasha, bool dashl, bool dashR, const string &mydot);

int main(int argc, char **argv)
{
	vector <string> userinput;
	vector <string> thefiles;
	vector <string> thedirectories;
	string thedot = ".";
	bool ishidden = false;
	bool islist = false;
	bool isrecursive = false;
	
	for(int i = 0; i < argc; ++i)
	{
		if (argv[i][0] == '-')
		{
			for(int j = 1; argv[i][j] != ' '; j++)
			{
				continue;
			}
		}
		else {userinput.push_back(argv[i]);}
	}

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
	else cout << "-R false\n----------------------------------\n";

	//first thing in vector will always be executable i.e(./a.out)
	//this will make vector so it only contains the filenames & directoies
	userinput.erase(userinput.begin());
//	for (int i = 0; i < userinput.size(); i++)
//	{
//		cout << i << ": "  << userinput.at(i) << endl;
//	}

	//the user input something aside from a flag: a file, directory, or invalid thing
	if(userinput.size() > 0)
	{
		sort(userinput.begin(), userinput.end(), less<string>());	

		for(int i = 0; i < userinput.size(); i++)
		{
			cout << "user entered: " << userinput.at(i) << endl;
		}
		
		for(int i = 0; i < userinput.size(); i++)
		{
			struct stat determine;
			if(stat(userinput.at(i).c_str(), &determine) == -1)
			{
				perror("stat() error\n");
				exit(1);
			}
			if(S_ISDIR(determine.st_mode))
			{
				thedirectories.push_back(userinput.at(i));
			}
			if(S_ISREG(determine.st_mode))
			{
				thefiles.push_back(userinput.at(i));
			}
		}//there are now 2 vectors, one of files and one of directories, what the fuck now?

		sort(thefiles.begin(), thefiles.end(), less<string>());	
		cout << "\nthefiles: " << thefiles.size() << "---------------------------------------" << endl;
		for(int i = 0; i < thefiles.size(); i++)
		{
			cout << thefiles.at(i) << endl;
		}
		
		sort(thedirectories.begin(), thedirectories.end(), less<string>());	
		cout << "\nthedirs: " << thedirectories.size() << endl;
		for(int i = 0; i < thedirectories.size(); i++)
		{
			cout << thedirectories.at(i) << "----------------------------------------------" << endl;
		}
	}
	else
	{
		checkflags(ishidden, islist, isrecursive, thedot);
		cout << "\n";
	}
/*
	//else its just ls, possibly with flags
	//if its just ls
	if((ishidden && islist && isrecursive) == false)
	{
		//setting errno to some obscure value so we know if it changed
		errno = 99;
		const char *dirName = ".";
		DIR *dirp = opendir(dirName);
		if(dirp == NULL) 
		{
			perror("opendir() error\n");
			exit(1);
		}
		dirent *direntp;
		while ((direntp = readdir(dirp)))
		{
			string temp = direntp->d_name;
			if(temp.at(0)  == '.')
			continue;

			if(errno != 99)
			{
				perror("readdir() error\n");
				exit(1);
			}
			cout << direntp->d_name << endl; 
		}
		if(closedir(dirp) == -1)
			{
				perror("closedir() error\n");
				exit(1);
			}
	}

	//there is some sort of flag
	if(ishidden == false)
	{
		outputreg(thefiles, thedirectories);
	}
	else
	{
		outputhidden(thefiles, thedirectories);
	}
*/

	return 0;
}//end of main() -------------------------------------------------------------------------------------------
//------------------------------------------------------------------------
void outputreg(const vector<string> losfiles, const vector<string> losdirs)
{
	for(int i = 0; i < losfiles.size(); i++)
	{
		if(losfiles.empty()) break;
		struct stat filebuf;

		if((stat(losfiles.at(i).c_str(), &filebuf)) == -1)
		{
			perror("stat() error!\n");
			exit(1);
		}
			cout << "FILES::::::";
			cout << losfiles.at(i) << endl;
	}

	for(int i = 0; i < losdirs.size(); i++)
		{
			if(losdirs.empty()) break;
			//errno is set to an obsure number so we know if it changed
			errno = 25 ;
			
			DIR *dirp = opendir(losdirs.at(i).c_str());
			if(dirp == NULL)
			{
				perror("opendir() error\n");
				exit(1);
			}
			dirent *direntp;
			while((direntp = readdir(dirp)))
			{
				string temp = direntp->d_name;
				if(temp.at(0)  == '.')
				continue;

				if(errno != 25)
				{
					perror("readdir() error\n");
					exit(1);
				}
				cout << "DIRECTORY :::::" << losdirs.at(i) << endl;
				cout << direntp->d_name << endl;
				//right here i want to store the names into a vector of direntp
			}
			//closedir(dirp);
			if(closedir(dirp) == -1)
			{
				perror("closedir() error\n");
				exit(1);
			}
			cout << "============================" << endl;
		}
}//void outputreg()
void outputhidden(const vector<string> losfiles, const vector<string> losdirs)
{
	for(int i = 0; i < losfiles.size(); i++)
	{
		if(losfiles.empty()) break;
		struct stat filebuf;

		if((stat(losfiles.at(i).c_str(), &filebuf)) == -1)
		{
			perror("stat() error!\n");
			exit(1);
		}
			cout << "FILES::::::";
			cout << losfiles.at(i) << endl;
	}

	for(int i = 0; i < losdirs.size(); i++)
		{
			if(losdirs.empty()) break;
			//errno is set to an obsure number so we know if it changed
			errno = 25 ;
			
			DIR *dirp = opendir(losdirs.at(i).c_str());
			if(dirp == NULL)
			{
				perror("opendir() error\n");
				exit(1);
			}
			dirent *direntp;
			while((direntp = readdir(dirp)))
			{
				if(errno != 25)
				{
					perror("readdir() error\n");
					exit(1);
				}
				cout << "HIDDEN DIRECTORY :::::" << losdirs.at(i) << endl;
				cout << direntp->d_name << endl;
			}
			//closedir(dirp);
			if(closedir(dirp) == -1)
			{
				perror("closedir() error\n");
				exit(1);
			}
			cout << "============================" << endl;
		}

}
void checkflags(bool dasha, bool dashl, bool dashR, const string &mydot)
{

}//void checkflags-------

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
