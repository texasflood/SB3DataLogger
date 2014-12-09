
void ADC_init(void);
uint16_t ADC_perform_single_conversion(void);
void distortInit(int signOfANeg, uint64_t a, uint64_t m, uint64_t d);
void fillLookup(int signOfANeg, uint64_t a, uint64_t m, uint64_t d);
void distortionSwitch(int distortionSet);
void clearVol(void);
uint16_t getVol(void);
void shelvingHighInitialise(int coefficient1, int coefficient2, int coefficient3, int coefficient4, int coefficient5);
void shelvingHighSwitch(int shelvingSet);
void delaySwitch(int delaySet); // NEW
void delayInit(void);
