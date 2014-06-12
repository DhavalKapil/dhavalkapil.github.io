---

layout: post
title: "Facebook spam spreads Trojan"
author: "Dhaval Kapil"
excerpt: "Analysing the trojan and speculating on its functions"
keywords: facebook, trojan, malware, message, analysis, java, dll, minerd, bitcoin

---

> ## Introduction

Somewhere back in May, I started getting messages such as this on facebook:

![A screenshot of the message](/assets/images/Facebook-spam-spreads-Trojan/fb_message_snipet.png)

I had around 300 friends then and got almost 8-9 such messages. The interesting thing was that those people were completely unrelated. According to me some of them didn't know each other. Suspecting some trick I decided to analyze the file.

> ## Analysis

### Handling the zip file

I downloaded the `zip` file and verified it's type using linux `file` command. Then after unzipping the file I found a `jar` file inside it. _Just for reference a jar file is a java archive file which can be run by simply double clicking it_. It is equivalent to `exe` file in C/C++. Another important point to note is that a `jar` file is infact a `zip` file(only the extension has been changed). So the next step I did was to extract the `jar` file. Doing that produced a single class file alongwith the usual META-INF
    
    vampire@linux:~/Documents/analysis$ file Form\_0320.ZIP 
    Form\_0320.ZIP: Zip archive data, at least v2.0 to extract
    vampire@linux:~/Documents/analysis$ unzip Form\_0320.ZIP 
    Archive:  Form\_0320.ZIP
      inflating: Form\_0320.jar           
    vampire@linux:~/Documents/analysis$ file Form\_0320.jar
    Form\_0320.jar: Zip archive data, at least v2.0 to extract
    vampire@linux:~/Documents/analysis$ unzip Form\_0320.jar 
     Archive:  Form\_0320.jar
       creating: META-INF/
      inflating: META-INF/MANIFEST.MF    
      inflating: CNQNYPYFEBMJTHYPMGR.class         
    vampire@linux:~/Documents/analysis$ ls
    Form\_0320.jar  Form\_0320.ZIP  CNQNYPYFEBMJTHYPMGR.class  META-INF

_Note: I am analysing Form\_0320.ZIP._

### Decompiling and analysing the java class file

Next I decompiled the class file using the `jad` tool. 

    vampire@linux:~/Documents/analysis$ jad CNQNYPYFEBMJTHYPMGR.class 
    Parsing CNQNYPYFEBMJTHYPMGR.class...The class file version is 51.0 (only 45.3, 46.0 and 47.0 are supported)
      Generating CNQNYPYFEBMJTHYPMGR.jad

It created `CNQNYPYFEBMJTHYPMGR.jad` which contains the source code for the java program. The resulting code was pretty much obscure. All Strings were stored as int arrays of ascii codes. The arrays were dynamically converted to `String` within the code. I present here an easy to understand version of the [code](/assets/files/Facebook-spam-spreads-Trojan/CNQNYPYFEBMJTHYPMGR.jad).

Instead of going through the code liny by line I will explain the whole code at once. The program tries to download a certain file from a dropbox link. The same file has multiple links so that if one fails the others might work. If it get's a successfull HTTP response code it downloads the file to `C:\temp\NWQGHJ.MXZ`. The program then register's it to windows using the `regsvr32` command line operation running in silent mode (/s). So what I did was that I downloaded the file(d.dat) manually from the dropbox link and ran a `file`command with it.

    vampire@linux:~/Documents/analysis$ file d.dat
    d.dat: PE32 executable (DLL) (GUI) Intel 80386, for MS Windows

This shows that the file is a `Windows DLL` file.

### Analysing the DLL file

I tried a few ways at first which seem to be leading to nothing, including `strings` and `objdump`. Later I renamed it to `d.dll`(did that to make sure my antivirus on windows finds it) and switched over to windows. Here's a snapshot of what my antivirus found:

![screenshot of my screen showing the result of the antivirus](/assets/images/Facebook-spam-spreads-Trojan/antivirus.png)

