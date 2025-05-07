FROM gcc:latest

# install dependencies
RUN apt-get update && \
    apt-get install -y cmake git libgtest-dev python3 python3-pip && \
    rm -rf /var/lib/apt/lists/*

# set working directory
WORKDIR /usr/src/gomail

# copy the entire project
COPY . .

# clean up any old build artifacts
RUN rm -rf build CMakeCache.txt CMakeFiles

# build the C++ code
RUN cmake -S . -B build && cmake --build build

# create entrypoint script
RUN echo '#!/bin/bash' > /entrypoint.sh && \
    echo 'cd /usr/src/gomail/build' >> /entrypoint.sh && \
    echo 'if [ "$1" = "server" ]; then' >> /entrypoint.sh && \
    echo '  shift' >> /entrypoint.sh && \
    echo '  if [ "$1" = "test" ]; then' >> /entrypoint.sh && \
    echo '    echo "Running test suite..."; ./tests' >> /entrypoint.sh && \
    echo '  else' >> /entrypoint.sh && \
    echo '    echo "Running server with args: $@"; ./server "$@"' >> /entrypoint.sh && \
    echo '  fi' >> /entrypoint.sh && \
    echo 'elif [ "$1" = "client" ]; then' >> /entrypoint.sh && \
    echo '  shift' >> /entrypoint.sh && \
    echo '  python3 /usr/src/gomail/src/client/client.py "$@"' >> /entrypoint.sh && \
    echo 'else' >> /entrypoint.sh && \
    echo '  echo "Usage:"' >> /entrypoint.sh && \
    echo '  echo "  server <port> <bloom_size> <hash_ids...>"' >> /entrypoint.sh && \
    echo '  echo "  server test       # runs all tests"' >> /entrypoint.sh && \
    echo '  echo "  client <host> <port>"' >> /entrypoint.sh && \
    echo 'fi' >> /entrypoint.sh && \
    chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
CMD ["server"]