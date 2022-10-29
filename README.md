&copy; 2022 Andrei Napruiu & Andra Stoica 
(napruiuandrei@gmail.com & andrastoica1204@gmail.com)

# Simple Linux FileSystem

# Contents:
1. [Short Description](#description)
2. [Code overview](#overview-of-the-code)
3. [Other comments](#commentaries)
2. [Commands](#commands)
3. [How to run it](#how-to-run-it)
4. [How to check it](#how-to-check-it)

# Description:

* This homework is based on a simple concept, the Linux FileSystem and its
bash commands. Using this program we can simulate inside the terminal a
FileTree that can contain both files and directories. All the files are
connected with one another and can contain, based on thei type, text or
more files respectively.

* Using the data structures already implemented by the homework team, we
adjusted and created functions so that our work can be more easy to complete.

* Also, all the structures and variables are dynamically allocated so that
the program can work better and faster on every machine.

# Overview of the code

* We mainly explained all the functions inside the tree.c file so that we can
easily collaborate on this project.

* There are some recursive functions so that the files and folders
are freed correctly or only to print the absolute path to a file/folder.

* The variables in the code are pretty self explanatory, so we are hoping that
our code is very easy to understand

# Commentaries:

* Although there is always room for improvement, we honestly believe that
we did our best to be as efficient as possible, we tried avoiding duplicated
code, so we added extra functions to facilitate the process.

* In addition, we have learned how to implement and use better the tree
structure.

# Commands
Below is the list of every command (in some cases explained).

> Every command is set to retunr an allocation error if something went wrong with the memory allocation.

## touch \<filename> [filecontent]
* It creates the file \<filename> in the current directory.
* If the content is specified as an argument, the file created will contain the given text.

## ls [arg]
* It prints all the files and directories in the current directory.
* If the argument is a file, it prints its content.
* If the argument is a folder, it will print all the files inside it.

## mkdir <dirname>
* It creates a directory in the current directory with the \<dirname> name.

## cd \<path>
* Changes the current directory in the one specified in the path.

## tree [path]
* It prints all the resources in from the current directory and so on.
* If the path is specified, it will only print everything from the path and so on.

## pwd
* It prints the absolute path(from root) to the current directory.

## rmdir \<dirname>
* It will delete a directory only if it isempty. It can't affect files.

## rm \<filename>
* It will delete only specified files.

## rmrec \<resourcename>
* It will delete the resource given as argument, even tho it is a directory not empty.

## cp \<source_path> \<destination_path>
* It will copy the file from source path to destination path.
* If the destination is a directory, the file will be copied inside it.
* If the destination is a file that doesn't exist but can be created, this command will do so.

## mv \<source_path> \<destination_path>
* Same rules as the copy command, but the source file will be deleted and only the destination one will remain.

## How to run it
1. Download the files as shown in the repo;
2. Open a terminal and change the current directory with the one containing the code;
3. Run the following commands:
	- make
	- ./sd_fs
	- the set of commands and numbers/files/strings(data)

## Example
```bash
student@pc:~$ make
gcc -g -std=c99 main.c tree.c -o sd_fs
student@pc:~$ ./sd_fs
...(data)
```

> Note: you can run the code using valgrind to keep track of memory leaks(they don't exist I can assure you)

For valgrind run:
```bash
student@pc:~$ make
gcc -g -std=c99 main.c tree.c -o sd_fs
student@pc:~$ valgrind --leak-check=full -s ./sd_fs
...(data)
```

# How to check it
1. Download the files as shown in the repo;
2. Open a terminal and change the current directory with the one containing the code;
3. Run the following command:

```bash
student@pc:~$ ./check.sh
```

> You will be prompted with a lot of lines verifying a number of inputs for each command. All the inputs can be found in **tests/** folder.

---
Thank you!
