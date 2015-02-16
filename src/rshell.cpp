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
#include <boost/tokenizer.hpp>
//;) test for commit to redirect branch
//3rd test
//ANOITHER TEST FOR COMMIT
using namespace std;
using namespace boost;

void parse(const string &usercommand, const char specialchar[]);

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
			char *argv[100];
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
