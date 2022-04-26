### Build/test container ###
# Define builder stage
FROM swan:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build

# Build and test
RUN cmake ..
RUN make
RUN ctest --output-on_failure


### Deploy container ###
# Define deploy stage
FROM ubuntu:focal as deploy

# Copy server output binary to "."
COPY --from=builder /usr/src/project/build/bin/server .

COPY --from=builder /usr/src/project/server_config .

COPY --from=builder /usr/src/project/files .

# Expose some port(s)
EXPOSE 8080
EXPOSE 80

# Use ENTRYPOINT to specify the binary name
ENTRYPOINT ["./server"]

# Use CMD to specify arguments to ENTRYPOINT
# Update with real server args
CMD ["server_config"]
