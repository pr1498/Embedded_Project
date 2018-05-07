
#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

unsigned int sample_duration_us = 1000000; //set to 1/2 sec, since 1 second beeps are being generated
unsigned long microseconds, microseconds_start;
uint16_t th_cnt, total_cnt;


/*
  These are the input and output vectors
  Input vectors receive computed results from FFT
*/
double vReal[samples];
double vImag[samples];

void sample_data() {
  unsigned long cnt = 0;

  microseconds_start = micros();

  //set previous avg to zero
  for (int freq_num = 0; freq_num < NUM_FREQ ; freq_num++) {
    total_avg[freq_num] = 0;
  }

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

    //uncomment this
    // FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  /* Weigh data */
    FFT.Compute(vReal, vImag, samples, FFT_FORWARD); /* Compute FFT */
    FFT.ComplexToMagnitude(vReal, vImag, samples); /* Compute magnitudes */
    //
    //    double x = FFT.MajorPeak(vReal, samples, samplingFrequency);
    //    Serial.print("Major Peak ");
    //    Serial.println(x, 0); //Print out what frequency is the most dominant.

    //        Serial.println("Computed magnitudes:");
    //        PrintVector(vReal, (samples >> 1), SCL_FREQUENCY);

    for (int freq_num = 0; freq_num < NUM_FREQ ; freq_num++) {
      uint16_t freq_idx_local = freq_idx[freq_num];
      double data = 0;
      //sum up data in 3 bins
      for (int i = freq_idx_local - 1 ; i <= (freq_idx_local + 1 ); i++) {
        data += vReal[i];
      }
      total_avg[freq_num] += data;
    }
    //  Serial.println(total_avg);
  }
  for (int i = 0; i < NUM_FREQ; i++) {
    total_avg[i] = total_avg[i] / cnt;
    //    Serial.print("Total avg : ");
    //    Serial.print(total_avg[i]);
    //    Serial.print(" for count ");
    //    Serial.println(cnt);
  }


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
