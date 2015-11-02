#include <stdint.h>
#include <stdlib.h>

#define rot(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

#define mix(a,b,c) \
{ \
  a -= c;  a ^= rot(c, 4);  c += b; \
  b -= a;  b ^= rot(a, 6);  a += c; \
  c -= b;  c ^= rot(b, 8);  b += a; \
  a -= c;  a ^= rot(c,16);  c += b; \
  b -= a;  b ^= rot(a,19);  a += c; \
  c -= b;  c ^= rot(b, 4);  b += a; \
}

#define final(a,b,c) \
{ \
  c ^= b; c -= rot(b,14); \
  a ^= c; a -= rot(c,11); \
  b ^= a; b -= rot(a,25); \
  c ^= b; c -= rot(b,16); \
  a ^= c; a -= rot(c,4);  \
  b ^= a; b -= rot(a,14); \
  c ^= b; c -= rot(b,24); \
}

uint32_t lookup3 (
  const void *key,
  size_t      length,
  uint32_t    initval
) {
  uint32_t  a,b,c;
  const uint8_t  *k;
  const uint32_t *data32Bit;

  data32Bit = key;
  a = b = c = 0xdeadbeef + (((uint32_t)length)<<2) + initval;

  while (length > 12) {
    a += *(data32Bit++);
    b += *(data32Bit++);
    c += *(data32Bit++);
    mix(a,b,c);
    length -= 12;
  }

  k = (const uint8_t *)data32Bit;
  switch (length) {
    case 12: c += ((uint32_t)k[11])<<24;
    case 11: c += ((uint32_t)k[10])<<16;
    case 10: c += ((uint32_t)k[9])<<8;
    case 9 : c += k[8];
    case 8 : b += ((uint32_t)k[7])<<24;
    case 7 : b += ((uint32_t)k[6])<<16;
    case 6 : b += ((uint32_t)k[5])<<8;
    case 5 : b += k[4];
    case 4 : a += ((uint32_t)k[3])<<24;
    case 3 : a += ((uint32_t)k[2])<<16;
    case 2 : a += ((uint32_t)k[1])<<8;
    case 1 : a += k[0];
             break;
    case 0 : return c;
  }
  final(a,b,c);
  return c;
}

/*

unsigned int stringToHash(char *word, unsigned int hashTableSize){
  unsigned int initval;
  unsigned int hashAddress;

  initval = 12345;
  hashAddress = lookup3(word, strlen(word), initval);
  return (hashAddress%hashTableSize);
  // If hashtable is guaranteed to always have a size that is a power of 2,
  // replace the line above with the following more effective line:
  //     return (hashAddress & (hashTableSize - 1));
}

*/