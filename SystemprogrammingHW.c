#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>

struct OPTION // option flag structure
{
	unsigned int a:1;
	unsigned int S:1;
	unsigned int l:1;
};

struct DIR_ENTRY
{
	struct dirent *dir_entry;
	struct stat buf;
};


int compare(struct DIR_ENTRY **arg1, struct DIR_ENTRY **arg2); //qsort(lib) compare func
int CHECK_OPTION(char *argv_option, struct OPTION *OPTION_FLAG);
void PRINT_LIST(struct DIR_ENTRY **MY_DIR_ENTRY, int DIR_ENTRY_SIZE);
void PRINT_LONG_LIST(struct DIR_ENTRY **MY_DIR_ENTRY, int DIR_ENTRY_SIZE); // for -l option
char* GET_USER_NAME(uid_t uid); // get user name from uid_t uid
char* GET_GROUP_NAME(gid_t gid); // get group name from gid_t gid
void PRINT_ERROR(); // if error(invalid input)


int main(int argc, char *argv[])
{
	int i;
	char *path = "."; // defalut path = current directory
	char *fullpath;
	DIR *dp;
	
	struct dirent *temp_dentry;
	struct DIR_ENTRY **MY_DIR_ENTRY; // list of DIR_ENTRY pointers
	int DIR_ENTRY_SIZE = 0;

	struct OPTION *OPTION_FLAG = malloc(sizeof(struct OPTION));

//	for(i = 0; i < argc; i++)
//		printf("argv[%d] = %s\n",i, argv[i]);

	if((dp = opendir(path)) == NULL) // default open : current path
	{
		printf("PATH ERROR!\n");
		return 0;
	}

	if(argc == 2)
	{
		if(CHECK_OPTION(argv[1], OPTION_FLAG)); // ls [option]

		else // ls [path]
		{
			path = argv[1];
			
			if((dp = opendir(path)) == NULL)
			{
				PRINT_ERROR();
				return 0;
			}
		}
	}

	else if(argc == 3) // ls [option] [path]
	{
		if(!CHECK_OPTION(argv[1], OPTION_FLAG)) // option
		{
			PRINT_ERROR();
			return 0;
		}

		if((dp = opendir(argv[2])) == NULL) // path
		{
			PRINT_ERROR();
			return 0;
		}

		path = argv[2];
	}

//============ DEFAULT ROUTIN BELOW =======================================

	while(temp_dentry = readdir(dp)) // get number of files in path directory
	{
		if(temp_dentry -> d_ino != 0)
			DIR_ENTRY_SIZE++;
	}

//	printf("size of entry : %d \n", DIR_ENTRY_SIZE);

	rewinddir(dp); //initial dp

	if(!(MY_DIR_ENTRY = malloc(sizeof(struct DIR_ENTRY*) * DIR_ENTRY_SIZE))) // DIR_ENTRY_SIZE at least 2
	{
		printf("MEMORY ERROR! THERE IS NO SPACE\n");
		return 0;
	}


	for(i = 0; i < DIR_ENTRY_SIZE; i++)
	{
		if(!(MY_DIR_ENTRY[i] = malloc(sizeof(struct DIR_ENTRY))))
		{
			printf("MEMORY ERROR! THERE IS NO SPACE\n");
			return 0;
		}
//		printf("MY_DIR_ENTRY[%d] : %d \n", i, &MY_DIR_ENTRY[i]);
	}

	for(i = 0; i < DIR_ENTRY_SIZE; i++) //get each files info into MY_DIR_ENTRY list
	{
		MY_DIR_ENTRY[i] -> dir_entry = readdir(dp);
		
		fullpath = malloc(strlen(path) + strlen(MY_DIR_ENTRY[i] -> dir_entry -> d_name) +2);
		sprintf(fullpath, "%s/%s", path, MY_DIR_ENTRY[i] -> dir_entry -> d_name);

//		printf("fullpath = %s\n", fullpath);

		if(lstat(fullpath, &MY_DIR_ENTRY[i]->buf) < 0)
			printf("stat error : %s\n", fullpath);

//		printf("size : %d\n", MY_DIR_ENTRY[i] -> buf.st_size);

		free(fullpath);
	}

	if(OPTION_FLAG -> S == 1) // -S : list sorted by file size 
	{
		qsort(&MY_DIR_ENTRY[0], DIR_ENTRY_SIZE, sizeof(MY_DIR_ENTRY[0]), compare);
	}

	if(OPTION_FLAG -> a == 0) // -a : print all files
	{
		for( i = 0; i < DIR_ENTRY_SIZE; i++)
		{
			if(MY_DIR_ENTRY[i] -> dir_entry -> d_name[0] == '.')
				MY_DIR_ENTRY[i] = NULL;
		}
	}

	{
		if(OPTION_FLAG -> l == 1) // -l : print long list format

		{
		
			PRINT_LONG_LIST(MY_DIR_ENTRY, DIR_ENTRY_SIZE);

		}


		else // default listing format

		{

			PRINT_LIST(MY_DIR_ENTRY, DIR_ENTRY_SIZE);

		}
	}

	closedir(dp);

	return 0;
}

