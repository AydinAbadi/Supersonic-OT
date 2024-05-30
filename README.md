# Supersonic-OT: An unconditionally secure OT that relies on only symmtric-key cryptography.



# Dependencies

* GMP: https://gmplib.org/

# Runnig a Test

* Clone the above library, and the file
* Install the library and unzip the file.
* Run the following command lines in order:
  
            g++ -c Rand.cpp
  
            g++  Rand.o  main.cpp  -o main -lgmpxx -lgmp
  
            ./main
