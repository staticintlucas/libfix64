from mpmath import mp as _mp, mpf as _mpf
import numpy as _np
from numpy.polynomial.chebyshev import cheb2poly as _cheb2poly
from numpy.polynomial import Chebyshev as _Cheby

_mp.prec = 256

zero = _mp.zero
one = _mp.one
two = _mpf(2)
half = _mp.one / 2

pi = _mp.pi
e = _mp.e
log2e = 1 / _mp.ln(2)
log10e = _mp.log10(e)
ln2 = _mp.ln2
ln10 = _mp.ln10
pi_2 = pi / 2
pi_4 = pi / 4
c_1_pi = 1 / pi
c_2_pi = 2 / pi
c_2_sqrtpi = 2 / _mp.sqrt(pi)
sqrt2 = _mp.sqrt(2)
sqrt1_2 = sqrt2 / 2

def chebyshev_coefs(func, n_coef):

    def impl():
        mpcos = _np.vectorize(_mp.cos)
        vfunc = _np.vectorize(func)

        N = _mpf(n_coef)
        k = _np.arange(N, dtype=_mpf)
        n = _np.arange(N, dtype=_mpf)

        xk = mpcos(pi * (k + half) / N)
        an = (((two - (n == 0)) / N) *
            _np.sum(mpcos(_np.outer(n * pi, k + half) / N) * vfunc(xk), axis=1))

        return _cheb2poly(an)[::-1] # reverse to make C implementation slightly simpler

    return impl
