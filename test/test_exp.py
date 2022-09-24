import math

from . import *

def test_exp(libfix64):
    args = fix_range(-3, 3, 1e5)
    for arg in args:
        expected = math.exp(arg)
        result = Fix64.from_repr(libfix64.fix64_exp(arg.repr))

        assert Fix64.approx_eq(result, expected), f"arg = {arg}"

def test_exp2(libfix64):
    args = fix_range(-30, 30, 1e5)
    for arg in args:
        expected = 2 ** float(arg)
        result = Fix64.from_repr(libfix64.fix64_exp2(arg.repr))

        assert Fix64.approx_eq(result, expected), f"arg = {arg}"

def test_log(libfix64):
    args = fix_log_range(1e-9, 1e9, 1e5)
    for arg in args:
        expected = math.log(arg)
        result = Fix64.from_repr(libfix64.fix64_log(arg.repr))

        assert Fix64.approx_eq(result, expected), f"arg = {arg}"

def test_log10(libfix64):
    args = fix_log_range(1e-9, 1e9, 1e5)
    for arg in args:
        expected = math.log10(arg)
        result = Fix64.from_repr(libfix64.fix64_log10(arg.repr))

        assert Fix64.approx_eq(result, expected), f"arg = {arg}"

def test_log2(libfix64):
    args = fix_log_range(1e-9, 1e9, 1e5)
    for arg in args:
        expected = math.log2(arg)
        result = Fix64.from_repr(libfix64.fix64_log2(arg.repr))

        assert Fix64.approx_eq(result, expected), f"arg = {arg}"
