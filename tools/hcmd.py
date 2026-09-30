"""Drive the HClock serial console from the PC.

    python tools/hcmd.py COM3 i              -> status
    python tools/hcmd.py COM3 h s:menu.png   -> hold OK, then save a screenshot
    python tools/hcmd.py COM11 d d o s:x.png

Each argument is one console command; "s:<file>" saves the screenshot as a PNG
(scaled 3x, OLED-blue on black). The port is opened without toggling DTR/RTS,
so the clock is not reset.
"""
import struct
import sys
import time
import zlib

import serial

SCALE = 3
FG = (90, 190, 255)
BG = (0, 0, 0)


def open_port(name):
    s = serial.Serial()
    s.port = name
    s.baudrate = 115200
    s.timeout = 0.2
    s.dtr = False
    s.rts = False
    s.open()
    return s


def read_until(s, pred, timeout=3.0):
    end = time.time() + timeout
    lines = []
    buf = b""
    while time.time() < end:
        buf += s.read(4096)
        while b"\n" in buf:
            line, buf = buf.split(b"\n", 1)
            line = line.decode("ascii", "replace").strip()
            lines.append(line)
            if pred(line):
                return lines
    return lines


def write_png(path, w, h, pages):
    rows = []
    for y in range(h):
        page, bit = pages[y // 8], y % 8
        row = bytearray()
        for x in range(w):
            on = (page[x] >> bit) & 1
            row += bytes(FG if on else BG) * SCALE
        rows.extend([b"\0" + bytes(row)] * SCALE)
    raw = b"".join(rows)

    def chunk(tag, data):
        return struct.pack(">I", len(data)) + tag + data + struct.pack(">I", zlib.crc32(tag + data) & 0xFFFFFFFF)

    png = b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", w * SCALE, h * SCALE, 8, 2, 0, 0, 0))
    png += chunk(b"IDAT", zlib.compress(raw, 9)) + chunk(b"IEND", b"")
    open(path, "wb").write(png)


def screenshot(s, path):
    s.write(b"s\n")
    lines = read_until(s, lambda l: l == "END", timeout=5)
    try:
        start = next(i for i, l in enumerate(lines) if l.startswith("SCREENSHOT"))
    except StopIteration:
        print("no screenshot received:", lines[-3:])
        return
    _, w, h = lines[start].split()
    w, h = int(w), int(h)
    pages = [bytes.fromhex(l) for l in lines[start + 1:start + 1 + h // 8]]
    write_png(path, w, h, pages)
    print(f"saved {path} ({w}x{h})")


def main():
    sys.stdout.reconfigure(errors="replace")  # stray non-ASCII bytes must not crash the tool
    s = open_port(sys.argv[1])
    time.sleep(0.3)
    s.reset_input_buffer()
    for cmd in sys.argv[2:]:
        if cmd.startswith("s:"):
            screenshot(s, cmd[2:])
            continue
        s.write((cmd + "\n").encode())
        for line in read_until(s, lambda l: l.startswith(("ok", "mode=", "RTC", "unknown", "commands", "usage")), 1.5):
            print(line)
        time.sleep(0.4)  # let the menu redraw before the next command
    s.close()


if __name__ == "__main__":
    main()
