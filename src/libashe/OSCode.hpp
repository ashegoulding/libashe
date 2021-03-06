#ifndef ASHE_OSCODE_HPP_
/* OS Codes.
 * Refer to 'Ashe-OS-Codes'.
 */
#ifndef ASHE_HOST_OSCODE
#error "'ASHE_HOST_OSCODE' undefined."
#endif

#define ASHE_ISOS_POSIX()                                                      \
    (ASHE_HOST_OSCODE == 0x01 || ASHE_HOST_OSCODE == 0x02 ||                   \
     ASHE_HOST_OSCODE == 0x03 || ASHE_HOST_OSCODE == 0x04)
#define ASHE_ISOS_WIN()                                                        \
    (ASHE_HOST_OSCODE == 0x10 || ASHE_HOST_OSCODE == 0x11 ||                   \
     ASHE_HOST_OSCODE == 0x12)

#endif
