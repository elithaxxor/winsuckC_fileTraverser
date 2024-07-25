# Large File Keyword Search

This C program searches for duplicate files in a specified directory based on their binary content and outputs the results to a file named `search_res.txt`. The program uses the `uthash` library to manage file data in a hashmap for efficient comparison.

## Features

- **Directory Traversal**: Lists and processes all files in a specified directory.
- **Binary Content Comparison**: Compares the binary content of files to find duplicates.
- **Result Logging**: Writes the search results to `search_res.txt`.
- **Memory Management**: Efficiently manages memory allocation and deallocation for file data.

## Requirements

- Windows operating system.
- GCC compiler or a compatible C compiler.
- uthash library (included in the source code).

## Installation

1. **Clone the Repository**:
   ```sh
   git clone https://github.com/yourusername/large-file-keyword-search.git
   cd large-file-keyword-search