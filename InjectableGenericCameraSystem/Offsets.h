#pragma once

// Offsets for camera intercept code. 
// TODO: Add AOB scanning to replace these offsets.
#define MATRIX_ADDRESS_INTERCEPT_START_OFFSET		0x41C5B9A		// example from Hitman 2016 (v1.7)
#define MATRIX_ADDRESS_INTERCEPT_CONTINUE_OFFSET	0x41C5BA8		// example from Hitman 2016 (v1.7)

#define	MATRIX_WRITE_INTERCEPT_START_OFFSET			0x41C9150		// example from Hitman 2016 (v1.7)
#define MATRIX_WRITE_INTERCEPT_CONTINUE_OFFSET		0x41C9171		// example from Hitman 2016 (v1.7)