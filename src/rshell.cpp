#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <boost/tokenizer.hpp>
#include <sys/stat.h>
#include <signal.h>
using namespace std;
using namespace boost;

void parse(const string &usercommand, const char specialchar[], const bool &isOR, const bool &isAND);
void myexec(const string &usercommand);
void inputredirect(const string &usercommand);
void outputredir(const string &usercommand, const bool &weappend);
void splitpipes(const string &usercommand);
void dopipes(char **argv1, char **argv2);
void ctrlc(int signum);
string getpath(string executethis);
void exittheshell();
string gohome(string mypath);
string mypath(string mypath);
string mycmd(string cmd);
int main()
{	
	if(signal(SIGINT, SIG_IGN) == SIG_ERR)
	{
		perror("signal error");
		exittheshell();
	}
	char host[64] = {0};
	string displayuser;
	string userinput = "";
	string username;
	username = getlogin();
	if(username == "")
	{
		perror("getlogin() error.");
	}

	if(gethostname(host,sizeof(host)-1) == -1)
	{
		perror("gethostname() error.");
	}

	for(int i = 0; i < 64; i++)
	{
		if(host[i] == '.') host[i] = '\0';
	}
	
	char mycwd[BUFSIZ];
	if(getcwd(mycwd, sizeof(mycwd)) == NULL)
	{
		perror("getcwd() error");
		exittheshell();
	}

	displayuser = username + "@" + host + "$ ";
	cout << mycwd << endl;
	cout << displayuser;

	char SEMICOLON[] = ";";
	char ANDING[] = "&&";
	char ORING[] = "||";
	bool isOR = false;
	bool isAND = false;
	while(1)
	{
		getline(cin, userinput);
		if(signal(SIGINT, ctrlc) == SIG_ERR)
		{
			perror("signal() error ctrl+c");
			exittheshell();
		}
		if(userinput.find('#') != string::npos)
		{
			userinput = userinput.substr(0, userinput.find('#'));
		}
		//check for special characters ; || &&
		if(userinput.find("&&") != string::npos)
		{
			isAND = true;
			isOR = false;
			parse(userinput,ANDING, isOR, isAND);
		}
		else if(userinput.find("||")!= string::npos)
		{
			isAND = false;
			isOR = true;
			parse(userinput,ORING,isOR,isAND);
		}
		//cheeck for piping and io redirection
		else if(userinput.find("|") != string::npos)
		{
			splitpipes(userinput);
		}
		else if(userinput.find("<") != string::npos)
		{
			inputredirect(userinput);
		}
		else if(userinput.find(">>") != string::npos)
		{
			outputredir(userinput, true);
		}
		else if(userinput.find(">") != string::npos)
		{
			outputredir(userinput, false);
		}
		else
		{
			isAND = false;
			isOR = false;
			parse(userinput,SEMICOLON, isOR, isAND);
		}
		if(getcwd(mycwd, sizeof(mycwd)) == NULL)
		{
			perror("getcwd() error");
			exit(1);
		}
		displayuser = username + "@" + host + "$ ";
		cout << mycwd << endl;
		cout << displayuser;
	}//while(1)
	return 0;
} // end of main
//-------------------------------------------------------------
void parse(const string &usercommand, const char specialchar[], const bool &isOR, const bool &isAND)
{
	typedef tokenizer<char_separator<char> > MYTOKENS;
	char_separator<char> MYSEPARATOR(specialchar);
	MYTOKENS tok(usercommand, MYSEPARATOR);
	for(MYTOKENS::iterator tok_iter = tok.begin();
		tok_iter != tok.end(); ++tok_iter)
	{
		int pid = fork();
		//if pid == -1 throw error
		if(pid == -1)
		{
			perror("error with fork()");
			exittheshell();
		}
		//else if pid == 0 were in child
		else if(pid == 0)
		{
			string dothistoken = *tok_iter;
			myexec(dothistoken);	
		}
		//else we are in parent
		else
		{
			int parentstatus = 0;
			if(wait(&parentstatus) == -1)
			{
				perror("error with wait()");
				exittheshell();
			}
			bool killkid = WIFEXITED(parentstatus);
			if((killkid == true) && (WEXITSTATUS(parentstatus) == 0) && (isOR == true))
			{
				return; 
			}
			else if((killkid == true) && (WEXITSTATUS(parentstatus) == 1) && (isAND == true))
			{
				return; 
			}
			else if((killkid == true) && (WEXITSTATUS(parentstatus) == 99))
			{
				exit(EXIT_SUCCESS);
			}
			else if((killkid == true) && (WEXITSTATUS(parentstatus) == 89))
			{
				string anothertemp = "";
				string holycrap = gohome(anothertemp);
				int i = chdir(holycrap.c_str());
				if(i == -1)
					perror("chdir");
				return;
			}
			else if((killkid == true) && (WEXITSTATUS(parentstatus) == 79))
			{
				cout << "cd with arguement\n";
				string justtemp = "";
				string anothertemp = "";
				string holycrap = gohome(anothertemp);
				string copyofholycrap = holycrap;
				string omgplz = mypath(justtemp); 
				copyofholycrap += omgplz;
				int i = chdir(copyofholycrap.c_str());
				if(i == -1)
					perror("chdir");
				return;
			}

		}
	}
}//void parse()
//-------------------------------------------------------------
void myexec(const string &usercommand)
{
	char *argv[512];
	typedef tokenizer<char_separator<char> > COMTOKEN;
	char_separator<char> COMSEPARATOR (" ");
	COMTOKEN etok(usercommand, COMSEPARATOR);

	typedef tokenizer<char_separator<char> > CHECKTOK;
	char_separator<char> SPACES (" ");
	CHECKTOK specialtok(usercommand,SPACES);
	CHECKTOK::iterator yoloiter = specialtok.begin();
	if(yoloiter != specialtok.end())
	{
		if((*yoloiter) == "exit")
		{
			exit(99);
		}
		if((*yoloiter) == "cd")
		{
			exit(89);
		}
//		if(((*yoloiter) == "cd") && (yoloiter != specialtok.end()))
//		{
//			exit(79);
//		}
	}

	int i = 0;
	for(COMTOKEN::iterator com_iter = etok.begin();
		com_iter != etok.end(); i++, com_iter++)
	{
		argv[i] = new char[(*com_iter).size()];
		strcpy(argv[i],(*com_iter).c_str());
	}
	argv[i]=0;
	string plzwork = mycmd(argv[0]); 
	if(execv(plzwork.c_str(),argv) == -1)
	{
		perror("error execv()");
		exittheshell();
	}
	//must delete argv
	int j = 0;
	for(COMTOKEN::iterator del_iter = etok.begin();
		del_iter != etok.end(); j++, del_iter++)
	{
		delete [] argv[j];
	}
}//void myexec()
//-------------------------------------------------------------
string gohome(string mypath)
{		
	const char* etgohome = getenv("HOME");
	if(chdir(etgohome) == -1)
	{
		perror("chdir() home error");
		exittheshell();
	}

	char mycwd[BUFSIZ];
	if(getcwd(mycwd, sizeof(mycwd)) == NULL)
	{
		perror("getcwd() error");
		exittheshell();
	}
	string yolotemp = mycwd;
	return yolotemp;
}
string mypath(string mypath)
{
	string slash = "/";
	vector <string> vcmd;
	const char* etgopath = getenv("PATH");
	if(chdir(etgopath) == -1)
	{
		perror("chdir() error");
		exittheshell();
	}

	char mycwd[BUFSIZ];
	if(getcwd(mycwd, sizeof(mycwd)) == NULL)
	{
		perror("getcwd() error");
		exittheshell();
	}
	string yolotemp = mycwd;
	typedef tokenizer<char_separator<char> > MYTOKENS;
	char_separator<char> MYSEPARATOR(":");
	MYTOKENS tok(yolotemp, MYSEPARATOR);
	for(MYTOKENS::iterator tok_iter = tok.begin();
		tok_iter != tok.end(); ++tok_iter)
	{
		string appending = *tok_iter + slash + mypath;
		vcmd.push_back(appending);
	}

	return vcmd.at(0);
}
string mycmd(string cmd)
{
	string slash = "/";
	vector <string> vcmd;
	char *myname = getenv("PATH");
	if(myname == NULL)
	{
		perror("getenv() err");
		exittheshell();
	}
	string mytemp = myname;
	typedef tokenizer<char_separator<char> > MYTOKENS;
	char_separator<char> MYSEPARATOR(":");
	MYTOKENS tok(mytemp, MYSEPARATOR);
	for(MYTOKENS::iterator tok_iter = tok.begin();
		tok_iter != tok.end(); ++tok_iter)
	{
		string appending = *tok_iter + slash + cmd;
		vcmd.push_back(appending);
	}
	for(unsigned i = 0; i < vcmd.size(); i++)
	{
		struct stat mybuf;
		if(stat(vcmd.at(i).c_str(), &mybuf) == -1)
		{
			continue;
		}
		else
			return vcmd.at(i);
	}
	//throws error
	return cmd;
}//string mycmd()
//-------------------------------------------------------------
void inputredirect(const string &usercommand)
{
	string carrot = "<";
	int flags = O_RDONLY;
	int carrotloc = usercommand.find(carrot);
	string leftofin = usercommand.substr(0, carrotloc);
	string rightofin = usercommand.substr(usercommand.find(carrot) + 2);
	string cpyright = rightofin;
	if((leftofin == "") || (rightofin == ""))
	{
		cout << "Invalid input\n";
		return;
	}
	//tokenize right side for spaces
	typedef tokenizer<char_separator<char> > MYTOKENS;
	char_separator<char> MYSEPARATOR(" ");
	MYTOKENS tok(rightofin, MYSEPARATOR);
	MYTOKENS::iterator tok_iter = tok.begin();
	//open the file
	int searchthis;
	if((searchthis = open((*tok_iter).c_str(), flags)) == -1)
	{
		perror("open() error");
		return;
	}
	//saving std out 
	int savestdout = dup(0);
	if(savestdout == -1)
	{
		perror("dup() error, output redirection");
		exit(1);
	}
	//setting writeto to the new stdout
	//must restore stdout later
	if(dup2(searchthis, 0) == -1)
	{
		perror("dup2() error, output redirection");
		exit(1);
	}
	//closing writeto
	if (close(searchthis) == -1)
	{
		perror("close() error, output redirection");
		exit(1);
	}
	//parsing left side for spaces, and calliing execvp on leftside
	char *argv[2];
	int i = 0;
	typedef tokenizer<char_separator<char> > COMTOKEN;
	char_separator<char> COMSEPARATOR (" ");
	COMTOKEN etok(leftofin, COMSEPARATOR);

	for(COMTOKEN::iterator com_iter = etok.begin();
		com_iter != etok.end(); i++, com_iter++)
	{
		argv[i] = new char[(*com_iter).size()];
		strcpy(argv[i],(*com_iter).c_str());
	}
	//only want first command, if argv i != NULL then error is thrown	
	argv[i] = NULL;
	string plzwork = mycmd(argv[0]); 
	if(execv(plzwork.c_str(),argv) == -1)
	{
		perror("error execv()");
		exit(1);
	}
	//restoring stdout
	if(dup2(savestdout, 1) == -1)
	{
		perror("dup2 error restoring");
	}
	//closing savestdout
	if(close(savestdout) == -1)
	{
		perror("colse() error");
	}
	//must delete argv
	int j = 0;
	for(COMTOKEN::iterator del_iter = etok.begin();
	del_iter != etok.end(); j++, del_iter++)
	{
		delete [] argv[j];
	}
}//void inputredirect()
//-------------------------------------------------------------
void outputredir(const string &usercommand, const bool &weappend)
{
	string carrot;
	int flags;
	if(weappend == true)
	{
		carrot = ">>";
		flags = O_CREAT | O_WRONLY | O_APPEND;
	}
	else
	{
		carrot = ">";
		flags = O_CREAT | O_WRONLY | O_TRUNC;
	}
	int carrotloc = usercommand.find(carrot);
	string leftofout = usercommand.substr(0, carrotloc);
	string rightofout = usercommand.substr(usercommand.find(carrot) + 2);
	if((leftofout == "") || (rightofout == ""))
	{
		cout << "Invalid entry\n";
		return;	
	}
	//tokenize right side for spaces
	typedef tokenizer<char_separator<char> > MYTOKENS;
	char_separator<char> MYSEPARATOR(" ");
	MYTOKENS tok(rightofout, MYSEPARATOR);
	MYTOKENS::iterator tok_iter = tok.begin();
	//open the file
	int writeto;
	if((writeto = open((*tok_iter).c_str(), flags, 0666)) == -1)
	{
		perror("open() error");
		return;
	}
	//saving std out 
	int savestdout = dup(1);
	if(savestdout == -1)
	{
		perror("dup() error, output redirection");
		exit(1);
	}
	//setting writeto to the new stdout
	//must restore stdout later
	if(dup2(writeto, 1) == -1)
	{
		perror("dup2() error, output redirection");
		exit(1);
	}
	//closing writeto
	if (close(writeto) == -1)
	{
		perror("close() error, output redirection");
		exit(1);
	}
	//parsing left side for spaces, and calliing execvp on leftside
	char *argv[2];
	int i = 0;
	typedef tokenizer<char_separator<char> > COMTOKEN;
	char_separator<char> COMSEPARATOR (" ");
	COMTOKEN etok(leftofout, COMSEPARATOR);

	for(COMTOKEN::iterator com_iter = etok.begin();
		com_iter != etok.end(); i++, com_iter++)
	{
		argv[i] = new char[(*com_iter).size()];
		strcpy(argv[i],(*com_iter).c_str());
	}
	//only want first command, if argv i != NULL then error is thrown	
	argv[i] = NULL;
	string plzwork = mycmd(argv[0]); 

	if(execv(plzwork.c_str(),argv) == -1)
	{
		perror("error execv()");
		exit(1);
	}
	//restoring stdout
	if(dup2(savestdout, 1) == -1)
	{
		perror("dup2 error restoring");
	}
	//closing savestdout
	if(close(savestdout) == -1)
	{
		perror("colse() error");
	}
	//must delete argv
	int j = 0;
	for(COMTOKEN::iterator del_iter = etok.begin();
	del_iter != etok.end(); j++, del_iter++)
	{
		delete [] argv[j];
	}
}//void outputredir)
//-------------------------------------------------------------
void splitpipes(const string &usercommand)
{
	string leftofpipe = usercommand.substr(0,usercommand.find("|"));
	string rightofpipe = usercommand.substr(usercommand.find("|")+1);
	if(!usercommand.find("|"))
	{
		return;
	}
	if(rightofpipe == "")
	{
		return;
	}
	char *argv1[512];
	int i = 0;
	typedef tokenizer<char_separator<char> > COMTOKEN;
	char_separator<char> COMSEPARATOR (" ");
	COMTOKEN etok(leftofpipe, COMSEPARATOR);

	for(COMTOKEN::iterator com_iter = etok.begin();
		com_iter != etok.end(); i++, com_iter++)
	{
		argv1[i] = new char[(*com_iter).size()];
		strcpy(argv1[i],(*com_iter).c_str());
	}
//+++++++++++++++++++++=	
	char *argv2[512];
	int j = 0;
	typedef tokenizer<char_separator<char> > COMTOKEN2;
	char_separator<char> COMSEPARATOR2 (" ");
	COMTOKEN2 etok2(rightofpipe, COMSEPARATOR2);

	for(COMTOKEN2::iterator com_iter2 = etok2.begin();
		com_iter2 != etok2.end(); j++, com_iter2++)
	{
		argv2[j] = new char[(*com_iter2).size()];
		strcpy(argv2[j],(*com_iter2).c_str());
	}
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	dopipes(argv1, argv2);
	//must delete argv
	int k = 0;
	for(COMTOKEN::iterator del_iter = etok.begin();
		del_iter != etok.end(); k++, del_iter++)
	{
		delete [] argv1[k];
	}
	int l = 0;
	for(COMTOKEN2::iterator del_iter2 = etok2.begin();
		del_iter2 != etok2.end(); l++, del_iter2++)
	{
		delete [] argv2[l];
	}
}//void splitpipes()
//-------------------------------------------------------------
void dopipes(char **argv1, char **argv2)
{
	int fd[2];
	if(pipe(fd) == -1)
	{
		perror("Apipe() error");
	}
	int pid = fork();
	if(pid == -1)
	{
		perror("Bpiping fork() error");
	}
	else if(pid == 0)
	{
		if(close(fd[0]) == -1)
		{
			perror("Dpiping close() error");
		}
		if(dup2(fd[1],1) == -1)
		{
			perror("Cpiping dup2() error");
		}
		if(execvp(argv1[0], argv1));
		{	
			perror("Epiping execvp() error");
			exit(1);
		}
	}
	else
	{
		if(wait(0) == -1)
		{
			perror("Fpiping wait() error");
			exit(1);
		}
		int holdstdin;
		if((holdstdin = dup(0)) == -1)
		{
			perror("Gpiping dup() error");
			exit(1);
		}
		
		
		if(dup2(fd[0],0) == -1)
		{
			perror("Hpiping dup2() error");
			exit(1);
		}

		if(close(fd[1]) == -1)
		{
			perror("Ipiping close() error");
			exit(1);
		}
	}
	string str(argv2[0]);
	splitpipes(str);
}//void dopipes()
//-------------------------------------------------------------
void ctrlc(int signum)
{
	if(signum == SIGINT)
	{
		int pid = getpid();
		if(pid == -1)
		{	
			perror("getpid() error");
			exittheshell();
		}
		if(pid == 0)
		{
			exit(0);
		}
		return;
	}
	cout << "\n";
}//ctrlc()

void exittheshell()
{
	exit(1);
}
