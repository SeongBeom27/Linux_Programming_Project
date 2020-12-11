#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <error.h>

int main()
{
    DIR *dir;
    struct dirent *ent;
    char systemdatapath[] = "./SystemData/";
    //dir = opendir("./SystemData/");
    dir = opendir(systemdatapath);
    if(dir != NULL)
    {
        while((ent = readdir(dir)) != NULL)
        {
            printf("%s\n", ent->d_name);
        }
        closedir(dir);
    }
    else{
        perror("Cannot open SystemData Directory.");
        return EXIT_FAILURE;
    }
    exit(0);
}

