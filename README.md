# CS 130 Swan Server README

## Table of Contents
* [Source Code](#source-code)
* [Build](#build)
* [Testing](#testing)
* [Running the Server](#running-the-server)
* [Adding a Request Handler](#adding-a-request-handler)

## Source Code

## Build
To run the server, enter the build subdirectory and run make:
~~~~
> cd build
> make
~~~~

## Testing
After building the server, you can run the full test suite in the `build` directory using:
~~~~
> cd build
> make test
~~~~
To run just the integration tests, use the following command:
~~~~
> cd tests
> ./integration_test.sh
~~~~

## Running the Server
After building the server, you can run the server from the base directory using:
~~~~
> ./build/bin/server <config file>
~~~~

This repository currently includes a default config file called `server_config` in the base directory.

## Adding a Request Handler


