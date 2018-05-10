# Arduino Challenge Dashboard

To test:

```bash
pushd arduinotest
wcosa build
wcosa upload
popd
```

To test just the serial port operation:
```bash
cat arduinotest/config.json | python -c "import sys, json; print json.load(sys.stdin)['port']" | xargs cat | hexdump -v -e '/1 "%02X\n"'
```

To test the whole thing:
```bash
go run server.go
```
Now, open `dashboard.html`.

Note that you may have a different port, and may have to change it in `arduinotest/config.json`.
