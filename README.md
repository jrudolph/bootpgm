# Windows Boot Program And Native API Kit

A Windows boot program is a piece of software which executes while Windows is booting. 
Information about how to write such a program is very incomplete. For an university project I collected several pieces of information on this topic I'd like to share...

An example of a boot program is autocheck the tool which checks the hard disk for errors right at the start of Windows - remember this blue screen just before the logon box appears? 

To write such a program you can only use the NT Native API. The native api consists of all the functions the kernel "exports" to user mode programs. Windows applications normally use the API of the Win32 subsystem (kernel32.dll, user32.dll, gdi32.dll...) which itself uses the native API to speak to the kernel. At the time a boot program is executing no subsystem is available so one has to fall back to the Native API.

## Contents
 * HowToBuild
 * WindowsNativeApi
 * [RegistryWriting Writing to the Registry at boot-time]
 * [CppExceptionProblems Problems with C++ Exceptions in Native API programs]

## [wiki:Screenshots Screenshots]
[[Image(Screenshots:screenshot2.png,link=wiki:Screenshots)]]

## Paper & Presentation
 * [attachment:winnapi-paper.pdf Paper(German)]
 * [attachment:winnapi.pdf Presentation(German)]

## More Information & Links
 * Sysinternals' [http://www.microsoft.com/technet/sysinternals/information/NativeApplications.mspx Inside Native Applications]
 * Gary Nebbett's
{{{
#!html
<a href="http://www.amazon.de/gp/product/1578701996?ie=UTF8&tag=virtvoid-21&linkCode=as2&camp=1638&creative=6742&creativeASIN=1578701996">Windows NT/2000 Native API Reference</a><img src="http://www.assoc-amazon.de/e/ir?t=virtvoid-21&l=as2&o=3&a=1578701996" width="1" height="1" border="0" alt="" style="border:none !important; margin:0px !important;" />
}}}
 * Sven Schreiber's
{{{
#!html
<a href="http://www.amazon.de/gp/product/0201721872?ie=UTF8&tag=virtvoid-21&linkCode=as2&camp=1638&creative=6742&creativeASIN=0201721872">Undocumented Windows 2000 Secrets, w. CD-ROM: A Programmer's Cookbook</a><img src="http://www.assoc-amazon.de/e/ir?t=virtvoid-21&l=as2&o=3&a=0201721872" width="1" height="1" border="0" alt="" style="border:none !important; margin:0px !important;" />
}}}
 or the [http://www.rawol.com/?topic=41 PDF Version]
 * NTInternals' [http://undocumented.ntinternals.net/ Undocumented NT Functions for Microsoft Windows NT/2000]
 * Microsoft Windows Internals 
{{{
#!html
<a href="http://www.amazon.de/gp/product/0735619174?ie=UTF8&tag=virtvoid-21&linkCode=as2&camp=1638&creative=6742&creativeASIN=0735619174">en</a><img src="http://www.assoc-amazon.de/e/ir?t=virtvoid-21&l=as2&o=3&a=0735619174" width="1" height="1" border="0" alt="" style="border:none !important; margin:0px !important;" />
}}}
 /
{{{
#!html
<a href="http://www.amazon.de/gp/product/0735619174?ie=UTF8&tag=virtvoid-21&linkCode=as2&camp=1638&creative=6742&creativeASIN=0735619174">de</a><img src="http://www.assoc-amazon.de/e/ir?t=virtvoid-21&l=as2&o=3&a=0735619174" width="1" height="1" border="0" alt="" style="border:none !important; margin:0px !important;" />
}}}
 * A [http://www.osronline.com/showThread.cfm?link=9504 thread] on OSR's forum
 * [http://metasploit.com/users/opcode/syscalls.html Syscall Table and comparison of different Windows versions]
 * Petter Nordahl's site about the [http://home.eunet.no/pnordahl/ntpasswd/ Offline Nt Password & Registry Editor] with some interesting information regarding the structure of the SAM
 * Bruce Ediger's comment about [http://www.users.qwest.net/~eballen1/nt.sekrits.html Windows NT, Secret APIs and the Consequences]

also see the OldPage (German)
