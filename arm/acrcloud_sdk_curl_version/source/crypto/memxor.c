/* memxor.c -- perform binary exclusive OR operation of two memory blocks.
   Copyright (C) 2005-2006, 2009-2012 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation; either version 2.1, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program; if not, see <http://www.gnu.org/licenses/>.  */

/* Written by Simon Josefsson.  The interface was inspired by memxor
   in Niels Möller's Nettle. */

#include "memxor.h"

void *
memxor (void * dest, const void * src, size_t n)
{
  char const *s = src;
  char *d = dest;

  for (; n > 0; n--)
    *d++ ^= *s++;

  return dest;
}
