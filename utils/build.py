#!/usr/bin/env python3
import argparse
import os
import platform
import random
import re
import shutil
import subprocess
import sys
import xml.etree.ElementTree as ET
from abc import ABC, abstractmethod, abstractproperty
from copy import deepcopy
from enum import Enum
from functools import lru_cache
from pathlib import Path
from typing import Dict, List, Optional
from uuid import uuid4

try:
    from tqdm import tqdm
except ModuleNotFoundError:

    def tqdm(iterable, *args, **kwargs):
        return iterable

    if os.isatty(sys.stdout.fileno()) and random.randint(0, 10) <= 1:
        print('TIP: run `pip install -m tqdm` to get a nice progress bar')

project_root = Path(__file__).resolve().parent.parent
dependencies_dir = project_root / '.dependencies'


def bootstrap(*args, interactive=False, check=False):
    """Run the bootstrap script."""
    bootstrap_py = project_root / 'utils' / 'bootstrap.py'
    result = subprocess.run([sys.executable, str(bootstrap_py)] + list(args),
                            check=False,
                            encoding='utf-8',
                            stdout=None if interactive else subprocess.PIPE,
                            stderr=None if interactive else subprocess.PIPE)
    return result


def project_version():
    """Return current project version (e. g. "3.13.0")"""
    with open(project_root / 'version.txt', 'r') as f:
        return f.read().strip()


@lru_cache()
def get_dependency(name):
    install_dir = Path(
        bootstrap('--print-dependency-directory', name,
                  check=True).stdout.strip())
    suffix = '.exe' if platform.system() == 'Windows' else ''
    if name == 'ninja':
        return install_dir / ('ninja' + suffix)
    elif name == 'cmake':
        return install_dir / 'bin' / ('cmake' + suffix)
    else:
        return install_dir


class BuildType(Enum):
    """Represents the -DCONFIG CMake option."""

    DEBUG = 'DEBUG'
    RELEASE = 'RELEASE'


class BuildConfiguration(ABC):
    @abstractmethod
    def get_cmake_cache_entries(self):
        """Convert the build configuration to CMake cache entries."""

    @abstractmethod
    def get_cmake_flags(self, build_dir: Path) -> List[str]:
        """Return all CMake command-line flags required to build this configuration."""

    @abstractproperty
    def name(self):
        """Name of the configuration."""

    def __hash__(self):
        return hash(self.name)


class FirmwareBuildConfiguration(BuildConfiguration):
    def __init__(self,
                 build_type: BuildType,
                 toolchain: Path = None,
                 generator: str = None,
                 version_suffix: str = None,
                 version_suffix_short: str = None,
                 custom_entries: List[str] = None):
        self.build_type = build_type
        self.toolchain = toolchain or FirmwareBuildConfiguration.default_toolchain(
        )
        self.generator = generator
        self.version_suffix = version_suffix
        self.version_suffix_short = version_suffix_short
        self.custom_entries = custom_entries or []

    @staticmethod
    def default_toolchain() -> Path:
        return Path(__file__).resolve().parent.parent / 'cmake/AvrGcc.cmake'

    def get_cmake_cache_entries(self):
        entries = []
        if self.generator.lower() == 'ninja':
            entries.append(('CMAKE_MAKE_PROGRAM', 'FILEPATH',
                            str(get_dependency('ninja'))))
        entries.extend([
            ('CMAKE_MAKE_PROGRAM', 'FILEPATH', str(get_dependency('ninja'))),
            ('CMAKE_TOOLCHAIN_FILE', 'FILEPATH', str(self.toolchain)),
            ('AVR_TOOLCHAIN_DIR', 'DIRPATH', str(get_dependency('avr-gcc'))),
            ('CMAKE_BUILD_TYPE', 'STRING', self.build_type.value.title()),
            ('PROJECT_VERSION_HASH', 'STRING', self.version_suffix or ''),
            ('PROJECT_VERSION_SUFFIX_SHORT', 'STRING',
             self.version_suffix_short or ''),
        ])
        entries.extend(self.custom_entries)
        return entries

    def get_cmake_flags(self, build_dir: Path) -> List[str]:
        cache_entries = self.get_cmake_cache_entries()
        flags = ['-D{}:{}={}'.format(*entry) for entry in cache_entries]
        flags += ['-G', self.generator or 'Ninja']
        flags += ['-S', str(Path(__file__).resolve().parent.parent)]
        flags += ['-B', str(build_dir)]
        return flags

    @property
    def name(self):
        components = [
            self.build_type.value,
        ]
        return '_'.join(components)


