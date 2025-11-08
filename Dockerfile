# Use an image that already has g++ and build tools
FROM gcc:13

# Set the working directory inside the container
WORKDIR /app

# Copy all project files into the container
COPY . .

# Copy frontend files (HTML, CSS, JS)
COPY frontend /app/frontend

# Install CMake and any extra libraries
RUN apt-get update && apt-get install -y cmake libssl-dev

# Configure and build project
RUN cmake -B build -S .
RUN cmake --build build

# Expose port 8080
EXPOSE 8080

# Run the compiled program
CMD ["sh", "-c", "./build/The_CleanInbox_Crew $PORT"]
