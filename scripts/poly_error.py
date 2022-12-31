#!/usr/bin/env python3

from jinja import ARGS

import matplotlib.pyplot as _plt
from mpmath import mp as _mp
import sympy as _sp
from sympy.abc import x as _x

polys = ARGS["poly"]
n_plots = len(polys)
x_plots = int(_mp.ceil(_mp.sqrt(n_plots)))
y_plots = int(_mp.ceil(n_plots / x_plots))

for i, (name, poly) in enumerate(polys.items()):
    efunc = poly.poly().as_expr() - poly.func
    lefunc = _sp.lambdify(_x, efunc, "mpmath")
    xpoints = _mp.linspace(poly.ival[0], poly.ival[1], 1000)
    ypoints = [lefunc(x) for x in xpoints]

    _plt.subplot(x_plots, y_plots, i+1)
    _plt.plot(xpoints, ypoints)
    _plt.title(f"error for ${poly.name}$")
    _plt.xlabel("$x$")
    _plt.ylabel("$|e|$")

_plt.tight_layout()
_plt.show()
