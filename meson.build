project('accelerant2', 'cpp',
	version : '0.1',
	default_options : ['warning_level=3', 'cpp_std=c++2a']
)

cpp = meson.get_compiler('cpp')
pkg = import('pkgconfig')

dep_libbe = cpp.find_library('be')


libaccelerant = shared_library('accelerant',
	[
		'AccelerantRoster.cpp',
		'util/AVLTreeBase.cpp'
	],
	include_directories: [
		'/boot/system/develop/headers/private/shared',
		'/boot/system/develop/headers/private/kernel',
	],
	dependencies: [
		dep_libbe,
	],
	gnu_symbol_visibility: 'hidden',
	install: true
)

dep_libaccelerant = declare_dependency(
	link_with: libaccelerant,
)

install_headers(
  'AccelerantRoster.h',
  subdir: 'accelerant',
)

pkg.generate(
	libaccelerant,
	name: 'libaccelerant',
	subdirs: [
		'accelerant',
	],
	description: 'accelerant',
)

executable('Tests',
	[
		'Tests.cpp',
	],
	include_directories: [
		'/boot/system/develop/headers/private/shared',
	],
	dependencies: [
		dep_libbe,
		dep_libaccelerant,
	],
	gnu_symbol_visibility: 'hidden',
	install: false
)
