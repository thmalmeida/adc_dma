#ifndef RMS_HPP__
#define RMS_HPP__

#include <stdio.h>
#include <math.h>

/* Root Mean Square - RMS class
	Xi: each value
	n: number o measurements samples;
	
	RMS = sqrt(sum(Xi)^2)/n_samples)	
*/

class rms {
public:
	// Circuit polarization parameters for currente sensor
	double Vmax = 2.450;							// Maximum voltage ADC can read [V];
	double Vmin = 0.120;							// Minimum voltage ADC can read [V];                     
	int d_max = 2895;                              // Max decimal value correlated with Vmax on 12 bit range;
	double GND  = 0.0;								// gnd for ADC converter [V];
	double Vdc = 3.3;								// Voltage divisor supply [V];
	double R1 = 180.0*1000;							// Voltage divisor top resistor [Ohms];
	double R2 = 120.0*1000;							// Voltage divisor bottom resistor [Ohms];
	double V_R2 = R2/(R1+R2)*Vdc;					// Voltage over R2 [V] or Vref for ADC converter [V];

	double Rb1 = 0.0;								// Burden resistor (bias) [Ohms];
	double Rb2 = 120.0;								// Burden resistor (bias) [Ohms];
	double N1 = 1;									// Current transformer sensor ration parameters
	double N2 = 2000;								// Current transformer sensor ration parameters

	// ADC sampling parameters. F_clk MHz, internal div = 128, 13 cycles for ADC conversion.
	// unsigned int n_bits = 12;						// ADC conversion resolution;
	// int F_clk = 8e6;								// Crystal system clock [Hz];
	// int div_1 = 1;									// AHB bus prescale;
	// int div_2 = 1;									// APB2 bus prescale;
	// int div_3 = 8;									// ADC prescale;

	// double adc_clk = F_clk/div_1/div_2/div_3;		// ADC clock after all prescalers
	// double adc_sampling_time = 1.5;					// amount of time clock to sample [cycles];
	// double adc_conv = 12.5;							// Fixed number of ADC peripheral takes to convert [cycles];
	// double T_conv = adc_sampling_time + adc_conv; 	// ADC conversion time [cycles];

	// int n_samples;				// number of samples (points)
	// int n_cycles;				// number of waves cycles
	// int f;						// signal frequency [Hz]
	// int nb;						// resolution [number of bits]
	// int Fs;						// sample frequency	[samples/s]

	rms() {}
	double calc_rms(double *v, int len) {
		double sum = 0;

		for(int i=0; i<len; i++) {
			sum += pow(v[i], 2);
		}

		return sqrt(sum/len);
	}
	void calc_Vadc_t(uint16_t* d_out, double* Vadc_t, int length) {
		// Finding Vadc_t after ADC read
		for(int i=0; i<length; i++) {
			Vadc_t[i] = d_out[i]*(Vmax-Vmin)/(d_max) + Vmin;
		}
    }
	void calc_iL_t(uint16_t* d_out, double* iL_t, int length) {
		for(int i=0; i<length; i++) {
			iL_t[i] = (d_out[i]*(Vmax-Vmin)/(d_max) + Vmin - V_R2)*(1/Rb2)*(N2/N1);
		}
	}
	double mean(double* v, int length) {
		double sum = 0;
		for(int i=0; i<length; i++) {
			sum += v[i];
		}
		return sum/static_cast<double>(length);
	}
	void linspace(double* v, double vi, double vf, int length) {
		double interval = (vf - vi)/static_cast<double>(length-1);
		v[0] = vi;
		for(int i= 1; i<length; i++) {
			v[i] = vi + static_cast<double>(i)*interval;
		}
	}
	void dc_remove(double* v, int length) {
		double dc_value = mean(v, length);

		for(int i=0; i<length; i++) {
			v[i] = v[i] - dc_value;
		}
	}
	void filter(uint16_t* v, int length) {
		for(int i=1; i<length; i++) {
			v[i] = 0.8*v[i-1] + 0.2*v[i];
		}
	}
};

#endif
