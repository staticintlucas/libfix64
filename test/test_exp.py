import math

from . import *

def test_exp(libfix64):
    args = fix_range(-3, 3, 1e5)
    for arg in args:
        expected = math.exp(fix2flt(arg))
        result = libfix64.fix64_exp(arg)

        assert isclose(fix2flt(result), expected), f"arg = {fix2flt(arg)}"

def test_exp2(libfix64):
    args = fix_range(-30, 30, 1e5)
    for arg in args:
        expected = 2 ** fix2flt(arg)
        result = libfix64.fix64_exp2(arg)

        assert isclose(fix2flt(result), expected), f"arg = {fix2flt(arg)}"

def test_log(libfix64):
    args = fix_log_range(1e-9, 1e9, 1e5)
    for arg in args:
        expected = math.log(fix2flt(arg))
        result = libfix64.fix64_log(arg)

        assert isclose(fix2flt(result), expected), f"arg = {fix2flt(arg)}"

def test_log10(libfix64):
    args = fix_log_range(1e-9, 1e9, 1e5)
    for arg in args:
        expected = math.log10(fix2flt(arg))
        result = libfix64.fix64_log10(arg)

        assert isclose(fix2flt(result), expected), f"arg = {fix2flt(arg)}"

def test_log2(libfix64):
    args = fix_log_range(1e-9, 1e9, 1e5)
    for arg in args:
        expected = math.log2(fix2flt(arg))
        result = libfix64.fix64_log2(arg)

        assert isclose(fix2flt(result), expected), f"arg = {fix2flt(arg)}"
