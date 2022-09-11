import math

from . import *

angles = fix_range(-2 * math.pi, 2 * math.pi, 1e5)

def test_sin(libfix64):
    for angle in angles:
        expected = math.sin(fix2flt(angle))
        result = libfix64.fix64_sin(angle)

        assert isclose(fix2flt(result), expected), f"angle = {angle}"

def test_cos(libfix64):
    for angle in angles:
        expected = math.cos(fix2flt(angle))
        result = libfix64.fix64_cos(angle)

        assert isclose(fix2flt(result), expected), f"angle = {angle}"

def test_tan(libfix64):
    for angle in angles:
        expected = sat_flt(math.tan(fix2flt(angle)))
        result = libfix64.fix64_tan(angle)

        assert isclose(fix2flt(result), expected), f"angle = {angle}"
