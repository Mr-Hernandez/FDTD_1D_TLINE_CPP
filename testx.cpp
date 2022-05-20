#include <iostream>
#include <math.h>
#include <fstream>
#include <unistd.h>
//#include <signal.h>

// for exec() and NewPID()
#include <memory>
#include <stdexcept>
#include <string.h>
#include <array>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>

//for new replot method
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>


int main(){
    char msg[] = "A";
//   char msg = 'A';
    /*size_t msgA_size = strnlen(&msg, 10);

    std::cout << "&msg is : " << strnlen(&msg, 10) << std::endl << std::endl;
    std::cout << "&msg is : " << strnlen(&msg, 10) << std::endl;

    std::cout << "size_t is: " << msgA_size << std::endl;
    std::cout << "msg: " << msg << "\n msg+1: " << &msg  << std::endl;
*/
    std::cout << "&msg is : " << strlen(msg) << std::endl;
    std::cout << "&msg is : " << strlen(msg) << std::endl;

    size_t msgA_size = strlen(msg);
    size_t msgB_size = strlen(msg);
    std::cout << "size_t is: " << msgA_size << std::endl;

 /*   std::cout << "&msg is : " << strnlen(&msg, 1) << std::endl;

    size_t msgA_size = strlen(&msg);
    size_t msgB_size = strlen(&msg);
    //std::cout << "size_t is: " << msgA_size << std::endl;
    std::cout << "&msg is : " << strlen(&msg) << std::endl;
*/
    return 0;

}
