#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

/* Forward declaration */
int get_size_dir(char *fname, size_t *blocks);

/* Gets in the blocks buffer the size of file fname using lstat. If fname is a
 * directory get_size_dir is called to add the size of its contents.
 */
int get_size(char *fname, size_t *blocks)
{
    struct stat st;
    if (lstat(fname, &st) == -1) {
        perror(fname);
        return -1;
    }

    *blocks += st.st_blocks;

    if (S_ISDIR(st.st_mode)) {
        if (get_size_dir(fname, blocks) == -1)
            return -1;
    }

    return 0;
}


/* Gets the total number of blocks occupied by all the files in a directory. If
 * a contained file is a directory a recursive call to get_size_dir is
 * performed. Entries . and .. are conveniently ignored.
 */
int get_size_dir(char *dname, size_t *blocks)
{
    DIR *dir = opendir(dname);
    if (!dir) {
        perror(dname);
        return -1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char subpath[4096];
        snprintf(subpath, sizeof(subpath), "%s/%s", dname, entry->d_name);

        if (get_size(subpath, blocks) == -1) {
            closedir(dir);
            return -1;
        }
    }

    closedir(dir);
    return 0;
}

/* Processes all the files in the command line calling get_size on them to
 * obtain the number of 512 B blocks they occupy and prints the total size in
 * kilobytes on the standard output
 */
int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <fichero1> [fichero2 ...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; i++) {
        size_t blocks = 0;
        if (get_size(argv[i], &blocks) == 0) {
            // bloques de 512B → KB: blocks * 512 / 1024 = blocks / 2
            printf("%zuK\t%s\n", blocks / 2, argv[i]);
        }
    }

    return 0;
}
