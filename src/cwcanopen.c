#include <cwutils/cwcanopen.h>
#include <cwutils/cwfuture.h>
#include <cwutils/cwtimer.h>
#include <cwutils/cwlog.h>
#include <canopen/canopen.h>

#include <unistd.h>

typedef struct CanTransaction {
    CanOpenListener* listener;
	CwFuture* timeout;
	void* data;
} CanTransaction;

static CanTransaction* can_transaction_new(CwArena* a, CanOpenListener* listener, CwFuture* timeout, void* data) {
	CanTransaction* self = cwnew(a, CanTransaction);
	self -> listener = listener;
	self -> timeout = timeout;
	self -> data = data;
	return self;
}

static int poll_transaction(int pc, void* data, CwFuture* self) {
    CanTransaction* t = (CanTransaction*)(data);

    enum Labels { Finished = 0, Setup = 1, Running, CleanUp, TimedOut, CanOpenError };

	switch (pc) {
    	case Setup:
        	return Running;

    	case Running:
        	usleep(100 * 1000);
        	if (cwfuture_poll(t -> timeout) < 1) return TimedOut;

        	self -> err = canopen_handle_all_frames(t -> listener);
        	if (self -> err) return CanOpenError; 
        	if (canopen_client_is_busy(t -> listener -> client)) return Running;

        	return Finished;


    	case TimedOut:
        	canopen_client_abort(t -> listener -> client, SdoErrorTimeOut);
        	return Finished;

    	case CanOpenError:
        	return Finished;

    	case Finished:       return 0;
    	default:             return 0;
	}
}

CwFuture* canopen_init_write_future(CwArena a, CanOpenListener* listener, CanOpenAddress dest, enum CanOpenType type, void* src) {
    CwFuture* timeout    = cwtimeout_ms(cwarena_section(&a, 0x100), 1000);
    CanTransaction* data = can_transaction_new(&a, listener, timeout, src);
    CwFuture* output     = cwfuture_new(a, poll_transaction, data);
    output -> err        = canopen_init_write(listener, dest, type, src);
    if (output -> err) output -> pc = 0;

    return output;
	// return cwfuture_new(a, poll_transaction, can_transaction_new(a, listener, cwtimeout_ms(a, 1000), src)); 
}

CwFuture* canopen_init_read_future(CwArena a, CanOpenListener* listener, CanOpenAddress src, enum CanOpenType type, void* dest) {
    CwFuture* timeout    = cwtimeout_ms(cwarena_section(&a, 0x100), 1000);
    CanTransaction* data = can_transaction_new(&a, listener, timeout, NULL);
    CwFuture* output     = cwfuture_new(a, poll_transaction, data);
    output -> err        = canopen_init_read(listener, src, type, dest);
    if (output -> err) output -> pc = 0;

    return output;

	// int err = canopen_init_read(listener, src, type, dest); (void)err;
	// return cwfuture_new(a, poll_transaction, can_transaction_new(a, listener, cwtimeout_ms(a, 1000), NULL)); 
}

CwFuture* canopen_write_u8(CwArena a, CanOpenListener* listener, CanOpenAddress dest, uint8_t value) {
    uint8_t* src = cwnew(&a, uint8_t);
    *src = value;
    return canopen_init_write_future(a, listener, dest, CanOpenTypeU8, src);
}

CwFuture* canopen_write_u16(CwArena a, CanOpenListener* listener, CanOpenAddress dest, uint16_t value) {
    uint16_t* src = cwnew(&a, uint16_t);
    *src = value;
    return canopen_init_write_future(a, listener, dest, CanOpenTypeU16, src);
}

CwFuture* canopen_write_u32(CwArena a, CanOpenListener* listener, CanOpenAddress dest, uint32_t value) {
    uint32_t* src = cwnew(&a, uint32_t);
    *src = value;
    return canopen_init_write_future(a, listener, dest, CanOpenTypeU32, src);
}

CwFuture* canopen_read_u8(CwArena a, CanOpenListener* listener, CanOpenAddress src, uint8_t* dest) {
    return canopen_init_read_future(a, listener, src, CanOpenTypeU8, dest);
}

CwFuture* canopen_read_u16(CwArena a, CanOpenListener* listener, CanOpenAddress src, uint16_t* dest) {
    return canopen_init_read_future(a, listener, src, CanOpenTypeU16, dest);
}

CwFuture* canopen_read_u32(CwArena a, CanOpenListener* listener, CanOpenAddress src, uint32_t* dest) {
    return canopen_init_read_future(a, listener, src, CanOpenTypeU32, dest);
}


static int canopen_cwstring_reader_init(void* self_v, void* src_v) {
	CwString* src  = (CwString*)(src_v);
	CwString* self = (CwString*)(self_v);

	free(self -> ptr);
	*self = *src;

	return 0;
}

static int canopen_cwstring_reader_read(void* self_v, uint8_t* dest, size_t size, bool* is_over) {
    CwString* self = (CwString*)(self_v);

	size_t i;
	for (i=0; i<size && self -> size > 0; i++, self -> size--) {
    	dest[i] = self -> ptr[i];
    	fprintf(stdout, "%02x ", dest[i]);
	}
	fprintf(stdout, "\n");
	self -> ptr += i;

    if (self -> size == 0) *is_over = true;
	return i;
}

static int canopen_cwstring_reader_remaining(void* self_v) {
    CwString* self = (CwString*)(self_v);
    return self -> size;
}


static int canopen_cwstring_writer_init(void* self_v, void* dest_v) {
	CwString*  dest = (CwString* )(dest_v);
	CwString** self = (CwString**)(self_v);
	*self = dest;
	return 0;
}

static int canopen_cwstring_writer_write(void* self_v, uint8_t* src, size_t size) {
    CwString* self = *((CwString**)(self_v));
    cwstring_append_slice(self, src, size);
	return 0;
}

static int canopen_cwstring_writer_end(void* self_v) {
    (void)(self_v);
    return 0;
}

CanOpenReaderVTable canopen_cwstring_reader = {
    .init = canopen_cwstring_reader_init,
    .read = canopen_cwstring_reader_read,
    .remaining = canopen_cwstring_reader_remaining,
};

CanOpenWriterVTable canopen_cwstring_writer = {
    .init  = canopen_cwstring_writer_init,
    .write = canopen_cwstring_writer_write,
    .end   = canopen_cwstring_writer_end,
};
