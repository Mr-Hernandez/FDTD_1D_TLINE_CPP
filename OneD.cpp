#include <iostream>
#include <math.h>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

// for exec() and NewPID()
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>

/*
 *
 *
 * What's Next?
 * 1. calls to gnuplot updating each iterations
 * 2. Either output file of V and I or feed directly to gnuplot call
 * 3. Consider single file creation vs multi file creation
 *   a. can gnuplot take only part of a file?
 *   stackoverflow.com/questions/36926994/how-to-plot-specific-subset-of-data-from-a-data-file-with-gnuplot 
 * 4. Make list of commands, perhaps create a header for gnuplot use
 * 5. 
 * 
 * issues to fix
 * 1. output file is appending rather than clearing and rewriting
 * 2. will gnuplot keep resizing as V and I change? 
 * Next
 * 1. automate child pid retrieval to kill process
 * 2. set auto timer for the for loop
 * 3. fix equations
 * 4. possibly make gif. 5. Consider this
 *		 */

typedef struct{ 
    double C;
    double L;
    double d;
    double ncells;
    double cfln;
    double simtime;
    int Rs;
    int RL;
    int load_case;
    double CL;
    double G;
    double R;
    double LL;
    double Zo;
    double vo;
    double dx;
    double dt;
    int nx;
    int nt;
    double Vs;
}details_t;

typedef struct{
    double cv;
    double ci;
    double c1;
    double c2;
}coeff_t;

void Vupdate(double V[], double I[], details_t& details, coeff_t& coeff);
void VsUpdate(double V[], double I[], int n, details_t& details, coeff_t& coeff);
double Vgourd(int n, double dt);
void Vlupdate(double V[], double I[], details_t& details, coeff_t& coeff);
void init_parameters(details_t& details);
void init_coeff(coeff_t& coeff, details_t& details);
void iupdate(double I[], double V[], details_t& details, coeff_t& coeff);
void CreateFile(double V[], double I[], details_t& details);
void plot(double P[]);
std::string exec(const char* cmd);
int NewPID();

int main(){
    details_t details;
    coeff_t coeff;
    
    init_parameters(details);
    init_coeff(coeff, details);

    double V[details.nx] = {0};
    double I[details.nx-1] = {0};
    pid_t pid = fork();
    if(pid == 0){ 
	    plot(V);
	    std::cout << "child id: " <<  pid << std::endl;
       	    std::cout << "child get: " << getpid() << std::endl; 
    }
    else if(pid > 0){
	    std::cout << "parent id: " << pid << std::endl;
            std::cout << "parent get: " << getpid() << std::endl; 
    //plot(V); 

    int count = 0;
    for (int i = 0; i < details.nt-1; i++){
	// do stuff here
	count++;
	Vupdate(V, I, details, coeff);
	VsUpdate(V, I, i, details, coeff);
	Vlupdate(V, I, details, coeff);
	iupdate(I, V, details, coeff);
        for (int i = 0; i < details.nx; i++){
		std::cout << "V[" << i << "] = " << V[i] << std::endl;
        }	

//	do {std::cout << "Press Enter to Continue" << std::endl;}
//	while(std::cin.get() != '\n');
	// file creation for V and I here
        CreateFile(V, I, details);
        system("sleep 0.2");
	// gnuplot call here
	//plot(V);
    }
    std::cout << "count: " << count << std::endl;
    for (int i = 0; i < details.nx; i++){
	std::cout << "V[" << i << "] = " << V[i] << std::endl;
    }
    }
	// kill child process here from parent process
    /*
     *if(pid > 0){ std::cout << pid << std::endl;
	    kill(getpid(), SIGKILL);
    }
    if(pid == 0){
            std::cout << "passed" << getpid() << std::endl;
	    kill(getpid(), SIGKILL); 
    }
     * */
    if(pid > 0) {
	std::cout << pid << std::endl;
//	std::string cmdx = "kill " + std::to_string(pid + 2);
//	system(cmdx.c_str());
       // killpg(pid, SIGKILL);
       //system("kill gnuplot");
       //int G = kill(pid + 2, SIGTERM);
       //std::cout << "G is " << G << std::endl;
	int K = NewPID();
	if(K > 0){
		std::cout << "killerPID is: " << K << std::endl;
		std::string cmd = "kill " + std::to_string(K);
		system(cmd.c_str());
	}
	
    }
    if(pid == 0){
        std::cout << "child process x: " << getpid() << std::endl;
	   // system("kill 2");
	
    }
    std::cout << "who done it: " << getpid() << std::endl;
    return 0; 
}

void Vupdate(double V[], double I[], details_t& details, coeff_t& coeff){
    for(int k = 1; k < details.nx-1; k++){
	V[k] = V[k] - coeff.cv * (I[k] - I[k-1]);
    }
}

