#include "AccelerantRoster.h"

#include <dlfcn.h>
#include <util/AVLTree.h>
#include <sys/stat.h>
#include <Path.h>
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


static void EnumPaths(void* arg, bool (*Callback)(const char* path, void* arg))
{
	const static directory_which vols[] = {
		B_USER_NONPACKAGED_ADDONS_DIRECTORY,
		B_USER_ADDONS_DIRECTORY,
		B_SYSTEM_NONPACKAGED_ADDONS_DIRECTORY,
		B_SYSTEM_ADDONS_DIRECTORY,
	};
	char path[PATH_MAX];
	getcwd(path, PATH_MAX);
	for(size_t i = 0; i < B_COUNT_OF(vols); i++) {
		if (find_directory(vols[i], -1, false, path, PATH_MAX) != B_OK) {
			continue;
		}
		if (Callback(path, arg)) return;
	}
}


void Accelerant::LastReferenceReleased()
{
	AccelerantHolder *holder = fHolder;
	delete this;
	AccelerantRoster::Instance().Unregister(holder);
}


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

	auto enumPathsCallback = [&] (const char* path, void* arg) {
		BPath addonPath(path);
		addonPath.Append("accelerants");
		addonPath.Append(signature);
		printf("path: \"%s\"\n", addonPath.Path());

		CObjectDeleter<void, int, dlclose> addon(dlopen(addonPath.Path(), 0));
		if (!addon.IsSet())
			return false;
		auto instantiate = (status_t (*)(Accelerant **acc, int fd))dlsym(addon.Get(), "instantiate_accelerant");
		if (instantiate == NULL)
			return false;
		res = instantiate(&acc, fd);
		if (res < B_OK)
			return false;
		outAddon = addon.Detach();
		res = B_OK;
		return true;
	};

	EnumPaths(&enumPathsCallback, [] (const char* path, void* arg) {
		return (*(decltype(enumPathsCallback)*)arg)(path, arg);
	});

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
	accDeleter.SetTo(acc, false);
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
