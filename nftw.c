#define _XOPEN_SOURCE 500
//Required header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>

// Declared variables to store counts and size
static int fcount = 0; 
static int dcount = 0; 
static off_t size = 0;


//Declared variables to store path and extension
static const char *extension = NULL;
static const char *srce_dir_path = NULL;
static const char *dstn_dir_path = NULL;

/*count - nftw callback function - single function to calculate count of files, dirs, size of files based on tf -- typeflag */
int count(const char *fp, const struct stat *stt, int tf, struct FTW *fb) {
    if (tf == FTW_F) {
        fcount++;
       size += stt->st_size;
    }

    if (tf == FTW_D) {
            dcount++;
        }
        return 0;
}

/* Below function block allows to copy contents of one file to another file
Opens the source file in read only mode and the destination file in create or read write mode,
then copies the contents of the source file to the destination file.
*/

int copying_file(const char *ptr_src, const char *ptr_dest) {
    int srce_fdsp, destn_fdsp;
    long int total_bytes_read;
    char bf[BUFSIZ];

    // Open source file
    srce_fdsp = open(ptr_src, O_RDONLY);
    if (srce_fdsp == -1) {
        printf("ERRORRR IN OPENING???\n");
        return -1;
    }

    //create or open destinationn file
    destn_fdsp = open(ptr_dest, O_WRONLY | O_CREAT , 0644);
    if (destn_fdsp == -1) {
        printf("ERRORRRR IN OPENINNG OR CREATING\n");
        close(srce_fdsp);
        return -1;
    }

    // Copy the file contents
    while ((total_bytes_read = read(srce_fdsp, bf, sizeof(bf))) > 0) {
        if (write(destn_fdsp, bf, total_bytes_read) != total_bytes_read) {
            close(srce_fdsp);
            close(destn_fdsp);
            return -1;
        }
    }

   //files are closed
    close(srce_fdsp);
    close(destn_fdsp);

        return (total_bytes_read == 0) ? 0 : -1;
    }


    /* Below function code block constructs the destination path and depending
    on the file type either copies or creates new directory in the destination*/
    int copy_func(const char *fp, const struct stat *stt, int tf, struct FTW *fb) {

    char dest_path[PATH_MAX];
    const char *last_folder = strrchr(srce_dir_path,'/') +1;
    int len = strlen(srce_dir_path) - strlen(last_folder);

        // Construct the destination path
        snprintf(dest_path, sizeof(dest_path), "%s/%s", dstn_dir_path,fp + len);

        switch (tf) {
            case FTW_F: // Regular file
                // Check if the file should be excluded
              ; int exclude = strlen(fp) - strlen(extension);

                if (extension != NULL) {
                    if (strcmp(fp + exclude , extension) == 0) {
                        return 0;
                    }
                }
                // Copy the file
                if (copying_file(fp, dest_path) == -1) {
                    perror("copy_file");
                    return -1;
                }
                break;
            case FTW_D: // Directory
                // Create the directory at the destination
                if (mkdir(dest_path, stt->st_mode) == -1 && errno != EEXIST) {
                    perror("mkdir");
                    return -1;
            }
            break;
        default:
            break;
    }
    return 0;

    }

    /* Below function reuses copy function to copy the files to the destination location */
int mov_func(const char *fp, const struct stat *stt, int tf, struct FTW *fb) {
    if (copy_func(fp, stt, tf, fb) == -1) {
        return -1;
    }

}

/*Below function check if its file or directory calls remove or rmdir accordingly to delete the files */
int remove_dirs(const char *fp, const struct stat *stt, int tf, struct FTW *fb) {
  if(fp ==  NULL)
  {
          printf("ERROR DEL");
          return -1;
  }
    if (tf == FTW_DP) {
        if (rmdir(fp) == -1) {
            printf("ERROR DEL");

        }
    } 
    else {
        if (remove(fp) == -1) {
            printf("ERROR DEL");

        }
    }
        return 0;

}
// Function to print help instructions
void help() {
    printf("HELP COMMANDS:\n/dftw -nf [root_dir]\n/dftw -nd [root_dir]\n/dftw -sf [root_dir]\n/dftw -cpx [source_dir] [dest_dir] [ext]\n");
    
}

//Start of the program Main function
int main(int argc, char *argv[]) {

    //Number of command line argument should be 3,4 or 5 if not call help
    if (argc < 3 || argc > 5) {
         help();
    }

    //Pointers to store command line arguments
    const char *action = argv[1];    //to store -nf -nd -sf -cpx -mv
    const char *root_dir = argv[2];

    /*Used strcmp to compare the action and call the count - its a nftw call back function*/
    if(argc == 3)
    {
        if (strcmp(action, "-nf") == 0) { //nf count files
        if (nftw(root_dir, count, 20, flag) == -1) {
            perror("nftw");
            exit(EXIT_FAILURE);
        }
        printf("FILE COUNT OPERATION SUCCESSFULL!\n");
        printf("Total number of files: %d\n", fcount);
        }
        else if (strcmp(action, "-nd") == 0) { //nd count directories
            if (nftw(root_dir, count, 20, FTW_PHYS) == -1) {
                perror("nftw");
                exit(EXIT_FAILURE);
 }
            printf("DIRECTORY COUNT OPERATION SUCCESSFULL!\n");
            printf("Total number of directories: %d\n", dcount);
        }
        else if (strcmp(action, "-sf") == 0) { //sf calculate size
            if (nftw(root_dir, count, 20, FTW_PHYS) == -1) {
                perror("nftw");
                exit(EXIT_FAILURE);
            }
            printf("FILE SIZE CALCULATION OPERATION SUCCESSFULL!\n");
            printf("Total size of files: %ld bytes\n", size);
        }
        else //if the action doesn't match with any of them then call help function
        {
         help();
         exit(EXIT_FAILURE);
        }

    }
    //Used strcmp to compare the action and call the count - its a nftw call back function
    else if (argc >= 4) {
        if(strcmp(action, "-cpx") == 0) //cpx copy files
        {
            if (argc == 5) {
            extension = argv[4];
            }
            srce_dir_path = root_dir;
            dstn_dir_path = argv[3];

            if (nftw(srce_dir_path, copy_func, 20, FTW_PHYS) == -1) {
                perror("nftw");
                exit(EXIT_FAILURE);
            }
            else
            {
                printf("COPIED SUCCESSFULL!\n");
            }
        }
        else if (strcmp(action, "-mv") == 0 && argc == 4) { //mv move files
        srce_dir_path = root_dir;
        dstn_dir_path = argv[3];

            if (nftw(srce_dir_path, mov_func, 20, FTW_PHYS) == -1) {
                perror("nftw");
                exit(EXIT_FAILURE);
            }
            else
            {
                printf("MOVED SUCCESSFULL!\n");
            }

            if (nftw(srce_dir_path, remove_dirs, 20, FTW_DEPTH | FTW_PHYS) == -1) {
                printf("ERROR IN DELETING");
                perror("nftw");
                exit(EXIT_FAILURE);
            }
            else
            {
                printf("DELETED SUCCESSFULL!\n");
            }

        }
        else //if the action doesn't match with any of them then call help function
        {
        help();
        exit(EXIT_FAILURE);
        }

    }
    else { //if the arguments count doesn't match with any of them then call help function
        help();
        exit(EXIT_FAILURE);
    }

    return 0; 
}
