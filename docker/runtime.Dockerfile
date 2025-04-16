FROM ubuntu:22.04

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libcurl4 \
    libssl3 \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Create necessary directories
RUN mkdir -p /app/config /app/downloads

# Set environment variables
ENV CONFIG_PATH=/app/config/config.json
ENV DOWNLOAD_PATH=/app/downloads

# Copy the binary from the build context
COPY build/tglink2aria2 /app/tglink2aria2

# Run the application
CMD ["/app/tglink2aria2"] 