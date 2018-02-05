# APES Homework 2, Problem 6 (and 7)

## Running tests

The CMocka unit test framework is included in this repository as a submodule. A
fresh clone should initialize the submodules before attempting to run tests:

```
$ cd [repo]
$ git submodule init
$ git submodule update
```

Once updated, the build system can automatically build the testing framework and run all unit tests:

```
$ make tests
```

