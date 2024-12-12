#ifndef _PACK_FILE_H_
#define _PACK_FILE_H_

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

#define MAGIC_BYTES_SIZE 6
#define HEADER_SIZE 11

enum EzPackFileChunkType: char {
    ChunkTypeNone = 0,
    ChunkTypeKey = 1,
    ChunkTypeEncData = 2,
};

class EzFileHeader {
 public:
  const char headerSize = HEADER_SIZE;
  const char magicBytes[MAGIC_BYTES_SIZE] = {0x53, 0x49, 0x47, 0x45, 0x4E, 0x43};
  unsigned char version = 0;
  uint32_t encrytedDataOffset = 0;

 public:
  bool read(FILE *file) {
    if(!file) {
        return false;
    }

    char magic[MAGIC_BYTES_SIZE];
    int numRead = 0;
    numRead = std::fread(magic, 1, MAGIC_BYTES_SIZE, file);
    if(numRead != MAGIC_BYTES_SIZE) {
        return false;
    }
    if(std::memcmp(magic, magicBytes, MAGIC_BYTES_SIZE)) {
        return false;
    }

    numRead = std::fread(&version, 1, 1, file);
    if(numRead != 1) {
        return false;
    }

    numRead = std::fread(&encrytedDataOffset, 1, sizeof(encrytedDataOffset), file);
    if(numRead != sizeof(encrytedDataOffset)) {
        return false;
    }

    return true;
  }

  bool write(FILE *file) {
    if(!file) {
        return false;
    }

    int numWrite = 0;
    numWrite = ::fwrite(magicBytes, 1, MAGIC_BYTES_SIZE, file);
    if(numWrite != MAGIC_BYTES_SIZE) {
        return false;
    }

    numWrite = ::fwrite(&version, 1, 1, file);
    if(numWrite != 1) {
        return false;
    }

    numWrite = ::fwrite(&encrytedDataOffset, 1, sizeof(encrytedDataOffset), file);
    if(numWrite != sizeof(encrytedDataOffset)) {
        return false;
    }

    return true;
  }
};

class EzChunk {
 public:
    uint32_t chunkSize = 0;
    unsigned char chunkType = ChunkTypeNone;

 public:
  bool read(FILE *file) {
    if(!file) {
        return false;
    }

    int numRead = 0;
    numRead = std::fread(&chunkSize, 1, sizeof(chunkSize), file);
    if(numRead != sizeof(chunkSize)) {
        return false;
    }

    numRead = std::fread(&chunkType, 1, sizeof(chunkType), file);
    if(numRead != sizeof(chunkType)) {
        return false;
    }
    
    if(!chunkType) {
        return false;
    }

    return true;
  }

  bool write(FILE *file) {
    if(!file) {
        return false;
    }

    int numWrite = 0;
    numWrite = ::fwrite(&chunkSize, 1, sizeof(chunkSize), file);
    if(numWrite != sizeof(chunkSize)) {
        return false;
    }

    numWrite = ::fwrite(&chunkType, 1, sizeof(chunkType), file);
    if(numWrite != sizeof(chunkType)) {
        return false;
    }

    return true;
  }
};

class EzChunkKeyData {
 public:
    uint32_t chunkSize = 0;
    unsigned char fprintSize = 0;
    std::string fprint;
    std::string wrapKey;

 public:
  bool read(FILE *file) {
    if(!file) {
        return false;
    }

    int numRead = 0;
    numRead = std::fread(&fprintSize, 1, sizeof(fprintSize), file);
    if(numRead != sizeof(fprintSize)) {
        return false;
    }

    fprint.resize(fprintSize);
    numRead = std::fread(fprint.data(), 1, fprintSize, file);
    if(numRead != fprintSize) {
        return false;
    }

    int wrapKeySize = chunkSize - 4 -1 -1 -fprintSize;
    if(wrapKeySize <= 0) {
        return false;
    }

    wrapKey.resize(wrapKeySize);
    numRead = std::fread(wrapKey.data(), 1, wrapKeySize, file);
    if(numRead != wrapKeySize) {
        return false;
    }

    return true;
  }

  bool write(FILE *file) {
    if(!file) {
        return false;
    }

    if(!fprintSize|| !fprint.size() || !wrapKey.size()) {
        return false;
    }

    int numWrite = 0;
    numWrite = std::fwrite(&fprintSize, 1, sizeof(fprintSize), file);
    if(numWrite != sizeof(fprintSize)) {
        return false;
    }

    numWrite = std::fwrite(fprint.data(), 1, fprint.size(), file);
    if(numWrite != fprint.size()) {
        return false;
    }

    numWrite = std::fwrite(wrapKey.data(), 1, wrapKey.size(), file);
    if(numWrite != wrapKey.size()) {
        return false;
    }

    return true;
  }
};

class EzChunkEncData {
 public:
    uint32_t chunkSize = 0;
    FILE* fstream = nullptr;
    std::string fileName;

 public:
  bool read(FILE *file);
  bool write(FILE *file);
};

#endif