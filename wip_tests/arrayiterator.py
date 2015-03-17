#!/usr/bin/env python

class ArrayIterator():
    def __init__(self, size_x, size_y, mode):
        self.size_x = size_x
        self.size_y = size_y
        self.mode = mode
        self.current = 0
        if mode == "spiral":
            self.current_length = 0
            self.length = 1
            self.current_x = (size_x-1) / 2
            self.current_y = (size_y-1) / 2
            self.dx = 1
            self.dy = 0
            self.iterations = 0
    def next(self):
        x = None
        if self.current >= self.size_x * self.size_y:
            return None, None
        if self.mode == "topleft":
            x = self.current % self.size_x
            y = self.current / self.size_x
        elif self.mode == "topright":
            x = self.size_x - (self.current % self.size_x) - 1
            y = self.current / self.size_x
        elif self.mode == "bottomleft":
            x = self.current % self.size_x
            y = self.size_y - (self.current / self.size_x) - 1
        elif self.mode == "bottomright":
            x = self.size_x - (self.current % self.size_x) - 1
            y = self.size_y - (self.current / self.size_x) - 1
        elif self.mode == "spiral":
            while x is None:
                if (self.current_x>=0) and (self.current_x<self.size_x) and (self.current_y>=0) and (self.current_y<self.size_y):
                    x = self.current_x
                    y = self.current_y
                if self.current_length == self.length:
                    # we're at a corner
                    self.iterations += 1
                    self.current_length = 0
                    # switch direction
                    self.dx, self.dy = -self.dy, self.dx
                if self.iterations == 2:
                    self.length += 1
                    self.iterations = 0
                self.current_length += 1
                self.current_x += self.dx
                self.current_y += self.dy
        self.current += 1
        return x, y

import sys

def main():
    size_x = int(sys.argv[1])
    size_y = int(sys.argv[2])
    mode = sys.argv[3]
    iterator = ArrayIterator(size_x, size_y, mode)
    current = 0
    array = [0] * (size_x*size_y)
    while True:
        x, y = iterator.next()
        if x is None:
            break
        current += 1
        array[y*size_x+x] = current
    for y in range(size_y):
        for x in range(size_x):
            print "%d\t" % array[y*size_x+x],
        print ""

main()

