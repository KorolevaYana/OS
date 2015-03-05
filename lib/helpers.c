#include <stdio.h>
#include "helpers.h"

ssize_t read_(int fd, void *buf, size_t count) {
  ssize_t result = 0;
  while (1) {
    ssize_t tmp = read(fd, buf + result, count - result);
		if (tmp == -1)
      return -1;
    result += tmp;
		if (result == count || tmp == 0) {
      return result;
		}
  }
}

ssize_t write_(int fd, const void *buf, size_t count) {
  ssize_t result = 0;
  while (1) {
    ssize_t tmp = write(fd, buf + result, count - result);
    if (tmp == -1)
      return -1;
    result += tmp;
    if (result == count) 
      return result;
  }
}
