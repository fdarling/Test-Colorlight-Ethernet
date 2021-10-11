#ifndef CRC32_H
#define CRC32_H
#include <stdint.h>

class crc32
{
protected:
    uint32_t m_crc_table;
public:
    crc32();
};

#endif // CRC32_H
