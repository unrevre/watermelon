#!/usr/bin/env python
# pylint: disable=missing-docstring,invalid-name

import re
import textwrap

BITS = 128

FILES = 9
RANKS = 10
SENTINEL = 1
OFFSET = 9

WIDTH = FILES + 2 * SENTINEL
HEIGHT = RANKS
POINTS = WIDTH * HEIGHT + 2 * OFFSET

class OutsideBoard(Exception):
    def __init__(self, index):
        message = '[{}] not a valid board index'.format(index)
        super().__init__(message)
        self.index = index


def coordinates(index):
    x = (index - OFFSET) % WIDTH - SENTINEL
    y = (index - OFFSET) // WIDTH
    if 0 <= x < FILES and 0 <= y < RANKS:
        return x, y
    raise OutsideBoard(index)

def point(x, y):
    return y * WIDTH + x + OFFSET

def index(x, y):
    return y * WIDTH + x + SENTINEL + OFFSET


class Mask():
    def __init__(self):
        self.bits = None
        self.clear()

    def __str__(self):
        strrep = ''.join((str(b) for b in self.bits[::-1]))
        hexrep = (hex(int(strrep[:64], 2)), hex(int(strrep[64:], 2)))
        return '(((__uint128_t){}) << 64) + {}'.format(hexrep[0], hexrep[1])

    def clear(self):
        self.bits = bytearray(128)

    def fill(self, index, count, value):
        for i in range(count):
            self.bits[index + i] = value


def main():
    mask = Mask()

    # templates
    typename = 'const __uint128_t'
    attr = ' __attribute__((aligned(64)))'

    class format():
        def __init__(self, f, identifier):
            self.f = f
            self.identifier = identifier

        def __call__(self, func):
            def wrapped(*args):
                self.f.write(self.identifier + ' =\n')
                args[0].clear()
                func(*args)
                self.f.write('   ' + str(*args))
                self.f.write(';\n\n')
            return wrapped

    class format_array():
        def __init__(self, f, identifier, n):
            self.f = f
            self.identifier = identifier
            self.n = n

        def __call__(self, func):
            def wrapped(*args):
                self.f.write(self.identifier + ' = {\n')
                for i in range(self.n):
                    args[0].clear()
                    func(*args, i)
                    self.f.write('   ' + str(*args))
                    if i < self.n - 1:
                        self.f.write(',\n')
                    else:
                        self.f.write('\n}')
                self.f.write(';\n\n')
            return wrapped

    class valid_board_index():
        def __init__(self, func):
            self.func = func

        def __call__(self, *args):
            try:
                coordinates(args[1])
                self.func(*args)
            except OutsideBoard:
                pass

    # generate source (.c) file
    output = 'masks.c'

    with open(output, 'a') as f:
        f.write(textwrap.dedent(
            """
            /* auto-generated by maskgen.py */
            #include "masks.h"

            """))

        # board mask    [BMASK]
        @format(f, '{} BMASK'.format(typename))
        def etch_board_mask(mask):
            for i in range(BITS):
                try:
                    mask.fill(index(*coordinates(i)), 1, 1)
                except OutsideBoard:
                    pass

        etch_board_mask(mask)

        # outer masks   [OMASK]
        @format(f, '{} OMASK'.format(typename))
        def etch_outer_mask(mask):
            for i in range(BITS):
                try:
                    coordinates(i)
                except OutsideBoard:
                    mask.fill(i, 1, 1)

        etch_outer_mask(mask)

        # point masks   [PMASK]
        @format_array(f, '{} PMASK[POINTS]{}'.format(typename, attr), POINTS)
        def etch_point_masks(mask, i):
            mask.fill(i, 1, 1)

        etch_point_masks(mask)

        # upper masks   [UMASK]
        @format_array(f, '{} UMASK[POINTS]{}'.format(typename, attr), POINTS)
        def etch_upper_masks(mask, i):
            mask.fill(i + 1, BITS - 1 - i, 1)

        etch_upper_masks(mask)

        # lower masks   [LMASK]
        @format_array(f, '{} LMASK[POINTS]{}'.format(typename, attr), POINTS)
        def etch_lower_masks(mask, i):
            mask.fill(0, i, 1)

        etch_lower_masks(mask)

        # rank masks    [RMASK]
        @format_array(f, '{} RMASK[POINTS]{}'.format(typename, attr), POINTS)
        @valid_board_index
        def etch_rank_masks(mask, i):
            mask.fill(index(0, (i - OFFSET) // WIDTH), FILES, 1)

        etch_rank_masks(mask)

        # file masks    [FMASK]
        @format_array(f, '{} FMASK[POINTS]{}'.format(typename, attr), POINTS)
        @valid_board_index
        def etch_file_masks(mask, i):
            for y in range(RANKS):
                mask.fill(point((i - OFFSET) % WIDTH, y), 1, 1)

        etch_file_masks(mask)

        # jiang mask    [JMASK]
        @format_array(f, '{} JMASK[2]'.format(typename), 2)
        def etch_jmasks(mask, side):
            h = HEIGHT - 3 if side else 0
            for y in range(h, h + 3):
                mask.fill(index(3, y), 3, 1)

        etch_jmasks(mask)

        # shi mask      [SMASK]
        @format_array(f, '{} SMASK[2]'.format(typename), 2)
        def etch_smasks(mask, side):
            h = HEIGHT - 3 if side else 0
            for x, y in ((3, h), (5, h), (4, h + 1), (3, h + 2), (5, h + 2)):
                mask.fill(index(x, y), 1, 1)

        etch_smasks(mask)

        # xiang mask    [XMASK]
        @format_array(f, '{} XMASK[2]'.format(typename), 2)
        def etch_xmasks(mask, side):
            h = HEIGHT - 5 if side else 0
            for x, y in ((2, h), (6, h), (0, h + 2), (4, h + 2), (8, h + 2),
                         (2, h + 4), (6, h + 4)):
                mask.fill(index(x, y), 1, 1)

        etch_xmasks(mask)

        # zu mask       [ZMASK]
        @format_array(f, '{} ZMASK[2]'.format(typename), 2)
        def etch_zmasks(mask, side):
            h = HEIGHT - 5 if side else 3
            for y in range(h, h + 2):
                for x, y in ((0, y), (2, y), (4, y), (6, y), (8, y)):
                    mask.fill(index(x, y), 1, 1)

            h = 0 if side else HEIGHT - 5
            for y in range(h, h + 5):
                mask.fill(index(0, y), FILES, 1)

        etch_zmasks(mask)

    # generate header (.h) file
    output = re.sub(r'.c$', '.h', output)

    with open(output, 'a') as f:
        f.write(textwrap.dedent(
            """
            /* auto-generated by maskgen.py */
            #ifndef MASKS_H
            #define MASKS_H

            #include "magics.h"

            extern {0} BMASK;
            extern {0} OMASK;

            extern {0} PMASK[POINTS];
            extern {0} UMASK[POINTS];
            extern {0} LMASK[POINTS];
            extern {0} RMASK[POINTS];
            extern {0} FMASK[POINTS];

            extern {0} JMASK[2];
            extern {0} SMASK[2];
            extern {0} XMASK[2];
            extern {0} ZMASK[2];

            #endif /* MASKS_H */
            """.format(typename)))

if __name__ == '__main__':
    main()
