/*
  File        : metrics.h
  Description : APES Homework 3

  CPU utilization metrics reporter worker declaration.

  Author      : Jeff Schornick (jesc5667@colorado.edu)
  Version     : Version and history information is available at the GitHub repository:
                https://github.com/jschornick/ecen5013_apes/hw3
*/

#ifndef _METRICS_H
#define _METRICS_H

// How often our CPU metrics will be collected and written to the log
#define METRICS_INTERVAL_MS 100  /* milliseconds */

// Funciton: metrics_reporter
//
// This is the entry function of the CPU metrics worker thread.
//
// The worker sleeps between reports by using a combination of POSIX timer and
// sempahore. For each report, the user and kernel CPU utilization of the
// process are collected and reported to the log. Context switch information
// (voluntary and involuntary) is also included in the report.
//
// Param[in] data - A pointer to the partially-initialized thread_info_t for this thread
void *metrics_reporter( void *data );

#endif /* _METRICS_H */
