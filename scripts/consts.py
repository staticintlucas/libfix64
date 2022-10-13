from mpmath import mp as _mp, mpf as _mpf

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
        cheby_poly = [
            [1] + [0] * (n_coef - 1),
            [0, 1] + [0] * (n_coef - 2),
        ]
        for _ in range(len(cheby_poly), n_coef):
            cheby_poly.append([2 * a - b for a, b in zip([0, *cheby_poly[-1]], cheby_poly[-2])])

        xk = [_mp.cos(pi * (k + half) / n_coef) for k in range(n_coef)]
        an = [((two - int(n == 0)) / n_coef) *
            sum(_mp.cos(n * pi * (k + half) / n_coef) * func(xk[k]) for k in range(n_coef))
                for n in range(len(cheby_poly))]

        coef = [_mp.zero] * n_coef
        for n in range(len(cheby_poly)):
            for i in range(len(coef)):
                coef[i] += an[n] * cheby_poly[n][i]

        return coef

    return impl
