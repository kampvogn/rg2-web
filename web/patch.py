# Replace exact text in a source file whose lines may end in CRLF, LF or a mix
# (old/new are given with \n; the variant that matches the file is used).
def patch(path, old, new, count=1):
    s = open(path, 'rb').read().decode('latin1')
    for o, n in ((old.replace('\n', '\r\n'), new.replace('\n', '\r\n')), (old, new)):
        if s.count(o) == count:
            open(path, 'wb').write(s.replace(o, n).encode('latin1'))
            return
    raise SystemExit(f'{path}: expected {count} match(es) for:\n{old[:200]}')
