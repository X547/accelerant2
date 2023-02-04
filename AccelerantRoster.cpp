#include "AccelerantRoster.h"

#include <string.h>
#include <dlfcn.h>
#include <util/AVLTree.h>
#include <sys/stat.h>
#include <String.h>
#include <FindDirectory.h>
#include <graphic_driver.h>
#include <AutoDeleterPosix.h>
#include <locks.h>

#include <new>

#define CheckRet(err) {status_t _err = (err); if (_err < B_OK) return _err;}


struct AccelerantHolder {
	struct Key {
		dev_t dev;
		ino_t ino;

		inline int Compare(const Key &b) const
		{
			if (dev < b.dev) return -1;
			if (dev > b.dev) return 1;
			if (ino < b.ino) return -1;
			if (ino > b.ino) return 1;
			return 0;
		}
	};

	FileDescriptorCloser fFd;
	Key fKey{};
	AVLTreeNode fNode;

	Accelerant *fAcc;
	CObjectDeleter<void, int, dlclose> fAddon;

	struct NodeDef {
		typedef AccelerantHolder::Key Key;
		typedef AccelerantHolder Value;

		inline AVLTreeNode* GetAVLTreeNode(Value* value) const
		{
			return &value->fNode;
		}

		inline Value* GetValue(AVLTreeNode* node) const
		{
			return (Value*)((char*)node - offsetof(AccelerantHolder, fNode));
		}

		inline int Compare(const Key& a, const Value* b) const
		{
			return a.Compare(b->fKey);
		}

		inline int Compare(const Value* a, const Value* b) const
		{
			return a->fKey.Compare(b->fKey);
		}
	};
};


struct AccelerantRosterPrivate {
	recursive_lock fLock = RECURSIVE_LOCK_INITIALIZER("AccelerantRoster");
	AVLTree<AccelerantHolder::NodeDef> fAccMap;
};


// #pragma mark - Accelerant

void Accelerant::LastReferenceReleased()
{
	printf("Accelerant::LastReferenceReleased()\n");
	AccelerantHolder *holder = fHolder;
	delete this;
	AccelerantRoster::Instance().Unregister(holder);
}

int32 Accelerant::AcquireReference()
{
	return BReferenceable::AcquireReference();
}

int32 Accelerant::ReleaseReference()
{
	return BReferenceable::ReleaseReference();
}

void *Accelerant::QueryInterface(const char *iface, uint32 version)
{
	if (strcmp(iface, B_ACCELERANT_IFACE_BASE) == 0 && version <= 0) {
		return static_cast<AccelerantBase*>(this);
	}
	return NULL;
}


// #pragma mark - AccelerantRoster

AccelerantRoster::AccelerantRoster(AccelerantRosterPrivate& data): fPrivate(data)
{
}

AccelerantRoster &AccelerantRoster::Instance()
{
	static AccelerantRosterPrivate data;
	static AccelerantRoster instance(data);
	return instance;
}

void AccelerantRoster::Unregister(AccelerantHolder *holder)
{
	RecursiveLocker lock(fPrivate.fLock);
	if (holder == NULL) return;
	fPrivate.fAccMap.Remove(holder);
	delete holder;
}

status_t AccelerantRoster::Load(Accelerant *&acc, void *&outAddon, int fd)
{
	char signature[1024];
	CheckRet(ioctl(fd, B_GET_ACCELERANT_SIGNATURE, &signature, sizeof(signature)));
	printf("signature: \"%s\"\n", signature);

	status_t res = B_ERROR;

	BString subPath;
	subPath.SetToFormat("accelerants/%s", signature);
	char** paths;
	size_t pathCount;
	CheckRet(find_paths(B_FIND_PATH_ADD_ONS_DIRECTORY, subPath.String(), &paths, &pathCount));
	MemoryDeleter pathArrayDeleter(paths);

	for(size_t i = 0; i < pathCount; i++) {
		printf("path: \"%s\"\n", paths[i]);
		CObjectDeleter<void, int, dlclose> addon(dlopen(paths[i], 0));
		if (!addon.IsSet())
			continue;
		auto instantiate = (status_t (*)(Accelerant **acc, int fd))dlsym(addon.Get(), "instantiate_accelerant");
		if (instantiate == NULL)
			continue;
		res = instantiate(&acc, fd);
		if (res < B_OK)
			continue;
		outAddon = addon.Detach();
		return B_OK;
	}
	return res;
}

status_t AccelerantRoster::FromFd(Accelerant *&acc, int fd)
{
	RecursiveLocker lock(fPrivate.fLock);

	//printf("fd: %d\n", fd);
	struct stat st;
	CheckRet(fstat(fd, &st));
	//printf("dev: %" B_PRIu64 "\n", (uint64)st.st_dev);
	//printf("ino: %" B_PRIu64 "\n", (uint64)st.st_ino);

	AccelerantHolder::Key key{.dev = st.st_dev, .ino = st.st_ino};
	AccelerantHolder *holder = fPrivate.fAccMap.Find(key);
	if (holder != NULL) {
		holder->fAcc->AcquireReference();
		acc = holder->fAcc;
		return B_OK;
	}
	BReference<Accelerant> accDeleter;
	void *addon{};
	CheckRet(Load(acc, addon, fd));
	accDeleter.SetTo(acc, true);
	holder = new(std::nothrow) AccelerantHolder{
		.fFd = dup(fd),
		.fKey = key,
		.fAcc = acc,
		.fAddon = addon
	};
	if (holder == NULL)
		return B_NO_MEMORY;
	acc->fHolder = holder;
	fPrivate.fAccMap.Insert(holder);
	accDeleter.Detach();
	return B_OK;
}
