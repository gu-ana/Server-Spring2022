### Build/test container ###
# Define builder stage
FROM swan:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build_coverage

# Build and test for coverage
RUN cmake -DCMAKE_BUILD_TYPE=Coverage ..
RUN make coverage
