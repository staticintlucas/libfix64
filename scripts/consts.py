from decimal import Decimal as _D, getcontext as _ctx
import math as _math
import sys

_ctx().prec = 64

pi = _D(3.1415926535897932384626433832795028841971693993751058209749445923)
e = _D(2.7182818284590452353602874713526624977572470936999595749669676277)
log2e = _D.log10(e) / _D.log10(_D(2))
log10e = _D.log10(e)
ln2 = _D.ln(_D(2))
ln10 = _D.ln(_D(10))
pi_2 = pi / 2
pi_4 = pi / 4
c_1_pi = _D(1) / pi
c_2_pi = _D(2) / pi
c_2_sqrtpi = _D(2) / _D.sqrt(pi)
sqrt2 = _D.sqrt(_D(2))
sqrt1_2 = sqrt2 / 2

def chebyshev_coefs(func, interval, n_coef):
    N = _D(n_coef)
    scaling = lambda x: (x + 1) / 2 * (interval[1] - interval[0]) + interval[0]
    sfunc = lambda x: func(scaling(x))

    cheby_poly = [
        [1] + [0] * (n_coef - 1),
        [0, 1] + [0] * (n_coef - 2),
    ]
    for _ in range(len(cheby_poly), n_coef):
        cheby_poly.append([2 * a - b for a, b in zip([0, *cheby_poly[-1]], cheby_poly[-2])])

    xk = [_D(_math.cos(pi * _D(k + 0.5) / N)) for k in range(n_coef)]
    an = [(_D(2 - int(n == 0)) / N) *
        sum(_D(_math.cos(n * pi * _D(k + 0.5) / N)) * _D(sfunc(xk[k])) for k in range(n_coef))
            for n in range(len(cheby_poly))]

    coef = [0] * n_coef
    for n in range(len(cheby_poly)):
        for i in range(len(coef)):
            coef[i] += an[n] * cheby_poly[n][i]

    return coef
