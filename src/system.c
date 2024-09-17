#include <system.h>
#include <canopen/canopen.h>
#include <cwutils/cwlog.h>
#include <timer.h>

extern CwLogger* logger;

void on_heartbeat(void* ptr, CanOpenFrame* frame) {
    System* self = (System*)(ptr);
	cwlog(self -> logger, CWLOG_DEBUG, "heartbeat");

    enum NmtState* device;
    switch (frame -> id) {
        case 0x708: device = &self -> imperio_state; break;
        case 0x70A: device = &self -> battery_state; break;
        default: return;
    }

    switch (frame -> data[0]) {
        case 0x04: *device = NmtOff;   return; 
        case 0x7F: *device = NmtPreOp; return; 
        case 0x05: *device = NmtOp;    return; 
        default: return;
    }
}

typedef struct TransactionState {
    CanOpenListener* listener;
    CwLogger* logger;
    CwFuture* timeout;
} TransactionState;

static int poll_transaction(int pc, void* data, CwFuture* self) {
    TransactionState* state = (TransactionState*)(data);
    (void)(self);

    int err = canopen_handle_all_frames(state -> listener);
    if (err) return -1;

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

CwFuture* canopen_init_write_future(CanOpenListener* listener, CanOpenAddress dest, enum CanOpenType type, void* src) {
	TransactionState* state = malloc(sizeof(TransactionState));
	state -> listener = listener;
	state -> logger = NULL;

	int err = canopen_init_write(listener, dest, type, src);
	(void)err;
	CwFuture* output = cwfuture_new(poll_transaction, state); 

	cwfuture_abort_on(output, timeout_ms(200));
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

	cwfuture_abort_on(output, timeout_ms(200));
	cwfuture_on_cleanup(output, free, state);
	return output;

}

CwFuture* canopen_write_u8(CanOpenListener* listener, CanOpenAddress dest, uint8_t value) {
    uint8_t* src = malloc(sizeof(uint8_t));
    *src = value;
    return canopen_init_write_future(listener, dest, CanOpenTypeU8, src);
}

CwFuture* canopen_write_u16(CanOpenListener* listener, CanOpenAddress dest, uint16_t value) {
    uint8_t* src = malloc(sizeof(uint8_t));
    *src = value;
    return canopen_init_write_future(listener, dest, CanOpenTypeU16, src);
}

CwFuture* canopen_write_u32(CanOpenListener* listener, CanOpenAddress dest, uint32_t value) {
    uint8_t* src = malloc(sizeof(uint8_t));
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
