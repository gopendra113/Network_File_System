# Network File System (NFS)

## Overview
The Network File System (NFS) project is developed as part of an Operating System course, aimed at creating a distributed file system where clients can access and manage files stored across multiple servers.

## Components

### Clients
Clients are systems or users interacting with the NFS, initiating operations such as reading, writing, deleting files, and more.

### Naming Server (NM)
The Naming Server acts as the central coordinator, managing the directory structure and routing client requests to appropriate Storage Servers.

### Storage Servers (SS)
Storage Servers store and manage the actual files and folders requested by clients, ensuring data persistence and availability.

## Functionalities

### Basic File Operations
1. **Writing a File/Folder:** Clients can create or update files and folders within the NFS.
2. **Reading a File:** Clients retrieve the contents of files stored in the NFS.
3. **Deleting a File/Folder:** Clients can remove unnecessary files and folders from the NFS.
4. **Creating a File/Folder:** Clients initiate the creation of new files and folders.
5. **Listing All Files and Folders:** Clients can explore the NFS structure to list files and subfolders within directories.
6. **Getting Additional Information:** Clients access metadata such as file size, permissions, and timestamps.

### Specifications

#### Initialization
1. **Naming and Storage Servers Initialization:** 
   - The Naming Server (NM) initializes first and coordinates with Storage Servers (SS).
   - Each Storage Server (SS) registers its details (IP, ports, accessible paths) with the NM.
   - NM starts accepting client requests once all SS are initialized.

2. **Storage Servers Functionality (SS):**
   - SS can dynamically add new entries to the NM during runtime.
   - Execute commands issued by NM like file/directory creation, deletion, and copying.
   - Facilitate client interactions for file read, write, and metadata retrieval.

3. **Naming Server (NM):**
   - Stores critical SS information and manages client task feedback.
   - Efficiently searches for SS to handle client requests using optimized data structures.
   - Implements LRU caching for faster responses and data redundancy for fault tolerance.

4. **Client Interactions:**
   - Clients communicate with NM to locate files and perform operations.
   - Direct communication with designated SS for efficient data exchange.
   - Supports concurrent client access and file reading; manages errors with descriptive codes.

5. **Additional Features:**
   - **Concurrency:** Handles multiple clients and concurrent file access.
   - **Error Handling:** Defines clear error codes for various NFS conditions.
   - **Search Optimization:** Uses efficient data structures for fast file lookup.
   - **Data Redundancy:** Implements replication for fault tolerance.
   - **Logging:** Maintains logs of all operations for debugging and monitoring.

## Implementation
- Implemented in c.


### Instructions

1. Make sure you compile the storage server code as "gcc storageserver.c server_functionalities.c" as it requires some functions that are written in server_funcitionalities.c

2. Give the ports for Client Comms and Naming Server Comms as command line arguments while running the executable for Storage Server. 

3. The storage server expects Accessible paths as user input.

4. Make sure you give the Accessible paths that are present inside the folder where the executable of storage server is run. 

# Assumptions

1. It is assumed that the Client will should go on to next operation if there is already a writer present without waiting until the writer completes his operation. 

2. All file /paths are unique.

3. Cache Size is kept as 10

4. Directory is deleted only when its empty, if it had some contents the will result in an unsuccessful deletion. 


