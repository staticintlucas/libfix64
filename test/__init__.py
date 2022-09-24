import math
import pytest
from ctypes import CDLL, c_int64

from pathlib import Path

HERE = Path(__file__).absolute().parent
ROOT = HERE.parent
OUT_DIR = ROOT / "output"
OBJ_DIR = OUT_DIR / "build"
TEST_SO = OBJ_DIR / "libfix64_tests.so"

class Fix64Meta(type):
    @property
    def MAX(self):
        return self.from_repr(self._REPR_MAX)

    @property
    def MIN(self):
        return self.from_repr(self._REPR_MIN)

    @property
    def EPS(self):
        return self.from_repr(1)

class Fix64(object, metaclass=Fix64Meta):
    FRAC_BITS = 32
    INT_BITS = 32
    BITS = INT_BITS + FRAC_BITS

    _REPR_MAX = (1 << 63) - 1
    _REPR_MIN = -(1 << 63)

    def __init__(self, val):
        self._repr = int(round(val * (1 << Fix64.FRAC_BITS)))
        self._sat()

    @property
    def repr(self):
        return self._repr

    @repr.setter
    def repr(self, repr):
        self._repr = repr

    @classmethod
    def from_repr(cls, repr):
        result = cls(0)
        result._repr = repr
        return result

    def _sat(self):
        if self._repr > Fix64._REPR_MAX:
            self._repr = Fix64._REPR_MAX
        elif self._repr < Fix64._REPR_MIN:
            self._repr = Fix64._REPR_MIN

    def _overflow(self):
        self._repr &= (1 << (Fix64.BITS + 1)) - 1
        if self._repr & (1 << Fix64.BITS):
            self._repr -= 1 << (Fix64.BITS + 1)

    def __float__(self):
        return self.repr / (1 << Fix64.FRAC_BITS)

    def __int__(self):
        return self.repr // (1 << Fix64.FRAC_BITS)

    def __format__(self, format_spec):
        return float(self).__format__(format_spec)

    def __str__(self):
        return f"{self:.10f}"

    def __repr__(self):
        return f"Fix64({self:.10f})"

    def __eq__(self, other):
        return self.repr == other.repr

    def approx_eq(self, other):
        return math.isclose(self, other, rel_tol=1e-12, abs_tol=Fix64.EPS)

    def __lt__(self, other):
        return self.repr < other.repr

    def __gt__(self, other):
        return self.repr > other.repr

    def __le__(self, other):
        return self.repr <= other.repr

    def __ge__(self, other):
        return self.repr >= other.repr

    def __add__(self, other):
        return Fix64.from_repr(self.repr + other.repr)._overflow()

    def add_sat(self, other):
        return Fix64.from_repr(self.repr + other.repr)._sat()

    def __sub__(self, other):
        return Fix64.from_repr(self.repr - other.repr)._overflow()

    def sub_sat(self, other):
        return Fix64.from_repr(self.repr - other.repr)._sat()

    def __mul__(self, other):
        return Fix64.from_repr(float(self) * float(other))._overflow()

    def mul_sat(self, other):
        return Fix64.from_repr(float(self) * float(other))._sat()

    def __truediv__(self, other):
        return Fix64.from_repr(float(self) / float(other))._overflow()

    def div_sat(self, other):
        return Fix64.from_repr(float(self) / float(other))._sat()

    def __pow__(self, other, modulo=None):
        return Fix64.from_repr(pow(float(self), float(other), modulo))._sat()

    def __neg__(self):
        return Fix64.from_repr(-self.repr)._sat()

    def __pos__(self):
        return self

    def __abs__(self):
        return Fix64.from_repr(abs(self.repr))._sat()

    def __round__(self, ndigits=None):
        return Fix64.from_repr(round(float(self), ndigits))._sat()

    def __trunc__(self):
        return Fix64.from_repr(math.trunc(float(self)))._sat()

    def __floor__(self):
        return Fix64.from_repr(math.floor(float(self)))._sat()

    def __ceil__(self):
        return Fix64.from_repr(math.ceil(float(self)))._sat()

def _flt_range(start, stop, n):
    step = (stop - start) / (n - 1)
    for i in range(int(n)):
        yield start + (i * step)

def fix_range(start, stop, n):
    for f in _flt_range(start, stop, n):
        yield Fix64(f)

def fix_log_range(start, stop, n):
    logstart = math.log(start)
    logstop = math.log(stop)
    for f in _flt_range(logstart, logstop, n):
        yield Fix64(math.exp(f))


@pytest.fixture
def libfix64():
    lib = CDLL(TEST_SO)
    lib.fix64_sin.argtypes = [c_int64]
    lib.fix64_sin.restype = c_int64
    lib.fix64_cos.argtypes = [c_int64]
    lib.fix64_cos.restype = c_int64
    lib.fix64_tan.argtypes = [c_int64]
    lib.fix64_tan.restype = c_int64

    lib.fix64_exp.argtypes = [c_int64]
    lib.fix64_exp.restype = c_int64
    lib.fix64_exp2.argtypes = [c_int64]
    lib.fix64_exp2.restype = c_int64
    lib.fix64_log.argtypes = [c_int64]
    lib.fix64_log.restype = c_int64
    lib.fix64_log10.argtypes = [c_int64]
    lib.fix64_log10.restype = c_int64
    lib.fix64_log2.argtypes = [c_int64]
    lib.fix64_log2.restype = c_int64
    yield lib
