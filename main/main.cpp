#include "nvs_flash.h"
#include "rms.hpp"
#include "adc.hpp"

static const char *TAG_ADC_DMA = "ADC_DMA";

void test_adc_dma(void *pvParameter) {
		// ADC DMA Test code
	ADC_driver adc0(adc_mode::stream);
	adc0.stream_config(0, 3);

	// Number of samples depends of sample frequency, signal frequency and number of cycles;
	const int n_samples = POINTS_PER_CYCLE*N_CYCLES;

	// adc raw data array;
	uint16_t adc_buffer[n_samples];						// são 700 pontos

	// time domain load current;
	double iL_t[n_samples];

	// rms load current;
	double iL_rms;

	// some dsp process;
	rms s0;
	
	while(1) {

		// Read stream array from ADC using DMA;
		adc0.stream_read(0, &adc_buffer[0], n_samples);

		// Convert digital ADC raw array to iL(t) signal;
		s0.calc_iL_t(&adc_buffer[0], &iL_t[0], n_samples);

		// Find the RMS value from iL(t) signal
		iL_rms = s0.calc_rms(&iL_t[0], n_samples);

		// print adc raw values for debug purposes
		printf("adc_buffer: ");
		for(int i=0; i<n_samples; i++) {
			printf("%u, ", adc_buffer[i]);
		}
		printf("\n");

		// print rms load current in Amperes
		ESP_LOGI(TAG_ADC_DMA, "iL_rms:%.2lf A\n", iL_rms);

		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
extern "C" void app_main(void)
{
	// Initialize NVS.
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		// 1.OTA app partition table has a smaller NVS partition size than the non-OTA
		// partition table. This size mismatch may cause NVS initialization to fail.
		// 2.NVS partition contains data in new format and cannot be recognized by this version of code.
		// If this happens, we erase NVS partition and initialize NVS again.
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}
	ESP_ERROR_CHECK( err );

	xTaskCreate(&test_adc_dma, "test_adc_dma0", 1024*20, NULL, 5, NULL);
}