int CHECK_OPTION(char *argv_option, struct OPTION *OPTION_FLAG)
{
	int i;

	if(argv_option[0] == '-')
	{
		for(i = 1; argv_option[i] != '\0'; i++)
		{
			if(argv_option[i] == 'a')
				OPTION_FLAG-> a = 1;
			if(argv_option[i] == 'S')
				OPTION_FLAG-> S = 1;
			if(argv_option[i] == 'l')
				OPTION_FLAG-> l = 1;
		}

//		printf( " a: %x S :%x l:%x \n", OPTION_FLAG->a, OPTION_FLAG->S, OPTION_FLAG->l);

		return 1;
	}

	else return 0;
}
void PRINT_LIST(struct DIR_ENTRY **MY_DIR_ENTRY, int DIR_ENTRY_SIZE)
{
	int i;
	for( i = 0; i < DIR_ENTRY_SIZE; i++)
	{
		if(MY_DIR_ENTRY[i] == NULL)
			continue;

		if(S_ISDIR(MY_DIR_ENTRY[i] -> buf.st_mode)) // ISDIR?
		{
			printf("\033[1;94m");
			printf("%s  ", MY_DIR_ENTRY[i]-> dir_entry-> d_name);
			printf("\033[0m");
		}

		else if(MY_DIR_ENTRY[i] -> buf.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) // Check permission of exc
		{
			printf("\033[1;92m");
			printf("%s  ", MY_DIR_ENTRY[i]-> dir_entry-> d_name);
			printf("\033[0m");
		}

		else // regular files
		{
			printf("%s  ", MY_DIR_ENTRY[i]->dir_entry->d_name);
		}
	}

	printf("\n");
}

void PRINT_LONG_LIST(struct DIR_ENTRY **MY_DIR_ENTRY, int DIR_ENTRY_SIZE)
{
	int i;
	struct stat buf;
	struct tm *timebuf; //time info structure

	for( i = 0; i < DIR_ENTRY_SIZE; i++ )
	{
		if(MY_DIR_ENTRY[i] == NULL)
			continue;

		buf = MY_DIR_ENTRY[i] -> buf;

		if(S_ISDIR(buf.st_mode)) printf("d"); else printf("-"); // file permissions - st_mode

		if(buf.st_mode & S_IRUSR) printf("r"); else printf("-");
		if(buf.st_mode & S_IWUSR) printf("w"); else printf("-");
		if(buf.st_mode & S_IXUSR) printf("x"); else printf("-");
		if(buf.st_mode & S_IRGRP) printf("r"); else printf("-");
		if(buf.st_mode & S_IWGRP) printf("w"); else printf("-");
		if(buf.st_mode & S_IXGRP) printf("x"); else printf("-");
		if(buf.st_mode & S_IROTH) printf("r"); else printf("-");
		if(buf.st_mode & S_IWOTH) printf("w"); else printf("-");
		if(buf.st_mode & S_IXOTH) printf("x"); else printf("-");
		
		printf(" %3d ", (int)buf.st_nlink);	// number of links - st_nlink

		printf("%8s ", GET_USER_NAME(buf.st_uid));	// owner user - st_uid
		printf("%8s ", GET_GROUP_NAME(buf.st_gid));	// owner group - st_gid
		printf("%8d ", (int)buf.st_size); // file size - st_size

		timebuf = gmtime(&buf.st_mtime); // last modification time - st_mtime

		printf("%02d/%02d/%02d %02d:%02d ",
				timebuf->tm_year%100,
				timebuf->tm_mon,
				timebuf->tm_mday,
				timebuf->tm_hour,
				timebuf->tm_min);

		if(S_ISDIR(MY_DIR_ENTRY[i] -> buf.st_mode)) // file name - d_name
		{
			printf("\033[1;94m");
			printf("%s  \n", MY_DIR_ENTRY[i]-> dir_entry-> d_name);
			printf("\033[0m");
		}

		else if(MY_DIR_ENTRY[i] -> buf.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
		{
			printf("\033[1;92m");
			printf("%s  \n", MY_DIR_ENTRY[i]-> dir_entry-> d_name);
			printf("\033[0m");
		}

		else
		{
			printf("%s  \n", MY_DIR_ENTRY[i]->dir_entry->d_name);
		}
	}
}


int compare(struct DIR_ENTRY **arg1, struct DIR_ENTRY **arg2)
{
	int value1, value2;
	struct DIR_ENTRY *temp1 = *arg1;
	struct DIR_ENTRY *temp2 = *arg2;

	value1 = temp1 -> buf.st_size;
	value2 = temp2 -> buf.st_size;
	
//	printf("value1 = %d value2 = %d\n", value1, value2);

	if(value1 > value2)
		return -1;

	else if(value1 == value2)
		return 0;

	else if(value1 < value2)
		return 1;
}

char* GET_USER_NAME(uid_t uid)
{
	struct passwd *pwd;

	pwd = getpwuid(uid);
	return (pwd == NULL) ? NULL : pwd->pw_name;
}

char* GET_GROUP_NAME(gid_t gid)
{
	struct group *grp;

	grp = getgrgid(gid);
	return (grp == NULL) ? NULL : grp->gr_name;
}

void PRINT_ERROR()
{

	printf("******ERROR! Please run program right way.************\n");
	printf("=================== HOW TO USE =======================\n");

	printf("./program\n"
			"./program [option]\n"
			"./program [path]\n"
			"./program [option] [path]\n"
			"(This program supports -l -S -a options only)\n");

	printf("======================================================\n");

}
