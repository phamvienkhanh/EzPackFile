
#include "packfile.h"

#include <iostream>

#define LOG_TEST(mesg, rs) std::cout << "Test: " << mesg << " : " << (rs ? "pass" : "failed") << "\n"
#define LOG_VAL(val) std::cout << "  " << #val << " = " << val << "\n"

void testWriteHeader () {
    auto f = std::fopen("testHeader.bin", "wb");

    EzFileHeader header;
    header.version = 1;
    header.encrytedDataOffset = 12345678;

    auto rs = header.write(f);
    std::fclose(f);

    LOG_TEST("write header", rs);
}

void testReadHeader () {
    auto f = std::fopen("testHeader.bin", "rb");

    EzFileHeader header;

    auto rs = header.read(f);
    std::fclose(f);

    bool check = rs && header.version == 1 && header.encrytedDataOffset == 12345678;

    LOG_TEST("read header", check);
    LOG_VAL((int)header.version);
    LOG_VAL(header.encrytedDataOffset);
}

void testWriteChunkKey () {
    
}

int main(int argc, char const *argv[])
{
    testWriteHeader();
    testReadHeader();

    return 0;
}
