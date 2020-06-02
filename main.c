
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include<time.h>

#define Max_letters 1000 // max number of chars allowed
#define Max_commands 100 // max commands allowed

// Clearing the shell using escape sequences
#define clear() printf("\033[H\033[J")

// Time telling function
void telltime()
{
    time_t rawtime;
      struct tm * timeinfo;

      time ( &rawtime );
      timeinfo = localtime ( &rawtime );
      printf ( "\n Currently it is  %s", asctime (timeinfo) );
}
// Greeter
void shell_initializer()
{
    clear();

     printf("      ███████╗██╗██████╗░███████╗░██████╗██╗░░██╗███████╗██╗░░░░░██╗░░░░░\n");
     printf("      ██╔════╝██║██╔══██╗██╔════╝██╔════╝██║░░██║██╔════╝██║░░░░░██║░░░░░\n");
     printf("     █████╗░░██║██████╔╝█████╗░░╚█████╗░███████║█████╗░░██║░░░░░██║░░░░░\n");
     printf("      ██╔══╝░░██║██╔══██╗██╔══╝░░░╚═══██╗██╔══██║██╔══╝░░██║░░░░░██║░░░░░\n");
     printf("     ██║░░░░░██║██║░░██║███████╗██████╔╝██║░░██║███████╗███████╗███████╗\n");
     printf("      ╚═╝░░░░░╚═╝╚═╝░░╚═╝╚══════╝╚═════╝░╚═╝░░╚═╝╚══════╝╚══════╝╚══════╝\n");

    char* username = getenv("USER");
    printf("\n\n\nWelcome user @%s .Feel at home.", username);
   telltime();
    printf("\n");
    sleep(5);
    clear();
}

// Prcocessing Input
int input_present(char* str)
{
    char* buffer;
    buffer = readline("\n>>> ");
    if (strlen(buffer) != 0) {
        add_history(buffer);
        strcpy(str, buffer);
        return 0;
    } else {
        return 1;
    }
}

// To tell the current directory
void dir_title()
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("\nPresently at the directory: %s", cwd);
}

// To execute system commands
void simple_execute(char** parsed)
{
    // Forking a child
    pid_t pid = fork();

    if (pid == -1) {
        printf("\nA child process could not be forked");
        return;
    } else if (pid == 0) {
        if (execvp(parsed[0], parsed) < 0) {
            printf("\nSorry, that could not be executed.");
        }
        exit(0);
    } else {
        // waiting for child to terminate
        wait(NULL);
        return;
    }
}

// To execute commands that were piped
void piped_execute(char** parsed1, char** parsed2)
{
    // 0 is read end, 1 is write end
    int pipefd[2];
    pid_t p1, p2;

    if (pipe(pipefd) < 0) {
        printf("\nInitialization of Pipe failed\n");
        return;
    }
    p1 = fork();
    if (p1 < 0) {
        printf("\nA child process could not be forked, (Command 1)\n");
        return;
    }

    if (p1 == 0) {
        // Child 1 executing..
        // It only needs to write at the write end
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        if (execvp(parsed1[0], parsed1) < 0) {
            printf("\nSorry, that could not be executed.(Command 1)\n");
            exit(0);
        }
    } else {
        // Parent executing
        p2 = fork();

        if (p2 < 0) {
            printf("\nA child process could not be forked,(Command 2)\n");
            return;
        }

        // Child 2 executing..
        // It only needs to read at the read end
        if (p2 == 0) {
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            if (execvp(parsed2[0], parsed2) < 0) {
                printf("\nSorry, that could not be executed.(Command 2)\n");
                exit(0);
            }
        } else {
            // parent executing, waiting for two children
            wait(NULL);
            wait(NULL);
        }
    }
}

