#!/usr/bin/env python3
# Generates manifest.json catalog of kernel drivers from the Driver/ folder.
# Output: Driver/manifest.json (or --out path).
#
# Entry fields: { file, kernel, brand, model?, type, module? }
# type:
#   selfextract : .sh with embedded payload (run -> installs driver)
#   insmod      : needs module .ko + load.sh run via insmod
#   kpm         : .kpm ELF kernel module
#
# Kernel version is taken from the filename or any parent folder name.
# Non-driver / unparsable files are skipped (warnings printed).
import os
import re
import sys
import json

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "Driver")
OUT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "manifest.json")
PREFIX = "drivers"

BRAND_KEYS = [
    ("coloros", "coloros"),
    ("oneplus", "oneplus"),
    ("oppo", "oppo"),
    ("xiaomi", "xiaomi"),
    ("realme", "realme"),
    ("pixel", "pixel"),
]
MODEL_KEYS = ["note12pro", "voyager", "a92s", "sm8350", "a13", "a14"]
SKIP_NAMES = ["check_driver_errors.sh", "load.sh"]
DRIVER_EXTS = (".sh", ".ko", ".kpm")


def parse_tags(*names):
    lower = " ".join(n or "" for n in names).lower()
    brand = "all"
    for key, val in BRAND_KEYS:
        if key in lower:
            brand = val
            break
    model = None
    for kw in MODEL_KEYS:
        if kw in lower:
            model = kw
            break
    return brand, model


VER_AT_START = re.compile(r"^(\d+\.\d+(?:\.\d+)?)")
VER_ANYWHERE = re.compile(r"(?:^|[^0-9])(\d+\.\d+(?:\.\d+)?)")


def kernel_from_start(name):
    """Version token only if the name STARTS with it (e.g. '4.14.117', '5.4-xiaomi').
    Ignores collection tags like 'GT V1.7' or 'V1.7'."""
    m = VER_AT_START.match(name or "")
    return m.group(1) if m else None


def extract_kernel(filename, parent_dirname=None):
    """Kernel for a driver file:
    1) version at start of the file name
    2) version at start of the immediate parent folder name
    3) version anywhere in the file name (device-prefixed names like oppo4.19.81.sh)
    """
    m = None
    if filename:
        m = VER_AT_START.match(filename)
    if not m and parent_dirname:
        m = VER_AT_START.match(parent_dirname)
    if not m and filename:
        m = VER_ANYWHERE.search(filename)
    return m.group(1) if m else None


def is_binary_driver(fn):
    if fn in SKIP_NAMES:
        return False
    return fn.endswith(DRIVER_EXTS)


def build_entries():
    entries = []
    seen = set()

    def add(file_rel, kernel, brand, model, dtype, extra=None):
        rel = PREFIX + "/" + file_rel.replace("\\", "/")
        key = (rel, kernel, dtype)
        if key in seen:
            return
        seen.add(key)
        e = {"file": rel, "kernel": kernel, "brand": brand, "type": dtype}
        if model:
            e["model"] = model
        if extra:
            e.update(extra)
        entries.append(e)

    # Walk collection dirs under Driver/
    for col in sorted(os.listdir(ROOT)):
        colpath = os.path.join(ROOT, col)
        if not os.path.isdir(colpath) or col.startswith("."):
            continue

        # 1) stand-alone driver files directly in the collection
        for fn in sorted(os.listdir(colpath)):
            fp = os.path.join(colpath, fn)
            if os.path.isfile(fp) and is_binary_driver(fn):
                brand, model = parse_tags(fn)
                kernel = extract_kernel(fn, col)
                if kernel is None:
                    print(f"[warn] skip no-kernel file: {col}/{fn}", file=sys.stderr)
                    continue
                dtype = "selfextract" if fn.endswith(".sh") else (
                    "ko" if fn.endswith(".ko") else "kpm")
                add(f"{col}/{fn}", kernel, brand, model, dtype)

        # 2) nested structure: collections may have version subfolders
        #    (e.g. GT/<ver>/load.sh + <ver>.ko, QX10/<ver>/xxx.ko.sh)
        for sub in sorted(os.listdir(colpath)):
            subpath = os.path.join(colpath, sub)
            if not os.path.isdir(subpath):
                continue

            # keep walking deeper if needed (e.g. GT V1.7/4.14.117/*)
            def walk(dirpath, rel_prefix):
                files = [f for f in os.listdir(dirpath)
                         if os.path.isfile(os.path.join(dirpath, f)) and is_binary_driver(f)]
                subdirs = [d for d in os.listdir(dirpath)
                           if os.path.isdir(os.path.join(dirpath, d))]
                leaf = rel_prefix.rstrip("/").split("/")[-1]

                # insmod pair: load.sh + .ko in the same folder
                loadsh = "load.sh" in [f for f in os.listdir(dirpath)]
                kos = [f for f in files if f.endswith(".ko")]
                if loadsh and kos:
                    kernel = extract_kernel("load.sh", leaf)
                    brand, model = parse_tags(rel_prefix)
                    if kernel is None:
                        print(f"[warn] skip insmod no-kernel: {rel_prefix}", file=sys.stderr)
                    else:
                        add(f"{rel_prefix}/load.sh", kernel, brand, model, "insmod",
                            extra={"module": f"{rel_prefix}/{kos[0]}"})
                    # treat remaining non-.ko files below too
                    files = [f for f in files if not f.endswith(".ko")]

                for fn in files:
                    brand, model = parse_tags(fn, rel_prefix)
                    kernel = extract_kernel(fn, leaf)
                    if kernel is None:
                        print(f"[warn] skip no-kernel: {rel_prefix}/{fn}", file=sys.stderr)
                        continue
                    dtype = "selfextract" if fn.endswith(".sh") else (
                        "ko" if fn.endswith(".ko") else "kpm")
                    add(f"{rel_prefix}/{fn}", kernel, brand, model, dtype)

                for sd in subdirs:
                    walk(os.path.join(dirpath, sd), f"{rel_prefix}/{sd}")

            walk(subpath, f"{col}/{sub}")

    # device/model-specific drivers that carry no kernel version in the name
    specials = [
        {"file": "KPM/Hell.kpm", "brand": "all", "model": "hell", "type": "kpm"},
        {"file": "QX10/xiaomi_sm8350-Voyager.ko.sh", "brand": "xiaomi", "model": "sm8350", "type": "selfextract"},
        {"file": "RT-proc/note12pro.sh", "brand": "xiaomi", "model": "note12pro", "type": "selfextract"},
    ]
    for sp in specials:
        e = {"file": PREFIX + "/" + sp["file"], "kernel": "any", "brand": sp["brand"],
             "model": sp["model"], "type": sp["type"]}
        entries.append(e)

    return entries


def main():
    entries = build_entries()
    manifest = {"version": 1, "driver_count": len(entries), "drivers": entries}
    dst = OUT
    if len(sys.argv) > 1:
        dst = sys.argv[1]
    with open(dst, "w", encoding="utf-8") as fh:
        json.dump(manifest, fh, indent=2, ensure_ascii=False)
    print(f"OK: {len(entries)} driver entries -> {dst}")


if __name__ == "__main__":
    main()