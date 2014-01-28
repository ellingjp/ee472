typedef struct measureData {
    int *temperatureRaw;
    int *systolicPressRaw;
    int *diastolicPressRaw;
    int *pulseRateRaw;
} MeasureData;

void initializeMeasureData(MeasureData *data);
void measureTask(void *dataptr);
