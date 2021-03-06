### Base environment container ###
# Get the base Ubuntu image from Docker Hub
FROM ubuntu:focal as base

ARG DEBIAN_FRONTEND=noninteractive

# Update the base image and install build environment
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    curl \
    httpie \
    libboost-log-dev \
    libboost-regex-dev \
    libboost-system-dev \
    libgtest-dev \
    netcat \
    gcovr \
    pkg-config \
    libcurlpp-dev \
    libcurl4-openssl-dev \
    jq


RUN curl -sL https://deb.nodesource.com/setup_16.x | bash
RUN apt-get install -y nodejs
