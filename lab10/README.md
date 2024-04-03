# Cmpt360-L10-Project
## DESCRIPTION

This program finds a specific file (with a certain pattern) in a system.

## AUTHORS

Salah Mohamed & Vy Tran

## INSTALLATION
To make a lab10 executable file, type **make** in the command line  
To make a glob test executable file, type **make globtest** in the command line  

To run ./lab10, type ./lab10 find [options]  
To run globtest, type ./globtest

## Warning
By using glob.h and regex.h, there will be some differences in
syntaxes/patterns.  

Example:  

filename1 = "\\.c$"; 		// For pattern = *.c
filename2 = ".ap\\.c"; 		// For pattern = ?ap.c
filename3 = "[c]*p\\.c$"; 	// For pattern = [c]*p.c
filename4 = "^[^c]*\\.c$"; 	// For pattern = [^c]*.c
filename5 = "code.*\\.c"; 	// For pattern = code*.c
filename6 = "co.e\\.c"; 	// For pattern = co?e.c  
  
If you intend to find "code.c", then on the command line, it should be:

find . -name co.e\\.c 		// For pattern = co?e.c; only 1 character  
  

## For more information, please contact us via emails (if needed)  
Salah Mohamed: mohameds36@mymacewan.ca  
Vy Tran: tranv33@mymacewan.ca

