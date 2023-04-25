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



char choice[32];
char path[256];

int ParseFileInDir(DIR *dir)
{
  int counter = 0;
  struct dirent *entry;
  struct stat file;
  regex_t extension;
    if(regcomp(&extension, ".c$", REG_EXTENDED))
    {
        printf("Error compiling the regular expression\n");
        exit(EXIT_FAILURE);
    }
  while ((entry = readdir(dir))!=NULL)
  {
    char *name=entry->d_name;
    char  auxpath[4096];
    if(strlen(name) > 2)
    {
        
        strcpy(auxpath,path);
        strcat(auxpath,name);
        if(lstat(auxpath,&file) == -1)
        {
            printf("Error could not get the file stats");
            exit(EXIT_FAILURE);
        }
        if (S_ISREG(file.st_mode) && regexec(&extension, entry->d_name, 0, NULL, 0) == 0)
            counter++;
    }
  }
   closedir(dir);   
   return counter;  
}
void regFileMenu(struct stat sb, char *path)
{
        
    printf("Regular File: %s \n", path);
    printf("1. Read: -n \n");
    printf("2. Size: -d \n");
    printf("3. Number of hard links: -h \n");
    printf("4. Time of last modification: -m \n");
    printf("5. Access rights: -a \n");
    printf("6. Create a symbolic link give:link name: -l \n");
}
void symFileMenu(struct stat sb, char *path)
{
        printf("Symbolic Link: %s \n", path);
        printf("1. Link name: -n \n");
        printf("2. Delete link: -l \n");
        printf("3. Size of the link: -d \n");
        printf("4. Size of the target: -t \n");
        printf("5. Access rights: -a \n");    
}
void dirFileMenu(DIR *dir,char *path)
{
    printf("Directory: %s \n",path);
    printf("1. Directory name : -n \n");
    printf("2. Size of the directory: -d \n");
    printf("3. Access rights: -a \n");
    printf("4. Total number of files with .c extension: -c \n");
}
void accessRights(struct stat sb)
{
    printf("Access rights for owner: \n \n");
    if(sb.st_mode & S_IRUSR)
        printf("Read permission for owner\n");
    if(sb.st_mode & S_IWUSR)
        printf("Write permission for owner\n");
    if(sb.st_mode & S_IXUSR)
        printf("Execute permission for owner\n");
    printf("\nAccess rights for group: \n \n");
    if(sb.st_mode & S_IRGRP)
        printf("Read permission for group\n");
    if(sb.st_mode & S_IWGRP)
        printf("Write permission for group\n");
    if(sb.st_mode & S_IXGRP)
        printf("Execute permission for group\n");
    printf("\nAccess rights for others: \n \n");
    if(sb.st_mode & S_IROTH)
        printf("Read permission for others\n");
    if(sb.st_mode & S_IWOTH)
        printf("Write permission for others\n");
    if(sb.st_mode & S_IXOTH)
        printf("Execute permission for others\n");
    printf("\n");
}
void regularFile(int i, struct stat st, char *fileName)
{
    if ( i!=0  && S_ISREG(st.st_mode))
    {
        switch(tolower(choice[i]))
        {
            case 'n':
                printf("File name : %s \n",fileName);
                break;
            case 'd':
                printf("Size  %ld bytes\n",st.st_size);
                break;
            case 'h':
                printf("Number of hard links   %ld\n",st.st_nlink);
                break;
            case 'm':
                printf("Time of last modification %s",ctime(&st.st_atime));
                break;
            case 'a':
                accessRights(st);
                break;
            case 'l':
                printf("Create a symbolic link; give:link name -l Introduce the name of the new link\n");
                char link[100];
                scanf("%s",link);
                if(symlink(fileName,link) == -1)
                {
                    printf("Error creating the link");
                    exit(EXIT_FAILURE);
                }
                printf("Link created successfully\n");
                break;
            default:
                printf("Invalid Operation %c\n",choice[i]);
                break;
        }
    }
}
void linkFile(int i, struct stat sb, char *fileName)
{
    if(i!=0 && S_ISLNK(sb.st_mode))
    {
        switch(tolower(choice[i]))
        {
            case 'n':
                printf("Link name : %s \n",fileName);
                break;
            case 'l':
                unlink(fileName);   
                printf("Deleting the link : %s\n",fileName);    
                break;
            case 'd':
                printf("Size of the link %ld \n",sb.st_size);
                break;
            case 't':
                struct stat sbNew;
                stat(fileName, &sbNew);
                printf("Size of the target %ld\n",sbNew.st_size);
                break;
            case 'a':
                accessRights(sb);
                break;
            default:
                printf("Invalid Operation %c\n",choice[i]); 
                break;
        }
    }
}
void directoryFile(int i,struct stat sb,char *fileName, DIR *dir)
{
    int nrCFiles=0;
     if(i!= 0 && S_ISDIR(sb.st_mode))
        {
            switch (tolower(choice[i]))
            {
            case 'n':
                printf("Directory name : %s \n",fileName);
                break;
            case 'd':
                struct stat sbNew2;
                stat(fileName,&sbNew2);
                printf("Size of the directory: %ld \n",sbNew2.st_size);
                break;
            case 'a':
               accessRights(sb);
                break;
            case 'c':
            strcat(path,"/");
            nrCFiles = ParseFileInDir(dir);
            printf("Total number of files with .c extension : %d \n",nrCFiles);
                break;
            default:
                printf("Invalid option %c\n",choice[i]);
                break;
            }
        }
}
void wrongOption(char *validCommands,char *choice)
{
     int charIndex = 0;
     if(choice[0] !='-')
        {
            printf("Put -%s\n\n",choice);
            return;
        }
    while(choice[charIndex]!='\0')
        {
            if(strchr(validCommands,choice[charIndex]) == NULL)
                printf("Invalid option: %c \n",choice[charIndex]);
            charIndex++;
        }
        printf("\n");
}
void menuFunction(struct stat st,char *path)
{
    DIR *dir;
    regex_t extension;
    char *validCommands;
        if (S_ISREG(st.st_mode))
        {
            regFileMenu(st,path);
            validCommands = "-ndhmal";
            if(regcomp(&extension,".c$",REG_EXTENDED) != 0)
                printf("Error compiling the regular expression\n");
        }
        if(S_ISLNK(st.st_mode))
        {
            symFileMenu(st,path);
            validCommands = "-nldta";
            if(regcomp(&extension,".c$",REG_EXTENDED) != 0)
                printf("Error compiling the regular expression");
        }

        if(S_ISDIR(st.st_mode))
        {
            dir = opendir(path);
            if(dir == NULL)
                printf("Error could not open the directory");
            dirFileMenu(dir,path);
            validCommands = "-ndac";
            if(regcomp(&extension,".c$",REG_EXTENDED) != 0)
                printf("Error compiling the regular expression");
        }
        printf("Enter your choice: ");
        scanf("%s",choice);
        system("clear");
        printf("Your choice %s  \n",choice);
        if(regexec(&extension,choice,0,NULL,0) != 0)
        {
            wrongOption(validCommands,choice);
            menuFunction(st,path);
            strcpy(choice,"");
        }
        int i = 0;
        while(choice[i] != '\0') 
        {
            regularFile(i,st,path);
            linkFile(i,st,path);
            directoryFile(i,st,path,dir);
            i++;
        }
}

/*void process(struct stat st, char* path)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("Fork failed\n");
        exit(EXIT_FAILURE);
    }
    if (pid == 0)
    {
        printf("Current file is %s\n", path);
        if (S_ISREG(st.st_mode))
        {
            //execlp("bash", "bash", );
            printf("Regular file\n");

        }

        if (S_ISLNK(st.st_mode))
        {
            printf("Link file\n");
        }

        if (S_ISDIR(st.st_mode))
        {
            printf("Directory\n");
        }
    }
}*/

int main(int argc, char *argv[])
{
    struct stat st;
    if (argc != 2) 
    {
        fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (lstat(argv[1], &st) == -1) 
    {
        perror("lstat");   
        exit(EXIT_FAILURE);
    }
    strcpy(path,argv[1]);
    //process(st, path);
    menuFunction(st,path);
}