void VsUpdate(double V[], double I[], int n, details_t& details, coeff_t& coeff){
    double Vs = Vgourd(n, details.dt);
    if(details.Rs > 0){
	    V[0] = coeff.c1 * (coeff.c2 * V[0] - I[0] + Vs / details.Rs);
    }else{
	    V[0] = Vs; // if Rs = 0, then V[0] = Vs
    }
}

double Vgourd(int n, double dt){
    double t_rise = 200*pow(10,-12);
    double t_fall = t_rise;
    double t_peak = 500*pow(10,-12);

    double t = (n-0.5)*dt;
    double Vsauce;
    if(t < 0){ Vsauce = 0;}
    else if(t >= 0 && t <= t_rise){ Vsauce = 2*(t / t_rise);}
    else if(t >= t_rise && t <= t_rise + t_peak){ Vsauce = 2;}
    else if(t >= t_rise + t_peak && t <= t_rise + t_peak + t_fall){
	    Vsauce = 2*(t_rise + t_peak + t_fall - t) / t_fall;}
	    
    else {Vsauce = 0;}
    return Vsauce;
}

void Vlupdate(double V[],double I[], details_t& details, coeff_t& coeff){
    // voltage at load node
    V[details.nx] = coeff.c1 * (coeff.c2 * V[details.nx] + I[details.nx-1]);

}

void iupdate(double I[], double V[], details_t& details, coeff_t& coeff){
    for(int k = 0; k < details.nx - 1; k++){
	I[k] = I[k] - coeff.ci * (V[k+1] - V[k]);
    }
}

void init_parameters(details_t& details){
// Initialize variable
    details.C = 100*pow(10,-12); // F/m
    details.L = 250*pow(10,-9); // H/m
    details.d = 0.5; // m
    details.ncells = 20;
    details.cfln = 1;
    details.simtime = 5*pow(10,-9);
    details.Rs = 50;  // Source Resistance
    details.RL = 50;  // Load Resistance
    details.load_case = 1;
	
    details.CL = 0;
    details.G = 0;
    details.R = 0;
    details.LL = 0;

    details.Zo = pow((details.L/details.C),0.5);
    details.vo = 1.0/sqrt(details.L*details.C);

    details.dx = details.d / details.ncells;
    details.dt = details.cfln * details.dx / details.vo; 
    details.nx = details.ncells + 1;
    details.nt =(int)(details.simtime / details.dt);
	
}

void init_coeff(coeff_t& coeff, details_t& details){
    // if load_case = 1
    coeff.cv = details.dt/(details.C*details.dx);
    coeff.ci = details.dt/(details.L*details.dx); // iupdate
    double b1 = details.C * details.dx * 0.5 / details.dt;
    double b2 = 0.5 / details.RL;
    coeff.c1 = 1.0 / (b1 + b2);
    coeff.c2 = b1 - b2;
}

void CreateFile(double V[], double I[], details_t& details){
    std::ofstream out1, out2;
    out1.open("V.txt", std::ofstream::out | std::ofstream::trunc);
    out2.open("I.txt", std::ofstream::out | std::ofstream::trunc);

    for(int k = 0; k < details.nx; k++){
   	out1 << k+1 << "\t" << V[k] << "\n";
    }

    for(int k = 0; k < details.nx-1; k++){
   	out2 << k << "\t" << I[k] << "\n";
    }

    out1.close();
    out2.close();
}

void plot(double P[]){
 //   system("gnuplot -p -e \"plot 'V.txt'\"");
	std::ifstream f;
	f.open("file");
	std::string cmd;
	std::getline(f, cmd);
	std::cout << "in my file: " << cmd << std::endl; 
       // execlp("./file", NULL);	
        system(cmd.c_str());

   // system("gnuplot -p -e \"set style data linepoints\"\"plot 'V.txt'\""); 
   // gnuplot -p -e "plot 'V.txt'" -e "set style data linepoints"
   
}

std::string exec(const char* cmd){
	std::array<char, 128> buffer;
	std::string result;
	std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
	while(!feof(pipe.get())){
		if(fgets(buffer.data(), 128, pipe.get()) != nullptr)
			result += buffer.data();
	}
	return result;
}

int NewPID(){
	std::string read = exec("ps aux");
	std::cout << read << std::endl;

	std::stringstream check1(read);
	std::string tmp;
	std::vector <std::string> tokens;
	
	while(getline(check1,tmp, '\n'))
	{
		std::stringstream check2(tmp);
		std::string tmp2;
		copy(std::istream_iterator<std::string>(check2),
		     std::istream_iterator<std::string>(),
		     std::back_inserter(tokens));
		
		for(int i = 0; i < tokens.size(); i++){
			std::cout << i << tokens[i] << std::endl;
			if(tokens[i] == "gnuplot"){
				std::cout << "Winner: " << tokens[1] << std::endl;
				int T = std::stoi(tokens[1]);
				std::cout << "Token 1 as int is: " << T << std::endl;
				return T;
			}
		}
		tokens.clear();
	}
	return 0;
}
