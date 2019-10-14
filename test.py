#!/usr/bin/python
# Copyright 2018 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""Runs the unit tests for the project.
"""

from __future__ import print_function

import argparse
import os
import subprocess
import sys

ROOT_DIR = os.path.dirname(os.path.realpath(__file__))
sys.path.append(os.path.join(ROOT_DIR, 'shaka', 'tools'))
import run_ios_tests
import utils


def _RunTests(no_colors):
  """Runs the tests in the given build dir."""
  tools_path = os.path.join(ROOT_DIR, 'shaka', 'tools')
  if subprocess.call([sys.executable, '-m', 'unittest', 'discover',
                      '-s', tools_path, '-p', '*_test.py']) != 0:
    return 1

  webidl_path = os.path.join(ROOT_DIR, 'shaka', 'tools', 'webidl')
  # Add our PLY checkout so the subprocess can find it.
  env = os.environ.copy()
  env['PYTHONPATH'] = (os.path.join(ROOT_DIR, 'third_party', 'ply', 'src') +
                       os.pathsep + env.get('PYTHONPATH', ''))
  if subprocess.call([sys.executable, '-m', 'unittest', 'discover',
                      '-s', webidl_path, '-p', '*_test.py'], env=env) != 0:
    return 1

  target_os = utils.GetGnArg('target_os') or utils.GetGnArg('host_os')
  if target_os == 'ios':
    return run_ios_tests.RunTests()
  else:
    args = [
        '--media_directory', os.path.join(ROOT_DIR, 'shaka', 'test', 'media'),
    ]
    if no_colors:
      args += ['--no_colors']
    return subprocess.call(['./tests'] + args)


def main(args):
  parser = argparse.ArgumentParser(description=__doc__)
  parser.add_argument('--no-colors', action='store_true',
                      help="Don't print colors in test output.")

  ns = parser.parse_args(args)
  return _RunTests(ns.no_colors)


if __name__ == '__main__':
  sys.exit(main(sys.argv[1:]))
