# Use a base image
FROM gcc:latest

# Set the working directory inside the container
WORKDIR /app

# Install SQLite3 development libraries
RUN apt-get update && apt-get install -y libsqlite3-dev

# Copy the entire 'app' directory into the container
COPY ./app /app

# Compile your application
RUN g++ --std=c++11 main.cpp DBHandler.cpp DeviceHandler.cpp LocationHandler.cpp -lsqlite3 -o my_program

# Define the command to run your server when the container starts
CMD ["./my_program"]