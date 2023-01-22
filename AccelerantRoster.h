#pragma once

#include <SupportDefs.h>

#ifdef __cplusplus

#include <Referenceable.h>

class BMessenger;
class AccelerantHolder;
class AccelerantRosterPrivate;


class _EXPORT AccelerantBase {
public:
	virtual int32 AcquireReference() = 0;
	virtual int32 ReleaseReference() = 0;
	virtual void *QueryInterface(const char *iface) = 0;
};

class _EXPORT Accelerant: public BReferenceable, public AccelerantBase {
private:
	friend class AccelerantRoster;
	AccelerantHolder* fHolder{};

public:
	virtual ~Accelerant() = default;
	void LastReferenceReleased() override;

	int32 AcquireReference() final;
	int32 ReleaseReference() final;
	void *QueryInterface(const char *iface) override;
};


extern "C" {

status_t _EXPORT instantiate_accelerant(Accelerant **acc, int fd);

};


class _EXPORT AccelerantRoster {
private:
	friend class Accelerant;
	AccelerantRosterPrivate& fPrivate;

	AccelerantRoster(AccelerantRosterPrivate& data);

	void Unregister(AccelerantHolder *holder);
	status_t Load(Accelerant *&acc, void *&addon, int fd);

public:
	static AccelerantRoster &Instance();
	status_t FromFd(Accelerant *&acc, int fd);
};

#endif

#define B_ACCELERANT_IFACE_BASE "base/v1"

typedef struct accelerant_base {
	struct accelerant_base_vtable *vt;
} accelerant_base;

typedef struct accelerant_base_vtable {
	int32 (*AcquireReference)(accelerant_base *acc);
	int32 (*ReleaseReference)(accelerant_base *acc);
	void *(*QueryInterface)(accelerant_base *acc, const char *iface);
} accelerant_base_vtable;
