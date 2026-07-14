## compile Tests

```bash
cd build
rm -Rf CMakeFiles/*
cmake ..
make
./runTests
```

## check test coverage 
```bash
gcovr -r ..
```
