
#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

unsigned int sample_duration_us = 500000; //set to 1/2 sec, since 1 second beeps are being generated
unsigned long microseconds, microseconds_start;
uint32_t total_avg; //to store adc output
uint16_t th_cnt, total_cnt;


/*
  These are the input and output vectors
  Input vectors receive computed results from FFT
*/
double vReal[samples];
double vImag[samples];

uint32_t sample_data(uint16_t freq_num) {
  uint32_t cnt = 0;
  uint16_t freq_idx_local = freq_idx[freq_num];
  microseconds_start = micros();
  while (micros() < microseconds_start + sample_duration_us) {
    cnt++;
    /*SAMPLING*/
    for (int i = 0; i < samples; i++)
    {
      microseconds = micros();    //Overflows after around 70 minutes!

      vReal[i] = analogRead(MIC_PIN);
      vImag[i] = 0;
      while (micros() < (microseconds + sampling_period_us)) {
        //empty loop
      }
    }

    //FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  /* Weigh data */
    FFT.Compute(FFT_FORWARD); /* Compute FFT */
    FFT.ComplexToMagnitude(); /* Compute magnitudes */
    Serial.println("Computed magnitudes:");
    PrintVector(vReal, (samples >> 1), SCL_FREQUENCY);

    total_avg += vReal[freq_idx_local];

    Serial.println();
  }
  total_avg = total_avg / cnt;
  Serial.print("Total avg : ");
  Serial.println(total_avg);
  return total_avg;
}

void PrintVector(double *vData, uint16_t bufferSize, uint8_t scaleType)
{
  for (uint16_t i = 0; i < (bufferSize); i++)
  {
    double abscissa = 0;
    /* Print abscissa value */
    switch (scaleType)
    {
      case SCL_INDEX:
        abscissa = (i * 1.0);
        break;
      case SCL_TIME:
        abscissa = ((i * 1.0) / samplingFrequency);
        break;
      case SCL_FREQUENCY:
        abscissa = ((i * 1.0 * samplingFrequency) / samples);
        break;
    }
    Serial.print(abscissa, 0);
    if (scaleType == SCL_FREQUENCY)
      Serial.print("Hz");
    Serial.print(" ");
    Serial.print(vData[i], 4);
    Serial.print("   ");
  }
  Serial.println();
}