// Help command builtin
void Help()
{   printf("\n\n\n");
    printf("      ███████╗██╗██████╗░███████╗░██████╗██╗░░██╗███████╗██╗░░░░░██╗░░░░░\n");
    printf("      ██╔════╝██║██╔══██╗██╔════╝██╔════╝██║░░██║██╔════╝██║░░░░░██║░░░░░\n");
    printf("     █████╗░░██║██████╔╝█████╗░░╚█████╗░███████║█████╗░░██║░░░░░██║░░░░░\n");
    printf("      ██╔══╝░░██║██╔══██╗██╔══╝░░░╚═══██╗██╔══██║██╔══╝░░██║░░░░░██║░░░░░\n");
    printf("     ██║░░░░░██║██║░░██║███████╗██████╔╝██║░░██║███████╗███████╗███████╗\n");
    printf("      ╚═╝░░░░░╚═╝╚═╝░░╚═╝╚══════╝╚═════╝░╚═╝░░╚═╝╚══════╝╚══════╝╚══════╝\n");
    printf("\n\n");
    printf("************************************************************************************\n");
   printf("\n Made by Anubhav Sharma");
    printf("\nShell has been checked thoroughly, but responsibilty of any losses due to its use to be borne by the user \n");
    printf(  "\nThe general commands used in UNIX are well supported\n");
    printf("\nPipe Handling is supported\n");
   printf("\nVarious New Commands are supported\n");
    return;
}

// Function to execute builtin commands
int Built_in_cmds(char** parsed1)
{
    int cmd_no = 5, i, req_cmdno = 0;
    char* commands[cmd_no];
    char* username;

    commands[0] = "exit";
    commands[1] = "cd";
    commands[2] = "help";
    commands[3] = "average";
    commands[4] ="hello";
    for (i = 0; i < cmd_no; i++) {
        if (strcmp(parsed1[0], commands[i]) == 0) {
            req_cmdno = i + 1;
            break;
        }
    }

    switch (req_cmdno) {
    case 1:
        username = getenv("USER");
        printf("\nHave a nice day %s. Nice meeting you.\n",username);
        exit(0);
    case 2:
        chdir(parsed1[1]);
        return 1;
    case 3:
        Help();
        return 1;
    case 4:
     printf("\n Specify a number, which is not in your database, to quit the process(-13 if you are unsure)\n");
        double quit;
        scanf("%lf",&quit);
        double sum=0.0;
        printf("Current average = %lf from 0 numbers. Type the next number\n..Type %lf if you want to quit\n.",sum,quit);
        double curr;
        scanf("%lf",&curr);
        long long int ti=1;
         double ave=0;
         double n=quit+1;
         if (curr==quit)
             n=quit;
        while (n!=quit)
        {
            ave=(curr/ti);

            printf("Current average = %lf from %lld numbers. Type the next number.Type %lf if you want to quit\n",ave,ti,quit);
            ti++;

            scanf("%lf",&n);
            curr=curr+n;
        }
        printf("Final average %lf, from %lld numbers",ave,(ti-1));
      return 1;
    case 5:
        username = getenv("USER");
        printf("\nHello %s.\n We know you are good with "
            "tricks and magic. If you are still lost"
            "\nType in help and press Enter..\n",
            username);
        return 1;
    default:
        break;
    }

    return 0;
}

// function for finding pipe
int parsePipe(char* str, char** piped_commands)
{
    int i;
    for (i = 0; i < 2; i++) {
        piped_commands[i] = strsep(&str, "|");
        if (piped_commands[i] == NULL)
            break;
    }

    if (piped_commands[1] == NULL)
        return 0; // returns zero if no pipe is found.
    else {
        return 1;
    }
}

// function for parsing command words
void parseSpace(char* str, char** parsed1)
{
    int i;

    for (i = 0; i < Max_commands; i++) {
        parsed1[i] = strsep(&str, " ");

        if (parsed1[i] == NULL)
            break;
        if (strlen(parsed1[i]) == 0)
            i--;
    }
}

int line_processor(char* str, char** parsed1, char** parsed2)
{

    char* piped_commands[2];
    int is_piped = 0;

    is_piped = parsePipe(str, piped_commands);

    if (is_piped) {
        parseSpace(piped_commands[0], parsed1);
        parseSpace(piped_commands[1], parsed2);

    } else {

        parseSpace(str, parsed1);
    }

    if (Built_in_cmds(parsed1))
        return 0;
    else
        return 1 + is_piped;
}

int main()
{
    char command_line[Max_letters], *parsedinput[Max_commands];
    char* parsedinput2[Max_commands];
    int exec_indicator = 0;
    shell_initializer();

    while (1) {
        // print shell line
        dir_title();
        // take input
        if (input_present(command_line))
            continue;
        // process
        exec_indicator = line_processor(command_line,
        parsedinput, parsedinput2);
        // indicator has 0-no command,1-simple command ,2- piped command

        // execute
        if (exec_indicator == 1)
            simple_execute(parsedinput);

        if (exec_indicator == 2)
            piped_execute(parsedinput, parsedinput2);
    }
    return 0;
}
