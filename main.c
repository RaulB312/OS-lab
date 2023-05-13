#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <sys/sysmacros.h>
#include <fcntl.h>
#include <dirent.h>
#include <regex.h>
#include <sys/wait.h>


char choice[32];
char path[256];

int nrFiles = 0;
int pidCounter = 0;

int ParseFileInDir(DIR *dir) 
{
    int count = 0;
    struct dirent *entry;

    // Loop through all entries in the directory
    while ((entry = readdir(dir)) != NULL) {
        // Check if the entry is a file
        if (entry->d_type == DT_REG) {
            // Check if the file extension is ".c"
            char *ext = strrchr(entry->d_name, '.');
            if (ext != NULL && strcmp(ext, ".c") == 0) {
                count++;
            }
        }
    }
    return count;
}
void regFileMenu(struct stat st, char *path)
{
        
    printf("\n Regular File:");
    printf("%s \n", path);
    printf("Menu for regular file \n");
    printf("1. Read: -n \n");
    printf("2. Size: -d \n");
    printf("3. Number of hard links: -h \n");
    printf("4. Time of last modification: -m \n");
    printf("5. Access rights: -a \n");
    printf("6. Create a symbolic link -l \n" );
                  
           
}
void symFileMenu(struct stat st, char *path)
{
        printf( "\nSymbolic Link:"); 
        printf("%s\n",path);
        printf("Menu for symbolic link \n");
        printf( "1. Link name: -n \n");
        printf("2. Delete link: -l \n");
        printf("3. Size of the link: -d \n");
        printf("4. Size of the target: -t \n");
        printf("5. Access rights: -a \n" );
          
}
void dirFileMenu(DIR *dir,char *path)
{
    printf("\nDirectory:");
    printf("%s\n",path);
    printf("Menu for directory file \n");
    printf("1. Directory name : -n \n");
    printf("2. Size of the directory: -d \n");
    printf("3. Access rights: -a \n");
    printf("4. Total number of files with .c extension: -c \n" );
}
void accessRights(struct stat st)
{
    printf( "\nAccess rights for owner: \n \n");
    if(st.st_mode & S_IRUSR)
        printf("->Read permission for owner\n");
    if(st.st_mode & S_IWUSR)
        printf("->Write permission for owner\n");
    if(st.st_mode & S_IXUSR)
        printf("->Execute permission for owner\n");
    printf("\nAccess rights for group: \n \n");
    if(st.st_mode & S_IRGRP)
        printf("->Read permission for group\n");
    if(st.st_mode & S_IWGRP)
        printf("->Write permission for group\n");
    if(st.st_mode & S_IXGRP)
        printf("->Execute permission for group\n");
    printf("\nAccess rights for others: \n \n");
    if(st.st_mode & S_IROTH)
        printf("->Read permission for others\n");
    if(st.st_mode & S_IWOTH)
        printf("->Write permission for others\n");
    if(st.st_mode & S_IXOTH)
        printf("->Execute permission for others\n");
    
    printf("\n");

    
}
void regularFile(int i, struct stat st, char *fileName)
{
    if ( i!=0  && S_ISREG(st.st_mode))
    {
        switch(choice[i])
        {
            case 'n':
                printf("File name : %s \n",fileName);
                break;
            case 'd':
                printf("Size  %ld bytes\n",st.st_size);
                break;
            case 'h':
                printf( "Number of hard links   %ld\n",st.st_nlink);
                break;
            case 'm':
                printf("Time of last modification %s",ctime(&st.st_atime));
                break;
            case 'a':
                accessRights(st);
                break;
            case 'l':
                printf("Introduce the name of the new link :\n");
                char link[100];
                scanf("%s",link);
                if(symlink(fileName,link) == -1)
                {
                    printf( "Error creating the link" );
                    exit(EXIT_FAILURE);
                }
                printf( "Link created successfully\n" );
                break;
            default:
                printf( "Invalid Operation %c\n",choice[i]);
                break;
        }
    }
}
void linkFile(int i, struct stat st, char *fileName)
{
    if(i!=0 && S_ISLNK(st.st_mode))
    {
        switch(choice[i])
        {
            case 'n':
                printf("Link name : %s \n",fileName);
                break;
            case 'l':
                unlink(fileName);   
                printf("Deleting the link : %s\n",fileName);
                break;
            case 'd':
                printf("Size of the link %ld bytes\n",st.st_size);
                break;
            case 't':
                struct stat stNew;
                stat(fileName, &stNew);
                printf("Size of the target %ld\n",stNew.st_size);
                break;
            case 'a':
                accessRights(st);
                break;
            default:
                printf( "Invalid Operation %c\n",choice[i]); 
                break;
        }
    }
}
void directoryFile(int i,struct stat st,char *fileName, DIR *dir)
{
    int nrCFiles=0;
     if(i!= 0 && S_ISDIR(st.st_mode))
    {
        switch (choice[i])
        {
            case 'n':
                printf("Directory name : %s \n",fileName);
                break;
            case 'd':
                struct stat stNew2;
                stat(fileName,&stNew2);
                printf("Size of the directory: %ld bytes\n",stNew2.st_size);
                break;
            case 'a':
                accessRights(st);
                break;
            case 'c':
                strcat(path,"/");
                nrCFiles = ParseFileInDir(dir);
                printf("Total number of files with .c extension : %d \n",nrCFiles);
                break;
            default:
                printf( "Invalid option %c\n",choice[i]);
                break;
        }
    }
}
void wrongOption(char *validCommands,char *choice)
{
     int i = 0;
     //Check if the first character is a dash
     if(choice[0] !='-')
        {
            printf("\nAdd \"-\" sign\n");
            return;
        }
        //Check if the option is valid
    while(choice[i]!='\0')
        {
            if(strchr(validCommands,choice[i]) == NULL)
            {
                printf( "Invalid option: %c \n",choice[i]);
            }
            i++;
        }
        printf("\n");
}
double getScore(int nrErrors, int nrWarnings)
{
    if(nrErrors == 0 && nrWarnings == 0)
    {
        return 10;
    }
    else if(nrErrors >= 1)
    {
        return 1;
    }
    else if(nrErrors == 0 && nrWarnings > 10)
    {
        return 2;
    }
    else
    {
        return (2 + 8 * (10 -nrWarnings) / 10);
    }
}
void menuFunction(struct stat st,char *path)
{
   
   DIR *dir;
   regex_t extension,extensionC;
   char *validCommands;
   int fd[2];
   int isCFile = 0;
    if (S_ISREG(st.st_mode))
    {
        regFileMenu(st,path);
        printf("\n");
        printf("Enter your choice:" );
        scanf("%s",choice);
        // Clear the screen for a better user experience 
        system("clear");
        printf("Your choice %s  \n",choice);
        if(regcomp(&extensionC,".c$",REG_EXTENDED !=0))
        {
            printf("Error compiling .c regular expression \n");
            exit(EXIT_FAILURE);
        }
        //Regex to find the regular files that have .c extension
        if(regexec(&extensionC,path, 0, NULL, 0) == 0)
        {
            if(pipe(fd)<0)
            {
                printf("Pipe creation failed \n");
                exit(1);
            }
            //Create a child process
            pid_t cpid = fork();
            //Check if the fork function was called successfully
            if(cpid == -1)
            {
                perror( "Fork failure \n" );
                exit(EXIT_FAILURE);
            }
            //Increase the counter of children
            pidCounter++;
            //Flag for checking if the file is a C file and to verify in the parent process
            isCFile = 1;
            if(cpid == 0)
            {
                //close the reading end of the pipe
                close(fd[0]);
                //irect the information from STDOUT to pipe
                int newfd = dup2(fd[1],1);
                // Check if the dup2 call is succesful
                if(newfd < 0)
                {
                    printf("Error dup2 \n");
                    exit(1);
                }
                //Call of  the script which write in the fileout.txt the errors and warnings from path
                execlp("bash","bash","script.sh",path,"fileout.txt",NULL);
                close(fd[1]);
            }
        }
           
        //Assign the valid commands for the regular file
        validCommands = "-ndhmal";

        //Compile the regular expression for the regular file
        if(regcomp(&extension,"^-[ndhmal]+$",REG_EXTENDED) != 0)
        {
            printf( "Error compiling the regular expression \n" );
            exit(EXIT_FAILURE);
        } 
    }
    if(S_ISLNK(st.st_mode))
    {
        symFileMenu(st,path);
        printf("Enter your choice:" );
        scanf("%s",choice);
        system("clear");
        printf("Your choice %s  \n",choice);
        pid_t cpidlink = fork();
        if(cpidlink == -1)
        {
            printf( "Fork failure \n" );
            exit(EXIT_FAILURE);
        }

        pidCounter++;
        if(cpidlink == 0)
        {
            // 2nd child process 
            //Change permision for link file
            // Set permissions for user, group, and others
            mode_t permissions = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP;

            if (chmod(path, permissions) != 0)
            {
                printf("Failed to change permissions of '%s'\n", path);
                exit(EXIT_FAILURE);
            }  
        }

        //Assign the valid commands for the link
        validCommands = "-nldta";
        //Compile the regular expression for the link
        if(regcomp(&extension,"^-[nldta]+$",REG_EXTENDED) != 0)
        {
            printf( "Error compiling the regular expression" );
            exit(EXIT_FAILURE);
        }
    }

    if(S_ISDIR(st.st_mode))
    {
        dir = opendir(path);
        if(dir == NULL)
        {
            printf( "Error could not open the directory" );
        }
        dirFileMenu(dir,path);
        printf("\n");
        printf("Enter your choice:" );
        scanf("%s",choice);
        system("clear");
        printf("Your choice %s  \n",choice);
        pid_t cpidDir = fork();
        if(cpidDir == -1)
        {
            printf( "Fork failure \n" );
            exit(EXIT_FAILURE);
        }
        //Obtain the name of the new file dirName_file.txt
        char newFileName[255]="";
        strcpy(newFileName,path);
        strcat(newFileName,"_file.txt");
        pidCounter++;
        if(cpidDir == 0)
        {
            // 2nd child process 
            //Call of  the script which create the file with the given name(newFileName)
            execlp("bash","bash","scriptDirFile.sh",newFileName,NULL);   
        }
        //Assign the valid commands for the directory
        validCommands = "-ndac";
        //Compile the regular expression for the directory
        if(regcomp(&extension,"^-[ndac]+$",REG_EXTENDED) != 0)
        {
            printf( "Error compiling the regular expression\n" );
            exit(EXIT_FAILURE);
        }
    }

    //2nc child process to handle the options
    pid_t pid = fork();
    if(pid == -1)
    {
        printf( "Fork error \n" );
        exit(EXIT_FAILURE);
    }
    //Increase the counter of children
    pidCounter++;
    if(pid == 0)
    {
        // Check if the choice is valid
        if(regexec(&extension, choice, 0, NULL, 0) != 0)
        {
            // Call the function that will print the invalid options
            wrongOption(validCommands,choice);
            menuFunction(st,path);
            strcpy(choice,"");
        }
        int i = 0;
        while(choice[i] != '\0') 
        {
                //Execute the functions that have been chosen
                regularFile(i,st,path);
                linkFile(i,st,path);
                directoryFile(i,st,path,dir);
                i++;
        }
        exit(1);
    }
    else
    {   
        //If it is a C file
        if(isCFile == 1)
        {
            //Close the writing end of the pipe
            close(fd[1]);
            //Buffer for the data from the reading end of the pipe
            char buff[512],*ptr;
            //Number of errors and warnings to be extracted from the buffer
            int nrErrors,nrWarnings;
            //Assign the number of readed bytes
            int nrBytesRead = read(fd[0],buff,512);
            if(nrBytesRead > 0)
            {
                //Termiante the string at the number of read bytes
                buff[nrBytesRead]='\0'; 
                //Extract the numbers for erros and warnings in tenth base
                nrErrors = strtol(buff, &ptr, 10);
                nrWarnings = strtol(ptr, NULL, 10);

                //printf("Errors -> %d Warnings -> %d \n",nrWarnings,nrErrors);
                //printf("Score: %f \n",getScore(nrWarnings,nrErrors));
                FILE* fp = fopen("fileout.txt", "w"); // Open file in write mode
                if (fp == NULL) 
                {
                    fprintf(stderr, "Failed to open file 'fileout.txt'\n");
                    return;
                }
                fprintf(fp, "<%s>: <%f>\n", path, getScore(nrWarnings,nrErrors));
                fclose(fp); // Close the file
            }
            else if (nrBytesRead == 0)
            {
                printf("Pipe closed by the child process\n");
            }
            else
            {
                perror("Error reading from the pipe");
                exit(EXIT_FAILURE);
            }
            //Close the reading end of the pipe
            close(fd[0]);
        }
        else
        {
            close(fd[1]);
            FILE* fp = fopen(path, "r");
            int count = 0;
            char line[1024];

            while (fgets(line, sizeof(line), fp)) // Read file line by line
                count++;

            fclose(fp);
            printf("Number of lines in the non .c file: %d\n", count);
            close(fd[0]);
        }
        //Wait for all childs
        for(int k = 0; k <pidCounter;k++)
        {
            
            int status;
            pid_t wait_status;
            wait_status = wait(&status);
            //Check the return value of wait function
            if (wait_status == -1)
            {
                exit(EXIT_FAILURE);
            }
            if (WIFEXITED(status)) 
            {
                printf("The process with PID <%d> has ended with the exit code <%d>\n",wait_status, WEXITSTATUS(status));
            } 
        }
        // Reset number of childs for each argument 
        pidCounter=0;
    }
}

int main(int argc, char *argv[])
{
    struct stat st;
    if (argc < 2) 
    {
        fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    for(int i=1;i<argc;i++)
    {
        // Check if the path is a valid file
        if (lstat(argv[i], &st) == -1) 
        {
            printf( "File doesn't exist \n" );
            exit(EXIT_FAILURE);
        }
        // Copy the path to the path variable
        strcpy(path,argv[i]);
         
        // Call the menu function
    
        menuFunction(st,path);
    }
}