class BuildResult:
    """Represents a result of an attempt to build the project."""

    def __init__(self, config_returncode: int, build_returncode: Optional[int],
                 stdout: Path, stderr: Path, products: List[Path]):
        self.config_returncode = config_returncode
        self.build_returncode = build_returncode
        self.stdout = stdout
        self.stderr = stderr
        self.products = products

    @property
    def configuration_failed(self):
        return self.config_returncode != 0

    @property
    def build_failed(self):
        return self.build_returncode != 0 and self.build_returncode is not None

    @property
    def is_failure(self):
        return self.configuration_failed or self.build_failed

    def __str__(self):
        return '<BuildResult config={self.config_returncode} build={self.build_returncode}>'.format(
            self=self)


def build(configuration: BuildConfiguration,
          build_dir: Path,
          configure_only=False,
          output_to_file=True) -> BuildResult:
    """Build a project with a single configuration."""
    flags = configuration.get_cmake_flags(build_dir=build_dir)

    # create the build directory
    build_dir.mkdir(parents=True, exist_ok=True)
    products = []

    if output_to_file:
        # stdout and stderr are saved to a file in the build directory
        stdout_path = build_dir / 'stdout.txt'
        stderr_path = build_dir / 'stderr.txt'
        stdout = open(stdout_path, 'w')
        stderr = open(stderr_path, 'w')
    else:
        stdout_path, stderr_path = None, None
        stdout, stderr = None, None

    # prepare the build
    config_process = subprocess.run([str(get_dependency('cmake'))] + flags,
                                    stdout=stdout,
                                    stderr=stderr,
                                    check=False)
    if not configure_only and config_process.returncode == 0:
        cmd = [
            str(get_dependency('cmake')), '--build',
            str(build_dir), '--config',
            configuration.build_type.value.lower()
        ]
        build_process = subprocess.run(cmd,
                                       stdout=stdout,
                                       stderr=stderr,
                                       check=False)
        build_returncode = build_process.returncode
        products.extend(build_dir / fname for fname in [
            'firmware', 'firmware.bin', 'firmware.bbf', 'firmware.dfu',
            'firmware.map'
        ] if (build_dir / fname).exists())
    else:
        build_returncode = None

    if stdout:
        stdout.close()
    if stderr:
        stderr.close()

    # collect the result and return
    return BuildResult(config_returncode=config_process.returncode,
                       build_returncode=build_returncode,
                       stdout=stdout_path,
                       stderr=stderr_path,
                       products=products)


def store_products(products: List[Path], build_config: BuildConfiguration,
                   products_dir: Path):
    """Copy build products to a shared products directory."""
    products_dir.mkdir(parents=True, exist_ok=True)
    for product in products:
        is_firmware = isinstance(build_config, FirmwareBuildConfiguration)
        has_custom_suffix = is_firmware and (build_config.version_suffix !=
                                             '<auto>')
        if has_custom_suffix:
            version = project_version()
            name = build_config.name.lower(
            ) + '_' + version + build_config.version_suffix
        else:
            name = build_config.name.lower()
        destination = products_dir / (name + product.suffix)
        shutil.copy(product, destination)


def list_of(EnumType):
    """Create an argument-parser for comma-separated list of values of some Enum subclass."""

    def convert(val):
        if val == '':
            return []
        values = [p.lower() for p in val.split(',')]
        if 'all' in values:
            return list(EnumType)
        else:
            return [EnumType(v.upper()) for v in values]

    convert.__name__ = EnumType.__name__
    return convert


