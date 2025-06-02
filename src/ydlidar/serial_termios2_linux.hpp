#ifndef SERIAL_TERMIOS2_LINUX_HPP
#define SERIAL_TERMIOS2_LINUX_HPP

#include <asm/ioctls.h>
#include <linux/serial.h>
#include <termios.h>
#include <sys/ioctl.h>

// These macros define ioctl commands for custom baud rates
#define TCGETS2     _IOR('T', 0x2A, struct termios2)
#define TCSETS2     _IOW('T', 0x2B, struct termios2)
#define TCSETSF2    _IOW('T', 0x2C, struct termios2)
#define BOTHER      0x1000

struct termios2 {
    tcflag_t c_iflag;   // input mode flags
    tcflag_t c_oflag;   // output mode flags
    tcflag_t c_cflag;   // control mode flags
    tcflag_t c_lflag;   // local mode flags
    cc_t c_line;        // line discipline
    cc_t c_cc[19];      // control characters
    speed_t c_ispeed;   // input speed
    speed_t c_ospeed;   // output speed
};

#endif // SERIAL_TERMIOS2_LINUX_HPP
