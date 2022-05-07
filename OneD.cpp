#include <iostream>
#include <math.h>

void Vupdate(double[]& a, int nx, double cv);
void VsUpdate(double[]& V, details& D, int n){


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
}coeff_t;

int main(){
    details_t details;
    coeff_t coeff;
    
    // Initialize variable
    details->C = 100*pow(10,-12); // F/m
    details->L = 250*pow(10,-9); // H/m
    details->d = 0.5; // m
    details->ncells = 100;
    details->cfln = 1;
    details-> simtime = 5*pow(10,-9);
    details->Rs = 50;  // Source Resistance
    details->RL = 50;  // Load Resistance
    details->load_case = 1;
	
    details->CL = 0;
    details->G = 0;
    details->R = 0;
    details->LL = 0;

    details->Zo = pow((L/C),0.5);
    details->vo = 1.0/sqrt(L*C);

    details->dx = d / ncells;
    details->dt = cfln * dx / vo; 
    details->nx = ncells + 1;
    details->nt =(int)(simtime / dt);
	
    double V[details->nx] = {0};
    double I[details->nx-1] = {0};
    
    coeff->cv = details->dt/(details->C*details->dx);

    int count = 0;
    for (int i = 0; i < nt; i++){
	// do stuff here
	count++;
	Vupdate(V, details->nx, coeff->cv);
    }
    std::cout << "count: " << count << std::endl;
    std::cout << V[0] << ":" << V[10] << ":" << V[20] << std::endl;
    return 0; 
}

void Vupdate(double[]& V, int nx, double cv){
    for(int k = 1; k < nx-1; k++){
	V[k] = V[k] - cv * (I[k] - I[k-1]);
    }

}

void VsUpdate(double[]& V, details& D, int n){
    details->Vs = Vgourd(n, dt);
    if(Rs > 0){
	    V[0] = c1 * (c2 * V[0] - I[0] + Vs / Rs);
    }else{
	    V[0] = Vs; // if Rs = 0, then V[0] = Vs
    }
}

double Vgourd(int n, double dt){
    double t_rise = 200*pow(10,-12);
    double t_fall = t_rise;
    double t_peak = 500*pow(10,-12);

    double t = (n-0.5)*dt;

    if(t < 0){ Vsauce = 0;}
    else if(t >= 0 && t <= t_rise){ Vsauce = 2*(t / t_rise);

}
