FROM ubuntu:22.04

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libcurl4-openssl-dev \
    libssl-dev \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Install tgbot-cpp
RUN git clone https://github.com/reo7sp/tgbot-cpp.git && \
    cd tgbot-cpp && \
    cmake . && \
    make -j$(nproc) && \
    make install && \
    cd .. && \
    rm -rf tgbot-cpp

# Install spdlog
RUN git clone https://github.com/gabime/spdlog.git && \
    cd spdlog && \
    cmake . && \
    make -j$(nproc) && \
    make install && \
    cd .. && \
    rm -rf spdlog

# Install nlohmann/json
RUN git clone https://github.com/nlohmann/json.git && \
    cd json && \
    cmake . && \
    make -j$(nproc) && \
    make install && \
    cd .. && \
    rm -rf json

# Install cpp-httplib
RUN git clone https://github.com/yhirose/cpp-httplib.git && \
    cd cpp-httplib && \
    cmake . && \
    make -j$(nproc) && \
    make install && \
    cd .. && \
    rm -rf cpp-httplib

# Install cpp-dotenv
RUN git clone https://github.com/adeharo9/cpp-dotenv.git && \
    cd cpp-dotenv && \
    cmake . && \
    make -j$(nproc) && \
    make install && \
    cd .. && \
    rm -rf cpp-dotenv

# Set working directory
WORKDIR /app

# Copy source code
COPY . .

# Build the project
RUN mkdir build && cd build \
    && cmake .. \
    && make -j$(nproc)

# The binary will be available at /app/build/tglink2aria2 