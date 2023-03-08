# PITCH

Program takes stream of messages in PITCH format on standard input and summarizes most
frequently executed stocks on standard output.



# BUILD

To build and execute in RHEL/CentOS operating system you will need to install the following

1. C++ development tools
```
yum install g++ make cmake
```

2. Gtest test suite
```
yum install gtest gtest-devel
```

3. You may want to install lib aio (Linux Asynchronous IO)
```
yum install libaio libaio-devel
```

3. You will also need to install lib aio (Linux Asynchronous IO)
```
yum install libaio libaio-devel
```

Build script provided creates executable in ./build directory

```
./build.sh
```

# EXECUTE

To execute program, run following command:

```
./build/pitch_processor < ../pitch_example_data
```

By default program uses IO Streams for reading input data, to use AIO instead use

```
./build/pitch_processor -aio < ../pitch_example_data
```
