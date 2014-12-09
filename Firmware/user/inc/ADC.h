
void ADC_init(void);
uint16_t ADC_perform_single_conversion(void);
void distortInit(int signOfANeg, uint64_t a, uint64_t m, uint64_t d);
void fillLookup(int signOfANeg, uint64_t a, uint64_t m, uint64_t d);
void distortionSwitch(int distortionSet);
