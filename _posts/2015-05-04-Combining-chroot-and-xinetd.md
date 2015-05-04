---

layout: post
title: "Combining-chroot-and-xinetd"
author: "Dhaval Kapil"
excerpt: "Running network services in a chroot jail"
keywords: xinetd, chroot, jail, network, daemon, linux, service

---

> ## Introduction

In this blog we will talk about running network applications securely. A simple program(that takes I/O from the console) can be run as a secure service using a combination of `xinetd` and `chroot`. I used this technique while developing challenges for [Backdoor](https://backdoor.sdslabs.co/). The [ECHO](https://backdoor.sdslabs.co/challenges/ECHO) challenge is a good example.

Key points:

1. The program running in the background takes I/O directly from the console.
2. `xinetd` handles all the network related requests.
3. The program is run in a jail directory using `chroot` with restricted access to directory structure.

I will give a simple walkthrough but first I expect the reader to be familiar with the following:

> ### xinetd

This is what [wikipedia](http://en.wikipedia.org/wiki/Xinetd) says:

      xinetd listens for incoming requests over a network and launches the appropriate service for that request. Requests are made using port numbers as identifiers and xinetd usually launches another daemon to handle the request.

Instead of starting each server individually, `xinetd` is the only daemon process to be started. It listens for each and every service listed in its configuration and starts the appropriate service whenever a new request comes up.

> ### chroot

Again from [wikipedia](http://en.wikipedia.org/wiki/Chroot):

      A chroot on Unix operating systems is an operation that changes the apparent root directory for the current running process and its children. A program that is run in such a modified environment cannot name (and therefore normally not access) files outside the designated directory tree. The modified environment is called a "chroot jail".

Setting up a `chroot` jail is easy though time consuming.

> ## Walkthrough - `reader`

We'll write a simple service to that takes the name of a file as the input and pritns the first 1024 bytes of the file.

> ### 1. Write source program for the service

      #include <stdio.h>
      #include <unistd.h>
      #include <fcntl.h>
      #include <errno.h>

      int main()
      { char file_name[50];
        char buf[1025];
        int fd;
        
        printf("Enter filename:\n");
        fflush(stdout);
        scanf("%s", file_name);
        
        fd = open(file_name, O_RDONLY);
        if(fd==-1)
        { printf("Error: %d\n", errno);
          return -1;
        }
        if(read(fd, buf, sizeof(buf))<0)
        { printf("Error: %d\n", errno);
          close(fd);
          return -1;
        }

        printf("%s\n", buf);
        close(fd);
        return 0;
      }

Download it [here](/assets/files/Combining-chroot-and-xinetd/source.c)

> ### 2. Creating a chroot jail

First of all let's compile our code and generate the binary.

> gcc reader.c -o reader

As it will be jailed, we need to import all the libraries that our binary `reader` will require. For finding all the required libraries we will use `ldd`.

      $ ldd reader
      linux-vdso.so.1 =>  (0x00007ffc79702000)
      libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f4b13787000)
      /lib64/ld-linux-x86-64.so.2 (0x00007f4b13b76000)

This is a sample output on my machine and it may vary for yours. So basically I copy the two libraries (libc.so.6 and ld-linux-x86-64.so.2) maintaining the same directory structure relative to my program. My directory structure now looks like this:

      ./
      |-- lib/
      |-- |-- x86_64-linux-gnu/
      |-- --- |-- libc.so.6
      |-- lib64/
      |-- |-- ld-linux-x86-64.so.2
      |-- reader.c
      |-- reader

To test that you have successfully created a jail test this:

> chroot . ./reader

You won't be able to view any file outside the reader's directory. If you are then you did something wrong!

> ### 3. Adding a configuration file in xinetd for `reader`

First of all make sure that `/etc/xinetd.conf` contains the following line:

> includedir /etc/xinetd.d

After that create a new configuration file `/etc/xinetd.d/reader`

      service reader
      {
        type    = UNLISTED
        protocol  = tcp
        socket_type = stream
        port    = 8001
        wait    = no

        server    = /usr/sbin/chroot
        server_args = /home/vampire/reader/ ./reader
        
        user    = root
      }

You can download the file [here](/assets/files/Combining-chroot-and-xinetd/reader).

Explanation:

1. type = UNLISTED: Standard services are listed in `/etc/services`. Our service is not standard so we will also need to specify the `protocol` and `port`.

2. protocol = tcp: We shall use tcp protocol.

3. socket_type = stream: We will use connection oriented socket

4. port = 8001: The port number our service will listen to

5. wait = no: Our service is multithreaded. There can be more than one client connected to it at a time.

6. server = /usr/sbin/chroot: This is the `chroot` binary in my machine. You can find yours by executing `which chroot`.

7. server_args = /home/vampire/reader/ ./reader: These are the parameters passed to `chroot` command.

8. user = root: Only root users can run `chroot`.

For complete list see the [man](http://linux.die.net/man/5/xinetd.conf) page.

> ### 4. Restart the xinetd daemon

The `xinetd` daemon can be restarted using the following command:

> /etc/init.d/xinetd restart

`xinetd` logs in `/var/log/syslog` by default.

Hurray! We have successfully run our service securely. To test it run the following command:

> nc localhost 8001

Change the IP/port accordingly. You should be able to run the program correctly. Also try giving different source file path names. You won't be able to access any file other than in `/home/vampire/reader/`. Also keep in mind to flush the buffer so that the text is displayed instantly.

Comments are welcome. If you know of a better way to do this, feel free to tell me!