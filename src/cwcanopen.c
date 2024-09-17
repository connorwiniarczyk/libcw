#include <cwutils/cwcanopen.h>
#include <cwutils/cwfuture.h>
#include <cwutils/cwtimer.h>
#include <cwutils/cwlog.h>
#include <canopen/canopen.h>

typedef struct TransactionState {
    CanOpenListener* listener;
    CwLogger* logger;
    CwFuture* timeout;
} TransactionState;

static int poll_transaction(int pc, void* data, CwFuture* self) {
    TransactionState* state = (TransactionState*)(data);
    (void)(self);

    int err = canopen_handle_all_frames(state -> listener);
    if (err) {
        fprintf(stderr, "sdo failed: %s\n", canopen_error_message(err));
        fprintf(stderr, "%s\n", canopen_sdo_error_message(state -> listener -> client -> error));
        return -1;
    }

	switch (pc) {
        case 1: return canopen_client_is_busy(state -> listener -> client) ? pc : pc + 1;
        case 2:
            switch (state -> listener -> client -> state) {
                case CanOpenClientStateAbortedLocal: return -1;
                case CanOpenClientStateAbortedRemote: return -1;
                case CanOpenClientStateSuccess: return 0;
                default: return -1;
            }
        default: return -1;
	}
}

void canopen_future_on_abort(void* data) {
    // CanOpenListener* l = (CanOpenListener*)(data);
	(void)canopen_client_abort(data, SdoErrorTimeOut);
}

CwFuture* canopen_init_write_future(CanOpenListener* listener, CanOpenAddress dest, enum CanOpenType type, void* src) {
	TransactionState* state = malloc(sizeof(TransactionState));
	state -> listener = listener;
	state -> logger = NULL;

	int err = canopen_init_write(listener, dest, type, src);
	(void)err;
	CwFuture* output = cwfuture_new(poll_transaction, state); 


	CwFuture* timeout = cwtimeout_ms(200);
	cwfuture_on_success(timeout, canopen_future_on_abort, listener -> client);

	// cwfuture_abort_on(output, timeout);
	cwfuture_on_cleanup(output, free, state);
	cwfuture_on_cleanup(output, free, src);
	return output;

}

CwFuture* canopen_init_read_future(CanOpenListener* listener, CanOpenAddress src, enum CanOpenType type, void* dest) {
	TransactionState* state = malloc(sizeof(TransactionState));
	state -> listener = listener;
	state -> logger = NULL;

	int err = canopen_init_read(listener, src, type, dest);
	(void)err;
	CwFuture* output = cwfuture_new(poll_transaction, state); 

	cwfuture_abort_on(output, cwtimeout_ms(200));
	cwfuture_on_cleanup(output, free, state);
	return output;

}

CwFuture* canopen_write_u8(CanOpenListener* listener, CanOpenAddress dest, uint8_t value) {
    uint8_t* src = malloc(sizeof(uint8_t));
    *src = value;
    return canopen_init_write_future(listener, dest, CanOpenTypeU8, src);
}

CwFuture* canopen_write_u16(CanOpenListener* listener, CanOpenAddress dest, uint16_t value) {
    uint16_t* src = malloc(sizeof(uint16_t));
    *src = value;
    return canopen_init_write_future(listener, dest, CanOpenTypeU16, src);
}

CwFuture* canopen_write_u32(CanOpenListener* listener, CanOpenAddress dest, uint32_t value) {
    uint32_t* src = malloc(sizeof(uint32_t));
    *src = value;
    return canopen_init_write_future(listener, dest, CanOpenTypeU32, src);
}

CwFuture* canopen_read_u8(CanOpenListener* listener, CanOpenAddress src, uint8_t* dest) {
    return canopen_init_read_future(listener, src, CanOpenTypeU8, dest);
}

CwFuture* canopen_read_u16(CanOpenListener* listener, CanOpenAddress src, uint16_t* dest) {
    return canopen_init_read_future(listener, src, CanOpenTypeU16, dest);
}

CwFuture* canopen_read_u32(CanOpenListener* listener, CanOpenAddress src, uint32_t* dest) {
    return canopen_init_read_future(listener, src, CanOpenTypeU32, dest);
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
