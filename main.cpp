
#include <iostream>

#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "hdmi_aux_analyzer.h"


using namespace std;


int main(int argc, const char* argv[]) {
  cout << "===== HDMI-AUX-Analyzer Started =====" << endl;

  if (argc < 2) {
    cerr << "[ERROR] Usage: " << argv[0] << " [input_file]" << endl;
    return EXIT_FAILURE;
  }

  const char* filename = argv[1];
  cout << "[INFO] Opening file: " << filename << endl;

  int ifd = open(filename, O_RDONLY);
  if (ifd < 0) {
    cerr << "[ERROR] Failed to open file: " << strerror(errno) << endl;
    return EXIT_FAILURE;
  }

  struct stat st;
  if (fstat(ifd, &st) != 0) {
    cerr << "[ERROR] Failed to get file information: " << strerror(errno) << endl;
    close(ifd);
    return EXIT_FAILURE;
  }

  size_t file_sz = st.st_size;
  cout << "[INFO] File size: " << file_sz << " bytes" << endl;

  uint8_t* data = (uint8_t*) calloc(1, file_sz);
  if (!data) {
    cerr << "[ERROR] Memory allocation failed" << endl;
    close(ifd);
    return EXIT_FAILURE;
  }

  ssize_t total_read = 0;
  while (total_read < file_sz) {
    ssize_t bytes = read(ifd, data + total_read, file_sz - total_read);
    if (bytes <= 0) {
      cerr << "[ERROR] Failed to read file: " << strerror(errno) << endl;
      free(data);
      close(ifd);
      return EXIT_FAILURE;
    }
    total_read += bytes;
  }

  close(ifd);
  cout << "[INFO] File read successfully" << endl;

  uint8_t* p = data;
  cout << "[INFO] Starting analysis..." << endl;

  for (size_t i = 0; i + 2 < file_sz; i++) {
    if (p[i] == 0xcd && p[i+1] == 0xab) {
      uint8_t type = p[i+2];

      switch (type) {
        case 0x83:
          cout << "[FOUND] Text Frame detected" << endl;
          break;
        case 0x82:
          cout << "[FOUND] AVI InfoFrame detected" << endl;
          ParseAVIInfoFrame(&p[i+2]);
          break;
        case 0x87:
          cout << "[FOUND] HDR InfoFrame detected" << endl;
          ParseHDRInfoFrame(&p[i+2]);
          break;
        default:
          cout << "[WARNING] Unhandled Frame Type: " << static_cast<int>(type) << endl;
          break;
      }
    }
  }

  cout << "[INFO] Analysis complete" << endl;
  free(data);

  cout << "===== HDMI-AUX-Analyzer Finished =====" << endl;
  return EXIT_SUCCESS;
}
