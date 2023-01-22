#pragma once

#include "AccelerantRoster.h"


#define B_ACCELERANT_IFACE_AMDGPU "amdgpu/v1"

#ifdef __cplusplus

class _EXPORT AccelerantAmdgpu {
public:
	virtual int AmdgpuQueryInfo(struct drm_amdgpu_info *info) = 0;
	virtual int AmdgpuBoAlloc(struct amdgpu_bo_alloc_request *alloc_buffer, uint32_t *buf_handle) = 0;
	virtual int AmdgpuCreateBoFromUserMem(void *cpu, uint64_t size, uint32_t *buf_handle) = 0;
	virtual int AmdgpuBoQueryInfo(uint32_t bo, struct amdgpu_bo_info *info) = 0;
	virtual int AmdgpuBoSetMetadata(uint32_t bo, struct amdgpu_bo_metadata *info) = 0;
	virtual int AmdgpuBoVaOpRaw(uint32_t bo, uint64_t offset, uint64_t size, uint64_t addr, uint64_t flags, uint32_t ops) = 0;
	virtual int AmdgpuBoCpuMap(uint32_t bo, void **cpu) = 0;
	virtual int AmdgpuCsSubmitRaw(uint32_t context_id, uint32_t bo_list_handle, int num_chunks, struct drm_amdgpu_cs_chunk *chunks, uint64_t *seq_no) = 0;
	virtual int AmdgpuWaitCs(uint32_t ctx_id, unsigned ip, unsigned ip_instance, uint32_t ring, uint64_t handle, uint64_t timeout_ns, bool *busy) = 0;
	virtual int AmdgpuCtxRaw(union drm_amdgpu_ctx *args) = 0;
};

#endif

typedef struct accelerant_amdgpu {
	struct accelerant_amdgpu_vtable *vt;
} accelerant_amdgpu;

typedef struct accelerant_amdgpu_vtable {
	int (*AmdgpuQueryInfo)(accelerant_amdgpu *acc, struct drm_amdgpu_info *info);
	int (*AmdgpuBoAlloc)(accelerant_amdgpu *acc, struct amdgpu_bo_alloc_request *alloc_buffer, uint32_t *buf_handle);
	int (*AmdgpuCreateBoFromUserMem)(accelerant_amdgpu *acc, void *cpu, uint64_t size, uint32_t *buf_handle);
	int (*AmdgpuBoQueryInfo)(accelerant_amdgpu *acc, uint32_t bo, struct amdgpu_bo_info *info);
	int (*AmdgpuBoSetMetadata)(accelerant_amdgpu *acc, uint32_t bo, struct amdgpu_bo_metadata *info);
	int (*AmdgpuBoVaOpRaw)(accelerant_amdgpu *acc, uint32_t bo, uint64_t offset, uint64_t size, uint64_t addr, uint64_t flags, uint32_t ops);
	int (*AmdgpuBoCpuMap)(accelerant_amdgpu *acc, uint32_t bo, void **cpu);
	int (*AmdgpuCsSubmitRaw)(accelerant_amdgpu *acc, uint32_t context_id, uint32_t bo_list_handle, int num_chunks, struct drm_amdgpu_cs_chunk *chunks, uint64_t *seq_no);
	int (*AmdgpuWaitCs)(accelerant_amdgpu *acc, uint32_t ctx_id, unsigned ip, unsigned ip_instance, uint32_t ring, uint64_t handle, uint64_t timeout_ns, bool *busy);
	int (*AmdgpuCtxRaw)(accelerant_amdgpu *acc, union drm_amdgpu_ctx *args);
} accelerant_amdgpu_vtable;
