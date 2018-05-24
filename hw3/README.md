# Project 3: The Kernel Crypto API
Encrypted Block Device
Using the default yocto kernel as described in previous assignments, you are asked to implement the following:

Write a RAM Disk device driver for the Linux Kernel which allocates a chunk of memory and presents it as a block device.

Using the Linux Kernel's Crypto API, add encryption to your block device such that your device driver encrypts and decrypts data when it is written and read. You will need to look at examples in the kernel for how to do this.

This will be developed in the drivers/block directory. Remember, as this is a block cypher, you may have more data than fits in one block of the cypher. Further, you need to be able to set the key as a module parameter.

A link you may find useful: LDD3 (Links to an external site.)Links to an external site.. This is an implementation against an older kernel. You may find SBD and 3.x to be useful search terms.

Some useful notes:

1. You will need to do this as a module, in order to use a module parameter.
2. You will need to move the module in to the running VM. I recommend scp.
3. Debugging modules externally is...annoying. I recommend doing most of your debugging and testing with your block device built right in to the kernel. Then move it to a module to test your module paramters.
4. It is up to you to demonstrate correctness.
5. Create an ext2 file system on the block device (mkfs.ext2).

You are also required to do the following:

* Prepare a document containing the design you plan to use to implement the necessary algorithms.
* Version control log (formatted as a table) -- there are any number of tools for generating a TeX table from repo logs
* Work log. What was done when?
* Answer the following questions in sufficient detail:
..1. What do you think the main point of this assignment is?
..2. How did you personally approach the problem? Design decisions, algorithm, etc.
..3. How did you ensure your solution was correct? Testing details, for instance. Ensure this is written in a way that the TAs can follow to ensure correctness.
..4. What did you learn?

Make sure your submission is a tarball labelled CS444_project3_#.tar.bz2 and includes the following:

* A linux patch file containing your solution
* The write-up in LaTeX format.