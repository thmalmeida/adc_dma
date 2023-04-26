#ifndef __ADC_HPP__
#define __ADC_HPP__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

// Includes for one shot read
#include "soc/soc_caps.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

// Includes for adc continuous mode
#include "esp_adc/adc_continuous.h"

#include "sdkconfig.h"

// Continuous macros
#define ADC_CHANNELS_NUMBER 1
#define POINTS_PER_CYCLE	350
#define N_CYCLES			2
#define READ_LENGTH 		POINTS_PER_CYCLE*N_CYCLES*SOC_ADC_DIGI_RESULT_BYTES			// 2 bytes of conversion
// #define READ_LENGTH 		POINTS_PER_CYCLE*N_CYCLES*SOC_ADC_DIGI_DATA_BYTES_PER_CONV	// 4 bytes of conversion for 4.4.3

enum class adc_mode {
	oneshot = 0,
	stream
};

enum class adc_states {
	stopped = 0,
	running
};

class ADC_driver{
public:
	bool do_calibration1 = false;
	adc_digi_pattern_config_t a;

	ADC_driver(adc_mode mode);
	~ADC_driver(void);

	// ----- Single read mode setup -----
	void oneshot_init(void);
	void deinit_driver_oneshot(void);
	void channel_config_oneshot(int channel, int attenuation, int bitwidth);
	void set_channel(int channel);
	int read(int channel);
	int read(int channel, int n_samples) {
		int adc_raw = read(channel);
		int filtered = static_cast<long int>(adc_raw);

		for(int i=1; i<n_samples; i++) {
			// v[i] = 0.8*v[i-1] + 0.2*read(channel);
			adc_raw = 0.8*adc_raw + 0.2*read(channel);
			filtered += (adc_raw + 1);
			filtered >>= 1;
		}
		return filtered;
	}

	// ----- Continuous mode setup -----
	// Parameters initizalization
	adc_channel_t channels_list[ADC_CHANNELS_NUMBER];	// channels list		
	adc_continuous_handle_t stream_handle = NULL;	// ADC handle
	// adc_continuous_evt_cbs_t stream_callback;		// Callback structure
	uint8_t result[READ_LENGTH] = {0};

	void stream_init(void);
	void stream_callback_config(void); 
	void stream_config(int channel, int attenuation);
	void stream_config(int* channels_list, int* attenuations_list, int n_channels);
	void stream_start(void);
	void stream_stop(void);
	void stream_read(int channel, uint16_t* buffer, int length);
	void stream_deinit(void);

	bool adc_calibration_init(adc_unit_t unit, adc_atten_t atten, adc_cali_handle_t *out_handle);
	void adc_calibration_deinit(adc_cali_handle_t handle);
	// void calibrate(void);

private:
	// one shot configuration
	adc_oneshot_unit_handle_t adc1_handle_;
    adc_cali_handle_t adc1_cali_handle_ = NULL;

	// used for single read
	adc_channel_t channel_;						// ADC channel
	adc_bitwidth_t width_ = ADC_BITWIDTH_12;	// bits for resolution conversion
	adc_atten_t attenuation_ = ADC_ATTEN_DB_0;	// attenuation for the channel
	adc_unit_t unit_ = ADC_UNIT_1;				// unit conversion

	// continuous DMA read
	adc_states adc_state_ = adc_states::stopped;
};

#endif /* ADC_HPP__ */
