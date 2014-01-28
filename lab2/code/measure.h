typedef struct measureData {
    int *temperatureRaw;
    int *systolicPressRaw;
    int *diastolicPressRaw;
    int *pulseRateRaw;
} MeasureData;

extern MeasureData measureData;

void initializeMeasureData(MeasureData *data);
void measureTask(void *dataptr);
