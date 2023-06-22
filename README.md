### NOTE: Status of this project *is work in progress* and it is not yet fully usable ###

Table of contents
-----------------

- [PROGRESS](#progress)
    + [Work notes](#work-notes)
- [What is PBNET?](#what-is-pbnet)
  * [Well, what is it?](#well-what-is-it)
  * [For the what?](#for-the-what)
  * [So it's a programmable calculator?](#so-it-s-a-programmable-calculator)
  * [PBNET was written in what?](#pbnet-was-written-in-what)
  * [Can I try it?](#can-i-try-it)
  * [How did you even find this thing?](#how-did-you-even-find-this-thing)
- [What does PBNET do?](#what-does-pbnet-do)
- [How does it work?](#how-does-it-work)
  * [Framing over serial (IPBSP)](#framing-over-serial-ipbsp)
  * [State machine on the PB-2000 side](#state-machine-on-the-pb-2000-side)
  * [State machine on the host side](#state-machine-on-the-host-side)
- [Project goals and limitations](#project-goals-and-limitations)
- [Throughput](#throughput)
- [Protocol support and features](#protocol-support-and-features)
- [Future plans / items under consideration](#future-plans---items-under-consideration)
- [Using PBNET](#using-pbnet)
  * [Requirements](#requirements)
    + [Host](#host)
    + [PB-2000C / AI-1000](#pb-2000c-ai-1000)
  * [Building and installing PBNET](#building-and-installing-pbnet)
  * [Configuring PBNET](#configuring-pbnet)
    + [PB-2000C / AI-1000 side](#pb-2000c-ai-1000-side)
    + [Host side](#host-side)
  * [Working with PBNET](#working-with-pbnet)
    + [API description (work in progress)](#api-description-work-in-progress)
      - [Structures (records) and data types](#structures-records--and-data-types)
      - [Procedures and functions](#procedures-and-functions)
      - [Error codes](#error-codes)
    + [Typical program flow](#typical-program-flow)
- [FAQ](#faq)
- [Acknowledgments](#acknowledgments)



## PROGRESS

- Packet encoding and decoding: done
- Rewritten enc_block, dec_block and pkt_checksum to ASM: done
- Respond to ICMP echo: done
- Configuration file support on PB-2000: done
- UDP sockets: done
- DNS resolver (IN A only): done
- Compiled library so far: 10.5 kB. Still too much and no TCP yet, but more to be rewritten in asm. After removing config file processing, we gain 2k binary size!
- Added emulated serial port via TCP to Piotr Piatek's PB-2000C emulator

**Current state: library build + net.exe tool: ICMP echo reply test (run with 'icmp' parameter) or DNS resolver test (run with 'ns <host>' parameter)**
**CAL -> `run net [ icmp | ns <host> ]`**

#### Work notes

DL-Pascal is great, but the machine code it produces is bloated, plus it's Pascal. Only way to reduce binary size is to write more in assembly:

- What in C would be shift operators, in DL-Pascal are functions, meaning that there is loading of parameters and several jumps, and then finally a loop(!), because HD61700 does not support arbitrary bit shifts - only 4-bit shifts (`diX`) and 1-bit shifts (`biX`)
- Mathematical expressions produce lots of machine code. For example: `a := a + 5` produces 7 bytes more than `inc(a,5)`, and even at that, `inc()` is a procedure (as per Pascal).
- Function call overhead is noticeable, and so is the machine code increase from declaring functions (~30 bytes). Anything that is single-use, should be inlined. An example is the checksum function which itself is about 50 bytes of machine code, including loading of arguments, where declaring it and calling it uses more machine code than it probably would if it was fully inlined
- A DL-Pascal cross-compiler would be the best way to go if we wanted to optimise things - or at least initially a macro processor that would speed up certain operations and produce less machine code. Shifts, swaps, increments and such should be asm macros

## What is PBNET?

### Well, what is it?

PBNET is an attempt at creating something resembling a semi working IP stack for the Casio PB-2000 personal computer. It consists of an uplink daemon running on Linux, written in C, and a library / API / utilities for the PB-2000, written in DL-Pascal.

### For the what?

Casio PB-2000 (PB-2000C / AI-1000) is an 8-bit programmable pocket computer made and sold by Casio in the late 1980s, sporting a QWERTY + numeric keyboard, 192x32 dot matrix LCD and up to 64 kB RAM (32 kB + RP-33 RAM pack). The RAM is shared between system area, heap, stack and file storage, and is battery-backed. The PB-2000 has a calculator mode and a menu-driven file manager. The CPU is a custom Hitachi HD61700, same as used in another pocket Casio computer, the PB-1000.

What makes the PB-2000C unique is that it supports a number of programming environments, unlike most similar devices of this era that out of the box only supported BASIC. The PB-2000 has a built-in ROM with a K&R C (yes, C!) interpreter - that's the PB-2000C - or one with LISP (really) - that's the AI-1000 that was only sold in Japan. But that's not where it ends - it also has a ROM card slot, and ROM cards were available with BASIC (OM-53B) and Prolog (OM-52P). C was also available as a ROM card for the AI model. The PB-2000 can interface with the rest of the world via the FA-7 interface (RS-232 serial, parallel port, tape audio in/out) or the MD-100, which is the latter, but replaces the tape interface with a 3.5" floppy disk drive.

Unlike other 8-bits you might have heard of like Commodore, Atari , Spectrum and such like, it does not have a TV output and no fancy scan line juggling circuitry (it does have an LCD controller that supports sprites though), nor does it have a sound chip - only a simple piezo buzzer out of which you can PWM some chirps. But it's pocketable and runs on batteries. It was meant for engineering and academic use.

In this document I will mostly be referring to the computer as PB-2000 or simply *PB*, which may mean either the PB-2000C or the AI-1000.

### So it's a programmable calculator?

Shut your trap! It's a pocket 8-bit!1!!!1!

### PBNET was written in what?

Why, DL-Pascal of course! None of the ROMs available for the PB-2000 were compilers, even the C one is an interpreter, so, fast is what they are not. There is only one exception, and at the same time the only third-party ROM ever made for the PB-2000, which is DL-Pascal that was developed in 1989-1990 by Hans Larsson of Data-Larsson, Sweden (http://www.datalarsson.se/)

DL-Pascal is a proper, fully featured Pascal compiler, mostly compatible with Turbo Pascal 5.5 (no Object Pascal though) and QuickPascal 1.0, which supports inline assembly, has the familiar CRT unit, supports building new units (libraries), preprocessor, etc. DL-Pascal takes the PB-2000 to where Casio were never able to. The 64 kB ROM contains the compiler, a full screen editor with search/replace/cut/copy/move/goto capabilities, a menu-driven file manager, a debugger, a calculator / CLI mode with batch file support (basic "shell scripting") and more. Compiled binaries are small and code runs fast (see note below), and Pascal provides many features that other supported ROMs never did. DL-Pascal also came with a 300+ page grimoire of a manual.

*Note on "fast" and "small binaries"*: DL-Pascal definitely produces fastest code out of any development environment supported on the PB platform just by virtue of being a compiler, however with the compiler being so tiny, sacrifices had to be made. There is little room for compiler optimisation, so compared to raw assembly, it is almost a waste of resources, however the convenience beats everything else available for the PB-2000C / AI-1000.

### Can I try it?

Well, yes and no. If you're even reading this, chances are that you already have one, but otherwise, the Casio PB-1000 and PB-2000 are sought after by collectors, although can still semi regularly be found on eBay for a few hundred (enter currency here). The hardware is is one part.

Now, as to DL-Pascal... If you have the DL-Pascal ROM, then congratulations, you are one of the chosen few. Firstly, knowledge of the history of DL-Pascal is very scarce. Most of what is known, comes from a handful (like, two maybe) of collectors on the French-speaking forum [silicium.org](http://silicium.org/forum/). From there and from e-mail exchanges with a handful of people I have learned that only several hundreds of this ROM were made, if that, and even fewer have resurfaced and are in people's hands - again, mostly collectors. Apparently DL-Pascal was also sold with a modified PB-2000C where it replaced the built-in ROM. DL-Pascal is truly a rarity in the world of computing. I managed to get hold of a pre-release ROM via eBay a few years ago - apparently a one-of-a-kind card, but I got access to the proper production version (1.2) in mid-2021. Because the author of DL-Pascal, mr. Hans Larsson, opposes people sharing the dump of this ROM for reasons that are not completely clear - or at least this is what is being said on forums - DL-Pascal remains mostly unseen and unknown. Even a Google search for "DL-Pascal" returns only a few results.

### How did you even find this thing?

The PB-2000 and me go a long way. When I was about seven or eight years old, a friend of the family sold his PB-2000C to my mother, and so began my adventure with computing. It came with the BASIC and Prolog ROMs, FA-7 and the FP-100 4-colour X-Y plotter. I used it throughout most of elementary school and secondary school and it's provided a huge help, mostly with maths, physics and chemistry, but also a great way to pass dull moments, because I wrote lots of games for it. Unfortunately in about 2001 I had it stolen, and so it never got into university with me, but it would have been proud knowing that I went for CS. In about 2010 I bought one again and never did much with it, then in 2016 I bought the DL-Pascal ROM, and finally in 2021 I started to use it again.

## What does PBNET do?

I want to say it's an IP stack, but I can only say this with some caveats. It is a half-duplex, synchronous, bi-directional packet pusher. It is a library and some applications that together allow the PB-2000C to talk UDP and TCP to the Internet, resolve DNS names and send and respond to ICMP echo requests.

## How does it work?

PBNET connects a Linux host (gateway, router, whatever) to the PB-2000 using its serial port, and allows it to exchange packets. The host side effectively handles framing and flow control ("Layer 2"), whereas the PB-2000 handles that, plus the actual IP packets. All of the processing is done entirely in the PB-2000, with the following exceptions:

- IP, ICMP, UDP and TCP checksums can be (optionally!) offloaded to the host side. The algorithm is dead-simple, but it adds even more delay, so this can be taken care of by the host
- If the PB-2000 is disconnected or otherwise unreachable, such as not running PBNET, the host can send ICMP unreachables on its behalf

### Framing over serial (IPBSP)

The natural choice for a project like this was to use [SLIP](https://en.wikipedia.org/wiki/Serial_Line_Internet_Protocol), which is a well-established standard, and that is what I tested initially. SLIP is trivial to implement, but it quickly turned out it wasn't going to work. SLIP requires hardware flow control in both directions, because it does not escape XON/XOFF. Unfortunately PB-2000C does not support hardware flow control in the RX direction, so a dedicated protocol was required that either worked with XON/XOFF and escaped those bytes, or did its own flow control. Because the PB-2000 is quite fragile when it comes to its serial buffer overflowing, flow control was required in the upper layers and because XON/XOFF is typically transparent, I had to do my own flow control.

I developed a simple block transfer protocol with explicit flow control, let's call it **IPBSP** for the lack of a better name: **IP over Broken Serial Ports**:

Data is transferred in up to n-byte blocks and the maximum is 224 bytes. The RX buffer is 256 bytes, but when it is filled up to more than 256-32=224 bytes, flow control triggers (XOFF), apparently even when not configured - at least in my version of DL-Pascal. *[NOTE: double check with DL-Pascal 1.2]*

The receipt of every block is confirmed with an `ACK` byte and the last block in a packet (which can be less than n bytes), ends with a `SEP` byte. Flow control is performed using two more bytes: `STX` (stop transmission) and `RTX` (resume transmission). So we have four control bytes.

Because the block size necessarily has to be controlled, we cannot afford an escape type encoding like the one used by SLIP, where a reserved byte XXX is transmitted as a pair: ESC + ESC_XXX - so we could have a portion encoded in anywhere between *n* and *n ** 2* bytes. I opted for a variant of the [escapeless encoding scheme by Ivan Korsarev](https://github.com/kosarev/escapeless), also see [a good description here](http://chilliant.blogspot.com/2020/01/escapeless-restartable-binary-encoding.html). When I first came across this, I couldn't believe that someone like Claude Shannon hadn't come up with this idea in, like, the 1940s - I really need to do more research here, I still refuse to accept that this isn't older than me!

Problem statement: transmit data using an alphabet *A* consisting of *a* symbols (256 for ASCII), over a medium supporting a reduced alphabet *A'* with *a'* symbols (a subset of *A*), which either lacks *x* symbols (here byte values), or they are otherwise unavailable or reserved. In our case *a* is 256, *x* is 4, *a'* is 252. To reduce *A* to *A'*, we inspect a block of maximum length *a minus x* for *x* non-occurring bytes. Logically, a block of up to 252 bytes cannot contain all 256 values. We find *x* non-occuring bytes in the block and replace any of the reserved bytes with one of those. Then we send those *x* replacements before a block of data. This ensures a fixed maximum block size, at the cost of *x* (here 4) bytes per block, and extra processing - see [goals and limitations](#project-goals-and-limitations). The decoding side reads this four-byte preamble and replaces any of the four replacements in the block that follows, with the four known reserved bytes. The end.

The encoder / decoder implementation handles two special cases. One: if there were no reserved bytes in the block, the first two bytes (replacements) in the header are equal - the decoding side will recognise this and copy the block as-is. Two: last 4 ASCII characters do not occur - we readily use `0xfcfdfeff` as replacements.

No acknowledgments are performed per block transmitted by the PB towards the host like we do in the other direction, only one `ACK` per whole packet. With data volumes being so low, there is near-zero chance that the host will not be able to read all data coming from the PB in time. Corruption is possible, but then this is what checksums and retries are for. If data corruption proves a problem, this will need to be revisited, and perhaps per-block checksums or parity bytes will need to be implemented.

There are numerous other projects for 8-bit computers that include an Ethernet adapter and provide an IP stack, but they often require either modifying the hardware or are based on add-on boards that say go into the ROM card slot. PBNET allows for connecting an unmodified PB-2000, as is, with a suitable serial interface, to the Internet.

### State machine on the PB-2000 side

**On the PB-2000 side, there is no state really**, everything happens on demand:

- When `pkt_get()` is called, the loop waiting for packets starts by sending the `RTX` byte, so the host knows it can forward a packet to PB, and while waiting for packets, PB responds to an `ACK` byte with another `ACK` (but this clears the RX buffer on the PB side). This way, the pbnet daemon on the host and any application using PBNET on the PB-2000 can be started in any order.

- When `pkt_send()` is called, the PB-2000 just encodes and sends the whole packet block by block, ending with a `SEP` byte, and then awaits an `ACK` from the host to confirm that the packet was received.

### State machine on the host side

| Start state | Trigger / Event        | Action / conditions / comments        | End state |
|-------------|------------------------|---------------------------------------|-----------|
| != READY    | [HOST-->PACKET-->PB]   | Queue packet or send ICMP unreachable | NO CHANGE |
|  ->IDLE     | Initial state          |                                       | NO CHANGE |
|    IDLE     | [PB--RTX-->HOST]       | PB ready to receive                   | ->READY   |
|    READY    | [PB--STX-->HOST]       | Stop accepting packets from host      | ->WRTX    |
|    READY    | [HOST--PACKET-->PB]    | [TX BLOCK #1]                         | ->WACK    |
|    WACK     | [PB--ACK-->HOST]       | [TX NEXT BLOCK...SEP]                 | ->WACK    |
|    WACK     | [PB--ACK-->HOST]       | [NO MORE BLOCKS]                      | ->WRTX    |
|    WRTX     | [PB--RTX-->HOST]       | PB ready to receive                   | ->READY   |
|    IDLE     | [PB--DATA-->HOST]      |                                       | ->WBLOCK  |
|    WBLOCK   | [PB--DATA-->HOST]      | Restart WBLOCK timeout timer          | ->WBLOCK  |
|    WBLOCK   | [PB--SEP-->HOST]       | [HOST--ACK-->PB], Forward packe       | ->IDLE    |
|    WACK     | TIMEOUT                | ACK timeout                           | ->IDLE    |
|    WRTX     | TIMEOUT                | RTX timeout                           | ->IDLE    |
|    WBLOCK   | TIMEOUT                | Block receipt timeout                 | ->IDLE    |

A `DEAD` state exists for any failures while transmitting and receiving, but its mechanic has not been implemented yet. This is meant to wait and then attempt to reopen the serial port and TUN interface before attempting to go into IDLE again.

## Project goals and limitations

- **PBNET requires DL-Pascal**. I admit that this severely limits the reach of this project. DL-Pascal however is the only environment available for the PB-2000 that allows building libraries and linking with them. Significant parts of PBNET are either already written in assembly or will be rewritten into assembly, but the interface is still via Pascal functions. Yes, I could write an all-assembly blob that someone could use to write applications for other ROMs than DL-Pascal, but this means mostly single-use applications. The goal is to provide a network API, not one application, but I may review this in the future since the majority of PB-2000C/AI-1000 owners only have the plain C/LISP ROM or BASIC. Even if you need to bundle a ton of machine code with your project, I suppose that is still an API - but what's the use if you have to write it all in assembly...

- **PBNET is a clean slate project**. It is nothing more than one guy's attempt to see what he can remember and put together from a set of classic RFCs. PBNET is not based on, nor is it influenced by, any existing minimal IP stack such as uIP/LwIP, etc. I intentionally restrained myself from looking at those, but I have worked with the BSD sockets API extensively in the past, so some references are inevitable. 

- **PBNET will only support the absolute working minimum.** It does not aim for RFC compliance and working with every guideline that an IP stack should conform to. While I would be glad to implement all this, storage is limited and I have to cheat. On the PB-2000, PBNET will mostly silently drop packets when they are too big, fragmented, or otherwise not expected. This is actually not unusual today with firewalls everywhere, still it could be better.

- **PBNET does not support IP fragments.** It could, of course, but RAM is severely limited. PBNET ignores fragments and always sets the DF bit in outgoing packets.

- **PBNET will do as much as possible in the PB-2000.** We could move most of processing and connection handling to the host side and have it serve as a proxy, e.g. the PB would send a request like "create UDP socket to a.b.c.d port n", and the host would then do that and send back an ID, and then if data arrived, it would only send the payload, prepended with the ID and length. TCP could be completely outsourced to the host as well - but that is not what we want. One thing that does make sense to do is to proxy SSL connections via the host, as a native SSL implementation would be a serious overkill.

- **PBNET is slow to respond.** That depends what we mean by "slow", however on the PB-2000 side, everything is written in DL-Pascal + assembly. While this is the best performance you can get on this platform with a high-level language, latency is significant. Every system call can add tens of milliseconds. Also the sheer slowness of the interface (9600 bps max) contributes to this. Overall, looking at test results, it seems like I am achieving at least 75% of RX/TX data rates achievable with no processing at all. See **throughput**.

- **PBNET works with the help of a host computer, not with two PB-2000s directly.** Other than for fun purposes, there isn't much value in talking between two PB-2000s. You can still do it via the host - it is by all mean possible to have IP between two PBs, but this would mean flow control in the TX direction as well, which will make it slower. Maybe I will revisit this.

- **Initially PBNET will only support IPv4**, which seemed fitting given the era when the PB-2000 was made. There are no technical limitations to supporting IPv6, just that it would be slower than IPv4. An IPv4 header is 20 bytes where an IPv6 header is twice that, so that difference alone would already consume 10% of the achievable bandwidth.

- **PBNET uses a dedicated half-duplex framing protocol**, so only one side transmits at a time. Unfortunately it has to be this way. The PB-2000 does not support hardware flow control on the RX side, so that already precludes the use of SLIP, and because we send arbitrary binary data, XON/XOFF is also a no-go. On top of this, PBNET is being developed using an FTDI-based serial interface replacing the FA-7 or MD-100, and there is no control over buffering and no transparent flow control, so flow control has to be explicit and must be performed on the application side - for example, when we have received a full packet, we must immediately instruct the other end to stop transmitting until we will have handled it - and, possibly, finished sending a response - only then we resume transmission. In fact, the host suspends further transmission immediately after it has sent the SEP byte. While the packet is being processed, the serial port RX buffer could easily overflow.

- **PBNET is synchronous, so only one operation happens at a time.** Unfortunately there is no way to call user-supplied ISRs (Interrupt Service Routines) without modifying either the hardware or the ROM, so we cannot jump and process packets when some data is received on the serial port while the PB-2000 is doing something else. The only way to operate is to wait for a packet, decide what to do with it, and send a response back if needed - or the other way round: send a packet, and wait for a response and only a response to that packet. Although we can have callbacks and simultaneous socket listeners, still only one thing happens at a time.

- **The host side uses a very shallow transmit queue, by default limited to 5 packets.** This prevents from queuing endlessly just to have the PB-2000 respond after tens of seconds.

- **There is no support for receiving multicast**. You can *transmit* multicast without issues, but for it to leave your host, you need multicast routing. "Why would you need multicast?!" is not the right question here - multicast could be needed for things like IoT integration/control or something else. It's just the fact that we would either need to support IGMP (and then probably PIM on the host), or statically subscribe to selected multicast groups via the host, listen on the host and forward to PB. Doable, but it's extra code and I'm not sure if it's worth the effort.

## Throughput

With the core routines (block encoding, decoding and checksums) written in HD61700 assembly, processing is already not the limiting factor and the framing overhead is low (4 bytes per max 224 bytes, plus SEP). Piotr Piatek's USB serial module is speed-limited on the RX side and maximum rates PBNET achieves are about 250 Bps RX + ~530 Bps TX. With the FTDI chip buffering, we don't have to worry about the speed at which we write to PB's port, but for use with Casio serial interfaces, PBNET uses staggered transmit and a fixed delay is added per byte (`-l` option, microseconds, default is 1000 = 1 ms). This does not affect the performance via the  USB interface, but allows uninterrupted transmission with the FA-7 or MD-100. This was tested by Piotr Piatek with an MD-100 and rates were 400+ Bps RX, 700+ Bps TX. With Piotr's USB module, we achieve ~250 Bps RX (limited by the module), and ~517 Bps TX (was ~530, but dropped since the PB now waits for an ACK after sending a packet). At this stage, optimising the encoding and decoding routines further, yields little improvement, as the majority of delays are on the serial port side - both hardware and RX/TX code in the ROM.

## Protocol support and features

PBNET supports (will support) ICMP, UDP and TCP, and has a very basic DNS resolver (see below). A simple TFTP server and a `net` tool (resolver, ping, show config) will probably be developed before TCP.

*Notes on DNS resolver*: The resolver is non-recursive, only handles IN A records (no reverse lookups, just returns back the IP address), and does not cache the results. Maintaining cache lifetime is difficult if you run the app/library on demand without a reliable real-time clock and with only three timers available to the user in DL-Pascal and how they are implemented. Best thing we could do is cache a result for *n* calls rather than *n* seconds.

## Future plans / items under consideration

- Initially PBNET will operate on single connections, one at a time, but the intention is to implement an event loop where multiple sockets can be handled using callbacks
- PBNET could eventually support IPv6 - it's only a matter of parsing more types of packets
- PBNET could be ported to other ROMs than DL-Pascal, but this would mostly lead to single-purpose applications - only DL-Pascal supports reusable, loadable libraries on the PB-2000 platform.

## Using PBNET

### Requirements

#### Host

- A serial port and a null-modem cable, or Piotr Piatek's USB adapter (http://www.pisi.com.pl/piotr433/usb100_e.htm)
- Linux or WSL, anything with a serial port connected to your PB-2000C, a C compiler
- The host daemon uses POSIX calls and Linux' TUN API, but it should be easily portable to BSD and other POSIX compatible OSes
- There is nothing wrong with Windows anymore really and a native Windows port is of course possible, but this is not the author's priority

#### PB-2000C / AI-1000

- VIP ticket: The DL-Pascal ROM card
- Storage is very limited, so it is recommended to have the RP-33 expansion installed to have a total of 64 kB of RAM
- PBNet uses very little stack space and uses less than the default 2 kB of heap/var space *TODO: exact figures*
- Either an MD-100 or FA-7 interface with a null-modem cable (DB25 on the PB side, likely DB9 on the host side), or the USB adapter

### Building and installing PBNET

1. Download the source on a Linux host, type `make`. This will build the host daemon, `host/pbnet`, build the `hd61700` assembler, compile the assembly files, include inline code and build all the resulting sources into `./pb2000/build`.
2. Copy files from `./pb2000/build` onto the PB-2000 however you like. There is a script included `pb2000/sendfile.sh` which cleans up Pascal sources and sends files to a serial device with optional speed limit in Bps (bytes per second), which translates to a per-byte delay. With an MD-100 or FA-7, you shouldn't need a speed limit or delay, but if you get  `Read Fault` while loading the file using MENU -> load -> rs232c, slow it down. *Note:* the `Read Fault`s I encountered were most likely due to a bug in my DL-Pascal pre-release.
3. Go into CAL mode, type `do build.bat`. This will build the `PBNET.DLE` library, `PBNET.UNI` unit file and the `net.exe` tool. ***Warning:** `build.bat` cleans up the sources after the build, including removing itself.*
4. To compile anything against PBNET, both `PBNET.UNI` and `PBNET.DLE` are required, but once compiled, `PBNET.UNI` can be deleted.

***Note on filename case:** Like most humans, I prefer lower case. Although the `do` and `run` commands are not case sensitive, compiling and linking requires the unit files and library files to be ALL CAPS. This is why the libraries are all uppercase, but extra tools like `net.exe` are lowercase.*

### Configuring PBNET

The default settings are normally enough for stable operation and best performance, but can be adjusted if needed - especially IP addressing and the DNS server, say if you have a local one.

#### PB-2000C / AI-1000 side

The defaults are: baud rate 9600, block size 224, IP address `10.99.99.2` and DNS server `9.9.9.9` (https://quad9.net/). There is no gateway setting - this is a point to point link and the PB only has one possible route.

To configure PBNET / change any defaults, create a new text file `pbnet.cfg`, which holds `key=value` settings, one per line, with optional comments starting with `;`. *Note: there must be a newline at the end, because the file is read with Pascal's `readln()` - without a newline, the last line will not be parsed.*

The supported settings are:
- `ip=<a.b.c.d>`: PB's IP address
- `baud=<75..9600>`: baud rate, 9600 bps is the default
- `dns=<e.f.g.h>`: IP address of the DNS resolver - only one
- `blocksize=<16..224>`: best to leave at default value (224), also must match on the host side!
- `ttl=<1..255>`: IP Time To Live (hop limit) set in packets generated by the PB. Default is 64
- `checksums=<tTyYfFnN>`: enable or disable checksum calculation and validation on the PB, default is true. *[not implemented yet, we correct the ICMP checksum but set UDP checksum to 0]*

**NOTE 1:** *If you are happy with the defaults, there is no need for `pbnet.cfg` to exist.*

**NOTE 2:** *The config file is parsed every time `pbn_init` is called, typically once in an application using PBNET, but this significantly contributes to startup delay*

**NOTE 3:** *The config file parser takes up precious code space - it may need to be simplified to be less user-friendly, but more compact, such as a compiled unit with a set of constants*

#### Host side

Just run `pbnet -h` to see the available options and defaults. Using command line parameters you can configure the serial device, baud rate, block size, delay, IP address and netmask of the host side, etc. The default netmask is /24 rather than say /30 which makes more sense for a point to point interface, but this is mostly to test and demonstrate the PB discarding packets meant for other addresses on the same subnet. The default IP address on the host side is `10.99.99.1`.

### Working with PBNET

To build your own program with PBNET, you need the `PBNET.UNI` unit file and `PBNET.DLE` library file. Then it's only a matter of specifying `uses pbnet;` in your Pascal code. Although units come with a "main" initialiser function that is called when the library is loaded, PBNET omits that and `pbnet_init` has to be called explicitly and none of the API calls will work unless it is.

On the host side, just run the `pbnet` binary with correct arguments.

#### API description (work in progress) ####

##### Structures (records) and data types #####

**PBNET uses a combined `ippkt` record** which uses Pascal's variants (like C's unions) to hold all protocol headers and payload. The `case` statements are just DL-Pascal's way to declare variants.

**The packet record** looks as follows: 

```pascal
type
    ippkt=record
        len:word; { total length of packet }
        l4_len:word; { length of layer 4 payload:icmp, udp, tcp, etc. }
        case :boolean of
            false:{ variant 1:raw packet buffer } (data:array [0..1499] of byte);
            true: { variant 2:IPv4 header } (
                ip:ip_hdr;
                case :byte of
                    0:{ variant 1:Raw IPv4 payload } (payload:array [0..1479] of byte);
                    1:{ variant 2:ICMP + payload   } (icmp:   icmp_hdr);
                    2:{ variant 3:UDP + payload    } (udp:    udp_hdr);
                    3:{ variant 4:TCP + payload    } (tcp:    tcp_hdr)
            )
    end;
```

This record is of a fixed size of 1500 bytes *total* (plus `len` and `l4_len` fields). Normally, only one `ippkt` variable is required for all operations. The `l4_len` field contains the payload length for the given protocol (ICMP, UDP, TCP) and is set automatically on packet receipt, and on transmission, this field is used to indicate the payload size we want to send with the given protocol.

**Protocol headers** are defined as follows:

```pascal
const
    { IP protocol numbers }
    ipr_zero=0; ipr_icmp=1; ipr_udp =17; ipr_tcp =6;
    { header lengths }
    ip_hlen=20; icmp_hlen=8; udp_hlen=8; tcp_hlen=20;
type
    quad=array [0..3] of byte;
    ipaddr=quad;
    { IPv4 header }
    ip_hdr=record
        ver_ihl:byte;
        dscp_ecn:byte;
        len:word;
        id:word;
        fl_foff:word;
        ttl:byte;
        proto:byte;
        csum:word;
        src:ipaddr;
        dst:ipaddr;
    end;
    { ICMP header + payload }
    icmp_hdr=record
        mtype:byte;
        mcode:byte;
        csum:word;
        id:word; { belongs to ICMP echo, just here for convenience }
        seq:word; { belongs to ICMP echo }
        payload:array [0..1471] of byte;
    end;
    { UDP header + payload }
    udp_hdr=record
        sport:word;
        dport:word;
        len:word;
        csum:word;
        payload:array [0..1471] of byte;
    end;
    { TCP header + payload }
    tcp_hdr=record
        sport:word;
        dport:word;
        seqno: quad;
        ackno: quad;
        doff:byte; { 5 without options, 6 with MSS }
        flags:byte;
        wsize:word;
        csum:word;
        uptr:word;
        payload:array[0..1459] of byte;
    end;

```

The **payload** (here using a variable `pkt` for illustration), can be accessed as:

- `pkt.data`: raw packet (IP payload + IPv4 header)
- `pkt.ip.payload`: IPv4 payload
- `pkt.udp.payload`: UDP payload
- `pkt.icmp.payload`: ICMP payload
- `pkt.tcp.payload`: TCP payload

The **socket** is defined as follows:

```pascal
    {tcp state}
    tcp_state=(none,listen,synsent,synrecv,estab,finwait1,finwait2,closewait,closing,lastack,timewait,closed);
    { a socket descriptor }
    socket=record
        state:tcp_state; { state (for TCP) }
        lport:word; { local port:source port for PB->world, destination port for world->PB }
        rport:word; { remore port}
        remote:ipaddr; { remote address }
    end;
```

Instead of *source* and *destination* (port, address) we look at *remote* and *local*.

Extra **constants** / special variables are:

```pascal
const
    port_any=0;
var
    addr_any:ipaddr=(0,0,0,0);
```

These are used to accept any remote source and any remote port. For example with UDP, if we set `sock.rport:=port_any` and `sock.remote:=addr_any` and pass this socket to `udp_recv`, it will accept the first UDP packet destined to our IP address and our `sock.lport`, and set `rport` and `remote` to the remote source port and IPv4 address, so from this point, further `udp_recv` calls will filter this and only this connection.

##### Procedures and functions #####

| Function / procedure | 
|----------------------|
| `procedure pbn_shutdown;` |
| `function  pbn_init:boolean;` |
| `procedure pkt_hold;` |
| `procedure pkt_ready;` |
| `function  pkt_send(var pkt:ippkt;timeout:word):shortint;` |
| `function  pkt_get(var pkt:ippkt;timeout:word):shortint;` |
| `function  echo_respond(var pkt:ippkt; timeout:word):shortint;` |
| `function  udp_recv(var pkt:ippkt;var sock:socket;timeout:word):shortint;` |
| `function  udp_send(var pkt:ippkt;var sock:socket;timeout:word):shortint;` |
| `function  tcp_connect(var pkt:ippkt;var sock:socket;timeout:word):shortint;` |
| `function  dns_resolve(shost:string;var rip:ipaddr;var pkt:ippkt;timeout: word; retr: byte):shortint;` |
| `function  strerr(e:shortint): string;` |
| `function  ipaddr_parse(var s:string;var a:ipaddr):byte;` |
| `function  ipaddr_str(var a:ipaddr):string;` |
| `function  quad_eq(var qa:array of byte;var qb:array of byte): boolean;` |

##### Error codes #####

Most PBNET calls return an error code. `E_OK = 0` is returned on success, negative values `E_XXX` are returned on failure and positive values are reserved for protocol-specific errors, such as DNS RCODE response codes.

| Code      | Value | Meaning |
|-----------|-------|---------|
| `E_OK`    | 0     | Success |
| `E_TMO`   | -1    | Timeout (timeouts are specified in tenths of seconds) |
| `E_INTR`  | -2    | Call interrupted (any key pressed - *TODO:* only interrupt on BRK key) |
| `E_TRUNC` | -3    | Packet truncated: there is a mismatch between the IP length field and actual length of packet received |
| `E_CRC`   | -4    | Incorrect checksum (either IP header or ICMP/UDP/TCP checksum) |
| `E_MTU`   | -5    | Packet too big |
| `E_UNXP`  | -6    | Unexpected packet received - we get this if we want to receive packets from a specific host and/or port, but received something else |
| `E_ERR`   | -7    | Any other error |
| `E_INIT`  | -8    | PBNET uninitialised |
| `E_ARG`   | -9    | Argument error |

`dns_resolve`, follow DNS RCODE on top of the above. The following ones are defined - others are still returned if they happen:

```pascal
const
    { DNS return codes (RCODE) }
    de_formerr=1;de_servfail=2;de_nxdomain=3;de_notimp=4;de_refused=5;de_notzone=9;
```

The function `function strerr(e:shortint):string` can be used to convert error codes to strings.

#### Typical program flow ###

[TODO]

## FAQ

- **Q:** *I have everything connected and running, the host is receiving packets from the PB, but I am getting no response from the DNS server or any other Internet or LAN host other than the host connected to the PB - why?*
- **A:** *PBNET forwards packets from your PB towards your host, but that is where it ends. The host needs to have IP forwarding enabled, your router must have a route to your PB via your host and if required, needs to allow NAT from your PB's IP address, or otherwise your host needs to NAT PB's address to its own. Configuring these things is up to you, the user.*

### Acknowledgments

Like pretty much every project relating to PB-1000 and PB-2000, I would like to thank Piotr Piatek (http://www.pisi.com.pl/piotr433/index.htm) for his insights into the hardware and help with testing. This man has created an amazing body of work around these platforms and he also developed FPGA/CPLD based hardware modules for serial communication and mass storage, replacing the bulky FA-7 or MD-100 - he also developed an emulator for the PB-2000 that I use for compiling PBNET without having to transfer the source back and forth. I would also like to thank Pascal a.k.a. Xerxes for pushing his pre-production DL-Pascal card to eBay one day, Juergen Keller for his help with accessing DL-Pascal 1.2 and Blue for the HD61700 cross-assembler (http://hd61700.yukimizake.net/)
