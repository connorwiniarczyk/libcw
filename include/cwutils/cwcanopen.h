#ifndef CWCANOPEN_H
#define CWCANOPEN_H

#include <canopen/canopen.h>
#include <cwutils/cwfuture.h>
#include <cwutils/cwarena.h>

CwFuture* canopen_init_write_future (CwArena a, CanOpenListener* listener, CanOpenAddress dest, enum CanOpenType type, void* src);
CwFuture* canopen_init_read_future  (CwArena a, CanOpenListener* listener, CanOpenAddress src,  enum CanOpenType type, void* dest);

CwFuture* canopen_write_u8          (CwArena a, CanOpenListener* listener, CanOpenAddress dest, uint8_t value);
CwFuture* canopen_write_u16         (CwArena a, CanOpenListener* listener, CanOpenAddress dest, uint16_t value);
CwFuture* canopen_write_u32         (CwArena a, CanOpenListener* listener, CanOpenAddress dest, uint32_t value);
CwFuture* canopen_read_u8           (CwArena a, CanOpenListener* listener, CanOpenAddress src,  uint8_t* dest);
CwFuture* canopen_read_u16          (CwArena a, CanOpenListener* listener, CanOpenAddress src,  uint16_t* dest);
CwFuture* canopen_read_u32          (CwArena a, CanOpenListener* listener, CanOpenAddress src,  uint32_t* dest);

extern CanOpenReaderVTable canopen_cwstring_reader;
extern CanOpenWriterVTable canopen_cwstring_writer;

extern CanOpenReaderVTable cwstr_reader;

#endif
