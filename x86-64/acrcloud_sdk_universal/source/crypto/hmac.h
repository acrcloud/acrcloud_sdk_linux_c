/* hmac.h -- hashed message authentication codes
   Copyright (C) 2005, 2009-2012 Free Software Foundation, Inc.

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

/* Written by Simon Josefsson.  */

#ifndef HMAC_H
# define HMAC_H 1

#include <stddef.h>

/* Compute Hashed Message Authentication Code with MD5, as described
   in RFC 2104, over BUFFER data of BUFLEN bytes using the KEY of
   KEYLEN bytes, writing the output to pre-allocated 16 byte minimum
   RESBUF buffer.  Return 0 on success.  */
int
hmac_md5 (const void *key, size_t keylen,
          const void *buffer, size_t buflen, void *resbuf);

/* Compute Hashed Message Authentication Code with SHA-1, over BUFFER
   data of BUFLEN bytes using the KEY of KEYLEN bytes, writing the
   output to pre-allocated 20 byte minimum RESBUF buffer.  Return 0 on
   success.  */
int
hmac_sha1 (const void *key, size_t keylen,
           const void *in, size_t inlen, void *resbuf);

#endif /* HMAC_H */