Hence it somehow connects the `dll` with the windows process `explorer.exe`. This process is the one used to browse the files in a computer. Next I used `strings` command and stored the result in a file.

    vampire@linux:~/Documents/analysis$ strings d.dat > file

Then I opened the file in a text editor and searched for .exe. This resulted in two matches:

> minerd.exe  
> explorer.exe

_Seems to be getting somewhere._ Browsing through the 59252 lines I found the following two snippets:

    explorer -o stratum+tcp://%s:%d -O %s:%s -t %d -R 1
    explorer -o stratum+tcp://%s:%d -O %s:%s -t %d -R 1
    explorer -o stratum+tcp://%s:%d -O %s:%s -t 1 -R 1
    explorer -o stratum+tcp://%s:%d -O %s:%s -t %d -R 1
    explorer -o stratum+tcp://%s:%d -O %s:%s -t 1 -R 1
    explorer -o stratum+tcp://%s:%d -O %s:%s -t %d -R 1  

and this one:

    Usage: minerd [OPTIONS]
    Options:
      -a, --algo=ALGO       specify the algorithm to use
                              scrypt    scrypt(1024, 1, 1) (default)
                              sha256d   SHA-256d
      -o, --url=URL         URL of mining server (default: http://127.0.0.1:9332/)
      -O, --userpass=U:P    username:password pair for mining server
      -u, --user=USERNAME   username for mining server
      -p, --pass=PASSWORD   password for mining server
          --cert=FILE       certificate for mining server using SSL
      -x, --proxy=[PROTOCOL://]HOST[:PORT]  connect through a proxy
      -t, --threads=N       number of miner threads (default: number of processors)
      -r, --retries=N       number of times to retry if a network call fails
                              (default: retry indefinitely)
      -R, --retry-pause=N   time to pause between retries, in seconds (default: 30)
      -T, --timeout=N       network timeout, in seconds (default: 270)
      -s, --scantime=N      upper bound on time spent scanning current work when
                              long polling is unavailable, in seconds (default: 5)
          --no-longpoll     disable X-Long-Polling support
          --no-stratum      disable X-Stratum support
      -q, --quiet           disable per-thread hashmeter output
      -D, --debug           enable debug output
      -P, --protocol-dump   verbose dump of protocol-level activities
          --benchmark       run in offline benchmark mode
      -c, --config=FILE     load a JSON-format configuration file
      -V, --version         display version information and exit
      -h, --help            display this help text and exit

I searched about minerd.exe on the internet and found out [this](http://deletemalware.blogspot.in/2013/06/what-is-minerdexe-and-how-to-remove-it.html):

> minerd.exe is a part of multi-threaded CPU miner for Bitcoin crypto-currency system. Very often this application causes CPU usage to go to 90% or even more. Needles to say it's not essential for Windows and may cause problems. If you knowingly installed this Bitcoin miner on your computer then there's nothing to worry about. Even if you antivirus says it's a trojan horse it's probably a false positive. However, cyber crooks and fraudsters are using this software to earn some extra money as well by monetizing botnets. They drop the main mining modules on infected computers and start mining. They usually set low mining speed, so that the minerd.exe process only uses unused CPU cycles.

So basically this file uses the CPU to mine for bitcoins. It is a core part of the bit coin virus. But it doesn't stop here. The file further sends the same message to other people on facebook. In case they open it, the same thing happens again. In this way a botnet(collection of programs communicating over a network with other similar programs to perform tasks) is set by the attacker to mine the bitcoins!

> ## Summary

In short this trojan does the following things:

1. The victim receives a message on facebook. The message contains a zip file and a text message(lol) so that the victim is eager to open it.
2. Inside the zip file is a jar file which the user opens again without suspecting anything.
3. The jar file downloads a dll file from a dropbox link to the local hard disk and registers it to windows.
4. This file then sends the same message to all the friends of the victim on facebook and simultaneously starts mining for bitcoins.
5. Within a short span of time a botnet of victims is set up.

> ## Preventive Measures

1. Never open a file of which you are not sure.
2. Confirm with your friend whether he has sent it or not.(preferably using a different communication channel)
3. Always have an updated antivirus program running on your windows machine.
