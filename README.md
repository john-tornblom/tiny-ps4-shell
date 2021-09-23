# Tiny PS4 Shell
tiny-ps4-shell is a small telnet server for the PS4 with a couple of basic
UNIX-like commands, e.g., cd, mkdir, stat, etc.

## Building
Assuming you have [the Open Orbis SDK][openorbis] installed on you machine,
tiny-ps4-shell can be compiled using the following two commands:

```console
john@localhost:tiny-ps4-shell$ export OO_PS4_TOOLCHAIN=/path/to/open/orbis
john@localhost:tiny-ps4-shell$ make
```

You can also compile for POSIX-like systems:
```console
john@localhost:tiny-ps4-shell$ make -f Makefile.posix
john@localhost:tiny-ps4-shell$ ./main_term.bin
```

## Installation
Install the compiled pkg on a PS4 with homebrew and the kexec syscall enabled.
Next, launch the application to install the telnet daemon. When you get the
notification "Install successful", terminate the app, and restart it again to
launch the installed daemon. Yoy may now telnet to the PS4 (port 2323).

## Usage
There are a handful of rudimentary commands available, e.g., cd, ls, and mkdir.
Type `help` in a connected telnet shell for more information. Below are a
couple of commands I have found useful.


To get a list of running processes:
```console
/$ ps
     PID      PPID     PGID      SID    TTY    COMMAND
       0         0        0        0      -    kernel
       1         0        1        1  ttyu0    mini-syscore.elf
...
```

To obtain root permissions:
```console
/$ seteuid 0
```

To escape the initial jailed sandbox:
```console
/$ jailbreak
```

To monitor kernel log:
```console
/$ cat /dev/klog
```

To decrypt SELF and SPRX files:
```console
/$  self2elf /mini-syscore.elf /mnt/usb0/mini-syscore.elf
```

## Limitations
The login session is not attached to a TTY, so you cannot signal for, e.g., SIGINT
with Ctrl+C. Furthermore, most of the commands are only partially implemneted.
If you find some limitation extra anoying, file a github issue and perhaps it will
be addressed.

## Reporting Bugs
If you encounter problems with tiny-ps4-shell, please [file a github issue][issues].
If you plan on sending pull requests which affect more than a few lines of code,
please file an issue before you start to work on you changes. This will allow us
to discuss the solution properly before you commit time and effort.

## License
tiny-ps4-shell is licensed under the GPLv3+.

[openorbis]: https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain
[issues]: https://github.com/john-tornblom/tiny-ps4-shell/issues/new

