### NOTE: Status of this project *is work in progress* and it is not yet fully usable

## PROGRESS

- Packet encoding and decoding: done
- Rewritten enc_block, dec_block and pkt_checksum to ASM: done
- Respond to ICMP echo: done
- Configuration file support on PB-2000: done
- UDP sockets: done
- DNS resolver (IN A only): done
- Compiled binary so far: ~10 kB

## What is PBNET?

### Well, what is it?

PBNET is an attempt at creating something resembling a semi working IP stack for the Casio PB-2000 personal computer. It consists of an uplink daemon running on Linux, written in C, and a library / API / utilities for the PB-2000, written in DL-Pascal.

### For the what?

Casio PB-2000 (PB-2000C / AI-1000) is an 8-bit programmable pocket computer made and sold by Casio in the late 1980s, sporting a QWERTY + numeric keyboard, 192x32 dot matrix LCD and up to 64 kB RAM (32 kB + RP-33 RAM pack). The RAM is shared between system area, heap, stack and file storage, and is battery-backed. The PB-2000 has a calculator mode and a menu-driven file manager. The CPU is a custom Hitachi HD61700, same as used in another pocket Casio computer, the PB-1000. What makes the PB-2000C unique is that it supports a number of programming environments, unlike most similar devices of this era that out of the box only supported BASIC. The PB-2000 has a built-in ROM with a K&R C (yes, C!) interpreter - that's the PB-2000C - or one with LISP (really) - that's the AI-1000 that was only sold in Japan. But that's not where it ends - it also has a ROM card slot, and ROM cards were available with BASIC (OM-53B) and Prolog (OM-52P). C was also available as a ROM card for the AI model. The PB-2000 can interface with the rest of the world via the FA-7 interface (RS-232 serial, parallel port, tape audio in/out) or the MD-100, which is the latter, but replaces the tape interface with a 3.5" floppy disk drive. Unlike other 8-bits you might have heard of like Commodore, Atari , ZX-Spectrum and such like, it does not have a TV output and no fancy scan line juggling circuitry, nor does it have a sound chip - only a simple piezo buzzer out of which you can PWM some chirps. But it's pocketable and runs on batteries. It was meant for engineering and academic use.

### So it's a programmable calculator?

Shut your trap! It's a pocket 8-bit!1!!!1!

### PBNET was written in what?

Why, DL-Pascal of course! None of the ROMs available for the PB-2000 were compilers, even the C one is an interpreter, so, fast is what they are not. There is only one exception, and at the same time the only third-party ROM ever made for the PB-2000, which is DL-Pascal that was developed in 1989-1990 by Hans Larsson of Data-Larsson, Sweden. DL-Pascal is a proper, fully featured Pascal compiler, mostly compatible with Turbo Pascal 5.5 and QuickPascal 1.0, which supports inline assembly, building units (libraries), preprocessor, etc. DL-Pascal takes the PB-2000 to where Casio were never able to. The 64 kB ROM contains the compiler, a full screen editor with search/replace/cut/copy/move/goto capabilities, a menu-driven file manager, a debugger, a calculator / CLI mode with batch file support (basic "shell scripting") and more. Compiled binaries are small and code runs fast, and Pascal provides many features that other supported ROMs never did. DL-Pascal also came with a 300+ page grimoire of a manual.

### Can I try?

Well, yes and no. If you're even reading this, chances are that you already have one, but otherwise, the Casio PB-1000 and PB-2000 are sought after by collectors, although can still semi regularly be found on eBay for a few hundred (enter currency here). The hardware is is one part.

Now, as to DL-Pascal... If you have the DL-Pascal ROM, then congratulations, you are one of the chosen few. Firstly, knowledge of the history of DL-Pascal is very scarce. Most of what is known, comes from a handful (like, two maybe) of collectors on the French-speaking forum silicium.org. From there and from e-mail exchanges with a handful of people I have learned that only several hundreds of this ROM were made, if that, and even fewer have resurfaced and are in people's hands - again, mostly collectors. Apparently DL-Pascal was also sold with a modified PB-2000C where it replaced the built-in ROM. DL-Pascal is truly a rarity in the world of computing. I managed to get hold of a pre-release ROM via eBay a few years ago - apparently a one-of-a-kind card, but I got access to the proper production version (1.2) in mid-2021. Because the author of DL-Pascal, mr. Hans Larsson, opposes people sharing the dump of this ROM for reasons that are not completely clear - or at least this is what is being said on forums - DL-Pascal remains mostly unseen and unknown. Even a Google search for "DL-Pascal" returns only a few results.

### How did you even find this thing?

