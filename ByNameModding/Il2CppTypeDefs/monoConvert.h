#pragma once

/* Based on mono src code
 * https://github.com/mono/mono
*/

#if defined (HOST_WIN32) || defined (_WIN32)
#include <wchar.h>
typedef wchar_t monoChar;
#else
typedef char16_t monoChar;
#endif
struct monoConvert {
private:
    static int decode_utf8(char *inbuf, size_t inleft, uint32_t *outchar) {
        unsigned char *inptr = (unsigned char *) inbuf;
        uint32_t u;
        int n, i;

        u = *inptr;

        if (u < 0x80) {
            *outchar = u;
            return 1;
        } else if (u < 0xc2) {
            return -1;
        } else if (u < 0xe0) {
            u &= 0x1f;
            n = 2;
        } else if (u < 0xf0) {
            u &= 0x0f;
            n = 3;
        } else if (u < 0xf8) {
            u &= 0x07;
            n = 4;
        } else if (u < 0xfc) {
            u &= 0x03;
            n = 5;
        } else if (u < 0xfe) {
            u &= 0x01;
            n = 6;
        } else {
            return -1;
        }

        if (n > inleft) {
            return -1;
        }

        for (i = 1; i < n; i++)
            u = (u << 6) | (*++inptr ^ 0x80);


        *outchar = u;

        return n;
    }

    static monoChar read_utf16(unsigned char *inptr) {
#if __LITTLE_ENDIAN
        return (inptr[1] << 8) | inptr[0];
#else
        return (inptr[0] << 8) | inptr[1];
#endif
    }

    static int decode_utf16(char *inbuf, size_t inleft, uint32_t *outchar) {
        unsigned char *inptr = (unsigned char *) inbuf;
        monoChar c;
        uint32_t u;

        if (inleft < 2) {
            return -1;
        }

        u = read_utf16(inptr);

        if (u < 0xd800) {
            *outchar = u;
            return 2;
        } else if (u < 0xdc00) {
            if (inleft < 4) {
                return -2;
            }

            c = read_utf16(inptr + 2);

            if (c < 0xdc00 || c > 0xdfff) {
                return -2;
            }

            u = ((u - 0xd800) << 10) + (c - 0xdc00) + 0x0010000UL;
            *outchar = u;

            return 4;
        } else if (u < 0xe000) {
            return -1;
        } else {
            *outchar = u;
            return 2;
        }
    }

    static int unicode_to_utf8(uint32_t c, char *outbuf) { // I think unichar is unicode char
        int base, n, i;

        if (c < 0x80) {
            base = 0;
            n = 1;
        } else if (c < 0x800) {
            base = 192;
            n = 2;
        } else if (c < 0x10000) {
            base = 224;
            n = 3;
        } else if (c < 0x200000) {
            base = 240;
            n = 4;
        } else if (c < 0x4000000) {
            base = 248;
            n = 5;
        } else if (c < 0x80000000) {
            base = 252;
            n = 6;
        } else {
            return -1;
        }

        if (outbuf != NULL) {
            for (i = n - 1; i > 0; i--) {
                outbuf[i] = (c & 0x3f) | 0x80;
                c >>= 6;
            }
            outbuf[0] = c | base;
        }

        return n;
    }

    static int unicode_to_utf16(uint32_t c, monoChar *outbuf) {
        uint32_t c2;

        if (c < 0xd800) {
            if (outbuf)
                *outbuf = (monoChar) c;

            return 1;
        } else if (c < 0xe000) {
            return -1;
        } else if (c < 0x10000) {
            if (outbuf)
                *outbuf = (monoChar) c;

            return 1;
        } else if (c < 0x110000) {
            if (outbuf) {
                c2 = c - 0x10000;

                outbuf[0] = (monoChar) ((c2 >> 10) + 0xd800);
                outbuf[1] = (monoChar) ((c2 & 0x3ff) + 0xdc00);
            }

            return 2;
        } else {
            return -1;
        }
    }

public:
    static monoChar *utf8_to_utf16(const char *str, long len) {
        monoChar *outbuf, *outptr;
        size_t outlen = 0;
        size_t inleft;
        char *inptr;
        uint32_t c;
        int u, n;
        if (str == NULL)
            return NULL;

        if (len < 0) {
            len = strlen(str);
        }

        inptr = (char *) str;
        inleft = len;

        while (inleft > 0) {
            if ((n = decode_utf8(inptr, inleft, &c)) < 0)
                return NULL;

            if (c == 0)
                break;

            if ((u = unicode_to_utf16(c, NULL)) < 0) {
                u = 2;
            }

            outlen += u;
            inleft -= n;
            inptr += n;
        }

        outptr = outbuf = (monoChar *) malloc((outlen + 1) * sizeof(monoChar));


        inptr = (char *) str;
        inleft = len;

        while (inleft > 0) {
            if ((n = decode_utf8(inptr, inleft, &c)) < 0)
                break;

            if (c == 0)
                break;

            u = unicode_to_utf16(c, outptr);
            if ((u < 0)) {
                outptr[0] = 0xFFFD;
                outptr[1] = 0xFFFD;
                u = 2;
            }

            outptr += u;
            inleft -= n;
            inptr += n;
        }

        *outptr = '\0';

        return outbuf;
    }

    static char *utf16_to_utf8(const monoChar *str, long len) {
        char *inptr, *outbuf, *outptr;
        size_t outlen = 0;
        size_t inleft;
        uint32_t c;
        int n;

        if (str == NULL)
            return NULL;

        if (len < 0) {
            len = 0;
            while (str[len])
                len++;
        }

        inptr = (char *) str;
        inleft = len * 2;

        while (inleft > 0) {
            if ((n = decode_utf16(inptr, inleft, &c)) < 0) {
                if (n == -2 && inleft > 2) {
                    inleft -= 2;
                    inptr += 2;
                }
                return NULL;
            } else if (c == 0)
                break;

            outlen += unicode_to_utf8(c, NULL);
            inleft -= n;
            inptr += n;
        }

        outptr = outbuf = (char *) malloc(outlen + 1);

        inptr = (char *) str;
        inleft = len * 2;

        while (inleft > 0) {
            if ((n = decode_utf16(inptr, inleft, &c)) < 0)
                break;
            else if (c == 0)
                break;

            outptr += unicode_to_utf8(c, outptr);
            inleft -= n;
            inptr += n;
        }

        *outptr = '\0';

        return outbuf;
    }
};
