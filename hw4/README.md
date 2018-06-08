# Project 4: The SLOB SLAB
Using the default yocto kernel, you are asked to implement the following:

Understand how the SLOB first-fit alogorithm works and implement the best-fit allocation algorithm. You must also write a program that computes the effeciency of the first-fit algorithm and best-fit algorithm and compare the fragmentation sufferred by each algorithm. This will require the addition of a system call which returns actual memory usage.

This will be developed in the slob.c file in the mm directory of the linux kernel.

A few useful hints:

* Best fit has to be over all allocated pages, not just the current page.
* Pick a system call number that has never been used.
* Be very careful with your memory tracking. A SLOB is just a list of blocks. A simple one, in fact.
You are also required to do the following:

* Prepare a document containing the design you plan to use to implement the necessary algorithms. This should have a commit date PRIOR to any code.
* Version control log (formatted as a table) -- there are any number of tools for generating a TeX table from repo logs
* Work log. What was done when?
* Answer the following questions in sufficient detail:
  * What do you think the main point of this assignment is?
  * How did you personally approach the problem? Design decisions, algorithm, etc.
  * How did you ensure your solution was correct? Testing details, for instance.
  * What did you learn?
Make sure your submission is a tarball labelled CS411_project4_#.tar.bz2 and includes the following:

* A linux patch file containing your solution
* The write-up in LaTeX format.
* A makefile for your write-up