The PB-2000 and me go a long way. When I was about seven or eight years old, a friend of the family sold his PB-2000C to my mother, and so began my adventure with computing. It came with the BASIC and Prolog ROMs, FA-7 and the FP-100 4-colour X-Y plotter. I used it throughout most of elementary school and secondary school and it's provided a huge help, mostly with maths, physics and chemistry, but also a great way to pass dull moments, because I wrote lots of games for it. Unfortunately in about 2001 I had it stolen, and so it never got into university with me, but it would have been proud knowing that I went for CS. In about 2010 I bought one again and never did much with it, then in 2016 I bought the DL-Pascal ROM, and finally in 2021 I started to use it again.

## What does PBNET do?

I want to say it's an IP stack, but I can only say this with some caveats. It is a half-duplex, synchronous, bi-directional packet pusher. It is a library and some applications that together allow the PB-2000C to talk UDP and TCP to the Internet, resolve DNS names and send and respond to ICMP echo requests.

## How does it work?

PBNET connects a Linux host (gateway, router, whatever) to the PB-2000 using its serial port, and allows it to exchange packets. The host side effectively handles framing and flow control ("Layer 2"), whereas the PB-2000 handles that, plus the actual IP packets. All of the processing is done entirely in the PB-2000, with the following exceptions:

- IP, ICMP, UDP and TCP checksums can be (optionally!) offloaded to the host side. The algorithm is dead-simple, but it adds even more delay, so this can be taken care of by the host
- If the PB-2000 is disconnected or otherwise unreachable, such as not running PBNET, the host can send ICMP unreachables on its behalf

### Framing over serial

