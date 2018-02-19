/*
  File        : letters.h
  Description : APES Homework 3

  Letter counting text analyzer worker declarations

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw3
*/

#ifndef _LETTERS_H
#define _LETTERS_H

// Filename to analyze for letter counts
#define LETTER_FILE "Valentinesday.txt"

// Letter counts that match this target will be reported in the log
#define LETTER_COUNT_TARGET 3

// Funciton: letter_counter
//
// This is the entry function of the letter counting worker thread.
//
// The worker reads through a text file (configured via LETTER_FILE above),
// counting each alphabetic character as it goes, ignoring case. Once the file
// has been completely read, any letter counts which match LETTER_COUNT_TARGET
// are reported to the log.
//
// Param[in] data - A pointer to the partially-initialized thread_info_t for this thread
void *letter_counter( void *data );

#endif /* _LETTERS_H */
