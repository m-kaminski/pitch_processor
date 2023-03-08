# PITCH

Program takes stream of messages in PITCH format on standard input and summarizes most
frequently executed stocks on standard output.



# To build and execute in RHEL/CentOS operating system you will need to install the following

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

