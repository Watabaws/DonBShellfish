#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>


int find_char(char * full_string, char to_find){
    char * cur_loc = full_string;
    int count = 0;
    while(cur_loc){
        cur_loc = strchr(cur_loc+1, to_find);
        count++;
    }

    return count;
}

int find_string(char ** full_string, char * to_find){
    char ** cur_str = full_string;
    int ind = 0;
    while(cur_str[ind]){
        //printf("cur_str[%d]: %s\n", ind, cur_str[ind]);
        if(!strcmp(cur_str[ind], to_find)){
          return ind;
        }
        ind++;
    }

    return -1;
}


char ** parse_args( char * line, int numseps, char * lf ){
	int ctr = numseps + 1;
	int i;
	char ** args = (char **)calloc(ctr, sizeof(char *));

	for(i = 0; line; i++){
	  args[i] = strsep(&line, lf);
	}

	args[i] = NULL; // null terminate array

	return args;

}

void redirect_out(char ** exec, int greater_than, int append){
    int new_out;
    if(append){
        new_out = open(exec[greater_than + 1], O_CREAT | O_WRONLY | O_APPEND, 755);
    }
    else{
        new_out = open(exec[greater_than + 1], O_CREAT | O_WRONLY, 755);
    }
    if(!fork()){
        dup2(new_out, STDOUT_FILENO);
        exec[greater_than] = 0;
        execvp(exec[0], exec);
    }
}

void redirect_in(char ** exec, int less_than){
    int new_in = open(exec[less_than + 1], O_CREAT | O_RDONLY, 755);
    if(!fork()){
        dup2(new_in, STDIN_FILENO);
        exec[less_than] = 0;
        execvp(exec[0], exec);
    }
}

void pipe_it(char ** exec, int pipe_loc){
    int status;
    int pipin_hot[2];
    pipe(pipin_hot);

    if(!fork()){
        close(pipin_hot[0]);
        dup2(pipin_hot[1], STDOUT_FILENO);
        exec[pipe_loc] = 0;
        execvp(exec[0], exec);
    }

    if(!fork()){
        close(pipin_hot[1]);
        dup2(pipin_hot[0], STDIN_FILENO);
        char ** sec_exec = exec + pipe_loc + 1;
        //printf("Where we at: %s\n", sec_exec[0]);
        execvp(sec_exec[0], sec_exec);
    }

    //printf("%s\n", strerror(errno));
}

int main(){
    char commands[256];
    printf("Command? \n");
    fgets(commands, 256, stdin);
    printf("Thank you!\n");

    //Strip the newline
    char *p = strchr(commands, '\n');
    if (p) {
      *p = 0;
    }

    //printf("How many commands was that?\n");
    int num_commands = find_char(commands, ';');
    //printf("Found %d commands\n", num_commands);

    char ** to_run = parse_args(commands, num_commands, ";");

    //Strip the spaces
    /*int i;
    for(i = 0; i < num_commands; i++){
        if(to_run[i][0] == ' '){
            to_run[i][0]++;
        }
        int len_com;
        for(len_com = 0; to_run[i][len_com] ; len_com++){}
        if(to_run[i][len_com] == ' '){
            to_run[i][len_com] = 0;
        }

    }*/

    //printf("First command: %s\n", to_run[0]);
    //printf("Second command: %s\n", to_run[1]);

    int cmd_count = 0;

    while(to_run[cmd_count]){
        int num_args = find_char(to_run[cmd_count],  ' ');
        //printf("We've got %d arguments\n", num_args);
        char ** exec = (char **)calloc(num_args, sizeof(char *));
        //printf("tryna: %s\n", to_run[cmd_count]);
        exec = parse_args(to_run[cmd_count], num_args, " ");
        exec[num_args] = NULL;

        //exec++;
        int append = 0;
        int less_than = find_string(exec, "<");
        //printf("<: %d\n", less_than);
        int greater_than = find_string(exec, ">");
        if(greater_than == -1){
            greater_than = find_string(exec, ">>");
            append = 1;
        }
        int pipe_loc = find_string(exec, "|");
        //printf("|: %d\n", pipe_loc);
        //printf("huh\n");

        if(less_than == -1 && greater_than == -1 && pipe_loc == -1){
          if(!fork()){
              //printf("Filename: %s\n", exec[0]);
              if(strcmp(exec[0], "cd")){
                if(strcmp(exec[0], "exit")){
                  execvp(exec[0], exec);
                }
                else{
                  return 2;
                }
              }
              else{
                //printf("issa cd\n");
                return 3;
              }
              //printf("Ruhroh: %s\n", strerror(errno));
          }
        }
        else if(!fork()){
          if(less_than != -1){
            redirect_in(exec, less_than);
          }

          if(greater_than != -1){
            redirect_out(exec, greater_than, append);
          }

          if(pipe_loc != -1){
              //printf("piping stuff\n");
            pipe_it(exec, pipe_loc);
            return 0;
          }
        }

        int status;
        int speaking_child = wait(&status);
        //printf("%d\n", WEXITSTATUS(status));
        if(WEXITSTATUS(status) == 3){
          //printf("CDing to: %s\n", exec[1]);
          chdir(exec[1]);
          char cwd[256];
          getcwd(cwd, 256);
          printf("Current directory: %s \n", cwd);
        }
        else if(WEXITSTATUS(status) == 2){
          exit(0);
        }

        cmd_count++;
        printf("\n");

    }


}
