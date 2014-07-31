/* Xz.c - Xz
2009-04-15 : Igor Pavlov : Public domain */

#if defined(_WIN32)
#include <winsock2.h>
#include <windows.h>
#endif

#include <openssl/ssl.h>
#include <openssl/err.h>
#include "libclamav/crypto.h"

#include "7zCrc.h"
#include "CpuArch.h"
#include "Xz.h"
#include "XzCrc64.h"

Byte XZ_SIG[XZ_SIG_SIZE] = { 0xFD, '7', 'z', 'X', 'Z', 0 };
Byte XZ_FOOTER_SIG[XZ_FOOTER_SIG_SIZE] = { 'Y', 'Z' };

unsigned Xz_WriteVarInt(Byte *buf, UInt64 v)
{
  unsigned i = 0;
  do
  {
    buf[i++] = (Byte)((v & 0x7F) | 0x80);
    v >>= 7;
  }
  while (v != 0);
  buf[i - 1] &= 0x7F;
  return i;
}

void Xz_Construct(CXzStream *p)
{
  p->numBlocks = p->numBlocksAllocated = 0;
  p->blocks = 0;
  p->flags = 0;
}

void Xz_Free(CXzStream *p, ISzAlloc *alloc)
{
  alloc->Free(alloc, p->blocks);
  p->numBlocks = p->numBlocksAllocated = 0;
  p->blocks = 0;
}

unsigned XzFlags_GetCheckSize(CXzStreamFlags f)
{
  int t = XzFlags_GetCheckType(f);
  return (t == 0) ? 0 : (4 << ((t - 1) / 3));
}

void XzCheck_Init(CXzCheck *p, int mode)
{
  p->mode = mode;
  switch (mode)
  {
    case XZ_CHECK_CRC32: p->crc = CRC_INIT_VAL; break;
    case XZ_CHECK_CRC64: p->crc64 = CRC64_INIT_VAL; break;
    case XZ_CHECK_SHA256:
        p->sha = cl_hash_init("sha256");
        break;
  }
}

void XzCheck_Update(CXzCheck *p, const void *data, size_t size)
{
  switch (p->mode)
  {
    case XZ_CHECK_CRC32: p->crc = CrcUpdate(p->crc, data, size); break;
    case XZ_CHECK_CRC64: p->crc64 = Crc64Update(p->crc64, data, size); break;
    case XZ_CHECK_SHA256:
        if ((p->sha))
            cl_update_hash(p->sha, (const Byte *)data, size);
        break;
  }
}

int XzCheck_Final(CXzCheck *p, Byte *digest)
{
  switch (p->mode)
  {
    case XZ_CHECK_CRC32:
      SetUi32(digest, CRC_GET_DIGEST(p->crc));
      break;
    case XZ_CHECK_CRC64:
    {
      int i;
      UInt64 v = CRC64_GET_DIGEST(p->crc64);
      for (i = 0; i < 8; i++, v >>= 8)
        digest[i] = (Byte)(v & 0xFF);
      break;
    }
    case XZ_CHECK_SHA256:
      if (!(p->sha))
          return 0;

      cl_finish_hash(p->sha, digest);
      break;
    default:
      return 0;
  }
  return 1;
}
