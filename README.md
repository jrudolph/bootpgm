# Windows Boot Program And Native API Kit

**Note: All of this information is from 2007 and may long be outdated**

A Windows boot program is a piece of software which executes while Windows is booting. 
Information about how to write such a program is very incomplete. For a university project 
I collected several pieces of information on this topic I'd like to share.

An example of a boot program is autocheck the tool which checks the hard disk for errors right at the start of Windows - remember this blue screen just before the logon box appears? 

To write such a program you can only use the NT Native API. The native api consists of all the functions the kernel "exports" to user mode programs. Windows applications normally use the API of the Win32 subsystem (kernel32.dll, user32.dll, gdi32.dll...) which itself uses the native API to speak to the kernel. At the time a boot program is executing no subsystem is available so one has to fall back to the Native API.

## How to build the sample boot program (worked in 2007...)
 * Clone the latest sources
 * Install the Windows DDK
 * Start the Windows DDK Command Line Environment
 * cd to the sources
 * use `build-interactive.bat` or simply `build -wg` to build the boot program

## Screenshots
![Screenshot 1](https://github.com/jrudolph/bootpgm/blob/master/doc/screenshot1.png)
![Screenshot 2](https://github.com/jrudolph/bootpgm/blob/master/doc/screenshot2.png)
![Screenshot 3](https://github.com/jrudolph/bootpgm/blob/master/doc/screenshot3.png)

## Paper & Presentation
 * [Paper (German)](https://github.com/jrudolph/bootpgm/blob/master/doc/winnapi-paper.pdf?raw=true)
 * [Presentation (German)](https://github.com/jrudolph/bootpgm/blob/master/doc/winnapi.pdf?raw=true)

## More Information & Links
 * Sysinternals' [Inside Native Applications](https://docs.microsoft.com/en-us/sysinternals/learn/inside-native-applications)
 * Gary Nebbett's <a href="http://www.amazon.de/gp/product/1578701996/ref=as_li_tl?ie=UTF8&camp=1638&creative=19454&creativeASIN=1578701996&linkCode=as2&tag=virtvoid-21&linkId=RGN3TH74V47EXNWI">Windows NT/2000 Native API Reference</a> (affiliate link)
 * Sven Schreiber's <a href="http://www.amazon.de/gp/product/0201721872/ref=as_li_tl?ie=UTF8&camp=1638&creative=19454&creativeASIN=0201721872&linkCode=as2&tag=virtvoid-21&linkId=DWDLWZLDJUHP2IAQ">Undocumented Windows 2000 Secrets, w. CD-ROM: A Programmer's Cookbook</a> (affiliate link) or the [PDF Version](https://users.du.se/~hjo/cs/common/books/Undocumented%20Windows%202000%20Secrets/sbs-w2k-preface.pdf)
 * NTInternals' [Undocumented NT Functions for Microsoft Windows NT/2000] (http://undocumented.ntinternals.net/)
 * Microsoft Windows Internals <a href="http://www.amazon.de/gp/product/0735625301/ref=as_li_tl?ie=UTF8&camp=1638&creative=19454&creativeASIN=0735625301&linkCode=as2&tag=virtvoid-21&linkId=KM2PRWRZ23EEACPN">Windows® Internals, Fifth Edition (PRO-Developer)</a> (affiliate link)
 * A [thread](http://www.osronline.com/showThread.cfm?link=9504) on OSR's forum
 * [Syscall Table and comparison of different Windows versions](http://j00ru.vexillium.org/ntapi/)
 * Petter Nordahl's site about the [Offline Nt Password & Registry Editor](https://pogostick.net/~pnh/ntpasswd/) with some interesting information regarding the structure of the SAM
 * Bruce Ediger's comment about [Windows NT, Secret APIs and the Consequences](http://www.stratigery.com/nt.sekrits.html)

# Additional Information

## Windows Native API

Windows has many ways to access system functions. The normal programmer would just use the methods exported by the dynamic link libraries kernel32.dll, user32.dll and others. They belong to a user-mode API called Win32. Windows was designed to have many of those user-mode APIs called '''Subsystems'''. There were or are Win32, POSIX and Os/2 subsystems. Every subsystem is an API and a runtime environment an application can use to access the system functions of the OS.

But how do these subsystems access the kernel?

The answer is: through ntdll. Ntdll is a native dynamic link library providing direct links to kernel mode functions. A program which only uses this API is called a native program and a flag in the executable header marks that fact (see the MS linker's `/SUBSYSTEM` parameter). The subsystems themselves are native programs, of course.

Also see [Wikipedia's article about the Native API](http://en.wikipedia.org/wiki/Native_API).

### Boot programs and the native API
Boot programs always have to use the native API and link against ntdll. That is because of the fact that other subsystems are just not available at the time a boot program gets executed. A corrollar: boot programs can't use the normal runtime library because it references Win32 APIs for various tasks. Therefore ntdll.dll exports several common runtime functions boot programs and other native programs can use.

### The registry at boot-time

At the time a boot program is executed the registry is not yet initialized. Machine/system and machine/hardware are loaded because they need to be loaded for Windows to find the drivers. The SAM at machine\SAM and the machine\software are not yet loaded. If you have to read values from this keys you have to plug the keys into the registry (`NtLoadKey`) from the corresponding file. Don't forget to unload the keys (`NtUnloadKey`) afterwards because Windows fails with a bluescreen afterwards if it finds hives mapped when it does not expect them to be.

### Writing to Registry at boot-time

Writing to Registry is even more difficult: The Registry is read-only at boot-time. The causes for this are not known to me but I guess it's because of security issues and they wanted to stop some faulty driver to wreak havoc in the registry even before Windows has booted.

There is a variable in the Windows Configuration Manager which controls if the Registry can be flushed to disk. It is called `CmpNoWrite`. You may use the kernel debugger to lookup the value...

So the registry is not writeable at boot-time.

What are the solutions for this problem?
 * Unset the flag using the Kernel Debugger. This works but this is not a very automatic solution. It is not portable as well since `CmpNoWrite` is at another position in the kernel everytime the kernel is built.
 * Unset the flag in the boot program from user-mode using a hack. (see [function `showNoWrite`](https://github.com/jrudolph/bootpgm/blob/master/win32/experimental.cpp#L235)). The same issues regarding portability apply.
 * Use `NtInitializeRegistry` to initialize the registry like smss.exe would do it after executing the boot program. That call loads the software and SAM hives and marks the Registry as writable. You don't even have to flush the registry to disk since the changes are persistent nevertheless after booting. That is because the registry can only be initialized once. If you call `NtInitializeRegistry`, the regular call from smss.exe will fail but Windows will start anyway. It is unclear if that works all the time, during our testing it seemed to work always. *A side note: I found this fact about how to make the registry writable in a usenet post from 1997 but it still works...*

## C++ exceptions in native programs

This native api program/library uses C++-features like classes in many
places. This seems to work without problems so far.
It would be appropriate to use C++ exceptions as well. This will not
work, however. At least not without some serious effort. C++ exceptions are working
through subtle mechanisms of Windows, the C++ compiler *and* the
runtime library all together.

To use exception handling one has to enable the specific options in
the compiler. You can use this lines in your SOURCES to enable it:

```
USE_NATIVE_EH=1
USE_RTTI=1
```

If we don't link a runtime library linking will error with unresolved
externals like `__CxxFrameHandler` and others.

Since we cannot use Win32 dlls in a native program we cannot link
against the standard runtime library (msvcrt). So the right choice seems to be the
use of the staticly linkable runtime library libc. This does not work
either. Even libc contains uncountable references to functions defined
in kernel32 and user32. We cannot link to them, of course.

So your choices are:
 * reimplement C++ exception handling on top of the native (API) features
 provided by Windows and the compiler
 * use structured exception handling as documented by Microsoft, this
 will not work in functions relying on automatic object deconstruction
 * don't use exceptions at all (that was my choice)

See also:
 * [Reversing Microsoft Visual C++ Part I: Exception Handling](http://www.openrce.org/articles/full_view/21)
 * [How a C++ compiler implements exception handling](http://www.codeproject.com/Articles/2126/How-a-C-compiler-implements-exception-handling)
 * [The Exception Model](http://blogs.msdn.com/cbrumme/archive/2003/10/01/51524.aspx)
 * [A Crash Course in SEH](http://www.microsoft.com/msj/0197/exception/exception.aspx)
 * [New Vectored Exception Handling in Windows XP](http://msdn.microsoft.com/msdnmag/issues/01/09/hood/default.aspx)
