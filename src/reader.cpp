#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdint>
#include <cstdio>

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::ios;
using std::vector;

enum ProgramHeaderEntryType : uint32_t {
  NONE = 0,
  LOADABLE = 1,
  DYNAMIC = 2,
  INTERPRETER = 3,
  NOTE = 4
};

struct ProgramHeaderEntry {
  ProgramHeaderEntryType type;
  uint32_t flags;
  uint64_t file_offset;
  void* virtual_address;
  uint64_t ignored;
  uint64_t file_size;
  uint64_t memory_size;
  uint64_t alignment;
};

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
      for (int i = 0; i < sizeof(n) / 2; i ++) {
        char tmpByte = bytes[sizeof(n) - i - 1];
        bytes[sizeof(n) - i - 1] = bytes[i];
        bytes[i] = tmpByte;
      } 
      }
    return n;
}

const char* getInstructionSetName(int16_t id) {
    switch (id) {
        case 0:
          return "Unspecified";
          case 2:
            return "Sparc";
            case 3:
              return "IA32";
              case 8:
                return "Mips";
                case 0x14:
                  return "Power PC";
                  case 0x28:
                    return "arm";
                    case 0x2a:
                      return "SuperH";
                      case 0x32:
                        return "Itanium";
                        case 0x3e:
                          return "Amd64";
                          case 0xb7:
                            return "Aarch64";
                            case 0xf3:
                              return "Risc-V";
                              default:
                                return nullptr;
    }
}

void* readPointer(const unsigned char* buffer, int pointerBits, int sourceEndianness) {
    switch (pointerBits) {
        case 32:
          return (void*)fixEndianness(*(int32_t*)buffer, sourceEndianness);
          case 64:
          return (void*)fixEndianness(*(int64_t*)buffer, sourceEndianness);
          default:
            cerr << "Unknown pointer width" << pointerBits << endl;
            return 0;
    }
}

void printElfFile(unsigned char* file, int size) {
    if (memcmp("\177ELF", file, 4) == 0) {
      cout << "Elf signature" << endl;
    }else {
      cerr << "No elf signature" << endl;
      return;
    }
    int bits;
    switch (file[4]) {
        case 1: {
          cout << "32 bit" << endl;
          bits = 32;
          break;
        }
        case 2: {
          cout << "64 bit" << endl;
          bits = 64;
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
    const char* instructionSet = getInstructionSetName(fixEndianness(((int16_t*)file)[9], endianness));
    if (instructionSet == nullptr) {
      cerr << "Unknown instruction set" << endl;
      return;
    }
    cout << instructionSet << endl;
    void* entrypoint = readPointer(file + 24, bits, endianness);
    if (entrypoint != nullptr) {
    printf("Entry point %p\n", entrypoint);
    }
    if (bits != 64) {
      cerr << "Only 64 bit executables are supported" << endl;
      return;
    }
    ProgramHeaderEntry* programHeader = (ProgramHeaderEntry*)(file + fixEndianness(((int64_t*)file)[4], endianness));
    if ((void*)programHeader == (void*)file) {
      cerr << "Missing program header" << endl;
      return;
    }
    int number_of_sections = fixEndianness(((int16_t*)file)[28], endianness);
    cout << number_of_sections << " sections:" << endl;
    for (int i = 0; i < number_of_sections; i ++) {
      ProgramHeaderEntry section = programHeader[i];
      cout << "Section" << endl;
      switch (section.type) {
        case NONE: {
          cout << "Null" << endl;
          continue;
        }
        case LOADABLE: {
          cout << "Loadable" << endl;
          break;
        }
        case DYNAMIC: {
          cout << "Dynamic" << endl;
          break;
        }
        case INTERPRETER: {
          cout << "Interpreter" << endl;
          break;
        }
        case NOTE: {
          cout << "note" << endl;
          break;
        }
        default:
          cerr << "Unknown" << endl;
          continue;
      }
      printf("Flags: %x\n", section.flags);
      if (section.flags & 1) {
        cout << "Executable ";
      }
      if (section.flags & 2) {
        cout << "Writable ";
      }
      if (section.flags & 4) {
        cout << "Readable";
      }
      cout << endl;
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
