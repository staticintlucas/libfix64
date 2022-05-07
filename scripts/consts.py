from decimal import Decimal as _D, getcontext as _ctx

def pi():
    # Compute pi to the current precision.
    # Taken from https://docs.python.org/3/library/decimal.html

    _ctx().prec += 2 # extra digits for intermediate steps
    three = _D(3)
    lasts, t, s, n, na, d, da = 0, three, 3, 1, 0, 0, 24
    while s != lasts:
        lasts = s
        n, na = n+na, na+8
        d, da = d+da, da+32
        t = (t * n) / d
        s += t
    _ctx().prec -= 2
    return +s # unary plus applies the new precision

def e():
    # Compute e to the current precision.
    # Adapted from https://docs.python.org/3/library/decimal.html
    _ctx().prec += 2
    i, lasts, s, fact = 0, 0, 1, _D(1)
    while s != lasts:
        lasts = s
        i += 1
        fact /= i
        s += fact
    _ctx().prec -= 2
    return +s

e = e()
log2e = _D.log10(e) / _D.log10(_D(2))
log10e = _D.log10(e)
ln2 = _D.ln(_D(2))
ln10 = _D.ln(_D(10))
pi = pi()
pi_2 = pi / 2
pi_4 = pi / 4
c_1_pi = _D(1) / pi
c_2_pi = _D(2) / pi
c_2_sqrtpi = _D(2) / _D.sqrt(pi)
sqrt2 = _D.sqrt(_D(2))
sqrt1_2 = sqrt2 / 2

def cordic_trig_k(n):
    k = _D(1)
    for i in range(n):
        k *= _D(1) / _D.sqrt(_D(1) + _D(2) ** _D(-2 * i))
    return k

def cordic_trig_atan(n):
    def atan(x):
        # Only works for range 0..1
        _ctx().prec += 2 # extra digits for intermediate steps
        s, lasts, d, m, sign = x, 0, 3, x, -1
        while s != lasts:
            m = m * x * x
            lasts = s
            s += sign * m / d
            sign = -sign
            d += 2
        _ctx().prec -= 2
        return +s

    return atan(_D(2) ** _D(-n)) / (pi / _D(2))
