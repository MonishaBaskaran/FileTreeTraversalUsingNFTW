# FileTreeTraversalUsingNFTW
File tree traversal allows programmers to explore directories and files within a file system.

Write a C program dftw that performs actions based on various command line arguments as 
listed below: 
dftw -nf [root_dir] 
 Lists the count of all files present in the entire subtree rooted at root_dir 
 root_dir is a valid absolute path of a directory that belongs to the subtree rooted at ~ 
(home directory) 
Example: $dftw -nf /home/pranga/chapter2 should list the count of all files in the entire 
subtree rooted at /home/pranga/chapter2 , i.e not only the files in the chapter2 folder but 
also all of its subdirectories, further subdirectories(if any) , and so on. 
dftw -nd [root_dir] 
 Lists the count of all directories present in the entire subtree rooted at root_dir 
dftw -sf [root_dir] 
 Lists the size of all files (in bytes) present in the entire subtree rooted at root_dir 
dftw -cpx [ source_dir] [destination_dir] [file extension] 
Copy the entire subdirectory rooted at source_dir to destination_dir and do not delete the 
directory (and contents) rooted at source_dir. 
 Exclude files of type denoted by the file extension
 The sub-directory structure at the destination must match the subdirectory structure at 
the source. 
 file extension: .c, .txt, .pdf (one of the three) 
 If file extension is not provided, all files must be copied 
Example: $dftw -cpx /home/pranga/chapter2 /home/pranga/Documents .txt must copy 
the entire subdirectory rooted at chapter2 to Documents (minus the .txt files). 
dftw -mv [ source_dir] [destination_dir] 
Move the entire subdirectory directory rooted at source_dir to destination_dir and delete the 
directory (and contents) rooted at source_dir. 
Mandatory Requirement: 
You must use the system call nftw() that allows you to traverse a file tree. This system call 
will recursively visit all the files/directories present in the tree and will call you own 
function (a function that you pass as a parameter).
