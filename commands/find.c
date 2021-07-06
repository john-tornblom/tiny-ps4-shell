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

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <signal.h>


/**
 * global state variable
 **/
static int interupted = 0;
static int xdev_flag;
static dev_t xdev_device;
static long file_size;
static const char *file_pattern;
static const char *file_type;


/**
 * Catch user interupts
 **/
static void
on_SIGINT(int val) {
  interupted = 1;
}


static int
match(const char * text, const char * pattern) {
  const char *retry_pat;
  const char *retry_text;
  int ch;
  int found;

  retry_pat = NULL;
  retry_text = NULL;

  while (*text || *pattern) {
    ch = *pattern++;

    switch (ch) {
    case '*':  
      retry_pat = pattern;
      retry_text = text;
      break;

    case '[':  
      found = 0;

      while((ch = *pattern++) != ']') {
	if(ch == '\\') {
	  ch = *pattern++;
	}

	if(ch == '\0') {
	  return 0;
	}

	if(*text == ch) {
	  found = 1;
	}
      }

      if(!found) {
	pattern = retry_pat;
	text = ++retry_text;
      }

      /* fall into next case */

    case '?':  
      if(*text++ == '\0') {
	return 0;
      }
      break;

    case '\\':  
      ch = *pattern++;
      if(ch == '\0') {
	return 0;
      }

      /* fall into next case */

    default:        
      if(*text == ch) {
	if(*text) {
	  text++;
	}
	break;
      }

      if(*text) {
	pattern = retry_pat;
	text = ++retry_text;
	break;
      }
      return 0;
    }

    if(!pattern) {
      return 0;
    }

  }
  return 1;
}



/**
 *
 **/
static int
test_file(const char * full_name, const struct stat * statbuf) {
  const char * cp;
  const char * entry_name;
  int want_type;
  int mode;

  mode = statbuf->st_mode;

  if(file_type != NULL) {
    want_type = 0;
    for (cp = file_type; *cp; cp++) {
      switch(*cp) {
      case 'f':
	if(S_ISREG(mode)) {
	  want_type = 1;
	}
	break;
	
      case 'd':
	if(S_ISDIR(mode)) {
	  want_type = 1;
	}
	break;

      case 'p':
	if(S_ISFIFO(mode)) {
	  want_type = 1;
	}
	break;

      case 'c':
	if(S_ISCHR(mode)) {
	  want_type = 1;
	}
	break;

      case 'b':
	if(S_ISBLK(mode)) {
	  want_type = 1;
	}
	break;

      case 's':
	if(S_ISSOCK(mode)) {
	  want_type = 1;
	}
	break;

      case 'l':
	if(S_ISLNK(mode)) {
	  want_type = 1;
	}
	break;

      default:
	break;
      }
    }
    
    if(!want_type) {
      return 0;
    }
  }

  if(file_size > 0) {
    if(!S_ISREG(mode) && !S_ISDIR(mode)) {
      return 0;
    }

    if(statbuf->st_size < file_size) {
      return 0;
    }
  }

  if(file_pattern) {
    if((entry_name = strrchr(full_name, '/'))) {
      entry_name++;
    } else {
      entry_name = full_name;
    }
    if(!match(entry_name, file_pattern)) {
      return 0;
    }
  }

  return 1;
}


/*
 *
 */
static void
examine_directory(const char * path) {
  DIR *dir;
  int need_slash;
  struct dirent *entry;
  struct stat statbuf;
#define	MAX_NAME_SIZE (1024 * 10)
  char full_name[MAX_NAME_SIZE];

  if(!(dir = opendir(path))) {
    fprintf(stderr, "Cannot read directory \"%s\": %s\n",
	    path, strerror(errno));
    return;
  }

  need_slash = (*path && (path[strlen(path) - 1] != '/'));
  while(!interupted && (entry = readdir(dir))) {
    if ((strcmp(entry->d_name, ".") == 0) ||
	(strcmp(entry->d_name, "..") == 0)) {
      continue;
    }

    strcpy(full_name, path);
    if (need_slash) {
      strcat(full_name, "/");
    }
    strcat(full_name, entry->d_name);

    if (lstat(full_name, &statbuf) < 0) {
      fprintf(stderr, "Cannot stat \"%s\": %s\n",
	      full_name, strerror(errno));
      continue;
    }

    if (test_file(full_name, &statbuf)) {
      printf("%s\n", full_name);
    }

    if(S_ISDIR(statbuf.st_mode) &&
       (!xdev_flag || (statbuf.st_dev == xdev_device))) {
      examine_directory(full_name);
    }
  }
  
  closedir(dir);
}



/**
 *
 **/
int
main_find(int argc, const char ** argv) {
  const char *cp;
  const char *path;
  struct stat statbuf;

  signal(SIGINT, on_SIGINT);
  
  argc--;
  argv++;

  xdev_flag = 0;
  file_type = NULL;
  file_pattern = NULL;
  file_size = 0;

  if((argc <= 0) || (**argv == '-')) {
    fprintf(stderr, "No path specified\n");
    return 1;
  }

  path = *argv++;
  argc--;

  while(argc > 0) {
    argc--;
    cp = *argv++;

    if(strcmp(cp, "-xdev") == 0) {
      xdev_flag = 1;
      
    } else if(strcmp(cp, "-type") == 0) {
      if ((argc <= 0) || (**argv == '-')) {
	fprintf(stderr, "Missing type string\n");
	return 1;
      }

      argc--;
      file_type = *argv++;
      
    } else if(strcmp(cp, "-name") == 0) {
      if((argc <= 0) || (**argv == '-')) {
	fprintf(stderr, "Missing file name\n");
	return 1;
      }

      argc--;
      file_pattern = *argv++;
    } else if(strcmp(cp, "-size") == 0) {
      if ((argc <= 0) || (**argv == '-')) {
	fprintf(stderr, "Missing file size\n");
	return 1;
      }

      argc--;
      cp = *argv++;

      file_size = 0;

      while(isdigit(*cp)) {
	file_size = file_size * 10 + (*cp++ - '0');
      }
      
      if(*cp || (file_size < 0)) {
	fprintf(stderr, "Bad file size specified\n");
	return 1;
      }
    } else {
      if(*cp != '-') {
	fprintf(stderr, "Missing dash in option\n");
      } else {
	fprintf(stderr, "Unknown option\n");
      }
      return 1;
    }
  }
  
  if(stat(path, &statbuf) < 0) {
      fprintf(stderr, "Cannot stat \"%s\": %s\n", path, strerror(errno));
      return 1;
    }
  
  if(!S_ISDIR(statbuf.st_mode)) {
    fprintf(stderr, "Path \"%s\" is not a directory\n", path);    
    return 1;
  }
  
  xdev_device = statbuf.st_dev;

  if(test_file(path, &statbuf)) {
    printf("%s\n", path);
  }
  
  examine_directory(path);

  return 0;
}



