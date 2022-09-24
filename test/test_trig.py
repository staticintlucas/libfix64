import math

from . import *

def test_sin(libfix64):
    angles = fix_range(-2 * math.pi, 2 * math.pi, 1e5)
    for angle in angles:
        expected = math.sin(angle)
        result = Fix64.from_repr(libfix64.fix64_sin(angle.repr))

        assert Fix64.approx_eq(result, expected), f"angle = {angle}"

def test_cos(libfix64):
    angles = fix_range(-2 * math.pi, 2 * math.pi, 1e5)
    for angle in angles:
        expected = math.cos(angle)
        result = Fix64.from_repr(libfix64.fix64_cos(angle.repr))

        assert Fix64.approx_eq(result, expected), f"angle = {angle}"

def test_tan(libfix64):
    angles = fix_range(-2 * math.pi, 2 * math.pi, 1e5)
    for angle in angles:
        expected = math.tan(angle)
        result = Fix64.from_repr(libfix64.fix64_tan(angle.repr))

        assert Fix64.approx_eq(result, expected), f"angle = {angle}"
