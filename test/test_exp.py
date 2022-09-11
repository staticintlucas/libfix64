from . import *

args = fix_range(-30, 30, 1e5)

def test_exp2(libfix64):
    for arg in args:
        expected = 2 ** fix2flt(arg)
        result = libfix64.fix64_exp2(arg)

        assert isclose(fix2flt(result), expected), f"arg = {arg}"
