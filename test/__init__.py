import math
import pytest
from ctypes import CDLL, c_int64

from pathlib import Path

HERE = Path(__file__).absolute().parent
ROOT = HERE.parent
OUT_DIR = ROOT / "output"
OBJ_DIR = OUT_DIR / "build"
TEST_SO = OBJ_DIR / "libfix64_tests.so"


FIX64_MAX = (1 << 63) - 1
FIX64_MIN =  -(1 << 63)

def flt2fix(x):
    result = int(round(x * (1 << 32)))
    return FIX64_MAX if result > FIX64_MAX else FIX64_MIN if result < FIX64_MIN else result

def fix2flt(x):
    return float(x) / (1 << 32)

def sat_flt(x):
    if x > fix2flt(FIX64_MAX):
        x = fix2flt(FIX64_MAX)
    elif x < fix2flt(FIX64_MIN):
        x = fix2flt(FIX64_MIN)
    return x

def fix_range(start, stop, n):
    step = (stop - start) / (n - 1)
    for i in range(int(n)):
        yield flt2fix(start + step * i)

def isclose(a, b):
    return math.isclose(a, b, rel_tol=1e-15, abs_tol=(1 / (1 << 32)))


@pytest.fixture
def libfix64():
    lib = CDLL(TEST_SO)
    lib.fix64_sin.argtypes = [c_int64]
    lib.fix64_sin.restype = c_int64
    lib.fix64_cos.argtypes = [c_int64]
    lib.fix64_cos.restype = c_int64
    lib.fix64_tan.argtypes = [c_int64]
    lib.fix64_tan.restype = c_int64

    lib.fix64_exp2.argtypes = [c_int64]
    lib.fix64_exp2.restype = c_int64
    yield lib