def cmake_cache_entry(arg):
    match = re.fullmatch(r'(.*):(.*)=(.*)', arg)
    if not match:
        raise ValueError('invalid cmake entry; must be <NAME>:<TYPE>=<VALUE>')
    return (match.group(1), match.group(2), match.group(3))


def main():
    parser = argparse.ArgumentParser()
    # yapf: disable
    parser.add_argument(
        '--build-type',
        type=list_of(BuildType),
        default='release',
        help=('Build type (debug or release; default: release; '
              'default for --generate-cproject: debug,release).'))
    parser.add_argument(
        '--version-suffix',
        type=str,
        default='<auto>',
        help='Version suffix (e.g. -BETA+1035.PR111.B4)')
    parser.add_argument(
        '--version-suffix-short',
        type=str,
        default='<auto>',
        help='Version suffix (e.g. +1035)')
    parser.add_argument(
        '--final',
        action='store_true',
        help='Set\'s --version-suffix and --version-suffix-short to empty string.')
    parser.add_argument(
        '--build-dir',
        type=Path,
        help='Specify a custom build directory to be used.')
    parser.add_argument(
        '--products-dir',
        type=Path,
        help='Directory to store built firmware (default: <build-dir>/products).')
    parser.add_argument(
        '-G', '--generator',
        type=str,
        default='Ninja',
        help='Generator to be used by CMake (default=Ninja).')
    parser.add_argument(
        '--toolchain',
        type=Path,
        help='Path to a CMake toolchain file to be used.')
    parser.add_argument(
        '--no-build',
        action='store_true',
        help='Do not build, configure the build only.'
    )
    parser.add_argument(
        '--no-store-output',
        action='store_false',
        help='Do not write build output to files - print it to console instead.'
    )
    parser.add_argument(
        '-D', '--cmake-def',
        action='append', type=cmake_cache_entry,
        help='Custom CMake cache entries (e.g. -DCUSTOM_COMPILE_OPTIONS:STRING=-Werror)'
    )
    args = parser.parse_args(sys.argv[1:])
    # yapf: enable

    build_dir_root = args.build_dir or Path(
        __file__).resolve().parent.parent / 'build'
    products_dir_root = args.products_dir or (build_dir_root / 'products')

    if args.final:
        args.version_suffix = ''
        args.version_suffix_short = ''

    # Check all dependencis are installed
    if bootstrap(interactive=True).returncode != 0:
        print('bootstrap.py failed.')
        sys.exit(1)

    # prepare configurations
    configurations = [
        FirmwareBuildConfiguration(
            build_type=build_type,
            version_suffix=args.version_suffix,
            version_suffix_short=args.version_suffix_short,
            generator=args.generator,
            custom_entries=args.cmake_def) for build_type in args.build_type
    ]

    # build everything
    configurations_iter = tqdm(configurations)
    results: Dict[BuildConfiguration, BuildResult] = dict()
    for configuration in configurations_iter:
        build_dir = build_dir_root / configuration.name.lower()
        description = 'Building ' + configuration.name.lower()
        if hasattr(configurations_iter, 'set_description'):
            configurations_iter.set_description(description)
        else:
            print(description)
        result = build(configuration,
                       build_dir=build_dir,
                       configure_only=args.no_build,
                       output_to_file=args.no_store_output is not False)
        store_products(result.products, configuration, products_dir_root)
        results[configuration] = result

    # print results
    print()
    print('Building finished: {} success, {} failure(s).'.format(
        sum(1 for result in results.values() if not result.is_failure),
        sum(1 for result in results.values() if result.is_failure)))
    failure = False
    max_configname_len = max(len(config.name) for config in results)
    for config, result in results.items():
        if result.configuration_failed:
            status = 'project configuration FAILED'
            failure = True
        elif result.build_failed:
            status = 'build FAILED'
            failure = True
        else:
            status = 'SUCCESS'

        print(' {} {}'.format(
            config.name.lower().ljust(max_configname_len, ' '), status))
    if failure:
        sys.exit(1)


if __name__ == "__main__":
    main()
