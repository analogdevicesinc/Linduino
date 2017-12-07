
//#define SerialUSB Serial

void setup()
{
	delay(1000);
	SerialUSB.begin(115200);
	while (!Serial); // wait for serial port to connect. Needed for native USB
}

void loop()
{
	delay(1000);
	SerialUSB.write("Hello!\n");
	unsigned long StartTime;
	unsigned long CurrentTime;
	unsigned long ElapsedTime;
	
	const uint16_t baseStrRepetion = 48;
	const uint32_t baseStrSize = 64;
	const uint32_t newStrSize = ((baseStrSize - 1)*baseStrRepetion) + 1;
	uint32_t numIterations = 4096;
	
	char baseStr[baseStrSize] = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ\n";
	char newStr[newStrSize];
	int j = 0, k = 0;
	
	// Populate newStr using baseStr
	for(j = 0; j < (newStrSize - 1); ++j)
	{
		newStr[j] = baseStr[k];
		k++;
		if(k == (baseStrSize - 1))
		{	
			k = 0;
			continue;
		}
	}
	newStr[newStrSize - 1] = '\0';
	
	SerialUSB.write("Measuring time for new string:\n");	
	delay(1000);
	
	StartTime = micros();
	for(uint32_t i = 0; i< numIterations; ++i){
		SerialUSB.write(newStr); 
	}
	CurrentTime = micros();
	ElapsedTime = CurrentTime - StartTime;
	SerialUSB.print(newStrSize * numIterations);
	SerialUSB.print(" characters in microSeconds = ");
	SerialUSB.println(ElapsedTime);
	delay(1000);
	
	/* while(true)
	{
	}; */
}

void createRandomString(uint32_t size, char* newStr)
{
	
}

void createDefiniteString(char* newStr)
{
	
}