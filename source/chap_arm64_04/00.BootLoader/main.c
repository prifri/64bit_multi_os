#if 0
// https://stackoverflow.com/questions/62853496/qemu-system-aarch64-memory-and-device-layout
static const MemMapEntry base_memmap[] = {
    /* Space up to 0x8000000 is reserved for a boot ROM */
    [VIRT_FLASH] =              {          0, 0x08000000 },
    [VIRT_CPUPERIPHS] =         { 0x08000000, 0x00020000 },
    /* GIC distributor and CPU interfaces sit inside the CPU peripheral space */
    [VIRT_GIC_DIST] =           { 0x08000000, 0x00010000 },
    [VIRT_GIC_CPU] =            { 0x08010000, 0x00010000 },
    [VIRT_GIC_V2M] =            { 0x08020000, 0x00001000 },
    [VIRT_GIC_HYP] =            { 0x08030000, 0x00010000 },
    [VIRT_GIC_VCPU] =           { 0x08040000, 0x00010000 },
    /* The space in between here is reserved for GICv3 CPU/vCPU/HYP */
    [VIRT_GIC_ITS] =            { 0x08080000, 0x00020000 },
    /* This redistributor space allows up to 2*64kB*123 CPUs */
    [VIRT_GIC_REDIST] =         { 0x080A0000, 0x00F60000 },
    [VIRT_UART] =               { 0x09000000, 0x00001000 },
    [VIRT_RTC] =                { 0x09010000, 0x00001000 },
    [VIRT_FW_CFG] =             { 0x09020000, 0x00000018 },
    [VIRT_GPIO] =               { 0x09030000, 0x00001000 },
    [VIRT_SECURE_UART] =        { 0x09040000, 0x00001000 },
    [VIRT_SMMU] =               { 0x09050000, 0x00020000 },
    [VIRT_PCDIMM_ACPI] =        { 0x09070000, MEMORY_HOTPLUG_IO_LEN },
    [VIRT_ACPI_GED] =           { 0x09080000, ACPI_GED_EVT_SEL_LEN },
    [VIRT_NVDIMM_ACPI] =        { 0x09090000, NVDIMM_ACPI_IO_LEN},
    [VIRT_PVTIME] =             { 0x090a0000, 0x00010000 },
    [VIRT_SECURE_GPIO] =        { 0x090b0000, 0x00001000 },
    [VIRT_MMIO] =               { 0x0a000000, 0x00000200 },
    /* ...repeating for a total of NUM_VIRTIO_TRANSPORTS, each of that size */
    [VIRT_PLATFORM_BUS] =       { 0x0c000000, 0x02000000 },
    [VIRT_SECURE_MEM] =         { 0x0e000000, 0x01000000 },
    [VIRT_PCIE_MMIO] =          { 0x10000000, 0x2eff0000 },
    [VIRT_PCIE_PIO] =           { 0x3eff0000, 0x00010000 },
    [VIRT_PCIE_ECAM] =          { 0x3f000000, 0x01000000 },
    /* Actual RAM size depends on initial RAM and device memory settings */
    [VIRT_MEM] =                { GiB, LEGACY_RAMLIMIT_BYTES },
};
#endif
// Data register of first UART (PL011)
volatile unsigned int * const UART0_DR = (unsigned int *) 0x09000000;
volatile unsigned int * const OUT = (unsigned int *) 0x0C000000;

// Stop guest so Qemu terminates
void system_off(void);

void print(const char *s) {
	while(*s != '\0') {
		*UART0_DR = (unsigned int)(*s++);
    }
}

void copy(const char *s)
{
	char *out = (char *)OUT;

	for (unsigned int i = 0; i < 80 * 25 * 2; i++)
	{
		*out = 0x0;
		out++;
		*out = 0xA;
		out++;
	}
#if 0
	out = (char *)OUT;
	while (*s != '\0')
	{
		*out = *s;
		out++;
		s++;
	}
	*out = '\0';
#endif
}
// This source file contains the main algorithm for the Kernel.
// Written in C and written by Daniel Lopez

//Global variable, containing the pointer to the video memory.gcc
volatile char* videomem = (volatile char*) 0xb800;
void print_string(int color, char* string)
{
   
   while(*string != 0)
   {
      *videomem = *string; // Assign each character to memory location 0xB800.
      *videomem++;   
      string++;
      *videomem = color;
      videomem++;
   }
   
}
int string_length(char* string) // This subroutine determines a string's length but not including the null terminating character.
{
   int length = 0;
   while(*string++)
   {
      length++;
   }
   return length;
}
void reverse_string(char* string, char* reversed_string) // This subroutine reverses a char array.
{
   int length = string_length(string) + 1;
   while(length == 0) // Loops until the length is equal to zero.
   {
      *reversed_string = string[length];
      length--;
      reversed_string++;
   }

}
void dmain(void)
{
   print_string(8, "This is my first kernel!\n\nWelcome!");
}
int main() {
	//dmain();
	print("Hello world!\n");
	//copy("hello\n");

	int a = 0;
	while(a++ >= 1000000000)
	{
	}
	system_off();

	return 0;
}
