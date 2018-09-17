#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>

#define NEXT_MESSAGE 0xab
#define END_MESSAGE 0xac
#define PAGE_SIZE 4096
#define UART_NAME "/dev/ttyUSB0"
#define FILE_NAME "file.bin"

void main(void)
{
	unsigned char buf[PAGE_SIZE] = {0};
	int i = 0;
	char end = END_MESSAGE;
	int j = 0;
	int uart = 0;
	FILE *file = 0;
	
	printf("Start loading\n");
	
	/* Открывает устройство UART */
	uart = open(UART_NAME, O_RDWR | O_NOCTTY);
	if (0 == uart)
	{
		printf("Can't open UART\n");
		return;
	}
	struct termios options;
	tcgetattr(uart, &options);
	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);
	options.c_cflag &= ~CSIZE;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~PARENB;
	options.c_cflag |= CS8;
	tcsetattr(uart, TCSANOW, &options);
	
	/* Открываем отправляемый файл */
	file = fopen(FILE_NAME, "r");
	if (0 == file)
	{
		printf("Can't open file\n");
		close(uart);
		return;
	}

	/* Цикл отправки файла через UART */
	while (1)
	{
		/* Ждем приема запроса на отправку следующей части */
		read(uart, buf, 1);
		if (NEXT_MESSAGE == buf[0])
		{
			/* Если достигнут конец файла - отправляем сообщение об окончании передачи, иначе передаем следующую часть */
			if (feof(file))
			{
				printf("End\n");
				write(uart, &end, 1);
			}
			else
			{
				memset(buf, 0, PAGE_SIZE);
				fread(buf, 1, PAGE_SIZE, file);
				printf("Next %d\n", ++i);
				for (j = 0; j < PAGE_SIZE; j++)
				{
					write(uart, &buf[j], 1);
				}
				printf("Waiting\n");
			}
		}
	}

	close(uart);
}