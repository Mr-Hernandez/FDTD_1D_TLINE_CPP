#include <iostream>
#include <math.h>
#include <fstream>
#include <unistd.h>
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
 * Try using refresh and replot  and volatile instead of reread.
 */


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
void plot2(double P[]);
void replot(double P[]);


int main(){
    details_t details;
    coeff_t coeff;
    
    init_parameters(details);
    init_coeff(coeff, details);

    double V[details.nx] = {0};
    double I[details.nx-1] = {0};
    pid_t pid = fork();
    if(pid == 0){ 
	    //plot(V);
        CreateFile(V, I, details);
        plot2(V);
	    std::cout << "child id: " <<  pid << std::endl;
       	std::cout << "child get: " << getpid() << std::endl; 
    }
    else if(pid > 0){
	    std::cout << "parent id: " << pid << std::endl;
        std::cout << "parent get: " << getpid() << std::endl; 
        //plot2(V);
    for (int i = 0; i < details.nt-1; i++){
        Vupdate(V, I, details, coeff);
        VsUpdate(V, I, i, details, coeff);
        Vlupdate(V, I, details, coeff);
        iupdate(I, V, details, coeff);
        for (int i = 0; i < details.nx; i++){
           // std::cout << "V[" << i << "] = " << V[i] << std::endl;
        }	

	// file creation for V and I here
        CreateFile(V, I, details);
        replot(V);
        system("sleep 0.01");
        // gnuplot call here
        //plot(V);
    }
        for (int i = 0; i < details.nx; i++){
           // std::cout << "V[" << i << "] = " << V[i] << std::endl;
        }
    } // pid > 0 end case
    
    if(pid > 0) {
        std::cout << pid << std::endl;
        
        int K = NewPID();
        if(K > 0){
            std::cout << "killerPID is: " << K << std::endl;
            std::string cmd = "kill " + std::to_string(K);
            system(cmd.c_str());
        }
    }
    if(pid == 0){
        std::cout << "child process x: " << getpid() << std::endl;
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
    if(t < 0)
    	{ Vsauce = 0;}
    else if(t >= 0 && t <= t_rise)
    	{ Vsauce = 2*(t / t_rise);}
    else if(t >= t_rise && t <= t_rise + t_peak)
    	{ Vsauce = 2;}
    else if(t >= t_rise + t_peak && t <= t_rise + t_peak + t_fall)
    	{ Vsauce = 2*(t_rise + t_peak + t_fall - t) / t_fall;}
    else {Vsauce = 0;}
    //std::cout << "Vsauce at n:" << n << " = " << Vsauce << std::endl;
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
    details.ncells = 100;
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
    out1.open("V2.txt", std::ofstream::out | std::ofstream::trunc);
    out2.open("I2.txt", std::ofstream::out | std::ofstream::trunc);


    for(int k = 0; k < details.nx; k++){
   	out1 << k+1 << "\t" << V[k] << "\n";
    }

    for(int k = 0; k < details.nx-1; k++){
   	out2 << k << "\t" << I[k] << "\n";
    }

    out1.close();
    out2.close();

    remove("V.txt");
    remove("I.txt");
    rename("V2.txt", "V.txt");
    rename("I2.txt", "I.txt");
    
}


void plot(double P[]){
	std::ifstream f;
	f.open("file");
	std::string cmd;
	std::getline(f, cmd);
	std::cout << "in my file: " << cmd << std::endl; 
    system(cmd.c_str());
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
//	std::cout << read << std::endl;

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
//			std::cout << i << tokens[i] << std::endl;
			if(tokens[i] == "gnuplot"){
//				std::cout << "Winner: " << tokens[1] << std::endl;
				int T = std::stoi(tokens[1]);
				std::cout << "Token 1 as int is: " << T << std::endl;
				return T;
			}
		}
		tokens.clear();
	}
	return 0;
}

void plot2(double P[]){
	std::ifstream f;
	f.open("load.file");
	std::string cmd;
	std::getline(f, cmd);
	std::cout << "in my file: " << cmd << std::endl; 
    system(cmd.c_str());
}

void replot(double P[]){
	std::ifstream f;
	f.open("load2.file");
	std::string cmd;
	std::getline(f, cmd);
	std::cout << "in my file: " << cmd << std::endl; 
    system(cmd.c_str());
}
