#!/usr/bin/env python3
"""bitmap.py - Task 8: working Python implementation of the slot bitmap (1 bit per slot)."""
import sys
from array import array

class SlotBitmap:
    def __init__(self, nslots):
        self.n = nslots
        self.bits = bytearray((nslots + 7) // 8)       # 80,000 slots -> 10,000 bytes
    def _chk(self, i):
        if not 0 <= i < self.n: raise IndexError(f"slot {i} out of range 0..{self.n-1}")
    def occupy(self, i):  self._chk(i); self.bits[i >> 3] |=  (1 << (i & 7))
    def release(self, i): self._chk(i); self.bits[i >> 3] &= ~(1 << (i & 7)) & 0xFF
    def is_occupied(self, i): self._chk(i); return (self.bits[i >> 3] >> (i & 7)) & 1
    def count_occupied(self): return sum(bin(b).count("1") for b in self.bits)   # O(n/8)
    def first_free(self):
        for byte_i, b in enumerate(self.bits):
            if b != 0xFF:
                for k in range(8):
                    s = byte_i * 8 + k
                    if s < self.n and not (b >> k) & 1: return s
        return -1

if __name__ == "__main__":
    N = 80000
    bm = SlotBitmap(N)
    ints = array("i", [0]) * N                          # same data as a 4-byte-int array
    for i in range(0, N, 3): bm.occupy(i); ints[i] = 1
    bm.occupy(79999); ints[79999] = 1
    bm.release(0); ints[0] = 0
    print("slots                    :", N)
    print("int array payload bytes  :", ints.itemsize * len(ints))
    print("bitmap payload bytes     :", len(bm.bits))
    print("occupied (bitmap)        :", bm.count_occupied())
    print("occupied (int array)     :", sum(ints))
    print("slot 3 / 4 / 79999       :", bm.is_occupied(3), bm.is_occupied(4), bm.is_occupied(79999))
    print("first free slot          :", bm.first_free())
    print("saving                   :", f"{(1 - len(bm.bits) / (ints.itemsize * len(ints))) * 100:.2f} %")
    print("python", sys.version.split()[0])
