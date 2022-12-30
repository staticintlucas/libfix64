#!/usr/bin/env python3

from jinja import ARGS

import numpy as _np
import numpy.polynomial.polynomial as _poly
import matplotlib.pyplot as _plt
from mpmath import mp as _mp, mpf as _mpf

polys = ARGS["poly"]
n_plots = len(polys)
x_plots = int(_mp.ceil(_mp.sqrt(n_plots)))
y_plots = int(_mp.ceil(n_plots / x_plots))

for i, (name, poly) in enumerate(polys.items()):
    xpoints = _np.linspace(poly.ival[0], poly.ival[1], 1000, dtype=_mpf)
    ypoints = _poly.polyval(xpoints, poly.coefs()[::-1]) - _np.vectorize(poly.func)(xpoints)

    _plt.subplot(x_plots, y_plots, i+1)
    _plt.plot(xpoints, ypoints)
    _plt.title(f"error for ${poly.name}$")
    _plt.xlabel("$x$")
    _plt.ylabel("$|e|$")

_plt.tight_layout()
_plt.show()
