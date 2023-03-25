# PITCH

Program takes stream of messages in Cboe PITCH format (TCP depth of 
book for Cboe exchanges) on standard input and summarizes most frequently 
executed stocks on standard output.

# BUILD

To build and execute in RHEL/CentOS operating system you will need to 
install the following

1. C++ development tools
```
yum install g++ make cmake
```


2. You may want to install lib aio (Linux Asynchronous IO)
```
yum install libaio libaio-devel
```

3. Gtest test libraries and gcov to calculate coverage
```
yum install gtest gtest-devel gcov
```

Build script provided creates executable in ./build directory

```
./build.sh
```

If you don't feel like installing CMake, gtest, or dealing with threads
and asynchronous I/O, for your convenience there is second build script which
generates simple minimalist build with all of these dependencies removed

```
./build-minimalist.sh
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

If you wish to run minimalist version, it operates the same, except, that 
certain options including particularly -aio and -mt, are removed.

# OPTIONS

```
./build/pitch_processor
Program parses stream of trade messages in PITCH format (Order Add,
Order Execute, Order Cancel, Trade) and calculates list of most frequently
executed symbols

Program accepts following commandline options:
-mt[=N] : multi-threaded execution. If =N given, specific number of threads
-a : force distinct thread affinity for working threads (only with -mt)
-st : single-threaded execution (default)
-aio[=bufsz,inflight] : use Linux AIO for input. Can select buffer size
                        (kB) and maximum number of requests in flight (this
                        shouldn't be set above 2 for pipes)
-ios : use io-streams for input (default option)
-nr[=N] : number of symbols to calculate (default: 10)
-v : print info about applied options (in order)

-h : display this very help message

(c) Maciej (Matt) Kaminski
```

# ALGORITHM

Solution revolves around two hashing tables (implemented using standard
C++ STL container std::unordered_map), first mapping id of open orders
to state of an order (decoded from PITCH stream), second mapping symbols
of executed shares to numbers of shares executed. 

Calculating summaries of trades is implemented in event_accumulator class
(src/pitch/event_accumulator.h).

* For Add Order message, a message is deposited in live_orders table
* For Cancel Order, number of shares outstanding is decremented through
    live_orders table and entry is erased if number of shares drops to 0;
    (if number drops below 0, exception out_of_range is thrown)
* For Order Executed, number of shares is also decremented from live_orders.
    but also it is added to counters table of executed orders.
* For Trade message, number of shares is added to counters table as well.

To calculate summary, std::sort or std::partial_sort is invoked.

If multiple worker threads are used, work is divided based on order_id,
in such a manner that if one order of given order_id is routed to a worker
thread, every other order with the same order_id will end up with
the same worker thread.

# TESTING

I have used following testing strategies to ensure quality of the program

1. Unit Testing implemented using tool called gtest,
    coverage is assessed using gcov. Every class is bundled with tests
2. Tool called Valgrind is used to verify for lack of memory leaks
    and bad memory accesses
3. Performance is evaluated using tool called time
4. Call-level proviling was performed using the tool called Callgrind,
    to find possible execution bottlenecks
5. Manual testing is performed to determine other areas for testing

# ERROR HANDLING

5 specific classes of errors are handled in this program

1. Unknown messages/lines that are too long or too short, are simply ignored
2. Parse errors, such as 36-bit integers having characters outside 0...9,A...Z
   range, are processed through exceptions, i.e. std::invalid_argument
3. Logic errors such as attempting to execute more shares than there are in
   active order, are processed through exceptions too, i.e. std::out_of_range
4. I/O errors such as failures to read are not retried and are handled through 
   std::runtime_error
5. Incorrect command-line arguments are reported and result in termination
   of program and help message being printed

System errors such as 4. are fatal and result in program being terminated.
Errors from categories 1-3 are not reported individually but are counted and
their number is printed to standard output

# PERFORMANCE

I have generated sample file with 100,000,000 (that is 100 Million) lines of
PITCH data, to run certain benchmarks on AMD Ryzen 9 5900X 12-core processor.

As a second set of test, I have piped 10 of this file to simulate 10 x more
(1,000,000,000 - 1 Billionlines) input data

Following command with various options was used to evaluate execution time:
```
time nice -10 ./build/pitch_processor -aio -mt=X < large_sample_3.6GB

time for i in `seq 10` ; do cat large_sample_3.6GB ; done |\
  nice -10 ./build/pitch_processor -aio -mt=X
```

I have observed that usage of asynchronous IO (Linux AIO) and threads give
certain boost in execution time, providing some numbers below:

Data for smaller set
```
  \ threads:         1          3          5          9         17
I/O method
IOS           0m9.724s   0m6.371s   0m6.923s   0m5.890s   0m6.124s

AIO (32k,2)   0m8.146s   0m4.254s   0m3.587s   0m3.675s   0m3.329s

AIO (32k,8)   0m8.033s   0m4.192s   0m3.336s   0m4.104s   0m3.569s

------------------- with forced thread affinity ------------------

AIO (32k,2)        N/A   0m4.081s   0m3.208s   0m3.395s   0m4.177s
```

Data for larger set

```
  \ threads:         1          3          5          9         17
I/O method
IOS          1m39.226s  0m54.013s   1m6.077s   1m2.318s  0m59.639s

AIO (32k,2)  1m21.075s  0m41.325s  0m33.790s  0m38.680s  0m34.566s

------------------- with forced thread affinity ------------------

AIO (32k,2)        N/A  0m40.574s  0m32.860s  0m33.767s  0m41.291s
```
(3,5,9 and 17 are total numbers of threads, corresponding to 2/4/8/16
worker threads and one I/O thread)

As it can be observed, benefits of increasing number of working threads
is diminishing quickly, when increasing this number beyond 4, as problem 
becomes strictly I/O bound rather than compute.

Sticking each thread to distinct physical does provide small benefit in
overall runtime performance, but the gain is diminishing when number of
working threads exceeds number of physical CPU cores, in which case threads
start to compete for resources with main thread. This can be improved
by CPU affinity arrangement function binding core 0 to main thread, and
remaining cores split in round robin pattern accross other threads

Finally the benefit of increasing number of asynchronous I/O requests 
concurently in flight beyond 2 is questionable.

Overall usage of multiple threads and asynchronous I/O enables application
to run 3 times as fast as when ran in single-threaded mode with I/O streams.

Note that to achieve correct results with multi-threading, distribution
of work between worker threads cannot be random, and instead hash needs
to be computed based on OrderID, so that Execute/Cancel commands
will be processed in the same worker thread as Add Order command for the
same order. This allows skipping locks on hash table structure, and
instead keeping distinct data structures between threads, and therefore
achieve higher thruput.

# CONTACT

If you have any questions, please contact me at maciej.a.kaminski@gmail.com