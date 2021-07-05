/* Copyright (C) 2021 John Törnblom

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3, or (at your option) any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING. If not, see
<http://www.gnu.org/licenses/>.  */

// Code inspired by http://members.tip.net.au/%7Edbell/programs/sash-3.8.tar.gz

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>


#define BUF_SIZE 1024*8

/**
 * state variable for catching user interupt signals
 **/
static int interupted = 0;


/**
 * Catch user interupts
 **/
static void
on_SIGINT(int val) {
  interupted = 1;
}



/*
 * See if the specified word is found in the specified string.
 */
static int
search(const char *string, const char *word, int ignore_case) {
  const char *cp1;
  const char *cp2;
  int len;
  int low_first;
  int ch1;
  int ch2;

  signal(SIGINT, on_SIGINT);
  
  len = strlen(word);
  
  if (!ignore_case) {
    while (1) {
      
      if(!(string = strchr(string, word[0]))) {
	return 0;
      }
      if(memcmp(string, word, len) == 0) {
	return 1;
      }
      string++;
    }
  }

  /*
   * Here if we need to check case independence.
   * Do the search by lower casing both strings.
   */
  low_first = *word;
  if (isupper(low_first)) {
    low_first = tolower(low_first);
  }
  
  while(1) {
    while (*string && (*string != low_first) &&
	   (!isupper(*string) || (tolower(*string) != low_first))) {
      string++;
    }

    if (*string == '\0') {
      return 0;
    }

    cp1 = string;
    cp2 = word;

    do {
      if (*cp2 == '\0') {
	return 1;
      }

      ch1 = *cp1++;

      if (isupper(ch1)) {
	ch1 = tolower(ch1);
      }
      
      ch2 = *cp2++;

      if (isupper(ch2)) {
	ch2 = tolower(ch2);
      }
    }
    while (ch1 == ch2);
    string++;
  }
}



/**
 *
 **/
int
main_grep(int argc, const char ** argv) {
  FILE * fp;
  const char* word;
  const char* name;
  const char* cp;
  int tell_name;
  int ignore_case;
  int tell_line;
  long line;
  char buf[BUF_SIZE];
  int r;

  if(argc <= 1) {
    fprintf(stderr, "%s: missing operand\n", argv[0]);
    return 1;
  }
  
  r = 1;
  ignore_case = 0;
  tell_line = 0;

  argc--;
  argv++;

  if(**argv == '-') {
    argc--;
    cp = *argv++;

    while(*++cp) {
      switch(*cp) {
      case 'i':
	ignore_case = 1;
	break;

      case 'n':
	tell_line = 1;
	break;
	
      default:
	fprintf(stderr, "Unknown option\n");
	return 1;
      }
    }
  }

  word = *argv++;
  argc--;

  tell_name = (argc > 1);

  while(argc-- > 0) {
    name = *argv++;

    if (!(fp = fopen(name, "r"))) {
      perror(name);
      r = 1;
      continue;
    }

    line = 0;
    
    while(fgets(buf, sizeof(buf), fp)) {
      if(interupted) {
	fclose(fp);
	return 1;
      }

      line++;

      cp = &buf[strlen(buf) - 1];
      if (*cp != '\n') {
	fprintf(stderr, "%s: Line too long\n", name);
      }

      if(search(buf, word, ignore_case)) {
	r = 0;
	if(tell_name) {
	  printf("%s: ", name);
	}
	if(tell_line) {
	  printf("%ld: ", line);
	}
	fputs(buf, stdout);
      }
    }
    
    if (ferror(fp)) {
      perror(name);
    }

    fclose(fp);
  }
  
  return r;
}



