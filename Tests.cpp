#include "AccelerantRoster.h"
#include "AccelerantDrm.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <AutoDeleterPosix.h>

static inline void Check(status_t res) {if (res < B_OK) abort();}

struct drm_version {
	int version_major;	  /**< Major version */
	int version_minor;	  /**< Minor version */
	int version_patchlevel;	  /**< Patch level */
	size_t name_len;	  /**< Length of name buffer */
	char *name;	  /**< Name of driver */
	size_t date_len;	  /**< Length of date buffer */
	char *date;	  /**< User-space buffer to hold date */
	size_t desc_len;	  /**< Length of desc buffer */
	char *desc;	  /**< User-space buffer to hold desc */
};


static void CAccessTest(accelerant_base *accBase)
{
	printf("accBase: %p\n", accBase);
	accelerant_drm *accDrm = (accelerant_drm*)accBase->vt->QueryInterface(accBase, B_ACCELERANT_IFACE_DRM, 0);
	printf("accDrm: %p\n", accDrm);

	char drmName[256];
	char drmDate[256];
	char drmDesc[256];
	drm_version version {
		.name_len = sizeof(drmName),
		.name = &drmName[0],
		.date_len = sizeof(drmDate),
		.date = &drmDate[0],
		.desc_len = sizeof(drmDesc),
		.desc = &drmDesc[0]
	};
	accDrm->vt->DrmVersion(accDrm, &version);

	printf("DRM version: %d.%d.%d\n", version.version_major, version.version_major, version.version_minor, version.version_patchlevel);
	printf("name: \"%s\"\n", version.name);
	printf("date: \"%s\"\n", version.date);
	printf("desc: \"%s\"\n", version.desc);
}


int main()
{
	FileDescriptorCloser fd(open("/dev/graphics/radeon_gfx_010000", B_READ_WRITE | O_CLOEXEC));
	Accelerant *acc = NULL;
	AccelerantRoster::Instance().FromFd(acc, fd.Get());
	BReference<Accelerant> accDeleter(acc, true);

	printf("acc: %p\n", acc);

	CAccessTest((accelerant_base*)static_cast<AccelerantBase*>(acc));

	return 0;
}
