#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>

#define PATH_LENGTH 1024

int main(int argc, char** argv) {
  DIR* input_dir;
  struct dirent* entry;
  struct stat buf;
  long long total_size = 0;

  if (argc != 2) {
    printf("dir_size <input_dir>\n");
    return -1;
  }

  input_dir = opendir(argv[1]);
  if (input_dir == NULL) {
    printf("Error while opening directory!\n");
    return -1;
  }

  while ((entry = readdir(input_dir)) != NULL) {
    char file_path[PATH_LENGTH];
    sprintf(file_path, "%s/%s", argv[1], entry->d_name);

    if (stat(file_path, &buf) == -1) {
      printf("Error while getting file stats for %s\n", entry->d_name);
      continue;
    }

    if (!S_ISDIR(buf.st_mode)) {
      printf("%10lld bytes\t%s\n", (long long)buf.st_size, entry->d_name);
      total_size += buf.st_size;
    }
  }

  printf("%10lld bytes\tTOTAL SIZE\n", total_size);

  closedir(input_dir);
  return 0;
}
