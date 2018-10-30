const char * Switch::getFiFoName(int x, int y) {
  std::string s = "fifo-" + std::to_string(x) + "-" + std::to_string(y);
  return s.c_str();
}

int makeFIFO(const char * pathName) {
  int status = mkfifo(pathName, S_IRUSR | S_IWUSR | S_IRGRP |
    S_IWGRP | S_IROTH | S_IWOTH);
  if (errno || status == -1) {
    printf("ERROR: error creating FIFO connection\n");
    exit(-1);
  }
  return status;
}

int openReadFIFO(int swID1, int swID2) {
  /* Opens a FIFO for reading */
  makeFIFO(getFiFoName(swID1, swID2));
  return open(getFiFoName(swID1, swID2), O_NONBLOCK | O_RDONLY);
}

int openWriteFIFO(int swID1, int swID2) {
  /* Opens a FIFO for writing */
  makeFIFO(getFiFoName(swID1, swID2));
  return open(getFiFoName(swID1, swID2), O_NONBLOCK | O_WRONLY);
}