#include "sd_card.h"
#include <asf.h>
#include <string.h>

#define _MAX_NUM_SD_FILES 4

typedef struct
{
	FIL file_object;
	bool is_open;
} _sd_file_t;

bool _driver_created = false;
_sd_file_t _files[_MAX_NUM_SD_FILES];
FATFS _fs;

uint16_t _MakeSdPath(char* targetBuffer, const char* filePath, int filePathLength)
{
	//first two characters need to be "0:"
	targetBuffer[0] = '0';
	targetBuffer[0] = LUN_ID_SD_MMC_0_MEM + '0';
	targetBuffer[1] = ':';
	strncpy(&targetBuffer[2], filePath, filePathLength);
	targetBuffer[filePathLength+2] = '\0';

	return filePathLength+2;
}

int SdCard_Create(void)
{
	if(!_driver_created)
	{
		FRESULT res;
		Ctrl_status status;
		//need to init the sd card stack
		sd_mmc_init();
		
		//look for card detect
		do
		{
			status = sd_mmc_test_unit_ready(0);
			if (CTRL_FAIL == status)
			{
				//printf("Card install FAIL\n\r");
				//printf("Please unplug and re-plug the card.\n\r");
				while (CTRL_NO_PRESENT != sd_mmc_check(0))
				{
					sd_mmc_init();
				}
			}
		} while (CTRL_GOOD != status);
		
		//mount the card on  the fat file system
		memset(&_fs, 0, sizeof(FATFS));
		res = f_mount(LUN_ID_SD_MMC_0_MEM, &_fs);
		if (FR_INVALID_DRIVE == res) 
		{
			return -1;
		}

		for(int i = 0; i < _MAX_NUM_SD_FILES; i++)
		{
			_files[i].is_open = false;
		}
	}

	_driver_created = true;
	return 0;
}

sd_file_t SdCard_OpenFile(char* file_path, uint16_t file_path_length, uint8_t file_mode)
{
	char path_buffer[256];
	_sd_file_t* file_ptr = NULL;
	FRESULT res;

	//find a free file handle
	for(int i = 0; i < _MAX_NUM_SD_FILES; i++)
	{
		if(_files[i].is_open == false)
		{
			_files[i].is_open = true;
			file_ptr = &_files[i];
		}
	}

	//if we couldn't find a free file to use, fail
	if(file_ptr == NULL)
	{
		return NULL;
	}

	_MakeSdPath(path_buffer, file_path, file_path_length);
	res = f_open(&file_ptr->file_object, (char const*)path_buffer, file_mode);
	if(res != FR_OK)
	{
		return NULL;
	}
	else
	{
		file_ptr->is_open = true;
		return file_ptr;
	}
}

int SdCard_CloseFile(sd_file_t file)
{
	_sd_file_t* handle = (_sd_file_t*)file;
	f_close(&handle->file_object);
	handle->is_open = false;
	return 0;
}

int SdCard_Read(sd_file_t file, void* buffer, uint32_t buffer_size)
{
	_sd_file_t* handle = (_sd_file_t*)file;
	if(!handle->is_open)
	{
		return -1; //error no file open
	}
	
	UINT bytes_read = 0;
	f_read(&handle->file_object, buffer, buffer_size, &bytes_read);
	return bytes_read;
}

int SdCard_Write(sd_file_t file, void* buffer, uint32_t buffer_size)
{
	_sd_file_t* handle = (_sd_file_t*)file;
	if(!handle->is_open)
	{
		return -1; //file not open
	}
	UINT bytes_read = 0;
	f_write(&handle->file_object, buffer, buffer_size, &bytes_read);
	return bytes_read;
}

int SdCard_GetFileSize(sd_file_t file)
{
	_sd_file_t* handle = (_sd_file_t*)file;
	if(!handle->is_open)
	{
		return -1;
	}
	return f_size(&handle->file_object);
}