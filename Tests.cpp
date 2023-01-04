#include "AccelerantRoster.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <AutoDeleterPosix.h>

static inline void Check(status_t res) {if (res < B_OK) abort();}


int main()
{
	FileDescriptorCloser fd(open("/dev/graphics/radeon_gfx_010000", B_READ_WRITE | O_CLOEXEC));
	Accelerant *acc = NULL;
	AccelerantRoster::Instance().FromFd(acc, fd.Get());
	BReference<Accelerant> accDeleter(acc, true);
	printf("acc: %p\n", acc);
	return 0;
}
