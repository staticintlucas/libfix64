#!/usr/bin/env python3

from genericpath import isfile

import sys
import subprocess
from pathlib import Path
from zipfile import ZipFile

HERE = Path(__file__).absolute().parent
SRC_DIR = HERE / "src"
OUT_DIR = HERE / "output"
SCRIPTS = HERE / "scripts"

def main(build=False, zip=False):
    # Check required modules are installed
    try:
        import jinja2 as _
    except ImportError as e:
        not_installed_error("jinja2")
    try:
        import mpmath as _
    except ImportError as e:
        not_installed_error("mpmath")

    # We can do this safely after checking requirements
    from scripts import jinja

    try:
        # Copy license to output
        with open(HERE / "LICENSE", "r") as src:
            with open(OUT_DIR / "LICENSE", "w") as dest:
                dest.write(src.read())

        # Copy Makefile
        with open(SRC_DIR / "Makefile", "r") as src:
            with open(OUT_DIR /  "Makefile", "w") as dest:
                dest.write(src.read())

        # Render Jinja files
        for src in SRC_DIR.glob("*.jinja"):
            if not src.is_file():
                continue
            dest_path = OUT_DIR / src.stem
            with open(dest_path, "w") as dest:
                dest.write(jinja.render(src))

        # Zip up output
        if zip:
            with ZipFile("fix64.zip", 'w') as zf:
                zf.write(OUT_DIR, "fix64")
                for f in OUT_DIR.iterdir():
                    if f.is_file():
                        zf.write(f, str("fix64" / f.relative_to(OUT_DIR)))
    except OSError as e:
        bad_path_error(e.filename, e.strerror)

    # Build output
    if build:
        result = subprocess.run("make", cwd=OUT_DIR)
        if result.returncode:
            sys.exit(result.returncode)

# Error handling functions
def not_installed_error(name):
    print(
        f"\x1b[1;31mError:\x1b[0m Python module \x1b[1m{name}\x1b[0m is not installed!",
        "",
        "\x1b[30mTo install all required modules run:\x1b[0m",
        "    \x1b[1mpython3 -m pip install -r requirements.txt\x1b[0m",
        "",
        f"\x1b[30mOr to install just \x1b[1m{name}\x1b[0;30m run:\x1b[0m",
        f"    \x1b[1mpython3 -m pip install {name}\x1b[0m",
        "",
        f"\x1b[30mAlternatively, you may be able to install \x1b[1m{name}\x1b[0;30m through your "
            "system's package manager\x1b[0m",
        sep="\n", file=sys.stderr
    )
    sys.exit(1)

def bad_path_error(path, reason):
    print(
        f"Error:\x1b[0m Failed to open \x1b[1m{path}\x1b[0m:",
        "",
        f"    \x1b[1m{reason}\x1b[0m",
        "",
        "\x1b[30mEnsure this script is run in a \x1b[1mfix64\x1b[0;30m source directory\x1b[0m",
        "",
        "\x1b[30mThe latest release can be found at:\x1b[0m",
        "    \x1b[1;30mhttps://github.com/staticintlucas/fix64/releases\x1b[0m",
        sep="\n", file=sys.stderr
    )
    sys.exit(1)

def some_other_error():
    import traceback
    traceback.print_exc()
    print(
        "",
        "\x1b[1;31mError:\x1b[0m An Python error occurred while generating sources\x1b[0m",
        "",
        "Please report this error at:",
        "    \x1b[1mhttps://github.com/staticintlucas/fix64/issues\x1b[0m",
        sep="\n", file=sys.stderr
    )
    sys.exit(1)

if __name__ == "__main__":
    try:
        args = [arg.lstrip("-") for arg in sys.argv[1:]]

        if "help" in args:
            print(
                f"Usage: {sys.argv[0]} [--build] [--zip]",
                "",
                "Generates C header and source files for fix64",
                "",
                "Options:",
                "  --build               Also build the library from generated sources",
                "  --zip                 Output a zip file containing generated sources",
                sep="\n"
            )
            sys.exit(0)

        build = "build" in args
        zip = "zip" in args or "archive" in args
        main(build, zip)

    except (KeyboardInterrupt, SystemExit):
        raise
    except:
        some_other_error()
