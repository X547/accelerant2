#pragma once

#include "AccelerantRoster.h"
#include <GraphicsDefs.h>


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
