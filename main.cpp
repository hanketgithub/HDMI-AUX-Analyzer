
#include <stdio.h>
#include <iostream>

#include <fcntl.h>
#include <unistd.h>

using namespace std;


int main(int argc, const char *argv[]) {
  int ifd;
  cout << "HDMI-AUX-Analyzer" << endl;

  if (argc < 2)
  {
    fprintf(stderr, "useage: %s [input_file]\n", argv[0]);        
    return -1;
  }

  // get input file name from comand line
  ifd = open(argv[1], O_RDONLY);
  if (ifd < 0)
  {
    perror(argv[1]);
    exit(EXIT_FAILURE);
  }  

  size_t file_sz = 0x1000;
  uint8_t *data = (uint8_t *) calloc(1, file_sz);

  read(ifd, data, file_sz);

  uint8_t *p = data;
  for (int i = 0; i < file_sz; i++) {
    if (p[i] == 0xcd && p[i+1] == 0xab) { // key
      uint8_t type = p[i+2];

      switch (type) {
        case 0x83: {
          printf("Text\n");
          break;
        }
        case 0x82: {
          ParseAVIInfoFrame(&p[i+2]);
          break;
        }
        case 0x87: {
          ParseHDRInfoFrame(&p[i+2]);
          break;
        }
        default: {
          printf("Type %d not processed\n\n", type);
          break;
        }
      }
    }
  }

  return 0;
}
