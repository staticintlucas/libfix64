#!/usr/bin/env python3

from pathlib import Path
import pkg_resources
from pkg_resources import DistributionNotFound, VersionConflict
import sys

HERE = Path(__file__).absolute().parent
ROOT = HERE.parent
REQUIREMENTS = ROOT / "requirements.txt"

reqs_ok = True

def strip_prefix(str, pfx):
    if str.startswith(pfx):
        return str[len(pfx):]
    return str

with open(REQUIREMENTS, "r") as file:
    for req in pkg_resources.parse_requirements(file):
        try:
            pkg_resources.require(str(req))

        except VersionConflict as e:
            reqs_ok = False
            specs = ",".join("".join(r) for r in e.req.specs)
            msg = (
                f'Could not find Python3 package "{e.req.project_name}": Found unsuitable version '
                f'"{e.dist.version}", but "{specs}" is required'
            )
            if hasattr(e, "required_by"):
                msg += f' by package "{", ".join(e.required_by)}"'

            print(msg, sep="\n", file=sys.stderr)

        except DistributionNotFound as e:
            reqs_ok = False
            specs = ",".join("".join(r) for r in e.req.specs)
            msg = (
                f'Could not find Python3 package "{e.req.project_name}": Package is missing, but '
                f'"{specs}" is required')
            if e.requirers:
                msg += f' by package "{", ".join(e.requirers)}"'

            print(msg, sep="\n", file=sys.stderr)

sys.exit(0 if reqs_ok else 1)
