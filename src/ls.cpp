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
#include <pwd.h>
#include <grp.h>

//#define BLUEDIR "\033[34m";
#define GREENEXE "\033[1;32m";
#define DEFAULT "\033[0;00m";
//#define GRAYHIDDEN "\033";
//#define DEFAULTCOL "\033[0m";
using namespace std;

void permis(const char *thingy);
void outputreg(const vector<string> losfiles, const vector<string> losdirs);
void checkflags(bool dasha, bool dashl, bool dashR, const string &mydot);
void outputfile(const bool &dashl, const string &myfiles);
void dothedir(const bool &dasha, const bool &dashl, const string &mydir);
void recurse(const bool &dasha, const bool &dashl, const string &mydir);

int main(int argc, char **argv)
{
	vector <string> userinput;
	vector <string> thefiles;
	vector <string> thedirectories;
	string thedot = ".";
	bool ishidden = false;
	bool islist = false;
	bool isrecursive = false;
	int count;

	for(int i = 0; i < argc; ++i)
	{
		if (argv[i][0] == '-')
			{
				for(int j = 1; argv[i][j] != ' '; j++)
					{continue;}
			}
		else {userinput.push_back(argv[i]);}
	}

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

	userinput.erase(userinput.begin());

	//the user input something aside from a flag: a file, directory, or invalid thing
	if(userinput.size() > 0)
	{
		sort(userinput.begin(), userinput.end(), less<string>());	
		int charlength = 1;
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
				if(userinput.at(i).size() > charlength)
					charlength = userinput.at(i).size();
				thefiles.push_back(userinput.at(i));
			}
		}//there are now 3 vectors,
		//just files, just directories, all userinput
		
		//we can list files straight up, just determine if -l -a
		for(int i = 0; i < userinput.size(); i++)
		{
			struct stat filelist;
			if(S_ISREG(filelist.st_mode))
			{
				outputfile(islist, userinput.at(i));
			}
		}


		//now for directories
		sort(thedirectories.begin(), thedirectories.end(), less<string>());	
		for(int i = 0; i < thedirectories.size(); i++)
		{
			if(thedirectories.empty()) break;
	//		if(isrecursive == true)
	//			recurse();
			dothedir(ishidden, islist, thedirectories.at(i));
		}


		

	}
	else// naked ls, now determine what flags
	{
		checkflags(ishidden, islist, isrecursive, thedot);
		cout << endl << endl;
	}
	return 0;
}//end of main() -------------------------------------------------------------------------------------------
void outputfile(const bool &dashl, const string &myfiles)
{
	//for files we dont care about hidden or recursive
	//all we want to know if list just filename or properties too?
	const char* temp = myfiles.c_str();

	if(dashl == false)//if we dont have to list properties, just output file
	{
		cout << myfiles << "\t---its working!" << endl;
	}
	else//we have to output its properties. 
		permis(temp);
}//void outputfile()

void dothedir(const bool &dasha, const bool &dashl, const string &mydir)
{
	vector<const char *>contents;
	string dummy;
	//setting errno to an obscure value so we know if it changed
	const char* temp = mydir.c_str();
	DIR *dirp = opendir(temp);
	if(dirp == NULL)
	{
		perror("opendir() error\n");
		exit(1);
	}
	dirent *direntp;

	cout << mydir << ":" << endl;
	while((direntp = readdir(dirp)))
	{
		errno = 33;
		if(errno != 33)
		{
			perror("readdir() erroar");
			exit(1);
		}

		//case 00: hidden and list false
		//WORKS!!!
		if((dasha == false) && (dashl == false))
		{
			dummy = direntp->d_name;
			if(dummy.at(0) == '.')
			continue;
			cout << direntp->d_name << endl;
		}

		//case 01: hidden false, list true
		//FIX IT!!!
		else if((dasha == false) && (dashl == true))
		{
			dummy = direntp->d_name;
			if(dummy.at(0) == '.')
			continue;
			contents.push_back(dummy.c_str());
			sort(contents.begin(),contents.end(), less<const char*>());
			for(int i = 0; i < contents.size(); i++)
			{
				permis(contents.at(i));
			}
		}

		//case 10: hidden true, list false
		//WORKS!!!!
		else if((dasha == true) && (dashl == false))
		{
			cout << direntp->d_name << endl;
		}

		//case 11: hidden true, list rue
		//FIX IT
		else if((dasha == true) && (dashl == true))
		{
			dummy = direntp->d_name;
			contents.push_back(dummy.c_str());
			for(int i = 0; i < contents.size(); i++)
			{
				permis(contents.at(i));
			}
		}
	}

	if(closedir(dirp) == -1)
	{
		perror("closedir() error()");
		exit(1);
	}
}//void dothedir()

void recurse(const bool &dasha, const bool &dashl, const string &mydir)
{

}

void outputreg(const vector<string> losfiles, const vector<string> losdirs)
{
/*
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
			if(closedir(dirp) == -1)
			{
				perror("closedir() error\n");
				exit(1);
			}
			cout << "============================" << endl;
		}
*/
}//void outputreg()
void checkflags(bool dasha, bool dashl, bool dashR, const string &mydot)
{
	//set errno to obbcsure value so we know if it changed
	errno = 59;
	vector <string> docheck;
	DIR *dirp = opendir(mydot.c_str());
	if(dirp == NULL)
	{
		perror("opendir() error\n");
		exit(1);
	}
	dirent *direntp;
	while(direntp = readdir(dirp))
	{
		if(errno != 59)
		{
			perror("readdir() error\n");
			exit(1);
		}
		docheck.push_back(direntp->d_name);
	}
	if(closedir(dirp) == -1)
	{
		perror("closedir() error\n");
		exit(1);
	}
	//contents of what we want to ls are now in a vector
	//now do flag check and determine how to output them
	sort(docheck.begin(),docheck.end(), less<string>());
	//cases: -a, -l, -R, -al, -aR, -lR, -alR
}//void checkflags-------

void permis(const char *thingy)
{
	struct passwd *mypasswd;
	struct group *mygroup;
	struct stat s;
	string lastedit;

	if((stat(thingy, &s)) == -1)
	{
		perror("stat() errorrr\n");
		exit(1);
	}
	if(S_ISREG(s.st_mode)) cout << "-";
	else if(S_ISDIR(s.st_mode)) cout << "d";
	else if(S_ISCHR(s.st_mode)) cout << "c";
	else if(S_ISBLK(s.st_mode)) cout << "b";
	else if(S_ISFIFO(s.st_mode)) cout << "f";
	else if(S_ISLNK(s.st_mode)) cout << "l";
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

	lastedit = ctime(&s.st_mtime);
	mypasswd = getpwuid(s.st_uid);
	if(mypasswd == NULL) {perror("getpwuid() error");}
	mygroup = getgrgid(s.st_gid);
	if(mygroup == NULL) {perror("getgrgid() error");}

	cout << " " << s.st_nlink << " " << mypasswd->pw_name
	     << " " << mygroup->gr_name << " " <<  s.st_size 
	     << " " << lastedit << thingy << endl;

}//void permis()
