#include <fstream>
#include <iostream>
#include <vector>
#include <string>

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::ios;
using std::vector;

void printElfFile(unsigned char* file, int size) {
    if (memcmp("\177ELF", file, 4) == 0) {
      cout << "Elf signature" << endl;
    }else {
      cerr << "No elf signature" << endl;
      return;
    }
}

int main(int argc, char** argv) {
  cout << "Elf reader" << endl;
  if (argc < 2) {
    cerr << "Specify a file" << endl;
    return -1;
  }
      ifstream input;
    input.open(argv[1], ios::binary);
    if (!input) {
        cerr << "Error openning file" << endl;
        return -1;
    }
    int fileSize = 0;
 vector<unsigned char> fileContent;
    while (!input.eof()) {
        char c;
        input.get(c);
        fileContent.push_back(c);
        fileSize++;
    }
    input.close();
    printElfFile(fileContent.data(), fileSize);
    return 0;
}
