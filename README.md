# HTTP Web Server

This is a simple HTTP web server that serves files from a path specified by the user in the URL. It is written in C and
uses sockets to handle and maintain connections, parse HTTP requests, and send HTTP responses with requested files. The
server also includes a file reader handler to read requested files from the file system and send them as part of the
HTTP response.

## How to use

No external libraries are required to build this project. To build and run the server, follow these steps:

1. Clone this repository to your local machine.
2. Open a terminal and navigate to the project directory.
3. Create a new build directory and navigate into it:
    ```bash
    mkdir build
    cd build
    ```
4. Run CMake to generate build files:
    ```bash
    cmake ..
    ```
5. Build the project using the generated build files:
    ```bash
    make
    ```
6. (Optional) Move the executable to the directory where you want to serve files from:
    ```bash
    mv C_Web_Server ../path/to/serve/files/from
    ```
7. Run the executable:
    ```bash
    ./C_Web_Server
    ```
8. Open a web browser and navigate to `localhost` to view the files in the directory where you ran the executable.

## The server code is divided into three main sections:

1. Socket handling: This section contains the code for creating, binding, and accepting connections on the server
   socket.
2. HTTP request parsing: This section contains the code for parsing incoming HTTP requests and generating appropriate
   responses.
3. File handling: This section contains the code for reading files from disk and sending them to clients in response to
   HTTP requests.

## Future improvements

### This server implementation is basic and lacks many of the features of a full-fledged web server. Some possible improvements include:

1. Support for multiple concurrent connections.
2. Support for more HTTP methods (e.g. PUT, DELETE).
3. Improved error handling and logging.
4. Better file handling (e.g. support for serving large files).
5. Actual safety, include not allowing .. in paths
6. URL encoding/decoding: no files with spaces can be served
