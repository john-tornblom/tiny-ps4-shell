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

## Usage
Install the compiled pkg on a jailbroken PS4, launch the application, and telnet
to the PS4 (port 2323).

## Reporting Bugs
If you encounter problems with tiny-ps4-shell, please [file a github issue][issues].
If you plan on sending pull requests which affect more than a few lines of code,
please file an issue before you start to work on you changes. This will allow us
to discuss the solution properly before you commit time and effort.

## License
tiny-ps4-shell is licensed under the GPLv3+.

[openorbis]: https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain
[issues]: https://github.com/john-tornblom/tiny-ps4-shell/issues/new

