# QxJson <img src=https://travis-ci.org/quarthex/QxJson.svg>

This is my own implementation of a JSON parser.  
It is written in pure C89.  
It uses POSIX libraries.  
No external dependency.

## Build instructions

CMake is needed.

```sh
mkdir build;
cd build;
cmake ../cmake;
```

Otherwise, you could just

```sh
gcc src/*.c -fPIC -shared -o libQxJson.so;
```

# License

This project is released under the MIT Licence.

