## Steps to Run the Project on Ubuntu:

1. **Clone the repository**:
   ```bash
   git clone https://github.com/muthumala19/Lab1-PThreads.git
2. **Navigate to the repository**:
   ```bash
   cd Lab1-PThreads
3. **There are 3 directories for each approach**:
   - serial_program
   - parallel_program_with_mutex
   - parallel_program_with_read_write_locks
4. **Open the directory of the approach using terminal you want to run**:
   ```bash
    cd <directory_name>
5. **Create a build directory using the command and change the directory**:
    ```bash
    mkdir build && cd build
6. **Run the command `cmake ..` to generate the makefile**:
    ```bash
    cmake ..
7. **Run the command `make` to build the project**:
    ```bash
    make
8. **Run the command `./<directory_name>` to run the project**:

## Additional Information:
1. `main.cpp` file of each parallel_program_with_mutex and parallel_program_with_read_write_locks has a constant property defined as `const int NUMBER_OF_THREADS =1;`.
2. Change the value of the constant property to the number of threads you want to run the program with.
