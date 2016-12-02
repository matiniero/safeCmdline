#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/wait.h> 
#include <sys/types.h>
#include <readline/readline.h>
#include <readline/history.h>

#define COMMAND_LENGTH 256
#define MAXLINE 512

#define CONFIG_FILE "clConf"

int timelimit=3;


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  run_safe
 *  Description:  run command with time check
 * =====================================================================================
 */
	void	
run_safe ( char *cmd, int *retValue )
{
	int retStat=-1;
	int cnt=0;

	char* prikaz = strtok(cmd," ");

	char* param[512];
	int i=1;
	param[0]=prikaz;
	while(param[i]=strtok(NULL," ")){
		printf("%s\n",param[i]);
		i++;
	}
	
	param[i]=NULL;//za poslednym parametrom ma byt NULL
            
	pid_t pid=fork();

	switch(pid){
	case 0: retStat=execvp(cmd,param);
		exit(retStat);
		break;
		
	case -1: puts("\nchyba!!!");
		break;
	default: while(1){  
			pid_t chPid = waitpid(pid, &retStat, WNOHANG); //chytam status ktory sa vrati, vdaka WNOHANG sa nebude blokovat beh programu
                 
			if((chPid==pid) && (WIFEXITED(retStat))){
				break;
			}
			if(cnt==timelimit){
				printf("\n\nTimelimit...(%d s)\n",timelimit);
				kill(pid,SIGINT);
				break;
			}
			sleep(1);
			cnt++;
		}
		printf("Proces skoncil po %d sekundach s hodnotou %d\n",cnt,retStat);
		break;
	}
	*retValue=retStat;
}		/* -----  end of function run_safe  ----- */



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  nacita vstup a spusti ho bezbecne
 * =====================================================================================
 */
int main(int argc, char* argv[]){

	FILE *config=fopen(CONFIG_FILE,"r");

	char confBuf[MAXLINE];

	if(config!=NULL){
		while(fgets(confBuf,MAXLINE,config)!=NULL){	
			char *key=strtok(confBuf,"=");
			char *value=strtok(NULL,"=");
			if(strcmp(key,"TIMELIMIT")==0)
				timelimit=atoi(value);
		}
		fclose(config);
	}

	char *buf;
	int ret;
 
	rl_bind_key('\t',rl_abort);//disable auto-complete
    
	while((buf = readline("\n >> "))!=NULL)
	{
		if (strcmp(buf,"quit")==0)
			break;

		FILE *config=fopen("./clConf","r");

        	char confBuf[MAXLINE];

        	if(config!=NULL){
                	while(fgets(confBuf,MAXLINE,config)!=NULL){
                       		char *key=strtok(confBuf,"=");
                        	char *value=strtok(NULL,"=");
                        	if(strcmp(key,"TIMELIMIT")==0)
                                	timelimit=atoi(value);
                	}
                	fclose(config);
        	}
		
	    	if (buf[0]!=0)
                	add_history(buf);
 
	    	if (strcmp(buf,"retval")==0){
               		printf("%d\n",ret); 
	    	}else{
	    		run_safe(buf, &ret);
	    	}
	}
    
	free(buf);
    
	return EXIT_SUCCESS;
} 
