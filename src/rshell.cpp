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

void parse(const string &usercommand, const char specialchar[]);
void pipes(const string &usercommand);
void inputredirect(const string &usercommand);
void outputredirect(const string &usercommand, bool istwobrackets);

int main()
{	
	//obtaining username and hostname
	char host[50] = {0};
	string displayuser;
	string userinput;

	string username = getlogin();
	if(getlogin() == NULL)
	{
		perror("getlogin() error.");
	}

	if(gethostname(host,sizeof(host)-1) == -1)
	{
		perror("gethostname() error.");
	}

	for(int i = 0; i < 50; i++)
	{
		if(host[i] == '.') host[i] = '\0';
	}
	
	displayuser = username + "@" + host + "$ ";
	char SEMICOLON[] = ";";
	char ANDING[] = "&&";
	char ORING[] = "||";

	while(1)
	{
		cout << displayuser;
		getline(cin, userinput);

		//checks for comment '#'
		char* checkcomment = new char[userinput.length()+1];
		strcpy(checkcomment, userinput.c_str());
		for(unsigned int i=0; i <= strlen(checkcomment); i++)
		{
			if(checkcomment[i] == '#')
			{
				checkcomment[i] = '\0';
			}
		}
		userinput = checkcomment;
		delete checkcomment;

		//check for special characters ; || &&
		if(userinput.find("&&") != string::npos)
		{
			parse(userinput,ANDING);
		}
		else if(userinput.find("||")!= string::npos)
		{
			parse(userinput,ORING);
		}
		//cheeck for piping and io redirection
		else if(userinput.find("|") != string::npos)
		{
			pipes(userinput);
		}
/*		else if(userinput.find("<") != string::npos)
		{
			inputredirect(userinput);
		}
		else if(userinput.find(">") != string::npos)
		{
			outputredirect(userinput, false);
		}
		else if(userinput.find(">>") != string::npos)
		{
			outputredirect(userinput, true);
		}
*/
		else
		{
			parse(userinput,SEMICOLON);
		}


		//exit check
		typedef tokenizer<char_separator<char> > EXITTOKEN;
		char_separator<char> EXITSEPARATOR (" ");
		EXITTOKEN etok(userinput, EXITSEPARATOR);
		for(EXITTOKEN::iterator exit_iter = etok.begin();
			exit_iter != etok.end(); ++exit_iter)
		{
			if(*exit_iter == "exit") exit(EXIT_SUCCESS);
		}

	}//while(1)
	return 0;
} // end of main
//-------------------------------------------------------------
void parse(const string &usercommand, const char specialchar[])
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
			exit(1);
		}
		//else if pid == 0 were in child
		else if(pid == 0)
		{
			string dothistoken = *tok_iter;
			char *argv[512];
			int i = 0;
			typedef tokenizer<char_separator<char> > COMTOKEN;
			char_separator<char> COMSEPARATOR (" ");
			COMTOKEN etok(dothistoken, COMSEPARATOR);

			for(COMTOKEN::iterator com_iter = etok.begin();
				com_iter != etok.end(); ++i, ++com_iter)
			{
				argv[i] = new char[(*com_iter).size()];
				strcpy(argv[i],(*com_iter).c_str());
			}
			
			argv[i] = 0;
			if(execvp(argv[0],argv) == -1)
			{
				perror("error execvp()");
				exit(1);
			}
			//must delete argv
			int j = 0;
			for(COMTOKEN::iterator del_iter = etok.begin();
				del_iter != etok.end(); ++j, ++del_iter)
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
		}
		
	}
}//void parse()
//-------------------------------------------------------------
void inputredirect(const string &usercommand)
{

}//void inputredirect()
//-------------------------------------------------------------
void outputredirect(const string &usercommand, bool istwobrackets)
{

}//void outputredirect()
//-------------------------------------------------------------
void pipes(const string &usercommand)
{
	/*
	int pipefd[2];
	int pipefd2[2];
	string leftofpipe = usercommand.substr(0,usercommand.find("|"));
	string rightofpipe = usercommand.substr(input.find("|")+1);
	if(pipe(pipefd) == -1)
	{
		perror("pipe() error");
		exit(1);
	}
	int pid = fork();
	if(pid == -1)
	{
		perror("piping fork() error");
		exit(1);
	}
	else if(pid == 0)
	{

	}
	
	string leftofpipe = usercommand.substr(0,usercommand.find("|"));
	string rightofpipe = usercommand.substr(usercommand.find("|")+1);
	char *pipeinput[512];
	strcpy(pipeinput, leftofpipe.c_str());
	int fd[2];
	
	if(pipe(fd) == -1)
	{
		perror("pipe() error");
		exit(1);
	}
	int pid = fork();
	if (pid == -1)
	{
		perror("piping fork() error");
		exit(1);
	}
	
	else if(pid == 0)
	{
		//in child, write to pipe
		if(dup2(fd[1], 1) == -1)
		{
			perror("dup2() error");
			exit(1);
		}
		if(close(fd[0]) == -1)
		{
			perror("close() error");
			exit(1);
		}
		if(execvp(pipeinput[0], pipeinput) == -1)
		{
			perror("piping execvp() error");
			exit(1);
		}
	}
	
	//now read in from pipe
	int holdstdin;
	//changing stdin
	if((holdstdin = dup(0)) == -1)
	{
		perror("piping dup() error");
		exit(1);
	}
	if(dup2(fd[0],0) == -1)
	{
		perror("piping dup2() error");
		exit(1);
	}
	if(close(fd[1]) == -1)
	{
		perror("piping close() error");
		exit(1);
	}
	if(wait(0) == -1)
	{
		perror("piping wait() error");
		exit(1);
	}
	//must restore stdin
	dup2 (holdstdin,0);
	*/
}//void pipes()
