# vdbdump
CLI tool for dumping contents of [ProtonSDK](https://github.com/SethRobinson/proton)'s VariantDB file.

## Compiling
The project should compile fine with any non-ancient GCC or MSVC compiler. There are build scripts (build.sh for Linux and build.bat for Windows) included to simplify the process. The only non-C99-compliant thing is the use of ##__VA_ARGS__, which should be supported by modern GCC and MSVC anyway.

## Usage
Simply do `./vdbdump <path>`. Example output for the `demo.dat` file should look like this:
```
rect|1.000,2.000,3.000,4.000
small_int|-523523
big_int|3563634
point3d|10.990,-2.000,5.355
point|5.000,-3.000
username|houz
pi_value|3.141
```
