# **C++ Multithreaded Rent Optimization Project** 🏠

## **Project Overview** 📄
This project implements a _multithreaded system_ to solve rental optimization problems using **C++**. 
The system leverages concurrency to efficiently handle multiple rental problems, maximizing the total profit from renting out items over specified time intervals.

## **Repository Structure** 📂
- **`common.h`**: Defines the common interface between the implementation and the testing environment.
- **`progtest_solver.h`**: Declares the interface for a problem solver provided by the Progtest environment.
- **`sample_tester.cpp`**: Contains sample test cases to validate the implementation.
- **`sample_tester.h`**: Header file for sample testing classes.
- **`solution.cpp`**: Implements the main logic for solving the rental optimization problem using multithreading and concurrency.
- **`Makefile`**: Build script for compiling and linking the project.

## **Project Description** 📚
The project is structured to solve a set of rental problems defined by intervals and payments. 
Each problem involves renting out a specified number of items over time intervals to maximize profit. 
The main components include:

- **CInterval**: Represents a time interval and the payment for renting during that interval.
- **CProblem**: Encapsulates a rental problem with multiple intervals and a count of items available for rent.
- **CProblemPack**: Contains a batch of problems to be solved.
- **CCompany**: Interface for delivering and receiving problem packs.
- **CProgtestSolver**: Interface for the Progtest problem solver used to solve batches of problems.
- **COptimizer**: Main class implementing the _multithreaded optimization logic_.
- **CCommunicator**: Manages the communication between the company and the optimizer, handling the receiving and sending of problem packs.
- **CWorker**: Represents worker threads that fill the solver with problems, solve them, and notify the communicator.

### **Concurrency and Multithreading Concepts** 🔄
- **Threads**: Multiple threads are used to handle communication with companies and solving of problems concurrently.
- **Mutexes**: Used to synchronize access to shared resources and prevent data races.
- **Condition Variables**: Used to block threads until a certain condition is met, facilitating coordination between threads.
- **Buffers**: Shared buffers for storing problem packs and solutions, protected by mutexes to ensure _thread-safe operations_.

## **Build Instructions** 🛠️
1. **Install Dependencies**: Ensure you have GCC and make installed.
2. **Compile the Project**: Run `make` in the project directory to compile the source files and build the executable.
3. **Generate Dependencies**: Run `make deps` to generate dependencies for the project.
4. **Clean the Build Directory**: Run `make clean` to remove generated files.

### **Makefile Targets** 📌
- **`all`**: Default target to build the project.
- **`deps`**: Generates dependencies for the source files.
- **`test`**: Compiles the object files and links them into the final executable.
- **`lib`**: Creates a static library from the object file.
- **`clean`**: Removes all generated files from the build directory.
- **`pack`**: Cleans the build directory and creates a tarball package of the project.

## **Run Instructions** 🚀
1. **Start the Optimizer**:
   ```sh
   ./test
   ```
   This will start the optimizer, which will process the rental problems using multiple threads.

2. **Test the Implementation**:
   The provided `sample_tester.cpp` includes sample test cases to validate the implementation.

## **Conclusion** 📝
This project demonstrates the use of _multithreading_ and _concurrency_ in C++ to solve _complex optimization problems_. 
The use of synchronization mechanisms like _mutexes_ and _condition variables_ ensures thread-safe operations and efficient problem-solving.
