<?php include 'incs/inc_spage.php'; ?>

<table valign="top" align="center" width="900" class="prompt">
<tr><td>	
<center><b>SOL-1 HARDWARE</b></center><br>

This website is connected to a homebrew 16Bit CPU/Minicomputer built from scratch using 74-Series
logic gates.
<br>It is constructed using wire-wrap boards and has around 270 chips in total. A few years ago I noticed the work
that Bill Buzbee did with his <a href="http://www.homebrewcpu.com/">Magic-1 homebrew minicomputer</a>, and I was fascinated by the idea. If you are interested in CPU homebrewing, then you have to take a look at Magic-1.
<br><br>
Some of the features of Sol-1 are:
<br><B>**</B> Support for user and kernel priviledge modes, with up to 256 processes running in parallel.
<br><B><B>**</B></B> Support for paged virtual memory, such that each process can have a total of 64KB RAM for itself.
<br><B>**</B> Two serial ports (16550), a real time clock(M48T02), 2 parallel ports(8255), a programmable timer(8253), an IDE hard-drive interface(2.5 Inch HDD), and a floppy disk controller(WD37C65).
<br><B>**</B> The CPU supports 8 prioritized external interrupts, and a DMA channel.
<br><B>**</B> The sequencer is microcoded, with 15 ROMS operating horizontally.
<br><B>**</B> Over 500 instructions are implemented right now, and up to 1024 are possible by using escape codes.
<br><B>**</B> Support for 8/16-Bit MUL and DIV instructions.
<br><B>**</B> Fast indexed string instructions in the spirit of x86's (REP) MOVSB, CMPSB, LODSB, STOSB, etc
<br><br>The name Sol-1 comes from the name of the Sun, which is Sol.
<br><br><br>

<center><b>REGISTER TABLE</b></center>


<table valign="top" align="center" >
<tr>
	<td colspan="3" height="30" width="250" align="center"><b>General Purpose Registers</b></td>
</tr>
<tr>
	<td width="200" align="right">A</td>
	<td width="100" align="center">AH/AL</td>
	<td width="400">Accumulator</td>
</tr>
<tr>
	<td width="200" align="right">B</td>
	<td width="100" align="center">BH/BL</td>
	<td width="400">Base Register (Secondary Counter Register)</td>
</tr>
<tr>
	<td width="200" align="right">C</td>
	<td width="100" align="center">CH/CL</td>
	<td width="400">Counter Register (Primary Counter)</td>
</tr>
<tr>
	<td width="200" align="right">D</td>
	<td width="100" align="center">DH/DL</td>
	<td width="400">Data Register / Data Pointer</td>
</tr>
<tr>
	<td width="200" align="right">G</td>
	<td width="100" align="center">GH/GL</td>
	<td width="400">General Register (For scratch)</td>
</tr>

<tr>
	<td colspan="3" height="30" align="center"><b>Special Purpose Registers</b></td>
</tr>
<tr>
	<td width="200" align="right">PC</td>
	<td width="100" align="center">-</td>
	<td width="400">Program Counter</td>
</tr>
<tr>
	<td width="200" align="right">SP</td>
	<td width="100" align="center">-</td>
	<td width="400">Stack Pointer</td>
</tr>
<tr>
	<td width="200" align="right">BP</td>
	<td width="100" align="center">-</td>
	<td width="400">Base Pointer (Used to manage stack frames)</td>
</tr>
<tr>
	<td width="200" align="right">SI</td>
	<td width="100" align="center">-</td>
	<td width="400">Source Index (Source address for string operations)</td>
</tr>
<tr>
	<td width="200" align="right">DI</td>
	<td width="100" align="center">-</td>
	<td width="400">Destination Index (Destination address for string operations)</td>
</tr>
</table>

<br><br><br>

<center><b>DOWNLOADS</b></center>
<br>

<table valign="top" align="center" >
<tr>
	<td width="250" align="right"><a href="files/bios.asm">bios.asm</a></td>
	<td width="10"></td>
	<td width="400">Source code for the bios</td>
</tr>
<tr>
	<td width="250" align="right"><a href="files/kernel.asm">kernel.asm</a></td>
	<td width="10"></td>
	<td width="400">Source code for the operating system kernel</td>
</tr>
<tr>
	<td align="right"><a href="files/opcode_list.txt">opcode_list.txt</a></td>
	<td width="10"></td>
	<td width="400">List of CPU opcodes</td>
</tr>
<tr>
	<td align="right"><a href="files/TASM1.TAB">TASM1.TAB</a></td>
	<td width="10"></td>
	<td width="400">TASM table</td>
</tr>
<tr>
	<td align="right"><a href="files/main.sch">main.sch</a></td>
	<td width="10"></td>
	<td width="400">Eagle schematics</td>
</tr>
<tr>
	<td align="right"><a href="files/main.png">main.png</a></td>
	<td width="10"></td>
	<td width="400">Schematics .png image</td>
</tr>
<tr>
	<td align="right"><a href="files/VM.png">VM.png</a></td>
	<td width="10"></td>
	<td width="400">A simple pictorial description of virtual memory</td>
</tr>
<tr>
	<td align="right"><a href="files/TASMMAN.HTM">TASMMAN.HTM</a></td>
	<td width="10"></td>
	<td width="400">Manual for TASM</td>
</tr>
</table>

<br />

<center><a href="http://github.com/Pconst167/SOL1_MiniComputer">SOL1_MiniComputer Git Repository</a></center>

</td></tr>
</table>


<?php include 'incs/inc_epage.php'; ?>