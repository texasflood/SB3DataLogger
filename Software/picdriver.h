#define no_error 0
#define RX_BUFF_LEN 64 

void RS232_Init(int port_no); 
void RS232_Close(void);
unsigned char* Read_Data_Block(void);
void send_command(int n);

int switch_distortion (int value);
int set_distortion (int value);
int get_vol(unsigned char *value);
void ComCallback(int portNumber, int eventMask,void *CallbackData);
int set_treble (double value);
int set_offset(int offset);
int set_volume (int value);
