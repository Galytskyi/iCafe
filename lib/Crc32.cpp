#include "Crc32.h"

quint32 CalcCRC32(const void* pBuf, int bufSize)
{
	unsigned char* ptr = (unsigned char*)pBuf;

	quint32 crc32 = 0xFFFFFFFF;

	while (bufSize > 0)
	{
		crc32 = (crc32 >> 8) ^ Crc32Table[(crc32 & 0xFF) ^ *ptr];

		ptr++;
		bufSize--;
	}

	return crc32 ^ 0xFFFFFFFF;
}






