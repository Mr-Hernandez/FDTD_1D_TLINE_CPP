#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fstream>

/*#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>*/
#include <string.h>
#include <iostream>

void CreateFile(int V[]);
void clearFile();

	
int main(int argc, char * argv[])
{
       int pipefd[2];
       pid_t cpid;
       char buf;
       char msg = 'O';
       pipe(pipefd); // create the pipe
       cpid = fork(); // duplicate the current process
       if (cpid == 0) // if I am the child then
       {
	       close(pipefd[1]); // close the write-end of the pipe, I'm not going to use it
           FILE* gnuPipe = popen("gnuplot -persist", "w");
           fflush(gnuPipe);
           fprintf(gnuPipe, "plot 'V2.txt'\n");
           fflush(gnuPipe);
           while (read(pipefd[0], &buf, 1) > 0){
               if(buf == 'G'){break;}
           }
           //int i = 0;
           //int V[20] = {0};
		   while (read(pipefd[0], &buf, 1) > 0) // read while EOF
		   {     //write(1, &buf, 1);
		       if(buf == 'D'){
			       std::cout << "Got D" << std::endl;
		       }else if(buf == 'E'){
			       std::cout << "Got E" << std::endl;
                   //V[i] = i;
                   //CreateFile(V);
                   //i++;
                   fprintf(gnuPipe, "set yrange [0:20]\n");
                   fprintf(gnuPipe, "set title \"Cave of Wonder\"\n");
                   //fprintf(gnuPipe, "reread\n");
                   fprintf(gnuPipe, "replot\n");
                   fflush(gnuPipe);
		       }else if(buf == 'X'){
			       std::cout << "Got X" << std::endl;
			       break;
		       }
               
		   }
           
	   std::cout << "Closing cuz X" << std::endl; 
           //write(1, "\n", 1);
           close(pipefd[0]); // close the read-end of the pipe
           exit(EXIT_SUCCESS);
       }
       else // if I am the parent then
       {
           close(pipefd[0]); // close the read-end of the pipe, I'm not going to use it
           std::cout << "enter G:"; std::cin >> msg;
	   write(pipefd[1], &msg, strlen(&msg)); // send the content of argv[1] to the reader
	   char INPUT[] = "DE";
       int V[20] = {0};
       for(int j = 0; j < 20; j++){
	   for(int i = 0; i < 20; i++){
           V[i] = i + j;
           //CreateFile(V);
		   //msg = INPUT[1];
		   //write(pipefd[1], &msg, strlen(&msg)); // send the content of argv[1] to the reader
           //system("sleep 0.2");
	   }
           CreateFile(V);
           msg = INPUT[1];
		   write(pipefd[1], &msg, strlen(&msg)); // send the content of argv[1] to the reader
           system("sleep 0.2");

       } 
	/*char INPUT[] = "DE"; 
    int V[] = {0};
        for (int i = 0; i < 20; i++){
            for(int j = 0; j < 20; j++){
                V[j] = 0;
                if(j == i){V[i] = 1;}
            }
            msg = INPUT[(int)i%2];
		    write(pipefd[1], &msg, strlen(&msg)); // send the content of argv[1] to the reader
            CreateFile(V);
            //fprintf(pipehandle, "replot\n");
            //fflush(pipehandle);
            system("sleep 0.1");
        }*/
        std::cout << "enter X to end:"; std::cin >> msg;
       std::cout << "clsing pipefd[1] now" << std::endl;
	   close(pipefd[1]); // close the write-end of the pipe, thus sending EOF to the reader
           wait(NULL); // wait for the child process to exit before I do the same
           exit(EXIT_SUCCESS);
       }
       return 0;
}


	//FILE* pipehandle = popen("gnuplot -persist", "w");
	//fprintf(pipehandle, "plot 'V.txt'\n");
	//fflush(pipehandle);
	//fprintf(pipehandle, "replot\n");
	//fflush(pipehandle);
	



	/*int V[] = {0};
	for (int i = 0; i < 20; i++){
		for(int j = 0; j < 20; j++){
			V[j] = 0;
			if(j == i){V[i] = 1;}
		}
		CreateFile(V);
		fprintf(pipehandle, "replot\n");
		fflush(pipehandle);
		system("sleep 0.1");
	}*/

void CreateFile(int V[]){
    std::ofstream out1;
    out1.open("V2.txt", std::ofstream::out | std::ofstream::trunc);

    for(int k = 0; k < 20; k++){
   	    out1 << k+1 << "\t" << V[k] << "\n";
    }
    out1.close();
}

void clearFile(){
    remove("V2.txt");
    //remove("I.txt");
}

