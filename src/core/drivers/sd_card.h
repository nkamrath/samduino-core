#ifndef _DRIVERS_SD_CARD_H_
#define _DRIVERS_SD_CARD_H_

#include <stdint.h>
#include <stdbool.h>

#define SD_FILE_WRITE_FLAG 				FA_WRITE
#define SD_FILE_CREATE_ALWAYS_FLAG 		FA_CREATE_ALWAYS
#define SD_FILE_CREATE_NEW_FLAG 		FA_CREATE_NEW
#define SD_FILE_READ_FLAG 				FA_READ
#define SD_FILE_OPEN_ALWAYS				FA_OPEN_ALWAYS

typedef void* sd_file_t;

int SdCard_Create(void);
sd_file_t SdCard_OpenFile(char* file_path, uint16_t file_path_length, uint8_t file_mode);
int SdCard_CloseFile(sd_file_t file);
int SdCard_Read(sd_file_t file, void* buffer, uint32_t buffer_size_bytes);
int SdCard_Write(sd_file_t file, void* buffer, uint32_t buffer_size_bytes);
int SdCard_GetFileSize(sd_file_t file);

#endif