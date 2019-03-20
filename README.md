# Compile TI BSL-Scripter from Source with VC++2017

The code here should compile properly with the Community version of Visual Studio 2017
available for free from Microsoft. For usage please see the TI documents and
INSTRUCTIONS.md in this directory.

This project includes everything required to compile the 
`BSL-Scripter.exe` file for Win32 except the boost headers.

# Dependencies Included

The binary dependencies are included for convenience, it took
a long time to compile boost on my machine and I ended up with
much more than I needed!

From boost (`1_69_0`) the libraries included here are:

* `filesystem`
* `program_options`

From libusb 1.0.22 (very easy to compile)

* `libusb-1.0`

From libhid

* `hidapi.lib`
* `hid.lib`

# Compile Boost (but only `filesystem` and `program_options`)

Since all that is required for a Windows build of `BSL-Scripter`
is filesystem and program_options is makes sense to compile only
those parts of the boost library. From a Visual Studio 2017
command prompt:

    cd \Users\User\workspace\boost_1_69_0
    bootstrap.bat
    .\b2 toolset=msvc-14.1 --with-system --with-filesystem ^
        --with-program_options toolset=msvc-14.1 address-model=32 ^
        runtime-link=static threading=multi variant=debug,release

The `.\b2` line should be all the same line, the `^` characters allow
multiline entry in the Windows Command Prompt.

The above command will build three libraries in 32bit
release and debug mode. The library files and headers will be
in `\Users\User\workspace\boost_1_69_0\stage` which can be used
in your Visual Studio configuration.

## References

* [How to use Boost in Visual Studio 2010](https://stackoverflow.com/questions/2629421/how-to-use-boost-in-visual-studio-2010)
* [Invoking b2](https://www.boost.org/doc/libs/1_51_0/doc/html/bbv2/overview.html#bbv2.overview.invocation)
* [Boost: Getting Started with Windows](https://www.boost.org/doc/libs/1_51_0/more/getting_started/windows.html)
* [Peabody's CP2101 Invoke Script](https://github.com/gbhug5a/CP2102-with-BSL-Scripter-for-MSP430)

# Setup Directories for Boost Headers

If you have not already, download boost (you need the headers):

* [boost 1.69.0](https://dl.bintray.com/boostorg/release/1.69.0/source/)

Open the properties for the project by right clicking on `BSL-Scripter` and
clicking `Properties`, then in `VC++` directories change this line:

    C:\Users\User\workspace4\boost_1_69_0;$(SolutionDir)ThirdParty\include;$(IncludePath)

To match where your boost was extracted to. All the libraries are included 
in this repository for this version of boost (probably not advisable to 
mix versions).

Now the code should compile, at least for "Debug" mode. To compile in Release mode
the same steps must be taken when the Release mode is active.

