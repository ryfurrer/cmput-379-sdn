#include <string>
#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>
#include <fcntl.h>

const char * getFiFoName(int x, int y) {
  std::string s = "fifo-" + std::to_string(x) + "-" + std::to_string(y);
  return s.c_str();
}

int makeFIFO(const char * pathName) {
  int status = mkfifo(pathName, S_IRUSR | S_IWUSR | S_IRGRP |
    S_IWGRP | S_IROTH | S_IWOTH);
  if (errno == 17 && status == -1) { // file exists error
    //printf("Note: %s previously existed.\n", pathName);
    errno = 0;
  } else if (status == -1) {
    printf("Unknown error creating %s. Exiting...\n", pathName);
    exit(-1);
  } else {
    printf("%s FIFO file made.\n", pathName);
  }
  return status;
}

int openReadFIFO(int swID1, int swID2) {
  /* Opens a FIFO for reading */
  makeFIFO(getFiFoName(swID1, swID2));
  int fd = open(getFiFoName(swID1, swID2), O_NONBLOCK | O_RDONLY);
  if (fd < 0) {
    printf("Error opening read fifo-%i-%i.\n", swID1, swID2);
    perror("Details");
    exit(-1);
  }
  return fd;
}

int openWriteFIFO(int swID1, int swID2) {
  /* Opens a FIFO for writing */
  makeFIFO(getFiFoName(swID1, swID2));
  int fd = open(getFiFoName(swID1, swID2), O_NONBLOCK | O_WRONLY);
  if (fd < 0) {
    printf("Error opening write fifo-%i-%i.\n", swID1, swID2);
    perror("Details");
    exit(-1);
  }
  return fd;
}

//int main(int argc, char *argv[]) { return 0;}
