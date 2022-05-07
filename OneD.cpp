#include <iostream>
#include <math.h>

void Vupdate(double[]& V, double cv, details_t* details, coeff_t* coeff);
void VsUpdate(double[]& V, int n, details_t* details, coeff_t* coeff);
double Vgourd(int n, double dt);
void Vlupdate(double[]& V, double[]& I, details_t* details, coeff_t* coeff);
void init_parameters(details_t* details);
void init_coeff(coeff_t* coeff);
void iupdate(double[]& I, details_t* details, coeff_t* coeff);

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

int main(){
    details_t details;
    coeff_t coeff;
    
    init_parameters();
    init_coeff();

    double V[details->nx] = {0};
    double I[details->nx-1] = {0};
    
    int count = 0;
    for (int i = 0; i < nt; i++){
	// do stuff here
	count++;
    }
    std::cout << "count: " << count << std::endl;
    std::cout << V[0] << ":" << V[10] << ":" << V[20] << std::endl;
    return 0; 
}

void Vupdate(double[]& V, double[]& I, double cv, details_t* details, coeff_t* coeff){
    for(int k = 1; k < details->nx-1; k++){
	V[k] = V[k] - coeff->cv * (I[k] - I[k-1]);
    }
}

void VsUpdate(double[]& V, double[]& I, int n, details_t* details, coeff_t* coeff){
    Vs = Vgourd(n, details->dt);
    if(details->Rs > 0){
	    V[0] = coeff->c1 * (coeff->c2 * V[0] - I[0] + Vs / details->Rs);
    }else{
	    V[0] = Vs; // if Rs = 0, then V[0] = Vs
    }
}

double Vgourd(int n, double dt){
    double t_rise = 200*pow(10,-12);
    double t_fall = t_rise;
    double t_peak = 500*pow(10,-12);

    double t = (n-0.5)*dt;
    double Vsauce = 0;

    if(t < 0){ Vsauce = 0;}
    else if(t >= 0 && t <= t_rise){ Vsauce = 2*(t / t_rise);}
    else if(t >= t_rise && t <= t_rise + t_peak){ Vsauce = 2;}
    else if(t >= t_rise + t_peak && t <= t_rise + t_peak + t_fall){
	    Vsauce = 2*(t_rise + t_peak + t_fall - t) / t_fall;}
	    
    else {Vsauce = 0;}
    return Vsauce;
}

void Vlupdate(double[]& V,double[]& I, double[]& I, details_t* details, coeff_t* coeff){
    // voltage at load node
    V(details->nx) = details->c1 * (details->c2 * V(details->nx) + I(details->nx-1));

}

void iupdate(double[]& I, double[]& V, details_t* details, coeff_t* coeff){
    for(int k = 0; k < details->nx - 1; k++){
	I[k] = I[k] - coeff->ci * (V[k+1] - V[k]);
    }
}

void init_parameters(details_t* details){
// Initialize variable
    details->C = 100*pow(10,-12); // F/m
    details->L = 250*pow(10,-9); // H/m
    details->d = 0.5; // m
    details->ncells = 100;
    details->cfln = 1;
    details->simtime = 5*pow(10,-9);
    details->Rs = 50;  // Source Resistance
    details->RL = 50;  // Load Resistance
    details->load_case = 1;
	
    details->CL = 0;
    details->G = 0;
    details->R = 0;
    details->LL = 0;

    details->Zo = pow((details->L/details->C),0.5);
    details->vo = 1.0/sqrt(details->L*details->C);

    details->dx = details->d / details->ncells;
    details->dt = details->cfln * details->dx / details->vo; 
    details->nx = details->ncells + 1;
    details->nt =(int)(details->simtime / details->dt);
	
}

void init_coeff(coeff_t* coeff){
    // if load_case = 1
    coeff->cv = details->dt/(details->C*details->dx);
    coeff->ci = details->dt/(details->L*details->dx); // iupdate
    double b1 = details->C * details->dx * 0.5 / details->dt;
    double b2 = 0.5 / details->RL;
    coeff->c1 = 1.0 / (b1 + b2);
    coeff->c2 = b1 - b2;
}
