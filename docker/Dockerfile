### Build/test container ###
# Define builder stage
FROM swan:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build

# Build and test
RUN cmake ..
RUN make -j 4
RUN ctest --output-on_failure

# build frontend
WORKDIR /usr/src/project/frontend
RUN npm install
RUN npm run build

### Deploy container ###
# Define deploy stage
FROM ubuntu:focal as deploy

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y libcurl4-openssl-dev libcurlpp-dev
RUN ldconfig -p | grep libcurlpp

# Copy server output binary to "."
COPY --from=builder /usr/src/project/build/bin/server .

COPY --from=builder /usr/src/project/server_config .

COPY --from=builder /usr/src/project/files ./files

COPY --from=builder /usr/src/project/frontend ./frontend
# Expose some port(s)
EXPOSE 8080
EXPOSE 80

# Use ENTRYPOINT to specify the binary name
ENTRYPOINT ["./server"]

# Use CMD to specify arguments to ENTRYPOINT
# Update with real server args
CMD ["server_config"]
