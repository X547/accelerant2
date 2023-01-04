#pragma once

#include <Referenceable.h>
#include <GraphicsDefs.h>

class BMessenger;
class AccelerantHolder;
class AccelerantRosterPrivate;


class _EXPORT Accelerant: public BReferenceable {
private:
	friend class AccelerantRoster;
	AccelerantHolder* fHolder{};

public:
	virtual ~Accelerant() = default;
	void LastReferenceReleased() override;
};

class _EXPORT AccelerantDrm {
public:
	virtual ~AccelerantDrm() = default;

	virtual void *DrmMmap(void *addr, size_t length, int prot, int flags, off_t offset) = 0;
	virtual int DrmIoctl(unsigned long request, void *arg) = 0;
};

class _EXPORT AccelerantDisplay {
public:
	union CursorUpdateValid {
		struct {
			uint32 enabled: 1;
			uint32 pos: 1;
			uint32 org: 1;
			uint32 buffer: 1;
			uint32 format: 1;
		};
		uint32 val;
	};

	struct CursorUpdateInfo {
		CursorUpdateValid valid;

		bool enabled;
		int32 x, y;
		int32 orgX, orgY;

		uint8 *buffer;

		int32 bytesPerRow;
		uint32 width, height;
		color_space colorSpace;
	};

	virtual ~AccelerantDisplay() = default;

	virtual status_t DisplayGetConsumer(int32 crtc, BMessenger &consumer) = 0;
	virtual status_t DisplayUpdateCursor(int32 crtc, const CursorUpdateInfo &info) = 0;
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
