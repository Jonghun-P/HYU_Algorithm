#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

struct OPTION
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


int CHECK_OPTION(char *argv_option, struct OPTION *OPTION_FLAG);
void PRINT_LONG_LIST(struct DIR_ENTRY **MY_DIR_ENTRY, int DIR_ENTRY_SIZE);
int compare(const void *arg1, const void *arg2);
int main(int argc, char *argv[])
{
	int i;
	char *path = ".";
	char *fullpath;
	DIR *dp;
	
	struct dirent *temp_dentry;
	struct DIR_ENTRY **MY_DIR_ENTRY;
	int DIR_ENTRY_SIZE = 0;

	struct OPTION *OPTION_FLAG = malloc(sizeof(struct OPTION));



	for(i = 0; i < argc; i++)
		printf("argv[%d] = %s\n",i, argv[i]);

	if((dp = opendir(path)) == NULL)
	{
		printf("path error!\n");
		return 0;
	}

	if(argc == 2)
	{
		if(CHECK_OPTION(argv[1], OPTION_FLAG));

		else 
		{
			path = argv[1];
			if((dp = opendir(path)) == NULL)
			{
			//	PATH ERROR
			}

		}
	}

	else if(argc == 3)
	{
		if(!CHECK_OPTION(argv[1], OPTION_FLAG))
		{
			printf("Invalid Option Error : plz check option form\n");
			return 0;
		}

		if((dp = opendir(argv[2])) == NULL)
		{
			//PATH ERROR
		}

		path = argv[2];
	}
	
	//===============================================================

	while(temp_dentry = readdir(dp))
	{
		if(temp_dentry -> d_ino != 0)
			DIR_ENTRY_SIZE++;
	}

	printf("size of entry : %d \n", DIR_ENTRY_SIZE);

	rewinddir(dp);

	MY_DIR_ENTRY = malloc(sizeof(struct DIR_ENTRY*) * DIR_ENTRY_SIZE);

	for(i = 0; i < DIR_ENTRY_SIZE; i++)
	{
		MY_DIR_ENTRY[i] = malloc(sizeof(struct DIR_ENTRY));
		printf("MY_DIR_ENTRY[%d] : %d \n", i, &MY_DIR_ENTRY[i]);
	}

	for(i = 0; i < DIR_ENTRY_SIZE; i++)
	{
		MY_DIR_ENTRY[i] -> dir_entry = readdir(dp);
		
		fullpath = malloc(strlen(path) + strlen(MY_DIR_ENTRY[i] -> dir_entry -> d_name) +2);
		sprintf(fullpath, "%s/%s", path, MY_DIR_ENTRY[i] -> dir_entry -> d_name);

		printf("fullpath = %s\n", fullpath);

		if(lstat(fullpath, &MY_DIR_ENTRY[i]->buf) < 0)
			printf("stat error : %s\n", fullpath);

		printf("size : %d\n", MY_DIR_ENTRY[i] -> buf.st_size);

		free(fullpath);
	}


	if(OPTION_FLAG -> a == 1)
	{
	}

	if(OPTION_FLAG -> S == 1)
	{
		qsort(MY_DIR_ENTRY[0] DIR_ENTRY_SIZE, sizeof(MY_DIR_ENTRY[0]), compare);

		PRINT_LONG_LIST(MY_DIR_ENTRY, DIR_ENTRY_SIZE);

	}

	if(OPTION_FLAG -> l == 1)
	{
		PRINT_LONG_LIST(MY_DIR_ENTRY, DIR_ENTRY_SIZE);
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
			printf("argv[%d] : %c \n", i, argv_option[i]);

			if(argv_option[i] == 'a')
				OPTION_FLAG-> a = 1;
			if(argv_option[i] == 'S')
				OPTION_FLAG-> S = 1;
			if(argv_option[i] == 'l')
				OPTION_FLAG-> l = 1;
		}

		printf( " a: %x S :%x l:%x \n", OPTION_FLAG->a, OPTION_FLAG->S, OPTION_FLAG->l);

		return 1;
	}

	else return 0;
}

void PRINT_LONG_LIST(struct DIR_ENTRY **MY_DIR_ENTRY, int DIR_ENTRY_SIZE)
{
	int i;
	struct stat buf;

	for( i = 0; i < DIR_ENTRY_SIZE; i++ )
	{
		buf = MY_DIR_ENTRY[i] -> buf;

		if(S_ISDIR(buf.st_mode)) // file permissions - st_mode
			printf("d");
		else printf("-");

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

		printf("%d ", buf.st_uid);	// owner user - st_uid
		printf("%d ", buf.st_gid);	// owner group - st_gid
		printf("%8d ", buf.st_size); // file size - st_size

//		timeinfo = localtime(&(buf.st_mtime));
//		strftime(buf_time, 20, "%b %d %H:%M", timeinfo);
		printf("%u ",buf.st_mtime); // time of last modification - st_mtime

		printf("%s \n",MY_DIR_ENTRY[i] -> dir_entry -> d_name);	// file name - d_name
	}
}


int compare(const void *arg1, const void *arg2)
{
	int value1, value2;

	value1 = ((struct DIR_ENTRY *)arg1) -> buf.st_size;
	value2 = ((struct DIR_ENTRY *)arg2) -> buf.st_size;

	if(value1 > value2)
		return -1;

	else if(value1 == value2)
		return 0;

	else if(value1 < value2)
		return 1;
}