The natural choice for a project like this was to use SLIP (https://en.wikipedia.org/wiki/Serial_Line_Internet_Protocol), which is a well-established standard, and that is what I tested initially. SLIP is trivial to implement, but it quickly turned out it wasn't going to work. SLIP requires hardware flow control in both directions, because it does not escape XON/XOFF. Unfortunately PB-2000C does not support hardware flow control in the RX direction, so a dedicated protocol was required that either worked with XON/XOFF and escaped those bytes, or did its own flow control. Because the PB-2000 is quite fragile when it comes to its serial buffer overflowing, flow control had to be explicitly controlled, and XON/XOFF is typically transparent, so I had to do my own.

To exchange data between the host and the PB, I developed a simple block transfer protocol with explicit flow control, that I could call IPBSP: IP over Broken Serial Ports. Data is transfered in up to n-byte blocks (generally as large as the PB's 256-byte buffer can fit without barfing a Read Fault error, but I settled for 190), and receipt of every block is confirmed with an `ACK` byte. The last block in a packet (which can be less than n bytes), ends with a `SEP` byte. Flow control is performed using two more bytes: `STX` (stop transmission) and `RTX` (resume transmission). So we have four control bytes. Because the block size necessarily has to be controlled, we cannot afford an escape type encoding like the one used by SLIP, where a reserved byte XXX is transmitted as a pair: ESC + ESC_XXX - so we could have a portion encoded in anywhere between n and n ** 2 bytes. Instead, I opted for a variant of the escapeless encoding scheme (http://chilliant.blogspot.com/2020/01/escapeless-restartable-binary-encoding.html), where with x reserved bytes, we inspect a block for x non-occurring bytes and replace the reserved bytes with those, and send those x replacements as a header before the block of data. This ensures a fixed maximum block size, at the cost of 4 bytes per block, and extra processing. See "goals and limitations". Note that no acknowledgments are performed in the PB->host direction. With data volumes being so low, there is near-zero chance that the host will not be able to read all data coming from the PB in time.

There are numerous other projects for 8-bit computers that include an Ethernet adapter and provide an IP stack, but they often require either modifying the hardware or are based on add-on boards that say go into the ROM card slot. PBNET allows for connecting an unmodified PB-2000, as is, with a suitable serial interface, to the Internet.

## Project goals and limitations

- **PBNET will only support the absolute working minimum.** It does not aim for RFC compliance and working with every guideline that an IP stack should conform to. While I would be glad to implement all this, storage is limited and I have to cheat. On the PB-2000, PBNET will mostly silently drop packets when they are too big, fragmented, or otherwise not expected. This is actually not unusual today with firewalls everywhere, still it could be better.

- **PBNET will do as much as possible in the PB-2000.** We could move most of processing and connection handling to the host side and have it serve as a proxy, e.g. the PB would send a request like "create UDP socket to a.b.c.d port n", and the host would then do that and send back an ID, and then if data arrived, it would only send the payload, prepended with the ID and length. TCP could be completely outsourced to the host as well - but that is not what we want.

- **PBNET is slow to respond.** That depends what we mean by "slow", howeve on the PB-2000 side, everything is written in DL-Pascal + assembly. While this is the best performance you can get on this platform with a high-level language, latency is significant. Every system call can add tens of milliseconds. Also the sheer slowness of the interface (9600 bps max) contributes to this. Overall, looking at test results, it seems like I am achieving at least 75% of RX/TX data rates achievable with no processing at all. See **throughput**.

- **PBNET works with the help of a host computer, not with two PB-2000s directly.** Other than for fun purposes, there isn't much value in talking between two PB-2000s. You can still do it via the host - it is by all mean possible to have IP between two PBs, but this would mean flow control in the TX direction as well, which will make it slower. Maybe I will revisit this.

- **Initially PBNET will only support IPv4**, which seemed fitting given the era when the PB-2000 was made. There are no technical limitations to supporting IPv6, just that it would be slower than IPv4. An IPv4 header is 20 bytes where an IPv6 header is twice that, so that difference alone would already consume 10% of the achievable bandwidth.

- **PBNET uses a dedicated half-duplex framing protocol**, so only one side transmits at a time. Unfortunately it has to be this way. The PB-2000 does not support hardware flow control on the RX side, so that already precludes the use of SLIP, and because we send arbitrary binary data, XON/XOFF is also a no-go. On top of this, PBNET is being developed using an FTDI-based serial interface replacing the FA-7 or MD-100, and there is no control over buffering and no transparent flow control, so flow control has to be explicit and must be performed on the application side - for example, when we have received a full packet, we must immediately instruct the other end to stop transmitting until we have handled it - and, possibly, finished sending a response - only then we resume transmission. In fact, the host suspends further transmission immediately after it has sent the SEP byte. While the packet is being processed, the serial port RX buffer could easily overflow.

- **PBNET is synchronous, so only one operation happens at a time.** Unfortunately there is no way to call user-supplied ISRs (Interrupt Service Routines) without modifying either the hardware or the ROM, so we cannot jump and process packets when some data is received on the serial port while the PB-2000 is doing something else. The only way to operate is to wait for a packet, decide what to do with it, and send a response back if needed - or the other way round: send a packet, and wait for a response and only a response to that packet. Although we can have callbacks and simultaneous socket listeners, still only one thing happens at a time.

- **The host side uses a very shallow transmit queue, by default limited to 5 packets.** This prevents from queuing endlessly just to have the PB-2000 respond after tens of seconds.

## Throughput

With the core routines (block encoding, decoding and checksums) written in HD61700 assembly, processing is already not the limiting factor and the framing overhead is low (4 bytes per 190 bytes). Piotr Piatek's USB serial module is speed-limited on the RX side and maximum rates PBNET achieves are about 250 Bps RX + ~530 Bps TX. With the FTDI chip buffering, we don't have to worry about the speed with which we write to PB's port, but for use with Casio serial interfaces, PBNET uses staggered transmit and a fixed delay is added per byte (`-l` option, microseconds, default is 1000 = 1 ms). This does not affect the preformance via the  USB interface, but allows uninterrupted transmission with the FA-7 or MD-100. This was tested by Piotr Piatek with an MD-100 and rates were 400+ Bps RX, 700+ Bps TX. With Piotr's USB module, we achieve ~250 Bps RX (limited by the module), and ~530 Bps TX.

## API and protocol support

PBNET supports (will support) ICMP, UDP and TCP, and has a very basic DNS resolver, which only handles IN A records.

## Future plans

- PBNET could eventually support IPv6 - it's only a matter of parsing more types of packets

## Building and installing PBNET

Download the source on a Linux host, type "make". This will build the host daemon, pbnet/pbnet, compile the assembly files and include into pb2000/pbnet.src.pas to make pb2000/pbnet.pas.

### Acknowledgments

Like pretty much every project relating to PB-1000 and PB-2000, I would like to thank Piotr Piatek (http://www.pisi.com.pl/piotr433/index.htm) for his insights into the hardware and help with testing. This man has created an amazing body of work around these platforms and he also developed FPGA/CPLD based hardware modules for serial communication and mass storage, replacing the bulky FA-7 or MD-100 - he also developed an emulator for the PB-2000 that I use for compiling PBNET without having to transfer the source back and forth. I would also like to thank Pascal a.k.a. Xerxes for pushing his pre-production DL-Pascal card to eBay one day, Juergen Keller for his help with accessing DL-Pascal 1.2 and Blue for the HD61700 cross-assembler (http://hd61700.yukimizake.net/)

