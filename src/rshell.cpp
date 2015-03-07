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

using namespace std;
using namespace boost;

void parse(const string &usercommand, const char specialchar[], const bool &isOR, const bool &isAND);
void inputredirect(const string &usercommand);
void outputappend(const string &usercommand);
void outputtrunc(const string &usercommand);
void splitpipes(const string &usercommand);
void dopipes(char **argv1, char **argv2);

int main()
{	
	//obtaining username and hostname
	char host[64] = {0};
	string displayuser;
	string userinput;
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
	
	displayuser = username + "@" + host + "$ ";
	char SEMICOLON[] = ";";
	char ANDING[] = "&&";
	char ORING[] = "||";
	bool isOR = false;
	bool isAND = false;
	while(1)
	{
		cout << displayuser;
		getline(cin, userinput);

		//checks for comment '#'
		string checkcomment = userinput;
		for(unsigned int i=0; i <= checkcomment.size()-1; i++)
		{
			if(checkcomment[i] == '#')
			{
				checkcomment[i] = '\0';
			}
		}
		userinput = checkcomment;
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
			outputappend(userinput);
		}
		else if(userinput.find(">") != string::npos)
		{
			outputtrunc(userinput);
		}
		else
		{
			isAND = false;
			isOR = false;
			parse(userinput,SEMICOLON, isOR, isAND);
		}
		//exit check
		typedef tokenizer<char_separator<char> > EXITTOKEN;
		char_separator<char> EXITSEPARATOR (" ");
		EXITTOKEN etok(userinput, EXITSEPARATOR);
		for(EXITTOKEN::iterator exit_iter = etok.begin();
			exit_iter != etok.end(); exit_iter++)
		{
			if(*exit_iter == "exit") exit(EXIT_SUCCESS);
		}
	}//while(1)
	return 0;
} // end of main
//-------------------------------------------------------------
void parse(const string &usercommand, const char specialchar[], const bool &isOR, const bool &isAND)
{
	typedef tokenizer<char_separator<char> > MYTOKENS;
	char_separator<char> MYSEPARATOR(specialchar);
	MYTOKENS tok(usercommand, MYSEPARATOR);
	string dothistoken;
	for(MYTOKENS::iterator tok_iter = tok.begin();
		tok_iter != tok.end(); tok_iter++)
	{
		int pid = fork();
		//if pid == -1 throw error
		if(pid == -1)
		{
			perror("error with fork()");
			exit(1);
		}
		//else if pid == 0 were in child
		else if(pid == 0)
		{
			dothistoken = *tok_iter;
			char *argv[512];
			int i = 0;
			typedef tokenizer<char_separator<char> > COMTOKEN;
			char_separator<char> COMSEPARATOR (" ");
			COMTOKEN etok(dothistoken, COMSEPARATOR);

			for(COMTOKEN::iterator com_iter = etok.begin();
				com_iter != etok.end(); i++, com_iter++)
			{
				argv[i] = new char[(*com_iter).size()];
				strcpy(argv[i],(*com_iter).c_str());
			}
			
			argv[i]=0;
			if(execvp(argv[0],argv) == -1)
			{
				perror("error execvp()");
				exit(1);
			}
			//must delete argv
			int j = 0;
			for(COMTOKEN::iterator del_iter = etok.begin();
				del_iter != etok.end(); j++, del_iter++)
				{
					delete [] argv[j];
				}

		}
		//else we are in parent
		else
		{
			int parentstatus = 0;
			if(wait(&parentstatus) == -1)
			{
				perror("error with wait()");
				exit(1);
			}
			//if first or succeed, return
			if((WEXITSTATUS(parentstatus) == 0) && (isOR == true))
			{
				return; 
			}
			//if first and doesnt succeed, return
			if((WEXITSTATUS(parentstatus) != 0) && (isAND == true))
			{
				cerr << "Error in first command\n";
				return; 
			}
		}
	}
}//void parse()
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
	if(execvp(argv[0],argv) == -1)
	{
		perror("error execvp()");
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
void outputappend(const string &usercommand)
{
	string carrot = ">>";
	int flags = O_CREAT | O_WRONLY | O_APPEND;
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
	if(execvp(argv[0],argv) == -1)
	{
		perror("error execvp()");
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
}//void outputappend()
//-------------------------------------------------------------
void outputtrunc(const string &usercommand)
{
	string carrot = ">";
	int flags = O_CREAT | O_WRONLY | O_TRUNC;
	int carrotloc = usercommand.find(carrot);
	string leftofout = usercommand.substr(0, carrotloc);
	string rightofout = usercommand.substr(usercommand.find(carrot) + 1);
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
	char *argv[1];
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
	if(execvp(argv[0],argv) == -1)
	{
		perror("error execvp()");
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
}//void outputtrunc()
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
