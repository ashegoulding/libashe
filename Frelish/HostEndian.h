#ifndef ASHE_HOSTENDIAN_H_
#define ASHE_HOSTENDIAN_H_

#define ASHE_LITTLE_ENDIAN 0x41424344UL 
#define ASHE_BIG_ENDIAN    0x44434241UL
#define ASHE_PDP_ENDIAN    0x42414443UL

#define ASHE_HOST_ENDIAN  ('ABCD') 

#define ASHE_IS_LITTLE_ENDIAN (ASHE_LITTLE_ENDIAN == ASHE_HOST_ENDIAN)
#define ASHE_IS_BIG_ENDIAN (ASHE_BIG_ENDIAN == ASHE_HOST_ENDIAN)
#define ASHE_IS_PDP_ENDIAN (ASHE_PDP_ENDIAN == ASHE_HOST_ENDIAN)

#endif