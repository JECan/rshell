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
#define GREENEXE cout <<"\033[32m";
#define DEFAULT cout << "\033[0m";
//#define GRAYHIDDEN "\033";
//#define DEFAULTCOL "\033[0m";
using namespace std;

//void permis(struct stat s);
void outputreg(const vector<string> losfiles, const vector<string> losdirs);
void outputhidden(const vector<string> losfiles, const vector<string> losdirs);

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
	bool ishidden = false;
	bool islist = false;
	bool isrecursive = false;
	
	//determine the filenames or directories entered by the user
	//and store them in a vector, disregard the flags
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

	for(int i = 0; i < userinput.size(); i++)
	{
		cout << "user entered: " << userinput.at(i) << endl;
	}
	
	//determines if file or directory, or if input is valid at all
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
	}

	//sort vector of files and directories alphabetically
	sort(thefiles.begin(), thefiles.end(), less<string>());	
	cout << "\nthefiles: " << thefiles.size() << endl;
	for(int i = 0; i < thefiles.size(); i++)
	{
		cout << thefiles.at(i) << endl;
	}
	
	
	sort(thedirectories.begin(), thedirectories.end(), less<string>());	
	cout << "\nthedirs: " << thedirectories.size() << endl;
	for(int i = 0; i < thedirectories.size(); i++)
	{
		cout << thedirectories.at(i) << endl;
	}

	if(ishidden == false)
	{
		outputreg(thefiles, thedirectories);
	}
	else
	{
		outputhidden(thefiles, thedirectories);
	}

	return 0;
}//end of main()
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
//		if(filebuf.st_mode & S_IXUSR)
//		{
//			cout << "ayayayay" << endl;	
//			cout << GREENEXE << losfiles.at(i) << endl;
//			cout << "lololol" << endl;
//		}
//		else
///		{
			cout << "ayayayay" << endl;	
			cout << losfiles.at(i) << endl;
			cout << "lololol" << endl;

//		}
	}

	for(int i = 0; i < losdirs.size(); i++)
		{
			if(losdirs.empty()) break;
			//errno is set to an obsure number so we know if it changed
			errno = 25 ;
			//char *dirname;
			//dirname = losdirs.at(i).c_str();
			
			DIR *dirp = opendir(losdirs.at(i).c_str());
			if(dirp == NULL)
			{
				perror("opendir() error\n");
				exit(1);
			}
			if((losdirs.at(i) == "./") || (losdirs.at(i) == "../"))
				continue;
			dirent *direntp;
			while((direntp = readdir(dirp)))
			{
				if(errno != 25)
				{
					perror("readdir() error\n");
					exit(1);
				}
				cout << direntp->d_name << endl;
			}
			closedir(dirp);
			if(closedir(dirp) == -1)
			{
				perror("closedir() error\n");
				exit(1);
			}
			cout << "asdfasdfasdf" << endl;
		}
}//void outputreg()
void outputhidden(const vector<string> losfiles, const vector<string> losdirs)
{

}

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
