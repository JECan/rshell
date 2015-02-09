#include <iostream>
#include <cctype>
#include <string.h>
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

#define TERMINALSZ 72;
//#define BLUEDIR "\033[34m";
#define GREENEXE "\033[1;32m";
#define DEFAULT "\033[0;00m";
//#define GRAYHIDDEN "\033";
//#define DEFAULTCOL "\033[0m";
using namespace std;

void permis(const string &thingy);
void checkflags(const bool &dasha, const bool &dashl, const string &mydot);
void outputfile(const bool &dashl, const string &myfiles);
void dothedir(const bool &dasha, const bool &dashl, const string &mydir);
void recurse(const bool &dasha, const bool &dashl, const string &mydir);
bool alphabet(const string &alpha, const string &beta);
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
					continue;
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
		sort(userinput.begin(), userinput.end(), alphabet);	
		int charlength = 1;
//		for(int i = 0; i < userinput.size(); i++)
//		{
//			cout << "user entered: " << userinput.at(i) << endl;
//		}
		
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
		sort(thedirectories.begin(), thedirectories.end(), alphabet);	
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
		if(isrecursive == false)
			checkflags(ishidden, islist, thedot);
		cout << endl;
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
		cout << left << setw(myfiles.size()+1) << myfiles << endl;
	}
	else//we have to output its properties. 
		permis(temp);
}//void outputfile()

void dothedir(const bool &dasha, const bool &dashl, const string &mydir)
{
	vector<string> vec1;//-l vector
	vector<string> vec2;//-al vector
	string dummy;
	string dummy1;
	string dummy2;
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
		if((dasha == false) && (dashl == false))
		{
			dummy = direntp->d_name;
			if(dummy.at(0) == '.')
				continue;
			cout << direntp->d_name << endl;
		}

		//case 01: hidden false, list true
		else if((dasha == false) && (dashl == true))
		{
			dummy1 = direntp->d_name;	
			if(dummy1.at(0) == '.')
				continue;
			vec1.push_back(dummy1);
			sort(vec1.begin(),vec1.end(), alphabet);
		}

		//case 10: hidden true, list false
		else if((dasha == true) && (dashl == false))
		{
			cout << direntp->d_name << endl;
		}

		//case 11: hidden true, list rue
		else if((dasha == true) && (dashl == true))
		{
			dummy2 = direntp->d_name;
			vec2.push_back(dummy2);
			sort(vec2.begin(),vec2.end(), alphabet);
		}
	}

	if((dasha == false) && (dashl == true))
	{
		for(int i = 0; i < vec1.size(); i++)
		{
			string mytemp = mydir;
			if(mydir.at(mydir.size()-1) != '/')
				mytemp += '/';
			mytemp += vec1.at(i);
			permis(mytemp);
		}
	}

	if((dasha == true) && (dashl == true))
	{
		for(unsigned i = 0; i < vec2.size(); i++)
		{
			string mytemp = mydir;
			if(mydir.at(mydir.size()-1) != '/')
				mytemp += '/';
			mytemp += vec2.at(i);
			permis(mytemp);

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

}//void recurse()

void checkflags(const bool &dasha, const bool &dashl, const string &mydot)
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
	sort(docheck.begin(),docheck.end(), alphabet);
	//cases: -a, -l, -R, -al, -aR, -lR, -alR
	
	//case 00: hidden and list false
	if((dasha == false) && (dashl == false))
	{
		for(int i = 0; i < docheck.size(); i++)
		{
			if(docheck.at(i).at(0) == '.')
				continue;
			cout << docheck.at(i) << endl;
		}
	}

	//case 01: hidden false, list true
	else if((dasha == false) && (dashl == true))
	{
		for(int i = 0; i < docheck.size(); i++)
		{
			if(docheck.at(i).at(0) == '.')
				continue;
			permis(docheck.at(i));
		}
	}

	//case 10: hidden true, list false
	else if((dasha == true) && (dashl == false))
	{
		for(int i = 0; i < docheck.size(); i++)
		{
			cout << docheck.at(i) << endl;
		}
	}

	//case 11: hidden true, list rue
	else if((dasha == true) && (dashl == true))
	{
		for(int i = 0; i < docheck.size(); i++)
		{
			permis(docheck.at(i));	
		}
	}

}//void checkflags-------

void permis(const string &thingy)
{
	struct passwd *mypasswd;
	struct group *mygroup;
	struct stat s;
	string lastedit;
	int total;

	if((stat(thingy.c_str(), &s)) == -1)
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
	if(lastedit.at(lastedit.size()-1) == '\n')
		lastedit.at(lastedit.size()-1) = '\0';
	mypasswd = getpwuid(s.st_uid);
	if(mypasswd == NULL) {perror("getpwuid() error");}
	mygroup = getgrgid(s.st_gid);
	if(mygroup == NULL) {perror("getgrgid() error");}
	
	int inodecol = 3;
	int pwnamecol = 10;
	int grnamecol = 8;
	int filesizecol = 8;
	int timecol = 15;

	cout << " " << left << setw(inodecol) << s.st_nlink
		 << setw(pwnamecol) << mypasswd->pw_name
		 << setw(grnamecol) << mygroup->gr_name
		 << setw(filesizecol) << s.st_size
		 << setw(timecol) << lastedit << " " << thingy
		 << endl;
}//void permis()
bool alphabet(const string &alpha, const string &beta)
{
	int i = 0;
	int j = 0;
	//if not hidden we must alphabetize
	if((alpha.size() > 1) && (alpha.at(0) == '.')) i++;
	if((beta.size() > 1) && (beta.at(0) == '.')) j++;
	
	for(; (i < alpha.size()) && (j <beta.size()); i++, j++)
	{
		if(tolower(alpha.at(i)) < tolower(beta.at(j)))
			return true;
		else if(tolower(alpha.at(i)) > tolower(beta.at(j)))
			return false;
	}
	return (alpha.size() < beta.size());
}
