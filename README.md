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

# OPTIONS

```
Program parses stream of trade messages in PITCH format (Order Add,
Order Execute, Order Cancel, Trade) and calculates list of most frequently
executed symbols

Program accepts following commandline options:
-mt[=N] : multi-threaded execution. If =N given, specific number of threads
-st : single-threaded execution (default)
-aio[=bufsz,inflight] : use Linux AIO for input. Can select buffer size
                        (kB) and maximum number of requests in flight (this
                        shouldn't be set above 2 for pipes)
-ios : use io-streams for input (default option)
-verbose : print options selected
-h : display this very help message

(c) Maciej (Matt) Kaminski
```

# PERFORMANCE

I have generated sample file with 100,000,000 (that is 100 Million) lines of
PITCH data, to run certain benchmarks on AMD Ryzen 9 5900X 12-core processor.

I have observed that usage of asynchronous IO (Linux AIO) and threads give
certain boost in execution time, providing some numbers below:

```
  \ threads:        1          3          5          9         17
I/O method
IOS          0m9.540s   0m6.034s   0m5.985s   0m6.068s   0m5.742s

AIO (32k,2)  0m8.061s   0m4.040s   0m4.234s   0m4.000s   0m4.183s

AIO (32k,8)  0m8.116s   0m4.028s   0m3.896s   0m4.118s   0m4.419s
```
(3,5,9 and 17 are total numbers of threads, corresponding to 2/4/8/16
 worker threads and one I/O thread)

As it can be observed, benefits of increasing number of working threads
is diminishing quickly, as problem becomes strictly I/O bound rather than
compute.

Note that to achieve correct results with multi-threading, distribution
of work between worker threads cannot be random, and instead hash needs
to be computed based on OrderID, so that Execute/Cancel commands
will be processed in the same worker thread as Add Order command for the
same order. This allows skipping locks on hash table structure, and
instead keeping distinct data structures between threads, and therefore
achieve higher thrupur.

