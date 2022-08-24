
# Building

```bash
DIR=build && cmake -E make_directory $DIR && cmake -E chdir $DIR cmake ..
DIR=build && cmake --build $DIR --config Debug --target TARGET
```

# Execute

```bash
./build/example_1.exe
```
