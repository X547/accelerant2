#pragma once

#include "AccelerantRoster.h"


#define B_ACCELERANT_IFACE_DRM "drm/v1"

struct drm_version;

#ifdef __cplusplus

class _EXPORT AccelerantDrm {
public:
	virtual void *DrmMmap(void *addr, size_t length, int prot, int flags, off_t offset) = 0;
	virtual int DrmIoctl(uint32_t request, void *arg) = 0;

	virtual int DrmVersion(struct drm_version *version) = 0;

	virtual int DrmBoClose(uint32_t handle) = 0;
	virtual int DrmBoExportFd(uint32_t handle, uint32_t flags, int *fd) = 0;
	virtual int DrmBoImportFd(int fd, uint32_t *handle) = 0;

	virtual int DrmSyncobjCreate(uint32_t flags, uint32_t *handle) = 0;
	virtual int DrmSyncobjDestroy(uint32_t handle) = 0;
	virtual int DrmSyncobjExportFd(uint32_t handle, int *obj_fd) = 0;
	virtual int DrmSyncobjImportFd(int obj_fd, uint32_t *handle) = 0;
	virtual int DrmSyncobjExportSyncFile(uint32_t handle, int *sync_file_fd) = 0;
	virtual int DrmSyncobjImportSyncFile(uint32_t handle, int sync_file_fd) = 0;
	virtual int DrmSyncobjWait(uint32_t *handles, uint32_t num_handles, int64_t timeout_nsec, uint32_t flags, uint32_t *first_signaled) = 0;
	virtual int DrmSyncobjReset(const uint32_t *handles, uint32_t handle_count) = 0;
	virtual int DrmSyncobjSignal(const uint32_t *handles, uint32_t handle_count) = 0;
	virtual int DrmSyncobjTimelineSignal(const uint32_t *handles, uint64_t *points, uint32_t handle_count) = 0;
	virtual int DrmSyncobjTimelineWait(uint32_t *handles, uint64_t *points, uint32_t num_handles, int64_t timeout_nsec, uint32_t flags, uint32_t *first_signaled) = 0;
	virtual int DrmSyncobjQuery(uint32_t *handles, uint64_t *points, uint32_t handle_count, uint32_t flags) = 0;
	virtual int DrmSyncobjTransfer(uint32_t dst_handle, uint64_t dst_point, uint32_t src_handle, uint64_t src_point, uint32_t flags) = 0;
	virtual int DrmSyncobjAccumulate(uint32_t syncobj1, uint32_t syncobj2, uint64_t point) = 0;
};

#endif

typedef struct accelerant_drm {
	struct accelerant_drm_vtable *vt;
} accelerant_drm;

typedef struct accelerant_drm_vtable {
	void (*DrmMmap)(accelerant_drm *acc, void *addr, size_t length, int prot, int flags, off_t offset);
	int (*DrmIoctl)(accelerant_drm *acc, uint32_t request, void *arg);

	int (*DrmVersion)(accelerant_drm *acc, struct drm_version *version);

	int (*DrmBoClose)(accelerant_drm *acc, uint32_t handle);
	int (*DrmBoExportFd)(accelerant_drm *acc, uint32_t handle, uint32_t flags, int *fd);
	int (*DrmBoImportFd)(accelerant_drm *acc, int fd, uint32_t *handle);

	int (*DrmSyncobjCreate)(accelerant_drm *acc, uint32_t flags, uint32_t *handle);
	int (*DrmSyncobjDestroy)(accelerant_drm *acc, uint32_t handle);
	int (*DrmSyncobjExportFd)(accelerant_drm *acc, uint32_t handle, int *obj_fd);
	int (*DrmSyncobjImportFd)(accelerant_drm *acc, int obj_fd, uint32_t *handle);
	int (*DrmSyncobjExportSyncFile)(accelerant_drm *acc, uint32_t handle, int *sync_file_fd);
	int (*DrmSyncobjImportSyncFile)(accelerant_drm *acc, uint32_t handle, int sync_file_fd);
	int (*DrmSyncobjWait)(accelerant_drm *acc, uint32_t *handles, uint32_t num_handles, int64_t timeout_nsec, uint32_t flags, uint32_t *first_signaled);
	int (*DrmSyncobjReset)(accelerant_drm *acc, const uint32_t *handles, uint32_t handle_count);
	int (*DrmSyncobjSignal)(accelerant_drm *acc, const uint32_t *handles, uint32_t handle_count);
	int (*DrmSyncobjTimelineSignal)(accelerant_drm *acc, const uint32_t *handles, uint64_t *points, uint32_t handle_count);
	int (*DrmSyncobjTimelineWait)(accelerant_drm *acc, uint32_t *handles, uint64_t *points, uint32_t num_handles, int64_t timeout_nsec, uint32_t flags, uint32_t *first_signaled);
	int (*DrmSyncobjQuery)(accelerant_drm *acc, uint32_t *handles, uint64_t *points, uint32_t handle_count, uint32_t flags);
	int (*DrmSyncobjTransfer)(accelerant_drm *acc, uint32_t dst_handle, uint64_t dst_point, uint32_t src_handle, uint64_t src_point, uint32_t flags);
	int (*DrmSyncobjAccumulate)(accelerant_drm *acc, uint32_t syncobj1, uint32_t syncobj2, uint64_t point);
} accelerant_drm_vtable;
