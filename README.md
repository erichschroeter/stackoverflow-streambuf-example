
# Building

```bash
DIR=build && cmake -E make_directory $DIR && cmake -E chdir $DIR cmake ..
DIR=build && cmake --build $DIR --config Debug --target TARGET
```

# Execute

```bash
./build/Debug/example_1.exe 1 10
./build/Debug/example_2.exe 1 10
./build/Debug/example_3.exe 1 10
```
