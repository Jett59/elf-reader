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

#define LITTLE_ENDIAN 0
#define BIG_ENDIAN 1

static inline int currentEndianness () {
      int16_t endiannessTest = 0x0100;
      return ((int8_t*)&endiannessTest)[0];
}

template <typename t>
static inline t fixEndianness(t n, int targetEndianness) {
    if (currentEndianness() != targetEndianness) {
      char* bytes = (char*)&n;
      for (int i = 0; i < sizeof(n); i ++) {
        char tmpByte = bytes[sizeof(n) - i - 1];
        bytes[sizeof(n) - i - 1] = bytes[i];
        bytes[i] = tmpByte;
      } 
      }
    return n;
}

void printElfFile(unsigned char* file, int size) {
    if (memcmp("\177ELF", file, 4) == 0) {
      cout << "Elf signature" << endl;
    }else {
      cerr << "No elf signature" << endl;
      return;
    }
    switch (file[4]) {
        case 1: {
          cout << "32 bit" << endl;
          break;
        }
        case 2: {
          cout << "64 bit" << endl;
          break;
        }
        default:
          cerr << "Unknown number of bits" << endl;
          return;
    }
    int endianness;
    switch (file[5]) {
        case 1: {
          cout << "Little endian" << endl;
          endianness = LITTLE_ENDIAN;
          break;
        }
        case 2: {
          cout << "Big endian" << endl;
          endianness = BIG_ENDIAN;
          break;
        }
        default:
          cerr << "Unknown endian" << endl;
          return;
    }
    if (currentEndianness() == endianness) {
      cout << "Same as host" << endl;
    }else {
      cout << "Not same as host" << endl;
    }
    cout << "Version " << (unsigned)file[6] << endl;
    switch (file[7]) {
        case 0: {
          cout << "Sysv" << endl;
          break;
        }
        default:
          cerr << "Unknown abi" << endl;
          return;
    }
    switch (fixEndianness(((int16_t*)file)[8], endianness)) {
        case 1: {
          cout << "Relocatable" << endl;
          break;
        }
        case 2: {
          cout << "executable" << endl;
          break;
        }
        case 3: {
          cout << "Shared object" << endl;
          break;
        }
        case 4: {
          cout << "Core" << endl;
          break;
        }
        default:
          cerr << "Unknown elf file type" << endl;
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
